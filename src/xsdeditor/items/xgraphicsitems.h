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

#ifndef XGRAPHICSITEMS_H
#define XGRAPHICSITEMS_H

#include <QGraphicsItem>
#include <QPen>


enum {
    TypeBase = QGraphicsItem::UserType + 128,
    //TypeElement = QGraphicsItem::UserType + 129,
    //TypeAttribute = QGraphicsItem::UserType + 130,
    //TypeSchema = QGraphicsItem::UserType + 131,
    TypeText = QGraphicsItem::UserType + 132,
    TypeLine = QGraphicsItem::UserType + 133,
    //TypeChoice = QGraphicsItem::UserType + 134,
    TypeRectItem = QGraphicsItem::UserType + 135,
    TypePolygonItem = QGraphicsItem::UserType + 136,
    TypeCircleItem = QGraphicsItem::UserType + 137,
    TypeRoundRectItem = QGraphicsItem::UserType + 138
};

#define IS_TYPE(xtype)  enum { Type = xtype }; virtual int type() const { return Type; }


class ItemServiceExecutor
{
public:
    virtual void execMenuEvent(QGraphicsSceneContextMenuEvent *event, QGraphicsItem *item) = 0 ;
};

QColor darkenColor(QColor color, int qty);

class GraphicsRoundRectItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
    ItemServiceExecutor *_service ;
    QColor _colorStart;
    QColor _colorMiddle;
    QColor _colorEnd;
    QColor _colorBorder;
    // complex gradient
    QColor _c0, _c1, _c2, _c3, _c4, _c5;
    bool _isOptional;
    bool _isSingleColor ;
    bool _isColorBorder ;
    bool _useDimShadow;
    bool _isComplexGradient;

    enum {
        OffsetRectX = 5,
        OffsetRectY = 5
    };

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

public:

    IS_TYPE(TypeRoundRectItem)

    GraphicsRoundRectItem(ItemServiceExecutor *service, QGraphicsItem * parent = 0);
    virtual ~GraphicsRoundRectItem();

    void setService(ItemServiceExecutor *newService);
    void drawShadow(QPainter *painter, QRectF &bounds);

    QColor colorBorder();
    void setColorBorder(const QColor value);
    QColor colorStart();
    void setColorStart(const QColor value);
    QColor colorEnd();
    void setColorEnd(const QColor value);
    QColor colorMiddle();
    void setColorMiddle(const QColor value);


    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    bool isOptional() const;
    void setOptional(bool isOptional);

    bool isUseColorBorder() const;
    void setUseColorBorder(bool value);
    bool isSingleColor() const;
    void setSingleColor(bool value);

    bool isUseDimShadow() const;
    void setUseDimShadow(bool useDimShadow);

    bool isComplexGradient() const;
    void setComplexGradient(bool isComplexGradient);
    void setComplexGradientColors(const QColor c0, const QColor c1, const QColor c2, const QColor c3, const QColor c4, const QColor c5);

signals:
    void itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant &value);
};


class GraphicsRectItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
    ItemServiceExecutor *_service ;
    bool _isShadow;

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void drawShadow(QPainter *painter, QRectF &bounds);

public:

    IS_TYPE(TypeRectItem)

    GraphicsRectItem(ItemServiceExecutor *service, QGraphicsItem * parent = 0);
    virtual ~GraphicsRectItem();

    void setService(ItemServiceExecutor *newService);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget *widget);

signals:
    void itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant &value);
};


class PolygonItem : public QObject, public QGraphicsPolygonItem
{
    Q_OBJECT
    ItemServiceExecutor *_service ;
    bool _isDoubleBorder;
    QColor _color;
    QPen _pen;
    bool _isShadow ;

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void drawShadow(QPainter *painter, QRectF &bounds);

public:

    IS_TYPE(TypePolygonItem)

    PolygonItem(ItemServiceExecutor *service, const bool doubleBorder = false, QGraphicsItem * parent = 0);
    virtual ~PolygonItem();

    void setService(ItemServiceExecutor *newService);
    void setColor(const QColor newColor);

signals:
    void itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant &value);
};

//------------------------


class CircleItem : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
    ItemServiceExecutor *_service ;
    bool _isShadow ;

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

public:

    IS_TYPE(TypeCircleItem)

    CircleItem(ItemServiceExecutor *menuBuilder, QGraphicsItem * parent = 0);
    virtual ~CircleItem();

    void setService(ItemServiceExecutor *newService);
signals:
    void itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant &value);
};

//------------------------

class TextItem : public QGraphicsTextItem
{
    Q_OBJECT

protected:
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

public:

    IS_TYPE(TypeText)

    TextItem(QGraphicsItem * parent = 0);
    virtual ~TextItem();

signals:
    void itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant &value);
};

class XSDItemContext;

class LineItem : public QGraphicsLineItem
{
    QGraphicsItem *_one;
    QGraphicsItem *_other;

public:

    IS_TYPE(TypeLine)

    LineItem(QGraphicsItem *newOne, QGraphicsItem *newOther, QGraphicsItem * parent = 0);
    virtual ~LineItem();

    void updatePosition(XSDItemContext *context);
};


#endif // XGRAPHICSITEMS_H
