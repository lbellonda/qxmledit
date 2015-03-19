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


#ifndef GENERICPERSISTENTDBDATA_H
#define GENERICPERSISTENTDBDATA_H

#include <data/GenericPersistentData.h>
#include <QStringList>
#include <QDateTime>

class GenericPersistentDBData : public GenericPersistentData
{
    int _id;
    QString _uuid;
    QString _name;
    QString _description;
    QSet<QString> _tags;
    QString _payload;
    QDateTime _creationDate;
    QDateTime _updateDate;
    QString _author;
    QString _updateUser;
    QString _fileName;
    bool _isReadOnly;
    QString _type;

private:
    void setFieldName(QString *field, const QString &fieldName);
    bool compareTags(GenericPersistentData *other);

public:
    GenericPersistentDBData();
    ~GenericPersistentDBData();

    //---
    virtual int id();
    virtual void setId(const int newId);
    // ---
    virtual QString uuid();
    virtual void setUuid(const QString &newUuid);
    // ---
    virtual QString name();
    virtual void setName(const QString &newName);
    // ---
    virtual QString description();
    virtual void setDescription(const QString &newDescription);
    // ---
    virtual void setPayload(const QString &newPayload);
    virtual QString payload();

    virtual QString type();
    virtual void setType(const QString &value);

    virtual QSet<QString> &tags();
    virtual void setTags(const QSet<QString> &newTags);

    virtual void setUpdateDate(const QDateTime date);
    virtual void setCreationUser(const QString &user);
    virtual void setCreationDate(const QDateTime date);

    virtual QString updateUser();
    virtual QDateTime updateDate();
    virtual QString creationUser();
    virtual QDateTime creationDate();

    virtual bool isReadOnly();
    virtual void setReadOnly(const bool newReadOnly);

    virtual bool isEqual(GenericPersistentData *other, QString *field);
};

#endif // GENERICPERSISTENTDBDATA_H
