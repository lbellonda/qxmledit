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


#ifndef TAGMARKER_H
#define TAGMARKER_H

#include <QGraphicsEllipseItem>
#include <QVector2D>
#include <QGraphicsDropShadowEffect>

class TagNode;
class NodesRelationsController;

class TagMarker : public QGraphicsEllipseItem
{

public:
    TagNode *tagNode;
    NodesRelationsController *controller;
    //---------------------------------------------------------------
    bool dragged ;
    int radius;
    //---------------------------------------------------------------
    QVector2D originalPosition;
    QVector2D position;
    QVector2D velocity;
    QVector2D acceleration;
    //---------------------------------------------------------------
    QGraphicsDropShadowEffect *effect;

    TagMarker(TagNode *newNode, NodesRelationsController *newController, QGraphicsItem *parent = 0);
    virtual ~TagMarker();

    virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);

    void enableMove(const bool isMovable);
};


#endif // TAGMARKER_H
