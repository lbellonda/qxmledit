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
#include "xmlutils.h"
#include "modules/xsd/xsdoperationparameters.h"
#include "modules/xsd/xsdhelper.h"

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

bool XSchemaAnnotation::compareToSimple(const XSchemaAnnotation* other)
{
    if(NULL == other) {
        return false;
    }
    XSDCompareOptions options;
    bool result = XSDCompareObject::XSDOBJECT_UNCHANGED == innerCompareTo((XSchemaObject*)other, options) ;
    return result ;
}

XSchemaAnnotation* XSchemaAnnotation::clone()
{
    XSchemaAnnotation *other = new XSchemaAnnotation(NULL, NULL);
    foreach(XInfoBase * info, _infos) {
        XInfoBase* newInfo = info->clone();
        other->_infos.append(newInfo);
    }
    other->_id = _id ;
    foreach(const QString & key, _otherAttributes.keys()) {
        other->_otherAttributes.insert(key, _otherAttributes[key]);
    }

    return other ;
}

void XSchemaAnnotation::addXInfo(XInfoBase *newInfo)
{
    _infos.append(newInfo);
}

QList<XInfoBase*> XSchemaAnnotation::infos()
{
    return _infos ;
}

QString XSchemaAnnotation::description()
{
    QString theDescription ;
    foreach(XInfoBase * infoBase, _infos) {
        theDescription += " ";
        theDescription += infoBase->contentString();
    }
    if(theDescription.length() > 100) {
        return theDescription.left(100) + QString("...");
    }
    return theDescription;
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

void XSchemaAnnotation::loadFromElement(XSDLoadContext *loadContext, Element *annotation, XSDOperationParameters * params)
{
    XSDOperationParameters localParams(params);
    localParams.setElementDeclarations(annotation, true);
    foreach(Attribute * attribute, annotation->getAttributesList()) {
        QString localName ;
        QString prefix;
        XmlUtils::decodeQualifiedName(attribute->name, prefix, localName);
        if((localName == IO_SCHEMA_ATTR_ID)
                && (prefix.isEmpty() || (localParams.getNSForPrefix(prefix) == Regola::XSDNameSpace))) {
            _id = attribute->value;
        } else {
            if(!readOtherAttributes(attribute, &localParams)) {
                raiseError(loadContext, this, annotation, attribute->name, false);
            }
        }
    } // for

    foreach(Element * child, *annotation->getChildItems()) {
        if(child->isElement()) {

            XSDOperationParameters childParams(&localParams);
            childParams.setElementDeclarations(child, false);

            QString childLocalName ;
            QString childPrefix;
            XmlUtils::decodeQualifiedName(child->tag(), childPrefix, childLocalName);

            if((childLocalName == IO_XSD_TAGAPPINFO) && (Regola::XSDNameSpace == childParams.getNSForPrefix(childPrefix))) {
                XAppInfo *xappInfo = new XAppInfo(this, _root);
                if(NULL == xappInfo) {
                    raiseError(loadContext, this, child, tr("Unable to read XAppInfo"));
                }
                if(child->hasAttribute(IO_APPINFO_ATTR_SOURCE)) {
                    xappInfo->setSource(child->getAttributeValue(IO_APPINFO_ATTR_SOURCE));
                }
                xappInfo->setContentString(XmlUtils::innerContent(child->getStringRepresentationForClipboard()));
                _infos.append(xappInfo);
            } else if((childLocalName == IO_XSD_DOCUMENTATION) && (Regola::XSDNameSpace == childParams.getNSForPrefix(childPrefix))) {
                XDocumentation *xdoc = new XDocumentation(this, _root);
                if(NULL == xdoc) {
                    raiseError(loadContext, this, child, tr("Unable to read XDocumentation"));
                }

                QString langAttribName = QString("xml:%1").arg(IO_XML_LANGUAGE);
                if(child->hasAttribute(langAttribName)) {
                    xdoc->setLanguage(child->getAttributeValue(langAttribName));
                }
                if(child->hasAttribute(IO_DOCUMENTATION_ATTR_SOURCE)) {
                    xdoc->setSource(child->getAttributeValue(IO_DOCUMENTATION_ATTR_SOURCE));
                }
                xdoc->setContentString(XmlUtils::innerContent(child->getStringRepresentationForClipboard()));
                _infos.append(xdoc);
            } else {
                raiseError(loadContext, this, child, child->tag(), true);
            }
        } // if element
    } // foreach child
}

Element *XSchemaAnnotation::toElement(XSDOperationParameters * params)
{
    // make a copy of the annotation element
    Element *theAnnotation = NULL ;
    theAnnotation = new Element(params->makeNameForXSDObject(IO_XSD_ANNOTATION), "", NULL, NULL);

    if(!_id.isEmpty()) {
        theAnnotation->addAttribute(IO_SCHEMA_ATTR_ID, _id);
    }
    foreach(const QString & key, _otherAttributes.keys()) {
        theAnnotation->addAttribute(key, _otherAttributes[key]);
    }

    // insert the children, using the model
    // refactorize to share the most of code with tests
    makeElementList(theAnnotation, params);
    return theAnnotation ;
}

Element *XSchemaAnnotation::makeElementList(Element *parent, XSDOperationParameters *params)
{
    XSDHelper helper;
    foreach(XInfoBase * object, _infos) {
        Element *newChild = NULL ;
        switch(object->getType()) {
        case SchemaTypeDocumentation:
            newChild = helper.makeElementDocumentation(static_cast<XDocumentation*>(object), parent, params);
            break;

        case SchemaTypeAppInfo:
            newChild = helper.makeElementAppInfo(static_cast<XAppInfo*>(object), parent, params);
            break;
        default:
            break;
        }
        if(NULL != newChild) {
            parent->addChild(newChild);
        }
    }
    return parent ;
}

//----------------------------------------------------------------------------------------------------

XInfoBase::XInfoBase(XSchemaObject * newParent, XSchemaRoot * newRoot): XSchemaObject(newParent, newRoot)
{
}

XInfoBase::~XInfoBase()
{
}

void XInfoBase::generateInnerNodes(QDomNode & parent)
{
    int nodi = _content.childNodes().count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = _content.childNodes().item(i) ;
        QDomNode newNode = childNode.cloneNode();
        parent.appendChild(newNode);
    }
}

void XInfoBase::copyTo(XInfoBase * other)
{
    other->_source = _source ;
    other-> _contentString = _contentString;
    other-> _content = _content ;
}

//----------------------------------------------------------------------------------------------------

XDocumentation::XDocumentation(XSchemaObject * newParent, XSchemaRoot * newRoot): XInfoBase(newParent, newRoot)
{
}

XDocumentation::~XDocumentation()
{
}

bool XDocumentation::generateDom(QDomDocument & document, QDomNode & parent)
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

XAppInfo::XAppInfo(XSchemaObject * newParent, XSchemaRoot * newRoot): XInfoBase(newParent, newRoot)
{
}

XAppInfo::~XAppInfo()
{
}

bool XAppInfo::generateDom(QDomDocument & document, QDomNode & parent)
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

