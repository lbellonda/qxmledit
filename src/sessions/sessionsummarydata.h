/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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


#ifndef SESSIONSUMMARYDATA_H
#define SESSIONSUMMARYDATA_H

#include "QXmlEditSessions_global.h"

#include <QDateTime>

class QXMLEDITSESSIONSSHARED_EXPORT BaseSessionDataInfo : public QObject
{
    Q_OBJECT
    BaseSessionDataInfo *_parent;
    int _index ;
public:
    BaseSessionDataInfo();
    ~BaseSessionDataInfo();

    BaseSessionDataInfo *parent();
    void setParent(BaseSessionDataInfo *newParent);
    virtual QString tooltip() = 0 ;
    virtual QString displayName() = 0;
    enum SDInfo {
        TYPE_CATEGORY,
        TYPE_FOLDER,
        TYPE_FILE
    };
    virtual SDInfo type() = 0;
    int index();
    void setIndex(const int value);
};

class QXMLEDITSESSIONSSHARED_EXPORT SessionDirectoryAccess : public BaseSessionDataInfo
{
    QString _path;
    QDateTime _lastAccess;
    int _times;
    //QStringList tags;
public:
    SessionDirectoryAccess();
    ~SessionDirectoryAccess();
    BaseSessionDataInfo::SDInfo type();
    virtual QString tooltip();
    virtual QString displayName();
    QString path();
    void setPath(const QString& value);
    QDateTime lastAccess();
    void setLastAccess(const QDateTime value);
};


class QXMLEDITSESSIONSSHARED_EXPORT SessionFileAccess : public BaseSessionDataInfo
{
    QString _path;
    QDateTime _lastAccess;
    qint64 _size;
    int _times;
    //QStringList _tags;
public:
    SessionFileAccess();
    ~SessionFileAccess();
    BaseSessionDataInfo::SDInfo type();

    QString path();
    void setPath(const QString& value);
    QDateTime lastAccess();
    void setLastAccess(const QDateTime value);
    qint64 size();
    void setSize(const qint64 value);
    int times();
    void setTimes(const int value);
    //---
    virtual QString tooltip();
    virtual QString displayName();
};

class QXMLEDITSESSIONSSHARED_EXPORT SessionCategoryAccess : public BaseSessionDataInfo
{
    QList<BaseSessionDataInfo*> _children;
    QString _name;
public:
    SessionCategoryAccess();
    ~SessionCategoryAccess();
    SDInfo type();
    QString name();
    void setName(const QString& value);
    void addChild(BaseSessionDataInfo* newChild);
    //--------------------
    virtual QString tooltip();
    virtual QString displayName();

    BaseSessionDataInfo* getChildAt(const int row);
    int getChildItemsNumber();
};



#endif // SESSIONSUMMARYDATA_H
