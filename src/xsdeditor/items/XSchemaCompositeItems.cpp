/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
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
#include "xsdeditor/items/xitemsdefinitions.h"


//--------------------------------------------------------------------------------------


ChoiceItem::ChoiceItem(XsdGraphicContext *newContext, XSchemaChoice *newItem, QGraphicsItem * /*parent*/)
    : XSDItem(newContext), _item(NULL),
      _graphicsItem(new CircleItem(this)), _labelItem(NULL)
{
    _extraSpace = 0;
    newContext->scene()->addItem(_graphicsItem);
    _graphicsItem->setData(XSD_ITEM_DATA, qVariantFromValue((void*)this));
    init(newContext);
    setItem(newItem);
}

ChoiceItem::~ChoiceItem()
{
    reset();
}

void ChoiceItem::reset()
{
}

void ChoiceItem::init(XsdGraphicContext * /*newContext*/)
{
    _contour = QRectF(0, 0, 30, 30);
    _graphicsItem->setRect(_contour);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsMovable, false);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    _graphicsItem->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    _graphicsItem->setPos(0, 0);

    _iconItem = new QGraphicsPixmapItem(_graphicsItem);
    //_iconItem->setPos((30 - _iconItem->boundingRect().width()) / 2, (30 - _iconItem->boundingRect().height()) / 2 + (_iconItem->boundingRect().height() / 2));
    /*int w = _graphicsItem->rect().width() ;
    int w2 = _iconItem->boundingRect().width();
    w=w;
    w2=w2;*/
    _iconItem->setPos((_graphicsItem->rect().width() - 24) / 2, (_graphicsItem->rect().height() - 24) / 2);
    QPixmap pixmap ;
    pixmap.load(PIXMAP_CHOICE_INFO);
    _iconItem->setPixmap(pixmap);

    _labelItem = new QGraphicsTextItem(_graphicsItem);
    _labelItem->setPlainText("choice");
    _labelItem->setPos(35, 16);
    _graphicsItem->childItems().append(_labelItem);

    createIconInfo(_graphicsItem, 24, 30);

    QLinearGradient gradient(0, 0, 0, 100);
    gradient.setColorAt(0, QColor::fromRgbF(0, 0, 1, 1));
    gradient.setColorAt(1, QColor::fromRgbF(0, 1, 1, .9));

    _graphicsItem->setBrush(QBrush(gradient));
    connect(_graphicsItem, SIGNAL(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)), this, SLOT(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)));
}

XSchemaChoice *ChoiceItem::choice() const
{
    return _item ;
}

QString ChoiceItem::preTooltipString()
{
    if(NULL != _item) {
        return _item->tagName();
    }
    return "";
}

void ChoiceItem::setItem(XSchemaChoice *newItem)
{
    QString annotationInfo ;
    bool showInfo = false;
    QString newText ;
    if(_item != newItem) {
        if(NULL != _item) {
            disconnect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
            //TODO disconnect(_item, SIGNAL(childRemoved(XSchemaObject*)), this, SLOT(childRemoved(XSchemaObject*)));
        }
        _item = newItem ;
        if(NULL != newItem) {
            connect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
            //TODO connect(_item, SIGNAL(childRemoved(XSchemaObject*)), this, SLOT(childRemoved(XSchemaObject*)));
            foreach(XSchemaObject * child, _item->getChildren()) {
                childAdded(child);
            }
            newText =  _item->description();
            if(NULL != _item->annotation()) {
                showInfo = true ;
                annotationInfo = _item->annotation()->text();
            }
        }
    }
    _labelItem->setPlainText(newText);
    _extraSpace = _labelItem->boundingRect().width();
    if(_isDiff) {
        QLinearGradient gradient(0, 0, 0, 100);
        setGradientColor(gradient, _item->compareState());
        _graphicsItem->setBrush(QBrush(gradient));
    }
    if(showInfo && !annotationInfo.isEmpty()) {
        _iconInfo->show();
        _iconInfo->setToolTip(annotationInfo);
        _iconInfo->setPos(30, (_graphicsItem->boundingRect().height() - _iconInfo->boundingRect().height()) / 2);
    } else {
        _iconInfo->hide();
    }
    buildTooltip();
}

void ChoiceItem::itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant & /*value*/)
{
    if(change == QGraphicsItem::ItemPositionChange) {
        RChild *theChain = chain();
        if(NULL != theChain) {
            chain()->updatePosition();
            updateScenePosition(_graphicsItem);
            foreach(RChild * child, _children.children()) {
                child->updatePosition();
            }
        }
    }
}

qreal ChoiceItem::extraSpace()
{
    return _extraSpace ;
}

//--------------------------------------------------------------------------------------


SequenceItem::SequenceItem(XsdGraphicContext *newContext, XSchemaSequence *newItem, QGraphicsItem * /*parent*/)
    : XSDItem(newContext), _item(NULL),
      _graphicsItem(new GraphicsRectItem(this)), _labelItem(NULL)
{
    _extraSpace = 0 ;
    newContext->scene()->addItem(_graphicsItem);
    _graphicsItem->setData(XSD_ITEM_DATA, qVariantFromValue((void*)this));
    init(newContext);
    setItem(newItem);
}

SequenceItem::~SequenceItem()
{
    reset();
}

void SequenceItem::reset()
{
}

void SequenceItem::init(XsdGraphicContext * /*newContext*/)
{
    _contour = QRectF(0, 0, 30, 30);
    _graphicsItem->setRect(_contour);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsMovable, false);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    _graphicsItem->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    _graphicsItem->setPos(11, 0);

    _labelItem = new QGraphicsTextItem(_graphicsItem);
    _labelItem->setPlainText(tr("Sequence"));
    _labelItem->setPos(60, 16);
    _graphicsItem->childItems().append(_labelItem);

    _icon = new QGraphicsPixmapItem(_graphicsItem);
    _icon ->setPos(5, 5);
    QPixmap pixmap ;
    pixmap.load(PIXMAP_SEQUENCE_INFO);
    _icon->setPixmap(pixmap);

    QLinearGradient gradient(0, 0, 0, 100);
    gradient.setColorAt(0, QColor::fromRgbF(0, 1, .8, .2));
    gradient.setColorAt(1, QColor::fromRgbF(0, 1, 1, 0));

    createIconInfo(_graphicsItem, 24, 30);

    _graphicsItem->setBrush(QBrush(gradient));
    connect(_graphicsItem, SIGNAL(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)), this, SLOT(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)));
}

qreal SequenceItem::extraSpace()
{
    return _extraSpace ;
}

XSchemaSequence *SequenceItem::sequence() const
{
    return _item ;
}

QString SequenceItem::preTooltipString()
{
    if(NULL != _item) {
        return _item->tagName();
    }
    return "" ;
}

void SequenceItem::setItem(XSchemaSequence *newItem)
{
    bool showInfo = false;
    QString descr = "" ;
    QString annotationInfo ;
    if(_item != newItem) {
        if(NULL != _item) {
            disconnect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
            //TODO disconnect(_item, SIGNAL(childRemoved(XSchemaObject*)), this, SLOT(childRemoved(XSchemaObject*)));
        }
        _item = newItem ;
        if(NULL != newItem) {
            connect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
            //TODO connect(_item, SIGNAL(childRemoved(XSchemaObject*)), this, SLOT(childRemoved(XSchemaObject*)));
            foreach(XSchemaObject * child, _item->getChildren()) {
                childAdded(child);
            }
            descr = _item->description();
            if(NULL != _item->annotation()) {
                showInfo = true ;
                annotationInfo = _item->annotation()->text();
            }
        }
    }
    if(showInfo && !annotationInfo.isEmpty()) {
        _iconInfo->show();
        _iconInfo->setToolTip(annotationInfo);
        _iconInfo->setPos(30, (_graphicsItem->boundingRect().height() - _iconInfo->boundingRect().height()) / 2);
    } else {
        _iconInfo->hide();
    }

    _labelItem->setPlainText(descr);
    _extraSpace = _labelItem->boundingRect().width();
    if(_isDiff) {
        QLinearGradient gradient(0, 0, 0, 100);
        setGradientColor(gradient, _item->compareState());
        _graphicsItem->setBrush(QBrush(gradient));
    }
    buildTooltip();
}

void SequenceItem::itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant & /*value*/)
{
    if(change == QGraphicsItem::ItemPositionChange) {
        RChild *theChain = chain();
        if(NULL != theChain) {
            chain()->updatePosition();
            updateScenePosition(_graphicsItem);
            foreach(RChild * child, _children.children()) {
                child->updatePosition();
            }
        }
    }
}


//-------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------

GroupItem::GroupItem(XsdGraphicContext *newContext, XSchemaObject *newItem, QGraphicsItem * /*parent*/)
    : XSDItem(newContext), _item(NULL), _graphicsItem(new PolygonItem(this)),
      _icon(NULL), _textItem(NULL)
{
    newContext->scene()->addItem(_graphicsItem);
    _graphicsItem->setData(XSD_ITEM_DATA, qVariantFromValue((void*)this));
    init(newContext);
    setItem(newItem);
}

GroupItem::~GroupItem()
{
    reset();
}

void GroupItem::reset()
{
}


void GroupItem::init(XsdGraphicContext *newContext)
{
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

    setGraphics(QColor::fromRgbF(.2, 1, 1, 1));

    _textItem = new QGraphicsTextItem(_graphicsItem);
    _textItem->setPos(30, 10);
    _textItem->setDefaultTextColor(QColor::fromRgb(0, 0, 0));
    //setStdFontToItem(_textItem, false, false, false);
    _textItem->setFont(newContext->normalFont());

    createIconInfo(_graphicsItem);
    createExtraAttrsIcon(_graphicsItem, 24);

    connect(_graphicsItem, SIGNAL(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)), this, SLOT(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)));
}


void GroupItem::setGraphics(/*const QPixmap & pixmap,*/ QColor bkColor)
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

void GroupItem::setItem(XSchemaObject *newItem)
{
    XSchemaGroup *newItemGroup = static_cast<XSchemaGroup *>(newItem);
    QString annotationInfo;
    bool showInfo = false;
    if(_item != newItem) {
        if(NULL != _item) {
            removeObject(_item);
            disconnect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
        }
        _item = newItemGroup ;
        if(NULL != newItem) {
            setObject(_item, NULL);
            connect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
            _textItem->setPlainText(newItem->description());
            setGraphics(QColor::fromRgbF(.2, 1, 1, 1));
            //setGraphics(/*newItem->icon(),*/ newItem->color());
            preAddChildren(_item);

            foreach(XSchemaObject * child, _item->getChildren()) {
                childAdded(child);
            }

            showInfo = true ;
            if(NULL != _item->annotation()) {
                annotationInfo = _item->annotation()->text();
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

void GroupItem::itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant & /*value*/)
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

//------------------------------
//--------------------------------------------------------------------------------------

AttributeGroupItem::AttributeGroupItem(XsdGraphicContext *newContext, XSchemaObject *newItem, QGraphicsItem * /*parent*/)
    : XSDItem(newContext), _item(NULL), _graphicsItem(new PolygonItem(this)),
      _icon(NULL), _textItem(NULL)
{
    newContext->scene()->addItem(_graphicsItem);
    _graphicsItem->setData(XSD_ITEM_DATA, qVariantFromValue((void*)this));
    init(newContext);
    setItem(newItem);
}

AttributeGroupItem::~AttributeGroupItem()
{
    reset();
}

void AttributeGroupItem::reset()
{
}


void AttributeGroupItem::init(XsdGraphicContext *newContext)
{
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

    setGraphics(QColor::fromRgbF(1, .8, .2, 1));

    _textItem = new QGraphicsTextItem(_graphicsItem);
    _textItem->setPos(30, 10);
    _textItem->setDefaultTextColor(QColor::fromRgb(0, 0, 0));
    //setStdFontToItem(_textItem, false, false, false);
    _textItem->setFont(newContext->normalFont());

    createIconInfo(_graphicsItem);
    createExtraAttrsIcon(_graphicsItem, 24);

    connect(_graphicsItem, SIGNAL(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)), this, SLOT(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)));
}


void AttributeGroupItem::setGraphics(/*const QPixmap & pixmap,*/ QColor bkColor)
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

void AttributeGroupItem::setItem(XSchemaObject *newItem)
{
    XSchemaAttributeGroup *newItemGroup = static_cast<XSchemaAttributeGroup *>(newItem);
    QString annotationInfo;
    bool showInfo = false;
    if(_item != newItem) {
        if(NULL != _item) {
            removeObject(_item);
            disconnect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
        }
        _item = newItemGroup ;
        if(NULL != newItem) {
            setObject(_item, NULL);
            connect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
            _textItem->setPlainText(newItem->description());
            setGraphics(QColor::fromRgbF(1, .8, .2, 1));
            //setGraphics(/*newItem->icon(),*/ newItem->color());
            preAddChildren(_item);

            foreach(XSchemaObject * child, _item->getChildren()) {
                childAdded(child);
            }

            showInfo = true ;
            if(NULL != _item->annotation()) {
                annotationInfo = _item->annotation()->text();
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

void AttributeGroupItem::itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant & /*value*/)
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

//------------------------------

