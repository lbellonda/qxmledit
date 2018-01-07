/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2018 by Luca Bellonda and individual contributors  *
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


#include "attributefilter.h"

AttributeFilter::AttributeFilter()
{
    _isTemporary = false;
    _isWhiteList = true ;
    _attributeNames.insert("class");
}

AttributeFilter::~AttributeFilter()
{
}

bool AttributeFilter::isIsWhiteList()
{
    return _isWhiteList ;
}


void AttributeFilter::setIsWhiteList(const bool value)
{
    _isWhiteList = value ;
}

QSet<QString> AttributeFilter::attributeNames()
{
    return _attributeNames ;
}

bool AttributeFilter::checkIfSkipped(const QString &name)
{
    bool skip = false;
    if(isIsWhiteList()) {
        if(!_attributeNames.contains(name)) {
            skip = true ;
        }
    } else {
        if(_attributeNames.contains(name)) {
            skip = true ;
        }
    }
    return skip;
}

void AttributeFilter::addName(const QString &name)
{
    _attributeNames.insert(name);
}

bool AttributeFilter::isTemporary()
{
    return _isTemporary;
}

void AttributeFilter::setTemporary(const bool newState)
{
    _isTemporary = newState;
}
