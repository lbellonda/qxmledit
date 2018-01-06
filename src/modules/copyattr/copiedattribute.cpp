/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013-2018 by Luca Bellonda and individual contributors  *
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


#include "copiedattribute.h"


CopyAttributesManager::CopyAttributesManager()
{

}

CopyAttributesManager::~CopyAttributesManager()
{
    reset();
}


void CopyAttributesManager::reset()
{
    while(!_copies.isEmpty()) {
        delete _copies.at(0);
        _copies.removeAt(0);
    }
}

void CopyAttributesManager::resetExcept(CopyAttributesSession *session)
{
    CopyAttributesSession *spare = NULL ;
    while(!_copies.isEmpty()) {
        CopyAttributesSession *removed  = _copies.at(0);
        if(removed != session) {
            delete removed;
        } else {
            spare = removed ;
        }
        _copies.removeAt(0);
    }
    if(NULL != spare) {
        _copies.append(spare);
    }
}


void CopyAttributesManager::addSession(CopyAttributesSession* newSession)
{
    _copies.append(newSession);
}

CopyAttributesSession *CopyAttributesManager::first()
{
    if(_copies.isEmpty()) {
        return NULL;
    }
    return _copies.at(_copies.size() - 1);
}


bool CopyAttributesManager::isEmpty()
{
    return _copies.isEmpty();
}

QList<CopyAttributesSession*>& CopyAttributesManager::sessions()
{
    return _copies;
}

//--------------------------------------------------------------------------------

CopyAttributesSession::CopyAttributesSession()
{
}

CopyAttributesSession::~CopyAttributesSession()
{
    Attribute::emptyList(_attributes);
}

QList<Attribute*> &CopyAttributesSession::attributes()
{
    return _attributes;
}

void CopyAttributesSession::setAttributes(QList<Attribute*> &attributes)
{
    _attributes = attributes;
}

QString CopyAttributesSession::name()
{
    return _name ;
}

void CopyAttributesSession::setName(const QString &newName)
{
    _name = newName ;
}

void CopyAttributesSession::clear()
{
    foreach(Attribute * attribute, _attributes) {
        delete attribute ;
    }
    _attributes.clear();
}

QString CopyAttributesSession::attributeValue(const QString &name)
{
    foreach(Attribute * attribute, _attributes) {
        if(attribute->name == name) {
            return attribute->value;
        }
    }
    return "";
}

void CopyAttributesSession::removeEmptyAttributes()
{
    bool isModified = false;
    do {
        isModified = false;
        foreach(Attribute * attribute, _attributes) {
            if(attribute->value.isEmpty()) {
                _attributes.removeAll(attribute);
                delete attribute ;
                isModified = true ;
                break;
            }
        }

    } while(isModified);
}
