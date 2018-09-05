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

#include "xsdeditor/xschema.h"
#include "xsdeditor/XSchemaIOContants.h"

//class XSchemaSimpleContent -----------------------------------------------------------------------------------------


XSchemaSimpleContent::XSchemaSimpleContent(XSchemaObject *newParent, XSchemaRoot *newRoot) :  XSchemaObject(newParent, newRoot), _mainAnnotation(NULL)
{
}

XSchemaSimpleContent::~XSchemaSimpleContent()
{
}

void XSchemaSimpleContent::reset()
{
    if(_mainAnnotation) {
        delete _mainAnnotation ;
        _mainAnnotation = NULL ;
    }
    XSchemaObject::reset();
}

bool XSchemaSimpleContent::generateDom(QDomDocument & /*document*/, QDomNode & /*parent*/)
{
    XsdError("TODO") ;
    return false;
}
/*****************************************************
XSchemaSimpleContent *XSchemaSimpleContent::readChild(XSchemaObject *parent, QDomElement &element)
{
    XSchemaSimpleContent *newObject = NULL ;
    int nodi = element.childNodes().count();
    for (int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = element.childNodes().item(i) ;
        if (childNode.isElement()) {
            QDomElement childElement = childNode.toElement();
            QString localName = childElement.localName();
            if( childElement.prefix() == parent->namespacePrefix ) {
                if( localName == IO_XSD_SIMPLECONTENT_RESTRICTION ) {
                    newObject = new XSchemaSimpleContentRestriction( parent, parent->_root );
                    newObject->readBaseProperties( element );
                    return newObject ;
                }
                if( localName == IO_XSD_SIMPLECONTENT_EXTENSION ) {
                    newObject = new XSchemaSimpleContentExtension( parent, parent->_root );
                    newObject->readBaseProperties( element );
                    return newObject ;
                }
            }
        }
    }
    raiseError( parent, childNode, true );
}
************************************/
/*
bool XSchemaSimpleContent::readOtherAttributes(QDomAttr & attribute)
{
    if(_root->existsNamespace(attribute.prefix())) {
        _otherTypeAttributes.insert(attribute.name(), attribute.value());   //TODO check namespace
        return true;
    }
    return false;
}
*/
// Reads base properties
bool XSchemaSimpleContent::readBaseProperties(XSDLoadContext *loadContext, QDomElement &element)
{
    bool isOk = true ;
    QDomNamedNodeMap attributes = element.attributes();
    int numAttrs = attributes.length();
    for(int i = 0 ; i < numAttrs ; i++) {
        QDomNode node = attributes.item(i);
        QDomAttr attr = node.toAttr();
        QString name = node.nodeName();

        if((name == IO_GENERIC_ID)) {
            _idContainer = attr.value() ;
        } else {
            if(!readOtherAttributes(attr)) {
                raiseError(loadContext, this, attr, false);
            }
        }
    }

    QString namespaceURI = _root->namespaceURI();
    int nodi = element.childNodes().count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = element.childNodes().item(i) ;

        if(childNode.isElement()) {
            QDomElement childElement = childNode.toElement();
            QString localName = childElement.localName();
            if(childElement.namespaceURI() == namespaceURI) {
                if(localName == IO_XSD_ANNOTATION) {
                    if(NULL != _mainAnnotation) {
                        raiseError(loadContext, this, childNode, true);
                    }
                    _mainAnnotation = new XSchemaAnnotation(this, _root) ;
                    if(NULL == _mainAnnotation) {
                        raiseErrorForObject(loadContext, element);
                    }
                    _mainAnnotation->loadFromDom(loadContext, childElement);
                } else if(localName == IO_XSD_SIMPLECONTENT_RESTRICTION) {
                    loadFromDom(loadContext, childElement);
                } else if(localName == IO_XSD_SIMPLECONTENT_EXTENSION) {
                    loadFromDom(loadContext, childElement);
                } else {
                    raiseError(loadContext, this, childNode, true);
                }
            } else {
                raiseError(loadContext, this, childNode, true);
            }
        }
    }
    return isOk;
}

QDomElement XSchemaSimpleContent::writeBaseProperties(QDomElement &element)
{
    QDomDocument document = element.ownerDocument();
    QDomElement node = createElement(document, QString(IO_XSD_SIMPLECONTENT));
    addAttrNotEmpty(node, IO_GENERIC_ID, _idContainer);
    addOtherAttributesToDom(node);
    if(NULL != _mainAnnotation) {
        _mainAnnotation->generateDom(document, node);
    }
    element.appendChild(node);
    return node;
}


void XSchemaSimpleContent::collectAttributes(XSchemaAttributesCollection & attributesCollection)
{
    foreach(XSchemaObject * child, getChildren()) {
        if(child->getType() == SchemaTypeAttribute) {
            XSchemaAttribute *aChild = static_cast<XSchemaAttribute*>(child);
            addAttributeToCollection(aChild->name(), attributesCollection, aChild);
        } else if(child->getType() == SchemaTypeAttributeGroup) {
            XSchemaAttributeGroup *aChild = static_cast<XSchemaAttributeGroup*>(child);
            addAttributeGroupToCollection(attributesCollection, aChild);
        }
    }
}


XSchemaElement *XSchemaSimpleContent::getBaseType()
{
    if(_base.isEmpty()) {
        return NULL ;
    }
    return getReferencedType(_base);
}

//class XSchemaSimpleContentRestriction -----------------------------------------------------------------------------------------

XSchemaSimpleContentRestriction::XSchemaSimpleContentRestriction(XSchemaObject *newParent, XSchemaRoot *newRoot) : XSchemaSimpleContent(newParent, newRoot)
{
    anyAttribute = false;
}

XSchemaSimpleContentRestriction::~XSchemaSimpleContentRestriction()
{
    reset();
}

void XSchemaSimpleContentRestriction::reset()
{
    XSchemaSimpleContent::reset();
}

void XSchemaSimpleContentRestriction::addEnumsToListIfAny(QStringList &enums)
{
    _facets.addEnumsToList(enums);
}

void XSchemaSimpleContentRestriction::addOtherFacets(QList<InfoFacet *> &list)
{
    _facets.addOtherFacets(list);
}

bool XSchemaSimpleContentRestriction::generateDom(QDomDocument &document, QDomNode &parent)
{
    QDomElement element = parent.toElement();
    QDomElement parentElement = writeBaseProperties(element);
    QDomElement restriction = createElement(document, IO_XSD_SIMPLECONTENT_RESTRICTION);
    addAttrNotEmpty(restriction, IO_GENERIC_ID, _id);
    addAttrNotEmpty(restriction, IO_GENERIC_BASE, _base);

    addOtherAttributesToDom(restriction);
    if(NULL != _annotation) {
        _annotation->generateDom(document, restriction);
    }
    // if there is a simple type.. TODO
    generateFacets(restriction, _facets);
    //attributes or attributegroup TODO
    //any attribute TODO
    parentElement.appendChild(restriction);
    return true;
}

void XSchemaSimpleContentRestriction::scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void * /*context*/)
{
    QString name = attribute.nodeName();

    if(name == IO_GENERIC_ID) {
        _id = attribute.value() ;
    } else if(name == IO_GENERIC_BASE) {
        _base = attribute.value() ;
    } else {
        if(!readOtherAttributes(attribute)) {
            raiseError(loadContext, this, attribute, false);
        }
    }
}

void XSchemaSimpleContentRestriction::scanForElements(XSDLoadContext *loadContext, QDomElement &element, void * context)
{
    XScanContext* theContext = (XScanContext*) context ;
    bool isRaiseError = false;
    QString name = element.localName();
    QString value = element.attribute(IO_FACET_ATTR_VALUE, "");
    if(element.namespaceURI() == _root->namespaceURI()) {
        if(name == IO_XSD_ANNOTATION) {
            readHandleAnnotation(loadContext, element);
            return;
        } else if(name == IO_XSD_SIMPLETYPE) {
            if(theContext->childFound) {
                raiseError(loadContext, this, element, true);
            }
            theContext->childFound = true;
            readHandleObject(loadContext, element, new XSchemaElement(this, _root));
            // } else if( scanForFacets( name, value, _facets ) ) { TODO
        } else if(name == IO_XSD_MINEXCLUSIVE) {
            _facets._minExclusive = value;
        } else if(name == IO_XSD_MININCLUSIVE) {
            _facets._minInclusive = value;
        } else if(name == IO_XSD_MAXEXCLUSIVE) {
            _facets._maxExclusive = value;
        } else if(name == IO_XSD_MAXINCLUSIVE) {
            _facets._maxInclusive = value;
        } else if(name == IO_XSD_TOTALDIGITS) {
            _facets._totalDigits = value;
        } else if(name == IO_XSD_FRACTIONDIGITS) {
            _facets._fractionDigits = value;
        } else if(name == IO_XSD_LENGTH) {
            _facets._length = value;
        } else if(name == IO_XSD_MINLENGTH) {
            _facets._minLength = value;
        } else if(name == IO_XSD_MAXLENGTH) {
            _facets._maxLength = value;
        } else if(name == IO_XSD_ENUMERATION) {   // TODO: WRONG!!!!
            _facets._enumeration.append(value);
        } else if(name == IO_XSD_WHITESPACE) {  // TODO: WRONG!!!!
            _facets._whiteSpace = value;
        } else if(name == IO_XSD_PATTERN) {
            _facets._pattern = value;
        } else if(name == IO_XSD_ATTRIBUTE) {
            readHandleObject(loadContext, element, new XSchemaAttribute(this, _root));
        } else if(name == IO_XSD_ATTRIBUTEGROUP) {
            readHandleObject(loadContext, element, new XSchemaAttributeGroup(this, _root));
        } else if(name == IO_XSD_ANYATTRIBUTE) {
            if(anyAttribute) {
                raiseError(loadContext, this, element, true);
            }
            readHandleObject(loadContext, element, new XSchemaAnyAttribute(this, _root));
            anyAttribute = true ;
        } else {
            isRaiseError = true;
        }
    } else {
        isRaiseError = true;
    }
    if(isRaiseError) {
        raiseError(loadContext, this, element, true);
    }
}


// TODO: validate structure

//class XSchemaSimpleContentExtension -----------------------------------------------------------------------------------------

XSchemaSimpleContentExtension::XSchemaSimpleContentExtension(XSchemaObject *newParent, XSchemaRoot *newRoot): XSchemaSimpleContent(newParent, newRoot)
{
    anyAttribute = false;
}

XSchemaSimpleContentExtension::~XSchemaSimpleContentExtension()
{
    reset();
}

void XSchemaSimpleContentExtension::reset()
{
    XSchemaSimpleContent::reset();
}

bool XSchemaSimpleContentExtension::generateDom(QDomDocument &document, QDomNode &parent)
{
    QDomElement element = parent.toElement();
    QDomElement parentElement = writeBaseProperties(element);
    QDomElement restriction = createElement(document, IO_XSD_SIMPLECONTENT_EXTENSION);
    addAttrNotEmpty(restriction, IO_GENERIC_ID, _id);
    addAttrNotEmpty(restriction, IO_GENERIC_BASE, _base);

    addOtherAttributesToDom(restriction);
    if(NULL != _annotation) {
        _annotation->generateDom(document, restriction);
    }
    //attributes or attributegroup TODO
    //any attribute TODO
    parentElement.appendChild(restriction);
    return true;

}

void XSchemaSimpleContentExtension::scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void * /*context*/)
{
    QString name = attribute.nodeName();

    if(name == IO_GENERIC_ID) {
        _id = attribute.value() ;
    } else if(name == IO_GENERIC_BASE) {
        _base = attribute.value() ;
    } else {
        if(!readOtherAttributes(attribute)) {
            raiseError(loadContext, this, attribute, false);
        }
    }
}

void XSchemaSimpleContentExtension::scanForElements(XSDLoadContext *loadContext, QDomElement &element, void * /*context*/)
{
    bool isRaiseError = true;
    QString name = element.localName();
    if(element.namespaceURI() == _root->namespaceURI()) {
        if(name == IO_XSD_ANNOTATION) {
            readHandleAnnotation(loadContext, element);
            return;
        } else if(name == IO_XSD_ATTRIBUTE) {
            readHandleObject(loadContext, element, new XSchemaAttribute(this, _root));
            isRaiseError = false ;
        } else if(name == IO_XSD_ATTRIBUTEGROUP) {
            readHandleObject(loadContext, element, new XSchemaAttributeGroup(this, _root));
            isRaiseError = false ;
        } else if(name == IO_XSD_ANYATTRIBUTE) {
            if(anyAttribute) {
                raiseError(loadContext, this, element, true);
            }
            readHandleObject(loadContext, element, new XSchemaAnyAttribute(this, _root));
            anyAttribute = true ;
            isRaiseError = false ;
        }
    }
    if(isRaiseError) {
        raiseError(loadContext, this, element, true);
    }
}

QString XSchemaSimpleContentExtension::description()
{
    QString description = QString("Extension of '%1'").arg(_base) ;
    return description ;
}

//class XSchemaComplexContent -----------------------------------------------------------------------------------------

XSchemaComplexContent::XSchemaComplexContent(XSchemaObject *newParent, XSchemaRoot *newRoot) : XSchemaObject(newParent, newRoot)
{
    _mixed = XEnums::XBOOL_UNSET ;
    anyAttribute = false;
    _mainAnnotation = NULL ;
}

XSchemaComplexContent::~XSchemaComplexContent()
{
    reset();
}

void XSchemaComplexContent::reset()
{
    XSchemaObject::reset();
}

bool XSchemaComplexContent::generateDom(QDomDocument &/*document*/, QDomNode &/*parent*/)
{
    XsdError("TODO");
    return false;
}

// Reads base properties
bool XSchemaComplexContent::readBaseProperties(XSDLoadContext *loadContext, QDomElement &element)
{
    bool isOk = true ;
    QDomNamedNodeMap attributes = element.attributes();
    int numAttrs = attributes.length();
    for(int i = 0 ; i < numAttrs ; i++) {
        QDomNode node = attributes.item(i);
        QDomAttr attr = node.toAttr();
        QString name = node.nodeName();

        if((name == IO_GENERIC_ID)) {
            _idContainer = attr.value() ;
        } else if((name == IO_GENERIC_MIXED)) {
            _mixed = XEnums::parseBool(attr.value());
        } else {
            if(!readOtherAttributes(attr)) {
                raiseError(loadContext, this, attr, false);
            }
        }
    }

    QString namespaceURI = _root->namespaceURI();
    int nodi = element.childNodes().count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = element.childNodes().item(i) ;

        if(childNode.isElement()) {
            QDomElement childElement = childNode.toElement();
            QString localName = childElement.localName();
            if(childElement.namespaceURI() == namespaceURI) {
                if(localName == IO_XSD_ANNOTATION) {
                    if(NULL != _mainAnnotation) {
                        raiseError(loadContext, this, childNode, true);
                    }
                    _mainAnnotation = new XSchemaAnnotation(this, _root) ;
                    if(NULL == _mainAnnotation) {
                        raiseErrorForObject(loadContext, element);
                    }
                    _mainAnnotation->loadFromDom(loadContext, childElement);
                } else if(localName == IO_XSD_COMPLEXCONTENT_RESTRICTION) {
                    loadFromDom(loadContext, childElement);
                } else if(localName == IO_XSD_COMPLEXCONTENT_EXTENSION) {
                    loadFromDom(loadContext, childElement);
                } else {
                    raiseError(loadContext, this, childNode, true);
                }
            } else {
                raiseError(loadContext, this, childNode, true);
            }
        }
    }
    return isOk;
}

void XSchemaComplexContent::reparent(XSchemaObject *newParent)
{
    XSchemaObject::reparent(newParent);
    if(NULL != _mainAnnotation) {
        _mainAnnotation->reparentRoot(_root);
    }
}

QDomElement XSchemaComplexContent::writeBaseProperties(QDomElement &element)
{
    QDomDocument document = element.ownerDocument();
    QDomElement node = createElement(document, QString(IO_XSD_COMPLEXCONTENT));
    addAttrNotEmpty(node, IO_GENERIC_ID, _idContainer);
    // TODO addAttrNotEmpty( node, IO_GENERIC_MIXED, _mixed );
    addOtherAttributesToDom(node);
    if(NULL != _mainAnnotation) {
        _mainAnnotation->generateDom(document, node);
    }
    element.appendChild(node);
    return node;
}

void XSchemaComplexContent::scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void * /*context*/)
{
    QString name = attribute.nodeName();

    if(name == IO_GENERIC_ID) {
        _id = attribute.value() ;
    } else if(name == IO_GENERIC_BASE) {
        _base = attribute.value() ;
    } else {
        if(!readOtherAttributes(attribute)) {
            raiseError(loadContext, this, attribute, false);
        }
    }
}

void XSchemaComplexContent::scanForElements(XSDLoadContext *loadContext, QDomElement &element, void * context)
{
    XScanContext *theContext = (XScanContext*) context;
    bool isRaiseError = false ;
    QString name = element.localName();
    if(element.namespaceURI() == _root->namespaceURI()) {
        if(name == IO_XSD_ANNOTATION) {
            readHandleAnnotation(loadContext, element);
        } else if(name == IO_XSD_ATTRIBUTE) {
            readHandleObject(loadContext, element, new XSchemaAttribute(this, _root));
        } else if(name == IO_XSD_ATTRIBUTEGROUP) {
            readHandleObject(loadContext, element, new XSchemaAttributeGroup(this, _root));
        } else if(name == IO_XSD_ANYATTRIBUTE) {
            if(anyAttribute) {
                raiseError(loadContext, this, element, true);
            }
            readHandleObject(loadContext, element, new XSchemaAnyAttribute(this, _root));
            anyAttribute = true ;
        } else if(name == IO_XSD_GROUP) {
            if(theContext->childFound) {
                raiseError(loadContext, this, element, true);
            }
            readHandleObject(loadContext, element, new XSchemaGroup(this, _root));
            theContext->childFound = true ;
        } else if(name == IO_XSD_ALL) {
            if(theContext->childFound) {
                raiseError(loadContext, this, element, true);
            }
            readHandleObject(loadContext, element, new XSchemaAll(this, _root));
            theContext->childFound = true ;
        } else if(name == IO_XSD_CHOICE) {
            if(theContext->childFound) {
                raiseError(loadContext, this, element, true);
            }
            readHandleObject(loadContext, element, new XSchemaChoice(this, _root));
            theContext->childFound = true ;
        } else if(name == IO_XSD_SEQUENCE) {
            if(theContext->childFound) {
                raiseError(loadContext, this, element, true);
            }
            readHandleObject(loadContext, element, new XSchemaSequence(this, _root));
            theContext->childFound = true ;
        } else {
            isRaiseError = true ;
        }
    } else {
        isRaiseError = true ;
    }
    if(isRaiseError) {
        raiseError(loadContext, this, element, true);
    }
}

//class XSchemaComplexContentRestriction -----------------------------------------------------------------------------------------

XSchemaComplexContentRestriction::XSchemaComplexContentRestriction(XSchemaObject *newParent, XSchemaRoot *newRoot) : XSchemaComplexContent(newParent, newRoot)
{
}

XSchemaComplexContentRestriction::~XSchemaComplexContentRestriction()
{
    reset();
}

void XSchemaComplexContentRestriction::reset()
{
    XSchemaComplexContent::reset();
}

bool XSchemaComplexContentRestriction::generateDom(QDomDocument &/*document*/, QDomNode &/*parent*/)
{
    XsdError("TODO");
    return false ;
}

QString XSchemaComplexContentRestriction::description()
{
    QString description = QString("Restriction of '%1'").arg(_base) ;
    return description ;
}

//class XSchemaComplexContentRestriction -----------------------------------------------------------------------------------------


XSchemaComplexContentExtension::XSchemaComplexContentExtension(XSchemaObject *newParent, XSchemaRoot *newRoot)  : XSchemaComplexContent(newParent, newRoot)
{
}

XSchemaComplexContentExtension::~XSchemaComplexContentExtension()
{
    reset();
}

void XSchemaComplexContentExtension::reset()
{
    XSchemaComplexContent::reset();
}

bool XSchemaComplexContentExtension::generateDom(QDomDocument & /*document*/, QDomNode & /*parent*/)
{
    XsdError("TODO");
    return false;
}


QString XSchemaComplexContentExtension::description()
{
    QString description = QString("Extension of '%1'").arg(_base) ;
    return description ;
}
