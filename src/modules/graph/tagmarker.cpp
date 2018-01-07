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


#include "tagmarker.h"
#include "tagnodes.h"
#include "nodesrelationscontroller.h"
#include "tagzorder.h"

TagMarker::TagMarker(TagNode *newNode, NodesRelationsController *newController, QGraphicsItem *parent) : QGraphicsEllipseItem(parent)
{
    dragged = false;
    controller = newController ;
    tagNode = newNode ;
    setFlags(flags() | ItemIsMovable | ItemSendsGeometryChanges);
    radius = 16 ;
    int halfRadius = radius / 2 ;
    setRect(-halfRadius, -halfRadius, radius, radius);
    //-----------------------------------
    QGraphicsTextItem *text = new QGraphicsTextItem(this);
    text->setHtml(QString("<html><body><span style='background-color:#FFF;color:#000;opacity:0.2'>%1</span></body></html>").arg(tagNode->tag));
    text->setPos(0, 0);
    text->setOpacity(0.5);
    //-----------------------------------
    setBrush(QBrush(QColor::fromRgb(255, 0, 0)));
    setToolTip(tagNode->tag);
    text->setToolTip(tagNode->tag);
    //-----------------------------------
    QRectF textBounds = text->boundingRect();
    text->setPos(-textBounds.width() / 2, -(4 + textBounds.height()));
    text->setZValue(TagsConst::TextZIndex);
    setZValue(TagsConst::MarkerZIndex);
    //-----------------------------------
    effect = new QGraphicsDropShadowEffect(NULL);
    setGraphicsEffect(effect);
}

TagMarker::~TagMarker()
{
    if(NULL != effect) {
        delete effect;
    }
}

QVariant TagMarker::itemChange(GraphicsItemChange change, const QVariant & value)
{
    if(change == ItemPositionChange) {
        if(dragged) {
            QPoint point = value.toPoint();
            position.setX(point.x());
            position.setY(point.y());
            controller->startTimer();
        }
    }
    return QGraphicsEllipseItem::itemChange(change, value);
}

//non spostare se non draggato
void TagMarker::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

void TagMarker::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    dragged = true ;
    QGraphicsItem::mousePressEvent(event);
    controller->onMarkerClicked(this);
}

void TagMarker::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    dragged = false ;
    QGraphicsItem::mouseReleaseEvent(event);
}

void TagMarker::enableMove(const bool isMovable)
{
    if(isMovable) {
        setFlags(flags() | ItemIsMovable | ItemSendsGeometryChanges);
    } else {
        setFlags(flags() & ~ItemIsMovable);
    }
}
