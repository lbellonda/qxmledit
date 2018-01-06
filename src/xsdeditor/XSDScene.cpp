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

#include "xsdwindow.h"
#include "utils.h"

XSDScene::XSDScene()
{
    _bkConfig.load();
    /*
    // a gradient background TODO
    QLinearGradient gradient(QPoint(0,0), QPoint(0,100));
    gradient.setColorAt(0, QColor(255, 255, 255));
    gradient.setColorAt(1, QColor(220, 220, 220));
    gradient.setSpread(QGradient::ReflectSpread);
    setBackgroundBrush(gradient);
    */
}

XSDScene::~XSDScene()
{
}

void XSDScene::updateBounds()
{
    QRectF nullRect;
    setSceneRect(nullRect);
    QRectF itemsRect = itemsBoundingRect();
    itemsRect.setWidth(itemsRect.width() + 5);
    itemsRect.setHeight(itemsRect.height() + 5);
    setSceneRect(itemsRect);
    applyBackground(&_bkConfig);
    /**************************************** TODO
        //QLinearGradient gradient(QPoint(0,0), QPoint(bounds.width()/4,0));
        QLinearGradient gradient(QPoint(0,0), QPoint(100, 0));
        //gradient.setColorAt(0, QColor(250, 250, 250));
        gradient.setColorAt(0, QColor(230, 230, 225));
        gradient.setColorAt(1, QColor(240, 240, 235));
        gradient.setSpread(QGradient::ReflectSpread);
        setBackgroundBrush(gradient);
    ************************************/

}

void XSDScene::gotoItem(QGraphicsItem *item)
{
    if(NULL == item) {
        return ;
    }
    setFocusItem(item);
    item->setSelected(true);
    QPointF itemPos = item->scenePos();
    foreach(QGraphicsView * view, views()) {
        view->centerOn(itemPos);
    }
}


void XSDScene::calculateGradientPoints(XSDGraphicsBackgroundConfiguration *bkgConfig, QPoint &startPoint, QPoint &endPoint)
{
    startPoint.setX(0);
    startPoint.setY(0);
    endPoint.setX(0);
    endPoint.setY(0);
    int endX = 0;
    int endY = 0;
    QRectF boundsRect = sceneRect();

    if(bkgConfig->gradientUnits() == XSDGraphicsBackgroundConfiguration::GradientUnitsPixels) {
        endX = bkgConfig->gradientHeight();
        endY = bkgConfig->gradientHeight();
    } else {
        float fw = boundsRect.width();
        if(0 == fw) {
            fw = 1 ;
        }
        float fh = boundsRect.height();
        if(0 == fh) {
            fh = 1 ;
        }
        endX = (int)(100.0 * bkgConfig->gradientHeight() / fw) ;
        endY = (int)(100.0 * bkgConfig->gradientHeight() / fh) ;
    }
    if(bkgConfig->gradientType() == XSDGraphicsBackgroundConfiguration::GradientRadial) {
        endPoint.setY(endY);
        endPoint.setX(endX);
    } else {

        switch(bkgConfig->gradientDirection()) {
        default:
        case XSDGraphicsBackgroundConfiguration::DirectionVertical:
            endPoint.setY(endY);
            break;
        case XSDGraphicsBackgroundConfiguration::DirectionHorizontal:
            endPoint.setX(endX);
            break;
        case XSDGraphicsBackgroundConfiguration::DirectionDiagonal:
            endPoint.setY(endY);
            endPoint.setX(endX);
            break;
        }
    }
}

void XSDScene::configureAndSetGradient(XSDGraphicsBackgroundConfiguration *bkgConfig, QGradient &gradient)
{
    gradient.setColorAt(0, bkgConfig->mainColor());
    gradient.setColorAt(1, bkgConfig->alternateColor());
    gradient.setSpread(QGradient::ReflectSpread);
    setBackgroundBrush(gradient);
}

void XSDScene::setBackgroundCfg(XSDGraphicsBackgroundConfiguration *bkgConfig)
{
    if(NULL == bkgConfig) {
        Utils::error(tr("Invalid scene background configuration."));
        return ;
    }
    bkgConfig->copyTo(&_bkConfig);
    applyBackground(&_bkConfig);
}

void XSDScene::applyBackground(XSDGraphicsBackgroundConfiguration *bkgConfig)
{
    if(NULL == bkgConfig) {
        Utils::error(tr("Invalid scene background configuration."));
        return ;
    }
    switch(bkgConfig->gradientType()) {
    default:
    case XSDGraphicsBackgroundConfiguration::GradientSolid: {
        QBrush solidBrush(bkgConfig->mainColor());
        setBackgroundBrush(solidBrush);
        break;
    }
    case XSDGraphicsBackgroundConfiguration::GradientLinear: {
        QPoint startPoint, endPoint;
        calculateGradientPoints(bkgConfig, startPoint, endPoint);
        QLinearGradient gradient(startPoint, endPoint);

        configureAndSetGradient(bkgConfig, gradient);

    }
    break;
    case XSDGraphicsBackgroundConfiguration::GradientRadial: {
        QPoint startPoint, endPoint;
        calculateGradientPoints(bkgConfig, startPoint, endPoint);
        QRadialGradient gradient(startPoint, endPoint.y());

        configureAndSetGradient(bkgConfig, gradient);
    }
    break;
    }

}
/*
    //QLinearGradient gradient(QPoint(0,0), QPoint(bounds.width()/4,0));
    QLinearGradient gradient(QPoint(0,0), QPoint(100, 0));
    //gradient.setColorAt(0, QColor(250, 250, 250));
    gradient.setColorAt(0, QColor(230, 230, 225));
    gradient.setColorAt(1, QColor(240, 240, 235));
    gradient.setSpread(QGradient::ReflectSpread);
    setBackgroundBrush(gradient);

*/

