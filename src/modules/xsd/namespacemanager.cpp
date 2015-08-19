/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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
#include "utils.h"

//#define WellKnownNamespacesFile ":/xsd/well_known_namespaces"

NamespaceDef::NamespaceDef(const NamespaceManager::EWellKnownNs theCodeForWellKnown, const QString &theNamespace,
                           const QString &theDescription, const QString &defaultPrefix)
{
    _namespace = theNamespace;
    _description = theDescription;
    _codeForWellKnown = theCodeForWellKnown;
    _defaultPrefix = defaultPrefix ;
}

NamespaceDef::~NamespaceDef()
{

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
const QString NamespaceManager::XSLFONamespace("http://www.w3.org/1999/XSL/Format");
const QString NamespaceManager::XSL1Namespace("http://www.w3.org/1999/XSL/Transform");
const QString NamespaceManager::XQueryLocalFuncNamespace("http://www.w3.org/2005/xquery-local-functions");

NamespaceManager::NamespaceManager()
{
    _inited = false;
    _dataInterface = NULL;
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
    insertItem(XSI_NAMESPACE, Regola::XSDSchemaInstance, QObject::tr("Schema Instance (xsi)"), "xsi");
    insertItem(XSD_NAMESPACE, Regola::XSDNameSpace, QObject::tr("XML Schema (xsd or xs)"), "xsd");
    insertItem(XSLFO_NAMESPACE, XSLFONamespace, QObject::tr("XSL-FO 1.0 (fo)"), "fo");
    insertItem(XSL1_NAMESPACE, XSL1Namespace, QObject::tr("XSL 1.0 (xsl)"), "xsl");
    insertItem(XQUERY_LOCALFUNC_NAMESPACE, XQueryLocalFuncNamespace, QObject::tr("xquery local functions (local)"), "local");
    Utils::TODO_THIS_RELEASE("fo;xsl;maven;");
    Utils::TODO_THIS_RELEASE("elimina codice sotto");
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
    _namespaces.clear();
}

void NamespaceManager::insertItem(const EWellKnownNs wellKnownNs, const QString &theNamespace, const QString &theDescription, const QString &defaultPrefix)
{
    NamespaceDef *def = new NamespaceDef(wellKnownNs, theNamespace, theDescription, defaultPrefix);
    _namespaces.insert(wellKnownNs, def);
    _uriNamespaces.insert(theNamespace, def);
}

/*
bool NamespaceManager::scanData(QDomNode &rootNode)
{
    bool isOk = true ;
    mapTokens.clear();
    int nodi = rootNode.childNodes().count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = rootNode.childNodes().item(i) ;

        if(childNode.isElement()) {
            QDomElement element = childNode.toElement();
            QString name = element.attribute("uri", "");
            QString description = element.attribute("description", "");
            QString prefix = element.attribute("prefix", "");
non mi e' chiara la costante a chi serve e chi la usa...
        ma serve veramente il file esterno? e con cosa lo decodifico?
                                                come faccio ad essere sicuro che le costanti siano a posto?
            insertItem(XSI_NAMESPACE, uri, description, prefix);
            if(isOk) {
                completeStyle(style);
            }
        }
    }//for
    return isOk ;
}//scanData()

bool NamespaceManager::loadWellKnownNamespaces()
{
    bool isOk = false;
    QFile file(WellKnownNamespacesFile);
    if(file.open(QIODevice::ReadOnly)) {
        QDomDocument document;
        if(document.setContent(&file)) {
            isOk = scanData(document);
        } else {
            Utils::error(tr("Unable to parse system namespaces XML"));
        }
        file.close();
    } else {
        Utils::error(QString(tr("Unable to load system namespaces.\n Error code is '%1'")).arg(file.error()));
    }
    return isOk ;
}
*/
