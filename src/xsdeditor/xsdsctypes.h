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
#ifndef XSDSCTYPES_H
#define XSDSCTYPES_H

#include "xsdeditor/xsdtypes.h"
#include "xsdeditor/xproperties.h"
#include "xsdeditor/xschema.h"

// simple and complex types definition


class XSchemaSimpleType : public XSchemaObject
{
    Q_OBJECT

protected:
    virtual void reset() ;
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) = 0 ;
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options) = 0 ;

public:
    XSchemaSimpleType(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaSimpleType();

    virtual ESchemaType getType()
    {
        return SchemaTypeSimpleType ;
    }
    TAG("simpleContent")

    virtual bool isSimpleType();

    virtual bool canAddRestriction();
    virtual bool canAddUnion();
    virtual bool canAddList();

    XSchemaSimpleTypeRestriction *getSimpleTypeRestriction();

signals:

};


class XSchemaSimpleTypeRestriction : public XSchemaSimpleType
{
    Q_OBJECT

    QString _base;
    RestrictionFacets _facets;

protected:
    virtual void reset();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    virtual void scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void *context);
    virtual void scanForElements(XSDLoadContext *loadContext, QDomElement &element, void *context);
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options);

public:
    XSchemaSimpleTypeRestriction(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaSimpleTypeRestriction();

    virtual QString description();
    QString descriptionDiffHtml();
    virtual ESchemaType getType()
    {
        return SchemaTypeSimpleTypeRestriction ;
    }
    TAG("restriction")

    void addEnumsToListIfAny(QStringList &enums);
    QString exportFacets();

    DECLARE_PROP_S(base, setBase)

signals:

};


class XSchemaSimpleTypeUnion : public XSchemaSimpleType
{
    Q_OBJECT
    QString _memberTypes;

protected:
    virtual void reset();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    virtual void scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void *context);
    virtual void scanForElements(XSDLoadContext *loadContext, QDomElement &element, void *context);
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options);

public:
    XSchemaSimpleTypeUnion(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaSimpleTypeUnion();

    virtual ESchemaType getType()
    {
        return SchemaTypeSimpleTypeUnion ;
    }
    TAG("union")

    virtual QString description();
    virtual bool canAddRestriction();
    virtual bool canAddUnion();
    virtual bool canAddList();

    DECLARE_PROP_S(memberTypes, setMmemberTypes)

signals:

};


class XSchemaSimpleTypeList : public XSchemaSimpleType
{
    Q_OBJECT
    QString _itemType;
protected:
    virtual void reset();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    virtual void scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void *context);
    virtual void scanForElements(XSDLoadContext *loadContext, QDomElement &element, void *context);
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options) ;

public:
    XSchemaSimpleTypeList(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaSimpleTypeList();

    virtual ESchemaType getType()
    {
        return SchemaTypeSimpleTypeList ;
    }
    TAG("list")

    DECLARE_PROP_S(itemType, setItemType)

signals:

};


#endif // XSDSCTYPES_H
