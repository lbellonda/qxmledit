/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
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

#ifndef XSCHEMAIOCONTANTS_H
#define XSCHEMAIOCONTANTS_H

/**
  definitions for xsd generation
  */


//------- schema
#define IO_XSD_SCHEMA "schema"
#define IO_SCHEMA_ATTR_AFORMDEFAULT "attributeFormDefault"
#define IO_SCHEMA_ATTR_EFORMDEFAULT "elementFormDefault"
#define IO_SCHEMA_QUALIFIED "qualified"
#define IO_SCHEMA_UNQUALIFIED "unqualified"
#define IO_SCHEMA_ATTR_TARGETNAMESPACE "targetNamespace"
#define IO_SCHEMA_ATTR_ID "id"
#define IO_SCHEMA_ATTR_VERSION  "version"
#define IO_SCHEMA_ATTR_BLOCKDEFAULT "blockDefault"
#define IO_SCHEMA_ATTR_FINALDEFAULT "finalDefault"

#define XML_LANGUAGE    "xml:lang"
#define XML_PREFIX    "xml"
#define IO_XML_LANGUAGE    "lang"
#define XML_NS_PREFIX   "xmlns"

#define FINALTYPE_ALL_STRING   "#all"
#define FINALTYPE_RESTRICTION_STRING   "restriction"
#define FINALTYPE_EXTENSION_STRING  "extension"
#define FINALTYPE_LIST_STRING "list"
#define FINALTYPE_UNION_STRING "union"
#define FINALTYPE_SUBSTITUTION_STRING "substitution"

//-- element and types

#define IO_XSD_TAGELEMENT "element"
#define IO_XSD_TAGINCLUDE   "include"
#define IO_XSD_TAGIMPORT    "import"
#define IO_XSD_TAGREDEFINE  "redefine"
#define IO_XSD_TAGGROUP     "group"
#define IO_XSD_TAGATTRIBUTEGROUP    "attributeGroup"
#define IO_XSD_TAGATTRIBUTE "attribute"
#define IO_XSD_TAGNOTATION  "notation"

#define IO_XSD_SIMPLETYPE   "simpleType"
#define IO_XSD_COMPLEXTYPE  "complexType"

#define IO_XSD_ATTRIBUTE  "attribute"

#define IO_XSD_UNIQUE   "unique"
#define IO_XSD_KEYREF   "keyref"
#define IO_XSD_KEY   "key"
#define IO_XSD_FIELD    "field"
#define IO_XSD_SELECTOR "selector"
#define IO_XSD_GROUP    "group"

#define IO_GENERIC_ID   "id"
#define IO_GENERIC_NAME "name"
#define IO_GENERIC_BASE "base"
#define IO_GENERIC_NAMESPACE    "namespace"
#define IO_GENERIC_REFER    "refer"
#define IO_GENERIC_REF  "ref"
#define IO_GENERIC_MINOCCURS    "minOccurs"
#define IO_GENERIC_MAXOCCURS    "maxOccurs"

#define IO_GENERIC_ATTR_SCHEMALOCATION  "schemaLocation"

#define IO_ELEMENT_ATTR_ID  "id"
#define IO_ELEMENT_ATTR_NAME    "name"
#define IO_ELEMENT_ATTR_REF "ref"
#define IO_ELEMENT_ATTR_TYPE "type"
#define IO_ELEMENT_ATTR_SUBSTGROUP "substitutionGroup"
#define IO_ELEMENT_ATTR_DEFAULT "default"
#define IO_ELEMENT_ATTR_FIXED   "fixed"
#define IO_ELEMENT_ATTR_FORM    "form"
#define IO_ELEMENT_ATTR_MAXOCCURS   "maxOccurs"
#define IO_ELEMENT_ATTR_MINOCCURS   "minOccurs"
#define IO_ELEMENT_ATTR_NILLABLE    "nillable"
#define IO_ELEMENT_ATTR_ABSTRACT    "abstract"
#define IO_ELEMENT_ATTR_BLOCK   "block"
#define IO_ELEMENT_ATTR_FINAL   "final"

#define IO_ATTRIBUTE_ATTR_DEFAULT   "default"
#define IO_ATTRIBUTE_ATTR_FIXED "fixed"
#define IO_ATTRIBUTE_ATTR_FORM  "form"
#define IO_ATTRIBUTE_ATTR_REF   "ref"
#define IO_ATTRIBUTE_ATTR_TYPE  "type"
#define IO_ATTRIBUTE_ATTR_USE   "use"

#define IO_COMPLEXTYPE_ATTR_ABSTRACT    "abstract"
#define IO_COMPLEXTYPE_ATTR_BLOCK       "block"
#define IO_COMPLEXTYPE_ATTR_FINAL       "final"
#define IO_COMPLEXTYPE_ATTR_ID          "id"
#define IO_SIMPLETYPE_ATTR_FINAL        "final"
#define IO_SIMPLETYPE_ATTR_ID           "id"
#define IO_SIMPLETYPE_ATTR_NAME         "name"

#define IO_SIMPLETYPE_UNION_ATTR_MEMBERTYPES    "memberTypes"
#define IO_SIMPLETYPE_LIST_ATTR_ITEMTYPE          "itemType"

#define IO_XSD_ANNOTATION       "annotation"
#define IO_ANNOTATION_ATTR_ID   "id"

#define IO_XSD_TAGAPPINFO       "appinfo"
#define IO_XSD_DOCUMENTATION    "documentation"

#define IO_DOCUMENTATION_ATTR_SOURCE    "source"
#define IO_APPINFO_ATTR_SOURCE          "source"

#define IO_XSD_EXTENSION     "extension"
#define IO_XSD_RESTRICTION     "restriction"
#define IO_XSD_LIST            "list"
#define IO_XSD_UNION           "union"

#define IO_INCLUDE_ATTR_SCHEMALOCATION  "schemaLocation"

#define IO_NOTATION_ATTR_PUBLIC "public"
#define IO_NOTATION_ATTR_SYSTEM "system"

#define IO_XSD_SELECTOR "selector"
#define IO_XSD_FIELD    "field"
#define IO_XSD_ALL      "all"
#define IO_XSD_CHOICE   "choice"
#define IO_XSD_SEQUENCE "sequence"
#define IO_XSD_SIMPLECONTENT    "simpleContent"
#define IO_XSD_COMPLEXCONTENT   "complexContent"
#define IO_XSD_ATTRIBUTEGROUP   "attributeGroup"
#define IO_XSD_ANYATTRIBUTE     "anyAttribute"
#define IO_XSD_ANY  "any"
#define IO_XSD_REDEFINE "redefine"

#define IO_REDEFINE_ATTR_SCHEMALOCATION_REF "schemaLocation"

#define IO_GENERIC_ATTR_XPATH  "xpath"
#define IO_GENERIC_UNBOUNDED    "unbounded"

#define IO_ANY_ATTR_PROCESSCONTENTS_LAX "lax"
#define IO_ANY_ATTR_PROCESSCONTENTS_SKIP   "skip"
#define IO_ANY_ATTR_PROCESSCONTENTS_STRICT "strict"

#define IO_XSD_SIMPLECONTENT_RESTRICTION    "restriction"
#define IO_XSD_SIMPLECONTENT_EXTENSION  "extension"

#define IO_XSD_COMPLEXCONTENT_RESTRICTION    "restriction"
#define IO_XSD_COMPLEXCONTENT_EXTENSION  "extension"

#define IO_XSD_MINEXCLUSIVE "minExclusive"
#define IO_XSD_MININCLUSIVE "minInclusive"
#define IO_XSD_MAXEXCLUSIVE "maxExclusive"
#define IO_XSD_MAXINCLUSIVE "maxInclusive"
#define IO_XSD_TOTALDIGITS "totalDigits"
#define IO_XSD_FRACTIONDIGITS "fractionDigits"
#define IO_XSD_LENGTH "length"
#define IO_XSD_MINLENGTH "minLength"
#define IO_XSD_MAXLENGTH "maxLength"
#define IO_XSD_ENUMERATION "enumeration"
#define IO_XSD_WHITESPACE "whiteSpace"
#define IO_XSD_PATTERN "pattern"

#define IO_FACET_ATTR_VALUE "value"

#define IO_GENERIC_MIXED       "mixed"
#define IO_GENERIC_ATTR_PROCESSCONTENTS "processContents"

#define IO_ATTRIBUTE_USE_OPTIONAL "optional"
#define IO_ATTRIBUTE_USE_PROHIBITED "prohibited"
#define IO_ATTRIBUTE_USE_REQUIRED "required"
#define IO_ATTRIBUTE_USE_MISSING    ""

#endif // XSCHEMAIOCONTANTS_H
