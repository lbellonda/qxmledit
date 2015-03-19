/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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


#include "attrfilterprofile.h"

#ifdef  LOG_DATA
#include <stdio.h>
#endif

AttrFilterProfile::AttrFilterProfile()
{
    _id = 0 ;
    _isWhiteList = true ;
}

AttrFilterProfile::AttrFilterProfile(const int newId, const QString &newName, const QString &newDescription, const bool newIsWhiteList, const QDateTime &newCreationTime, const QDateTime &newUpdateTime)
{
    _id = newId ;
    _name = newName;
    _description = newDescription;
    _isWhiteList = newIsWhiteList;
    _creationTime =  newCreationTime ;
    _updateTime = newUpdateTime;
}

AttrFilterProfile::~AttrFilterProfile()
{
}


int AttrFilterProfile::id()
{
    return _id ;
}

void AttrFilterProfile::setId(const int value)
{
    _id = value ;
}

QString AttrFilterProfile::name()
{
    return _name ;
}


void AttrFilterProfile::setName(const QString& value)
{
    _name = value ;
}

QString AttrFilterProfile::description()
{
    return _description ;
}


void AttrFilterProfile::setDescription(const QString& value)
{
    _description = value ;
}

QDateTime AttrFilterProfile::creationTime()
{
    return _creationTime ;
}


void AttrFilterProfile::setCreationTime(const QDateTime &value)
{
    _creationTime = value ;
}

QDateTime AttrFilterProfile::updateTime()
{
    return _updateTime ;
}


void AttrFilterProfile::setUpdateTime(const QDateTime &value)
{
    _updateTime = value ;
}


bool AttrFilterProfile::isWhiteList()
{
    return _isWhiteList ;
}


void AttrFilterProfile::setWhiteList(const bool value)
{
    _isWhiteList = value ;
}


bool AttrFilterProfile::compareToBase(const AttrFilterProfile * other)
{
    bool isEquals = true ;
    if(NULL == other) {
        isEquals = false;
    } else {
        if(_name != other->_name) {
            isEquals = false;
        } else if(_description != other->_description) {
            isEquals = false;
        } else if(_id != other->_id) {
            isEquals = false;
        } else if(_isWhiteList != other->_isWhiteList) {
            isEquals = false;
        } else {
            isEquals = true;
        }
    }
    return isEquals;
}

bool AttrFilterProfile::compareTo(const AttrFilterProfile * other)
{
    bool isEquals = true ;
    if(!compareToBase(other)) {
        return false;
    }
    if(_creationTime.toString(Qt::ISODate) != other->_creationTime.toString(Qt::ISODate)) {
#ifdef  LOG_DATA
        fprintf(stdout, "1: cr %s\n", _creationTime.toString(Qt::ISODate).toLatin1().data());
        fprintf(stdout, "2: cr %s\n", other->_creationTime.toString(Qt::ISODate).toLatin1().data());
        fflush(stdout);
#endif
        isEquals = false;
    } else if(_updateTime.toString(Qt::ISODate) != other->_updateTime.toString(Qt::ISODate)) {
#ifdef  LOG_DATA
        fprintf(stdout, "1: u %s\n", _updateTime.toString(Qt::ISODate).toLatin1().data());
        fprintf(stdout, "2: u %s\n", other->_updateTime.toString(Qt::ISODate).toLatin1().data());
        fflush(stdout);
#endif
        isEquals = false;
    } else {
        isEquals = true;
    }
    return isEquals;
}

