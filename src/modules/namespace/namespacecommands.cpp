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


#include "namespacecommands.h"

NamespaceCommands::NamespaceCommands()
{
    _declareNs = DoNotDeclare ;
    _useNsElement = false ;
}

NamespaceCommands::~NamespaceCommands()
{
    reset();
}

QList<NamespaceSpec *> NamespaceCommands::namespaces()
{
    return _namespaces;
}


NamespaceCommands::ENSDecl NamespaceCommands::declareNs() const
{
    return _declareNs;
}

void NamespaceCommands::setDeclareNs(const ENSDecl declareNs)
{
    _declareNs = declareNs;
}

NamespaceSpec *NamespaceCommands::tagSpec()
{
    return &_tagSpec;
}

bool NamespaceCommands::useNsElement() const
{
    return _useNsElement;
}

void NamespaceCommands::setUseNsElement(bool useNsElement)
{
    _useNsElement = useNsElement;
}

void NamespaceCommands::reset()
{
    foreach(NamespaceSpec * spec, _namespaces) {
        delete spec;
    }
    _namespaces.clear();
}

bool NamespaceCommands::addNamespace(const QString &prefix, const QString &uri)
{
    NamespaceSpec * spec = new NamespaceSpec();
    if(NULL != spec) {
        spec->prefix = prefix ;
        spec->uri = uri ;
        _namespaces.append(spec);
        return true;
    }
    return false;
}
