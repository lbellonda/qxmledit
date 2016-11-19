/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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

#include "genericpersistentdbdata.h"
#include <QUuid>
#include <QSet>

GenericPersistentDBData::GenericPersistentDBData()
{
    _isReadOnly = false;
    setUuid(QUuid::createUuid().toString());
    setId(0);
    setCreationDate(QDateTime::currentDateTime());
    setUpdateDate(QDateTime::currentDateTime());
}

GenericPersistentDBData::~GenericPersistentDBData()
{
}


bool GenericPersistentDBData::isReadOnly()
{
    return _isReadOnly ;
}

void GenericPersistentDBData::setReadOnly(const bool newReadOnly)
{
    _isReadOnly = newReadOnly ;
}

void GenericPersistentDBData::setId(const int newId)
{
    _id = newId;
}

int GenericPersistentDBData::id()
{
    return _id ;
}

void GenericPersistentDBData::setName(const QString &newName)
{
    _name = newName;
}

QString GenericPersistentDBData::name()
{
    return _name ;
}

void GenericPersistentDBData::setDescription(const QString &newDescription)
{
    _description = newDescription;
}

QString GenericPersistentDBData::description()
{
    return _description ;
}


void GenericPersistentDBData::setPayload(const QString &newPayload)
{
    _payload = newPayload ;
}

QString GenericPersistentDBData::payload()
{
    return _payload ;
}

void GenericPersistentDBData::setType(const QString &newType)
{
    _type = newType;
}

QString GenericPersistentDBData::uuid()
{
    return _uuid;
}

void GenericPersistentDBData::setUuid(const QString &newUuid)
{
    _uuid = newUuid;
}

QString GenericPersistentDBData::type()
{
    return _type ;
}

QSet<QString> &GenericPersistentDBData::tags()
{
    return _tags;
}

void GenericPersistentDBData::setTags(const QSet<QString> &newTags)
{
    QSet<QString> filteredTags ;
    foreach(QString s, newTags.values()) {
        filteredTags.insert(s.toLower());
    }

    _tags = filteredTags ;
}

void GenericPersistentDBData::setUpdateDate(const QDateTime date)
{
    _updateDate = date ;
}

void GenericPersistentDBData::setCreationDate(const QDateTime date)
{
    _creationDate = date ;
}

QDateTime GenericPersistentDBData::updateDate()
{
    return _updateDate ;
}

QDateTime GenericPersistentDBData::creationDate()
{
    return _creationDate;
}

QString GenericPersistentDBData::creationUser()
{
    return _author;
}

QString GenericPersistentDBData::updateUser()
{
    return _updateUser;
}

void GenericPersistentDBData::setCreationUser(QString const& value)
{
    _author = value;
}

void GenericPersistentDBData::setFieldName(QString *field, const QString &fieldName)
{
    if(NULL != field) {
        *field = fieldName ;
    }
}

#define CHECK(x)    \
    if( x() != other->x()) { \
        setFieldName(field, #x);\
        return false;\
    }


#define CHECKTIME(x)    \
    if( x().toString() != other->x().toString()) { \
        setFieldName(field, #x);\
        return false;\
    }



bool GenericPersistentDBData::compareTags(GenericPersistentData *other)
{
    if(tags().size() != other->tags().size()) {
        return false;
    }
    foreach(QString tag, tags()) {
        if(!other->tags().contains(tag)) {
            return false;
        }
    }
    return true;
}

bool GenericPersistentDBData::isEqual(GenericPersistentData *other, QString *field)
{
    if(NULL == other) {
        setFieldName(field, "NULL");
        return false;
    }
    CHECK(id);
    CHECK(uuid);
    CHECK(name);
    CHECK(description);
    CHECK(payload);
    CHECK(type);
    if(!compareTags(other)) {
        setFieldName(field, "tags");
        return false;
    }
    CHECK(updateUser);
    CHECKTIME(updateDate);
    CHECK(creationUser);
    CHECKTIME(creationDate);
    CHECK(isReadOnly);
    return true;
}

//-------------------------- i/o ----------------------------------


