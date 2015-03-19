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


#ifndef GENERICPERSISTENTDATA_H
#define GENERICPERSISTENTDATA_H

#include <QApplication>
#include <QDateTime>
#include <QSet>

class GenericPersistentData
{

public:
    virtual ~GenericPersistentData() {}

    virtual int id() = 0;
    virtual void setId(const int newId) = 0;
    // ---
    virtual QString uuid() = 0;
    virtual void setUuid(const QString &newUuid) = 0;
    // ---
    virtual QString name() = 0;
    virtual void setName(const QString &newName) = 0;
    // ---
    virtual QString description() = 0;
    virtual void setDescription(const QString &newDescription) = 0;
    // ---
    virtual void setPayload(const QString &newPayload) = 0;
    virtual QString payload() = 0;

    virtual QString type() = 0;
    virtual void setType(const QString &value) = 0;

    virtual QSet<QString> &tags() = 0;
    virtual void setTags(const QSet<QString> &newTags) = 0;

    virtual void setUpdateDate(const QDateTime date) = 0;
    virtual void setCreationUser(const QString &user) = 0;
    virtual void setCreationDate(const QDateTime date) = 0;

    virtual QString updateUser() = 0;
    virtual QDateTime updateDate() = 0;
    virtual QString creationUser() = 0;
    virtual QDateTime creationDate() = 0;

    virtual bool isReadOnly() = 0;
    virtual void setReadOnly(const bool newReadOnly) = 0;
    virtual bool isEqual(GenericPersistentData *other, QString *field) = 0;
};

#endif // GENERICPERSISTENTDATA_H
