/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2018 by Luca Bellonda and individual contributors  *
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


#ifndef NAMESPACECOMMANDS_H
#define NAMESPACECOMMANDS_H

#include "xmlEdit.h"

class NamespaceSpec
{
public:
    QString prefix;
    QString uri;
};

class NamespaceCommands
{
public:
    enum ENSDecl {
        DoNotDeclare,
        DeclareInElement
    };

private:
    QList<NamespaceSpec*> _namespaces;
    ENSDecl _declareNs;
    NamespaceSpec _tagSpec;
    bool _useNsElement;

    void reset();
public:
    NamespaceCommands();
    ~NamespaceCommands();
    QList<NamespaceSpec*> namespaces();
    ENSDecl declareNs() const;
    void setDeclareNs(const ENSDecl declareNs);
    NamespaceSpec *tagSpec();
    bool useNsElement() const;
    void setUseNsElement(bool useNsElement);
    bool addNamespace(const QString &prefix, const QString &uri);
};

#endif // NAMESPACECOMMANDS_H
