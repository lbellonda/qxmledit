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

#include "scxmlinfo.h"
#include "utils.h"

Element *SCXMLState::element() const
{
    return _element;
}

void SCXMLState::setElement(Element *element)
{
    _element = element;
}

QString SCXMLState::id() const
{
    return _id;
}

void SCXMLState::setId(const QString &id)
{
    _id = id;
}

bool SCXMLState::isParallel() const
{
    return _parallel;
}

void SCXMLState::setParallel(bool parallel)
{
    _parallel = parallel;
}

SCXMLState::SCXMLState()
{
    _element = NULL ;
    _parallel = false ;
}

SCXMLState::~SCXMLState()
{
    EMPTYPTRLIST(_children, SCXMLState);
}

void SCXMLState::addChild(SCXMLState *child)
{
    _children.append(child);
}

void SCXMLState::allStates(QStringList &result)
{
    result << id();
    foreach(SCXMLState *state, _children) {
        state->allStates(result);
    }
}

//-------------

SCXMLInfo::SCXMLInfo()
{
}

SCXMLInfo::~SCXMLInfo()
{
    EMPTYPTRLIST(_children, SCXMLState);
}

void SCXMLInfo::addChild(SCXMLState *child)
{
    _children.append(child);
}

QStringList SCXMLInfo::allStates()
{
    Utils::TODO_THIS_RELEASE("fare SCXMLInfo::allStates");
    QStringList result ;
    foreach(SCXMLState *state, _children) {
        state->allStates(result);
    }
    return result;
    /*QStringList x;
    x << "a" << "b" << "c" ;
    return x ;*/
}
