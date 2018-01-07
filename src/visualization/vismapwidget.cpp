/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2018 by Luca Bellonda and individual contributors  *
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

#include "vismapwidget.h"
#include "ui_vismapwidget.h"

#include <QPainter>

VisMapWidget::VisMapWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VisMapWidget)
{
    _map = NULL ;
    _newData = false;
    ui->setupUi(this);
}

VisMapWidget::~VisMapWidget()
{
    delete ui;
}


void VisMapWidget::setColorMap(ColorMap *newMap)
{
    _map = newMap;
    _newData = true ;
    update();
}

void VisMapWidget::paintEvent(QPaintEvent * /*e*/)
{
    generateImage();
    QPainter painter(this);
    painter.drawImage(0, 0, _cachedImage);
}

void VisMapWidget::force()
{
    _newData = true ;
}

QImage VisMapWidget::cachedImage()
{
    generateImage();
    return _cachedImage;
}


void VisMapWidget::generateImage()
{
    QSize theSize = size();
    if(_newData
            || _cachedImage.isNull()
            || (_cachedImage.size() != theSize)) {
        _newData = false;
        if(NULL != _map) {
            uint *colors = _map->values();
            _cachedImage = QImage(theSize, QImage::Format_RGB32);
            int maxX = theSize.width();
            int maxY = theSize.height();
            for(int x = 0 ; x < maxX ; x ++) {
                uint color = colors[(int)(((float)x / (float)maxX) * ColorMap::MapElements)];
                for(int y = 0 ; y < maxY ; y++) {
                    _cachedImage.setPixel(x, y, color);
                }
            }
        }
    }
}
