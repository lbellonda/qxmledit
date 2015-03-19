/***************************************************************************
 *   This file is part of qaccessorsgen                                    *
 *   Copyright (C) 2011 by Luca Bellonda                                   *
 *   lbellonda _at_ gmail.com                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *    Boston, MA  02110-1301, USA.                                         *
 ***************************************************************************/


#ifndef SESSIONDATAMODEL_H
#define SESSIONDATAMODEL_H

#include <QAbstractItemModel>
#include <QFileIconProvider>

/*
class BaseSessionDataInfo {

public:
    BaseSessionDataInfo();
    ~BaseSessionDataInfo();

    enum SDInfo {
      TYPE_CATEGORY,
      TYPE_FOLDER,
      TYPE_FILE
    };
    SDInfo type()=0;
};

class SessionDirectoryAccess : public BaseSessionDataInfo {
    QString path;
    QDateTime lastAccess;
    QStringList tags;
public:
    SessionDirectoryAccess();
    ~SessionDirectoryAccess();
    SDInfo type();
};


class SessionFileAccess : public BaseSessionDataInfo {
    QString filePath;
    QDateTime lastAccess;
    qint64 size;
    QStringList tags;
public:
    SessionFileAccess();
    ~SessionFileAccess();
    SDInfo type();
};

class SessionCategoryAccess : public BaseSessionDataInfo {
    QList<BaseSessionDataInfo*> children;
    int totalItems;
public:
    SessionCategoryAccess();
    ~SessionCategoryAccess();
    SDInfo type();
};
*/
#include "sessionsummary.h"

class SessionDataModel : public QAbstractItemModel
{
    Q_OBJECT
    SessionSummary* _data;
    //QFileIconProvider _iconProvider; TODO delete
    QFont _categoryFont;
    QIcon _folderIcon;
    QIcon _xsdIcon;
    QIcon _xmlIcon;
    QIcon _genericIcon;
public:
    explicit SessionDataModel(QObject *parent = 0);
    virtual ~SessionDataModel();

    //---------------------------------- Model Interface
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;

    //---------------------------------- end Model Interface
    using QAbstractItemModel::setData;
    void setData(SessionSummary* value);
    void deleteData();
    void setFont(const QFont font);
    BaseSessionDataInfo *getGenericItem(const QModelIndex & index);

signals:

public slots:

};

#endif // SESSIONDATAMODEL_H
