/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016-2018 by Luca Bellonda and individual contributors  *
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

#include "xsdeditor/xschema.h"
#include "xsdeditor/XSchemaIOContants.h"
#include "utils.h"

#define SET_TYPE_FROMSTRING(aType, cfr, theValue) if(cfr==theValue) {_type=aType;}

XSDFacet::XSDFacet(const EType type, const QString &value)
{
    _type = type;
    _value = value ;
    _annotation = NULL ;
    _idPresent = false ;
    _fixed = XEnums::XBOOL_UNSET;
}

XSDFacet::XSDFacet(const QString &tagName, const QString &value)
{
    _type = Enumeration;
    SET_TYPE_FROMSTRING(MinExclusive, "minExclusive", tagName)
    SET_TYPE_FROMSTRING(MinInclusive, "minInclusive", tagName)
    SET_TYPE_FROMSTRING(MaxExclusive, "maxExclusive", tagName)
    SET_TYPE_FROMSTRING(MaxInclusive, "maxInclusive", tagName)
    SET_TYPE_FROMSTRING(TotalDigits, "totalDigits", tagName)
    SET_TYPE_FROMSTRING(FractionDigits, "fractionDigits", tagName)
    SET_TYPE_FROMSTRING(Length, "length", tagName)
    SET_TYPE_FROMSTRING(MinLength, "minLength", tagName)
    SET_TYPE_FROMSTRING(MaxLength, "maxLength", tagName)
    SET_TYPE_FROMSTRING(Enumeration, "enumeration", tagName)
    SET_TYPE_FROMSTRING(WhiteSpace, "whiteSpace", tagName)
    SET_TYPE_FROMSTRING(Pattern, "pattern", tagName)
    _value = value ;
    _annotation = NULL ;
    _idPresent = false ;
    _fixed = XEnums::XBOOL_UNSET;
}

XSDFacet::~XSDFacet()
{
    if(NULL != _annotation) {
        delete _annotation ;
    }
}

QString XSDFacet::typeString()
{
    switch(_type) {
    case MinExclusive: return "minExclusive";
    case MinInclusive: return "minInclusive";
    case MaxExclusive: return "maxExclusive";
    case MaxInclusive: return "maxInclusive";
    case TotalDigits: return "totalDigits";
    case FractionDigits: return "fractionDigits";
    case Length: return "length";
    case MinLength: return "minLength";
    case MaxLength: return "maxLength";
    case Enumeration: return "enumeration";
    case WhiteSpace: return "whiteSpace";
    case Pattern: return "pattern";
    default: return "";
    }
}

XEnums::XBool XSDFacet::fixed()
{
    return _fixed ;
}

void XSDFacet::setFixed(const XEnums::XBool newFixed)
{
    _fixed = newFixed ;
}


bool XSDFacet::hasFixed()
{
    switch(_type) {
    default:
        return false;
    case MinExclusive:
    case MinInclusive:
    case MaxExclusive:
    case MaxInclusive:
    case TotalDigits:
    case FractionDigits:
    case Length:
    case MinLength:
    case MaxLength:
    case WhiteSpace:
        return true ;
    }
}

QString XSDFacet::value()
{
    return _value;
}

void XSDFacet::setValue(const QString &newValue)
{
    _value = newValue ;
}

QString XSDFacet::annotationString()
{
    if(NULL == _annotation) {
        return "" ;
    }
    return _annotation->description();
}

QMap<QString, QString> &XSDFacet::otherAttributes()
{
    return _otherAttributes ;
}

XSDFacet *XSDFacet::clone()
{
    XSDFacet *other = new XSDFacet(_type, _value);
    if(NULL != _annotation) {
        other->_annotation = _annotation->clone();
    }
    foreach(const QString & key, _otherAttributes.keys()) {
        other->_otherAttributes.insert(key, _otherAttributes[key]);
    }
    other->_id = _id ;
    other->_idPresent = _idPresent ;
    other->_fixed = _fixed ;

    return other ;
}

bool XSDFacet::compareTo(const XSDFacet *other)
{
    if(NULL == other) {
        return false;
    }
    bool sameAnnot = true ;
    if((NULL != other->_annotation) || (NULL != _annotation)) {
        if(NULL != other->_annotation) {
            sameAnnot = other->_annotation->compareToSimple(_annotation);
        } else if(NULL != _annotation) {
            sameAnnot = _annotation->compareToSimple(other->_annotation);
        }
    }
    return (other->_type == _type)
           && (other->_value == _value)
           && sameAnnot;
}

QString XSDFacet::toString()
{
    return QString("[%1] [%2] [%3]").arg(_type).arg(_value).arg((NULL == _annotation ? "" : _annotation->text()));
}


XSchemaAnnotation *XSDFacet::annotation() const
{
    return _annotation;
}

void XSDFacet::setAnnotation(XSchemaAnnotation *value)
{
    if(NULL != _annotation) {
        delete _annotation ;
    }
    _annotation = value;
}

XSDFacet::EType XSDFacet::type() const
{
    return _type;
}

void XSDFacet::setType(const EType value)
{
    _type = value;
}

QSet<QString> XSDFacet::allTags()
{
    QSet<QString> tags;
    tags << "minExclusive";
    tags << "minInclusive";
    tags << "maxExclusive";
    tags << "maxInclusive";
    tags << "totalDigits";
    tags << "fractionDigits";
    tags << "length";
    tags << "minLength";
    tags << "maxLength";
    tags << "enumeration";
    tags << "whiteSpace";
    tags << "pattern";

    return tags ;
}

bool XSDFacet::idPresent() const
{
    return _idPresent;
}

void XSDFacet::setIdPresent(bool value)
{
    _idPresent = value;
}

QString XSDFacet::id() const
{
    return _id;
}

void XSDFacet::setId(const QString &id)
{
    _id = id;
}

void XSDFacet::setFixedString(const QString &value)
{
    if(value == "true") {
        _fixed = XEnums::XBOOL_TRUE;
    } else if(value == "false") {
        _fixed = XEnums::XBOOL_FALSE;
    } else {
        _fixed = XEnums::XBOOL_UNSET;
    }
}
