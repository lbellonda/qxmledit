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


#include "xsdoperationfactory.h"
#include "utils.h"
#include "xsdeditor/XSchemaIOContants.h"
#include "modules/xsd/xsdoperationparameters.h"


XSDOperationFactory::XSDOperationFactory()
{
}

XSDOperationFactory::~XSDOperationFactory()
{
}


//-----

XSDOper *XSDOperationFactory::getComplexTypeReferenceElement()
{
    XSDOper *oper = new XSDOper();
    addAnnotation(oper);
    QPair<QString, QString> attrName(IO_ELEMENT_ATTR_NAME, XSDOperationParameters::TokenName);
    oper->attributesToAdd << attrName;
    QPair<QString, QString> attrRef(IO_ELEMENT_ATTR_TYPE, XSDOperationParameters::TokenType);
    oper->attributesToAdd << attrRef;
    oper->attributesToRemove << IO_ELEMENT_ATTR_REF;
    return oper;
}

XSDOper *XSDOperationFactory::getSimpleTypeReferenceAttribute()
{
    XSDOper *oper = new XSDOper();
    addAnnotation(oper);
    QPair<QString, QString> attrName(IO_ELEMENT_ATTR_NAME, XSDOperationParameters::TokenName);
    oper->attributesToAdd << attrName;
    QPair<QString, QString> attrRef(IO_ELEMENT_ATTR_TYPE, XSDOperationParameters::TokenType);
    oper->attributesToAdd << attrRef;
    oper->attributesToRemove << IO_ELEMENT_ATTR_REF;
    return oper;
}


void XSDOperationFactory::addAnnotation(XSDOper *parent)
{
    XSDOper *oper = new XSDOper();
    oper->name = IO_XSD_ANNOTATION;
    oper->canStay = true ;
    parent->children.append(oper);
}

void XSDOperationFactory::addAttributesAndGroups(XSDOper *parent)
{
    XSDOper *oper = new XSDOper();
    oper->name = IO_XSD_ATTRIBUTE;
    oper->canStay = true ;
    parent->children.append(oper);
    oper = new XSDOper();
    oper->name = IO_XSD_ATTRIBUTEGROUP;
    oper->canStay = true ;
    parent->children.append(oper);
}


XSDOper *XSDOperationFactory::createComplexType(const QString &typeName)
{
    XSDOper *oper = new XSDOper();
    oper->name = IO_XSD_COMPLEXTYPE;
    oper->add = true ;
    if(!typeName.isEmpty()) {
        QPair<QString, QString> attrName(IO_ELEMENT_ATTR_NAME, XSDOperationParameters::TokenName);
        oper->attributesToAdd << attrName;
    }
    addAnnotation(oper);
    return oper;
}

XSDOper *XSDOperationFactory::createSimpleType(const QString &typeName)
{
    XSDOper *oper = new XSDOper();
    oper->name = IO_XSD_SIMPLETYPE;
    oper->add = true ;
    if(!typeName.isEmpty()) {
        QPair<QString, QString> attrName(IO_ELEMENT_ATTR_NAME, typeName);
        oper->attributesToAdd << attrName;
    }
    addAnnotation(oper);
    return oper;
}


XSDOper *XSDOperationFactory::getBaseComplexTypeNoRef()
{
    XSDOper *oper = new XSDOper();
    addAnnotation(oper);
    QPair<QString, QString> attrName(IO_ELEMENT_ATTR_NAME, XSDOperationParameters::TokenName);
    oper->attributesToAdd << attrName;
    oper->attributesToRemove << IO_ELEMENT_ATTR_REF;
    oper->attributesToRemove << IO_ELEMENT_ATTR_TYPE;
    return oper;
}

XSDOper *XSDOperationFactory::getBaseSimpleTypeNoRef()
{
    XSDOper *oper = new XSDOper();
    addAnnotation(oper);
    QPair<QString, QString> attrName(IO_ELEMENT_ATTR_NAME, XSDOperationParameters::TokenName);
    oper->attributesToAdd << attrName;
    oper->attributesToRemove << IO_ELEMENT_ATTR_REF;
    oper->attributesToRemove << IO_ELEMENT_ATTR_TYPE;
    return oper;
}

XSDOper *XSDOperationFactory::getBaseRef()
{
    XSDOper *oper = new XSDOper();
    addAnnotation(oper);
    QPair<QString, QString> attrName(IO_ELEMENT_ATTR_NAME, XSDOperationParameters::TokenName);
    oper->attributesToAdd << attrName;
    QPair<QString, QString> attrType(IO_ELEMENT_ATTR_TYPE, XSDOperationParameters::TokenType);
    oper->attributesToAdd << attrType;
    oper->attributesToRemove << IO_ELEMENT_ATTR_REF;
    return oper;
}


XSDOper *XSDOperationFactory::getComplexTypeBaseElement(XSDOper **complexType)
{
    XSDOper *oper = getBaseComplexTypeNoRef();

    *complexType = createComplexType("");
    oper->children.append(*complexType);

    addAttributesAndGroups(oper);

    return oper;
}

XSDOper *XSDOperationFactory::getSimpleTypeBaseElement(XSDOper **simpleType)
{
    XSDOper *oper = getBaseSimpleTypeNoRef();
    *simpleType = createSimpleType("");
    oper->children.append(*simpleType);

    return oper;
}

XSDOper *XSDOperationFactory::getSimpleTypeBaseAttribute(XSDOper **simpleType)
{
    XSDOper *oper = getBaseSimpleTypeNoRef();
    *simpleType = createSimpleType("");
    oper->children.append(*simpleType);

    return oper;
}

XSDOper *XSDOperationFactory::getComplexTypeBaseType()
{
    XSDOper *complexType = createComplexType(XSDOperationParameters::TokenName);

    addAttributesAndGroups(complexType);

    return complexType;
}

XSDOper *XSDOperationFactory::getSimpleTypeBaseType(const QString &typeName)
{
    XSDOper *simpleType = createSimpleType(typeName);

    return simpleType;
}


XSDOper *XSDOperationFactory::getComplexContent()
{
    XSDOper *complexContent = new XSDOper();
    complexContent->name = IO_XSD_COMPLEXCONTENT;
    complexContent->add = true ;
    addAnnotation(complexContent);
    return complexContent;
}

XSDOper *XSDOperationFactory::getSimpleContent()
{
    XSDOper *complexContent = new XSDOper();
    complexContent->name = IO_XSD_SIMPLECONTENT;
    complexContent->add = true ;
    addAnnotation(complexContent);
    return complexContent;
}


XSDOper *XSDOperationFactory::createComplexTypeComplexContentRestriction(XSDOper *complexType)
{
    XSDOper *complexContent = getComplexContent();
    complexType->children.append(complexContent);

    XSDOper *restriction = new XSDOper();
    complexContent->children.append(restriction);
    restriction->name = IO_XSD_COMPLEXCONTENT_RESTRICTION;
    restriction->add = true ;
    QPair<QString, QString> attrRef(IO_GENERIC_BASE, XSDOperationParameters::TokenType);
    restriction->attributesToAdd << attrRef;

    addStay(restriction, IO_XSD_ANNOTATION);
    addStay(restriction, IO_XSD_GROUP);
    addStay(restriction, IO_XSD_ALL);
    addStay(restriction, IO_XSD_CHOICE);
    addStay(restriction, IO_XSD_SEQUENCE);
    addStay(restriction, IO_XSD_ATTRIBUTE);
    addStay(restriction, IO_XSD_ATTRIBUTEGROUP);

    return complexContent;
}

XSDOper *XSDOperationFactory::createComplexTypeComplexContentExtension(XSDOper *complexType)
{
    XSDOper *complexContent = getComplexContent();
    complexType->children.append(complexContent);

    XSDOper *extension = new XSDOper();
    complexContent->children.append(extension);
    extension->name = IO_XSD_COMPLEXCONTENT_EXTENSION;
    extension->add = true ;
    QPair<QString, QString> attrRef(IO_GENERIC_BASE, XSDOperationParameters::TokenType);
    extension->attributesToAdd << attrRef;

    addStay(extension, IO_XSD_ANNOTATION);
    addStay(extension, IO_XSD_GROUP);
    addStay(extension, IO_XSD_ALL);
    addStay(extension, IO_XSD_CHOICE);
    addStay(extension, IO_XSD_SEQUENCE);
    addStay(extension, IO_XSD_ATTRIBUTE);
    addStay(extension, IO_XSD_ATTRIBUTEGROUP);

    return complexContent;
}

XSDOper *XSDOperationFactory::getComplexTypeComplexContentExtensionElement()
{
    XSDOper *complexType = NULL ;
    XSDOper *oper = getComplexTypeBaseElement(&complexType);

    createComplexTypeComplexContentExtension(complexType);

    return oper;
}

XSDOper *XSDOperationFactory::getComplexTypeComplexContentExtensionType()
{
    XSDOper *complexType = getComplexTypeBaseType();
    createComplexTypeComplexContentExtension(complexType);

    return complexType ;
}

XSDOper *XSDOperationFactory::getComplexTypeComplexContentRestrictionType()
{
    XSDOper *complexType = getComplexTypeBaseType();
    createComplexTypeComplexContentRestriction(complexType);

    return complexType;
}

XSDOper *XSDOperationFactory::getComplexTypeComplexContentRestrictionElement()
{
    XSDOper *complexType = NULL ;
    XSDOper *oper = getComplexTypeBaseElement(&complexType);

    createComplexTypeComplexContentRestriction(complexType);

    return oper;
}

XSDOper *XSDOperationFactory::createComplexTypeSequence(XSDOper *complexType)
{
    XSDOper *seq = addAdd(complexType, IO_XSD_SEQUENCE, true);
    addStay(seq, IO_XSD_ANNOTATION);
    addStay(seq, IO_XSD_TAGELEMENT);
    addStay(seq, IO_XSD_GROUP);
    addStay(seq, IO_XSD_CHOICE);
    addStay(seq, IO_XSD_SEQUENCE);
    return seq;
}


XSDOper *XSDOperationFactory::getComplexTypeSequenceElement()
{
    XSDOper *complexType = NULL ;
    XSDOper *oper = getComplexTypeBaseElement(&complexType);

    createComplexTypeSequence(complexType);

    return oper;
}

XSDOper *XSDOperationFactory::getComplexTypeSequenceType()
{
    XSDOper *complexType = getComplexTypeBaseType();
    createComplexTypeSequence(complexType);

    return complexType ;
}

XSDOper *XSDOperationFactory::getComplexTypeNoneElement()
{
    XSDOper *complexType = NULL ;
    XSDOper *oper = getComplexTypeBaseElement(&complexType);

    createComplexTypeNone(complexType);

    return oper;
}

XSDOper *XSDOperationFactory::getComplexTypeNoneType()
{
    XSDOper *complexType = getComplexTypeBaseType();
    createComplexTypeNone(complexType);

    return complexType ;
}


XSDOper *XSDOperationFactory::createComplexTypeNone(XSDOper *complexType)
{
    addStay(complexType, IO_XSD_ANNOTATION);
    addStay(complexType, IO_XSD_COMPLEXCONTENT);
    addStay(complexType, IO_XSD_SIMPLECONTENT);
    addStay(complexType, IO_XSD_ALL);
    addStay(complexType, IO_XSD_GROUP);
    addStay(complexType, IO_XSD_CHOICE);
    addStay(complexType, IO_XSD_SEQUENCE);
    return complexType;
}

XSDOper *XSDOperationFactory::getComplexTypeSimpleContentNoneType()
{
    XSDOper *complexType = getComplexTypeBaseType();
    createComplexTypeSimpleTypeNone(complexType);

    return complexType ;
}

XSDOper *XSDOperationFactory::getComplexTypeSimpleContentNoneElement()
{
    XSDOper *complexType = NULL ;
    XSDOper *oper = getComplexTypeBaseElement(&complexType);

    createComplexTypeSimpleTypeNone(complexType);

    return oper;
}


XSDOper *XSDOperationFactory::createComplexTypeSimpleTypeNone(XSDOper *complexType)
{
    addStay(complexType, IO_XSD_ANNOTATION);
    addStay(complexType, IO_XSD_SIMPLECONTENT);
    return complexType;
}


XSDOper *XSDOperationFactory::getComplexTypeAllElement()
{
    XSDOper *complexType = NULL ;
    XSDOper *oper = getComplexTypeBaseElement(&complexType);

    createComplexTypeAll(complexType);

    return oper;
}

XSDOper *XSDOperationFactory::getComplexTypeAllType()
{
    XSDOper *complexType = getComplexTypeBaseType();
    createComplexTypeAll(complexType);

    return complexType ;
}

XSDOper *XSDOperationFactory::createComplexTypeAll(XSDOper *complexType)
{
    XSDOper *all = addAdd(complexType, IO_XSD_ALL, true);
    addStay(all, IO_XSD_ANNOTATION);
    addStay(all, IO_XSD_TAGELEMENT);
    return all;
}

//-----
XSDOper *XSDOperationFactory::getComplexTypeChoiceElement()
{
    XSDOper *complexType = NULL ;
    XSDOper *oper = getComplexTypeBaseElement(&complexType);

    createComplexTypeChoice(complexType);

    return oper;
}

XSDOper *XSDOperationFactory::getComplexTypeChoiceType()
{
    XSDOper *complexType = getComplexTypeBaseType();
    createComplexTypeChoice(complexType);

    return complexType ;
}

XSDOper *XSDOperationFactory::createComplexTypeChoice(XSDOper *complexType)
{
    XSDOper *choice = addAdd(complexType, IO_XSD_CHOICE, true);
    addStay(choice, IO_XSD_ANNOTATION);
    addStay(choice, IO_XSD_TAGELEMENT);
    addStay(choice, IO_XSD_GROUP);
    addStay(choice, IO_XSD_CHOICE);
    addStay(choice, IO_XSD_SEQUENCE);
    return choice;
}

//---------------------------------

XSDOper *XSDOperationFactory::createComplexTypeGroup(XSDOper *complexType)
{
    XSDOper *group = addAdd(complexType, IO_XSD_GROUP, true);
    addStay(group, IO_XSD_ANNOTATION);
    addStay(group, IO_XSD_ALL);
    addStay(group, IO_XSD_CHOICE);
    addStay(group, IO_XSD_SEQUENCE);
    return group;
}

XSDOper *XSDOperationFactory::getComplexTypeGroupElement()
{
    XSDOper *complexType = NULL ;
    XSDOper *oper = getComplexTypeBaseElement(&complexType);

    createComplexTypeGroup(complexType);

    return oper;
}

XSDOper *XSDOperationFactory::getComplexTypeGroupType()
{
    XSDOper *complexType = getComplexTypeBaseType();
    createComplexTypeGroup(complexType);

    return complexType ;
}

//---------------------------------

XSDOper *XSDOperationFactory::addStay(XSDOper *parent, const QString &name)
{
    XSDOper *child = new XSDOper();
    parent->children.append(child);
    child ->name = name;
    child ->canStay = true ;

    return child;
}


XSDOper *XSDOperationFactory::addAdd(XSDOper *parent, const QString &name, const bool isAnnotation)
{
    XSDOper *child = new XSDOper();
    parent->children.append(child);
    child ->name = name;
    child ->add = true ;
    if(isAnnotation) {
        addAnnotation(child);
    }

    return child;
}

XSDOper *XSDOperationFactory::createSimpleContentRestriction(XSDOper *complexType)
{
    XSDOper *simpleContent = getSimpleContent();
    complexType->children.append(simpleContent);

    XSDOper *restriction = new XSDOper();
    simpleContent->children.append(restriction);
    restriction->name = IO_XSD_SIMPLECONTENT_RESTRICTION;
    restriction->add = true ;
    QPair<QString, QString> attrRef(IO_GENERIC_BASE, XSDOperationParameters::TokenType);
    restriction->attributesToAdd << attrRef;

    addStay(restriction, IO_XSD_ANNOTATION);
    addStay(restriction, IO_XSD_SIMPLETYPE);
    addStay(restriction, IO_XSD_MINEXCLUSIVE);
    addStay(restriction, IO_XSD_MININCLUSIVE);
    addStay(restriction, IO_XSD_MAXEXCLUSIVE);
    addStay(restriction, IO_XSD_MAXINCLUSIVE);
    addStay(restriction, IO_XSD_TOTALDIGITS);
    addStay(restriction, IO_XSD_FRACTIONDIGITS);
    addStay(restriction, IO_XSD_LENGTH);
    addStay(restriction, IO_XSD_MINLENGTH);
    addStay(restriction, IO_XSD_MAXLENGTH);
    addStay(restriction, IO_XSD_ENUMERATION);
    addStay(restriction, IO_XSD_WHITESPACE);
    addStay(restriction, IO_XSD_PATTERN);
    addStay(restriction, IO_XSD_ATTRIBUTE);
    addStay(restriction, IO_XSD_ATTRIBUTEGROUP);
    return complexType;
}


XSDOper *XSDOperationFactory::createSimpleContentExtension(XSDOper *complexType)
{
    XSDOper *simpleContent = getSimpleContent();
    complexType->children.append(simpleContent);

    XSDOper *extension = new XSDOper();
    simpleContent->children.append(extension);
    extension->name = IO_XSD_SIMPLECONTENT_EXTENSION;
    extension->add = true ;
    QPair<QString, QString> attrRef(IO_GENERIC_BASE, XSDOperationParameters::TokenType);
    extension->attributesToAdd << attrRef;

    addStay(extension, IO_XSD_ANNOTATION);
    addStay(extension, IO_XSD_ATTRIBUTE);
    addStay(extension, IO_XSD_ATTRIBUTEGROUP);

    return simpleContent;
}

XSDOper *XSDOperationFactory::getComplexTypeSimpleContentRestrictionType()
{
    XSDOper *complexType = getComplexTypeBaseType();

    createSimpleContentRestriction(complexType);

    return complexType ;
}

XSDOper *XSDOperationFactory::getComplexTypeSimpleContentExtensionType()
{
    XSDOper *complexType = getComplexTypeBaseType();

    createSimpleContentExtension(complexType);

    return complexType ;
}


XSDOper *XSDOperationFactory::getComplexTypeSimpleContentRestrictionElement()
{
    XSDOper *complexType = NULL ;
    XSDOper *oper = getComplexTypeBaseElement(&complexType);

    createSimpleContentRestriction(complexType);

    return oper;
}

XSDOper *XSDOperationFactory::getComplexTypeSimpleContentExtensionElement()
{
    XSDOper *complexType = NULL ;
    XSDOper *oper = getComplexTypeBaseElement(&complexType);

    createSimpleContentExtension(complexType);
    return oper;
}

XSDOper *XSDOperationFactory::getComplexTypeSimpleContentListType()
{
    XSDOper *complexType = getComplexTypeBaseType();

    createSimpleTypeList(complexType);

    return complexType ;
}

XSDOper *XSDOperationFactory::getComplexTypeSimpleContentListElement()
{
    XSDOper *complexType = NULL ;
    XSDOper *oper = getComplexTypeBaseElement(&complexType);

    createSimpleTypeList(complexType);

    return oper;
}

XSDOper *XSDOperationFactory::getComplexTypeSimpleContentUnionType()
{
    XSDOper *complexType = getComplexTypeBaseType();

    createSimpleTypeUnion(complexType);

    return complexType ;
}

XSDOper *XSDOperationFactory::getComplexTypeSimpleContentUnionElement()
{
    XSDOper *complexType = NULL ;
    XSDOper *oper = getComplexTypeBaseElement(&complexType);

    createSimpleTypeUnion(complexType);

    return oper;
}
//---------------------

XSDOper *XSDOperationFactory::getSimpleTypeRestrictionElement()
{
    XSDOper *simpleType = NULL ;
    XSDOper *oper = getSimpleTypeBaseElement(&simpleType);

    createSimpleTypeRestriction(simpleType);

    return oper;
}

XSDOper *XSDOperationFactory::getSimpleTypeRestrictionAttribute()
{
    XSDOper *simpleType = NULL ;
    XSDOper *oper = getSimpleTypeBaseAttribute(&simpleType);

    createSimpleTypeRestriction(simpleType);

    return oper;
}

XSDOper *XSDOperationFactory::getSimpleTypeRestrictionType()
{
    XSDOper *simpleType = getSimpleTypeBaseType(XSDOperationParameters::TokenName);

    createSimpleTypeRestriction(simpleType);

    return simpleType;
}

XSDOper *XSDOperationFactory::getSimpleTypeNoneElement()
{
    XSDOper *simpleType = NULL ;
    XSDOper *oper = getSimpleTypeBaseElement(&simpleType);

    createSimpleTypeNone(simpleType);

    return oper;
}

XSDOper *XSDOperationFactory::getSimpleTypeNoneAttribute()
{
    XSDOper *oper = getBaseRef();
    return oper;
}

XSDOper *XSDOperationFactory::getSimpleTypeNoneType()
{
    XSDOper *simpleType = getSimpleTypeBaseType(XSDOperationParameters::TokenName);

    createSimpleTypeNone(simpleType);

    return simpleType;
}


XSDOper *XSDOperationFactory::getSimpleTypeListElement()
{
    XSDOper *simpleType = NULL ;
    XSDOper *oper = getSimpleTypeBaseElement(&simpleType);

    createSimpleTypeList(simpleType);

    return oper;
}

XSDOper *XSDOperationFactory::getSimpleTypeListAttribute()
{
    XSDOper *simpleType = NULL ;
    XSDOper *oper = getSimpleTypeBaseAttribute(&simpleType);

    createSimpleTypeList(simpleType);

    return oper;
}

XSDOper *XSDOperationFactory::getSimpleTypeListType()
{
    XSDOper *simpleType = getSimpleTypeBaseType(XSDOperationParameters::TokenName);

    createSimpleTypeList(simpleType);

    return simpleType;
}

XSDOper *XSDOperationFactory::getSimpleTypeUnionElement()
{
    XSDOper *simpleType = NULL ;
    XSDOper *oper = getSimpleTypeBaseElement(&simpleType);

    createSimpleTypeUnion(simpleType);

    return oper;
}


XSDOper *XSDOperationFactory::getSimpleTypeUnionAttribute()
{
    XSDOper *simpleType = NULL ;
    XSDOper *oper = getSimpleTypeBaseAttribute(&simpleType);

    createSimpleTypeUnion(simpleType);

    return oper;
}

XSDOper *XSDOperationFactory::getSimpleTypeUnionType()
{
    XSDOper *simpleType = getSimpleTypeBaseType(XSDOperationParameters::TokenName);

    createSimpleTypeUnion(simpleType);

    return simpleType;
}

XSDOper *XSDOperationFactory::getSimpleTypeReference()
{
    XSDOper *simpleType = getSimpleTypeBaseType();
    simpleType->attributesToAdd << QPair<QString, QString>(IO_GENERIC_NAME, XSDOperationParameters::TokenName);
    simpleType->attributesToRemove << IO_ATTRIBUTE_ATTR_TYPE;

    return simpleType;
}

XSDOper *XSDOperationFactory::getSimpleTypeElement()
{
    XSDOper *simpleType = NULL ;
    XSDOper *oper = getSimpleTypeBaseElement(&simpleType);

    oper->attributesToRemove <<  IO_ATTRIBUTE_ATTR_REF;
    simpleType->attributesToAdd << QPair<QString, QString>(IO_GENERIC_NAME, XSDOperationParameters::TokenName);
    simpleType->attributesToAdd << QPair<QString, QString>(IO_ATTRIBUTE_ATTR_TYPE, XSDOperationParameters::TokenType);

    return oper;
}

//---------

XSDOper *XSDOperationFactory::createSimpleTypeUnion(XSDOper *simpleType)
{
    XSDOper *xunion = addAdd(simpleType, IO_XSD_UNION);
    addStay(xunion, IO_XSD_ANNOTATION);
    addStay(xunion, IO_XSD_SIMPLETYPE);
    return simpleType;
}

XSDOper *XSDOperationFactory::createSimpleTypeList(XSDOper *simpleType)
{
    XSDOper *list = addAdd(simpleType, IO_XSD_LIST);
    addStay(list, IO_XSD_ANNOTATION);
    addStay(list, IO_XSD_SIMPLETYPE);
    return simpleType;
}

XSDOper *XSDOperationFactory::createSimpleTypeNone(XSDOper *simpleType)
{
    addStay(simpleType, IO_XSD_LIST);
    addStay(simpleType, IO_XSD_UNION);
    addStay(simpleType, IO_XSD_RESTRICTION);
    return simpleType;
}

XSDOper *XSDOperationFactory::createSimpleTypeRestriction(XSDOper *simpleType)
{
    XSDOper *restriction = addAdd(simpleType, IO_XSD_RESTRICTION);
    restriction->attributesToAdd << QPair<QString, QString>(IO_GENERIC_BASE, XSDOperationParameters::TokenType);
    addStay(restriction, IO_XSD_ANNOTATION);
    addStay(restriction, IO_XSD_SIMPLETYPE);
    addStay(restriction, IO_XSD_MINEXCLUSIVE);
    addStay(restriction, IO_XSD_MININCLUSIVE);
    addStay(restriction, IO_XSD_MAXEXCLUSIVE);
    addStay(restriction, IO_XSD_MAXINCLUSIVE);
    addStay(restriction, IO_XSD_TOTALDIGITS);
    addStay(restriction, IO_XSD_FRACTIONDIGITS);
    addStay(restriction, IO_XSD_LENGTH);
    addStay(restriction, IO_XSD_MINLENGTH);
    addStay(restriction, IO_XSD_MAXLENGTH);
    addStay(restriction, IO_XSD_ENUMERATION);
    addStay(restriction, IO_XSD_WHITESPACE);
    addStay(restriction, IO_XSD_PATTERN);
    addStay(restriction, IO_XSD_ATTRIBUTE);
    addStay(restriction, IO_XSD_ATTRIBUTEGROUP);
    return simpleType;
}
