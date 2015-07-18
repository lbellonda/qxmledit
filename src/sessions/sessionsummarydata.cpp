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


#include "sessionsummarydata.h"
#include <QFileInfo>

//------------------------------------------------------------------------------------------------------------------------------------

BaseSessionDataInfo::BaseSessionDataInfo()
{
    _parent = NULL;
    _index = 0;
}

BaseSessionDataInfo::~BaseSessionDataInfo() {}

BaseSessionDataInfo *BaseSessionDataInfo::parent()
{
    return _parent ;
}

void BaseSessionDataInfo::setParent(BaseSessionDataInfo *newParent)
{
    _parent = newParent ;
}

int BaseSessionDataInfo::index()
{
    return _index ;
}

void BaseSessionDataInfo::setIndex(const int value)
{
    _index = value ;
}

//------------------------------------------------------------------------------------------------------------------------------------

SessionCategoryAccess::SessionCategoryAccess()
{
}

SessionCategoryAccess::~SessionCategoryAccess()
{
    foreach(BaseSessionDataInfo * child, _children) {
        delete child;
    }
    _children.clear();
}

BaseSessionDataInfo::SDInfo SessionCategoryAccess::type()
{
    return TYPE_CATEGORY ;
}

QString SessionCategoryAccess::name()
{
    return _name ;
}


void SessionCategoryAccess::setName(const QString& value)
{
    _name = value ;
}

void SessionCategoryAccess::addChild(BaseSessionDataInfo* newChild)
{
    newChild->setParent(this);
    _children.append(newChild);
    newChild->setIndex(_children.size());
}

BaseSessionDataInfo* SessionCategoryAccess::getChildAt(const int row)
{
    return _children.at(row);
}

QString SessionCategoryAccess::tooltip()
{
    return QString(tr("CATEGORY"));
}

QString SessionCategoryAccess::displayName()
{
    return _name;
}

int SessionCategoryAccess::getChildItemsNumber()
{
    return _children.size();
}

//------------------------------------------------------------------------------------------------------------------------------------

SessionFileAccess::SessionFileAccess()
{
    _times = 0;
    _size = 0 ;
}

SessionFileAccess::~SessionFileAccess()
{
}

BaseSessionDataInfo::SDInfo SessionFileAccess::type()
{
    return TYPE_FILE ;
}

QString SessionFileAccess::path()
{
    return _path ;
}


void SessionFileAccess::setPath(const QString& value)
{
    _path = value ;
}

QDateTime SessionFileAccess::lastAccess()
{
    return _lastAccess ;
}

void SessionFileAccess::setLastAccess(const QDateTime value)
{
    _lastAccess = value ;
}

qint64 SessionFileAccess::size()
{
    return _size ;
}


void SessionFileAccess::setSize(const qint64 value)
{
    _size = value ;
}

int SessionFileAccess::times()
{
    return _times ;
}


void SessionFileAccess::setTimes(const int value)
{
    _times = value ;
}


QString SessionFileAccess::tooltip()
{
    QFileInfo info(_path);
    qint64 fileSize = info.size();
    QDateTime dtLastMod = info.lastModified();
    QString dateMod = dtLastMod.toString(Qt::DefaultLocaleLongDate);
    QString dateAccessed = _lastAccess.toString(Qt::DefaultLocaleLongDate);

    return QString(tr("%1\n  last accessed on %2\n  last updated on %3\n  size %4\n  accessed %5 times")).arg(_path).arg(dateAccessed).arg(dateMod).arg(fileSize).arg(_times);
}

QString SessionFileAccess::displayName()
{
    QFileInfo info(path());
    return QString(tr("%1 (%2)")).arg(info.fileName()).arg(_times);
}

//------------------------------------------------------------------------------------------------------------------------------------

SessionDirectoryAccess::SessionDirectoryAccess() : BaseSessionDataInfo()
{
    _times = 0 ;
}

SessionDirectoryAccess::~SessionDirectoryAccess()
{
}

BaseSessionDataInfo::SDInfo SessionDirectoryAccess::type()
{
    return TYPE_FOLDER ;
}

QString SessionDirectoryAccess::tooltip()
{
    QString dateAccessed = _lastAccess.toString(Qt::DefaultLocaleLongDate);
    return QString(tr("%1\n  last access: %2")).arg(_path).arg(dateAccessed);
}

QString SessionDirectoryAccess::displayName()
{
    //TODO
    return _path;
}


QString SessionDirectoryAccess::path()
{
    return _path ;
}

void SessionDirectoryAccess::setPath(const QString& value)
{
    _path = value ;
}

QDateTime SessionDirectoryAccess::lastAccess()
{
    return _lastAccess ;
}

void SessionDirectoryAccess::setLastAccess(const QDateTime value)
{
    _lastAccess = value ;
}

