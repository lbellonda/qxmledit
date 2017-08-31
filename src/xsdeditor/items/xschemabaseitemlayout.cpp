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

#include "xsdeditor/xsdwindow.h"
#include "utils.h"
#include <QGraphicsTextItem>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsColorizeEffect>
#include "xsdeditor/items/xitemsdefinitions.h"

qreal XSDItem::placeAllStrategyHorPyramidNew0(XSDItemContext *context)
{
    const qreal overallHeight = calcChildrenHeightStrategyNew0(context, true);
    QGraphicsItem *thisItem = graphicItem() ;
    QRectF bounds = thisItem->boundingRect();
    int rootYPosition = (overallHeight - bounds.height()) / 2;
    placeObjectNew0(context, 0, 0, rootYPosition);
    afterPositionChange();
    QStack<XSDItem*> chain ;
    QList<QGraphicsItem*> rendered;
    QList<XSDItem *> itemsRendered;
    updateObjectPlacementNew0(context, rendered, itemsRendered, chain);
    afterPositionChange();
    finalOffset();
    afterPositionChange();
    const qreal overallFinalHeight = calcOverallHeight(rendered);
    Utils::TODO_THIS_RELEASE("fare centra su root, ma a livello piu alto");
    return overallFinalHeight ;
}

void XSDItem::finalOffset()
{
    QRectF bounds(0, 0, 0, 0);
    bool first = false;
    QRectF itemsRect = graphicItem()->scene()->itemsBoundingRect();
    Utils::TODO_THIS_RELEASE(QString("%1").arg(itemsRect.isValid() ? "1" : "0"));
    foreach(QGraphicsItem * item, graphicItem()->scene()->items()) {
        QRectF itemBounds = item->boundingRect();
        if(NULL != item->parentItem()) {
            continue;
        }
        Utils::TODO_THIS_RELEASE("itemBounds.setTopLeft(item->pos());");
        if(!itemBounds.isEmpty() && item->isVisible() && (itemBounds.top() > 0)) {
            if(first) {
                bounds = itemBounds;
                first = false;
            } else {
                bounds = bounds.united(itemBounds);
            }
            if(itemBounds.top() <= 0) {
                printf("zero\n");
                /*QGraphicsTextItem *i = (QGraphicsTextItem*)item;
                QString text = i->toPlainText();
                Utils::TODO_THIS_RELEASE(text);*/
            }
        }
    }

    const int topElementPos = bounds.top();
    if(topElementPos > 20) {
        const int delta = topElementPos - 20;
        foreach(QGraphicsItem * item, graphicItem()->scene()->items()) {
            if(NULL != item->parentItem()) {
                continue;
            }
            QRectF itemBounds = item->boundingRect();
            if(!itemBounds.isEmpty() && item->isVisible() /*&& (itemBounds.top() > 0)*/) {
                item->setY(item->y() - delta);
            }
        }
    }
}

qreal XSDItem::calcChildrenHeightStrategyNew0(XSDItemContext *context, const bool isRecursive)
{
    qreal directChildrenHeight = 0 ;
    qreal lastChildBounds = 0 ;
    bool isFirst = true;
    foreach(RChild * rchild, _children.children()) {
        XSDItem *xsdItem = rchild->item();

        if(isFirst) {
            isFirst = false;
        } else {
            directChildrenHeight += context->gapBetweenChildren() ;
        }
        if(isRecursive) {
            lastChildBounds = xsdItem->calcChildrenHeightStrategyNew0(context, true) ;
        } else {
            lastChildBounds = xsdItem->graphicItem()->boundingRect().height();
        }
        directChildrenHeight += lastChildBounds;
    }
    _childrenSizeInvalid = false;

    // now center the children in the bounding rect
    qreal thisHeight = graphicItem()->boundingRect().height();
    if(directChildrenHeight < thisHeight) {
        directChildrenHeight = thisHeight ;
    }
    directChildrenHeight += offsetHeight();
    directChildrenHeight += marginBottom();
    //QString str = QString("DEBUG CODE: %3\nChildren count:%1, height:%2").arg(_children.children().size()).arg(_childrenHeight).arg(graphicItem()->toolTip());
    //graphicItem()->setToolTip(str);
    return directChildrenHeight ;
}

void XSDItem::placeObjectNew0(XSDItemContext *context, const int level, const qreal xPos, const qreal yPos)
{
    Utils::TODO_THIS_RELEASE("manca da centrare la linea esattamente in mezzo, c'e' un piccolo offset superiore.");
    /*if(!strcmp(this->metaObject()->className(), "ContainerItem")) {
        int debug = 1;
        debug++;
    }*/
    QGraphicsItem *thisItem = graphicItem() ;
    qreal allChildrenHeight = calcChildrenHeightStrategyNew0(context, false) ;
    qreal yPosThis = yPos;
    thisItem->setPos(xPos, yPosThis);
    QRectF bounds = thisItem->boundingRect();
    qreal xOffset = xPos + 2 * bounds.width() + context->stemLength() / 4 + extraSpace();
    qreal yFirstPos = 0;
    // finds the total height
    qreal minYPosChildren = yPos + (bounds.height() / 2) - (allChildrenHeight / 2) + offsetHeight() ;
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
        int origYPos = itemYPos ;
        int childHeight  = xsdItem->graphicItem()->boundingRect().height();
        QRectF childBounds = xsdItem->graphicItem()->boundingRect();

        xsdItem->placeObjectNew0(context, level + 1, xOffset, origYPos);

        maxYPosChildren = origYPos + childBounds.height() / 2 ;
        if(isFirst) {
            isFirst = false ;
            minYPosChildren = maxYPosChildren;
            yFirstPos = origYPos + childBounds.height() / 2;
        } else {
            itemYPos += context->gapBetweenChildren();
        }
        xsdItem->afterDispose(origYPos, childHeight);
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
            //secondLine->setPen(QPen(QColor::fromRgb(255,255,255)));
        } else {
            secondLine->hide();
        }
    }
    afterDisposeAllChildren();
}

void XSDItem::updateSummaryLineBounds(const qreal gap, const bool isEnlarging)
{
    QGraphicsLineItem *line = _children.secondLine(this);
    if((NULL != line) && line->isVisible()) {
        QLineF qline = line->line();
        QPointF p2 = qline.p2();
        p2.setY(p2.y() + gap);
        qline.setP2(p2);
        if(!isEnlarging) {
            QPointF p1 = qline.p1();
            p1.setY(p1.y() + gap);
            qline.setP1(p1);
        }
        line->setLine(qline);
        //line->setPen(QPen(QColor::fromRgb(255, 0, 0)));
    }
    QGraphicsLineItem *secondLine = _children._line;
    if((NULL != secondLine) && secondLine->isVisible()) {
        QLineF qline = secondLine->line();
        QPointF p1 = qline.p1();
        QPointF p2 = qline.p2();
        p1.setY(p1.y() + gap);
        p2.setY(p2.y() + gap);
        qline.setP1(p1);
        qline.setP2(p2);
        secondLine->setLine(qline);
        //secondLine->setPen(QPen(QColor::fromRgb(0,0xC0, 0)));
    }
}

void XSDItem::moveDownBy(const qreal gapThis, const qreal gap, const bool isRecursive, const bool isEnlarging)
{
    QGraphicsItem *thisItem = graphicItem() ;
    thisItem->setY(thisItem->y() + gapThis);
    if(isRecursive) {
        foreach(RChild * rchild, _children.children()) {
            XSDItem *xsdItem = rchild->item();
            xsdItem->moveDownBy(gap, gap, true, isEnlarging);
        }
    }
    updateSummaryLineBounds(gapThis, isEnlarging);
    if(NULL != chain()) {
        chain()->updatePosition();
    }
    foreach(RChild * child, _children.children()) {
        child->updatePosition();
    }
}

void XSDItem::updateAnObjectPlacementNew0(XSDItemContext * /*context*/, XSDItem *target, const qreal gap)
{
    bool targetEngaged = false;
    bool existsBefore = false;
    foreach(RChild * rchild, _children.children()) {
        XSDItem *xsdItem = rchild->item();
        if(NULL != target) {
            if(!targetEngaged) {
                if(xsdItem == target) {
                    targetEngaged = true ;
                } else {
                    existsBefore = true ;
                }
            } else {
                xsdItem->moveDownBy(gap, gap, true, false);
            }
        } else {
            xsdItem->moveDownBy(gap, gap, true, false);
        }
    }
    Utils::TODO_THIS_RELEASE("spostare al centro");
    moveDownBy(gap, gap, false, existsBefore);
}

bool XSDItem::updateObjectPlacementNew0(XSDItemContext *context, QList<QGraphicsItem*> &rendered, QList<XSDItem *> &itemsRendered, QStack<XSDItem*> chain)
{
    bool collisionFound = false;
    chain.push(this);
    QGraphicsItem *thisItem = graphicItem() ;
    qreal maxGap = 0;
    bool existsCollision = false ;
    qreal extens = 0;
    if(NULL != _chain) {
        extens = _chain->line()->boundingRect().width();
    }
    QRectF thisBounds(thisItem->x() - extens, thisItem->y(), thisItem->boundingRect().width() + extens, thisItem->boundingRect().height());
    foreach(QGraphicsItem *other, rendered) {
        QRectF otherBounds(other->x(), other->y(), other->boundingRect().width(), other->boundingRect().height());
        qreal minVal = qMax(thisBounds.left(), otherBounds.left());
        qreal maxVal = qMin(thisBounds.right(), otherBounds.right());
        if(maxVal > minVal) {
            const qreal value = otherBounds.bottom() - thisBounds.top() ;
            if(value > 0) {
                existsCollision = true ;
                //QGraphicsColorizeEffect *r = new QGraphicsColorizeEffect(); r->setColor(QColor::fromRgb(255,0,0));other->setGraphicsEffect(r);
                if(value > maxGap) {
                    maxGap = value + 2 ;
                }
            }
        }
    }
    foreach(XSDItem *otherItem, itemsRendered) {
        if(NULL != otherItem->_chain) {
            QGraphicsItem *otherLine = otherItem->_chain->line() ;
            QGraphicsItem *other = otherItem->graphicItem();
            QRectF otherBounds(other->x() - otherLine->boundingRect().width(), other->y() + other->boundingRect().height() / 2, otherLine->boundingRect().width(), 2);
            qreal minVal = qMax(thisBounds.left(), otherBounds.left());
            qreal maxVal = qMin(thisBounds.right(), otherBounds.right());
            if(maxVal > minVal) {
                const qreal value = otherBounds.bottom() - thisBounds.top() ;
                if(value > 0) {
                    existsCollision = true ;
                    //QGraphicsColorizeEffect *r = new QGraphicsColorizeEffect(); r->setColor(QColor::fromRgb(255,0,0));other->setGraphicsEffect(r);
                    if(value > maxGap) {
                        maxGap = value + 2 ;
                    }
                }
            }
        }
    }
    if(existsCollision) {
        //thisItem->setGraphicsEffect(new QGraphicsColorizeEffect());
        /*
        * per ogni padre, segna da quale figlio spostare ( da qui in basso).
        * poi partendo dal padre, aggiungi l'offset ad ogni figlio e da qui in poi, in modo ricorsivo:
        * sposta il padre al centro
        */
        const int size = chain.size();
        for(int index = size - 1 ; index >= 0 ; index --) {
            XSDItem *item = chain.at(index);
            XSDItem *target = NULL ;
            if((index + 1) < size) {
                target = chain.at(index + 1);
            }
            Utils::TODO_THIS_RELEASE("move the parent at the center of the new items");
            item->updateAnObjectPlacementNew0(context, target, maxGap);
        }
    }
    rendered.append(thisItem);
    itemsRendered.append(this);
    //do it for each child
    foreach(RChild * rchild, _children.children()) {
        XSDItem *xsdItem = rchild->item();
        if(xsdItem->updateObjectPlacementNew0(context, rendered, itemsRendered, chain)) {
            collisionFound = true ;
        }
    }
    chain.pop();
    return existsCollision || collisionFound ;
}

qreal XSDItem::calcOverallHeight(QList<QGraphicsItem*> &rendered)
{
    bool minValueSet = false;
    qreal minValue = 0;
    qreal maxValue = 0;
    foreach(QGraphicsItem *item, rendered) {
        QRectF bounds(item->x(), item->y(), item->boundingRect().width(), item->boundingRect().height());
        if(maxValue < bounds.bottom()) {
            maxValue = bounds.bottom();
        }
        if(minValueSet) {
            if(minValue > bounds.top()) {
                minValue = bounds.top();
            }
        } else {
            minValue = bounds.top();
            minValueSet = true ;
        }
    }
    return maxValue - minValue ;
}
