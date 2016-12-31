/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2016 by Luca Bellonda and individual contributors  *
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

#include "namespacemanager.h"
#include "regola.h"
#include "modules/specialized/xinclude/xincludeeditormanager.h"
#include "modules/specialized/scxml/scxmleditormanager.h"
#include "modules/specialized/specificpropertiesdialog.h"
#include "modules/xslt/xsleditormanager.h"
#include "undo/elinsertcommand.h"
#include "xmleditwidget.h"

#include "utils.h"

//#define WellKnownNamespacesFile ":/xsd/well_known_namespaces"

//------

NamespaceEditorInsertChoiceProvider::NamespaceEditorInsertChoiceProvider() {}
NamespaceEditorInsertChoiceProvider::~NamespaceEditorInsertChoiceProvider() {}

//------

NamespaceHandlerForEdit::NamespaceHandlerForEdit()
{
}

NamespaceHandlerForEdit::~NamespaceHandlerForEdit()
{
}

bool NamespaceHandlerForEdit::insertAction(QTreeWidget *tree, Regola *regola, Element *element, Element *newElement, const bool isChild)
{
    // empty path as root
    QList<int> destPath ;
    if(NULL != element) {
        destPath = element->indexPathOfNewRelative(isChild);
    } else {
        // as root
        destPath << regola->getChildItems()->size();
    }
    ElInsertCommand *cmd = new ElInsertCommand(tree, regola, newElement, destPath);
    regola->addUndo(cmd);
    return true ;

}

//------

NamespaceDef::NamespaceDef(const NamespaceManager::EWellKnownNs theCodeForWellKnown, const QString &theNamespace, const QString &theSchemaLocation,
                           const QString &theDescription, const QString &defaultPrefix)
{
    _namespace = theNamespace;
    _schemaLocation = theSchemaLocation ;
    _description = theDescription;
    _codeForWellKnown = theCodeForWellKnown;
    _defaultPrefix = defaultPrefix ;
}

NamespaceDef::~NamespaceDef()
{

}
QString NamespaceDef::schemaLocation() const
{
    return _schemaLocation;
}

void NamespaceDef::setSchemaLocation(const QString &location)
{
    _schemaLocation = location;
}

QString NamespaceDef::defaultPrefix() const
{
    return _defaultPrefix;
}

void NamespaceDef::setDefaultPrefix(const QString &defaultPrefix)
{
    _defaultPrefix = defaultPrefix;
}

QString NamespaceDef::description() const
{
    return _description;
}

void NamespaceDef::setDescription(const QString &description)
{
    _description = description;
}

QString NamespaceDef::uri() const
{
    return _namespace;
}

void NamespaceDef::setUri(const QString &value)
{
    _namespace = value;
}

//------

HandlerForInsert::HandlerForInsert()
{
}

HandlerForInsert::~HandlerForInsert()
{
    EMPTYPTRLIST(elements, SingleHandlerForInsert);
}

//------

const QString NamespaceManager::XSLFONamespace("http://www.w3.org/1999/XSL/Format");
const QString NamespaceManager::XSL1Namespace("http://www.w3.org/1999/XSL/Transform");
const QString NamespaceManager::XQueryLocalFuncNamespace("http://www.w3.org/2005/xquery-local-functions");
const QString NamespaceManager::MavenPom4Namespace("http://maven.apache.org/xsd/maven-4.0.0.xsd");
const QString NamespaceManager::XHTML11Namespace("http://www.w3.org/1999/xhtml");
const QString NamespaceManager::XIncludeNamespace("http://www.w3.org/2001/XInclude");
const QString NamespaceManager::SCXMLNamespace("http://www.w3.org/2005/07/scxml");

const QString NamespaceManager::NoNamespaceSchemaLocationAttributeName("noNamespaceSchemaLocation");
const QString NamespaceManager::SchemaLocationAttributeName("schemaLocation");

const QString NamespaceManager::XIncludePrefix("xi");
const QString NamespaceManager::SCXLMPrefix("scxml");

NamespaceManager::NamespaceManager()
{
    _inited = false;
    _dataInterface = NULL;
    _insertEditorProvider = this ;
}

NamespaceManager::~NamespaceManager()
{
    reset();
}

void NamespaceManager::init()
{
    if(_inited) {
        return ;
    }
    _inited = true ;

    insertItem(XSI_NAMESPACE, Regola::XSDSchemaInstance, Regola::XSDSchemaInstance, QObject::tr("Schema Instance (xsi)"), "xsi");
    insertItem(XSD_NAMESPACE, Regola::XSDNameSpace, "http://www.w3.org/2009/XMLSchema.xsd" /*"http://www.w3.org/2012/04/XMLSchema.xsd"*/, QObject::tr("XML Schema (xsd or xs)"), "xsd");
    // there is no "official" xsl*-fo schema repository
    insertItem(XSLFO_NAMESPACE, XSLFONamespace, "", QObject::tr("XSL-FO 1.0 (fo)"), "fo");
    insertItem(XSL1_NAMESPACE, XSL1Namespace, "http://www.w3.org/1999/11/xslt10.dtd", QObject::tr("XSL 1.0 (xsl)"), "xsl", new XSLEditorManager());
    // predefined namespace
    insertItem(XQUERY_LOCALFUNC_NAMESPACE, XQueryLocalFuncNamespace, "", QObject::tr("xquery local functions (local)"), "local");
    insertItem(MAVEN_NAMESPACE, MavenPom4Namespace, "http://maven.apache.org/xsd/maven-4.0.0.xsd", QObject::tr("Maven POM 4 (local)"), "local");
    insertItem(GENERIC_NAMESPACE, XHTML11Namespace, "http://www.w3.org/MarkUp/SCHEMA/xhtml11.xsd", QObject::tr("XHTML 1.1 (html)"), "html");
    insertItem(XINCLUDE_NAMESPACE, XIncludeNamespace, "https://www.w3.org/2001/XInclude/XInclude.xsd", QObject::tr("XInclude 1.1 (xi)"), XIncludePrefix, new XIncludeEditorManager());
    insertItem(SCXML_NAMESPACE, SCXMLNamespace, "http://www.w3.org/2011/04/SCXML/scxml.xsd", QObject::tr("SXCML 1.1 (scxml)"), SCXLMPrefix, new SCXMLEditorManager());
}

DataInterface *NamespaceManager::dataInterface() const
{
    return _dataInterface;
}

void NamespaceManager::setDataInterface(DataInterface *dataInterface)
{
    _dataInterface = dataInterface;
}

QString NamespaceManager::namespaceUri(const EWellKnownNs eWellKnownNs)
{
    init();
    if(_namespaces.contains(eWellKnownNs)) {
        return _namespaces[eWellKnownNs]->_namespace;
    }
    return "";
}

QList<NamespaceDef*> NamespaceManager::allNamespaces()
{
    init();
    QList<NamespaceDef*> result = _namespaces.values();
    return result;
}

NamespaceDef* NamespaceManager::namespacesForUri(const QString &uri)
{
    init();
    if(_uriNamespaces.contains(uri)) {
        return _uriNamespaces[uri];
    }
    return NULL ;
}

void NamespaceManager::reset()
{
    foreach(NamespaceDef * def, _namespaces.values()) {
        delete def;
    }
    EMPTYPTRLIST(_editHandlers.values(), NamespaceHandlerForEdit);
    _namespaces.clear();
}

void NamespaceManager::insertItem(const EWellKnownNs wellKnownNs, const QString &theNamespace, const QString &theSchemaLocation,
                                  const QString &theDescription, const QString &defaultPrefix, NamespaceHandlerForEdit *editHandler)
{
    NamespaceDef *def = new NamespaceDef(wellKnownNs, theNamespace, theSchemaLocation, theDescription, defaultPrefix);
    _namespaces.insert(wellKnownNs, def);
    if(wellKnownNs != GENERIC_NAMESPACE) {
        _uriNamespaces.insert(theNamespace, def);
    }
    if(NULL != editHandler) {
        _editHandlers.insert(theNamespace, editHandler);
    }
}

bool NamespaceManager::editElement(QWidget *parent, XmlEditWidget *editor, QTreeWidget *tree, Regola *regola, Element *element)
{
    if((NULL != element) && element->isElement()) {
        QXName qname ;
        element->qName(&qname);
        NamespaceHandlerForEdit *handler = _editHandlers[qname.ns];
        if(NULL != handler) {
            handler->handleEdit(parent, editor, tree, regola, element);
            return true;
        }
    }
    return false;
}

bool NamespaceManager::insertElement(QWidget *parent, XmlEditWidget *editor, QTreeWidget *tree, Regola *regola, Element *element, const bool isChildOrSibling)
{
    bool result = false ;
    const bool isSXLT = editor->editMode() == XmlEditWidgetEditMode::XSLT ;
    const bool isSCXML = editor->editMode() == XmlEditWidgetEditMode::SCXML ;
    QList<HandlerForInsert*> handlers ;
    foreach(NamespaceHandlerForEdit *handler, _editHandlers.values()) {
        HandlerForInsert* hfi = handler->handlerForInsert(editor, regola, element, isChildOrSibling) ;
        if(NULL != hfi) {
            if(!hfi->elements.isEmpty()) {
                handlers.append(hfi);
            } else {
                delete hfi;
            }
        }
    }
    if(isSCXML) {
        sortListGivingPrecedenceTo(NamespaceManager::SCXMLNamespace, handlers);
    }
    if(isSXLT) {
        sortListGivingPrecedenceTo(NamespaceManager::XSL1Namespace, handlers);
        Utils::TODO_THIS_RELEASE("sortListGivingPrecedenceTo(NamespaceManager::XSL2Namespace, handlers);");
    }
    HandlerForInsert * handler = _insertEditorProvider->handleInsertElementForSpecialized(parent, &handlers);
    if(NULL != handler) {
        result = handler->handler->handleInsert(editor, tree, regola, element, isChildOrSibling, handler->outputSelectedCode);
    }
    EMPTYPTRLIST(handlers, HandlerForInsert);
    return result;
}

void NamespaceManager::sortListGivingPrecedenceTo(const QString &ns, QList<HandlerForInsert*> &handlers)
{
    HandlerForInsert *thisHandler = NULL ;
    foreach(HandlerForInsert *handler, handlers) {
        if(ns == handler->nameSpace) {
            thisHandler = handler ;
            handlers.removeOne(handler);
            break;
        }
    }
    if(NULL != thisHandler) {
        handlers.insert(0, thisHandler);
    }
}

HandlerForInsert *NamespaceManager::handleInsertElementForSpecialized(QWidget *parent, QList<HandlerForInsert*> *handlers)
{
    SpecificPropertiesDialog spd(parent, handlers);
    if(spd.exec() == QDialog::Accepted) {
        spd.handler()->outputSelectedCode = spd.selectedItemCode() ;
        return spd.handler();
    }
    return NULL ;
}

void NamespaceManager::setProviderForInsert(NamespaceEditorInsertChoiceProvider *newProvider)
{
    _insertEditorProvider = newProvider ;
}

XIncludeEditorManager *NamespaceManager::xIncludeEditorManager()
{
    return static_cast<XIncludeEditorManager *>(_editHandlers[XIncludeNamespace]);
}

SCXMLEditorManager *NamespaceManager::scxmlEditorManager()
{
    return static_cast<SCXMLEditorManager *>(_editHandlers[SCXMLNamespace]);
}
