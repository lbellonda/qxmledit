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


#include "xsdeditor/xsdwindow.h"
#include "utils.h"
#include <QGraphicsTextItem>
#include <QGraphicsSceneContextMenuEvent>
#include "xsdeditor/items/xitemsdefinitions.h"

//--------------------------------------------------------------------------------------
// TODO: remember to inject images in html with src: resources like that:<img src=':/xsdimages/link2'/>

//--------------------------------------------------------------------------------------

XItemMenuBuilder::XItemMenuBuilder(XSDItem * newItem)
{
    _item = newItem ;
    _rootMenuBuilder = NULL ;
}

XItemMenuBuilder::~XItemMenuBuilder()
{
}

void XItemMenuBuilder::setRootMenuBuilder(XMainMenuBuilder *rootMenuBuilder)
{
    _rootMenuBuilder = rootMenuBuilder ;
}

XMainMenuBuilder *XItemMenuBuilder::rootMenuBuilder()
{
    return _rootMenuBuilder ;
}


QMenu *XItemMenuBuilder::buildMenu()
{
    if(NULL != _rootMenuBuilder) {
        return _rootMenuBuilder->buildMenu(_item->item());
    }
    return NULL ;
}

//--------------------------------------------------------------------------------------

XSDItemContext::XSDItemContext(QXmlEditData *appData)
{
    _isDebug = false;
    _strategy = COMPACT;
    _renderingStrategy = appData->isXsdDisplayHoriz() ? DISPLAYSTR_NEW0 : DISPLAYSTR_UNDER;
    _gapBetweenChildren = 10 ;
    _stemLength = 50 ;
}

XSDItemContext::~XSDItemContext()
{
}


XSDItemContext::EDrawStrategy XSDItemContext::getStrategy()
{
    return _strategy ;
}

XSDItemContext::ESchemaRenderingStrategy XSDItemContext::renderingStrategy() const
{
    return _renderingStrategy;
}

void XSDItemContext::setRenderingStrategy(const ESchemaRenderingStrategy value)
{
    _renderingStrategy = value;
}

int XSDItemContext::gapBetweenChildren() const
{
    return _gapBetweenChildren;
}

void XSDItemContext::setGapBetweenChildren(int gapBetweenChildren)
{
    _gapBetweenChildren = gapBetweenChildren;
}

int XSDItemContext::stemLength() const
{
    return _stemLength;
}

void XSDItemContext::setStemLength(int stemLength)
{
    _stemLength = stemLength;
}

bool XSDItemContext::isDebug() const
{
    return _isDebug;
}

void XSDItemContext::setIsDebug(bool isDebug)
{
    _isDebug = isDebug;
}


//--------------------------------------------------------------------------------------

QColor XSDItem::colorEqualsStart = QColor::fromRgbF(.8, .8, .8, 1);
QColor XSDItem::colorEqualsEnd = QColor::fromRgbF(1, 1, 1, 1);
QColor XSDItem::colorAddStart = QColor::fromRgbF(0, 1, 0, 1);
QColor XSDItem::colorAddEnd = QColor::fromRgbF(.8, 1, .8, 1);
QColor XSDItem::colorDelStart = QColor::fromRgbF(1, 0, 0, 1) ;
QColor XSDItem::colorDelEnd = QColor::fromRgbF(1, .8, .5, 1) ;
QColor XSDItem::colorModStart = QColor::fromRgbF(1, .8, .5, 1) ;
QColor XSDItem::colorModEnd = QColor::fromRgbF(1, 1, .2, 1) ;
QColor XSDItem::colorInvalidStart = QColor::fromRgbF(1, 0, 1, 1) ;
QColor XSDItem::colorInvalidEnd = QColor::fromRgbF(1, .7, 1, 1) ;
//--------------------------------------------------------------------------------------

int XSDItem::_instances = 0 ;

XSDItem::XSDItem(XsdGraphicContext *newContext) : _menuBuilder(this), _iconInfo(NULL), _bounds(0, 0, 0, 0)
{
    _instances++;
    _id = _instances ;
    _childrenSizeInvalid = true ;
    _childrenHeight = 0 ;
    _realChildrenHeight = 0 ;
    _iconExtraAttrs = NULL ;
    _context = newContext;
    _chain = NULL ;
    _isBase = false;
    _isDiff = (NULL != newContext) ? newContext->contextType() == XsdGraphicContext::CONTEXT_DIFF : false ;
    _yToAdd = 0;
}

XSDItem::~XSDItem()
{
    //TODO removeObject( _item );
}

RChild *XSDItem::chain()
{
    return _chain ;
}

void XSDItem::setChain(RChild* newChain)
{
    _chain = newChain;
}

QList<XSchemaObject*> childrenSource()
{
    QList<XSchemaObject*> result ;

    return result;
}

void XSDItem::setRootContext(XsdGraphicContext *value)
{
    _context = value ;
    if(NULL != _context) {
        _menuBuilder.setRootMenuBuilder(value->menuBuilder());
    }
}

void XSDItem::afterPositionChange()
{
    RChild *theChain = chain();
    if(NULL != theChain) {
        chain()->updatePosition();
    }
    foreach(RChild * child, _children.children()) {
        child->item()->afterPositionChange();
    }
}

void XSDItem::setObject(XSchemaObject *newObject, XSchemaObject *oldObject)
{
    Q_ASSERT(_context != NULL);
    _context->removeObject(oldObject);
    _context->addObject(newObject, this);
}

void XSDItem::removeObject(XSchemaObject *object)
{
    Q_ASSERT(_context != NULL);
    _context->removeObject(object);
}

void XSDItem::doShowInfo(const bool showInfo, const QString &tipText, QList<QGraphicsItem*> &items)
{
    if(showInfo && !tipText.isEmpty()) {
        _iconInfo->setToolTip(tipText);
        _iconInfo->show();
        items.append(_iconInfo);
    } else {
        _iconInfo->hide();
    }
}

void XSDItem::createIconInfo(QGraphicsItem *theParent, const int xPos, const int yPos)
{
    if(NULL == _iconInfo) {
        _iconInfo = new QGraphicsPixmapItem(theParent);
        _iconInfo->hide();
        _iconInfo->setPos(xPos, yPos);
        QPixmap pixmapInfo ;
        pixmapInfo.load(PIXMAP_XSD_INFO);
        _iconInfo->setPixmap(pixmapInfo);
    }
}

void XSDItem::createExtraAttrsIcon(QGraphicsItem *parent, const int xPos)
{
    _iconExtraAttrs = new QGraphicsPixmapItem(parent);
    _iconExtraAttrs->hide();
    _iconExtraAttrs->setPos(xPos, 4);
    QPixmap pixmap ;
    pixmap.load(PIXMAP_XSD_EXTRA_ATTRS);
    _iconExtraAttrs->setPixmap(pixmap);
}

void XSDItem::setStdFontToItem(QGraphicsTextItem *gItem, const bool isBigger, const bool isBold, const bool isIalic)
{
    QFont fontElement(gItem->font());
    if(isIalic) {
        fontElement.setItalic(true);
    }
    if(isBold) {
        fontElement.setBold(true);
    }
    if(isBigger) {
        fontElement.setPointSize(10);
    } else {
        fontElement.setPointSize(0);
    }
    gItem ->setFont(fontElement);
}


QGraphicsTextItem *XSDItem::createTypeItem(QGraphicsItem *parent, XsdGraphicContext *context)
{
    QGraphicsTextItem  *gItem = new QGraphicsTextItem(parent);
    //setStdFontToItem(gItem, false, false, true);
    gItem->setFont(context->typeFont());
    gItem->setDefaultTextColor(QColor(0x80, 0x80, 0x80));
    return gItem;
}

void XSDItem::buildTooltip()
{
    if(_isDiff) {
        return ;
    }
    QString annotationInfo;
    if(NULL != item()) {
        if(NULL != item()->annotation()) {
            annotationInfo = item()->annotation()->text();
        }
    }
    QString preTooltip = preTooltipString();
    QString toolTip ;
    QString andStr ;
    if(!preTooltip.isEmpty() && !annotationInfo.isEmpty()) {
        andStr = "\n";
    }
    toolTip = preTooltip + andStr + annotationInfo ;
    graphicItem()->setToolTip(toolTip);
}


/**
  WARNING: this routine calls an asynchronous dispatch handler at the end of which
  the original item can even be destroyed, so after the exec() don't do anything that
  depends from this object or the caller
  */
void XSDItem::execMenuEvent(QGraphicsSceneContextMenuEvent *event, QGraphicsItem *item)
{
    if(NULL != item) {
        item->scene()->clearSelection();
        item->setSelected(true);
    }
    QMenu *menu = _menuBuilder.buildMenu();
    if(NULL != menu) {
        menu->exec(event->screenPos());
        delete menu;
    } else {
        Utils::error(tr("Cannot open menu"));
    }
    /** WARNING: DO NOT DO ANYTHING OTHER HERE
      */
}

void XSDItem::setGradientColor(QLinearGradient &gradient, const XSDCompareState::EXSDCompareState state)
{
    switch(state) {
    default:
        gradient.setColorAt(0, colorInvalidStart);
        gradient.setColorAt(1, colorInvalidEnd);
        break;
    case XSDCompareState::COMPARE_UNCHANGED:
        gradient.setColorAt(0, colorEqualsStart);
        gradient.setColorAt(1, colorEqualsEnd);
        break;
    case XSDCompareState::COMPARE_MODIFIED:
        gradient.setColorAt(0, colorModStart);
        gradient.setColorAt(1, colorModEnd);
        break;
    case XSDCompareState::COMPARE_ADDED:
        gradient.setColorAt(0, colorAddStart) ;
        gradient.setColorAt(1, colorAddEnd);
        break;
    case XSDCompareState::COMPARE_DELETED:
        gradient.setColorAt(0, colorDelStart);
        gradient.setColorAt(1, colorDelEnd);
        break;
    }
}

void XSDItem::setGradientColor(GraphicsRoundRectItem *item, const XSDCompareState::EXSDCompareState state)
{
    switch(state) {
    default:
        item->setColorStart(colorInvalidStart);
        item->setColorMiddle(colorInvalidEnd);
        item->setColorEnd(colorInvalidStart);
        break;
    case XSDCompareState::COMPARE_UNCHANGED:
        item->setColorStart(colorEqualsStart);
        item->setColorMiddle(colorEqualsEnd);
        item->setColorEnd(colorEqualsStart);
        break;
    case XSDCompareState::COMPARE_MODIFIED:
        item->setColorStart(colorModStart);
        item->setColorMiddle(colorModEnd);
        item->setColorEnd(colorModStart);
        break;
    case XSDCompareState::COMPARE_ADDED:
        item->setColorStart(colorAddStart);
        item->setColorMiddle(colorAddEnd);
        item->setColorEnd(colorAddStart);
        break;
    case XSDCompareState::COMPARE_DELETED:
        item->setColorStart(colorDelStart);
        item->setColorMiddle(colorDelEnd);
        item->setColorEnd(colorDelStart);
        break;
    }
}

QString XSDItem::preTooltipString()
{
    return "" ;
}

void XSDItem::setToolTipState(QGraphicsItem *item, const XSDCompareState::EXSDCompareState state)
{
    switch(state) {
    default:
        break;
    case XSDCompareState::COMPARE_UNCHANGED:
        item->setToolTip(tr("Unchanged"));
        break;
    case XSDCompareState::COMPARE_MODIFIED:
        item->setToolTip(tr("Modified"));
        break;
    case XSDCompareState::COMPARE_ADDED:
        item->setToolTip(tr("Added"));
        break;
    case XSDCompareState::COMPARE_DELETED:
        item->setToolTip(tr("Deleted"));
        break;
    }
}

XsdGraphicContext *XSDItem::context()
{
    return _context;
}

RChildren *XSDItem::rChildren()
{
    return &_children;
}

QString XSDItem::dumpAsString(const int indent)
{
    QString append = QString().fill(' ', indent) ;
    if(NULL != item()) {
        if(!item()->name().isEmpty()) {
            append += QString("%1 '%2'").arg(item()->typeString()).arg(item()->name()) ;
        } else {
            append += QString("%1").arg(item()->typeString()) ;
        }
    } else {
        append += "null";
    }
    append += "\n";

    RChildren *itemChildren = rChildren();
    if(NULL != itemChildren) {
        foreach(RChild* child, itemChildren->children()) {
            if(NULL != child->item()) {
                append += child->item()->dumpAsString(indent + 1);
            } else {
                append += "null\n";
            }
        }
    }
    return append ;
}

//--------------------------------------------------------------------------------------


/** to judge what item has to be created is not the duty of the graphic item.
  Any type created in the original structure is legal.
  */
XSDItem *XSDItem::factoryCreateItem(XSchemaObject *newChild, QGraphicsItem *parent)
{
    return createItem(_context, newChild, parent);
}

XSDItem *XSDItem::createItem(XsdGraphicContext *context, XSchemaObject *newChild, QGraphicsItem *parent)
{
    if(NULL == newChild) {
        return NULL ;
    }
    ESchemaType type = newChild->getType()  ;
    XSDItem* newItem ;
    // debug code qDebug(QString("type is %1 %2").arg(type).arg(newChild->typeString()).toLatin().data());
    if(SchemaTypeAttribute == type) {
        newItem = new AttributeItem(context, (XSchemaAttribute *)newChild, parent);
    } else if(SchemaTypeChoice  == type) {
        newItem = new ChoiceItem(context, (XSchemaChoice *)newChild, parent);
    } else if(SchemaTypeSequence  == type) {
        newItem = new SequenceItem(context, (XSchemaSequence *)newChild, parent);   //TODO
    } else if(SchemaTypeElement  == type) {
        newItem = new ElementItem(context, (XSchemaElement *)newChild, parent);
    } else if(SchemaTypeSimpleTypeRestriction == type) {
        newItem = new RestrictionItem(context, newChild, parent);
    } else if(SchemaTypeSimpleTypeList == type) {
        newItem = new ListItem(context, (XSchemaSimpleTypeList*)newChild, parent);
    } else if(SchemaTypeSimpleTypeUnion == type) {
        newItem = new UnionItem(context, (XSchemaSimpleTypeUnion*)newChild, parent);
    } else if(SchemaTypeInclude == type) {
        newItem = new IncludeItem(context, newChild, parent);
    } else if(SchemaTypeRedefine == type) {
        newItem = new RedefineItem(context, newChild, parent);
    } else if(SchemaTypeImport == type) {
        newItem = new ImportItem(context, newChild, parent);
    } else if(SchemaTypeNotation == type) {
        newItem = new GenericItem(context, newChild, parent);
    } else if(SchemaTypeAnnotation == type) {
        newItem = new GenericItem(context, newChild, parent);
    } else if(SchemaTypeGroup == type) {
        newItem = new GroupItem(context, newChild, parent);
    } else if(SchemaTypeAttributeGroup == type) {
        newItem = new AttributeGroupItem(context, newChild, parent);
    } else if(SchemaTypeComplexContentExtension == type) {
        newItem = new DerivationItem(context, newChild, parent);
    } else if(SchemaTypeComplexContentRestriction == type) {
        newItem = new DerivationItem(context, newChild, parent);
    } else if(SchemaContainer == type) {
        newItem = new ContainerItem(context, (XSchemaContainer*)newChild, parent);
    } else if(SchemaTypeAll == type) {
        newItem = new AllItem(context, (XSchemaAll*)newChild, parent);
    } else if(SchemaTypeSchema == type) {
        newItem = new RootItem(context, (XSDSchema*)newChild, parent);
    } else if(SchemaTypeOutlineElement == type) {
        newItem = new OutlineElementItem(context, (XSchemaOutlineElement*)newChild, parent);
    } else if(SchemaTypeOutlineSequence == type) {
        newItem = new OutlineSequenceItem(context, (XSchemaOutlineSequence*)newChild, parent);
    } else if(SchemaTypeOutlineChoice == type) {
        newItem = new OutlineChoiceItem(context, (XSchemaOutlineChoice*)newChild, parent);
    } else if(SchemaTypeOutlineAll == type) {
        newItem = new OutlineAllItem(context, (XSchemaOutlineAll*)newChild, parent);
    } else if(SchemaTypeOutlineGroup == type) {
        newItem = new OutlineGroupItem(context, (XSchemaOutlineGroup*)newChild, parent);
    } else if(SchemaTypeOutlineAny == type) {
        newItem = new OutlineAnyItem(context, (XSchemaOutlineAny*)newChild, parent);
    } else {
        newItem = new GenericItem(context, newChild, parent);
    }

    return newItem ;
}

QRectF XSDItem::measureOptimumDimensions(const int nItems, QGraphicsItem ** items)
{
    QRectF childrenBoundsRect(0, 0, 0, 0); // = graphicItem()->boundingRect();
    // the children list must be tailored on the particular item.
    int nIterations = nItems ;
    while(nIterations --) {
        if((NULL != *items) && ((*items)->isVisible())) {
            QRectF childRect = (*items)->boundingRect();
            QPointF origin = (*items)->pos();
            childRect.translate(origin);
            QRectF chTemp = childrenBoundsRect.united(childRect);
            childrenBoundsRect = chTemp ;
        }
        items++;
    }
    //QRectF childrenBounds = graphicItem()->childrenBoundingRect() ;
    if(childrenBoundsRect.isEmpty()) {
        childrenBoundsRect = graphicItem()->boundingRect();
    }
    return childrenBoundsRect ;
}

QRectF XSDItem::measureOptimumDimensions(QList<QGraphicsItem*> &items)
{
    QRectF childrenBoundsRect(0, 0, 0, 0); // = graphicItem()->boundingRect();
    // the children list must be tailored on the particular item.
    foreach(QGraphicsItem * item, items) {
        if((NULL != item) && item->isVisible()) {
            QRectF childRect = item->boundingRect();
            QPointF origin = item->pos();
            childRect.translate(origin);
            QRectF chTemp = childrenBoundsRect.united(childRect);
            childrenBoundsRect = chTemp ;
        }
    }
    if(childrenBoundsRect.isEmpty()) {
        childrenBoundsRect = graphicItem()->boundingRect();
    }
    return childrenBoundsRect ;
}

QRectF XSDItem::disposeHorizontallyAndAlignLower(QList<QGraphicsItem*> &items, const int xPos, const int yPos)
{
    int horPos = xPos;
    QRectF childrenBoundsRect(xPos, yPos, 0, 4);
    int maxHeight = 0 ;
    // the children list must be tailored on the particular item.
    foreach(QGraphicsItem * item, items) {
        if((NULL != item) && item->isVisible()) {
            QRectF childRect = item->boundingRect();
            maxHeight = std::max(maxHeight, (int)childRect.height());
            item->setPos(horPos, yPos);
            QPointF origin = item->pos();
            childRect.translate(origin);
            horPos += childRect.width() ;//+ 2;
            QRectF chTemp = childrenBoundsRect.united(childRect);
            childrenBoundsRect = chTemp ;
        }
    }

    foreach(QGraphicsItem * item, items) {
        if((NULL != item) && item->isVisible()) {
            QRectF childRect = item->boundingRect();
            item->setY(yPos + maxHeight - childRect.height());
        }
    }

    if(childrenBoundsRect.isEmpty()) {
        childrenBoundsRect = graphicItem()->boundingRect();
    }
    return childrenBoundsRect ;
}


XSDItem *XSDItem::addChild(XSchemaObject *child)
{
    XSDItem *xsdItem = factoryCreateItem(child, graphicItem());
    if(NULL != xsdItem) {
        xsdItem->setRootContext(_context);
        _children.init(this);
        // set the position of the new item
        newChildPosition(xsdItem->graphicItem());

        if(!_children.createChild(this, xsdItem)) {
            delete xsdItem ;
            xsdItem = NULL ;
        }

    }
    return xsdItem ;
}

qreal XSDItem::recalcChildrenPos(XSDItemContext *context)
{
    switch(context->renderingStrategy()) {
    default:
    case XSDItemContext::DISPLAYSTR_UNDER:
        return recalcChildrenPosStrategyUnder(context);
    case XSDItemContext::DISPLAYSTR_HOR_PYRAMID:
        return calcChildrenHeightAndDisposeStrategyHorPyramid(context);
    case XSDItemContext::DISPLAYSTR_NEW0:
        return calcChildrenHeightAndDisposeStrategyHorPyramidNew0(context);
    }
}


qreal XSDItem::calcChildrenHeightAndDisposeStrategyHorPyramidNew0(XSDItemContext *context)
{
    qreal value = placeAllStrategyHorPyramidNew0(context);
    return value ;
}

qreal XSDItem::calcChildrenHeightAndDisposeStrategyHorPyramid(XSDItemContext *context)
{
    qreal value = calcChildrenHeightStrategyHorPyramid(context);
    reDisposeAllStrategyHorPyramid(context);
    return value ;
}

qreal XSDItem::calcChildrenHeightStrategyHorPyramid(XSDItemContext *context)
{
    if(true || _childrenSizeInvalid) {
        _childrenHeight = 0 ;
        qreal lastChildBounds = 0 ;
        bool isFirst = true;
        foreach(RChild * rchild, _children.children()) {
            XSDItem *xsdItem = rchild->item();

            if(isFirst) {
                isFirst = false;
            } else {
                _childrenHeight += context->gapBetweenChildren() ;
            }
            lastChildBounds = xsdItem->calcChildrenHeightStrategyHorPyramid(context) ;
            _childrenHeight += lastChildBounds;
        }
        _childrenSizeInvalid = false;
    }
    // now center the children in the bounding rect
    qreal thisHeight = graphicItem()->boundingRect().height();
    _realChildrenHeight = _childrenHeight ;
    if(_childrenHeight < thisHeight) {
        _childrenHeight = thisHeight ;
    }
    _childrenHeight += offsetHeight();
    _childrenHeight += marginBottom();
    //QString str = QString("DEBUG CODE: %3\nChildren count:%1, height:%2").arg(_children.children().size()).arg(_childrenHeight).arg(graphicItem()->toolTip());
    //graphicItem()->setToolTip(str);
    return _childrenHeight ;
}

qreal XSDItem::offsetHeight()
{
    return 0 ;
}

qreal XSDItem::marginBottom()
{
    return 0;
}

qreal XSDItem::extraSpace()
{
    return 0;
}

void XSDItem::afterDisposeAllChildren()
{
}


void XSDItem::afterDispose()
{
    // nothing
}

QRectF XSDItem::calcDependentBounds()
{
    QRectF thisBounds(-1, -1, -1, -1);
    foreach(RChild * rchild, _children.children()) {
        XSDItem *xsdItem = rchild->item();
        QRectF childBounds = xsdItem->graphicItem()->boundingRect();
        childBounds.moveTo(xsdItem->graphicItem()->pos());
        if(!thisBounds.isValid()) {
            thisBounds = childBounds;
        } else {
            thisBounds = thisBounds.united(childBounds);
        }
        QRectF depBounds = xsdItem->calcDependentBounds();
        if(depBounds.isValid()) {
            if(!thisBounds.isValid()) {
                thisBounds = depBounds;
            } else {
                thisBounds = thisBounds.united(depBounds);
            }
        }
    }
    return thisBounds;
}

qreal XSDItem::recalcChildrenPosStrategyUnder(XSDItemContext *context)
{
    QGraphicsItem *parentItem = graphicItem() ;
    QPointF posParent = parentItem->scenePos();
    QPointF offset = posParent;
    QRectF bounds = parentItem->boundingRect();
    offset.setX(offset.x() + 3 * bounds .width() / 4);
    switch(context->getStrategy()) {
    default:
    case XSDItemContext::ONE_AFTER:
        offset.setY(offset.y() + bounds .height() + YGAP);
        break;
    case XSDItemContext::COMPACT:
        offset.setY(offset.y() + bounds .height());
        break;
    }
    // finds the total height
    qreal totalHeight = 0;
    qreal startY = offset.y();
    qreal lastHeight = 0 ;
    qreal lastChildBounds = 0 ;
    foreach(RChild * rchild, _children.children()) {
        XSDItem *xsdItem = rchild->item();

        QGraphicsItem *child = xsdItem->graphicItem();
        offset.setY(startY + totalHeight);
        QPointF newPosGlobal(offset.x(), offset.y());

        child->setPos(newPosGlobal);
        lastHeight = xsdItem->recalcChildrenPos(context);
        totalHeight += lastHeight + 10 ;
        lastChildBounds = child->boundingRect().height() ;
    }
    startY = offset.y() - (totalHeight / 2);
    if(startY < 0) {
        startY = 0 ;
    }

    if(NULL != _children._line) {
        qreal posX = parentItem->x() - bounds.left() + bounds.width() / 2 ;

        switch(context->getStrategy()) {
        default:
        case XSDItemContext::ONE_AFTER:
            _children._line->setLine(posX, parentItem->y() - bounds.top() + bounds.height(),
                                     posX, parentItem->y() - bounds.top() + bounds.height() + totalHeight - lastHeight - 10 + (lastChildBounds / 2) + YGAP);
            break;
        case XSDItemContext::COMPACT:
            _children._line->setLine(posX, parentItem->y() - bounds.top() + bounds.height(),
                                     posX, parentItem->y() - bounds.top() + bounds.height() + totalHeight - lastHeight - 10 + (lastChildBounds / 2));
            break;
        }

    }
    return totalHeight + bounds.height();
}

void XSDItem::totalSize(QRectF &totalSize)
{
    QGraphicsItem *parentItem = graphicItem() ;
    QRectF boundsInParentCoordinated = parentItem->mapToParent(parentItem->boundingRect()).boundingRect();
    totalSize = totalSize.united(boundsInParentCoordinated);
    foreach(RChild * child, _children.children()) {
        QRectF childTotalSize(0, 0, 0, 0);
        child->item()->totalSize(childTotalSize);
        // converti il pos in top
        QRectF inThisCoords = parentItem->mapToParent(childTotalSize).boundingRect();
        totalSize = inThisCoords.united(totalSize);
    }
}

QString XSDItem::itemLabelForChart()
{
    return "?";
}

QColor XSDItem::itemColorForChart()
{
    return QColor::fromRgb(0xE0, 0xE0, 0xE0);
}

QColor XSDItem::itemColorForGroupsForChart()
{
    return QColor::fromRgb(0xFF, 0x80, 0x40);
}

void XSDItem::newChildPosition(QGraphicsItem *newChild)
{
    QGraphicsItem *parentItem = graphicItem() ;
    QPointF posParent = parentItem->scenePos();
    QPointF offset = posParent;
    QRectF bounds = parentItem->boundingRect();
    offset.setX(offset.x() + bounds .width());
    offset.setY(offset.y() + bounds .height());
    // finds the total height
    qreal totalHeight = 0 ;
    foreach(RChild * rchild, _children.children()) {
        QGraphicsItem *child = rchild->item()->graphicItem();
        if(newChild != child)  {
            totalHeight += child->boundingRect().height();
        }
        totalHeight += 10 ;
    }
    qreal startY = offset.y() - (totalHeight / 2);
    if(startY < 0) {
        startY = 0 ;
    }
    foreach(RChild * rchild, _children.children()) {
        QGraphicsItem *child = rchild->item()->graphicItem();
        if(newChild != child)  {
            qreal height = child->boundingRect().height();
            offset.setY(startY + height);
            QPointF newPosGlobal(offset.x(), startY);
            QPointF newPos = child->mapToScene(newPosGlobal);
            child->setPos(newPos);
            startY += height ;
            startY += 10 ;
        }
    }

    QPointF newPosGlobal(offset.x(), startY);
    QPointF newPos = newChild->mapToScene(newPosGlobal);
    newChild->setPos(newPos);

    if(NULL != _children._line) {
        qreal posX = posParent.x() + bounds.width() / 2 ;
        _children._line->setLine(posX, posParent.y() + bounds.height(),
                                 posX, startY + newChild->boundingRect().height() / 2);

    }
}

void XSDItem::remove(XSchemaObject *child)
{
    int indexOfChild = _children.findObject(child);
    if(indexOfChild >= 0) {
        _children.deleteAt(indexOfChild);
    }
}


void XSDItem::preAddChildren(XSchemaObject *object)
{
    if(_context->isOutline()) {
        ; //nothing
    } else if(_context->isShowBaseObjects()) {
        QList<XSchemaObject*> baseAttributes;
        QList<XSchemaObject*> baseElements;
        bool isOk = object->findBaseObjects(_context->searchContext(), baseElements, baseAttributes);
        if(!isOk) {
            Utils::error(tr("Error collecting information on base types for:").append(_context->searchContext().typeErrors().join(",")));
            _context->searchContext().resetErrors();
        } else {
            foreach(XSchemaObject * child, baseElements) {
                childAdded(child);
            }
            foreach(XSchemaObject * child, baseAttributes) {
                childAdded(child);
            }
        }
    }
}

void XSDItem::childAdded(XSchemaObject* newChild)
{
    if(NULL == newChild) {
        return ;
    }
    XSDItem *child = addChild(newChild);
    if(NULL == child) {
        Utils::error(tr("An error occurred inserting the graphic item corresponding to the object."));
    }
}


//--------------------------------------------------------------------------------------

int RChild::instances = 0;
int RChild::totalInstances = 0;
RChild::RChild(XSDItem *newParent, XSDItem *newItem, LineItem *newLine)
{
    instances++;
    totalInstances++;
    _item = newItem ;
    _line = newLine ;
    _parent = newParent ;
    _id = instances;
    //printf("Now++ %d, id:%d\n", totalInstances, _id);
    //fflush(stdout);
}

RChild::~RChild()
{

    if(NULL != _line) {
        delete _line ;
        _line = NULL ;
    }
    if(NULL != _item) {
        delete _item ;
        _item = NULL ;
    }
    _parent = NULL ;
    totalInstances--;
    //printf("--Now %d, id:%d\n", totalInstances, _id);
    //fflush(stdout);
}

void RChild::updatePosition()
{
    if(NULL != _line) {
        _line->updatePosition(_parent->context()->itemContext());
    }
}

LineItem *RChild::line()
{
    return _line ;
}

int RChildren::findObject(XSchemaObject *object)
{
    foreach(RChild * child, _children) {
        if(child->item()->item() == object) {
            return _children.indexOf(child);
        }
    }
    return -1 ;
}

void RChildren::deleteAt(const int &position)
{
    RChild *child = _children.at(position);
    _children.removeAt(position);
    if(NULL != child) {
        delete child ;
    }
}

//--------------------------------------------------------------------------------------

RootItem::RootItem(XsdGraphicContext *newContext, XSDSchema *newSchema, QGraphicsItem *parent) : XSDItem(newContext), _graphicsItem(new PolygonItem(this, false)), _item(NULL)
{
    _graphicsItem->setData(XSD_ITEM_DATA, qVariantFromValue((void*)this));
    if(NULL != parent) {
        newContext->scene()->addItem(_graphicsItem);
    }
    init(newContext);
    if(NULL != newSchema) {
        setItem(newSchema);
    }
}

RootItem::~RootItem()
{
}

void RootItem::init(XsdGraphicContext *context)
{
    QPainterPath path;
    path.addRoundedRect(0, 0, 80, 50, 16, 16);
    _contour = path.toFillPolygon();
    _graphicsItem->setPolygon(_contour);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsMovable, false);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    _graphicsItem->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    _graphicsItem->setPos(0, 0);
    _graphicsItem->setBrush(QBrush(QColor(0xFF, 0xFF, 0xFF)));
    QGraphicsTextItem *textItem = new QGraphicsTextItem(_graphicsItem);
    if(NULL != textItem) {
        textItem->setPlainText(tr("Root"));
        textItem->setPos(10, 4);
        _graphicsItem->childItems().append(textItem);
        textItem->setDefaultTextColor(QColor::fromRgb(0, 0, 0));
        //setStdFontToItem(textItem, false, false, false);
        textItem->setFont(context->normalFont());
    }
}


XSDSchema *RootItem::schema() const
{
    return _item ;
}

void RootItem::setItem(XSDSchema *newItem)
{
    if(_item != newItem) {
        if(NULL != _item) {
            disconnect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
            disconnect(_item, SIGNAL(deleted(XSchemaObject*)), this, SLOT(deleted(XSchemaObject*)));
        }
        _item = newItem ;
        if(NULL != newItem) {
            connect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
            connect(_item, SIGNAL(childRemoved(XSchemaObject*)), this, SLOT(childRemoved(XSchemaObject*)));
            connect(_item, SIGNAL(deleted(XSchemaObject*)), this, SLOT(objectDeleted(XSchemaObject*)));

            // follow the elements childrens
            foreach(XSchemaObject * child, _item->getChildren()) {
                childAdded(child);
            }
            {
                /*XSchemaRoot *root = static_cast<XSchemaRoot *>(_item);
                XSDSchema *schema = root->schema();
                QString tooltip = QString("targetNamespace=\"%4\"\nnamespacePrefix=\"%1\"\nElements=%2\nAttributes=%3")
                                  .arg(root->namespacePrefix())
                                  .arg(schema->elementsQualifiedString())
                                  .arg(schema->attributesQualifiedString())
                                  .arg(schema->targetNamespace());
                foreach(QString ns, schema->allNamespaces()) {
                    tooltip += QString("\nnamespace: " + ns);
                }

                _graphicsItem->setToolTip(tooltip);*/
            }
        }
    }
    buildTooltip();
}

QString RootItem::itemLabelForChart()
{
    return "Schema";
}


QString RootItem::preTooltipString()
{
    if(NULL == _item) {
        return "" ;
    }
    XSchemaRoot *root = static_cast<XSchemaRoot *>(_item);
    XSDSchema *schema = root->schema();
    QString tooltip = QString("targetNamespace=\"%4\"\nnamespacePrefix=\"%1\"\nElements=%2\nAttributes=%3")
                      .arg(root->namespacePrefix())
                      .arg(schema->elementsQualifiedString())
                      .arg(schema->attributesQualifiedString())
                      .arg(schema->targetNamespace());
    foreach(QString ns, schema->allNamespaces()) {
        tooltip += QString("\nnamespace: " + ns);
    }
    return tooltip;
}

void RootItem::objectDeleted(XSchemaObject* /*self*/)
{
    //TODO _graphicsItem->childItems().clear(); // NO, DEVO FARE UN REMOVE!
    _graphicsItem->scene()->removeItem(_graphicsItem);
    delete this;
}

void RootItem::childRemoved(XSchemaObject* child)
{
    remove(child);
}


//--------------------------------------------------------------------------------------

RootOutlineItem::RootOutlineItem(XsdGraphicContext *newContext, XSDSchema *newSchema, QGraphicsItem *parent) : RootItem(newContext, newSchema, parent)
{
}

RootOutlineItem::~RootOutlineItem()
{
    removeModel();
}

void RootOutlineItem::init(XsdGraphicContext *context)
{
    QPainterPath path;
    path.addRoundedRect(0, 0, 80, 50, 16, 16);
    _contour = path.toFillPolygon();
    _graphicsItem->setPolygon(_contour);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsMovable, false);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    _graphicsItem->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    _graphicsItem->setPos(0, 0);
    _graphicsItem->setBrush(QBrush(QColor(0xFF, 0xFF, 0xFF)));
    QGraphicsTextItem *textItem = new QGraphicsTextItem(_graphicsItem);
    if(NULL != textItem) {
        textItem->setPlainText(tr("Root"));
        textItem->setPos(10, 4);
        _graphicsItem->childItems().append(textItem);
        textItem->setDefaultTextColor(QColor::fromRgb(0, 0, 0));
        //setStdFontToItem(textItem, false, false, false);
        textItem->setFont(context->normalFont());
    }
}

void RootOutlineItem::setItem(XSDSchema *newItem)
{
    if(_item != newItem) {
        if(NULL != _item) {
            disconnect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
            disconnect(_item, SIGNAL(deleted(XSchemaObject*)), this, SLOT(deleted(XSchemaObject*)));
            removeModel();
        }
        _item = newItem ;
        if(NULL != newItem) {
            connect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
            connect(_item, SIGNAL(childRemoved(XSchemaObject*)), this, SLOT(childRemoved(XSchemaObject*)));
            connect(_item, SIGNAL(deleted(XSchemaObject*)), this, SLOT(objectDeleted(XSchemaObject*)));

            // follow the elements childrens
            outlineModeChildren();
            {
                XSDSchema *schema = newItem;
                QString tooltip = QString("targetNamespace=\"%4\"\nnamespacePrefix=\"%1\"\nElements=%2\nAttributes=%3")
                                  .arg(schema->root()->namespacePrefix())
                                  .arg(schema->elementsQualifiedString())
                                  .arg(schema->attributesQualifiedString())
                                  .arg(schema->targetNamespace());
                foreach(QString ns, schema->allNamespaces()) {
                    tooltip += QString("\nnamespace: " + ns);
                }

                _graphicsItem->setToolTip(tooltip);
            }
        }
    }
    buildTooltip();
}

void RootOutlineItem::outlineModeChildren()
{
    removeModel();
    if(NULL != schema()) {
        XSchemaInquiryContext context;
        context.setHonorRestrictions(true);
        context.setFullCollection(true);
        if(_chosenRoot.isEmpty()) {
            QList<XSchemaElement *> items = schema()->collectCandidateRootElement();
            foreach(XSchemaElement * child, items) {
                XSchemaOutlineElement *element = new XSchemaOutlineElement(child->xsdParent(), child->root());
                element->setElement(context, child);
                _outlineItems.append(element);
                childAdded(element);
            }
        } else {
            XSchemaElement *newRoot = schema()->topLevelElement(_chosenRoot);
            if(NULL != newRoot) {
                XSchemaOutlineElement *element = new XSchemaOutlineElement(newRoot->xsdParent(), newRoot->root());
                element->setElement(context, newRoot);
                _outlineItems.append(element);
                childAdded(element);
            }
        }
    }
}

void RootOutlineItem::removeModel()
{
    EMPTYPTRLIST(_outlineItems, XSchemaOutlineContainer);
}

void RootOutlineItem::objectDeleted(XSchemaObject* /*self*/)
{
    //TODO _graphicsItem->childItems().clear(); // NO, DEVO FARE UN REMOVE!
    _graphicsItem->scene()->removeItem(_graphicsItem);
    delete this;
}

void RootOutlineItem::childRemoved(XSchemaObject* child)
{
    remove(child);
}

QString RootOutlineItem::chosenRoot() const
{
    return _chosenRoot;
}

void RootOutlineItem::setChosenRoot(const QString &chosenRoot)
{
    _chosenRoot = chosenRoot;
}

//--------------------------------------------------------------------------------------

ContainerItem::ContainerItem(XsdGraphicContext *newContext, XSchemaContainer *newItem, QGraphicsItem * /*parent*/) : XSDItem(newContext), _graphicsItem(new GraphicsRectItem(NULL)), _boundsItem(NULL), _item(NULL)
{
    _debugLine = NULL ;
    _topOffsetForLabel = 0 ;
    _marginBottom = 10 ;
    newContext->scene()->addItem(_graphicsItem);
    _graphicsItem->setData(XSD_ITEM_DATA, qVariantFromValue((void*)this));
    init(newContext);
    setItem(newItem);
}

ContainerItem::~ContainerItem()
{
    if(NULL != _item) {
        delete _item ;
    }
}

void ContainerItem::init(XsdGraphicContext *context)
{
    _graphicsItem->setFlag(QGraphicsItem::ItemIsMovable, false);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    _graphicsItem->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    _graphicsItem->setRect(0, 0, 1, 1);

    QPainterPath path;
    path.addRoundedRect(0, 0, 80, 50, 16, 16);
    _contour = path.toFillPolygon();
    _boundsItem = new PolygonItem(this, false);
    _boundsItem->setPolygon(_contour);
    _boundsItem->setFlag(QGraphicsItem::ItemIsMovable, false);
    _boundsItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    _boundsItem->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    _boundsItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    _boundsItem->setPos(0, 0);
    _boundsItem->setBrush(QBrush(QColor(0xC0, 0xC0, 0xC0)));
    _label = new TextItem(_boundsItem);
    if(NULL != _label) {
        _label->setPos(10, 4);
        _label->setDefaultTextColor(QColor::fromRgb(0, 0, 0));
        _label->setFont(context->normalFont());
    }
    _boundsItem->setZValue(_graphicsItem->zValue() - 10);
    _context->scene()->addItem(_boundsItem);
}


qreal ContainerItem::offsetHeight()
{
    return _topOffsetForLabel;
}

qreal ContainerItem::marginBottom()
{
    return _marginBottom;
}

void ContainerItem::setItem(XSchemaContainer *newItem)
{
    QString newLabel = "";
    if(_item != newItem) {
        if(NULL != _item) {
            disconnect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
            disconnect(_item, SIGNAL(deleted(XSchemaObject*)), this, SLOT(deleted(XSchemaObject*)));
            delete _item;
        }
        _item = newItem ;
        if(NULL != newItem) {
            connect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
            connect(_item, SIGNAL(childRemoved(XSchemaObject*)), this, SLOT(childRemoved(XSchemaObject*)));
            connect(_item, SIGNAL(deleted(XSchemaObject*)), this, SLOT(objectDeleted(XSchemaObject*)));
            // follow the elements childrens
            foreach(XSchemaObject * child, _item->existingChildren()) {
                childAdded(child);
            }
            newLabel = newItem->label();
        }
    }
    _label->setPlainText(newLabel);
    QRectF labelRect = _label->boundingRect();
    _topOffsetForLabel = labelRect.height() + ContainerLabelPad ;
    buildTooltip();
}

void ContainerItem::objectDeleted(XSchemaObject* /*self*/)
{
    _graphicsItem->scene()->removeItem(_graphicsItem);
    delete this;
}

void ContainerItem::childRemoved(XSchemaObject* child)
{
    remove(child);
}

void ContainerItem::afterDispose()
{
    QRectF dependentBounds = calcDependentBounds();
    if(dependentBounds.isValid()) {
        // add self
        QPainterPath path;
        path.addRoundedRect(0, 0, dependentBounds.width() + ContainerLabelPad, dependentBounds.height() + 2 * ContainerLabelPad, 16, 16);
        _contour = path.toFillPolygon();
        _boundsItem->setPolygon(_contour);
        _boundsItem->setPos(graphicItem()->x(), dependentBounds.top() - 2 * ContainerLabelPad);
        if(_context->isDebug()) {
            if(NULL == _debugLine) {
                _debugLine = new QGraphicsLineItem(graphicItem());
            }
            if(NULL != _debugLine) {
                _debugLine->setPos(0, 0);
                _debugLine->setLine(0, 0, 200, 0);
                _debugLine->setPen(QPen(QColor::fromRgb(255, 0, 0)));
            }
        }
    }
}

void ContainerItem::afterDisposeAllChildren()
{
    if(NULL != _label) {
        _label->setPos(10, 4);
    }
}

QString ContainerItem::itemLabelForChart()
{
    QString name ;
    if(NULL != _item) {
        name = _item->name();
    }
    return name ;
}

QColor ContainerItem::itemColorForChart()
{
    return XSDItem::itemColorForChart();
}

//--------------------------------------------------------------------------------------

RChildren::RChildren()
{
    _line = NULL;
    _secondLine = NULL ;
}

RChildren::~RChildren()
{
    reset();
}

void RChildren::reset()
{
    foreach(RChild * child, _children) {
        delete child;
    }
    _children.clear();
}

void RChildren::showChildLine()
{
    if(!_children.isEmpty()) {
        RChild *first = _children.first();
        if((NULL != first->line()) && !first->line()->isVisible()) {
            first->line()->show();
        }
    }
}

void RChildren::suppressChildLine()
{
    if(!_children.isEmpty()) {
        RChild *first = _children.first();
        if((NULL != first->line()) && first->line()->isVisible()) {
            first->line()->hide();
        }
    }
}

bool RChildren::init(XSDItem * parent)
{
    if(NULL == _line) {
        QGraphicsItem *parentItem = parent->graphicItem();
        _line = new QGraphicsLineItem(NULL);
        if(NULL == _line) {
            return false;
        }
        QGraphicsScene *scene = parentItem->scene() ;
        if(NULL != scene) {
            scene->addItem(_line);
            return true ;
        }
    }
    return true ;
}

int RChildren::childrenSize()
{
    return _children.size();
}

RChild* RChildren::childAt(const int index)
{
    if((index >= 0) && (index < _children.size())) {
        return _children.at(index);
    }
    return NULL ;
}

QGraphicsLineItem *RChildren::secondLine(XSDItem *parent)
{
    if(NULL == _secondLine) {
        QGraphicsItem *parentItem = parent->graphicItem();
        _secondLine = new QGraphicsLineItem(NULL);
        QGraphicsScene *scene = parentItem->scene() ;
        if(NULL != scene) {
            scene->addItem(_secondLine);
        }
    }
    return _secondLine ;
}

bool RChildren::createChild(XSDItem *parent, XSDItem *item)
{
    QGraphicsItem *childItem = item->graphicItem();
    QGraphicsItem *parentItem = parent->graphicItem();

    init(parent);

    LineItem *line = new LineItem(parentItem, childItem/*, parent->graphicItem()*/);
    if(NULL != line) {
        RChild *child = new RChild(parent, item, line);
        if(NULL != child) {
            QGraphicsScene *scene = parentItem->scene() ;
            if(NULL != scene) {
                // item should be already added scene->addItem(childItem);
                scene->addItem(line);
            }
            _children.append(child);
            line->setZValue(std::min(parentItem->zValue() - 1, childItem->zValue()) - 1);
            item->setChain(child);
        } else {
            delete line ;
            line  = NULL ;
        }
    }
    return (NULL != line) ;
}


//--------------------------------------------------------------------------------


ElementItem::ElementItem(XsdGraphicContext *newContext, XSchemaElement *newItem, QGraphicsItem * /*parent*/)
    : XSDItem(newContext), _item(NULL),
      _graphicsItem(new GraphicsRoundRectItem(this)),
      _textItem(NULL), _propertiesItem(NULL), _iconLink(NULL), _separator(NULL)
{
    newContext->scene()->addItem(_graphicsItem);
    _graphicsItem->setToolTip("TEST TOOLTIP");
    _graphicsItem->setData(XSD_ITEM_DATA, qVariantFromValue((void*)this));
    init(newContext);
    setItem(newItem);
}

ElementItem::~ElementItem()
{
    /* if( NULL != _textItem ) {
         //TODO disconnect(_textItem, SIGNAL(itemChange(GraphicsItemChange,QVariant&)), this, SLOT(textChanged()) );
     }*/
    reset();
}

void ElementItem::reset()
{
    /*foreach( AttributeItem* attribute, _attributes){
        delete attribute ;
    }
    _attributes.clear();*/
    //_graphicsItem->childItems().clear();
    //_graphicsItem->scene()->removeItem(_graphicsItem);
    //_graphicsItem->childItems().clear();
}

void ElementItem::init(XsdGraphicContext *newContext)
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

    //TODO _textItem->setTextInteractionFlags(Qt::TextEditable);
    _textItem = new QGraphicsTextItem(_graphicsItem);
    _textItem->setPos(45, 10);
    _textItem->setDefaultTextColor(QColor::fromRgb(0, 0, 0));
    //setStdFontToItem(_textItem, false, false, false);
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

    //_graphicsItem->scene()->addItem(&_textItem);
    //TODO connect(_textItem, SIGNAL(itemChanged(GraphicsItemChange, QVariant&)), this, SLOT(textChanged()));
    connect(_graphicsItem, SIGNAL(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)), this, SLOT(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)));
}

XSchemaElement *ElementItem::element() const
{
    return _item ;
}

void ElementItem::setItem(XSchemaElement *newItem)
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
            if(!_context->isHideAttributes() || !context()->isOutline()) {
                foreach(XSchemaObject * attr, _item->attributes()) {
                    childAdded(attr);
                }
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
        _textItem->setPlainText(_item->nameDescr());
        QString occurences = _item->occurrencesDescr();
        QString type = _item->xsdType();
        QString simpleComplex = _item->simpleComplexDescr();

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

        // simple type?
        bool isSimpleTypeInElements = (newItem-> isTypeOrElement()
                                       && (newItem->category() == XSchemaElement::EES_SIMPLETYPE_ONLY)
                                       && (newItem->xsdParent()->getType() != SchemaTypeSchema));
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
        if(! _item->ref().isEmpty()) {
            if(NULL == _iconLink) {
                _iconLink = new QGraphicsPixmapItem(_graphicsItem);
                QPixmap pixmap ;
                pixmap.load(PIXMAP_XSD_LINK);
                _iconLink->setPixmap(pixmap);
            }
        }
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

void ElementItem::setIconType()
{
    QPixmap pixmap ;
    if(NULL == _item) {
        _iconType->hide();
    }
    if(_item->isTypeOrElement()) {
        pixmap.load(PIXMAP_XSD_TYPE);
    } else {
        pixmap.load(PIXMAP_XSD_ELEMENT);
    }
    _iconType->setPixmap(pixmap);
}

void ElementItem::elmNameChanged(const QString &newName)
{
    _textItem->setPlainText(newName);
}

void ElementItem::textChanged()
{
    if(NULL != _item) {
        _item->setName(_textItem->toPlainText());
    }
}

void ElementItem::changeGraphics()
{
    if(NULL == _item) {
        return ;
    }
    //QLinearGradient gradient(0, 0, 0, 100);
    if(_isDiff) {
        setToolTipState(_graphicsItem, _item->compareState());
        setGradientColor(_graphicsItem, _item->compareState());
    } else {

        if(_item->isTypeOrElement()) {
            _graphicsItem->setColorStart(QColor(237, 254, 250));
            _graphicsItem->setColorMiddle(QColor(205, 254, 241));
            _graphicsItem->setColorEnd(QColor(237, 254, 250));
        } else {
            _graphicsItem->setColorStart(QColor(237, 250, 254));
            _graphicsItem->setColorMiddle(QColor(205, 241, 254));
            _graphicsItem->setColorEnd(QColor(237, 250, 254));
        }
    }
    if(! _item->ref().isEmpty()) {
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
    } else {
        if(NULL != _iconLink) {
            _iconLink->hide();
            _textItem->setPos(25, 10);
        }
    }
}


void ElementItem::itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant & /*value*/)
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

void ElementItem::childAdded(XSchemaObject *newChild)
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

QString ElementItem::itemLabelForChart()
{
    if(NULL == _item) {
        return "";
    }
    if(_item->isTypeOrElement()) {
        return QString("element: %1 %2").arg(_item->nameOrReference()).arg(_item->occurrencesDescr());
    } else {
        return QString("type: %1").arg(_item->nameOrReference());
    }
}

QColor ElementItem::itemColorForChart()
{
    if(NULL == _item) {
        return "";
    }
    if(_item->isTypeOrElement()) {
        return QColor::fromRgb(0x00, 0xC0, 0xFF);
    } else {
        return QColor::fromRgb(0x00, 0xFF, 0x00);
    }
}

//--------------------------------------------------------------------------

AttributeItem::AttributeItem(XsdGraphicContext *newContext, XSchemaAttribute *newItem, QGraphicsItem * /*parent*/)
    : XSDItem(newContext),
      _item(NULL), _graphics(new GraphicsRoundRectItem(this)), _textItem(NULL)
{
    newContext->scene()->addItem(_graphics);
    _graphics->setData(XSD_ITEM_DATA, qVariantFromValue((void*)this));
    init();
    setItem(newItem);
}

AttributeItem::~AttributeItem()
{
    //if( NULL != _textItem ) {
    //TODO disconnect(_textItem, SIGNAL(itemChange(GraphicsItemChange,QVariant&)), this, SLOT(textChanged()) );
    //}
}

void AttributeItem::init()
{
    _contour = QRectF(0, 0, 80, 20);
    _graphics->setRect(_contour);
    _graphics->setFlag(QGraphicsItem::ItemIsMovable, false);
    _graphics->setFlag(QGraphicsItem::ItemIsSelectable, true);
    _graphics->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    _graphics->setPos(0, 0);

    /*QLinearGradient gradient(0, 0, 0, 100);
    gradient.setColorAt(0, QColor::fromRgbF(1, 1, .5, 1));
    gradient.setColorAt(1, QColor::fromRgbF(1, 1, .7, 0.9));*/
    _graphics->setColorStart(QColor::fromRgbF(1, 1, .5, 1));
    _graphics->setColorMiddle(QColor::fromRgbF(1, 1, .7, 0.9));
    _graphics->setColorEnd(QColor::fromRgbF(1, 1, .5, 1));

    //_graphics->setBrush(QBrush(gradient));
    _textItem = new TextItem(_graphics);
    _textItem->setPos(24, 0);
    _textItem->setDefaultTextColor(QColor::fromRgb(0, 0, 0));

    _iconType = new QGraphicsPixmapItem(_graphics);
    _iconType ->setPos(4, 4);
    QPixmap pixmap ;
    pixmap.load(PIXMAP_XSD_ATTRIBUTE);
    _iconType->setPixmap(pixmap);

    createIconInfo(_graphics, 24, 4);
    createExtraAttrsIcon(_graphics, 24);

    //_textItem->setTextInteractionFlags(Qt::TextEditable);
    _graphics->childItems().append(_textItem);
    //TODO connect(_textItem, SIGNAL(itemChange(GraphicsItemChange, QVariant&)), this, SLOT(textChanged()));
}

XSchemaAttribute *AttributeItem::item() const
{
    return _item ;
}

void AttributeItem::setItem(XSchemaAttribute *newItem)
{
    bool isOptional = false ;
    QString annotationInfo ;
    if(_item != newItem) {
        QString newName = "";
        if(NULL != _item) {
            removeObject(_item);
            disconnect(_item, SIGNAL(nameChanged(QString)), this, SLOT(nameChanged(QString)));
            //TODO
        }
        _item = newItem ;
        if(NULL != newItem) {
            setObject(newItem, NULL);
            connect(_item, SIGNAL(nameChanged(QString)), this, SLOT(nameChanged(QString)));
            preAddChildren(_item);
            foreach(XSchemaObject * child, _item->getChildren()) {
                childAdded(child);
            }
            newName = _item->description();
            if(NULL != _item->annotation()) {
                annotationInfo = _item->annotation()->text();
            }
            if(_item->use() == XSchemaAttribute::Optional) {
                isOptional = true;
            }
        }
        _textItem->setPlainText(tr("%1").arg(newName));
    }

    if(_isDiff) {
        //QLinearGradient gradient(0, 0, 0, 100);
        //setGradientColor(gradient, _item->compareState());
        //_graphics->setBrush(QBrush(gradient));
        setGradientColor(_graphics, _item->compareState());
        setToolTipState(_graphics, _item->compareState());
    }

    QList<QGraphicsItem*> items;
    items.append(_iconType);
    doShowInfo(true, annotationInfo, items);

    if(_item->hasOtherAttributes()) {
        _iconExtraAttrs->setVisible(true);
        items.append(_iconExtraAttrs);
        _iconExtraAttrs->setToolTip(_item->getOhterAttrsAsTextList());
    }
    items.append(_textItem);

    QRectF size = disposeHorizontallyAndAlignLower(items, 4, 4);
    qreal width = size.x() + size.width() + 4;
    qreal height = size.y() + size.height() + 4;
    _contour = QRectF(0, 0, width, height);
    _graphics->setRect(_contour);
    _graphics->setOptional(isOptional);
    buildTooltip();
}

void AttributeItem::childAdded(XSchemaObject *newChild)
{
    if(NULL == newChild) {
        return ;
    }
    if(newChild->getType() == SchemaTypeElement) {
        // look for childrens (this is a simple type with restrictions and so on )
        QList<XSchemaObject*> elementChildren = newChild->getChildren();
        if(!elementChildren.isEmpty()) {
            // att: non deve essere una annotation, ma un simple type
            XSchemaObject* st = elementChildren.at(0);
            addChild(st);
        }
    } else {
        addChild(newChild);
    }
}


void AttributeItem::nameChanged(const QString &newName)
{
    _textItem->setPlainText(newName);
}

void AttributeItem::textChanged()
{
    if(/*( NULL != _textItem ) &&*/ (NULL != _item)) {
        _item->setName(_textItem->toPlainText());
    }
}

QString AttributeItem::itemLabelForChart()
{
    return tr("attribute: %1").arg(_textItem->toPlainText());
}

QColor AttributeItem::itemColorForChart()
{
    return QColor::fromRgb(0xFF, 0xFF, 0xC0);
}

//--------------------------------------------------------------------------------------

GenericItem::GenericItem(XsdGraphicContext *newContext, XSchemaObject *newItem, QGraphicsItem * /*parent*/)
    : XSDItem(newContext), _item(NULL), _graphicsItem(new PolygonItem(this)),
      _icon(NULL), _textItem(NULL)
{
    newContext->scene()->addItem(_graphicsItem);
    _graphicsItem->setData(XSD_ITEM_DATA, qVariantFromValue((void*)this));
    init(newContext);
    setItem(newItem);
}

GenericItem::~GenericItem()
{
    /* if( NULL != _textItem ) {
         //TODO disconnect(_textItem, SIGNAL(itemChange(GraphicsItemChange,QVariant&)), this, SLOT(textChanged()) );
     }*/
    reset();
}

void GenericItem::reset()
{
    /*foreach( AttributeItem* attribute, _attributes){
        delete attribute ;
    }
    _attributes.clear();*/
}


void GenericItem::init(XsdGraphicContext *newContext)
{
    //_contour = QRectF( 0, 0, 100, 100 );
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

    /*_icon = new QGraphicsPixmapItem(_graphicsItem);
    _icon->setPos(10, 10 );*/
    setGraphics(QColor::fromRgbF(1, 1, 1, 1));

    _textItem = new QGraphicsTextItem(_graphicsItem);
    _textItem->setPos(30, 10);
    _textItem->setDefaultTextColor(QColor::fromRgb(0, 0, 0));
    //setStdFontToItem(_textItem, false, false, false);
    _textItem->setFont(newContext->normalFont());

    createIconInfo(_graphicsItem);
    createExtraAttrsIcon(_graphicsItem, 24);

    //_graphicsItem->childItems().append(&_textItem);
    //_graphicsItem->scene()->addItem(&_textItem);
    //TODO connect( &_textItem, SIGNAL(itemChanged(GraphicsItemChange,QVariant&)), this, SLOT(textChanged()) );
    connect(_graphicsItem, SIGNAL(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)), this, SLOT(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)));
}

/* TODO
XSchemaObject *GenericItem::item() const
{
    return _item ;
}
*/

void GenericItem::setGraphics(/*const QPixmap & pixmap,*/ QColor bkColor)
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

void GenericItem::setItem(XSchemaObject *newItem)
{
    QString annotationInfo;
    bool showInfo = false;
    if(_item != newItem) {
        if(NULL != _item) {
            removeObject(_item);
            //TODO disconnect( _item, SIGNAL(nameChanged(QString)), this, SLOT(elmNameChanged(QString)) );
            disconnect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
            //TODO disconnect(_item, SIGNAL(childRemoved(XSchemaObject*)), this, SLOT(childRemoved(XSchemaObject*)));
            //disconnect( _item, SIGNAL(itemChanged(QGraphicsItem::GraphicsItemChange,QVariant&)), this, SLOT(itemChanged(QGraphicsItem::GraphicsItemChange,QVariant&)));
        }
        _item = newItem ;
        if(NULL != newItem) {
            setObject(_item, NULL);
            //TODO connect( _item, SIGNAL(nameChanged(QString)), this, SLOT(elmNameChanged(QString)) );
            connect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
            //TODO connect(_item, SIGNAL(childRemoved(XSchemaObject*)), this, SLOT(childRemoved(XSchemaObject*)));
            //connect( _item, SIGNAL(itemChanged(QGraphicsItem::GraphicsItemChange,QVariant&)), this, SLOT(itemChanged(QGraphicsItem::GraphicsItemChange,QVariant&)));
            _textItem->setPlainText(newItem->description());
            setGraphics(/*newItem->icon(),*/ newItem->color());
            foreach(XSchemaObject * child, _item->getChildren()) {
                childAdded(child);
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

void GenericItem::itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant & /*value*/)
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

QString GenericItem::itemLabelForChart()
{
    QString name ;
    QString tag ;
    if(NULL != _item) {
        name = _item->name();
        tag = _item->tagName();
    }
    return QString("%1 %2").arg(tag).arg(name) ;
}

QColor GenericItem::itemColorForChart()
{
    return XSDItem::itemColorForChart();
}

//--------------------------------------------------------------------------------------

AllItem::AllItem(XsdGraphicContext *newContext, XSchemaAll *newItem, QGraphicsItem * /*parent*/)
    : XSDItem(newContext), _item(NULL), _graphicsItem(new PolygonItem(this)),
      _icon(NULL), _textItem(NULL)
{
    newContext->scene()->addItem(_graphicsItem);
    _graphicsItem->setData(XSD_ITEM_DATA, qVariantFromValue((void*)this));
    init(newContext);
    setItem(newItem);
}

AllItem::~AllItem()
{
    reset();
}

void AllItem::reset()
{
}

void AllItem::init(XsdGraphicContext *newContext)
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

void AllItem::setGraphics(/*const QPixmap & pixmap,*/ QColor bkColor)
{
    QLinearGradient gradient(0, 0, 0, 100);
    QColor clr2(bkColor);
    clr2 = clr2.lighter();
    clr2.setAlpha(100);
    gradient.setColorAt(0, bkColor);
    gradient.setColorAt(1, clr2);
    _graphicsItem->setBrush(QBrush(gradient));
}

void AllItem::setItem(XSchemaObject *newItem)
{
    QString annotationInfo;
    bool showInfo = false;
    if(_item != newItem) {
        if(NULL != _item) {
            removeObject(_item);
            //TODO disconnect( _item, SIGNAL(nameChanged(QString)), this, SLOT(elmNameChanged(QString)) );
            disconnect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
            //TODO disconnect(_item, SIGNAL(childRemoved(XSchemaObject*)), this, SLOT(childRemoved(XSchemaObject*)));
            //disconnect( _item, SIGNAL(itemChanged(QGraphicsItem::GraphicsItemChange,QVariant&)), this, SLOT(itemChanged(QGraphicsItem::GraphicsItemChange,QVariant&)));
        }
        _item = (XSchemaAll*)newItem ;
        if(NULL != newItem) {
            setObject(_item, NULL);
            //TODO connect( _item, SIGNAL(nameChanged(QString)), this, SLOT(elmNameChanged(QString)) );
            connect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
            //TODO connect(_item, SIGNAL(childRemoved(XSchemaObject*)), this, SLOT(childRemoved(XSchemaObject*)));
            //connect( _item, SIGNAL(itemChanged(QGraphicsItem::GraphicsItemChange,QVariant&)), this, SLOT(itemChanged(QGraphicsItem::GraphicsItemChange,QVariant&)));
            _textItem->setPlainText(newItem->description());
            setGraphics(/*newItem->icon(),*/ newItem->color());
            foreach(XSchemaObject * child, _item->getChildren()) {
                childAdded(child);
            }
            if(NULL != _item->annotation()) {
                showInfo = true ;
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

void AllItem::itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant & /*value*/)
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

QString AllItem::itemLabelForChart()
{
    QString name ;
    if(NULL != _item) {
        name = _item->description();
    }
    return QString("all %1").arg(name) ;
}

QColor AllItem::itemColorForChart()
{
    return XSDItem::itemColorForChart();
}

//----------------------------------------------------------------------------------------------------------

DerivationItem::DerivationItem(XsdGraphicContext *newContext, XSchemaObject *newItem, QGraphicsItem * /*parent*/)
    : XSDItem(newContext), _item(NULL), _graphicsItem(new PolygonItem(this)),
      _icon(NULL), _textItem(NULL), _iconLink(NULL)
{
    newContext->scene()->addItem(_graphicsItem);
    _graphicsItem->setData(XSD_ITEM_DATA, qVariantFromValue((void*)this));
    init(newContext);
    setItem(newItem);
}

DerivationItem::~DerivationItem()
{
    reset();
}

void DerivationItem::reset()
{
}


void DerivationItem::init(XsdGraphicContext *newContext)
{
    _contour = QRectF(0, 0, 100, 100);
    /*QPainterPath path;
    path.moveTo(150, 50);
    path.arcTo(100, 0, 50, 50, 0, 90);
    path.arcTo(0, 0, 50, 50, 90, 90);
    path.arcTo(0, 50, 50, 50, 180, 90);
    path.arcTo(100, 50, 50, 50, 270, 90);
    path.lineTo(150, 25);
    _contour = path.toFillPolygon();*/
    _graphicsItem->setPolygon(_contour);

    _graphicsItem->setFlag(QGraphicsItem::ItemIsMovable, false);
    _graphicsItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    _graphicsItem->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    _graphicsItem->setPos(0, 0);

    setGraphics(QColor::fromRgbF(1, 1, 1, 1));

    _textItem = new QGraphicsTextItem(_graphicsItem);
    _textItem->setPos(30, 10);
    _textItem->setDefaultTextColor(QColor::fromRgb(0, 0, 0));
    _textItem->setFont(newContext->normalFont());

    createIconInfo(_graphicsItem);
    createExtraAttrsIcon(_graphicsItem, 24);

    connect(_graphicsItem, SIGNAL(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)), this, SLOT(itemChanged(QGraphicsItem::GraphicsItemChange, const QVariant&)));
}

void DerivationItem::setGraphics(/*const QPixmap & pixmap,*/ QColor bkColor)
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

void DerivationItem::setItem(XSchemaObject *newItem)
{
    QString annotationInfo;
    bool showInfo = false;
    if(_item != newItem) {
        if(NULL != _item) {
            removeObject(_item);
            //TODO disconnect( _item, SIGNAL(nameChanged(QString)), this, SLOT(elmNameChanged(QString)) );
            disconnect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
            //TODO disconnect(_item, SIGNAL(childRemoved(XSchemaObject*)), this, SLOT(childRemoved(XSchemaObject*)));
            //disconnect( _item, SIGNAL(itemChanged(QGraphicsItem::GraphicsItemChange,QVariant&)), this, SLOT(itemChanged(QGraphicsItem::GraphicsItemChange,QVariant&)));
        }
        _item = newItem ;
        if(NULL != newItem) {
            setObject(_item, NULL);
            //TODO connect( _item, SIGNAL(nameChanged(QString)), this, SLOT(elmNameChanged(QString)) );
            connect(_item, SIGNAL(childAdded(XSchemaObject*)), this, SLOT(childAdded(XSchemaObject*)));
            //TODO connect(_item, SIGNAL(childRemoved(XSchemaObject*)), this, SLOT(childRemoved(XSchemaObject*)));
            //connect( _item, SIGNAL(itemChanged(QGraphicsItem::GraphicsItemChange,QVariant&)), this, SLOT(itemChanged(QGraphicsItem::GraphicsItemChange,QVariant&)));
            _textItem->setPlainText(newItem->description());
            setGraphics(/*newItem->icon(),*/ newItem->color());
            foreach(XSchemaObject * child, _item->getChildren()) {
                childAdded(child);
            }
            if(NULL != _item->annotation()) {
                showInfo = true ;
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
    XSchemaComplexContent *ccont = (XSchemaComplexContent *)_item;
    if(ccont->hasAReference()) {
        if(NULL == _iconLink) {
            _iconLink = new QGraphicsPixmapItem(_graphicsItem);
            QPixmap pixmap ;
            pixmap.load(PIXMAP_XSD_LINK);
            _iconLink->setPixmap(pixmap);
        }
        if(_iconLink->isVisible()) {
            items.append(_iconLink);
        }
    } else {
        if(NULL != _iconLink) {
            _iconLink->hide();
        }
    }

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

void DerivationItem::itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant & /*value*/)
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

QString DerivationItem::itemLabelForChart()
{
    QString name ;
    if(NULL != _item) {
        name = _item->description();
    }
    return QString("derivation %1").arg(name) ;
}

QColor DerivationItem::itemColorForChart()
{
    return XSDItem::itemColorForChart();
}

//----------------------------------------------------------------------------------------------------------

void updateScenePosition(QGraphicsItem * /*item*/)
{
}


qreal XSDItem::calcYPosTop(qreal yPosExt, qreal offsetY)
{
    QRectF bounds = graphicItem()->boundingRect();
    qreal result = yPosExt - bounds.top() + offsetY;
    return result;
}

qreal XSDItem::calcYPosHalf(qreal yPosExt, qreal offsetY)
{
    QRectF bounds = graphicItem()->boundingRect();
    qreal result = yPosExt - bounds.top() + bounds.height() / 2 + offsetY;
    return result;
}


void XSDItem::reDisposeAllStrategyHorPyramid(XSDItemContext *context)
{
    qreal overallHeight = calcChildrenHeightStrategyHorPyramid(context);
    QGraphicsItem *thisItem = graphicItem() ;
    QRectF bounds = thisItem->boundingRect();
    int rootYPosition = (overallHeight) / 2 - bounds.top() - bounds.height() / 2;
    disposeObjectHorPyramid(context, 0, 0, rootYPosition);
}


void XSDItem::disposeObjectHorPyramid(XSDItemContext *context, const int level, const qreal xPos, const qreal yPos)
{
    /*if(!strcmp(this->metaObject()->className(), "ContainerItem")) {
        int debug = 1;
        debug++;
    }*/
    QGraphicsItem *thisItem = graphicItem() ;
    qreal allChildrenHeight = _childrenHeight - offsetHeight();
    qreal yPosThis = yPos;
    thisItem->setPos(xPos, yPosThis);
    QRectF bounds = thisItem->boundingRect();
    qreal xOffset = xPos + 2 * bounds.width() + context->stemLength() / 4 + extraSpace();
    qreal yFirstPos = 0;
    // finds the total height
    qreal minYPosChildren = yPos - bounds.top() + bounds.height() / 2 - allChildrenHeight / 2 + offsetHeight() ;
    qreal maxYPosChildren = minYPosChildren ;
    bool isFirst = true;
    /*QString log = QString("level:%1, name=%2, posY=%3, height=%4, minPos=%5, overall=%6").arg(level).arg(item()->name()).arg(yPos).arg(bounds.height()).arg(minYPosChildren).arg(_childrenHeight);
    printf( "%s\n", log.toLatin1().data());
    fflush(stdout);*/

    qreal itemYPos = minYPosChildren;
    /*if(context->isDebug() && (level < 3)) {
        QGraphicsLineItem *lineStart = new QGraphicsLineItem(thisItem);
        int lineStartY = bounds.height() / 2 - allChildrenHeight / 2;
        lineStart->setLine(bounds.width(), lineStartY, bounds.width() * 2, lineStartY);
        lineStart->setPen(QPen(QColor::fromRgb(255, 0, 255)));
        int lineEndY = bounds.height() / 2 + allChildrenHeight / 2;
        QGraphicsLineItem *lineEnd = new QGraphicsLineItem(thisItem);
        lineEnd->setLine(bounds.width(), lineEndY, bounds.width() * 2, lineEndY);
        lineEnd->setPen(QPen(QColor::fromRgb(192, 192, 192)));
    }*/
    foreach(RChild * rchild, _children.children()) {
        XSDItem *xsdItem = rchild->item();
        int childHeight  = xsdItem->calcChildrenHeightStrategyHorPyramid(context);
        QRectF childBounds = xsdItem->graphicItem()->boundingRect();

        int realItemYPos = itemYPos + childHeight / 2 - childBounds.top() - childBounds.height() / 2;
        xsdItem->disposeObjectHorPyramid(context, level + 1, xOffset, realItemYPos);

        maxYPosChildren = realItemYPos + childBounds.height() / 2 ;
        if(isFirst) {
            isFirst = false ;
            minYPosChildren = maxYPosChildren;
            yFirstPos = realItemYPos + childBounds.height() / 2;
        } else {
            itemYPos += context->gapBetweenChildren();
        }
        xsdItem->afterDispose();
        itemYPos += childHeight ;
    }
    QGraphicsLineItem *line = _children.secondLine(this);
    if(NULL != line) {
        if(_children.children().size() > 1) {
            line->show();
            qreal lastXParent = xPos + bounds.width() ;
            qreal posX = lastXParent + (xOffset - lastXParent) / 2 ;
            //QPointF thisPos = thisItem->scenePos();
            //fprintf(stdout, " this is %d %d, min child:%d - %d", (int)thisPos.x(), (int)thisPos.y(), (int)minYPosChildren, (int)maxYPosChildren );
            //fflush(stdout);
            line->setLine(posX, minYPosChildren, posX, maxYPosChildren);
        } else {
            line->hide();
        }
    }
    QGraphicsLineItem *secondLine = _children._line;
    if(NULL != secondLine) {
        if(_children.children().size() > 0) {
            secondLine->show();
            qreal lastXParent = xPos + bounds.width() ;
            qreal posX = lastXParent + (xOffset - lastXParent) / 2 ;
            qreal yPosLine = yPosThis + bounds.height() / 2 + offsetHeight();
            if(_children.children().size() == 1) {
                yPosLine = yFirstPos ;
            }
            secondLine->setLine(lastXParent, yPosLine, posX, yPosLine);
        } else {
            secondLine->hide();
        }
    }
    afterDisposeAllChildren();
}

void XSDItem::resetId()
{
    _instances = 0 ;
}
