/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
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
#include "xmlEdit.h"
#include "balsamiqtree.h"
#include <stdio.h>
#include <QDomDocument>
#include "utils.h"


//--------------------------------------------------------------------------------------------------------------


BalsamiqDataTree::BalsamiqDataTree()
{
    _actual = NULL ;
    _level = 0 ;
}

BalsamiqDataTree::~BalsamiqDataTree()
{
    foreach(BalsamiqDataTreeItem * item, _children) {
        delete item;
    }
    _children.clear();
}

BalsamiqDataTreeItem::BalsamiqDataTreeItem()
{
    _level = 0 ;
    _parent = NULL ;
}

BalsamiqDataTreeItem::~BalsamiqDataTreeItem()
{
    foreach(BalsamiqDataTreeItem * item, _children) {
        delete item;
    }
    _children.clear();
}

QString BalsamiqDataTreeItem::text()
{
    return _text ;
}

void BalsamiqDataTreeItem::setText(const QString &newText)
{
    _text = newText ;
}

int BalsamiqDataTreeItem::level()
{
    return _level ;
}

void BalsamiqDataTreeItem::setLevel(const int newLevel)
{
    _level = newLevel ;
}

BalsamiqDataTreeItem *BalsamiqDataTreeItem::parent()
{
    return _parent ;
}

void BalsamiqDataTreeItem::setParent(BalsamiqDataTreeItem *newParent)
{
    _parent = newParent ;
}

QVector<BalsamiqDataTreeItem *> &BalsamiqDataTreeItem::children()
{
    return _children ;
}

void BalsamiqDataTreeItem::appendChild(BalsamiqDataTreeItem *newChild)
{
    _children.append(newChild);
}

//--------------------------------------------------------------------------------------------------------------

QVector<BalsamiqDataTreeItem *> &BalsamiqDataTree::children()
{
    return _children ;
}


bool BalsamiqDataTree::scanText(const QString &text)
{
    QStringList stringList = text.split('\n');
    foreach(QString str, stringList) {
        BalsamiqDataTreeItem *newItem = new BalsamiqDataTreeItem();
        if(NULL == newItem) {
            return false ;
        }
        newItem->setText(str);
        examineItem(newItem);
    }
    return true;
}

//--------------------------------------------------------------------------------------------------------------

int BalsamiqDataTree::countSpaces(BalsamiqDataTreeItem *item)
{
    QString str = item->text() ;
    int result = - 0 ;
    int len = str.length();
    for(int i = 0 ; i < len ; i ++) {
        QChar ch = str.at(i);
        if(ch == '.') {
            result ++ ;
        } else {
            break ;
        }
    }
    item->setText(item->text().mid(result));
    return result ;
}

void BalsamiqDataTree::insertRoot(BalsamiqDataTreeItem *newItem, const int newLevel)
{
    _children.append(newItem);
    setNewItem(newItem, newLevel, NULL);
    setNewActual(newItem);
}

void BalsamiqDataTree::setNewActual(BalsamiqDataTreeItem *newItem)
{
    _actual = newItem;
    _level = newItem->level() ;
}

void BalsamiqDataTree::setNewItem(BalsamiqDataTreeItem *newItem, const int newLevel, BalsamiqDataTreeItem *newParent)
{
    newItem->setLevel(newLevel);
    newItem->setParent(newParent);
}

void BalsamiqDataTree::examineItem(BalsamiqDataTreeItem *newItem)
{
    int thisLevel = countSpaces(newItem);

    if(NULL == _actual) {
        insertRoot(newItem, thisLevel);
    } else {
        if(thisLevel > _actual->level()) {
            //recursive call up to first available position
            BalsamiqDataTreeItem *item = _actual;
            while((NULL != item) && (thisLevel > item->level())) {
                item = item->parent();
            }
        }
        if(NULL == _actual) {
            insertRoot(newItem, thisLevel);
        } else {
            // item NULL oppure minore o uguale di livello
            if(thisLevel > _actual->level()) {
                _actual->appendChild(newItem);
                setNewItem(newItem, thisLevel, _actual);
                setNewActual(newItem);
            } else if(thisLevel == _actual->level()) {
                BalsamiqDataTreeItem *newParent = _actual->parent() ;
                if(NULL != newParent) {
                    newParent->appendChild(newItem);
                    setNewItem(newItem, thisLevel, newParent);
                    setNewActual(newItem);
                } else {
                    insertRoot(newItem, thisLevel);
                }
            }
        }
    }
}

