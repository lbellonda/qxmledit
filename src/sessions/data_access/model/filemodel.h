/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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


#ifndef FILEMODEL_H
#define FILEMODEL_H

#include <QString>
#include <QDateTime>
#include <QVector>

class FileModel;

class AccessModel
{
public:
    QDateTime accessDate;
    FileModel *fileModel;

    AccessModel();
    ~AccessModel();
};

class FileModel
{
public:
    int id;
    QString path;
    int starred ;
    QString description;
    QDateTime creationDate;
    QVector<AccessModel*> accesses;

    FileModel();
    ~FileModel();
};

class SessionModel
{
public:
    int id;
    QString name;
    QString description;
    QDateTime creationDate;
    QDateTime updateDate;
    QDateTime lastAccess;
    int starred ;
    bool enabled;
    QVector<FileModel*> files;
    QVector<AccessModel*> accesses;

    SessionModel();
    ~SessionModel();
    void clear();
};


#endif // FILEMODEL_H
