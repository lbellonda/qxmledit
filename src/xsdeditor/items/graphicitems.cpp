/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2023 by Luca Bellonda and individual contributors  *
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


#include "xsdeditor/xsdwindow.h"
#include "utils.h"
#include <QGraphicsTextItem>

GraphicsRectItem::GraphicsRectItem(ItemServiceExecutor *service, QGraphicsItem * parent) : QGraphicsRectItem(parent)
{
    setService(service);
    _isShadow = true;
}

GraphicsRectItem::~GraphicsRectItem()
{
}

void GraphicsRectItem::setService(ItemServiceExecutor *newService)
{
    _service = newService;
}

void GraphicsRectItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if(NULL != _service) {
        _service->execMenuEvent(event, this);
    }
}

QVariant GraphicsRectItem::itemChange(GraphicsItemChange change,
                                      const QVariant &value)
{
    emit itemChanged(change, value);
    return QGraphicsItem::itemChange(change, value);
}

void GraphicsRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget *widget)
{
    QRectF bounds = boundingRect();
    const int OffsetRectX = 3;
    const int OffsetRectY = 3;
    QRectF shadowRect = bounds.translated(OffsetRectX, OffsetRectY);
    drawShadow(painter, shadowRect);
    QGraphicsRectItem::paint(painter, option, widget);
}

void GraphicsRectItem::drawShadow(QPainter *painter, QRectF &bounds)
{
    QLinearGradient shadowGradient;
    shadowGradient.setStart(0, 0);
    shadowGradient.setFinalStop(0, bounds.height());

    QColor colorStart(128, 128, 128);
    QColor colorEnd(82, 82, 82);
    shadowGradient.setColorAt(0, colorStart);
    shadowGradient.setColorAt(1, colorEnd);

    QBrush brush(shadowGradient);
    painter->setBrush(brush);

    QPen pen(Qt::NoPen);
    painter->setPen(pen);

    painter->setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRect(bounds);
    painter->drawPath(path);
}

//-------------------------------------------------------------------------------

GraphicsRoundRectItem::GraphicsRoundRectItem(ItemServiceExecutor *service, QGraphicsItem * parent) : QGraphicsRectItem(parent)
{
    _colorStart = QColor(237, 250, 254);
    _colorMiddle = QColor(205, 241, 254);
    _colorEnd = QColor(237, 250, 254);
    _isOptional = false;
    _isSingleColor = false;
    _useDimShadow = false ;
    _isComplexGradient = false ;
    _isColorBorder = false ;

    setService(service);
}

GraphicsRoundRectItem::~GraphicsRoundRectItem()
{
}

bool GraphicsRoundRectItem::isUseDimShadow() const
{
    return _useDimShadow;
}

void GraphicsRoundRectItem::setUseDimShadow(bool value)
{
    _useDimShadow = value;
}

bool GraphicsRoundRectItem::isSingleColor() const
{
    return _isSingleColor;
}

void GraphicsRoundRectItem::setSingleColor(bool value)
{
    _isSingleColor = value;
    update();
}

bool GraphicsRoundRectItem::isUseColorBorder() const
{
    return _isSingleColor;
}

void GraphicsRoundRectItem::setUseColorBorder(bool value)
{
    _isColorBorder = value;
    update();
}

bool GraphicsRoundRectItem::isOptional() const
{
    return _isOptional;
}

void GraphicsRoundRectItem::setOptional(bool isOptional)
{
    _isOptional = isOptional;
}

void GraphicsRoundRectItem::setService(ItemServiceExecutor *newService)
{
    _service = newService;
}

void GraphicsRoundRectItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if(NULL != _service) {
        _service->execMenuEvent(event, this);
    }
}

QVariant GraphicsRoundRectItem::itemChange(GraphicsItemChange change,
        const QVariant &value)
{
    emit itemChanged(change, value);
    return QGraphicsItem::itemChange(change, value);
}

QColor GraphicsRoundRectItem::colorBorder()
{
    return _colorBorder ;
}

void GraphicsRoundRectItem::setColorBorder(const QColor value)
{
    _colorBorder = value ;
    update();
}

QColor GraphicsRoundRectItem::colorStart()
{
    return _colorStart ;
}

void GraphicsRoundRectItem::setColorStart(const QColor value)
{
    _colorStart = value ;
    update();
}

QColor GraphicsRoundRectItem::colorEnd()
{
    return _colorEnd ;
}

void GraphicsRoundRectItem::setColorEnd(const QColor value)
{
    _colorEnd = value ;
    update();
}

QColor GraphicsRoundRectItem::colorMiddle()
{
    return _colorMiddle ;
}

void GraphicsRoundRectItem::setColorMiddle(const QColor value)
{
    _colorMiddle = value ;
    update();
}

void GraphicsRoundRectItem::drawShadow(QPainter *painter, QRectF &bounds)
{
    QBrush brush(QColor(82, 82, 82));
    painter->setBrush(brush);

    QPen pen(Qt::NoPen);
    painter->setPen(pen);

    if(_useDimShadow) {
        bounds.setHeight(bounds.height() - 2);
        bounds.setWidth(bounds.width() - 2);
    }
    painter->setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(bounds, 10, 10);
    painter->drawPath(path);
}

static int darken(int nuance, int amount)
{
    int value = nuance - amount ;
    if(value < 0) {
        value = 0 ;
    }
    return value ;
}

QColor darkenColor(QColor color, int qty)
{
    int darkenedRed = darken(color.red(), qty);
    int darkenedGreen = darken(color.green(), qty);
    int darkenedBlue = darken(color.blue(), qty);
    return QColor(darkenedRed, darkenedGreen, darkenedBlue);
}

void GraphicsRoundRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem * /*option*/, QWidget */*widget*/)
{
    QRectF bounds = boundingRect();

    bounds.setWidth(bounds.width() - OffsetRectX);
    bounds.setHeight(bounds.height() - OffsetRectY);
    // First gradient: background
    QRectF shadowRect = bounds.translated(OffsetRectX, OffsetRectY);
    drawShadow(painter, shadowRect);

    if(!_isSingleColor) {
        QLinearGradient gradient;
        gradient.setStart(0, 0);
        gradient.setFinalStop(0, bounds.height());
        if(_isComplexGradient) {
            gradient.setColorAt(0, _c0);
            gradient.setColorAt(0.1, _c1);
            gradient.setColorAt(0.35, _c2);
            gradient.setColorAt(0.53, _c3);
            gradient.setColorAt(0.9, _c4);
            gradient.setColorAt(1, _c5);
        } else {
            gradient.setColorAt(0, _colorStart);
            gradient.setColorAt(0.3, _colorMiddle);
            gradient.setColorAt(1, _colorEnd);
        }

        QBrush brush(gradient);
        painter->setBrush(brush);
    } else {
        QBrush brush(_colorStart);
        painter->setBrush(brush);
    }

    if(isOptional()) {
        QPen pen(Qt::DashLine);
        pen.setColor(QColor(0, 0, 120));
        painter->setPen(pen);
    } else {
        QPen pen(Qt::SolidLine);
        if(_isColorBorder) {
            pen.setColor(_colorBorder);
        } else {
            pen.setColor(darkenColor(_colorStart, 128));
        }
        painter->setPen(pen);
    }
    painter->setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(bounds, 10, 10);
    painter->drawPath(path);
    /// @note: https://stackoverflow.com/questions/29196610/qt-drawing-a-filled-rounded-rectangle-with-border");
}

bool GraphicsRoundRectItem::isComplexGradient() const
{
    return _isComplexGradient;
}

void GraphicsRoundRectItem::setComplexGradient(bool value)
{
    _isComplexGradient = value;
}

void GraphicsRoundRectItem::setComplexGradientColors(const QColor c0, const QColor c1, const QColor c2, const QColor c3, const QColor c4, const QColor c5)
{
    _c0 = c0 ;
    _c1 = c1 ;
    _c2 = c2 ;
    _c3 = c3 ;
    _c4 = c4 ;
    _c5 = c5 ;
}

//-------------------------------------------------------------------------------

LineItem::LineItem(QGraphicsItem *newOne, QGraphicsItem *newOther, QGraphicsItem * parent)
    : QGraphicsLineItem(parent)
{
    _one = newOne ;
    _other = newOther ;
}

LineItem::~LineItem()
{
}

void LineItem::updatePosition(XSDItemContext *context)
{
    if((NULL != _one) && (NULL != _other)) {
        QRectF r1 = _one->boundingRect();
        QRectF r2 = _other->boundingRect();
        if((NULL != context) && (XSDItemContext::DISPLAYSTR_HOR_PYRAMID == context->renderingStrategy())) {
            QPointF pos1 = mapFromItem(_one, r1.left() + r1.width(), r1.top() + (r1.height() / 2)) ;
            QPointF pos2 = mapFromItem(_other, r2.left(), r2.top() + (r2.height() / 2)) ;
            QLineF line(pos1.x() + (pos2.x() - pos1.x()) / 2, pos2.y(), pos2.x(), pos2.y());
            setLine(line);
        } else if((NULL != context) && (XSDItemContext::DISPLAYSTR_NEW0 == context->renderingStrategy())) {
            QPointF pos1 = mapFromItem(_one, r1.left() + r1.width(), r1.top() + (r1.height() / 2)) ;
            QPointF pos2 = mapFromItem(_other, r2.left(), r2.top() + (r2.height() / 2)) ;
            QLineF line(pos1.x() + (pos2.x() - pos1.x()) / 2, pos2.y(), pos2.x(), pos2.y());
            setLine(line);
        } else {
            QPointF pos1 = mapFromItem(_one, r1.left() + (r1.width() / 2), r1.top() + (r1.height() / 2)) ;
            QPointF pos2 = mapFromItem(_other, r2.left() + (r2.width() / 2), r2.top() + (r2.height() / 2)) ;
            QLineF line(pos1.x(), pos2.y(), pos2.x(), pos2.y());
            setLine(line);
        }
    }
}

//--------------------------------------------------------------------------------
TextItem::TextItem(QGraphicsItem * parent) : QGraphicsTextItem(parent)
{
}

TextItem::~TextItem()
{
}

QVariant TextItem::itemChange(QGraphicsItem::GraphicsItemChange change,
                              const QVariant &value)
{
    emit itemChanged(change, value);
    return QGraphicsTextItem::itemChange(change, value);
}

//--------------------------------------------------------------------------------

PolygonItem::PolygonItem(ItemServiceExecutor *service, const bool doubleBorder, QGraphicsItem * parent) : QGraphicsPolygonItem(parent)
{
    setService(service);
    _isDoubleBorder = doubleBorder ;
    _color = QColor::fromRgb(0, 0, 0);
    _pen.setColor(_color);
    _isShadow = true;
}

PolygonItem::~PolygonItem()
{
}

void PolygonItem::setService(ItemServiceExecutor *newService)
{
    _service = newService;
}

void PolygonItem::setColor(const QColor newColor)
{
    _color = newColor ;
    _pen.setColor(_color);
}

void PolygonItem::drawShadow(QPainter *painter, QRectF &bounds)
{
    QBrush brush(QColor(0xC0, 0xC0, 0xC0));
    painter->setBrush(brush);

    QPen pen(Qt::NoPen);
    painter->setPen(pen);
    if(_isShadow) {
        bounds.setHeight(bounds.height() + 22);
        bounds.setWidth(bounds.width() + 22);
    }
    painter->setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addPolygon(polygon().translated(2, 2));
    painter->drawPath(path);
}
void PolygonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(_isShadow) {
        QRectF bounds = boundingRect();
        drawShadow(painter, bounds);
    }

    QGraphicsPolygonItem::paint(painter, option, widget);
    if(_isDoubleBorder) {
        QRectF bounds = boundingRect();
        QPen oldPen = painter->pen();
        painter->setPen(_pen);
        painter->drawRoundedRect(bounds, 8, 8);
        bounds.adjust(4, 4, -4, -4);
        painter->drawRoundedRect(bounds, 8, 8);
        painter->setPen(oldPen);
    }
}


void PolygonItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if(NULL != _service) {
        _service->execMenuEvent(event, this);
    }
}


QVariant PolygonItem::itemChange(GraphicsItemChange change,
                                 const QVariant &value)
{
    emit itemChanged(change, value);
    return QGraphicsItem::itemChange(change, value);
}

//--------------------------------------------------------------------------------

CircleItem::CircleItem(ItemServiceExecutor *service, QGraphicsItem * parent) : QGraphicsEllipseItem(parent)
{
    setService(service);
    _isShadow = true;
}

CircleItem::~CircleItem()
{
}

void CircleItem::setService(ItemServiceExecutor *newService)
{
    _service = newService;
}

void CircleItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if(NULL != _service) {
        _service->execMenuEvent(event, this);
    }
}

QVariant CircleItem::itemChange(GraphicsItemChange change,
                                const QVariant &value)
{
    emit itemChanged(change, value);
    return QGraphicsItem::itemChange(change, value);
}

//--------------------------------------------------------------------------------
