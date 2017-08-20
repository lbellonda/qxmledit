/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2017 by Luca Bellonda and individual contributors       *
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

#ifndef XSCHEMAOUTLINEELEMENT_H
#define XSCHEMAOUTLINEELEMENT_H

#include "xsdeditor/xschema.h"

class XSchemaInquiryContext;

class XSchemaOutlineContainer : public XSchemaObject
{
    Q_OBJECT
protected:
    XSchemaObject *_target;
public:

    XSchemaOutlineContainer(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaOutlineContainer();

    virtual QString tagName();
    virtual ESchemaType getType()
    {
        return SchemaTypeOutlineContainer ;
    }
    virtual void setSchemaObject(XSchemaObject *target);
    XSchemaObject *schemaObject();
protected:
    virtual bool generateDom(QDomDocument &, QDomNode &);
    XSDCompareObject::EXSDCompareObject innerCompareTo(XSchemaObject *, XSDCompareOptions &);
    void collectOutlineChildrenOfObject(XSchemaInquiryContext &context, QList<XSchemaObject *> &baseElements);
    void collectOutlineContainerChildren(XSchemaInquiryContext &context, XSchemaOutlineContainer *container, XSchemaObject *theSchemaObject);
    void collectOutlineContainerChildrenGroupRef(XSchemaInquiryContext &context, XSchemaGroup *theSchemaGroup);
};

class XSchemaOutlineElement : public XSchemaOutlineContainer
{
    Q_OBJECT

public:

    friend class XSchemaOutlineContainer;
    friend class TestXSDDiff;
private:

    QList<XSchemaObject*> _attributes;
    XSchemaElement *_element;
    XSchemaElement *_realElement;
    bool _isAlreadyProcessed;
protected:
    virtual void reset();
    //void calculateElement();

public:

    XSchemaOutlineElement(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaOutlineElement();

    bool isEnrolled();
    virtual QString tagName();
    virtual ESchemaType getType()
    {
        return SchemaTypeOutlineElement ;
    }

    bool hasAttributes();
    QList<XSchemaObject*> &attributes()
    {
        return _attributes;
    }

    virtual int minOccurrences() ;
    virtual int maxOccurrences() ;

    QString occurrencesDescr();


protected:
    // IO handling
    void decodeInnerElement();

public:
    XSchemaElement *getElement() const;
    void setElement(XSchemaInquiryContext &context, XSchemaElement *value);
private:
    void calculateElement(XSchemaInquiryContext &context);
};


class XSchemaOutlineGroup : public XSchemaOutlineContainer
{
    Q_OBJECT
    bool _isAlreadyProcessed;
    XSchemaGroup *_group;
public:

    XSchemaOutlineGroup(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaOutlineGroup();

    virtual QString description();
    virtual void setSchemaObject(XSchemaObject *target);
    virtual ESchemaType getType()
    {
        return SchemaTypeOutlineGroup ;
    }

    virtual QString name();
    bool isAlreadyProcessed() const;
    void setAlreadyProcessed(bool isAlreadyProcessed);
};

class XSchemaOutlineSequence : public XSchemaOutlineContainer
{
    Q_OBJECT
protected:
    XSchemaSequence *_sequence;
public:

    XSchemaOutlineSequence(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaOutlineSequence();

    virtual QString description();
    virtual void setSchemaObject(XSchemaObject *target);
    virtual ESchemaType getType()
    {
        return SchemaTypeOutlineSequence ;
    }
};

class XSchemaOutlineChoice : public XSchemaOutlineContainer
{
    Q_OBJECT
protected:
    XSchemaChoice *_choice;
public:

    XSchemaOutlineChoice(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaOutlineChoice();

    virtual QString description();
    virtual void setSchemaObject(XSchemaObject *target);
    virtual ESchemaType getType()
    {
        return SchemaTypeOutlineChoice ;
    }
};

class XSchemaOutlineAll : public XSchemaOutlineContainer
{
    Q_OBJECT
    XSchemaAll *_all;
public:

    XSchemaOutlineAll(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaOutlineAll();

    virtual QString description();
    virtual void setSchemaObject(XSchemaObject *target);
    virtual ESchemaType getType()
    {
        return SchemaTypeOutlineAll ;
    }
};

class XSchemaOutlineAny : public XSchemaOutlineContainer
{
    Q_OBJECT
    XSchemaAny *_any;
public:

    XSchemaOutlineAny(XSchemaObject *newParent, XSchemaRoot *newRoot);
    ~XSchemaOutlineAny();

    virtual QString description();
    virtual void setSchemaObject(XSchemaObject *target);
    virtual ESchemaType getType()
    {
        return SchemaTypeOutlineAny ;
    }
};

#endif // XSCHEMAOUTLINEELEMENT_H
