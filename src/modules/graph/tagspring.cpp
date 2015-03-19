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


#include "tagspring.h"
#include "tagmarker.h"
#include "tagnodes.h"
#include <QPen>
#include <math.h>
#include "utils.h"
#include "tagzorder.h"

#define COLOR   QColor::fromRgb(0xFF, 0xFF, 0xFF)

TagSpring::TagSpring(TagMarker *newFrom, TagMarker *newTo, double stiffness, double newLength, double damping)
{
    _isVisible = true ;
    arrow1 = NULL ;
    arrow2 = NULL ;
    from = newFrom ;
    to = newTo ;
    setValues(stiffness, newLength, damping);
    line = new QGraphicsLineItem(0, 0, to->position.x() - from->position.x(), to->position.y() - from->position.y(), newFrom);
    line->setZValue(TagsConst::LineZIndex);
    bool isDoubleArrow = false ;
    if(NULL != newTo->tagNode->targets.value(newFrom->tagNode->tag)) {
        isDoubleArrow = true ;
    }

    arrow1 = createArrow(false);
    if(isDoubleArrow) {
        arrow2 = createArrow(true);
    }

    line->setPen(QPen(COLOR));
    //-----------------------------------
    effect = new QGraphicsDropShadowEffect(NULL);
    line->setGraphicsEffect(effect);
}

TagSpring::~TagSpring()
{
    if(NULL != effect) {
        delete effect;
    }
}

void TagSpring::setValues(double stiffness, double newLength, double damping)
{
    length = newLength ;
    k = (1 - damping) * stiffness;
}

void TagSpring::updatePos()
{
    int dx = to->position.x() - from->position.x();
    int dy = to->position.y() - from->position.y();
    line->setLine(0, 0, dx, dy);
    double angleRad = atan2((double)dy, (double)dx) ;
    double angleDeg = angleRad * 180 / 3.142;
    arrow1->setPos(dx - (to->radius * cos(angleRad)), dy - (to->radius * sin(angleRad)));
    arrow1->setRotation(angleDeg);
    if(NULL != arrow2) {
        arrow2->setPos(to->radius * cos(angleRad), to->radius * sin(angleRad));
        arrow2->setRotation(angleDeg);
    }
}

QGraphicsPolygonItem *TagSpring::createArrow(const bool isLeft)
{
    QPolygonF contour ;
    if(isLeft) {
        contour.append(QPointF(0, 0));
        contour.append(QPointF(5, -5));
        contour.append(QPointF(5, 5));
        contour.append(QPointF(0, 0));
    } else {
        contour.append(QPointF(0, 0));
        contour.append(QPointF(-5, -5));
        contour.append(QPointF(-5, 5));
        contour.append(QPointF(0, 0));
    }
    QGraphicsPolygonItem *arrow = new QGraphicsPolygonItem(line);
    arrow->setPolygon(contour);
    arrow->setBrush(QBrush(COLOR));
    arrow->setPen(QPen(COLOR));
    return arrow;
}


void TagSpring::setVisible(const bool value)
{
    _isVisible = value ;
    line->setVisible(_isVisible);
}

bool TagSpring::isVisible()
{
    return _isVisible  ;
}
