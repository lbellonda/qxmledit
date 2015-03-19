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

#include "elmpath.h"
#include "element.h"
#include "xmlutils.h"

ElmPath::ElmPath()
{
    _calculated = false;
    _parent = NULL ;
}

ElmPath::~ElmPath()
{
}

QString ElmPath::uriFromPrefix(const QString &prefix)
{
    if(_namespacesByPrefix.contains(prefix)) {
        return _namespacesByPrefix[prefix];
    }
    if(NULL != _parent) {
        return _parent->uriFromPrefix(prefix);
    }
    return "";
}

void ElmPath::addNamespace(const QString &prefix, const QString &namespaceUri)
{
    _namespacesByPrefix.insert(prefix, namespaceUri);
}

QString ElmPath::contextPath()
{
    if(_calculated) {
        return _cachedValue ;
    }
    QString prefix;
    QString localName;
    XmlUtils::decodeQualifiedName(_element->tag(), prefix, localName);
    QString nsUri = uriFromPrefix(prefix);
    QString result;
    if(nsUri.isEmpty()) {
        result = QString("/%1").arg(localName);
    } else {
        result = QString("/{%1}%2").arg(nsUri).arg(localName);
    }
    _cachedValue = result ;
    return result ;
}

QString ElmPath::namespaceForName(const QString &name)
{
    QString prefix;
    QString localName;
    XmlUtils::decodeQualifiedName(name, prefix, localName);
    QString nsUri;
    if(!prefix.isEmpty()) {
        nsUri = uriFromPrefix(prefix);
    }
    return nsUri;
}

QString ElmPath::namespaceForPrefix(const QString &prefix)
{
    QString nsUri;
    if(!prefix.isEmpty()) {
        nsUri = uriFromPrefix(prefix);
    }
    return nsUri;
}


//-----

ElmPathResolver::ElmPathResolver()
{
    baseElement = NULL ;
}

ElmPathResolver::~ElmPathResolver()
{
    reset();
}

void ElmPathResolver::reset()
{
    foreach(ElmPath * el, pathList) {
        delete el;
    }
    pathList.clear();
}

void ElmPathResolver::select(Element *element)
{
    baseElement = NULL ;
    reset();
    if((NULL == element) || !(element->isElement() || element->isText())) {
        return ;
    }
    baseElement = element ;
    Element *aParent = element;
    ElmPath *previous = NULL ;
    while(NULL != aParent) {
        ElmPath *path = new ElmPath();
        path->_element = aParent ;
        if(NULL != previous) {
            previous->_parent = path ;
        }
        previous = path ;
        foreach(Attribute * attribute, path->_element->getAttributesList()) {
            if(XmlUtils::isDeclaringNS(attribute->name)) {
                QString nsPrefix;
                if(XmlUtils::getNsPrefix(attribute->name, nsPrefix)) {
                    path->addNamespace(nsPrefix, attribute->value);
                }
            }
        }
        pathList.insert(0, path);
        aParent = aParent->parent();
    }
}


QString ElmPathResolver::path()
{
    QString result ;
    foreach(ElmPath * el, pathList) {
        result += el->contextPath();
    }
    return result;
}

QString ElmPathResolver::textPath()
{
    QString result = path();
    result += "/text()";
    return result;
}

QString ElmPathResolver::attributePath(const QString &attributeName)
{
    QString result = path();

    QString prefix;
    QString localName;
    XmlUtils::decodeQualifiedName(attributeName, prefix, localName);
    QString nsUri ;
    if(!pathList.isEmpty() && !prefix.isEmpty()) {
        nsUri = pathList.last()->namespaceForPrefix(prefix);
    }
    if(nsUri.isEmpty()) {
        result += QString("/@%1").arg(localName);
    } else {
        result += QString("/@{%1}%2").arg(nsUri).arg(localName);
    }
    return result;
}


