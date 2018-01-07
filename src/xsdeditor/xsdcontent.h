/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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

#ifndef XSDCONTENT_H
#define XSDCONTENT_H

#include "xsdeditor/xsdtypes.h"
#include "xsdeditor/xproperties.h"
#include "xsdeditor/xschema.h"

// simple and complex content definition

class XSchemaSimpleContent : public XSchemaObject
{
    Q_OBJECT
protected:
    QString _base;
    QString _idContainer;
    XSchemaAnnotation *_mainAnnotation;


    virtual void reset();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    QDomElement writeBaseProperties(QDomElement &element);
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options) = 0 ;

public:
    XSchemaSimpleContent(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaSimpleContent();
    bool readBaseProperties(XSDLoadContext *loadContext, QDomElement &element);

    virtual ESchemaType getType()
    {
        return SchemaTypeSimpleContent;
    }

    static XSchemaSimpleContent *readChild(XSchemaObject *parent, QDomElement &element);
    void collectAttributes(XSchemaAttributesCollection & attributesCollection);
    XSchemaElement *getBaseType();
};


class XSchemaSimpleContentRestriction : public XSchemaSimpleContent
{
    Q_OBJECT

    bool    anyAttribute;
    RestrictionFacets _facets;

protected:
    virtual void reset();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;

    virtual void scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void * context);
    virtual void scanForElements(XSDLoadContext *loadContext, QDomElement &element, void * context);
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options) ;

public:
    XSchemaSimpleContentRestriction(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaSimpleContentRestriction();

    virtual ESchemaType getType()
    {
        return SchemaTypeSimpleContentRestriction ;
    }
    TAG("restriction")
    void addEnumsToListIfAny(QStringList &enums);
};


class XSchemaSimpleContentExtension : public XSchemaSimpleContent
{
    Q_OBJECT

    bool    anyAttribute;

protected:
    virtual void reset();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;

    virtual void scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void * context);
    virtual void scanForElements(XSDLoadContext *loadContext, QDomElement &element, void * context);
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options) ;

public:
    XSchemaSimpleContentExtension(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaSimpleContentExtension();

    virtual ESchemaType getType()
    {
        return SchemaTypeSimpleContentExtension ;
    }
    TAG("extension")
    virtual QString description();

signals:

};


class XSchemaComplexContent : public XSchemaObject
{
    Q_OBJECT
protected:
    bool    anyAttribute;
    QString _base;
    XEnums::XBool _mixed;
    XSchemaAnnotation *_mainAnnotation;
    QString _idContainer;
    //QMap<QString, QString> _otherTypeAttributes;

protected:
    virtual void reset();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;

    void scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void * context);
    void scanForElements(XSDLoadContext *loadContext, QDomElement &element, void * context);
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options) = 0 ;

public:
    XSchemaComplexContent(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaComplexContent();

    bool readBaseProperties(XSDLoadContext *loadContext, QDomElement &element);
    QDomElement writeBaseProperties(QDomElement &element);
    virtual void reparent(XSchemaObject *newParent);

    virtual ESchemaType getType()
    {
        return SchemaTypeComplexContent ;
    }
    TAG("complexContent")
    virtual bool examineType(XValidationContext *context, XSingleElementContent *parent);
    void collectAttributes(XSchemaAttributesCollection & attributesCollection);
    XSchemaElement *getBaseType();
    virtual bool hasAReference();
    virtual QString referencedObjectName();
    virtual XReferenceType referencedObjectType();

signals:

};


class XSchemaComplexContentRestriction : public XSchemaComplexContent
{
    Q_OBJECT

protected:
    virtual void reset();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options) ;

public:
    XSchemaComplexContentRestriction(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaComplexContentRestriction();

    virtual ESchemaType getType()
    {
        return SchemaTypeComplexContentRestriction ;
    }
    TAG("restriction")

    virtual QString description();
signals:

};


class XSchemaComplexContentExtension : public XSchemaComplexContent
{
    Q_OBJECT


protected:
    virtual void reset();
    virtual bool generateDom(QDomDocument &document, QDomNode &parent) ;
    virtual XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *target, XSDCompareOptions &options) ;

public:
    XSchemaComplexContentExtension(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaComplexContentExtension();

    virtual ESchemaType getType()
    {
        return SchemaTypeComplexContentExtension ;
    }
    TAG("extension")
    virtual QString description();
signals:

};

#endif // XSDCONTENT_H
