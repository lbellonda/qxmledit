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


#include "models.h"

SessionFilesDataModel::SessionFilesDataModel(QObject *parent): QAbstractTableModel(parent)
{
    _model = NULL;
}

SessionFilesDataModel::~SessionFilesDataModel()
{
}

void SessionFilesDataModel::setModel(SessionModel *model)
{
    _model = model;
}

int SessionFilesDataModel::rowCount(const QModelIndex & /*parent*/) const
{
    if(NULL != _model) {
        return _model->files.size();
    }
    return 0;
}

int SessionFilesDataModel::columnCount(const QModelIndex & /*parent*/) const
{
    return  3;
}

QVariant SessionFilesDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if((role == Qt::DisplayRole) && (orientation == Qt::Horizontal)) {
        switch(section) {
        case 0: return tr("Path");
        case 1: return tr("Description");
        case 2: return tr("Count");
        }
    }
    return QVariant();
}


QVariant SessionFilesDataModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DisplayRole) {
        if((NULL != _model) && index.isValid()) {
            FileModel *file = _model->files.at(index.row());
            switch(index.column()) {
            case 0:
                return file->path;
            case 1:
                return file->description;
            case 2:
                return QString("%1").arg(file->accesses.size());
            }
        }
    } else if(role == Qt::UserRole) {
        return qVariantFromValue((void*)_model->files.at(index.row()));
    }
    return QVariant();
}

//----------------------------------------------------------------------------------------

SessionAccessDataModel::SessionAccessDataModel(QObject *parent): QAbstractTableModel(parent)
{
    _model = NULL;
}

SessionAccessDataModel::~SessionAccessDataModel()
{
}

void SessionAccessDataModel::setModel(SessionModel *model)
{
    _model = model;
}

int SessionAccessDataModel::rowCount(const QModelIndex & /*parent*/) const
{
    if(NULL != _model) {
        return _model->accesses.size();
    }
    return 0;
}

int SessionAccessDataModel::columnCount(const QModelIndex & /*parent*/) const
{
    return  2;
}

QVariant SessionAccessDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if((role == Qt::DisplayRole) && (orientation == Qt::Horizontal)) {
        switch(section) {
        case 0: return tr("Path");
        case 1: return tr("Date");
        }
    }
    return QVariant();
}

QVariant SessionAccessDataModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DisplayRole) {
        if((NULL != _model) && index.isValid()) {
            AccessModel *access = _model->accesses.at(index.row());
            switch(index.column()) {
            case 0:
                return access->fileModel->path;
            case 1: {
                QString sAccess = access->accessDate.toString(Qt::DefaultLocaleShortDate);
                return sAccess;
            }
            }
        }
    } else if(role == Qt::UserRole) {
        return qVariantFromValue((void*)_model->accesses.at(index.row()));
    }
    return QVariant();
}
