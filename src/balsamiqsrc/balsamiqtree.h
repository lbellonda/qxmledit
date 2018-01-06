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

#ifndef BALSAMIQTREE_H
#define BALSAMIQTREE_H

#include <QString>

class BalsamiqDataTreeItem
{

    QString _text;
    int _level;
    BalsamiqDataTreeItem *_parent;
    QVector<BalsamiqDataTreeItem *> _children;
public:
    BalsamiqDataTreeItem();
    ~BalsamiqDataTreeItem();

    QString text();
    void setText(const QString &newText);
    int level();
    void setLevel(const int newLevel);
    BalsamiqDataTreeItem *parent();
    void setParent(BalsamiqDataTreeItem *newParent);
    QVector<BalsamiqDataTreeItem *> &children();
    void appendChild(BalsamiqDataTreeItem *newChild);

};

class BalsamiqDataTree
{

    QVector<BalsamiqDataTreeItem *> _children;
    BalsamiqDataTreeItem *_actual;
    int _level;

    void examineItem(BalsamiqDataTreeItem* newItem);
    void setNewItem(BalsamiqDataTreeItem *newItem, const int newLevel, BalsamiqDataTreeItem *newParent);
    void setNewActual(BalsamiqDataTreeItem *newItem);
    void insertRoot(BalsamiqDataTreeItem *newItem, const int newLevel);
    int countSpaces(BalsamiqDataTreeItem *item);

public:
    BalsamiqDataTree();
    ~BalsamiqDataTree();

    bool scanText(const QString &text);
    QVector<BalsamiqDataTreeItem *> &children();
};

#endif // BALSAMIQTREE_H
