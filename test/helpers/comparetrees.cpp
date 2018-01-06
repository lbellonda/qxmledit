/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013-2018 by Luca Bellonda and individual contributors  *
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

#include "comparetrees.h"
#include <QString>

CompareTrees::CompareTrees()
{
}

CompareTrees::~CompareTrees()
{
}


bool CompareTrees::compareTrees(QTreeWidget *treeLeft, QTreeWidget *treeRight)
{
    int itemsLeft = treeLeft->topLevelItemCount();
    int itemsRight = treeRight->topLevelItemCount();
    if( itemsRight != itemsLeft ) {
        _error = QString("top level items differs: left:%1, right:%2").arg(itemsLeft).arg(itemsRight);
        return false;
    }
    QString path ="/";
    for( int i = 0 ; i < itemsLeft ; i ++ ) {
        QString itemPath = QString("%1/%2").arg(path).arg(i);
        if(! compareItems(itemPath, treeLeft->topLevelItem(i), treeRight->topLevelItem(i))) {
            return false;
        }
    }
    return true;
}


bool CompareTrees::compareItems(const QString &path, QTreeWidgetItem *itemLeft, QTreeWidgetItem *itemRight)
{
    if( itemLeft->text(0) != itemRight->text(0) ) {
        _error = QString("items differ: path:%1 left:'%2', right:'%3'").arg(path).arg(itemLeft->text(0)).arg(itemRight->text(0));
        return false;
    }

    int itemsLeft = itemLeft->childCount();
    int itemsRight = itemRight->childCount();
    if( itemsRight != itemsLeft ) {
        _error = QString("children items differs: path:%1 left:%2, right:%3").arg(path).arg(itemsLeft).arg(itemsRight);
        return false;
    }
    for( int i = 0 ; i < itemsLeft ; i ++ ) {
        QString itemPath = QString("%1/%2").arg(path).arg(i);
        if(! compareItems(itemPath, itemLeft->child(i), itemRight->child(i))) {
            return false;
        }
    }
    return true;
}

QString CompareTrees::error()
{
    return _error ;
}
