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


NamespaceDef::NamespaceDef(const NamespaceManager::EWellKnownNs theCodeForWellKnown, const QString &theNamespace, const QString &theDescription)
{
    _namespace = theNamespace;
    _description = theDescription;
    _codeForWellKnown = theCodeForWellKnown;
}

NamespaceDef::~NamespaceDef()
{

}

//------

NamespaceManager::NamespaceManager()
{
    insertItem(XSI_NAMESPACE, Regola::XSDSchemaInstance, QObject::tr("Schema Instance (xsi)"));
    insertItem(XSD_NAMESPACE, Regola::XSDNameSpace, QObject::tr("XML Schema (xsd or xs)"));
}

NamespaceManager::~NamespaceManager()
{
    reset();
}

QString NamespaceManager::namespaceUri(const EWellKnownNs eWellKnownNs)
{
    if(_namespaces.contains(eWellKnownNs)) {
        return _namespaces[eWellKnownNs]->_namespace;
    }
    return "";
}

void NamespaceManager::reset()
{
    foreach(NamespaceDef * def, _namespaces.values()) {
        delete def;
    }
    _namespaces.clear();
}

void NamespaceManager::insertItem(const EWellKnownNs wellKnownNs, const QString &theNamespace, const QString &theDescription)
{
    NamespaceDef *def = new NamespaceDef(wellKnownNs, theNamespace, theDescription);
    _namespaces.insert(wellKnownNs, def);
}
