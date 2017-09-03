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


//------------

OutlineElementItem::OutlineElementItem(XsdGraphicContext *newContext, XSchemaOutlineElement *newItem, QGraphicsItem * /*parent*/)
    : XSDItem(newContext), _item(NULL),
      _graphicsItem(new GraphicsRoundRectItem(this)),
      _textItem(NULL)
{
    newContext->scene()->addItem(_graphicsItem);
    _graphicsItem->setData(XSD_ITEM_DATA, qVariantFromValue((void*)this));
    _graphicsItem->setUseDimShadow(true);
    init(newContext);
    setItem(newItem);
}

OutlineElementItem::~OutlineElementItem()
{
    reset();
}

void OutlineElementItem::reset()
{
}

void OutlineElementItem::init(XsdGraphicContext *newContext)
{
    _graphicsItem->setFlag(QGraphicsItem::ItemIsMovable, false);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    _graphicsItem->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    _graphicsItem->setPos(0, 0);

    _graphicsItem->setBrush(QBrush(QColor::fromRgb(255, 255, 255)));

    QPainterPath path;
    path.moveTo(0, 60);
    path.lineTo(0, 20);
    path.lineTo(30, 0);
    path.lineTo(200, 0);
    path.lineTo(200, 60);
    path.lineTo(0, 60);

    _contour = path.toFillPolygon();

    _textItem = new QGraphicsTextItem(_graphicsItem);
    _textItem->setPos(25, 10);
    _textItem->setDefaultTextColor(QColor::fromRgb(0, 0, 0));
    _textItem->setFont(newContext->mainFont());
    _graphicsItem->setPen(QPen(Qt::NoPen));
    _graphicsItem->childItems().append(_textItem);

    createIconInfo(_graphicsItem, 24, 30);
    _iconAttrs = new QGraphicsPixmapItem(_graphicsItem);
    _iconAttrs->hide();
    QPixmap pixmapInfo ;
    pixmapInfo.load(PIXMAP_XSD_ATTRIBUTE);
    _iconAttrs->setPixmap(pixmapInfo);

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
    bool showAttrib = false ;
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
            if(_item->minOccurrences() == 0) {
                isOptional = true ;
            }
        }
    }
    setIconType();
    if(NULL != newItem) {
        QString text = "<HTML><BODY><B>";

        text += Utils::escapeHTML(_item->name());
        text += "</B>";
        QString occurences = _item->occurrencesDescr();
        if(!occurences.isEmpty()) {
            text += "&nbsp;&nbsp;";
            text += "<FONT COLOR='red' size='-2'><I>";
            text += Utils::escapeHTML(occurences);
            text += "</I></FONT>";
        }
        if(newItem->isEnrolled()) {
            text += " (...)";
        }
        text += "</BODY></HTML>";
        _textItem->setHtml(text);

        QRectF sizeUpper;
        QList<QGraphicsItem*> upperItems;

        upperItems.append(_textItem);
        if(NULL != newItem->getElement()) {
            if(NULL != newItem->getElement()->annotation()) {
                annotationInfo = newItem->getElement()->annotation()->text();
            }
        }
        if(!annotationInfo.isEmpty()) {
            _textItem->setToolTip(annotationInfo);
            _graphicsItem->setToolTip(annotationInfo);
            showInfo = true ;
        }
        doShowInfo(showInfo, annotationInfo, upperItems);
        if(_item->hasAttributes()) {
            showAttrib = true ;
            upperItems.append(_iconAttrs);
            // make tooltip
            QString textAT = "<HTML><BODY><B>Attributes:</B><br/>";
            foreach(XSchemaObject *a, _item->attributes()) {
                switch(a->getType()) {
                default:
                    break;
                case SchemaTypeAttribute: {
                    XSchemaAttribute *attr = static_cast<XSchemaAttribute*>(a);
                    QString name ;
                    if(attr->referencedObjectName().isEmpty()) {
                        name = attr->name();
                    } else {
                        name = tr("reference to ") + attr->referencedObjectName();
                    }
                    textAT += "&nbsp;&nbsp;" + Utils::escapeHTML(name) + "<BR/>";
                }
                break;
                case SchemaTypeAttributeGroup: {
                    XSchemaAttributeGroup *ag = static_cast<XSchemaAttributeGroup*>(a);
                    textAT += "&nbsp;&nbsp;Group: " + Utils::escapeHTML(ag->referencedObjectName()) + "<BR/>";
                }
                break;
                }
            }
            textAT += "</BODY></HTML>";
            _iconAttrs->setToolTip(textAT);
        }
        _iconAttrs->setVisible(showAttrib);
        sizeUpper = disposeHorizontallyAndAlignLower(upperItems, 4, 1);
        qreal width = sizeUpper.x() + sizeUpper.width() + 6;
        qreal height = sizeUpper.y() + sizeUpper.height() + 4 ;//+ 10;
        _graphicsItem->setRect(QRect(0, 0, width, height));

    } else {
        _textItem->setPlainText("");
        _textItem->setToolTip("");
        _iconInfo->hide();
        _iconAttrs->setVisible(false);
    }
    _graphicsItem->setOptional(isOptional);
    changeGraphics();
}

void OutlineElementItem::setIconType()
{
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
    if(_isDiff) {
        setToolTipState(_graphicsItem, _item->compareState());
        setGradientColor(_graphicsItem, _item->compareState());
    } else {
        _graphicsItem->setSingleColor(true);
        _graphicsItem->setColorStart(QColor(255, 255, 255));
    }
    Utils::TODO_THIS_RELEASE("fare IS ENROLLED");
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
    child = addChild(newChild);
    if(NULL == child) {
        Utils::error(tr("An error occurred inserting the graphic item corresponding to the object."));
    }
}

//------------

OutlineSequenceItem::OutlineSequenceItem(XsdGraphicContext *newContext, XSchemaOutlineSequence *newItem, QGraphicsItem * /*parent*/)
    : XSDItem(newContext), _item(NULL),
      _graphicsItem(new GraphicsRectItem(this)), _labelItem(NULL)
{
    _extraSpace = 0 ;
    newContext->scene()->addItem(_graphicsItem);
    _graphicsItem->setData(XSD_ITEM_DATA, qVariantFromValue((void*)this));
    init(newContext);
    setItem(newItem);
}

OutlineSequenceItem::~OutlineSequenceItem()
{
    reset();
}

void OutlineSequenceItem::reset()
{
}

void OutlineSequenceItem::init(XsdGraphicContext * /*newContext*/)
{
    _contour = QRectF(0, 0, 30, 30);
    _graphicsItem->setRect(_contour);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsMovable, false);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    _graphicsItem->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    _graphicsItem->setPos(11, 0);

    _labelItem = new QGraphicsTextItem(_graphicsItem);
    _labelItem->setPlainText(labelText());
    _labelItem->setPos(0, 27);
    _graphicsItem->childItems().append(_labelItem);

    _icon = new QGraphicsPixmapItem(_graphicsItem);
    _icon ->setPos(5, 5);
    QPixmap pixmap ;
    pixmap.load(iconName());
    _icon->setPixmap(pixmap);

    createIconInfo(_graphicsItem, 24, 30);

    _graphicsItem->setBrush(QBrush(QColor::fromRgb(255, 255, 255)));
    connect(_graphicsItem, SIGNAL(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)), this, SLOT(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)));
}

qreal OutlineSequenceItem::extraSpace()
{
    return _extraSpace ;
}

QString OutlineSequenceItem::preTooltipString()
{
    return "Sequence";
}

void OutlineSequenceItem::setItem(XSchemaOutlineSequence *newItem)
{
    bool showInfo = false;
    QString descr = "" ;
    QString annotationInfo ;
    if(_item != newItem) {
        if(NULL != _item) {
            disconnect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
        }
        _item = newItem ;
        if(NULL != newItem) {
            connect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
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

    _labelItem->setHtml(
        QString("<HTML><BODY><FONT COLOR='red' size='-2'><I>%1</I></FONT></BODY></HTML>")
        .arg(Utils::escapeHTML(descr)));
    int posX = (30 - _labelItem->boundingRect().width()) / 2;
    _labelItem->setPos(posX, _labelItem->y());
    _extraSpace = _labelItem->boundingRect().width();
    if(_isDiff) {
        QLinearGradient gradient(0, 0, 0, 100);
        setGradientColor(gradient, _item->compareState());
        _graphicsItem->setBrush(QBrush(gradient));
    }
    buildTooltip();
}

void OutlineSequenceItem::itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant & /*value*/)
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

QString OutlineSequenceItem::labelText()
{
    return tr("Sequence");
}

QString OutlineSequenceItem::iconName()
{
    return PIXMAP_SEQUENCE_INFO;
}

XSchemaOutlineSequence *OutlineSequenceItem::sequence() const
{
    return (XSchemaOutlineSequence*)_item ;
}

//----------------------------------------

OutlineChoiceItem::OutlineChoiceItem(XsdGraphicContext *newContext, XSchemaOutlineChoice *newItem, QGraphicsItem * /*parent*/)
    : XSDItem(newContext), _item(NULL),
      _graphicsItem(new GraphicsRectItem(this)), _labelItem(NULL)
{
    _extraSpace = 0 ;
    newContext->scene()->addItem(_graphicsItem);
    _graphicsItem->setData(XSD_ITEM_DATA, qVariantFromValue((void*)this));
    init(newContext);
    setItem(newItem);
}

OutlineChoiceItem::~OutlineChoiceItem()
{
    reset();
}

void OutlineChoiceItem::reset()
{
}

void OutlineChoiceItem::init(XsdGraphicContext * /*newContext*/)
{
    _contour = QRectF(0, 0, 30, 30);
    _graphicsItem->setRect(_contour);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsMovable, false);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    _graphicsItem->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    _graphicsItem->setPos(11, 0);

    _labelItem = new QGraphicsTextItem(_graphicsItem);
    _labelItem->setPlainText(labelText());
    _labelItem->setPos(0, 27);
    _graphicsItem->childItems().append(_labelItem);

    _icon = new QGraphicsPixmapItem(_graphicsItem);
    _icon ->setPos(5, 5);
    QPixmap pixmap ;
    pixmap.load(iconName());
    _icon->setPixmap(pixmap);

    createIconInfo(_graphicsItem, 24, 30);

    _graphicsItem->setBrush(QBrush(QColor::fromRgb(255, 255, 255)));
    connect(_graphicsItem, SIGNAL(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)), this, SLOT(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)));
}

qreal OutlineChoiceItem::extraSpace()
{
    return _extraSpace ;
}

QString OutlineChoiceItem::preTooltipString()
{
    return "Choice";
}

void OutlineChoiceItem::setItem(XSchemaOutlineChoice *newItem)
{
    bool showInfo = false;
    QString descr = "" ;
    QString annotationInfo ;
    if(_item != newItem) {
        if(NULL != _item) {
            disconnect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
        }
        _item = newItem ;
        if(NULL != newItem) {
            connect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
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

    _labelItem->setHtml(
        QString("<HTML><BODY><FONT COLOR='red' size='-2'><I>%1</I></FONT></BODY></HTML>")
        .arg(Utils::escapeHTML(descr)));
    int posX = (30 - _labelItem->boundingRect().width()) / 2;
    _labelItem->setPos(posX, _labelItem->y());
    _extraSpace = _labelItem->boundingRect().width();
    if(_isDiff) {
        QLinearGradient gradient(0, 0, 0, 100);
        setGradientColor(gradient, _item->compareState());
        _graphicsItem->setBrush(QBrush(gradient));
    }
    buildTooltip();
}

void OutlineChoiceItem::itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant & /*value*/)
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

QString OutlineChoiceItem::labelText()
{
    return "Choice";
}

QString OutlineChoiceItem::iconName()
{
    return PIXMAP_CHOICE_INFO;
}

XSchemaOutlineChoice *OutlineChoiceItem::choice() const
{
    return _item ;
}

//----------------------------------------

OutlineAllItem::OutlineAllItem(XsdGraphicContext *newContext, XSchemaOutlineAll *newItem, QGraphicsItem * /*parent*/)
    : XSDItem(newContext), _item(NULL),
      _graphicsItem(new GraphicsRectItem(this)), _labelItem(NULL)
{
    _extraSpace = 0 ;
    newContext->scene()->addItem(_graphicsItem);
    _graphicsItem->setData(XSD_ITEM_DATA, qVariantFromValue((void*)this));
    init(newContext);
    setItem(newItem);
}

OutlineAllItem::~OutlineAllItem()
{
    reset();
}

void OutlineAllItem::reset()
{
}

void OutlineAllItem::init(XsdGraphicContext * /*newContext*/)
{
    _contour = QRectF(0, 0, 30, 30);
    _graphicsItem->setRect(_contour);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsMovable, false);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    _graphicsItem->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    _graphicsItem->setPos(11, 0);

    _labelItem = new QGraphicsTextItem(_graphicsItem);
    _labelItem->setPlainText(labelText());
    _labelItem->setPos(0, 27);
    _graphicsItem->childItems().append(_labelItem);

    createIconInfo(_graphicsItem, 24, 30);

    _graphicsItem->setBrush(QBrush(QColor::fromRgb(255, 255, 255)));
    connect(_graphicsItem, SIGNAL(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)), this, SLOT(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)));
}

qreal OutlineAllItem::extraSpace()
{
    return _extraSpace ;
}

QString OutlineAllItem::preTooltipString()
{
    return "All";
}

void OutlineAllItem::setItem(XSchemaOutlineAll *newItem)
{
    bool showInfo = false;
    QString descr = "" ;
    QString annotationInfo ;
    if(_item != newItem) {
        if(NULL != _item) {
            disconnect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
        }
        _item = newItem ;
        if(NULL != newItem) {
            connect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
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
    QString descrOther ;
    if(!descr.isEmpty()) {
        descrOther = QString(" <FONT COLOR='red' size='-2'><I>%1</I></FONT>")
                     .arg(Utils::escapeHTML(descrOther));
    }

    _labelItem->setHtml(
        QString("<HTML><BODY>All %1%2</BODY></HTML>")
        .arg(descrOther).arg(Utils::escapeHTML(_item->name())));
    _labelItem->setPos(2, 2);
    if(_graphicsItem->boundingRect().width() < _labelItem->boundingRect().width()) {
        QRectF rect = _graphicsItem->rect();
        _graphicsItem->setRect(rect.left(), rect.top(), _labelItem->boundingRect().width() + 4, rect.height());
    }

    _extraSpace = _labelItem->boundingRect().width();
    if(_isDiff) {
        _graphicsItem->setBrush(QBrush(QColor::fromRgb(255, 255, 255)));
    }
    buildTooltip();
}

void OutlineAllItem::itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant & /*value*/)
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

QString OutlineAllItem::labelText()
{
    return "All";
}

XSchemaOutlineAll *OutlineAllItem::all() const
{
    return _item ;
}

//----------------------------------------

OutlineGroupItem::OutlineGroupItem(XsdGraphicContext *newContext, XSchemaOutlineGroup *newItem, QGraphicsItem * /*parent*/)
    : XSDItem(newContext), _item(NULL),
      _graphicsItem(new GraphicsRectItem(this)), _labelItem(NULL)
{
    _extraSpace = 0 ;
    newContext->scene()->addItem(_graphicsItem);
    _graphicsItem->setData(XSD_ITEM_DATA, qVariantFromValue((void*)this));
    init(newContext);
    setItem(newItem);
}

OutlineGroupItem::~OutlineGroupItem()
{
    reset();
}

void OutlineGroupItem::reset()
{
}

void OutlineGroupItem::init(XsdGraphicContext * /*newContext*/)
{
    _contour = QRectF(0, 0, 30, 30);
    _graphicsItem->setRect(_contour);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsMovable, false);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    _graphicsItem->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    _graphicsItem->setPos(11, 0);

    _labelItem = new QGraphicsTextItem(_graphicsItem);
    _labelItem->setPlainText(labelText());
    _labelItem->setPos(0, 27);
    _graphicsItem->childItems().append(_labelItem);

    createIconInfo(_graphicsItem, 24, 30);

    _graphicsItem->setBrush(QBrush(QColor::fromRgb(255, 255, 255)));
    connect(_graphicsItem, SIGNAL(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)), this, SLOT(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)));
}

qreal OutlineGroupItem::extraSpace()
{
    return _extraSpace ;
}

QString OutlineGroupItem::preTooltipString()
{
    return "Group";
}

void OutlineGroupItem::setItem(XSchemaOutlineGroup *newItem)
{
    bool showInfo = false;
    QString descr = "" ;
    QString annotationInfo ;
    if(_item != newItem) {
        if(NULL != _item) {
            disconnect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
        }
        _item = newItem ;
        if(NULL != newItem) {
            connect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
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

    QString descrOther ;
    if(!descr.isEmpty()) {
        descrOther = QString(" <FONT COLOR='red' size='-2'><I>%1</I></FONT>")
                     .arg(Utils::escapeHTML(descrOther));
    }

    _labelItem->setHtml(
        QString("<HTML><BODY>Group %1%2</BODY></HTML>")
        .arg(descrOther).arg(Utils::escapeHTML(_item->name())));
    _labelItem->setPos(2, 2);
    if(_graphicsItem->boundingRect().width() < _labelItem->boundingRect().width()) {
        QRectF rect = _graphicsItem->rect();
        _graphicsItem->setRect(rect.left(), rect.top(), _labelItem->boundingRect().width() + 4, rect.height());
    }
    _extraSpace = _labelItem->boundingRect().width();
    if(_isDiff) {
        _graphicsItem->setBrush(QBrush(QColor::fromRgb(255, 255, 255)));
    }
    buildTooltip();
}

void OutlineGroupItem::itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant & /*value*/)
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

QString OutlineGroupItem::labelText()
{
    return "Group";
}

XSchemaOutlineGroup *OutlineGroupItem::group() const
{
    return _item ;
}

//----
//---


OutlineAnyItem::OutlineAnyItem(XsdGraphicContext *newContext, XSchemaOutlineAny *newItem, QGraphicsItem * /*parent*/)
    : XSDItem(newContext), _item(NULL),
      _graphicsItem(new GraphicsRectItem(this)), _labelItem(NULL)
{
    _extraSpace = 0 ;
    newContext->scene()->addItem(_graphicsItem);
    _graphicsItem->setData(XSD_ITEM_DATA, qVariantFromValue((void*)this));
    init(newContext);
    setItem(newItem);
}

OutlineAnyItem::~OutlineAnyItem()
{
    reset();
}

void OutlineAnyItem::reset()
{
}

void OutlineAnyItem::init(XsdGraphicContext * /*newContext*/)
{
    _contour = QRectF(0, 0, 30, 30);
    _graphicsItem->setRect(_contour);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsMovable, false);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    _graphicsItem->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    _graphicsItem->setPos(11, 0);

    _labelItem = new QGraphicsTextItem(_graphicsItem);
    _labelItem->setPlainText(labelText());
    _labelItem->setPos(0, 27);
    _graphicsItem->childItems().append(_labelItem);

    createIconInfo(_graphicsItem, 24, 30);

    _graphicsItem->setBrush(QBrush(QColor::fromRgb(255, 255, 255)));
    connect(_graphicsItem, SIGNAL(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)), this, SLOT(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)));
}

qreal OutlineAnyItem::extraSpace()
{
    return _extraSpace ;
}

QString OutlineAnyItem::preTooltipString()
{
    return "Any";
}

void OutlineAnyItem::setItem(XSchemaOutlineAny *newItem)
{
    bool showInfo = false;
    QString descr = "" ;
    QString annotationInfo ;
    if(_item != newItem) {
        if(NULL != _item) {
            disconnect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
        }
        _item = newItem ;
        if(NULL != newItem) {
            connect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
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

    QString descrOther ;
    if(!descr.isEmpty()) {
        descrOther = QString(" <FONT COLOR='red' size='-2'><I>%1</I></FONT>")
                     .arg(Utils::escapeHTML(descrOther));
    }

    _labelItem->setHtml(
        QString("<HTML><BODY>Any %1%2</BODY></HTML>")
        .arg(descrOther).arg(Utils::escapeHTML(_item->name())));
    _labelItem->setPos(2, 2);
    if(_graphicsItem->boundingRect().width() < _labelItem->boundingRect().width()) {
        QRectF rect = _graphicsItem->rect();
        _graphicsItem->setRect(rect.left(), rect.top(), _labelItem->boundingRect().width() + 4, rect.height());
    }
    _extraSpace = _labelItem->boundingRect().width();
    if(_isDiff) {
        _graphicsItem->setBrush(QBrush(QColor::fromRgb(255, 255, 255)));
    }
    buildTooltip();
}

void OutlineAnyItem::itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant & /*value*/)
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

QString OutlineAnyItem::labelText()
{
    return "Any";
}

XSchemaOutlineAny *OutlineAnyItem::any() const
{
    return _item ;
}

