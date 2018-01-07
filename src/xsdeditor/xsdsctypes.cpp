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
#include "xsdeditor/xsddefinitions.h"
#include "xsdeditor/XSchemaIOContants.h"

XSchemaSimpleType::XSchemaSimpleType(XSchemaObject *newParent, XSchemaRoot *newRoot) : XSchemaObject(newParent, newRoot)
{
}

XSchemaSimpleType::~XSchemaSimpleType()
{
    reset();
}

void XSchemaSimpleType::reset()
{
    XSchemaObject::reset();
}

bool XSchemaSimpleType::canAddRestriction()
{
    return !containsSimpleType();
}

bool XSchemaSimpleType::canAddUnion()
{
    return !containsSimpleType();
}

bool XSchemaSimpleType::canAddList()
{
    return !containsSimpleType();
}

bool XSchemaSimpleType::isSimpleType()
{
    return true;
}

//------------------------------------------------------------------------------------------

XSchemaSimpleTypeRestriction::XSchemaSimpleTypeRestriction(XSchemaObject *newParent, XSchemaRoot *newRoot) : XSchemaSimpleType(newParent, newRoot)
{
    ;
}

XSchemaSimpleTypeRestriction::~XSchemaSimpleTypeRestriction()
{
    reset();
}

IMPL_PROPS(XSchemaSimpleTypeRestriction, base, setBase)

bool XSchemaSimpleTypeRestriction::generateDom(QDomDocument &document, QDomNode &parent)
{
    QDomElement node = createElement(document, IO_XSD_RESTRICTION);
    addAttrNotEmpty(node, IO_GENERIC_ID, _id);
    addAttrNotEmpty(node, IO_GENERIC_BASE, _base);
    addOtherAttributesToDom(node);
    if(NULL != _annotation) {
        _annotation->generateDom(document, node);
    }
    generateFacets(node, _facets);

    if(!generateInnerDom(document, node)) {
        return false;
    }
    parent.appendChild(node);
    return true;
}

void XSchemaSimpleTypeRestriction::reset()
{
    XSchemaSimpleType::reset();
}

QString XSchemaSimpleTypeRestriction::description()
{
    QString result = tr("Simple type restriction");
    if(!_base.isEmpty()) {
        result += " (";
        result += _base;
        result += ")";
    }
    result += _facets.toString();
    return result;
}

QString XSchemaSimpleTypeRestriction::descriptionDiffHtml()
{
    QString result = tr("Simple type restriction");
    if(!_base.isEmpty()) {
        result += " (";
        result += _base;
        result += ")";
    }
    result += _facets.toStringDiffHtml();
    return result;
}

QString XSchemaSimpleTypeRestriction::exportFacets()
{
    return _facets.toString();
}

void XSchemaSimpleTypeRestriction::addEnumsToListIfAny(QStringList &enums)
{
    _facets.addEnumsToList(enums);
}

void XSchemaSimpleTypeRestriction::scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void * /*context*/)
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


void XSchemaSimpleTypeRestriction::scanForElements(XSDLoadContext *loadContext, QDomElement &element, void *context)
{
    XScanContext *theContext = (XScanContext*) context;
    QString name = element.localName();
    QString value = element.attribute(IO_FACET_ATTR_VALUE, "");
    if(element.namespaceURI() == _root->namespaceURI()) {
        if(name == IO_XSD_ANNOTATION) {
            readHandleAnnotation(loadContext, element);
        } else if(name == IO_XSD_SIMPLETYPE) {
            if(theContext->childFound) {
                raiseError(loadContext, this, element, true);
            }
            theContext->childFound = true;
            readHandleObject(loadContext, element, new XSchemaElement(this, _root));
            /*} else if( scanForFacets( name, value, _facets ) ) { TODO: fare poi
                  // nothing TODO: delete*/
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
        } else {
            raiseError(loadContext, this, element, true);
        }
    } else {
        raiseError(loadContext, this, element, true);
    }
}

//--------------------------------------------------------------------------------

XSchemaSimpleTypeUnion::XSchemaSimpleTypeUnion(XSchemaObject *newParent, XSchemaRoot *newRoot) : XSchemaSimpleType(newParent, newRoot)
{
    ;
}

XSchemaSimpleTypeUnion::~XSchemaSimpleTypeUnion()
{
    reset();
}

IMPL_PROPS(XSchemaSimpleTypeUnion, memberTypes, setMmemberTypes)

bool XSchemaSimpleTypeUnion::generateDom(QDomDocument & /*document*/, QDomNode & /*parent*/)
{
    XsdError("TODO");
    return false ;
}

void XSchemaSimpleTypeUnion::reset()
{
    XSchemaSimpleType::reset();
}

bool XSchemaSimpleTypeUnion::canAddRestriction()
{
    return true ;
}


bool XSchemaSimpleTypeUnion::canAddUnion()
{
    return true;
}

bool XSchemaSimpleTypeUnion::canAddList()
{
    return true;
}

QString XSchemaSimpleTypeUnion::description()
{
    return tr("union: [%1]").arg(_memberTypes);
}

void XSchemaSimpleTypeUnion::scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void * /*context*/)
{
    QString name = attribute.nodeName();

    if(name == IO_GENERIC_ID) {
        _id = attribute.value() ;
    } else if(name == IO_SIMPLETYPE_UNION_ATTR_MEMBERTYPES) {
        _memberTypes = attribute.value() ;
    } else {
        if(!readOtherAttributes(attribute)) {
            raiseError(loadContext, this, attribute, false);
        }
    }
}

void XSchemaSimpleTypeUnion::scanForElements(XSDLoadContext *loadContext, QDomElement &element, void * /*context*/)
{
    QString name = element.localName();
    if(element.namespaceURI() == _root->namespaceURI()) {
        if(name == IO_XSD_ANNOTATION) {
            readHandleAnnotation(loadContext, element);
        } else if(name == IO_XSD_SIMPLETYPE) {
            readHandleObject(loadContext, element, new XSchemaElement(this, _root));
        } else {
            raiseError(loadContext, this, element, true);
        }
    } else {
        raiseError(loadContext, this, element, true);
    }
}


//-----------------------------------------------------------------------------------------

XSchemaSimpleTypeList::XSchemaSimpleTypeList(XSchemaObject *newParent, XSchemaRoot *newRoot) : XSchemaSimpleType(newParent, newRoot)
{
    ;
}

XSchemaSimpleTypeList::~XSchemaSimpleTypeList()
{
    reset();
}

IMPL_PROPS(XSchemaSimpleTypeList, itemType, setItemType)

bool XSchemaSimpleTypeList::generateDom(QDomDocument &document, QDomNode &parent)
{
    QDomElement node = createElement(document, IO_XSD_LIST);
    addAttrNotEmpty(node, IO_GENERIC_ID, id());
    addAttrNotEmpty(node, IO_SIMPLETYPE_LIST_ATTR_ITEMTYPE, itemType());
    //TODO ohther attributes
    if(NULL != _annotation) {
        _annotation->generateDom(document, node);
    }
    foreach(XSchemaObject * child, _children) {
        child->generateDom(document, parent);
        break;
    }
    parent.appendChild(node);
    return true ;
}

void XSchemaSimpleTypeList::reset()
{
    XSchemaSimpleType::reset();
}

void XSchemaSimpleTypeList::scanForAttributes(XSDLoadContext *loadContext, QDomAttr &attribute, void * /*context*/)
{
    QString name = attribute.nodeName();

    if(name == IO_GENERIC_ID) {
        _id = attribute.value() ;
    } else if(name == IO_SIMPLETYPE_LIST_ATTR_ITEMTYPE) {
        _itemType = attribute.value() ;
    } else {
        if(!readOtherAttributes(attribute)) {
            raiseError(loadContext, this, attribute, false);
        }
    }
}

void XSchemaSimpleTypeList::scanForElements(XSDLoadContext *loadContext, QDomElement &element, void *context)
{
    XScanContext *theContext = (XScanContext*) context;
    QString name = element.localName();
    if(element.namespaceURI() == _root->namespaceURI()) {
        if(name == IO_XSD_ANNOTATION) {
            readHandleAnnotation(loadContext, element);
        } else if(name == IO_XSD_SIMPLETYPE) {
            if(theContext->childFound) {
                XsdError("TODO");
            }
            theContext->childFound = true;
            readHandleObject(loadContext, element, new XSchemaElement(this, _root));
        } else {
            //TODO (minExclusive | minInclusive | maxExclusive | maxInclusive | totalDigits | fractionDigits | length | minLength | maxLength | enumeration | whiteSpace | pattern)*))
        }
    } else {
        XsdError("TODO");
    }
}
