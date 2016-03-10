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


#ifndef ELMPATH_H
#define ELMPATH_H

#include <QMap>
#include <QHash>
#include <QString>
#include "libQXmlEdit_global.h"

class Element ;

class LIBQXMLEDITSHARED_EXPORT ElmPath
{
    Element *_element;
    QMap<QString, QString> _namespacesByPrefix;
    ElmPath *_parent;
    bool _calculated;
    QString _cachedValue;
public:
    ElmPath();
    ~ElmPath();

    QString uriFromPrefix(const QString &prefix);
    QString contextPath();
    QString namespaceForPrefix(const QString &prefix);
    QString namespaceForName(const QString &name);
    void addNamespace(const QString &prefix, const QString &namespaceUri);

    friend class ElmPathResolver;

};


class LIBQXMLEDITSHARED_EXPORT ElmPathResolver
{
    QList<ElmPath*> pathList;
    Element *baseElement;

    void reset();
public:
    ElmPathResolver();
    ~ElmPathResolver();

    void select(Element *element);
    QString path();
    QString textPath();
    QString attributePath(const QString &attributeName);
    void collectParentNamespaces(Element *element, QHash<QString, QString> &nsMap);
};

#endif // ELMPATH_H
