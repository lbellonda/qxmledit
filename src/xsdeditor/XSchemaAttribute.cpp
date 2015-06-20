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

#include "xsdeditor/xschema.h"
#include "xsdeditor/XSchemaIOContants.h"
#include "utils.h"

IMPL_QPROPS(XSchemaAttribute, xsdType, setXsdType, PROPERTY_ELEMENT_TYPE)
IMPL_QPROPS(XSchemaAttribute, defaultValue, setDefaultValue, PROPERTY_ELEMENT_TYPE)
IMPL_QPROP(XSchemaAttribute::EUse, XSchemaAttribute, use, setUse, PROPERTY_ATTRIBUTE_USE)

XSchemaAttribute::XSchemaAttribute(XSchemaObject *newParent, XSchemaRoot *newRoot)
    : XSchemaObject(newParent, newRoot)
{
    INIT_XSD_ATTRD(_defaultValue);
    INIT_XSD_ATTRD(_fixed);
    INIT_XSD_ATTR(_form, EQ_MISSING) ;
    INIT_XSD_ATTRD(_ref);
    INIT_XSD_ATTRD(_xsdType);
    INIT_XSD_ATTR(_use, Missing);
}

XSchemaAttribute::~XSchemaAttribute()
{
    reset();
}

void XSchemaAttribute::reset()
{
    XSchemaObject::reset();
}

XReferenceType XSchemaAttribute::referencedObjectType()
{
    if(_ref.isEmpty()) {
        return XRT_NONE;
    } else {
        return XRT_ATTRIBUTE;
    }
}

bool XSchemaAttribute::hasType()
{
    return !_xsdType.isEmpty();
}

QString XSchemaAttribute::referencedObjectName()
{
    return _ref ;
}

XSchemaSimpleTypeRestriction *XSchemaAttribute::getSimpleTypeRestriction()
{
    foreach(XSchemaObject * child, _children) {
        if(child->getType() == SchemaTypeElement) {
            XSchemaElement *element = static_cast<XSchemaElement *>(child);
            return element->getSimpleTypeRestriction();
        }
    }
    return NULL ;
}

QString XSchemaAttribute::description()
{
    QString description ;
    if(!_name.isEmpty()) {
        description.append(_name);
    } else if(! _ref.isEmpty()) {
        description.append(tr("ref to "));
        description.append(_ref);
    }
    if(!_xsdType.isEmpty()) {
        description.append(", type: ");
        description.append(_xsdType);
    }
    if(!_fixed.isEmpty()) {
        description.append(tr(", fixed:"));
        description.append(_fixed);
    }
    if(!_defaultValue.isEmpty()) {
        description.append("\n");
        description.append(tr("default:"));
        description.append(_defaultValue);
    }
    if(_use != Missing) {
        description.append(" (");
        description.append(toStringUse(_use));
        description.append(")");
    }
    return description;
}

QString XSchemaAttribute::toStringUse(const EUse theUse)
{
    switch(theUse) {
    default:
        return "" ;
    case Optional:
        return "optional";
    case Prohibited:
        return "prohibited";
    case Required:
        return "required";
    }
}

QString XSchemaAttribute::toStringForSave(const EUse theUse)
{
    switch(theUse) {
    default:
        // note the empty tag: it is essential
        return IO_ATTRIBUTE_USE_MISSING ;
    case Optional:
        return IO_ATTRIBUTE_USE_OPTIONAL;
    case Prohibited:
        return IO_ATTRIBUTE_USE_PROHIBITED;
    case Required:
        return IO_ATTRIBUTE_USE_REQUIRED;
    }
}

bool XSchemaAttribute::hasAReference()
{
    return !_ref.isEmpty();
}


XSchemaAttribute* XSchemaAttribute::resolveReference()
{
    return _root->schema()->topLevelAttribute(_ref);
}


// ---------- save -----------------------------

bool XSchemaAttribute::generateDom(QDomDocument &document, QDomNode &parent)
{
    QDomElement node = createElement(document, IO_XSD_ATTRIBUTE);

    //TODO node.setAttribute( IO_ATTRIBUTE_ATTR_FORM, (EQ_UNQUALIFIED==_attributeFormDefault)? IO_SCHEMA_UNQUALIFIED:IO_SCHEMA_QUALIFIED);
    addAttrNotEmpty(node, IO_GENERIC_NAME, _name);
    addAttrNotEmpty(node, IO_GENERIC_ID, _id);
    addAttrNotEmpty(node, IO_ATTRIBUTE_ATTR_FIXED, _fixed);
    addAttrNotEmpty(node, IO_ATTRIBUTE_ATTR_DEFAULT, _defaultValue);
    addAttrNotEmpty(node, IO_ATTRIBUTE_ATTR_REF, _ref);
    addAttrNotEmpty(node, IO_ATTRIBUTE_ATTR_TYPE, _xsdType);
    addAttrNotEmpty(node, IO_ATTRIBUTE_ATTR_USE, toStringForSave(_use));
    addOtherAttributesToDom(node);
    if(NULL != _annotation) {
        _annotation->generateDom(document, node);
    }

    if(!generateInnerDom(document, node)) {
        return false;
    }
    parent.appendChild(node);
    return true;
}

// ---------- load -----------------------------

void XSchemaAttribute::scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void * /*context*/)
{
    QString name = attribute.nodeName();

    if(name == IO_GENERIC_ID) {
        _id = attribute.value() ;
    } else if(name == IO_GENERIC_NAME) {
        SET_XSD_ATTR(_name, attribute.value()) ;
    } else if(name == IO_ATTRIBUTE_ATTR_DEFAULT) {
        SET_XSD_ATTR(_defaultValue, attribute.value()) ;
    } else if(name == IO_ATTRIBUTE_ATTR_FIXED) {
        SET_XSD_ATTR(_fixed, attribute.value());
    } else if(name == IO_ATTRIBUTE_ATTR_FORM) {
        SET_XSD_ATTR(_form, decodeForm(attribute.value()));
    } else if(name == IO_ATTRIBUTE_ATTR_REF) {
        SET_XSD_ATTR(_ref, attribute.value()) ;
    } else if(name == IO_ATTRIBUTE_ATTR_TYPE) {
        SET_XSD_ATTR(_xsdType, attribute.value()) ;
    } else if(name == IO_ATTRIBUTE_ATTR_USE) {
        if(IO_ATTRIBUTE_USE_OPTIONAL == attribute.value()) {
            SET_XSD_ATTR(_use, Optional) ;
        } else if(IO_ATTRIBUTE_USE_PROHIBITED == attribute.value()) {
            SET_XSD_ATTR(_use, Prohibited) ;
        } else if(IO_ATTRIBUTE_USE_REQUIRED == attribute.value()) {
            SET_XSD_ATTR(_use, Required);
        } else {
            raiseError(loadContext, this, attribute, false);
        }
    } else {
        if(!readOtherAttributes(attribute)) {
            raiseError(loadContext, this, attribute, false);
        }
    }
}

void XSchemaAttribute::scanForElements(XSDLoadContext *loadContext, QDomElement &element, void *context)
{
    XScanContext *theContext = (XScanContext*) context;
    QString name = element.localName();
    if(element.namespaceURI() == _root->namespaceURI()) {
        if(name == IO_XSD_ANNOTATION) {
            readHandleAnnotation(loadContext, element);
        } else if(name == IO_XSD_SIMPLETYPE) {
            if(theContext->childFound) {
                raiseError(loadContext, this, element, true);
            }
            theContext->childFound = true;
            //readHandleSimpleType(element);
            readHandleObject(loadContext, element, new XSchemaElement(this, _root));
        } else {
            raiseError(loadContext, this, element, true);
        }
    } else {
        raiseError(loadContext, this, element, true);
    }
}


bool XSchemaAttribute::readHandleSimpleType(XSDLoadContext *loadContext, QDomElement &element)
{
    bool isOk = true ;
    QString namespaceURI = _root->namespaceURI();
    int nodi = element.childNodes().count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = element.childNodes().item(i) ;

        bool childFound = false;
        if(childNode.isElement()) {
            QDomElement element = childNode.toElement();
            QString localName = element.localName();
            if(element.namespaceURI() == namespaceURI) {
                if(localName == IO_XSD_ANNOTATION) {
                } else if(localName == IO_XSD_RESTRICTION) {
                    if(childFound) {
                        raiseError(loadContext, this, childNode, true);
                    }
                    childFound = true;
                    readHandleObject(loadContext, element, new XSchemaSimpleTypeRestriction(this, _root));
                } else if(localName == IO_XSD_LIST) {
                    if(childFound) {
                        raiseError(loadContext, this, childNode, true);
                    }
                    childFound = true;
                    readHandleObject(loadContext, element, new XSchemaSimpleTypeList(this, _root));
                } else if(localName == IO_XSD_UNION) {
                    if(childFound) {
                        raiseError(loadContext, this, childNode, true);
                    }
                    childFound = true;
                    readHandleObject(loadContext, element, new XSchemaSimpleTypeUnion(this, _root));
                } else {
                    raiseError(loadContext, this, childNode, true);
                }
            }
        }
    }
    return isOk;
}

/*
  Refer to paragraph "3.2.3 Constraints on XML Representations of Attribute Declarations" of XML Schema Part 1: Structures Second Edition
*/

void XSchemaAttribute::validateAfterRead(XSDLoadContext *loadContext, QDomElement &node, void * /*context*/)
{
    //1 default and fixed must not both be present.
    if(_defaultValue_used && _fixed_used) {
        raiseError(loadContext, XSD_LOAD_ERROR_ATTRIBUTE_FIXED_AND_USE, this, node, tr("'fixed' and 'use' must not both be present."));
    }
    // 2 If default and use are both present, use must have the ·actual value· optional.
    if(_defaultValue_used && _use_used && (_use != Optional)) {
        raiseError(loadContext, XSD_LOAD_ERROR_ATTRIBUTE_DEFAULT_AND_USE, this, node, tr("If default and use are both present, use must have the actual value optional."));
    }
    //3 If the item's parent is not <schema>, then all of the following must be true:
    if(!isTopLevel()) {
        // 3.1 One of ref or name must be present, but not both.
        if((!_ref_used && !_name_used) || (_ref_used && _name_used)) {
            raiseError(loadContext, XSD_LOAD_ERROR_ATTRIBUTE_NAME_AND_REF, this, node, tr("One of ref or name must be present, but not both."));
        }
        //3.2 If ref is present, then all of <simpleType>, form and type must be absent.
        if(_ref_used) {
            if(_form_used || _xsdType_used || hasSimpleType()) {
                raiseError(loadContext, XSD_LOAD_ERROR_ATTRIBUTE_REF_AND_TYPE, this, node, tr("If ref is present, then all of <simpleType>, form and type must be absent."));
            }
        }
    } else {
        // regola6: top level
        //6. no ref if top -> name
        if(_ref_used) {
            raiseError(loadContext, XSD_LOAD_ERROR_ATTRIBUTE_REF_GLOBAL, this, node, tr("Global attribute ref must be absent."));
        }
        if(!_name_used) {
            raiseError(loadContext, XSD_LOAD_ERROR_ATTRIBUTE_NAME_GLOBAL, this, node, tr("Global attribute name must be present."));
        }
    }
    //4 type and <simpleType> must not both be present.
    if(_xsdType_used && hasSimpleType()) {
        raiseError(loadContext, XSD_LOAD_ERROR_ATTRIBUTE_TYPE_AND_SIMPLETYPE, this, node, tr("type and <simpleType> must not both be present.."));
    }
}
