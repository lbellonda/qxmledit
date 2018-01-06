/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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
#include <QGraphicsSceneContextMenuEvent>
#include "xsdeditor/items/xitemsdefinitions.h"


//--------------------------------------------------------------------------------------

IncludeItem::IncludeItem(XsdGraphicContext *newContext, XSchemaObject *newItem, QGraphicsItem * /*parent*/)
    : XSDItem(newContext), _item(NULL), _graphicsItem(new PolygonItem(this)),
      _icon(NULL), _textItem(NULL)
{
    newContext->scene()->addItem(_graphicsItem);
    _graphicsItem->setData(XSD_ITEM_DATA, qVariantFromValue((void*)this));
    init(newContext);
    setItem(newItem);
}

IncludeItem::~IncludeItem()
{
    reset();
}

void IncludeItem::reset()
{
}


void IncludeItem::init(XsdGraphicContext *newContext)
{
    _isDiff = newContext->contextType() == XsdGraphicContext::CONTEXT_DIFF ;
    QPainterPath path;
    path.moveTo(150, 50);
    path.arcTo(100, 0, 50, 50, 0, 90);
    path.arcTo(0, 0, 50, 50, 90, 90);
    path.arcTo(0, 50, 50, 50, 180, 90);
    path.arcTo(100, 50, 50, 50, 270, 90);
    path.lineTo(150, 25);
    _contour = path.toFillPolygon();
    _graphicsItem->setPolygon(_contour);

    _graphicsItem->setFlag(QGraphicsItem::ItemIsMovable, false);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    _graphicsItem->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    _graphicsItem->setPos(0, 0);

    setGraphics(QColor::fromRgbF(1, 1, 1, 1));

    _textItem = new QGraphicsTextItem(_graphicsItem);
    _textItem->setPos(30, 10);
    _textItem->setDefaultTextColor(QColor::fromRgb(0, 0, 0));
    //setStdFontToItem(_textItem, false, false, false);
    _textItem->setFont(newContext->normalFont());

    createIconInfo(_graphicsItem);
    createExtraAttrsIcon(_graphicsItem, 24);

    connect(_graphicsItem, SIGNAL(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)), this, SLOT(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)));
}


void IncludeItem::setGraphics(/*const QPixmap & pixmap,*/ QColor bkColor)
{
    QLinearGradient gradient(0, 0, 0, 100);
    QColor clr2(bkColor);
    clr2 = clr2.lighter();
    clr2.setAlpha(100);
    gradient.setColorAt(0, bkColor);
    gradient.setColorAt(1, clr2);
    _graphicsItem->setBrush(QBrush(gradient));
    //_icon->setPixmap(pixmap);
}

void IncludeItem::setItem(XSchemaObject *newItem)
{
    QString annotationInfo;
    bool showInfo = false;
    if(_item != newItem) {
        if(NULL != _item) {
            removeObject(_item);
            disconnect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
        }
        _item = newItem ;
        if(NULL != newItem) {
            setObject(_item, NULL);
            connect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
            _textItem->setPlainText(newItem->description());
            setGraphics(/*newItem->icon(),*/ newItem->color());
            // look for children of the pointed schema.
            XSchemaInclude* incl = static_cast<XSchemaInclude*>(_item);
            XSDSchema *definitions = NULL ;
            if(_context->isShowAllSchema()) {
                definitions = incl->root()->schema()->infoPool()->mainSchema()->includedSchemaByObject(incl);
            }

            foreach(XSchemaObject * child, _item->getChildren()) {
                childAdded(child);
            }

            if(NULL != definitions) {
                childAdded(definitions);
            }

            if(NULL != _item->annotation()) {
                showInfo = true ;
                annotationInfo = _item->annotation()->text();
            } else {
                if(_item->getType() == SchemaTypeAnnotation) {
                    showInfo = true ;
                    XSchemaAnnotation *annotation = static_cast<XSchemaAnnotation *>(_item);
                    annotationInfo = annotation->text();
                }
            }
        } else {
            setGraphics(QColor(0, 0, 0, 0));
            _textItem->setPlainText("");
        }
    }
    // TODO: refactor
    QList<QGraphicsItem*> items;
    items.append(_icon);
    doShowInfo(showInfo, annotationInfo, items);
    if(_item->hasOtherAttributes()) {
        _iconExtraAttrs->setVisible(true);
        items.append(_iconExtraAttrs);
        _iconExtraAttrs->setToolTip(_item->getOhterAttrsAsTextList());
    }
    items.append(_textItem);

    QRectF size = disposeHorizontallyAndAlignLower(items, 4, 4) ;
    QPainterPath path;
    qreal width = size.x() + size.width() + 10;
    qreal height = size.y() + size.height() ;

    if(_isDiff) {
        QLinearGradient gradient(0, 0, 0, 100);
        setGradientColor(gradient, _item->compareState());
        _graphicsItem->setBrush(QBrush(gradient));
        setToolTipState(_graphicsItem, _item->compareState());
    }

    path.moveTo(0, 0);
    path.lineTo(width, 0);
    path.lineTo(width, height);
    path.lineTo(0, height);
    _contour = path.toFillPolygon();
    _graphicsItem->setPolygon(_contour);
    buildTooltip();
}

void IncludeItem::itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant & /*value*/)
{
    if(change == QGraphicsItem::ItemPositionChange) {
        RChild *theChain = chain();
        if(NULL != theChain) {
            chain()->updatePosition();
            updateScenePosition(_graphicsItem);
        }
        foreach(RChild * child, _children.children()) {
            child->updatePosition();
        }
    }
}

QString IncludeItem::itemLabelForChart()
{
    QString name ;
    if(NULL != _item) {
        XSchemaInclude* incl = static_cast<XSchemaInclude*>(_item);
        name = incl->schemaLocation();
    }
    return QString("include %1").arg(name) ;
}

QColor IncludeItem::itemColorForChart()
{
    return XSDItem::itemColorForChart();
}

//------------------------------


RedefineItem::RedefineItem(XsdGraphicContext *newContext, XSchemaObject *newItem, QGraphicsItem * /*parent*/)
    : XSDItem(newContext), _item(NULL), _graphicsItem(new PolygonItem(this)),
      _icon(NULL), _textItem(NULL)
{
    newContext->scene()->addItem(_graphicsItem);
    _graphicsItem->setData(XSD_ITEM_DATA, qVariantFromValue((void*)this));
    init(newContext);
    setItem(newItem);
}

RedefineItem::~RedefineItem()
{
    reset();
}

void RedefineItem::reset()
{
}


void RedefineItem::init(XsdGraphicContext *newContext)
{
    _isDiff = newContext->contextType() == XsdGraphicContext::CONTEXT_DIFF ;
    QPainterPath path;
    path.moveTo(150, 50);
    path.arcTo(100, 0, 50, 50, 0, 90);
    path.arcTo(0, 0, 50, 50, 90, 90);
    path.arcTo(0, 50, 50, 50, 180, 90);
    path.arcTo(100, 50, 50, 50, 270, 90);
    path.lineTo(150, 25);
    _contour = path.toFillPolygon();
    _graphicsItem->setPolygon(_contour);

    _graphicsItem->setFlag(QGraphicsItem::ItemIsMovable, false);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    _graphicsItem->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    _graphicsItem->setPos(0, 0);

    setGraphics(QColor::fromRgbF(1, 1, 1, 1));

    _textItem = new QGraphicsTextItem(_graphicsItem);
    _textItem->setPos(30, 10);
    _textItem->setDefaultTextColor(QColor::fromRgb(0, 0, 0));
    //setStdFontToItem(_textItem, false, false, false);
    _textItem->setFont(newContext->normalFont());

    createIconInfo(_graphicsItem);
    createExtraAttrsIcon(_graphicsItem, 24);

    connect(_graphicsItem, SIGNAL(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)), this, SLOT(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)));
}


void RedefineItem::setGraphics(/*const QPixmap & pixmap,*/ QColor bkColor)
{
    QLinearGradient gradient(0, 0, 0, 100);
    QColor clr2(bkColor);
    clr2 = clr2.lighter();
    clr2.setAlpha(100);
    gradient.setColorAt(0, bkColor);
    gradient.setColorAt(1, clr2);
    _graphicsItem->setBrush(QBrush(gradient));
    //_icon->setPixmap(pixmap);
}

void RedefineItem::setItem(XSchemaObject *newItem)
{
    QString annotationInfo;
    bool showInfo = false;
    if(_item != newItem) {
        if(NULL != _item) {
            removeObject(_item);
            disconnect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
        }
        _item = newItem ;
        if(NULL != newItem) {
            setObject(_item, NULL);
            connect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
            _textItem->setPlainText(newItem->description());
            setGraphics(/*newItem->icon(),*/ newItem->color());
            // look for children of the pointed schema.
            XSchemaRedefine* redef = static_cast<XSchemaRedefine*>(_item);
            XSDSchema* definitions = NULL ;
            if(_context->isShowAllSchema()) {
                definitions = redef->root()->schema()->infoPool()->mainSchema()->redefinedSchemaByObject(redef);
            }

            foreach(XSchemaObject * child, _item->getChildren()) {
                childAdded(child);
            }

            if(NULL != definitions) {
                childAdded(definitions);
            }

            if(NULL != _item->annotation()) {
                showInfo = true ;
                annotationInfo = _item->annotation()->text();
            } else {
                if(_item->getType() == SchemaTypeAnnotation) {
                    showInfo = true ;
                    XSchemaAnnotation *annotation = static_cast<XSchemaAnnotation *>(_item);
                    annotationInfo = annotation->text();
                }
            }
        } else {
            setGraphics(QColor(0, 0, 0, 0));
            _textItem->setPlainText("");
        }
    }
    // TODO: refactor
    QList<QGraphicsItem*> items;
    items.append(_icon);
    doShowInfo(showInfo, annotationInfo, items);
    if(_item->hasOtherAttributes()) {
        _iconExtraAttrs->setVisible(true);
        items.append(_iconExtraAttrs);
        _iconExtraAttrs->setToolTip(_item->getOhterAttrsAsTextList());
    }
    items.append(_textItem);

    QRectF size = disposeHorizontallyAndAlignLower(items, 4, 4) ;
    QPainterPath path;
    qreal width = size.x() + size.width() + 10;
    qreal height = size.y() + size.height() ;

    if(_isDiff) {
        QLinearGradient gradient(0, 0, 0, 100);
        setGradientColor(gradient, _item->compareState());
        _graphicsItem->setBrush(QBrush(gradient));
        setToolTipState(_graphicsItem, _item->compareState());
    }

    path.moveTo(0, 0);
    path.lineTo(width, 0);
    path.lineTo(width, height);
    path.lineTo(0, height);
    _contour = path.toFillPolygon();
    _graphicsItem->setPolygon(_contour);
    buildTooltip();
}

void RedefineItem::itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant & /*value*/)
{
    if(change == QGraphicsItem::ItemPositionChange) {
        RChild *theChain = chain();
        if(NULL != theChain) {
            chain()->updatePosition();
            updateScenePosition(_graphicsItem);
        }
        foreach(RChild * child, _children.children()) {
            child->updatePosition();
        }
    }
}

QString RedefineItem::itemLabelForChart()
{
    QString name ;
    if(NULL != _item) {
        name = _item->description();
    }
    return QString("redefine %1").arg(name) ;
}

QColor RedefineItem::itemColorForChart()
{
    return XSDItem::itemColorForChart();
}


//--------------------------------------------------------------------------------------

ImportItem::ImportItem(XsdGraphicContext *newContext, XSchemaObject *newItem, QGraphicsItem * /*parent*/)
    : XSDItem(newContext), _item(NULL), _graphicsItem(new PolygonItem(this)),
      _icon(NULL), _textItem(NULL)
{
    newContext->scene()->addItem(_graphicsItem);
    _graphicsItem->setData(XSD_ITEM_DATA, qVariantFromValue((void*)this));
    init(newContext);
    setItem(newItem);
}

ImportItem::~ImportItem()
{
    reset();
}

void ImportItem::reset()
{
}


void ImportItem::init(XsdGraphicContext *newContext)
{
    _isDiff = newContext->contextType() == XsdGraphicContext::CONTEXT_DIFF ;
    QPainterPath path;
    path.moveTo(150, 50);
    path.arcTo(100, 0, 50, 50, 0, 90);
    path.arcTo(0, 0, 50, 50, 90, 90);
    path.arcTo(0, 50, 50, 50, 180, 90);
    path.arcTo(100, 50, 50, 50, 270, 90);
    path.lineTo(150, 25);
    _contour = path.toFillPolygon();
    _graphicsItem->setPolygon(_contour);

    _graphicsItem->setFlag(QGraphicsItem::ItemIsMovable, false);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    _graphicsItem->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    _graphicsItem->setPos(0, 0);

    setGraphics(QColor::fromRgbF(1, 1, 1, 1));

    _textItem = new QGraphicsTextItem(_graphicsItem);
    _textItem->setPos(30, 10);
    _textItem->setDefaultTextColor(QColor::fromRgb(0, 0, 0));
    //setStdFontToItem(_textItem, false, false, false);
    _textItem->setFont(newContext->normalFont());

    createIconInfo(_graphicsItem);
    createExtraAttrsIcon((_graphicsItem), 24);

    connect(_graphicsItem, SIGNAL(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)), this, SLOT(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)));
}


void ImportItem::setGraphics(/*const QPixmap & pixmap,*/ QColor bkColor)
{
    QLinearGradient gradient(0, 0, 0, 100);
    QColor clr2(bkColor);
    clr2 = clr2.lighter();
    clr2.setAlpha(100);
    gradient.setColorAt(0, bkColor);
    gradient.setColorAt(1, clr2);
    _graphicsItem->setBrush(QBrush(gradient));
    //_icon->setPixmap(pixmap);
}

void ImportItem::setItem(XSchemaObject *newItem)
{
    QString annotationInfo;
    bool showInfo = false;
    if(_item != newItem) {
        if(NULL != _item) {
            removeObject(_item);
            disconnect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
        }
        _item = newItem ;
        if(NULL != newItem) {
            setObject(_item, NULL);
            connect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
            _textItem->setPlainText(newItem->description());
            setGraphics(/*newItem->icon(),*/ newItem->color());
            // look for children of the pointed schema.
            XSchemaImport* incl = static_cast<XSchemaImport*>(_item);
            XSDSchema *definitions = NULL ;
            if(_context->isShowAllSchema()) {
                definitions = incl->root()->schema()->infoPool()->mainSchema()->importedSchemaByObject(incl);
            }

            foreach(XSchemaObject * child, _item->getChildren()) {
                childAdded(child);
            }

            if(NULL != definitions) {
                childAdded(definitions);
            }

            if(NULL != _item->annotation()) {
                showInfo = true ;
                annotationInfo = _item->annotation()->text();
            } else {
                if(_item->getType() == SchemaTypeAnnotation) {
                    showInfo = true ;
                    XSchemaAnnotation *annotation = static_cast<XSchemaAnnotation *>(_item);
                    annotationInfo = annotation->text();
                }
            }
        } else {
            setGraphics(QColor(0, 0, 0, 0));
            _textItem->setPlainText("");
        }
    }
    // TODO: refactor
    QList<QGraphicsItem*> items;
    items.append(_icon);
    doShowInfo(showInfo, annotationInfo, items);
    if(_item->hasOtherAttributes()) {
        _iconExtraAttrs->setVisible(true);
        items.append(_iconExtraAttrs);
        _iconExtraAttrs->setToolTip(_item->getOhterAttrsAsTextList());
    }
    items.append(_textItem);

    QRectF size = disposeHorizontallyAndAlignLower(items, 4, 4) ;
    QPainterPath path;
    qreal width = size.x() + size.width() + 10;
    qreal height = size.y() + size.height() ;

    if(_isDiff) {
        QLinearGradient gradient(0, 0, 0, 100);
        setGradientColor(gradient, _item->compareState());
        _graphicsItem->setBrush(QBrush(gradient));
        setToolTipState(_graphicsItem, _item->compareState());
    }

    path.moveTo(0, 0);
    path.lineTo(width, 0);
    path.lineTo(width, height);
    path.lineTo(0, height);
    _contour = path.toFillPolygon();
    _graphicsItem->setPolygon(_contour);
    buildTooltip();
}

void ImportItem::itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant & /*value*/)
{
    if(change == QGraphicsItem::ItemPositionChange) {
        RChild *theChain = chain();
        if(NULL != theChain) {
            chain()->updatePosition();
            updateScenePosition(_graphicsItem);
        }
        foreach(RChild * child, _children.children()) {
            child->updatePosition();
        }
    }
}

QString ImportItem::itemLabelForChart()
{
    QString name ;
    if(NULL != _item) {
        XSchemaImport* incl = static_cast<XSchemaImport*>(_item);
        name = incl->schemaLocation();
    }
    return QString("import %1").arg(name) ;
}

QColor ImportItem::itemColorForChart()
{
    return XSDItem::itemColorForChart();
}

//------------------------------
