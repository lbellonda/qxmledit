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


#ifndef XSDHELPER_H
#define XSDHELPER_H

#include "regola.h"
#include "modules/xsd/xsdoperationparameters.h"
#include "modules/xsd/xsdhelperoperations.h"
#include "modules/xsd/xsdoperationfactory.h"
#include "xsdeditor/xschema.h"

class XSDHelper
{
    bool execOper(Element *element, XSDOperationParameters *params, XSDOper *oper);


    bool processElement(Regola *regola, Element *targetElement, XSDOperationParameters * params);
    bool processType(Element *targetElement, XSDOperationParameters * params);
    bool processAttribute(Element *targetElement, XSDOperationParameters * params);
    bool elementSimpleType(Regola *regola, Element *targetElement, XSDOperationParameters * params);
    bool attributeSimpleType(Element *targetElement, XSDOperationParameters * params);
    bool processTypeSimpleType(Element *targetElement, XSDOperationParameters * params);
    bool elementSimpleTypeReference(Regola *regola, Element *element, XSDOperationParameters *params);
    bool elementComplexTypeReference(Element *element, XSDOperationParameters *params);

    bool updateElementComplexType(Element *targetElement, XSDOperationParameters * params);
    bool updateTypeComplexType(Element *targetElement, XSDOperationParameters * params);

    bool updateComplexContentRestrictionElement(Element *element, XSDOperationParameters *params);
    bool updateSimpleContentRestrictionElement(Element *element, XSDOperationParameters *params);
    bool insertComplexContentGenericElement(Regola *regola, Element *element, XSDOperationParameters *params);
    bool updateComplexContentGenericElement(Regola *regola, Element *element, XSDOperationParameters *params);
    bool insertComplexTypeNoneElement(Element *element, XSDOperationParameters *params);
    bool insertComplexTypeSequenceElement(Element *element, XSDOperationParameters *params);
    bool updateComplexTypeSequenceElement(Element *element, XSDOperationParameters *params);
    bool insertComplexTypeChoiceElement(Element *element, XSDOperationParameters *params);
    bool updateComplexTypeChoiceElement(Element *element, XSDOperationParameters *params);
    bool insertComplexTypeAllElement(Element *element, XSDOperationParameters *params);
    bool updateComplexTypeAllElement(Element *element, XSDOperationParameters *params);
    bool insertComplexTypeGroupElement(Element *element, XSDOperationParameters *params);
    bool updateComplexTypeGroupElement(Element *element, XSDOperationParameters *params);
    bool updateComplexTypeNoneElement(Element *element, XSDOperationParameters *params);

    bool doInsert(Regola *regola, Element *targetElement, XSDOperationParameters * params);
    bool doUpdate(Regola *regola, Element *targetElement, XSDOperationParameters * params);

    bool insertSimpleContentRestrictionElement(Regola *regola, Element *element, XSDOperationParameters *params);
    bool insertSimpleContentExtensionElement(Regola *regola, Element *element, XSDOperationParameters *params);
    bool insertComplexTypeSimpleContentExtension(Element *element, XSDOperationParameters *params);
    bool insertComplexTypeSimpleContentRestriction(Element *element, XSDOperationParameters *params);
    void removeRefAttribute(Element *element);
    void removeTypeAttribute(Element *element);
    void setTypeAttribute(Element *element, const QString &typeName);
    void setNameAttribute(Element *element, const QString &name);
    void removeChildren(Regola *regola, Element * newElement, const bool isConserveAnnotations);
    QString elementName(XSDOperationParameters *params);
    QString makeName(XSDOperationParameters *params, const QString &localName);
    void removeChildrenFromElement(Element *element, QList<Element*>childrenToDeleteList);
    int findInChildren(const QString &name, Element *el, const int index, const int maxEl);

    void applyOperation(Element *element, XSDOper *oper, XSDOperationParameters *params);

    void copyInnerContent(XInfoBase *doc, Element *element);


    XSDOperationFactory _factory;
public:
    XSDHelper();
    ~XSDHelper();

    //------

    bool doOperation(const ElementOp::Op op, QTreeWidget *theWidget, Regola *regola, Element *targetElement, QList<int> newPath, XSDOperationParameters *params);

    //--
    Element *findAnnotation(Element *element, XSDOperationParameters *params);
    bool doAnnotation(QTreeWidget * theWidget, Regola *regola, Element * targetElement, Element * currentAnnotation, Element *newAnnotation);
    Element *makeElementOther(XSchemaOther *other, Element *parent);
    Element *makeElementDocumentation(XDocumentation *doc, Element *parent, XSDOperationParameters *params);
    Element *makeElementAppInfo(XAppInfo* appInfo, Element *parent, XSDOperationParameters *params);
};

class XSDAnnotationEditor
{
public:
    XSDAnnotationEditor();
    virtual ~XSDAnnotationEditor();
    virtual bool hasResult() = 0 ;
    virtual void exec(Element *origAnnot, XSDOperationParameters *params) = 0;
    virtual Element *annotation() = 0 ;
};

class XSDAnnotationEditProvider
{
public:
    XSDAnnotationEditProvider();
    virtual ~XSDAnnotationEditProvider();
    virtual XSDAnnotationEditor *newEditor(QWidget *window) = 0 ;
    virtual void autoDelete() = 0;
};


#endif // XSDHELPER_H
