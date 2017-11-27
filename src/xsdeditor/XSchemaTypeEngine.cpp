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

#include "xsdeditor/xschema.h"
#include "xsdeditor/xsddefinitions.h"
#include "xsdeditor/XSchemaIOContants.h"
#include "utils.h"


//------------------------------------------------------------------------


bool XSchemaElement::findBaseObjects(XSchemaInquiryContext &context, QList<XSchemaObject*> &baseElements, QList<XSchemaObject*> & baseAttributes)
{
    bool isOk = true ;
    if(!collectAllBaseTypeElements(context, baseElements, baseAttributes)) {
        isOk = false;
    }
    return isOk ;
}

bool XSchemaGroup::findBaseObjects(XSchemaInquiryContext &context, QList<XSchemaObject*> &baseElements, QList<XSchemaObject*> &/*baseAttributes*/)
{
    bool isOk = true ;
    if(!handleReference(context, baseElements)) {
        isOk = false;
    }
    return isOk ;
}

bool XSchemaAttributeGroup::findBaseObjects(XSchemaInquiryContext &context, QList<XSchemaObject*> & /*baseElements*/, QList<XSchemaObject*> &baseAttributes)
{
    bool isOk = true ;
    if(!handleReference(context, baseAttributes)) {
        isOk = false;
    }
    return isOk ;
}

bool XSchemaAttribute::findBaseObjects(XSchemaInquiryContext &context, QList<XSchemaObject*> &baseChildren, QList<XSchemaObject*> & /*baseAttributes*/)
{
    bool isOk = true ;
    if(!handleReference(context, baseChildren)) {
        isOk = false;
    }
    return isOk ;
}


/**
  if base type (xsd) do not expand.
  if already collected, do not recurse into
 */

bool XSchemaGroup::handleReference(XSchemaInquiryContext &context, QList<XSchemaObject*> &result)
{
    XSchemaGroup *group = NULL ;

    if(!hasAReference()) {
        // No reference.
        return true;
    }

    if(context.isEnrolled(this)) {
        return true ;
    }
    context.enroll(this);

    group = resolveReference(this);
    if(context.isEnrolled(group)) {
        return true ;
    }
    if(NULL != group) {
        XSchemaContainer *container = new XSchemaContainer(NULL, _root);
        container->setFromGroup(group);
        container->collectChildrenOfObject(context, group);
        result.append(container);
    }
    return true ;
}

bool XSchemaAttribute::handleReference(XSchemaInquiryContext &context, QList<XSchemaObject*> &result)
{
    XSchemaAttribute *attribute = NULL ;

    if(hasType()) {
        if(context.isEnrolled(this)) {
            return true ;
        }
        context.enroll(this);
        XSchemaElement *element = _root->schema()->topLevelType(this->xsdType());
        if(NULL != element) {
            if(!element->isPredefined()) {
                XSchemaContainer *container = new XSchemaContainer(NULL, _root);
                container->setFromElement(element);
                container->collectChildrenOfObject(context, element);
                result.append(container);
            }
            return true;
        }
        return false;
    } else if(hasAReference()) {
        attribute = resolveReference();
        if(NULL != attribute) {
            XSchemaContainer *container = new XSchemaContainer(NULL, _root);
            container->setFromAttribute(attribute);
            // Expand the type using another hop and this container as parent.
            container->addExistingChild(attribute);
            //container->collectChildrenOfObject(context, attribute);
            result.append(container);
            return true;
        }
        return false ;
    }
    return true ;
}

bool XSchemaAttributeGroup::handleReference(XSchemaInquiryContext &context, QList<XSchemaObject*> &result)
{
    XSchemaAttributeGroup *group = NULL ;

    if(!hasAReference()) {
        // No reference.
        return true;
    }

    if(context.isEnrolled(this)) {
        return true ;
    }
    context.enroll(this);

    group = resolveReference(this);
    if(context.isEnrolled(group)) {
        return true ;
    }
    if(NULL != group) {
        XSchemaContainer *container = new XSchemaContainer(NULL, _root);
        container->setFromAttributeGroup(group);
        container->collectChildrenOfObject(context, group);
        result.append(container);
    }
    return true ;
}


/**
  if base type (xsd) do not expand.
  if already collected, do not recurse into
 */

bool XSchemaElement::collectAllBaseTypeElements(XSchemaInquiryContext &context, QList<XSchemaObject*> &result, QList<XSchemaObject*> & attributes)
{
    bool ok = false ;
    XSchemaFindReferenceContext typeContext;
    XSchemaElement *elm = NULL ;

    if(context.isEnrolled(this)) {
        return true ;
    }
    context.enroll(this);

    switch(category()) {
    case EES_EMPTY:
        elm = resolveType(typeContext, this);
        if(context.isEnrolled(elm)) {
            return true ;
        }
        break;
    case EES_REFERENCE: {
        elm = resolveReference(typeContext, this);
        if(context.isEnrolled(elm)) {
            return true ;
        }
        if(NULL != elm) {
            if(!elm->xsdType().isEmpty()) {
                elm = resolveType(typeContext, elm);
                if(context.isEnrolled(elm)) {
                    return true ;
                }
            }
        }
    }
    break;
    case EES_SIMPLETYPE_ONLY:
    case EES_SIMPLETYPE_WITHATTRIBUTES:
    case EES_COMPLEX_DEFINITION:
        elm = resolveType(typeContext, this);
        break;
    case EES_COMPLEX_DERIVED: {
        collectElementsOfComplexDerived(context, result);
        if(context.isFullCollection()) {
            collectAttributesOfComplexDerived(context, attributes);
        }
        return true ;
    }
    break;
    default:
        //
        //Utils::error(QString("cat %1").arg(category()));
        break;
    }
    // if no base or reference, nothing to do
    if(typeContext.isPredefinedType() || ((elm == this) && !context.isFullCollection())) {
        ok = true ;
    } else {
        if(NULL != elm) {
            ok = true ;
            context.enroll(elm);
            if(!elm->collectAllElementsOfBaseTypes(context, result)) {
                context.addError(this);
                ok = false ;
            }
            if(context.isFullCollection()) {
                if(!elm->collectAllAttributesOfBaseTypes(context, attributes)) {
                    context.addError(this);
                    ok = false ;
                }
            }
        } else {
            context.addError(this);
            ok = false;
        }
    }
    return ok;
}

bool XSchemaElement::collectAllElementsOfBaseTypes(XSchemaInquiryContext &context, QList<XSchemaObject*> &result)
{
    switch(category()) {
    case EES_EMPTY:
    case EES_SIMPLETYPE_ONLY: {
        XSchemaContainer *container = new XSchemaContainer(NULL, _root);
        container->setFromElement(this);
        container->collectChildrenOfObject(context, this);
        result.append(container);
    }
        // nothing collectElementsOfSimpleDerived(result);
    break;
    case EES_REFERENCE:
        // we should never go here
        return false;
    case EES_SIMPLETYPE_WITHATTRIBUTES:
    case EES_COMPLEX_DEFINITION: {
        XSchemaContainer *container = new XSchemaContainer(NULL, _root);
        container->setFromElement(this);
        container->collectChildrenOfObject(context, this);
        result.append(container);
    }
    break;
    case EES_COMPLEX_DERIVED:
        collectElementsOfComplexDerived(context, result);
        break;
    default:
        return false ;
    }
    return true;
}

bool XSchemaElement::collectAllAttributesOfBaseTypes(XSchemaInquiryContext &context, QList<XSchemaObject*> &result)
{
    switch(category()) {
    case EES_EMPTY:
    case EES_SIMPLETYPE_ONLY:
        break;
    case EES_REFERENCE:
        // we should never go here
        return false;
    case EES_SIMPLETYPE_WITHATTRIBUTES:
        foreach(XSchemaObject* child, _attributes) {
            result.append(child);
        }
        // collect attributes of the extensions
        collectAttributesOfSimpleDerived(context, result);
        break;
    case EES_COMPLEX_DEFINITION: {
        foreach(XSchemaObject* child, _attributes) {
            result.append(child);
        }
    }
    break;
    case EES_COMPLEX_DERIVED:
        collectAttributesOfComplexDerived(context, result);
        break;
    default:
        return false ;
    }
    return true;
}

void XSchemaElement::collectElementsOfComplexDerived(XSchemaInquiryContext &context, QList<XSchemaObject*> &result)
{
    XSchemaElement *baseTypeOrElement ;
    if(!hasAReference()) {
        baseTypeOrElement = this ;
    } else {
        if(isTypeOrElement()) {
            baseTypeOrElement = getReferencedElement();
        } else {
            baseTypeOrElement = getReferencedType();
        }
    }
    XSchemaContainer * container = NULL ;
    XSchemaContainer *directContainer = NULL ;
    XSchemaComplexContentRestriction *restriction = baseTypeOrElement->getRestriction();
    if(NULL != restriction) {
        container = new XSchemaContainer(NULL, _root);
        container->setName(restriction->referencedObjectName());
        XSchemaElement *typeRestricted = restriction->getReferencedType(restriction->referencedObjectName());
        container->setLabel(tr("Restriction: %1").arg(restriction->referencedObjectName()));
        if(!context.isHonorRestrictions()) {
            if(NULL != typeRestricted) {
                container->collectChildrenOfObject(context, typeRestricted);
            }
        } else {
            container->collectChildrenOfObject(context, restriction);
        }
    } else {
        XSchemaComplexContentExtension *extension = baseTypeOrElement->getExtension();
        if(NULL != extension) {
            XSchemaElement *extType = extension->getBaseType();
            if((NULL != extType) && !extType->isPredefined()) {
                container = new XSchemaContainer(NULL, _root);
                container->setName(extType->name());
                container->setLabel(tr("Extension: %1").arg(extType->name()));
                if(NULL != extType) {
                    container->collectChildrenOfObject(context, extType);
                }
            }
            if(context.isFullCollection()) {
                // append standard children
                directContainer = new XSchemaContainer(NULL, _root);
                directContainer->setFromElement(this);
                directContainer->collectChildrenOfObject(context, extension);
            }
        }
    }
    if(NULL != container) {
        result.append(container);
    }
    if(NULL != directContainer) {
        result.append(directContainer);
    }
}

void XSchemaElement::collectAttributesOfComplexDerived(XSchemaInquiryContext & /*context*/, QList<XSchemaObject*> &result)
{
    XSchemaElement *baseTypeOrElement ;
    if(!hasAReference()) {
        baseTypeOrElement = this ;
    } else {
        if(isTypeOrElement()) {
            baseTypeOrElement = getReferencedElement();
        } else {
            baseTypeOrElement = getReferencedType();
        }
    }
    XSchemaComplexContentRestriction *restriction = baseTypeOrElement->getRestriction();
    if(NULL != restriction) {
        foreach(XSchemaObject* child, restriction->getChildren()) {
            switch(child->getType()) {
            case SchemaTypeAttribute:
            case SchemaTypeAttributeGroup:
                result.append(child);
                break;
            default:
                break;
            }
        }
    } else {
        foreach(XSchemaObject* child, baseTypeOrElement->_attributes) {
            result.append(child);
        }
        XSchemaComplexContentExtension *extension = baseTypeOrElement->getExtension();
        if(NULL != extension) {
            XSchemaElement *extType = extension->getBaseType();
            if((NULL != extType) && !extType->isPredefined()) {
                foreach(XSchemaObject* child, extType->_attributes) {
                    result.append(child);
                }
            }
            foreach(XSchemaObject* child, extension->getChildren()) {
                const ESchemaType type = child->getType();
                if((SchemaTypeAttribute == type) || (SchemaTypeAttributeGroup == type)) {
                    result.append(child);
                }
            }
        }
    }
}

void XSchemaElement::collectAttributesOfSimpleDerived(XSchemaInquiryContext & /*context*/, QList<XSchemaObject*> &result)
{
    XSchemaElement *baseTypeOrElement ;
    if(!hasAReference()) {
        baseTypeOrElement = this ;
    } else {
        if(isTypeOrElement()) {
            baseTypeOrElement = getReferencedElement();
        } else {
            baseTypeOrElement = getReferencedType();
        }
    }
    foreach(XSchemaObject* child, baseTypeOrElement->_attributes) {
        result.append(child);
    }
    XSchemaSimpleContentExtension *extension = baseTypeOrElement->getSimpleContentExtension();
    if(NULL != extension) {
        foreach(XSchemaObject* child, extension->getChildren()) {
            const ESchemaType type = child->getType();
            if((SchemaTypeAttribute == type) || (SchemaTypeAttributeGroup == type)) {
                result.append(child);
            }
        }
    }
}

void XSchemaContainer::collectChildrenOfObject(XSchemaInquiryContext &context, XSchemaObject* target)
{
    foreach(XSchemaObject * child, target->getChildren()) {
        if(child->getType() == SchemaTypeElement) {
            XSchemaElement *element = static_cast<XSchemaElement*>(child);
            QList<XSchemaObject*> baseAttributes;
            QList<XSchemaObject*> baseElements;
            bool isOk = element->findBaseObjects(context, baseElements, baseAttributes);
            if(!isOk) {
                Utils::error(tr("Error collecting information on base types."));
            } else {
                foreach(XSchemaObject * child, baseElements) {
                    _existingChildren.append(child);
                }
                foreach(XSchemaObject * child, baseAttributes) {
                    _existingChildren.append(child);
                }
            }
        } else {
            _existingChildren.append(child);
        }
    }
    if(target->getType() == SchemaTypeElement) {
        XSchemaElement *element = static_cast<XSchemaElement*>(target);
        foreach(XSchemaObject * attribute, element->attributes()) {
            _existingChildren.append(attribute);
        }
    }
}

void XSchemaContainer::addExistingChild(XSchemaObject *newChild)
{
    _existingChildren.append(newChild);
}
