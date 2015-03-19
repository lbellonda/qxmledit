/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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


#ifndef TAGNODES_H
#define TAGNODES_H

#include <QDialog>
#include <QGraphicsEllipseItem>
#include <QVector2D>
#include "libQXmlEdit_global.h"

class NodesRelationsDialog;
class NodesRelationsController ;

class LIBQXMLEDITSHARED_EXPORT TagNodeTarget
{
public:
    QString tag;
    int count;
    TagNodeTarget(const QString &newTag);
    ~TagNodeTarget();
};

//----------------------------------------------------------------------------------------------------------------

class LIBQXMLEDITSHARED_EXPORT TagNode
{
public:
    QString tag;
    int id;
    int count;
    int linksIn;
    int linksOut;
    QHash<QString, TagNodeTarget*> targets;
    TagNode(const QString &newTag, const int newId);
    ~TagNode();
};

class TagNode2
{
public:
    QString tag;
    int id;
    QHash<QString, TagNodeTarget*> targets;
    QVector2D originalPosition;
    QVector2D position;
    QVector2D velocity;
    QVector2D acceleration;
    QGraphicsEllipseItem *item ;
};



#endif // TAGNODES_H
