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

#include "xschemaoutlineelement.h"
#include "xsdeditor/xsddefinitions.h"
#include "xsdeditor/XSchemaIOContants.h"
#include "utils.h"

XSchemaOutlineElement::XSchemaOutlineElement(XSchemaObject *newParent, XSchemaRoot *newRoot) : XSchemaOutlineContainer(newParent, newRoot)
{
    _element = NULL ;
    _realElement = NULL ;
    _isAlreadyProcessed = false ;
}

XSchemaOutlineElement::~XSchemaOutlineElement()
{
    reset();
}


void XSchemaOutlineElement::reset()
{
    XSchemaObject::reset();
}

XSchemaElement *XSchemaOutlineElement::getElement() const
{
    return _element;
}

void XSchemaOutlineElement::setElement(XSchemaInquiryContext &context, XSchemaElement *value)
{
    const bool calculate = (_element != value);
    _element = value;
    _realElement = NULL ;
    if(calculate) {
        calculateElement(context);
    }
}

QString XSchemaOutlineElement::tagName()
{
    return "XSchemaOutlineElement";
}

void XSchemaOutlineElement::calculateElement(XSchemaInquiryContext &context)
{
    reset();
    if(NULL == _element) {
        return ;
    }
    _realElement = NULL ;
    if(_element->isTypeOrElement()) {
        _name = _element->name();
        _realElement = _element ;
    } else {
        if(XRT_ELEMENT == _element->referencedObjectType()) {
            XSchemaElement *referencedElement = _element->getReferencedElement();
            if(NULL == referencedElement) {
                // no way, unable to reference something.
                return ;
            }
            _name = referencedElement->name();
            _realElement = referencedElement ;
        } else {
            _name = _element->name();
            const QString type = _element->typeString();
            if(!type.isEmpty()) {
                _realElement = _element->resolveType(_element);
            }
            if(NULL == _realElement) {
                _realElement = _element ;
            }
        }
    }

    //check if a parent is enrolled
    /*debug code if(NULL == _realElement) {
        printf("Real elm is null\n");
    } else {
        printf("EL: %s\n", _realElement->name().toLatin1().data());
    }*/
    if(isEnrolled()) {
        _isAlreadyProcessed = true;
        return ;
    }
    QList<XSchemaObject*> baseElements;
    bool isEnrolledSub = false;
    context.setEnrollFlag(&isEnrolledSub);
    bool isOk = _element->findBaseObjects(context, baseElements, _attributes);
    if(isOk) {
        if(isEnrolledSub) {
            _isAlreadyProcessed = true;
        }
        collectOutlineChildrenOfObject(context, baseElements);
    }
}

bool XSchemaOutlineElement::isEnrolled()
{
    XSchemaObject *theParent = xsdParent();
    while(theParent != NULL) {
        if(theParent->getType() == SchemaTypeOutlineElement) {
            XSchemaOutlineElement* outlineElement = static_cast<XSchemaOutlineElement*>(theParent);
            if(outlineElement->_realElement == _realElement) {
                return true;
            }
        }
        theParent = theParent->xsdParent();
    }
    return false;
}

bool XSchemaOutlineElement::hasAttributes()
{
    return !_attributes.isEmpty();
}

QString XSchemaOutlineElement::occurrencesDescr()
{
    if(NULL != _element) {
        return _element->occurrencesDescr();
    }
    return "";
}

int XSchemaOutlineElement::minOccurrences()
{
    if(NULL != _element) {
        return _element->minOccurrences();
    }
    return 1;
}

int XSchemaOutlineElement::maxOccurrences()
{
    if(NULL != _element) {
        return _element->maxOccurrences();
    }
    return 1;
}


/*
Remember:
element: (simpleType|complexType)?
complexType:
    simpleContent|complexContent|((group|all|choice|sequence)
complex:
    - group (all, choice, seq)
    - all (elements)
    - seq (elm, group, choice, seq, any)
    - choice (==seq)
extension: group|all|choice|sequence
  restriction:(group|all|choice|sequence)

*/

void XSchemaOutlineContainer::collectOutlineChildrenOfObject(XSchemaInquiryContext &context, QList<XSchemaObject*> &baseElements)
{
    foreach(XSchemaObject * child, baseElements) {
        switch(child->getType()) {
        default:
            //Debug message(">>>>found type:" + child->typeString());
            break;
        // take children
        case SchemaContainer:
            collectOutlineChildrenOfObject(context, static_cast<XSchemaContainer*>(child)->existingChildren());
            delete child;
            // remove the (unused)container object
            break;

        case SchemaTypeElement: {
            XSchemaElement *element = static_cast<XSchemaElement*>(child);
            XSchemaOutlineElement *outlineElement = new XSchemaOutlineElement(this, _root);
            addChild(outlineElement);
            // Delve deep into children.
            outlineElement->setSchemaObject(child);
            if(context.isRecursive()) {
                outlineElement->setElement(context, element);
            }
        }
        break;
        case SchemaTypeGroup:
            collectOutlineContainerChildrenGroupRef(context, static_cast<XSchemaGroup*>(child));
            break;
        case SchemaTypeAll:
            collectOutlineContainerChildren(context, new XSchemaOutlineAll(this, _root), child);
            break;
        case SchemaTypeSequence:
            collectOutlineContainerChildren(context, new XSchemaOutlineSequence(this, _root), child);
            break;
        case SchemaTypeChoice:
            collectOutlineContainerChildren(context, new XSchemaOutlineChoice(this, _root), child);
            break;
        case SchemaTypeAny:
            collectOutlineContainerChildren(context, new XSchemaOutlineAny(this, _root), child);
            break;
        }
    }
}

void XSchemaOutlineContainer::collectOutlineContainerChildren(XSchemaInquiryContext &context, XSchemaOutlineContainer *container, XSchemaObject *theSchemaObject)
{
    addChild(container);
    // Delve deep into children.
    container->setSchemaObject(theSchemaObject);
    container->collectOutlineChildrenOfObject(context, theSchemaObject->getChildren());
}

void XSchemaOutlineContainer::collectOutlineContainerChildrenGroupRef(XSchemaInquiryContext &context, XSchemaGroup *theSchemaGroup)
{
    XSchemaOutlineGroup *group = new XSchemaOutlineGroup(this, _root);
    addChild(group);
    XSchemaGroup *refGroup = NULL ;
    const QString &groupName = theSchemaGroup->referencedObjectName();
    if(!groupName.isEmpty()) {
        refGroup = _root->schema()->topLevelGroup(groupName);
    }
    if(NULL == refGroup) {
        return ;
    }
    // Delve deep into children.
    group->setName(refGroup->name());
    group->setSchemaObject(refGroup);
    /*if(context.isGroupEnrolled(refGroup)) {
        _al = tru el
        return ;
    }*/
    if(context.isEnrolled(refGroup)) {
        group->setAlreadyProcessed(true);
        return ;
    }
    group->collectOutlineChildrenOfObject(context, refGroup->getChildren());
    //context.enrollGroup(refGroup, group->childrenCount());
    context.enroll(refGroup);
}

void XSchemaOutlineGroup::collectOutlineContainerChildrenGroup(XSchemaInquiryContext &context)
{
    if(NULL == _group) {
        return ;
    }
    // Delve deep into children.
    if(context.isEnrolled(_group)) {
        setAlreadyProcessed(true);
        return ;
    }
    collectOutlineChildrenOfObject(context, _group->getChildren());
    context.enroll(_group);
}

bool XSchemaOutlineGroup::isAlreadyProcessed() const
{
    return _isAlreadyProcessed;
}

void XSchemaOutlineGroup::setAlreadyProcessed(bool value)
{
    _isAlreadyProcessed = value;
}

QString XSchemaOutlineGroup::name()
{
    if(NULL != _group) {
        return _group->name();
    }
    return "" ;
}

