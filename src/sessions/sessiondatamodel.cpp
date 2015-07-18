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

#include "sessiondatamodel.h"

#define FOLDERICON  ":/mime/folder"
#define XSDICON  ":/mime/xsd"
#define XMLICON  ":/mime/xml"
#define GENERICICON  ":/mime/generic"

SessionDataModel::SessionDataModel(QObject *parent) :
    QAbstractItemModel(parent),
    _folderIcon(FOLDERICON),
    _xsdIcon(XSDICON),
    _xmlIcon(XMLICON),
    _genericIcon(GENERICICON)
{
    _data = NULL ;
}

SessionDataModel::~SessionDataModel()
{
    ;
}

void SessionDataModel::setData(SessionSummary* value)
{
    _data = value ;
}

void SessionDataModel::deleteData()
{
    if(NULL != _data) {
        SessionSummary* currentData = _data ;
        _data = NULL ;
        delete currentData;
    }
}

void SessionDataModel::setFont(const QFont font)
{
    _categoryFont = font ;
    _categoryFont.setBold(false);
    _categoryFont.setPointSize(_categoryFont.pointSize() + 2);
}

BaseSessionDataInfo *SessionDataModel::getGenericItem(const QModelIndex & index)
{
    BaseSessionDataInfo *element = static_cast<BaseSessionDataInfo*>(index.internalPointer());
    return element ;
}

QVariant SessionDataModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }
    BaseSessionDataInfo *element = static_cast<BaseSessionDataInfo*>(index.internalPointer());
    if(NULL != element) {
        switch(role) {
        case Qt::ToolTipRole:
            return QVariant(element->tooltip());
            break;
        case Qt::DisplayRole:
            return QVariant(element->displayName());
            /*if( element->type() == BaseSessionDataInfo::TYPE_FILE ) {
                SessionFileAccess *fileAccess = static_cast<SessionFileAccess*>(element);
                QFileInfo info(fileAccess->path());
                return QVariant(info.fileName());
            } else if( element->type() == BaseSessionDataInfo::TYPE_FOLDER ) {
                / *SessionDirectoryAccess *folderAccess = static_cast<SessionDirectoryAccess*>(element);
                QFileInfo info(folderAccess->path());
                return QVariant(info.fileName());* /
                return QVariant("TODO");
            } else if( element->type() == BaseSessionDataInfo::TYPE_CATEGORY ) {
                SessionCategoryAccess *category = static_cast<SessionCategoryAccess*>(element);
                return category->name();
            }*/
            break;
        case Qt::FontRole:
            if(element->type() == BaseSessionDataInfo::TYPE_CATEGORY) {
                return _categoryFont;
            }
            break;
        case Qt::DecorationRole: {
            if(element->type() == BaseSessionDataInfo::TYPE_FILE) {
                SessionFileAccess *fileAccess = static_cast<SessionFileAccess*>(element);
                QString path = fileAccess->path();
                if(path.endsWith(".xml", Qt::CaseInsensitive)) {
                    return _xmlIcon;
                }
                if(path.endsWith(".xsd", Qt::CaseInsensitive)) {
                    return _xsdIcon;
                }
                return _genericIcon;
            } else if(element->type() == BaseSessionDataInfo::TYPE_FOLDER) {
                return  _folderIcon ;
            }
        }
        break;
        default:
            break;
        } // switch displayRole
    }
    return QVariant();
}

Qt::ItemFlags SessionDataModel::flags(const QModelIndex &index) const
{
    if(!index.isValid()) {
        return QAbstractItemModel::flags(index);
    }
    BaseSessionDataInfo *element = static_cast<BaseSessionDataInfo*>(index.internalPointer());
    if(NULL != element) {
        if(element->type() == BaseSessionDataInfo::TYPE_CATEGORY) {
            return Qt::ItemIsEnabled ;
        }
    }
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled ;
}

QVariant SessionDataModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int /*role*/) const
{
    return tr("Session data");
}

QModelIndex SessionDataModel::index(int row, int column, const QModelIndex &parent) const
{
    if((row < 0) || (column > 0)  || (parent.isValid() && (parent.column() > 0))) {
        return QModelIndex();
    }
    BaseSessionDataInfo *d = NULL;
    if(!parent.isValid()) {
        d = _data->categories()->at(row);
        if(NULL != d) {
            return createIndex(row, column, d);
        }
    } else {
        BaseSessionDataInfo* parentBaseElement = static_cast<BaseSessionDataInfo*>(parent.internalPointer());
        if(parentBaseElement->type() == BaseSessionDataInfo::TYPE_CATEGORY) {
            SessionCategoryAccess* parentElement = static_cast<SessionCategoryAccess*>(parentBaseElement);
            BaseSessionDataInfo* element = parentElement->getChildAt(row);
            if(NULL != element) {
                return createIndex(row, column, element);
            }
        }
    }
    return QModelIndex();
}

int SessionDataModel::rowCount(const QModelIndex &parent) const
{
    if(parent.column() > 0) {
        return 0;
    }
    if(NULL == _data) {
        return 0;
    }

    int resRowCount = 0 ;
    if(!parent.isValid()) {
        resRowCount = _data->categories()->size();
    } else {
        BaseSessionDataInfo *parentElement;
        parentElement = static_cast<BaseSessionDataInfo*>(parent.internalPointer());
        if(NULL != parentElement) {
            if(parentElement->type() != BaseSessionDataInfo::TYPE_CATEGORY) {
                return 0;
            }
            SessionCategoryAccess *category = static_cast<SessionCategoryAccess*>(parent.internalPointer());
            resRowCount = category->getChildItemsNumber();
        }
    }
    return resRowCount ;
}

int SessionDataModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 1;
}

QModelIndex SessionDataModel::parent(const QModelIndex &index) const
{
    if(index.isValid()) {
        BaseSessionDataInfo *element = static_cast<BaseSessionDataInfo*>(index.internalPointer());
        if(NULL != element) {
            BaseSessionDataInfo *parentElement = element->parent() ;
            if(NULL != parentElement) {
                return createIndex(parentElement->index(), 0, parentElement);
            }
        }
    }
    return QModelIndex();
}


//------------------------------------------------------------------------------------------------------------------------------------
