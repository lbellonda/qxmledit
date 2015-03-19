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


#ifndef ATTRFILTERPROFILE_H
#define ATTRFILTERPROFILE_H

#include<QString>
#include<QDateTime>

#include "../QXmlEditSessions_global.h"

class QXMLEDITSESSIONSSHARED_EXPORT AttrFilterProfile
{
    QString _name;
    QString _description;
    QDateTime _creationTime;
    QDateTime _updateTime;
    int _id;
    bool _isWhiteList;

public:
    AttrFilterProfile(const int id, const QString &name, const QString &description, const bool newIsWhiteList, const QDateTime &creationTime, const QDateTime &updateTime);
    AttrFilterProfile();
    ~AttrFilterProfile();

    int id();
    void setId(const int value);
    QString name();
    void setName(const QString& value);
    QString description();
    void setDescription(const QString& value);
    QDateTime creationTime();
    void setCreationTime(const QDateTime &value);
    QDateTime updateTime();
    void setUpdateTime(const QDateTime &value);
    bool isWhiteList();
    void setWhiteList(const bool value);

    bool compareTo(const AttrFilterProfile * other);
    bool compareToBase(const AttrFilterProfile * other);
};

#endif // ATTRFILTERPROFILE_H
