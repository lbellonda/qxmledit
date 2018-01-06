/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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


#include "anonexception.h"
#include "xmlutils.h"

//--
#define ATTR_CRITERIA   "criteria"
#define ATTR_ATYPE  "anonType"
#define ATTR_PATH   "path"
#define ATTR_USENS  "useNamespace"
#define ATTR_FIXEDVALUE "fixedValue"
//--


AnonException::AnonException()
{
    _anonType = AnonType::UseDefault ;
    _criteria = AnonInclusionCriteria::ExcludeWithChildren ;
    _useNamespace = false ;
}

AnonException::~AnonException()
{
}

AnonException *AnonException::clone()
{
    AnonException *cloned = new AnonException();
    cloned->_criteria = _criteria ;
    cloned->_anonType = _anonType ;
    cloned->_path = _path;
    cloned->_useNamespace = _useNamespace;
    cloned->_fixedValue = _fixedValue;
    return cloned ;
}

AnonType::Type AnonException::anonType()
{
    return _anonType;
}

void AnonException::setAnonType(const AnonType::Type newType)
{
    _anonType = newType ;
}

AnonInclusionCriteria::Criteria AnonException::criteria()
{
    return _criteria;
}

void AnonException::setCriteria(const AnonInclusionCriteria::Criteria parmCriteria)
{
    _criteria = parmCriteria ;
}

QString AnonException::fixedValue()
{
    return _fixedValue;
}

QString AnonException::path() const
{
    return _path;
}

void AnonException::setPath(const QString &value)
{
    _path = value;
}

QString AnonException::fixedValue() const
{
    return _fixedValue;
}

void AnonException::setFixedValue(const QString &value)
{
    _fixedValue = value ;
}

bool AnonException::isUseNamespace() const
{
    return _useNamespace;
}

void AnonException::setUseNamespace(bool useNamespace)
{
    _useNamespace = useNamespace;
}

bool AnonException::readFromDom(const QDomElement &element)
{
    bool result = true;
    _criteria = (AnonInclusionCriteria::Criteria)XmlUtils::readFromInt(element.attribute(ATTR_CRITERIA), _criteria);
    _anonType = (AnonType::Type)XmlUtils::readFromInt(element.attribute(ATTR_ATYPE), _anonType);
    _path = element.attribute(ATTR_PATH);
    _useNamespace = XmlUtils::readFromBool(element.attribute(ATTR_USENS), false);
    _fixedValue = element.attribute(ATTR_FIXEDVALUE);
    return result;
}

bool AnonException::saveToDom(QDomElement &element)
{
    bool result = true;
    element.setAttribute(ATTR_CRITERIA, XmlUtils::intToStringValue(_criteria));
    element.setAttribute(ATTR_ATYPE, XmlUtils::intToStringValue(_anonType));
    element.setAttribute(ATTR_PATH, _path);
    element.setAttribute(ATTR_USENS, XmlUtils::boolToBoolValue(_useNamespace));
    element.setAttribute(ATTR_FIXEDVALUE, _fixedValue);
    return result;
}

bool AnonException::isInclude()
{
    if((AnonInclusionCriteria::Include == _criteria)
            || (AnonInclusionCriteria::IncludeWithChildren == _criteria)) {
        return true;
    }
    return false;
}

bool AnonException::compare(AnonException *other)
{
    bool result = true ;
    if(NULL == other) {
        result = false;
    } else if(_criteria != other->_criteria) {
        result = false;
    } else if(_anonType != other->_anonType) {
        result = false;
    } else if(_path != other->_path) {
        result = false;
    } else if(_useNamespace != other->_useNamespace) {
        result = false;
    } else if(_fixedValue != other->_fixedValue) {
        result = false;
    }
    return result ;
}
