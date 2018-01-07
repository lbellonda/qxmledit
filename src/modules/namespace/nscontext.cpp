/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016-2018 by Luca Bellonda and individual contributors  *
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

#include "nscontext.h"
#include "xmlutils.h"
#include "utils.h"

NSContext::NSContext(const NSContext *newParent)
{
    _parent = newParent ;
    if(NULL != newParent) {
        _currentNs = _parent->_currentNs ;
    }
}

NSContext::~NSContext()
{
}

void NSContext::addNamespaceAttribute(const QString &attributeName, const QString &value)
{
    if(XmlUtils::isDeclaringNS(attributeName)) {
        QString nsPrefix;
        if(XmlUtils::getNsPrefix(attributeName, nsPrefix)) {
            addNamespace(nsPrefix, value);
        }
    }
}

QSet<QString> NSContext::namespacesOfContext()
{
    QSet<QString> result;
    foreach(const QString & ns, _namespacesAndPrefixes.keys()) {
        result.insert(ns);
    }
    return result;
}

QSet<QString> NSContext::prefixesForNamespace(const QString &ns) const
{
    if(_namespacesAndPrefixes.contains(ns)) {
        return _namespacesAndPrefixes[ns];
    }
    return QSet<QString>();
}

bool NSContext::hasPrefixForNamespace(const QString &ns, const QString &prefix) const
{
    QSet<QString> set = prefixesForNamespace(ns);
    if(set.contains(prefix)) {
        return true;
    }
    if(NULL != _parent) {
        return _parent->hasPrefixForNamespace(ns, prefix);
    }
    return false;
}

void NSContext::addNamespace(const QString &prefix, const QString &ns)
{
    _namespacesByPrefix.insert(prefix, ns);
    // one ns, more prefixes
    if(!_namespacesAndPrefixes.contains(ns)) {
        QSet<QString> set;
        _namespacesAndPrefixes.insert(ns, set);
    }
    _namespacesAndPrefixes[ns].insert(prefix);
}

QString NSContext::uriFromName(const QString &name)
{
    QString prefix;
    QString localName;
    XmlUtils::decodeQualifiedName(name, prefix, localName);
    return uriFromPrefix(prefix);
}

QString NSContext::uriFromPrefix(const QString &name) const
{
    if(_namespacesByPrefix.contains(name)) {
        return _namespacesByPrefix[name];
    }
    if(NULL != _parent) {
        return _parent->uriFromPrefix(name);
    }
    return "";
}

