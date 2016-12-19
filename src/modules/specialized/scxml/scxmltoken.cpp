/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016 by Luca Bellonda and individual contributors       *
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

#include "scxmltoken.h"
#include "utils.h"

SCXMLGroup::SCXMLGroup(const QString &newName)
{
    _name = newName ;
}

SCXMLGroup::~SCXMLGroup()
{
    EMPTYPTRLIST(_children, SCXMLTokenChild);
}

void SCXMLGroup::addChild(SCXMLTokenChild* newToken)
{
    _children.append(newToken);
}

#define IMPL_SCXML_TAG(x) const QString SCXMLToken::Tag_ ## x = #x

IMPL_SCXML_TAG(scxml);
IMPL_SCXML_TAG(state);
IMPL_SCXML_TAG(parallel);
IMPL_SCXML_TAG(transition);
IMPL_SCXML_TAG(initial);
IMPL_SCXML_TAG(final);
IMPL_SCXML_TAG(onentry);
IMPL_SCXML_TAG(onexit);
IMPL_SCXML_TAG(history);
IMPL_SCXML_TAG(raise);
IMPL_SCXML_TAG(if);
IMPL_SCXML_TAG(elseif);
IMPL_SCXML_TAG(else);
IMPL_SCXML_TAG(foreach);
IMPL_SCXML_TAG(log);
IMPL_SCXML_TAG(datamodel);
IMPL_SCXML_TAG(data);
IMPL_SCXML_TAG(assign);
IMPL_SCXML_TAG(donedata);
IMPL_SCXML_TAG(content);
IMPL_SCXML_TAG(param);
IMPL_SCXML_TAG(script);
IMPL_SCXML_TAG(cancel);
IMPL_SCXML_TAG(finalize);
IMPL_SCXML_TAG(send);
IMPL_SCXML_TAG(invoke);

QString SCXMLToken::name() const
{
    return _name;
}

void SCXMLToken::setName(const QString &name)
{
    _name = name;
}

QString SCXMLToken::description() const
{
    return _description;
}

void SCXMLToken::setDescription(const QString &description)
{
    _description = description;
}

QList<SCXMLTokenChild *> SCXMLToken::children() const
{
    return _children;
}

void SCXMLToken::addChild(SCXMLTokenChild *child)
{
    _children.append(child);
}

SCXMLToken::SCXMLToken()
{
}

SCXMLToken::~SCXMLToken()
{
    EMPTYPTRLIST(_children, SCXMLTokenChild);
}

QString SCXMLTokenChild::name() const
{
    return _name;
}

void SCXMLTokenChild::setName(const QString &name)
{
    _name = name;
}

QString SCXMLTokenChild::description() const
{
    return _description;
}

void SCXMLTokenChild::setDescription(const QString &value)
{
    _description = value;
}

int SCXMLTokenChild::min() const
{
    return _min;
}

void SCXMLTokenChild::setMin(int min)
{
    _min = min;
}

int SCXMLTokenChild::max() const
{
    return _max;
}

void SCXMLTokenChild::setMax(int max)
{
    _max = max;
}

SCXMLToken *SCXMLTokenChild::parent() const
{
    return _parent;
}

void SCXMLTokenChild::setParent(SCXMLToken *parent)
{
    _parent = parent;
}

SCXMLTokenChild::SCXMLTokenChild()
{
}

SCXMLTokenChild::~SCXMLTokenChild()
{
}
