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


#ifndef XSDOPERATIONFACTORY_H
#define XSDOPERATIONFACTORY_H

#include "xsdhelperoperations.h"

class XSDOperationFactory
{

    XSDOper *getBaseComplexTypeNoRef();
    XSDOper *getBaseSimpleTypeNoRef();
    XSDOper *getBaseRef();

    void addAnnotation(XSDOper *parent);
    void addAttributesAndGroups(XSDOper *parent);
    XSDOper *createComplexType(const QString &typeName);
    XSDOper *createSimpleType(const QString &typeName);
    XSDOper *getComplexTypeBaseElement(XSDOper **complexType);
    XSDOper *getSimpleTypeBaseElement(XSDOper **simpleType);
    XSDOper *getSimpleTypeBaseAttribute(XSDOper **simpleType);
    XSDOper *getComplexTypeBaseType();
    XSDOper *getSimpleTypeBaseType(const QString &typeName = "");
    XSDOper *getComplexContent();
    XSDOper *getSimpleContent();
    XSDOper *createSimpleContentRestriction(XSDOper *complexType);
    XSDOper *createSimpleContentExtension(XSDOper *complexType);
    XSDOper *createComplexTypeNone(XSDOper *complexType);
    XSDOper *createComplexTypeAll(XSDOper *complexType);
    XSDOper *createComplexTypeChoice(XSDOper *complexType);
    XSDOper *createComplexTypeGroup(XSDOper *complexType);
    XSDOper *createComplexTypeSequence(XSDOper *complexType);
    XSDOper *createComplexTypeComplexContentExtension(XSDOper *complexType);
    XSDOper *createComplexTypeComplexContentRestriction(XSDOper *complexType);
    XSDOper *createComplexTypeSimpleTypeNone(XSDOper *complexType);

    XSDOper *createSimpleTypeRestriction(XSDOper *simpleType);
    XSDOper *createSimpleTypeNone(XSDOper *simpleType);
    XSDOper *createSimpleTypeList(XSDOper *simpleType);
    XSDOper *createSimpleTypeUnion(XSDOper *simpleType);


    XSDOper *addStay(XSDOper *parent, const QString &name);
    XSDOper *addAdd(XSDOper *parent, const QString &name, const bool isAnnotation = false);

public:
    XSDOperationFactory();
    ~XSDOperationFactory();

    XSDOper *getComplexTypeReferenceElement();
    XSDOper *getSimpleTypeReferenceAttribute();
    XSDOper *getComplexTypeComplexContentRestrictionElement();
    XSDOper *getComplexTypeComplexContentExtensionElement();
    XSDOper *getComplexTypeNoneElement();
    XSDOper *getComplexTypeNoneType();
    XSDOper *getComplexTypeSequenceElement();
    XSDOper *getComplexTypeSequenceType();
    XSDOper *getComplexTypeChoiceElement();
    XSDOper *getComplexTypeChoiceType();
    XSDOper *getComplexTypeAllElement();
    XSDOper *getComplexTypeAllType();
    XSDOper *getComplexTypeGroupElement();
    XSDOper *getComplexTypeGroupType();
    XSDOper *getComplexTypeSimpleContentRestrictionElement();
    XSDOper *getComplexTypeSimpleContentExtensionElement();
    XSDOper *getComplexTypeComplexContentExtensionType();
    XSDOper *getComplexTypeComplexContentRestrictionType();
    //---
    XSDOper *getComplexTypeSimpleContentRestrictionType();
    XSDOper *getComplexTypeSimpleContentExtensionType();
    XSDOper *getComplexTypeSimpleContentNoneType();
    XSDOper *getComplexTypeSimpleContentNoneElement();
    XSDOper *getComplexTypeSimpleContentUnionType();
    XSDOper *getComplexTypeSimpleContentUnionElement();
    XSDOper *getComplexTypeSimpleContentListType();
    XSDOper *getComplexTypeSimpleContentListElement();
    //--
    XSDOper *getSimpleTypeRestrictionElement();
    XSDOper *getSimpleTypeRestrictionAttribute();
    XSDOper *getSimpleTypeRestrictionType();
    XSDOper *getSimpleTypeNoneElement();
    XSDOper *getSimpleTypeNoneAttribute();
    XSDOper *getSimpleTypeNoneType();
    XSDOper *getSimpleTypeListElement();
    XSDOper *getSimpleTypeListType();
    XSDOper *getSimpleTypeUnionElement();
    XSDOper *getSimpleTypeUnionType();
    XSDOper *getSimpleTypeUnionAttribute();
    XSDOper *getSimpleTypeListAttribute();
    //--
    XSDOper *getSimpleTypeReference();
    XSDOper *getSimpleTypeElement();

};

#endif // XSDOPERATIONFACTORY_H
