/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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


#include "xsdoperationparameters.h"
#include "element.h"
#include "xsdeditor/XSchemaIOContants.h"
#include "utils.h"

const QString XSDOperationParameters::TokenType("$type");
const QString XSDOperationParameters::TokenName("$name");

XSDOperationParameters::XSDOperationParameters()
{
    init();
}

/*!
 * \brief XSDOperationParameters::XSDOperationParameters to build namespace declaration hierarchies
 * \param theParent
 */
XSDOperationParameters::XSDOperationParameters(XSDOperationParameters *theParent)
{
    init();
    _parent = theParent;
}

void XSDOperationParameters::init()
{
    _typeTo = EXT_ANONYMOUS_TYPE ;
    _typeSpec = TS_COMPLEX;
    _conserveAnnotations = true ;
    _objType = EOI_ELEMENT ;
    _usePrefix = false;
    _subOper = TSS_NONE ;
    _typeContent = TSC_COMPLEX;
    _parent = NULL;
}

XSDOperationParameters::~XSDOperationParameters()
{
}

//-----

QString XSDOperationParameters::decodeToken(const QString &code)
{
    if(code == TokenType) {
        return typeName();
    } else if(code == TokenName) {
        return name();
    }
    return "" ;
}


void XSDOperationParameters::copyTo(XSDOperationParameters *dest)
{
    dest->_xsdNamespacePrefix = _xsdNamespacePrefix;
    dest->_typeName = _typeName;
    dest->_typeTo = _typeTo;
    dest->_typeSpec = _typeSpec;
    dest->_conserveAnnotations = _conserveAnnotations;
    dest->_objType = _objType;
    dest->_usePrefix = _usePrefix;
    dest->_name = _name;
    dest->_subOper = _subOper;
    dest->_typeContent = _typeContent;
}

//-----

QString XSDOperationParameters::xsdNamespacePrefix() const
{
    return _xsdNamespacePrefix;
}

void XSDOperationParameters::setXsdNamespacePrefix(const QString &xsdNamespacePrefix)
{
    _xsdNamespacePrefix = xsdNamespacePrefix;
}

void XSDOperationParameters::setElementDeclarations(Element *element, const bool upToRoot)
{
    element->declaredNamespaces(_prefixesToNamespaces);
    if(upToRoot && (NULL != element->parent())) {
        setElementDeclarations(element->parent(), upToRoot);
    }
}

QString XSDOperationParameters::getNSForPrefix(const QString &prefix)
{
    if(_prefixesToNamespaces.contains(prefix)) {
        return _prefixesToNamespaces[prefix];
    }
    if(NULL != _parent) {
        return _parent->getNSForPrefix(prefix);
    }
    return "" ;
}

QString XSDOperationParameters::typeName() const
{
    return _typeName;
}

void XSDOperationParameters::setTypeName(const QString &typeName)
{
    _typeName = typeName;
}

XSDOperationParameters::EXSDTypeTo XSDOperationParameters::typeTo() const
{
    return _typeTo;
}

void XSDOperationParameters::setTypeTo(const EXSDTypeTo &typeTo)
{
    _typeTo = typeTo;
}

XSDOperationParameters::EXSDTypeSpec XSDOperationParameters::typeSpec() const
{
    return _typeSpec;
}

void XSDOperationParameters::setTypeSpec(const EXSDTypeSpec &typeSpec)
{
    _typeSpec = typeSpec;
}

bool XSDOperationParameters::isConserveAnnotations() const
{
    return _conserveAnnotations;
}

void XSDOperationParameters::setConserveAnnotations(bool conserveAnnotations)
{
    _conserveAnnotations = conserveAnnotations;
}

XSDOperationParameters::EObjectType XSDOperationParameters::objType() const
{
    return _objType;
}

void XSDOperationParameters::setObjType(const EObjectType &objType)
{
    _objType = objType;
}

bool XSDOperationParameters::usePrefix() const
{
    return _usePrefix;
}

void XSDOperationParameters::setUsePrefix(bool usePrefix)
{
    _usePrefix = usePrefix;
}

QString XSDOperationParameters::name() const
{
    return _name;
}

void XSDOperationParameters::setName(const QString &name)
{
    _name = name;
}

XSDOperationParameters::EXSDTypeSpecSub XSDOperationParameters::subOper() const
{
    return _subOper;
}

void XSDOperationParameters::setSubOper(const EXSDTypeSpecSub &subOper)
{
    _subOper = subOper;
}

XSDOperationParameters::EXSDTypeContent XSDOperationParameters::typeContent() const
{
    return _typeContent;
}

void XSDOperationParameters::setTypeContent(const EXSDTypeContent &typeContent)
{
    _typeContent = typeContent;
}


//--------------------------------------------------

void XSDOperationParameters::setupFromElement(Element *inputElement)
{
    QString localName = inputElement->localName();
    if(localName == IO_XSD_TAGELEMENT) {
        decodeElement(inputElement);
    } else if(localName == IO_XSD_ATTRIBUTE) {
        decodeAttribute(inputElement);
    } else if(localName == IO_XSD_COMPLEXTYPE) {
        decodeComplexType(inputElement);
    } else if(localName == IO_XSD_SIMPLETYPE) {
        decodeSimpleType(inputElement);
    } else {
        // catch all
        decodeElement(inputElement);
    }
}


QSet<QString> XSDOperationParameters::xsdNamespacePrefixes() const
{
    return _xsdNamespacePrefixes;
}

void XSDOperationParameters::setXsdNamespacePrefixes(const QSet<QString> &xsdNamespacePrefixes)
{
    _xsdNamespacePrefixes = xsdNamespacePrefixes;
}

void XSDOperationParameters::setupSimpleType(Element *simpleType)
{
    _typeSpec = TS_SIMPLE ;
    Element *restr = simpleType->getFirstChildByLocalName(IO_XSD_RESTRICTION);
    if(NULL != restr) {
        _subOper = TSS_RESTRICTION;
        Attribute *typeBase = restr->getAttribute(IO_GENERIC_BASE);
        if(NULL != typeBase) {
            _typeName = typeBase->value;
        }
    } else if(NULL != simpleType->getFirstChildByLocalName(IO_XSD_LIST)) {
        _subOper =  TSS_LIST;
    } else if(NULL != simpleType->getFirstChildByLocalName(IO_XSD_UNION)) {
        _subOper =  TSS_UNION;
    }
}

bool XSDOperationParameters::setupComplexTypeSimpleContent(Element *simpleContent)
{
    if(NULL == simpleContent) {
        return false ;
    }
    _typeContent = TSC_SIMPLE ;
    Element *restr = simpleContent->getFirstChildByLocalName(IO_XSD_RESTRICTION);
    if(NULL != restr) {
        _subOper = TSS_RESTRICTION;
        Attribute *typeBase = restr->getAttribute(IO_GENERIC_BASE);
        if(NULL != typeBase) {
            _typeName = typeBase->value;
        }
    } else {
        Element *ext = simpleContent->getFirstChildByLocalName(IO_XSD_EXTENSION);
        if(NULL != ext) {
            _subOper = TSS_EXTENSION;
            Attribute *typeBase = ext->getAttribute(IO_GENERIC_BASE);
            if(NULL != typeBase) {
                _typeName = typeBase->value;
            }
        }
    }
    return true ;
}

bool XSDOperationParameters::setupComplexTypeComplexContent(Element *complexContent)
{
    if(NULL == complexContent) {
        return false ;
    }
    _typeContent = TSC_COMPLEX ;
    Element *restr = complexContent->getFirstChildByLocalName(IO_XSD_RESTRICTION);
    if(NULL != restr) {
        _subOper = TSS_RESTRICTION;
        Attribute *typeBase = restr->getAttribute(IO_GENERIC_BASE);
        if(NULL != typeBase) {
            _typeName = typeBase->value;
        }
    } else {
        Element *ext = complexContent->getFirstChildByLocalName(IO_XSD_EXTENSION);
        if(NULL != ext) {
            _subOper = TSS_EXTENSION;
            Attribute *typeBase = ext->getAttribute(IO_GENERIC_BASE);
            if(NULL != typeBase) {
                _typeName = typeBase->value;
            }
        }
    }
    return true ;
}

bool XSDOperationParameters::setupComplexTypeGroup(Element *input)
{
    if(NULL == input) {
        return false ;
    }
    _typeContent = TSC_NONE ;
    _subOper = TSS_GROUP;
    return true ;
}

bool XSDOperationParameters::setupComplexTypeChoice(Element *input)
{
    if(NULL == input) {
        return false ;
    }
    _typeContent = TSC_NONE ;
    _subOper = TSS_CHOICE;
    return true ;
}

bool XSDOperationParameters::setupComplexTypeSeq(Element *input)
{
    if(NULL == input) {
        return false ;
    }
    _typeContent = TSC_NONE ;
    _subOper = TSS_SEQUENCE;
    return true ;
}

bool XSDOperationParameters::setupComplexTypeAll(Element *input)
{
    if(NULL == input) {
        return false ;
    }
    _typeContent = TSC_NONE ;
    _subOper = TSS_ALL;
    return true ;
}

void XSDOperationParameters::setupComplexType(Element *complexType)
{
    _typeSpec = TS_COMPLEX ;
    if(setupComplexTypeSimpleContent(complexType->getFirstChildByLocalName(IO_XSD_SIMPLECONTENT))) {
        return;
    }
    if(setupComplexTypeComplexContent(complexType->getFirstChildByLocalName(IO_XSD_COMPLEXCONTENT))) {
        return;
    }
    if(setupComplexTypeGroup(complexType->getFirstChildByLocalName(IO_XSD_GROUP))) {
        return;
    }
    if(setupComplexTypeChoice(complexType->getFirstChildByLocalName(IO_XSD_CHOICE))) {
        return;
    }
    if(setupComplexTypeSeq(complexType->getFirstChildByLocalName(IO_XSD_SEQUENCE))) {
        return;
    }
    if(setupComplexTypeAll(complexType->getFirstChildByLocalName(IO_XSD_ALL))) {
        return;
    }
}

void XSDOperationParameters::extractName(Element *inputElement)
{
    QString localName = inputElement->localName();
    _name = localName;
    Attribute *nameAttr = inputElement->getAttribute(IO_GENERIC_NAME);
    if(NULL != nameAttr) {
        _name = nameAttr->value;
    }
}

void XSDOperationParameters::decodeElement(Element *inputElement)
{
    _objType = EOI_ELEMENT ;
    _typeSpec = TS_COMPLEX ;
    _typeContent = TSC_NONE ;
    _subOper = TSS_NONE ;
    _typeName = "";

    extractName(inputElement);
    Attribute *typeAttr = inputElement->getAttribute(IO_ATTRIBUTE_ATTR_TYPE);
    if(NULL != typeAttr) {
        _typeName = typeAttr->value;
        _typeTo = EXT_REFERENCE_TYPE;
    } else {
        Element *simpleType = inputElement->getFirstChildByLocalName(IO_XSD_SIMPLETYPE);
        if(NULL != simpleType) {
            setupSimpleType(simpleType);
        } else {
            Element *complexType = inputElement->getFirstChildByLocalName(IO_XSD_COMPLEXTYPE);
            if(NULL != complexType) {
                setupComplexType(complexType);
            }
        }
    }
}

void XSDOperationParameters::decodeSimpleType(Element *inputElement)
{
    _objType = EOI_TYPE ;
    _typeSpec = TS_SIMPLE ;
    _typeContent = TSC_NONE ;
    _subOper = TSS_NONE ;
    _typeName = "";

    extractName(inputElement);
    setupSimpleType(inputElement);
}

void XSDOperationParameters::decodeComplexType(Element *inputElement)
{
    _objType = EOI_TYPE ;
    _typeSpec = TS_COMPLEX ;
    _typeContent = TSC_NONE ;
    _subOper = TSS_NONE ;
    _typeName = "";

    extractName(inputElement);
    setupComplexType(inputElement);
}

void XSDOperationParameters::decodeAttribute(Element *inputElement)
{
    _objType = EOI_ATTRIBUTE ;
    _typeSpec = TS_SIMPLE ;
    _typeContent = TSC_NONE ;
    _subOper = TSS_NONE ;
    _typeName = "";

    extractName(inputElement);
    Attribute *typeAttr = inputElement->getAttribute(IO_ATTRIBUTE_ATTR_TYPE);
    if(NULL != typeAttr) {
        _typeName = typeAttr->value;
        _typeTo = EXT_REFERENCE_TYPE;
    } else {
        Element *simpleType = inputElement->getFirstChildByLocalName(IO_XSD_SIMPLETYPE);
        if(NULL != simpleType) {
            setupSimpleType(simpleType);
        }
    }
}

QString XSDOperationParameters::makeNameForXSDObject(const QString &localName)
{
    QString newTag;
    QString prefix;
    if(!xsdNamespacePrefix().isEmpty()) {
        prefix = xsdNamespacePrefix();
        prefix += ":";
    }
    newTag = prefix ;
    newTag += localName ;
    return newTag;
}
