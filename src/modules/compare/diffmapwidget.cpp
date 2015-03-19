/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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


#include "diffmapwidget.h"
#include "ui_diffmapwidget.h"
#include "utils.h"
#include "comparechrome.h"

#include <QPainter>
#include <QList>
#include <QMouseEvent>

DiffMapWidget::DiffMapWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DiffMapWidget)
{
    _data = NULL ;
    _newData = false;
    _selection = -1 ;
    ui->setupUi(this);
}

DiffMapWidget::~DiffMapWidget()
{
    delete ui;
}

void DiffMapWidget::setDiffList(QList<QTreeWidgetItem*> *newData)
{
    _data = newData;
    if(NULL == _data) {
        _selection = -1 ;
    }
    _newData = true ;
    update();
}

void DiffMapWidget::setSelection(const int newValue)
{
    _selection = newValue ;
    update();
}

void DiffMapWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        if(NULL != _data) {
            QSize theSize = size();

            int viewHeight = theSize.height() ;
            if(viewHeight == 0) {
                viewHeight = 1 ;
            }
            int dataSize = _data->size();
            if(dataSize < 1) {
                dataSize = 1 ;
            }
            int newY = event->pos().y();
            int newSelection = (newY * dataSize) / viewHeight ;
            emit userChangedSelection(newSelection);
        }
    }
}

void DiffMapWidget::paintEvent(QPaintEvent * /*e*/)
{
    generateImage();
    QPainter painter(this);
    painter.drawImage(0, 0, _cachedImage);
    if((_selection != -1) && (NULL != _data)) {
        QSize theSize = size();
        QBrush brush(Qt::NoBrush);
        painter.setBrush(brush);
        painter.setPen(QColor::fromRgb(255, 0, 0));
        int viewHeight = theSize.height() ;
        if(viewHeight == 0) {
            viewHeight = 1 ;
        }
        int dataSize = _data->size();
        if(dataSize < 1) {
            dataSize = 1 ;
        }

        int slotHeight = viewHeight / dataSize ;
        if(slotHeight < 2) {
            slotHeight = 2 ;
        }
        int center = (_selection * viewHeight)  / dataSize ;
        painter.drawRoundedRect(0, center - 1, theSize.width(), slotHeight, 10.0, 10.0, Qt::RelativeSize);
    }
}


void DiffMapWidget::generateImage()
{
    QSize theSize = size();
    if(_newData
            || _cachedImage.isNull()
            || (_cachedImage.size() != theSize)) {
        _newData = false;
        if(NULL != _data) {
            CompareChrome chrome ;
            _cachedImage = QImage(theSize, QImage::Format_RGB32);
            QPainter painter(&_cachedImage);
            int maxX = theSize.width();
            int maxY = theSize.height();
            if(maxY == 0) {
                maxY  = 1 ;
            }

            painter.fillRect(0, 0, maxX, maxY, chrome.equalsColor());

            int dataSize = _data->size();
            if(dataSize < 1) {
                dataSize = 1 ;
            }
            bool useMeanValue = false ;
            if(dataSize  > maxY) {
                useMeanValue = true ;
            }
            int slotHeight = maxY / dataSize ;
            if(slotHeight < 1) {
                slotHeight = 1 ;
            }
            int index = 0;
            int lastY = -1 ;

            /*QColor emptyColor = chrome.emptyColor();
            QColor addedColor = chrome.addedColor(); TODO
            QColor deletedColor = chrome.deletedColor();
            QColor modifiedColor = chrome.modifiedColor();*/
            QColor equalsColor = chrome.equalsColor();

            double factor = (double)maxY / (double)dataSize  ;
            foreach(QTreeWidgetItem * item, *_data) {
                int y = index * factor;

                QColor color = item->backgroundColor(0);
                bool drawColor = true ;
                if(useMeanValue && (lastY == y)) {
                    if(color == equalsColor) {
                        drawColor = false ;
                    }
                }
                if(drawColor) {
                    painter.fillRect(0, y, maxX, slotHeight, color);
                }
                lastY = y ;
                index++;
            }
        }
    }
}
