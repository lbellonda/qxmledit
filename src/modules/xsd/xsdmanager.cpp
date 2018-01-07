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


#include "xsdmanager.h"
#include "xsdeditor/XSchemaIOContants.h"
#include "utils.h"

XSDManager::XSDManager()
{
    _modifiable.insert(IO_XSD_TAGELEMENT);
    _modifiable.insert(IO_XSD_ATTRIBUTE);
    _modifiable.insert(IO_XSD_COMPLEXTYPE);
    _modifiable.insert(IO_XSD_SIMPLETYPE);
    //---
    _attributeParents.insert(IO_XSD_SCHEMA);
    _attributeParents.insert(IO_XSD_ATTRIBUTEGROUP);
    _attributeParents.insert(IO_XSD_COMPLEXTYPE);
    _attributeParents.insert(IO_XSD_EXTENSION);
    _attributeParents.insert(IO_XSD_RESTRICTION);
    //--
    _typeParents.insert(IO_XSD_SCHEMA);
    _typeParents.insert(IO_XSD_REDEFINE);
    _typeParents.insert(IO_XSD_ATTRIBUTE);
    _typeParents.insert(IO_XSD_TAGELEMENT);
    _typeParents.insert(IO_XSD_LIST);
    _typeParents.insert(IO_XSD_RESTRICTION);
    _typeParents.insert(IO_XSD_UNION);

    //--
    _elementParents.insert(IO_XSD_SCHEMA);
    _elementParents.insert(IO_XSD_CHOICE);
    _elementParents.insert(IO_XSD_SEQUENCE);
    _elementParents.insert(IO_XSD_ALL);
    //----
    _annotationParents << IO_XSD_ALL << IO_XSD_ANY << IO_XSD_ANYATTRIBUTE << IO_XSD_ATTRIBUTE ;
    _annotationParents << IO_XSD_ATTRIBUTEGROUP << IO_XSD_CHOICE << IO_XSD_COMPLEXCONTENT ;
    _annotationParents << IO_XSD_COMPLEXTYPE << IO_XSD_TAGELEMENT << IO_XSD_ENUMERATION << IO_XSD_EXTENSION ;
    _annotationParents << IO_XSD_FIELD << IO_XSD_FRACTIONDIGITS << IO_XSD_GROUP << IO_XSD_TAGIMPORT ;
    _annotationParents << IO_XSD_TAGINCLUDE << IO_XSD_KEY << IO_XSD_KEYREF << IO_XSD_LENGTH  << IO_XSD_LIST ;
    _annotationParents << IO_XSD_MAXEXCLUSIVE << IO_XSD_MAXINCLUSIVE << IO_XSD_MAXLENGTH << IO_XSD_MINEXCLUSIVE;
    _annotationParents << IO_XSD_MININCLUSIVE << IO_XSD_MINLENGTH << IO_XSD_TAGNOTATION << IO_XSD_PATTERN ;
    _annotationParents << IO_XSD_REDEFINE << IO_XSD_RESTRICTION << IO_XSD_SCHEMA << IO_XSD_SELECTOR ;
    _annotationParents << IO_XSD_SEQUENCE << IO_XSD_SIMPLECONTENT << IO_XSD_SIMPLETYPE << IO_XSD_TOTALDIGITS ;
    _annotationParents << IO_XSD_UNION << IO_XSD_UNIQUE << IO_XSD_WHITESPACE ;
    //--
    _facetParents.insert(IO_XSD_RESTRICTION);
    //--
    _facets << IO_XSD_RESTRICTION;
    _facets << IO_XSD_MINEXCLUSIVE;
    _facets << IO_XSD_MININCLUSIVE;
    _facets << IO_XSD_MAXEXCLUSIVE;
    _facets << IO_XSD_MAXINCLUSIVE;
    _facets << IO_XSD_TOTALDIGITS;
    _facets << IO_XSD_FRACTIONDIGITS;
    _facets << IO_XSD_LENGTH;
    _facets << IO_XSD_MINLENGTH;
    _facets << IO_XSD_MAXLENGTH;
    _facets << IO_XSD_ENUMERATION;
    _facets << IO_XSD_WHITESPACE;
    _facets << IO_XSD_PATTERN;
    //
}

XSDManager::~XSDManager()
{
}

bool XSDManager::canInsertElement(const QString &parentTag)
{
    if(_elementParents.contains(parentTag)) {
        return true ;
    }
    return false;
}

bool XSDManager::canInsertType(const QString &parentTag)
{
    if(_typeParents.contains(parentTag)) {
        return true ;
    }
    return false;
}

bool XSDManager::canInsertAttribute(const QString &parentTag)
{
    if(_attributeParents.contains(parentTag)) {
        return true ;
    }
    return false;
}

bool XSDManager::canModifyType(const QString &tag)
{
    if(_modifiable.contains(tag)) {
        return true ;
    }
    return false;
}

bool XSDManager::canEditAnnotation(const QString &tag)
{
    if(_annotationParents.contains(tag)) {
        return true ;
    }
    return false;
}

bool XSDManager::canEditFacetAsParent(const QString &tag)
{
    if(_facetParents.contains(tag)) {
        return true ;
    }
    return false;
}

bool XSDManager::canEditFacet(const QString &tag)
{
    if(_facets.contains(tag)) {
        return true ;
    }
    return false;
}
