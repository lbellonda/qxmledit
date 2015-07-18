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
