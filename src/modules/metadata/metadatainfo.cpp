/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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


#include "metadatainfo.h"
#include "modules/metadata/metadataparser.h"
#include "modules/services/systemservices.h"
#include <QDateTime>
#include "utils.h"

static MetadataInfoFactory uniqueInstance;

MetadataInfoFactory::MetadataInfoFactory()
{
    _userProvider = NULL ;
    _dateProvider = NULL ;
}

MetadataInfoFactory::~MetadataInfoFactory()
{

}

MetadataUserProvider *MetadataInfoFactory::userProvider()
{
    return uniqueInstance._userProvider;
}

void MetadataInfoFactory::setUserProvider(MetadataUserProvider *userProvider)
{
    uniqueInstance._userProvider = userProvider;
}

MetadataDateProvider *MetadataInfoFactory::dateProvider()
{
    return uniqueInstance._dateProvider;
}

void MetadataInfoFactory::setDateProvider(MetadataDateProvider *dateProvider)
{
    uniqueInstance._dateProvider = dateProvider;
}

void MetadataInfoFactory::initInstance(MetadataInfo* newInstance)
{
    uniqueInstance._initInstance(newInstance);
}

void MetadataInfoFactory::_initInstance(MetadataInfo* newInstance)
{
    if(NULL != _dateProvider) {
        newInstance->setDateProvider(_dateProvider);
    }
    if(NULL != _userProvider) {
        newInstance->setUserProvider(_userProvider);
    }
}

//-------------------------------------------
const QString UpdatableMetadata::CreationUserName("creationUser");
const QString UpdatableMetadata::CreationDateName("creationDate");
const QString UpdatableMetadata::UpdateUserName("updateUser");
const QString UpdatableMetadata::UpdateDateName("updateDate");
const QString UpdatableMetadata::RevisionName("revision");
const QString UpdatableMetadata::MetaVersionName("metainfo");
//--
const QString UpdatableMetadata::MetaDataModelVersion("1.0");
const QString UpdatableMetadata::MetaDataStartVersion("0");

//
UpdatableMetadata::UpdatableMetadata()
{
    _creationUser.setName(CreationUserName);
    _creationDate.setName(CreationDateName);
    _updateUser.setName(UpdateUserName);
    _updateDate.setName(UpdateDateName);
    _revision.setName(RevisionName);
    _metaVersion.setName(MetaVersionName);
    _revision.setValue(MetaDataStartVersion);
    _metaVersion.setValue(MetaDataModelVersion);
}

UpdatableMetadata::~UpdatableMetadata()
{
    foreach(PseudoAttribute * a, _other) {
        delete a;
    }
    _other.clear();
}


MetadataUserProvider *UpdatableMetadata::userProvider() const
{
    return _userProvider;
}

void UpdatableMetadata::setUserProvider(MetadataUserProvider *userProvider)
{
    _userProvider = userProvider;
}

MetadataDateProvider *UpdatableMetadata::dateProvider() const
{
    return _dateProvider;
}

void UpdatableMetadata::setDateProvider(MetadataDateProvider *dateProvider)
{
    _dateProvider = dateProvider;
}

/*
void UpdatableMetadata::readAttribute(MetadataParsedResult *input, PseudoAttribute *target, const QString &defaultValue)
{
    PseudoAttribute *attr = input->find(target->name());
    if( NULL != attr ) {
        attr->copyTo(target);
        target->setUsed(true);
    } else {
        target->reset(defaultValue);
    }
}
*/


PseudoAttribute *UpdatableMetadata::metaVersion()
{
    return &_metaVersion;
}

PseudoAttribute *UpdatableMetadata::revision()
{
    return &_revision;
}

PseudoAttribute *UpdatableMetadata::updateDate()
{
    return &_updateDate;
}

PseudoAttribute *UpdatableMetadata::updateUser()
{
    return &_updateUser;
}

PseudoAttribute *UpdatableMetadata::creationDate()
{
    return &_creationDate;
}

PseudoAttribute *UpdatableMetadata::creationUser()
{
    return &_creationUser;
}

bool UpdatableMetadata::readAttribute(PseudoAttribute *attr, PseudoAttribute *target)
{
    if(attr->name() == target->name()) {
        attr->copyTo(target);
        target->setUsed(true);
        target->setExists(true);
        return true;
    } else {
        return false;
    }
}

void UpdatableMetadata::read(MetadataParsedResult *input)
{
    foreach(PseudoAttribute * a, input->attributes()) {
        bool deleteAttribute = true;
        if(!readAttribute(a, &_creationUser)) {
            if(!readAttribute(a, &_creationDate)) {
                if(!readAttribute(a, &_updateUser)) {
                    if(!readAttribute(a, &_updateDate)) {
                        if(!readAttribute(a, &_revision)) {
                            if(!readAttribute(a, &_metaVersion)) {
                                if(a->name() != MetadataInfo::TYPE_ATTR) {
                                    _other.append(a);
                                    deleteAttribute = false;
                                }
                            }
                        }
                    }
                }
            }
        }
        if(deleteAttribute) {
            delete a;
        }
    }
    input->forgetAttributes();
}

void UpdatableMetadata::update(const bool insertOnlyIfsNotExisting)
{
    if(insertOnlyIfsNotExisting) {
        if(!_updateDate.exists()) {
            _updateDate.setValue(_dateProvider->date());
        }
        if(!_updateUser.exists()) {
            _updateUser.setValue(_userProvider->user());
        }
    } else {
        _updateDate.setValue(_dateProvider->date());
        _updateUser.setValue(_userProvider->user());
        bool ok = true;
        int value = _revision.value().toInt(&ok);
        if(!ok) {
            value = 1 ;
        } else {
            value++;
        }
        _revision.setValue(QString("%1").arg(value));
    }
}

void UpdatableMetadata::writeAnAttribute(QString &result, PseudoAttribute *attr, const QString &defaultValue)
{
    if(!attr->used()) {
        if(defaultValue.isEmpty()) {
            return ;
        }
        attr->setValue(defaultValue);
    }
    result += " ";
    result += attr->toString();
}

void UpdatableMetadata::markNewRecord()
{
    _updateDate.setValue(_dateProvider->date());
    _updateUser.setValue(_userProvider->user());
    _creationDate.setValue(_dateProvider->date());
    _creationUser.setValue(_userProvider->user());
    _revision.setValue(MetaDataStartVersion);
    _metaVersion.setValue(MetaDataModelVersion);
}

QString UpdatableMetadata::toProcessingInstruction()
{
    QString result = " type=\"info\" ";

    writeAnAttribute(result, &_creationUser);
    writeAnAttribute(result, &_creationDate);
    writeAnAttribute(result, &_updateUser);
    writeAnAttribute(result, &_updateDate);
    writeAnAttribute(result, &_revision, MetaDataStartVersion);
    writeAnAttribute(result, &_metaVersion, MetaDataModelVersion);
    // sort others
    QMap<int, PseudoAttribute*>sorted;
    foreach(PseudoAttribute * a, _other) {
        sorted.insert(a->position(), a);
    }
    //write others
    foreach(int pos, sorted.keys()) {
        PseudoAttribute *a = sorted.value(pos);
        result += " ";
        result += a->toString();
    }
    result += " ";
    return result;
}

//-------------------------------------------

const QString MetadataInfo::QXMLEDIT_TARGET_PI = "qxmledit";
const QString MetadataInfo::UPDATABLE_ATTR = "info";
const QString MetadataInfo::TYPE_ATTR = "type";
const QString MetadataInfo::VALUE_ATTR = "value";
const QString MetadataInfo::FORMATTING_TYPE("formatting");

const QString MetadataInfo::ProjectMetaType("project");
const QString MetadataInfo::CopyrightMetaType("copyright");
const QString MetadataInfo::VersionMetaType("version");
const QString MetadataInfo::DomainMetaType("domain");
const QString MetadataInfo::NameMetaType("name");
//--
const QString MetadataInfo::FormattingAttrIndentEnabled("indent");
const QString MetadataInfo::FormattingAttrIndentValue("indentValue");
const QString MetadataInfo::FormattingAttrSortAlphaAttr("sortAlphaAttr");
const QString MetadataInfo::FormattingAttrAttrLineLen("attrLineLen");
const QString MetadataInfo::FormattingON("on");
const QString MetadataInfo::FormattingOFF("off");

//---------

MetadataInfo::MetadataInfo() : _userProvider(this), _dateProvider(this)
{
    _updatable.setDateProvider(this);
    _updatable.setUserProvider(this);
    setupVariable(&project, MetadataInfo::ProjectMetaType);
    setupVariable(&copyright, MetadataInfo::CopyrightMetaType);
    setupVariable(&version, MetadataInfo::VersionMetaType);
    setupVariable(&domain, MetadataInfo::DomainMetaType);
    setupVariable(&name, MetadataInfo::NameMetaType);
    //--
    MetadataInfoFactory::initInstance(this);
}

MetadataInfo::~MetadataInfo()
{
}

void MetadataInfo::setupVariable(PseudoAttribute *attr, const QString &name)
{
    attr->setName(name);
    _attributes.insert(name, attr);
}

PseudoAttribute *MetadataInfo::attribute(const QString &name)
{
    return _attributes[name];
}

void MetadataInfo::markNewRecord()
{
    _updatable.markNewRecord();
}

UpdatableMetadata *MetadataInfo::updatable()
{
    return &_updatable;
}

MetadataUserProvider *MetadataInfo::userProvider() const
{
    return _userProvider;
}

void MetadataInfo::setUserProvider(MetadataUserProvider *userProvider)
{
    _userProvider = userProvider;
    _updatable.setUserProvider((NULL == _userProvider) ? this : _userProvider);
}

MetadataDateProvider *MetadataInfo::dateProvider() const
{
    return _dateProvider;
}

void MetadataInfo::setDateProvider(MetadataDateProvider *dateProvider)
{
    _dateProvider = dateProvider;
    _updatable.setDateProvider((NULL == _dateProvider) ? this : _dateProvider);
}

QString MetadataInfo::user()
{
    return SystemServices::userNameFromEnv();
}

QString MetadataInfo::date()
{
    return QDateTime::currentDateTime().toString(Qt::ISODate);
}

bool MetadataInfo::parseUpdatable(const QString &inputData, const int row)
{
    if(inputData.isEmpty()) {
        return false;
    }
    MetadataParser parser;
    MetadataParsedResult attributes;
    if(parser.parseMetadata(inputData, &attributes, row)) {
        PseudoAttribute *attribute = attributes.find(TYPE_ATTR);
        if(NULL != attribute) {
            if(attribute->value() == UPDATABLE_ATTR) {
                _updatable.read(&attributes);
                return true;
            }
        }
    }
    return false;
}

bool MetadataInfo::parseFormattingInfo(const QString &inputData, const int row, XMLIndentationSettings *settings)
{
    if(inputData.isEmpty()) {
        return false;
    }
    MetadataParser parser;
    MetadataParsedResult attributes;
    if(parser.parseMetadata(inputData, &attributes, row)) {
        PseudoAttribute *attribute = attributes.find(TYPE_ATTR);
        if(NULL != attribute) {
            if(attribute->value() == FORMATTING_TYPE) {
                decodeFormatSettings(&attributes, settings);
                return true;
            }
        }
    }
    return false;
}

bool MetadataInfo::isFormattingInfo(const QString &inputData)
{
    if(inputData.isEmpty()) {
        return false;
    }
    MetadataParser parser;
    MetadataParsedResult attributes;
    if(parser.parseMetadata(inputData, &attributes, -1)) {
        PseudoAttribute *attribute = attributes.find(TYPE_ATTR);
        if(NULL != attribute) {
            if(attribute->value() == FORMATTING_TYPE) {
                return true ;
            }
        }
    }
    return false;
}

void MetadataInfo::decodeFormatSettings(MetadataParsedResult *input, XMLIndentationSettings *settings)
{
    settings->useFormatting = true ;
    foreach(PseudoAttribute *attr, input->attributes()) {
        if(attr->name() == FormattingAttrIndentEnabled) {
            settings->useIndent = FormattingON == attr->value() ;
        } else if(attr->name() == FormattingAttrIndentValue) {
            bool isOk = false ;
            const int value = attr->value().toInt(&isOk);
            if(isOk) {
                settings->indent = value ;
            }
        } else if(attr->name() == FormattingAttrSortAlphaAttr) {
            settings->saveAttrMethod = (attr->value() == FormattingON) ? Regola::SaveAttributesSortingAlphabetically : Regola::SaveAttributesNoSort ;
        } else if(attr->name() == FormattingAttrAttrLineLen) {
            if(attr->value() == FormattingOFF) {
                settings->indentAttributesSetting = QXmlEditData::AttributesIndentationNone;
                settings->indentAttributesColumns = 0 ;
            } else {
                settings->indentAttributesSetting = QXmlEditData::AttributesIndentationMaxCols;
                bool isOk = false ;
                const int value = attr->value().toInt(&isOk);
                if(isOk) {
                    settings->indentAttributesColumns = value ;
                }
            }
        }
    }
    input->clean();
}

QString MetadataInfo::toStringAttribute(const QString &name, const QString &value)
{
    QString result = " ";
    result += name ;
    result += "=\"" ;
    result += MetadataParserInfo::escape(value) ;
    result += "\"" ;
    return result;
}

QString MetadataInfo::toFormatInfo(XMLIndentationSettings *settings)
{
    QString info ;

    info += toStringAttribute(TYPE_ATTR, FORMATTING_TYPE);
    info += toStringAttribute(FormattingAttrIndentEnabled, settings->useIndent ? FormattingON : FormattingOFF);
    info += toStringAttribute(FormattingAttrIndentValue, QString::number(settings->indent));
    if(settings->saveAttrMethod == Regola::SaveAttributesSortingAlphabetically) {
        info += toStringAttribute(FormattingAttrSortAlphaAttr, FormattingON);
    } else {
        info += toStringAttribute(FormattingAttrSortAlphaAttr, FormattingOFF);
    }
    if(settings->indentAttributesSetting == QXmlEditData::AttributesIndentationNone) {
        info += toStringAttribute(FormattingAttrAttrLineLen, FormattingOFF);
    } else {
        info += toStringAttribute(FormattingAttrAttrLineLen, QString::number(settings->indentAttributesColumns));
    }

    return info ;
}

bool MetadataInfo::lookForOneAttribute(MetadataParsedResult *attributes, PseudoAttribute *inAttr, PseudoAttribute *targetAttr, const QString &name)
{
    if(inAttr->value() == name) {
        targetAttr->read(attributes->find(VALUE_ATTR));
        targetAttr->setName(name);
        return true;
    }
    return false;
}

bool MetadataInfo::parseOtherMetadata(const QString &inputData, const int rowPos)
{
    if(inputData.isEmpty()) {
        return false;
    }
    bool found = false;
    MetadataParser parser;
    MetadataParsedResult attributes;
    if(parser.parseMetadata(inputData, &attributes, rowPos)) {
        PseudoAttribute *attribute = attributes.find(TYPE_ATTR);
        if(NULL != attribute) {
            if(lookForOneAttribute(&attributes, attribute, &project, MetadataInfo::ProjectMetaType)) {
                found = true ;
            } else if(lookForOneAttribute(&attributes, attribute, &copyright, MetadataInfo::CopyrightMetaType)) {
                found = true ;
            } else if(lookForOneAttribute(&attributes, attribute, &version, MetadataInfo::VersionMetaType)) {
                found = true ;
            } else if(lookForOneAttribute(&attributes, attribute, &domain, MetadataInfo::DomainMetaType)) {
                found = true ;
            } else if(lookForOneAttribute(&attributes, attribute, &name, MetadataInfo::NameMetaType)) {
                found = true ;
            }
        }
    }
    return found;
}

QList<PseudoAttribute*> MetadataInfo::manualAttributeListByCondition(const bool readOnes, const bool writeOnes)
{
    QList<PseudoAttribute*> result;

    PseudoAttribute *attr = &project;
    if((attr->exists() == readOnes) && (attr->used() == writeOnes)) {
        result.append(attr);
    }
    attr = &copyright;
    if((attr->exists() == readOnes) && (attr->used() == writeOnes)) {
        result.append(attr);
    }
    attr = &version;
    if((attr->exists() == readOnes) && (attr->used() == writeOnes)) {
        result.append(attr);
    }
    attr = &domain;
    if((attr->exists() == readOnes) && (attr->used() == writeOnes)) {
        result.append(attr);
    }
    attr = &name;
    if((attr->exists() == readOnes) && (attr->used() == writeOnes)) {
        result.append(attr);
    }
    return result;
}
