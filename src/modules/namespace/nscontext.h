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


#ifndef NSCONTEXT_H
#define NSCONTEXT_H

#include "xmlEdit.h"
#include <QSet>
#include <QHash>
#include "libQXmlEdit_global.h"

class LIBQXMLEDITSHARED_EXPORT NSContext
{
    const NSContext *_parent;
    QString _currentNs;
    QHash<QString, QString> _namespacesByPrefix;
    QHash<QString, QSet<QString> > _namespacesAndPrefixes;

public:
    void addNamespace(const QString &prefix, const QString &ns);

public:
    NSContext(const NSContext *newParent);
    virtual ~NSContext();
    //---------
    void addNamespaceAttribute(const QString &attributeName, const QString &value);
    QSet<QString> namespacesOfContext();
    QSet<QString> prefixesForNamespace(const QString &ns) const;
    bool hasPrefixForNamespace(const QString &ns, const QString &prefix) const;
    //---
    QString uriFromName(const QString &name);
    QString uriFromPrefix(const QString &name) const;
};

#endif // NSCONTEXT_H
