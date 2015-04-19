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
#include <QClipboard>
#include <QApplication>
#include <QFile>
#include "utils.h"
#include "xsdeditor/XSchemaIOContants.h"

XSchemaAnnotation::XSchemaAnnotation(XSchemaObject *newParent, XSchemaRoot *newRoot) : XSchemaObject(newParent, newRoot)
{
}

XSchemaAnnotation::~XSchemaAnnotation()
{
    reset();
}

void XSchemaAnnotation::reset()
{
    foreach(XInfoBase * infoBase, _infos) {
        delete infoBase;
    }
    XSchemaObject::reset();
    //emit resetAction(); TODO
}

bool XSchemaAnnotation::generateDom(QDomDocument &document, QDomNode & parent)
{
    QDomElement node = createElement(document, IO_XSD_ANNOTATION);
    addAttrNotEmpty(node, IO_ANNOTATION_ATTR_ID, _id);
    addOtherAttributesToDom(node);

    foreach(XInfoBase * infoBase, _infos) {
        infoBase->generateDom(document, node);   // appinfo & documentation
    }
    parent.appendChild(node);
    return true ;
}

void XSchemaAnnotation::reparent(XSchemaObject *newParent)
{
    XSchemaObject::reparent(newParent);
    foreach(XSchemaObject * child, _infos) {
        child->reparentRoot(_root);
    }
}

QString XSchemaAnnotation::description()
{
    QString theDescription(tr("Annotation"));//TODO
    /*foreach( XInfoBase* infoBase, _infos ) {
        QDomNode node = infoBase->content();
        QDomDocumentFragment docf = node.toDocumentFragment();
        QDomDocument doc = docf.toDocument();
        QString str = doc.toString();
        theDescription += str ;
    }*/
    return theDescription ;
}

QString XSchemaAnnotation::text()
{
    QString theText;
    foreach(XInfoBase * infoBase, _infos) {
        QDomNode node = infoBase->content();
        if(node.isElement()) {
            QDomElement element = node.toElement();
            theText += element.text();
        } else {
            QDomDocument doc ;
            QDomNode node2 = doc.importNode(node, true);
            doc.appendChild(node2);
            QString str = doc.toString();
            theText += str ;
        }
    }
    return theText.trimmed() ;
}


void XSchemaAnnotation::loadFromDom(XSDLoadContext *loadContext, QDomElement &annotation)
{
    if(!((annotation.localName() == IO_XSD_ANNOTATION) && (annotation.namespaceURI() == _root->xsdNS()))) {
        XsdError("Annotation node not valid");
        return ;
    }
    QDomNamedNodeMap attributes = annotation.attributes();
    int numAttrs = attributes.length();
    for(int i = 0 ; i < numAttrs ; i++) {
        QDomNode node = attributes.item(i);
        QDomAttr attr = node.toAttr();
        QString name = node.nodeName();
        if(name == IO_SCHEMA_ATTR_ID) {
            _id = attr.value();
        } else {
            if(!readOtherAttributes(attr)) {
                raiseError(loadContext, this, attr, false);
            }
        }
    } // for

    QString namespacePrefix = _root->namespacePrefix();
    int nodi = annotation.childNodes().count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = annotation.childNodes().item(i) ;

        if(childNode.isElement()) {
            QDomElement element = childNode.toElement();
            QString name = element.localName();

            if(element.prefix() == namespacePrefix) {
                if(name == IO_XSD_TAGAPPINFO) {
                    XAppInfo *xappInfo = new XAppInfo(this, _root);
                    if(NULL == xappInfo) {
                        XsdError("Unable to read XAppInfo");
                    }
                    if(element.hasAttribute(IO_APPINFO_ATTR_SOURCE)) {
                        xappInfo->setSource(element.attribute(IO_APPINFO_ATTR_SOURCE));
                    }
                    xappInfo->setContent(element.cloneNode());
                    _infos.append(xappInfo);
                } else if(name == IO_XSD_DOCUMENTATION) {
                    XDocumentation *xdoc = new XDocumentation(this, _root);
                    if(NULL == xdoc) {
                        XsdError("Unable to read XDocumentation");
                    }
                    /* TODO QDomNamedNodeMap    attributes = element.attributes();
                    int len = attributes.length();
                    for( int i = 0 ; i < len ; i ++ ) {
                        QDomNode nd = attributes.item(i);
                        QString s = nd.nodeName();
                    }*/

                    if(element.hasAttributeNS(_xsdXML, IO_XML_LANGUAGE)) {
                        xdoc->setLanguage(element.attributeNS(_xsdXML, IO_XML_LANGUAGE));
                    }
                    if(element.hasAttribute(IO_DOCUMENTATION_ATTR_SOURCE)) {
                        xdoc->setSource(element.attribute(IO_DOCUMENTATION_ATTR_SOURCE));
                    }
                    xdoc->setContent(element.cloneNode());
                    _infos.append(xdoc);
                } else {
                    raiseError(loadContext, this, element, true);
                }
            }
        }
    }
}


//----------------------------------------------------------------------------------------------------

XInfoBase::XInfoBase(XSchemaObject *newParent, XSchemaRoot *newRoot): XSchemaObject(newParent, newRoot)
{
}

XInfoBase::~XInfoBase()
{
}

void XInfoBase::generateInnerNodes(QDomNode &parent)
{
    int nodi = _content.childNodes().count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = _content.childNodes().item(i) ;
        QDomNode newNode = childNode.cloneNode();
        parent.appendChild(newNode);
    }
}

void XInfoBase::copyTo(XInfoBase *other)
{
    other->_source = _source ;
    other-> _contentString = _contentString;
    other-> _content = _content ;
}

//----------------------------------------------------------------------------------------------------

XDocumentation::XDocumentation(XSchemaObject *newParent, XSchemaRoot *newRoot): XInfoBase(newParent, newRoot)
{
}

XDocumentation::~XDocumentation()
{
}

bool XDocumentation::generateDom(QDomDocument &document, QDomNode &parent)
{
    QDomElement node = _root->createElementWithNamespace(document, IO_XSD_DOCUMENTATION);
    addAttrNotEmpty(node, IO_DOCUMENTATION_ATTR_SOURCE, source());
    addAttrNotEmpty(node, XML_LANGUAGE, _language);
    generateInnerNodes(node);
    parent.appendChild(node);
    return true;
}

bool XDocumentation::isAnnotationElement()
{
    return true;
}

XDocumentation *XDocumentation::clone()
{
    XDocumentation *cloned = new XDocumentation(NULL, NULL);
    copyTo(cloned);
    cloned->_language = _language ;
    return cloned ;
}

//----------------------------------------------------------------------------------------------------

XAppInfo::XAppInfo(XSchemaObject *newParent, XSchemaRoot *newRoot): XInfoBase(newParent, newRoot)
{
}

XAppInfo::~XAppInfo()
{
}

bool XAppInfo::generateDom(QDomDocument &document, QDomNode &parent)
{
    QDomElement node = _root->createElementWithNamespace(document, IO_XSD_TAGAPPINFO);
    addAttrNotEmpty(node, IO_APPINFO_ATTR_SOURCE, source());
    generateInnerNodes(node);
    parent.appendChild(node);
    return true;
}

bool XAppInfo::isAnnotationElement()
{
    return true;
}

XAppInfo *XAppInfo::clone()
{
    XAppInfo *cloned = new XAppInfo(NULL, NULL);
    copyTo(cloned);
    return cloned ;
}

