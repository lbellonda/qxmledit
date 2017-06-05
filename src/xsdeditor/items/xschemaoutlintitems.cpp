/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2017 by Luca Bellonda and individual contributors       *
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

// outline items

#include "xsdeditor/xsdwindow.h"
#include "utils.h"
#include <QGraphicsTextItem>
#include <QGraphicsSceneContextMenuEvent>
#include "xsdeditor/items/xitemsdefinitions.h"


OutlineElementItem::OutlineElementItem(XsdGraphicContext *newContext, XSchemaOutlineElement *newItem, QGraphicsItem * /*parent*/)
    : XSDItem(newContext), _item(NULL),
      _graphicsItem(new GraphicsRoundRectItem(this)),
      _textItem(NULL), _propertiesItem(NULL), _iconLink(NULL), _separator(NULL)
{
    Utils::TODO_THIS_RELEASE("check for item type");
    newContext->scene()->addItem(_graphicsItem);
    _graphicsItem->setToolTip("TEST TOOLTIP");
    _graphicsItem->setData(XSD_ITEM_DATA, qVariantFromValue((void*)this));
    init(newContext);
    setItem(newItem);
}

OutlineElementItem::~OutlineElementItem()
{
    Utils::TODO_THIS_RELEASE("assicurarsi sia eliminato");
    /* if( NULL != _textItem ) {
         //TODO disconnect(_textItem, SIGNAL(itemChange(GraphicsItemChange,QVariant&)), this, SLOT(textChanged()) );
     }*/
    reset();
}

void OutlineElementItem::reset()
{
    /*foreach( AttributeItem* attribute, _attributes){
        delete attribute ;
    }
    _attributes.clear();*/
    //_graphicsItem->childItems().clear();
    //_graphicsItem->scene()->removeItem(_graphicsItem);
    //_graphicsItem->childItems().clear();
}

void OutlineElementItem::init(XsdGraphicContext *newContext)
{
    _graphicsItem->setFlag(QGraphicsItem::ItemIsMovable, false);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    _graphicsItem->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    _graphicsItem->setPos(0, 0);

    QLinearGradient gradient(0, 0, 0, 100);
    gradient.setColorAt(0, QColor::fromRgbF(0, 1, 0, 1));
    gradient.setColorAt(1, QColor::fromRgbF(1, 1, 1, 1));

    _graphicsItem->setBrush(QBrush(gradient));

    QPainterPath path;
    path.moveTo(0, 60);
    path.lineTo(0, 20);
    path.lineTo(30, 0);
    path.lineTo(200, 0);
    path.lineTo(200, 60);
    path.lineTo(0, 60);

    _contour = path.toFillPolygon();

    _propertiesItem = new QGraphicsTextItem(_graphicsItem);
    _propertiesItem->setFont(newContext->normalFont());
    _propertiesItem->setPlainText(tr("Element"));
    _propertiesItem->setPos(25, 30);
    _propertiesItem->setDefaultTextColor(QColor::fromRgb(0, 0, 0));
    _graphicsItem->childItems().append(_propertiesItem);

    _textItem = new QGraphicsTextItem(_graphicsItem);
    _textItem->setPos(45, 10);
    _textItem->setDefaultTextColor(QColor::fromRgb(0, 0, 0));
    _textItem->setFont(newContext->mainFont());
    _graphicsItem->setPen(QPen(Qt::NoPen));
    _graphicsItem->childItems().append(_textItem);

    createIconInfo(_graphicsItem, 24, 30);

    createExtraAttrsIcon(_graphicsItem, 24);
    _iconExtraAttrs->setY(24);

    _iconType = new QGraphicsPixmapItem(_graphicsItem);
    _iconType->setPos(4, 4);

    _separator = new QGraphicsLineItem(_graphicsItem);
    _separator->setLine(0, 0, 20, 25);

    _typeItem = createTypeItem(_graphicsItem, newContext);

    connect(_graphicsItem, SIGNAL(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)), this, SLOT(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)));
}

XSchemaOutlineElement *OutlineElementItem::element() const
{
    return _item ;
}

void OutlineElementItem::setItem(XSchemaOutlineElement *newItem)
{
    QString annotationInfo;
    bool isOptional = false;
    bool showInfo = false;
    if(_item != newItem) {
        removeObject(_item);
        QString newName = "";
        QString newDescription = "";
        if(NULL != _item) {
            disconnect(_item, SIGNAL(nameChanged(QString)), this, SLOT(elmNameChanged(QString)));
            disconnect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
            //TODO disconnect(_item, SIGNAL(childRemoved(XSchemaObject*)), this, SLOT(childRemoved(XSchemaObject*)));
            //disconnect( _item, SIGNAL(itemChanged(QGraphicsItem::GraphicsItemChange,QVariant&)), this, SLOT(itemChanged(QGraphicsItem::GraphicsItemChange,QVariant&)));
        }
        _item = newItem ;
        if(NULL != newItem) {
            setObject(newItem, NULL);
            connect(_item, SIGNAL(nameChanged(QString)), this, SLOT(elmNameChanged(QString)));
            connect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
            // TODO connect(_item, SIGNAL(childRemoved(XSchemaObject*)), this, SLOT(childRemoved(XSchemaObject*)));
            //connect( _item, SIGNAL(itemChanged(QGraphicsItem::GraphicsItemChange,QVariant&)), this, SLOT(itemChanged(QGraphicsItem::GraphicsItemChange,QVariant&)));
            preAddChildren(_item);

            foreach(XSchemaObject * child, _item->getChildren()) {
                childAdded(child);
            }
            newName = _item->name();
            newDescription = _item->description();
            if(NULL != _item->annotation()) {
                showInfo = true ;
                annotationInfo = _item->annotation()->text();
            }
            if(_item->minOccurrences() == 0) {
                isOptional = true ;
            }
        }
        //_labelItem->setPlainText(newDescription);
    }
    setIconType();
    if(NULL != newItem) {

        // TODO: move the icons as appropriated
        Utils::TODO_THIS_RELEASE("fare");
        _textItem->setPlainText("_item->nameDescr()");
        QString occurences = "_item->occurrencesDescr()";
        QString type = "_item->xsdType()";
        QString simpleComplex = "_item->simpleComplexDescr()";

        bool hasProperties = false;
        if(!occurences.isEmpty()) {
            _propertiesItem->setPlainText(occurences);
            hasProperties = true;
        }
        if(!simpleComplex.isEmpty()) {
            if(!type.isEmpty()) {
                type = QString("%1, %2").arg(type).arg(simpleComplex);
            } else {
                type = QString("%1").arg(simpleComplex);
            }
        }
        _typeItem->setPlainText(type);

        Utils::TODO_THIS_RELEASE("fare");
        // simple type?
        bool isSimpleTypeInElements = true ; /*(newItem-> isTypeOrElement()
                                       && (newItem->category() == XSchemaElement::EES_SIMPLETYPE_ONLY)
                                       && (newItem->xsdParent()->getType() != SchemaTypeSchema));*/
        QRectF sizeUpper, sizeLower ;
        QList<QGraphicsItem*> upperItems;
        QList<QGraphicsItem*> lowerItems;

        doShowInfo(showInfo, annotationInfo, upperItems);
        if(_item->hasOtherAttributes()) {
            _iconExtraAttrs->setVisible(true);
            lowerItems.append(_iconExtraAttrs);
            _iconExtraAttrs->setToolTip(_item->getOhterAttrsAsTextList());
        }
        _propertiesItem->setVisible(hasProperties);
        if(hasProperties) {
            lowerItems.append(_propertiesItem);
        }

        upperItems.append(_textItem);
        upperItems.append(_typeItem);
        Utils::TODO_THIS_RELEASE("fare");
        /*if(! _item->ref().isEmpty()) {
            if(NULL == _iconLink) {
                _iconLink = new QGraphicsPixmapItem(_graphicsItem);
                QPixmap pixmap ;
                pixmap.load(PIXMAP_XSD_LINK);
                _iconLink->setPixmap(pixmap);
            }
        }*/
        upperItems.append(_iconLink) ;

        sizeUpper = disposeHorizontallyAndAlignLower(upperItems, 25, 4);
        sizeLower = disposeHorizontallyAndAlignLower(lowerItems, 25, 30);
        qreal width = sizeUpper.x() + std::max(sizeUpper.width(), sizeLower.width()) + 10;
        qreal height = sizeUpper.y() + sizeUpper.height() + sizeLower.height() + 4 ;//+ 10;

        _separator->setLine(20, 28, width - 10, 28);

        if(isSimpleTypeInElements) {
            QPainterPath path;
            path.moveTo(0, height);
            path.lineTo(0, 20);
            path.lineTo(30, 0);
            path.lineTo(width, 0);
            path.lineTo(width, height);
            path.lineTo(0, height);
            //_graphicsItem->setColor(QColor::fromRgb(.2, .6, .1));

            _contour = path.toFillPolygon();
            //_graphicsItem->setPolygon(_contour);
            _graphicsItem->setRect(QRect(0, 0, width, height));

        } else {
            QPainterPath path;
            path.moveTo(0, height);
            path.lineTo(0, 0);
            path.lineTo(width, 0);
            path.lineTo(width, height);
            path.lineTo(0, height);
            //_graphicsItem->setColor(QColor::fromRgb(.8, .4, .1));

            _contour = path.toFillPolygon();
            //_graphicsItem->setPolygon(_contour);
            _graphicsItem->setRect(QRect(0, 0, width, height));
        }

    } else {
        _textItem->setPlainText("");
        _propertiesItem->setPlainText("");
        _iconInfo->hide();
    }
    _graphicsItem->setOptional(isOptional);
    changeGraphics();
    buildTooltip();
}

void OutlineElementItem::setIconType()
{
    QPixmap pixmap ;
    if(NULL == _item) {
        _iconType->hide();
    }
    Utils::TODO_THIS_RELEASE("fare");
    /*if(_item->isTypeOrElement()) {
        pixmap.load(PIXMAP_XSD_TYPE);
    } else {
        pixmap.load(PIXMAP_XSD_ELEMENT);
    }*/
    _iconType->setPixmap(pixmap);
}

void OutlineElementItem::elmNameChanged(const QString &newName)
{
    _textItem->setPlainText(newName);
}

void OutlineElementItem::textChanged()
{
    if(NULL != _item) {
        _item->setName(_textItem->toPlainText());
    }
}

void OutlineElementItem::changeGraphics()
{
    if(NULL == _item) {
        return ;
    }
    //QLinearGradient gradient(0, 0, 0, 100);
    if(_isDiff) {
        setToolTipState(_graphicsItem, _item->compareState());
        setGradientColor(_graphicsItem, _item->compareState());
    } else {
        Utils::TODO_THIS_RELEASE("fare");
        /*if(_item->isTypeOrElement()) {
            _graphicsItem->setColorStart(QColor(237, 254, 250));
            _graphicsItem->setColorMiddle(QColor(205, 254, 241));
            _graphicsItem->setColorEnd(QColor(237, 254, 250));
        } else*/ {
            _graphicsItem->setColorStart(QColor(237, 250, 254));
            _graphicsItem->setColorMiddle(QColor(205, 241, 254));
            _graphicsItem->setColorEnd(QColor(237, 250, 254));
        }
    }
    Utils::TODO_THIS_RELEASE("fare");
    /*if(! _item->ref().isEmpty()) {
        if(NULL == _iconLink) {
            _iconLink = new QGraphicsPixmapItem(_graphicsItem);
            QPixmap pixmap ;
            pixmap.load(PIXMAP_XSD_LINK);
            _iconLink->setPixmap(pixmap);
        }
        if(_iconInfo->isVisible()) {
            _iconLink ->setPos(65, 6);
            _textItem->setPos(45, 10);
        } else {
            _iconLink->setPos(24, 6);
            _textItem->setPos(45, 10);
        }
    } else*/ {
        if(NULL != _iconLink) {
            _iconLink->hide();
            _textItem->setPos(25, 10);
        }
    }
}


void OutlineElementItem::itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant & /*value*/)
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

void OutlineElementItem::childAdded(XSchemaObject *newChild)
{
    if(NULL == newChild) {
        return ;
    }
    XSDItem *child = NULL ;
    /*ESchemaType type = newChild->getType()  ;
    if( SchemaTypeAttribute == type ) { TODO
        child = addAttribute( (XSchemaAttribute *)newChild, QPointF(0,0) );
    } else  {*/
    child = addChild(newChild);
    /*}*/
    if(NULL == child) {
        Utils::error(tr("An error occurred inserting the graphic item corresponding to the object."));
    }
}

