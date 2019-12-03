/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2019 by Luca Bellonda and individual contributors       *
 *    as indicated in the AUTHORS file                                    *
 *  lbellonda _at_ gmail.com                                              *
 *                                                                        *
 * This library is free software; you can redistribute it and/or          *
 * modify it under the terms of the GNU Library General Public            *
 * License as published by the Free Software Foundation; either           *
 * version 2 of the License, or (at your option) any later version.       *
 *                                                                        *
 * This library is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 * Library General Public License for more details.                       *
 *                                                                        *
 * You should have received a copy of the GNU Library General Public      *
 * License along with this library; if not, write to the                  *
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,       *
 * Boston, MA  02110-1301  USA                                            *
 **************************************************************************/

#include "extractionscriptelement.h"
#include "xmlutils.h"
#include "utils.h"

//------------------------------------------------------------------------
int ExtractionScriptAttribute::instances = 0;

ExtractionScriptAttribute::ExtractionScriptAttribute()
{
    instances ++ ;
}

ExtractionScriptAttribute::~ExtractionScriptAttribute()
{
    instances -- ;
}

bool ExtractionScriptAttribute::compareTo(ExtractionScriptAttribute *other)
{
    if(nameSpace != other->nameSpace) {
        return false;
    }
    if(name != other->name) {
        return false;
    }
    if(value != other->value) {
        return false;
    }
    return true ;
}

//------------------------------------------------------------------------

static const QString NULL_NS = "" ;

ExtractionScriptElementEvent::ExtractionScriptElementEvent(QObject *parent) : QObject(parent)
{
    _useNamespaces = true;
    _isModified = false ;
    _isError = false ;
}

ExtractionScriptElementEvent::~ExtractionScriptElementEvent()
{
    EMPTYPTRLIST(_attributesSorted, ExtractionScriptAttribute)
}

//-----------------

bool ExtractionScriptElementEvent::isModified() const
{
    return _isModified;
}

bool ExtractionScriptElementEvent::resetModifed()
{
    bool result = _isModified ;
    _isModified = false;
    return result ;
}

bool ExtractionScriptElementEvent::compareTo(ExtractionScriptElementEvent &other)
{
    if(_isError != other._isError) {
        return false;
    }
    if(_useNamespaces != other._useNamespaces) {
        return false;
    }
    if(_isModified != other._isModified) {
        return false;
    }
    if(_elementName != other._elementName) {
        return false;
    }
    if(_localName != other._localName) {
        return false;
    }
    if(_nameSpace != other._nameSpace) {
        return false;
    }

    if(_attributesSorted.size() != other._attributesSorted.size()) {
        return false;
    }
    foreach(const QString &key, _attributesByNamespaceAndName.keys()) {
        if(!other._attributesByNamespaceAndName.contains(key)) {
            return false;
        }
        if(!_attributesByNamespaceAndName[key]->compareTo(other._attributesByNamespaceAndName[key])) {
            return false;
        }
    }
    FORINT(index, _attributesSorted.size()) {
        if(!_attributesSorted.at(index)->compareTo(other._attributesSorted.at(index))) {
            return false;
        }
    }
    return true ;
}

//-----

void ExtractionScriptElementEvent::setAttribute(QXmlStreamAttribute* attribute)
{
    if(attribute->name().isEmpty()) {
        triggerError(QString("setAttribute: %1").arg(tr("empty name")));
        return ;
    }
    checkInternalState();
    if(_useNamespaces) {
        setAttributeValueByNameNS(attribute->namespaceUri().toString(), attribute->name().toString(), attribute->value().toString());
    } else {
        setAttributeValueByName(attribute->qualifiedName().toString(), attribute->value().toString());
    }
    checkInternalState();
}

//-----

int ExtractionScriptElementEvent::iRemoveAttributeByName(const QString &nameSpace, const QString &attributeName)
{
    checkInternalState();
    ExtractionScriptAttribute *attribute = findAttribute(NULL_NS, attributeName);
    if(NULL != attribute) {
        _attributesSorted.removeOne(attribute);
        const QString key = makeAttributeKey(NULL_NS, attributeName);
        _attributesByNamespaceAndName.remove(key);
        delete attribute ;
        _isModified = true ;
    } else {
        triggerError(QString("removeAttributeByName: %1 '%2'-'%3'").arg(tr("unknownAttribute")).arg(nameSpace).arg(attributeName));
    }
    checkInternalState();
    return _attributesSorted.size();
}

int ExtractionScriptElementEvent::removeAttributeByName(const QString &attributeName)
{
    return iRemoveAttributeByName(NULL_NS, attributeName);
}

int ExtractionScriptElementEvent::removeAttributeByNameNS(const QString &nameSpace, const QString &attributeName)
{
    if(!useNamespaces()) {
        triggerError(QString("removeAttributeByNameNS: %1 '%2' - '%3'").arg(tr("namespaces needed")).arg(nameSpace).arg(attributeName));
    }
    return iRemoveAttributeByName(nameSpace, attributeName);
}

int ExtractionScriptElementEvent::removeAttributeByIndex(const int index)
{
    checkInternalState();
    if((index >= 0) && (index < _attributesSorted.size())) {
        ExtractionScriptAttribute* attribute = _attributesSorted.at(index);
        _attributesSorted.removeAt(index);
        const QString key = makeAttributeKey(attribute);
        _attributesByNamespaceAndName.remove(key);
        delete attribute ;
        _isModified = true ;
    } else {
        triggerError(QString("removeAttributeByIndex: %1 '%2'").arg(tr("unknownAttribute")).arg(index));
    }
    checkInternalState();
    return _attributesSorted.size();
}

static bool sortLessThanByNameFunction(const ExtractionScriptAttribute* attr1, const ExtractionScriptAttribute* attr2)
{
    return attr1->name.toLower() < attr2->name.toLower();
}

static bool sortLessThanByNsAndNameFunction(const ExtractionScriptAttribute *attr1, const ExtractionScriptAttribute *attr2)
{
    const QString nameSpace1 = attr1->nameSpace.toLower() ;
    const QString nameSpace2 = attr2->nameSpace.toLower() ;
    if(nameSpace1 != nameSpace2) {
        return nameSpace1 < nameSpace2 ;
    }
    return attr1->name.toLower() < attr2->name.toLower();
}

void ExtractionScriptElementEvent::sortAttributesByName()
{
    checkInternalState();
    qSort(_attributesSorted.begin(), _attributesSorted.end(), sortLessThanByNameFunction);
    _isModified = true ;
    checkInternalState();
}

void ExtractionScriptElementEvent::sortAttributesByNamespaceAndName()
{
    checkInternalState();
    qSort(_attributesSorted.begin(), _attributesSorted.end(), sortLessThanByNsAndNameFunction);
    _isModified = true ;
    checkInternalState();
}

//-----
QString ExtractionScriptElementEvent::errorMessage() const
{
    return _errorMessage;
}

void ExtractionScriptElementEvent::setErrorMessage(const QString &errorMessage)
{
    _errorMessage = errorMessage;
}


bool ExtractionScriptElementEvent::isError() const
{
    return _isError;
}

void ExtractionScriptElementEvent::setError(const bool value)
{
    _isError = value ;
}

void ExtractionScriptElementEvent::resetError()
{
    _isError = false ;
}

bool ExtractionScriptElementEvent::useNamespaces() const
{
    return _useNamespaces;
}

void ExtractionScriptElementEvent::setUseNamespaces(const bool value)
{
    _useNamespaces = value ;
}

QString ExtractionScriptElementEvent::elementName() const
{
    return _elementName;
}

void ExtractionScriptElementEvent::setElementName(const QString &value)
{
    _elementName = value;
}

QString ExtractionScriptElementEvent::localName() const
{
    return _localName ;
}

void ExtractionScriptElementEvent::setLocalName(const QString &value)
{
    _localName = value;
}

QString ExtractionScriptElementEvent::nameSpace() const
{
    return _nameSpace;
}

void ExtractionScriptElementEvent::setNameSpace(const QString &value)
{
    _nameSpace = value ;
}

int ExtractionScriptElementEvent::attributesCount()
{
    checkInternalState();
    return _attributesSorted.size();
}

QString ExtractionScriptElementEvent::iAttributeValueByNameNS(const QString &namespaceId, const QString &attributeName)
{
    checkInternalState();
    ExtractionScriptAttribute * attribute = findAttribute(namespaceId, attributeName);
    if(NULL != attribute) {
        return attribute->value ;
    }
    return "" ;
}

QString ExtractionScriptElementEvent::attributeValueByName(const QString &attributeName)
{
    return iAttributeValueByNameNS(NULL_NS, attributeName);
}

QString ExtractionScriptElementEvent::attributeValueByNameNS(const QString &namespaceId, const QString &attributeName)
{
    if(!useNamespaces()) {
        triggerError(QString("attributeValueByNameNS: %1 '%2' - '%3'").arg(tr("namespaces needed")).arg(namespaceId).arg(attributeName));
        return "";
    }
    return iAttributeValueByNameNS(namespaceId, attributeName);
}

void ExtractionScriptElementEvent::iSetAttributeValueByName(const QString &theNameSpace, const QString &theAttributeName, const QString &attributeValue)
{
    checkInternalState();
    ExtractionScriptAttribute *attribute = findAttribute(theNameSpace, theAttributeName);
    if(NULL == attribute) {
        attribute = new ExtractionScriptAttribute();
        attribute->name = theAttributeName ;
        attribute->nameSpace = theNameSpace ;
        addAttribute(attribute);
        _isModified = true ;
    }
    if(attribute->value != attributeValue) {
        attribute->value = attributeValue ;
        _isModified = true ;
    }
}

void ExtractionScriptElementEvent::setAttributeValueByNameNS(const QString &nameSpace, const QString &attributeName, const QString &attributeValue)
{
    if(!useNamespaces()) {
        triggerError(QString("setAttributeValueByNameNS: %1 '%2' - '%3'").arg(tr("namespaces needed")).arg(nameSpace).arg(attributeName));
    }
    iSetAttributeValueByName(nameSpace, attributeName, attributeValue);
}

void ExtractionScriptElementEvent::setAttributeValueByName(const QString &attributeName, const QString &attributeValue)
{
    iSetAttributeValueByName(NULL_NS, attributeName, attributeValue);
}

QString ExtractionScriptElementEvent::attributeValueByIndex(const int index)
{
    checkInternalState();
    if(index < _attributesSorted.size()) {
        ExtractionScriptAttribute *attribute = _attributesSorted.at(index);
        return attribute->value;
    } else {
        triggerError(QString("attributeValueByIndex: %1 '%2'").arg(tr("out of range")).arg(index));
    }
    return "" ;
}

void ExtractionScriptElementEvent::setAttributeValueByIndex(const int index, const QString &attributeValue)
{
    checkInternalState();
    if(index < _attributesSorted.size()) {
        ExtractionScriptAttribute *attribute = _attributesSorted.at(index);
        if(attribute->value != attributeValue) {
            attribute->value = attributeValue ;
            _isModified = true ;
        }
    } else {
        triggerError(QString("setAttributeValueByIndex: %1 '%2' ").arg(tr("out of range")).arg(index));
    }
}


QString ExtractionScriptElementEvent::attributeNameByIndex(const int index)
{
    checkInternalState();
    if(index < _attributesSorted.size()) {
        return _attributesSorted.at(index)->name;
    } else {
        triggerError(QString("attributeNameByIndex: %1 '%2'").arg(tr("out of range")).arg(index));
    }
    return "" ;
}

QString ExtractionScriptElementEvent::attributeNameSpaceByIndex(const int index)
{
    if(!useNamespaces()) {
        triggerError(QString("attributeNameSpaceByIndex: %1 %2").arg(tr("namespaces needed")).arg(index));
    }
    checkInternalState();
    if(index < _attributesSorted.size()) {
        return _attributesSorted.at(index)->nameSpace;
    } else {
        triggerError(QString("attributeNameSpaceByIndex: %1 '%2'").arg(tr("out of range")).arg(index));
    }
    return "" ;
}

void ExtractionScriptElementEvent::setAttributeNameByIndex(const int index, const QString &attributeName)
{
    iSetAttributeNameByIndex(index, NULL_NS, attributeName);
}

void ExtractionScriptElementEvent::iSetAttributeNameByIndex(const int index, const QString &namespaceId, const QString &attributeName)
{
    const int currentAttributesCount = _attributesSorted.size();
    if(index < currentAttributesCount) {
        ExtractionScriptAttribute *attribute = _attributesSorted.at(index);
        QString newKey = makeAttributeKey(namespaceId, attributeName);
        QString currentKey = makeAttributeKey(attribute);
        const bool exists = _attributesByNamespaceAndName.contains(newKey);
        if(exists && (newKey != currentKey)) {
            triggerError(QString("setAttributeNameByIndex: %1 %2 '%3'").arg(tr("already existing")).arg(index).arg(attributeName));
        } else {
            attribute->name = attributeName ;
            attribute->nameSpace = namespaceId ;
            _attributesByNamespaceAndName.remove(currentKey);
            _attributesByNamespaceAndName.insert(newKey, attribute);
            _isModified = true ;
        }
    } else {
        triggerError(QString("setAttributeNameByIndex: %1 %2 '%3' - '%4'").arg(tr("out of range")).arg(index).arg(namespaceId).arg(attributeName));
    }
    checkInternalState();
}

void ExtractionScriptElementEvent::setAttributeNameByIndexNS(const int index, const QString &namespaceId, const QString &attributeName)
{
    if(!useNamespaces()) {
        triggerError(QString("setAttributeNameByIndex: %1 %2 '%3' - '%4'").arg(tr("namespaces needed")).arg(index).arg(namespaceId).arg(attributeName));
        return ;
    }
    iSetAttributeNameByIndex(index, namespaceId, attributeName);
}

void ExtractionScriptElementEvent::checkInternalState()
{
    if(!iCheckInternalState()) {
        Utils::error("Bad internal state");
#ifdef QXMLEDIT_TEST
        throw "Bad internal state";
#endif
    }
}

bool ExtractionScriptElementEvent::iCheckInternalState()
{
#ifdef QXMLEDIT_TEST
    // input validation
    if(_attributesSorted.size() != _attributesByNamespaceAndName.size()) {
        return false;
    }
    foreach(ExtractionScriptAttribute *attribute, _attributesSorted) {
        const QString key = makeAttributeKey(attribute);
        if(!_attributesByNamespaceAndName.contains(key)) {
            return false;
        }
        if(_attributesByNamespaceAndName[key] != attribute) {
            return false;
        }
    }
#endif
    return true ;
}

void ExtractionScriptElementEvent::dump()
{
    int index = 0;
    printf("%s\n", QString("Tag: %1, name:%2, ns:%3, useNs:%4, error:%5")
           .arg(_elementName).arg(localName()).arg(nameSpace()).arg(useNamespaces()).arg(isError()).toLatin1().data());
    foreach(ExtractionScriptAttribute *attr, _attributesSorted) {
        QString value = QString("%1 %2 %3 = %4").arg(index).arg(attr->nameSpace).arg(attr->name).arg(attr->value);
        printf("%s\n", value.toLatin1().data());
        index++ ;
    }
}

QList<ExtractionScriptAttribute *> ExtractionScriptElementEvent::attributes()
{
    return _attributesSorted ;
}

QString ExtractionScriptElementEvent::makeAttributeKey(const QString &attributeNameSpace, const QString &attributeName)
{
    return QString("%1::\u0001::%2").arg(attributeNameSpace).arg(attributeName);
}

QString ExtractionScriptElementEvent::makeAttributeKey(ExtractionScriptAttribute *attribute)
{
    return makeAttributeKey(attribute->nameSpace, attribute->name);
}

ExtractionScriptAttribute *ExtractionScriptElementEvent::findAttribute(const QString &attributeNameSpace, const QString &attributeName)
{
    QString key = makeAttributeKey(attributeNameSpace, attributeName);
    if(_attributesByNamespaceAndName.contains(key)) {
        return _attributesByNamespaceAndName[key];
    }
    return NULL ;
}

bool ExtractionScriptElementEvent::addAttribute(ExtractionScriptAttribute *newAttribute)
{
    checkInternalState();
    QString key = makeAttributeKey(newAttribute->nameSpace, newAttribute->name);
    if(!_attributesByNamespaceAndName.contains(key)) {
        _attributesSorted.append(newAttribute);
        _attributesByNamespaceAndName[key] = newAttribute ;
        _isModified = true ;
        return true ;
    } else {
        triggerError(QString("addAttribute: %1 '%2' - '%3'").arg(tr("existingAttribute")).arg(newAttribute->nameSpace).arg(newAttribute->name));
        return false;
    }
}

void ExtractionScriptElementEvent::triggerError(const QString &message)
{
    setError(true);
    setErrorMessage(message);
}
