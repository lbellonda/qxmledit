/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2017-2018 by Luca Bellonda and individual contributors  *
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
#include "timelapse.h"
#include <stdio.h>
#include <cstdint>

//#define QXMLEDIT_LAYOUT_DEBUG(x)    x
//#define QXMLEDIT_LAYOUT_DEBUG(x)    do { x; } while(false)
//#define QXMLEDIT_LAYOUT_DEBUG1(x)   x
//#define QXMLEDIT_LAYOUT_DEBUG3(x)   x

#define QXMLEDIT_LAYOUT_DEBUG(x)
#define QXMLEDIT_LAYOUT_DEBUG1(x)
#define QXMLEDIT_LAYOUT_DEBUG3(x)

static const int FinalOffsetFromTop = 20;

qreal XSDItem::placeAllStrategyHorPyramidNew0(XSDItemContext *context)
{
    QXMLEDIT_LAYOUT_DEBUG3(TimeLapse dtTotal("overall"));
    resetLayoutData();
    // placement is subject to offset
    QRectF initialOverallBounds(0, 0, 0, 0) ;
    placeObjectNew0(context, 0, 20, 0, initialOverallBounds);

    //afterPositionChange();
    {
        QStack<XSDItem*> chain ;
        QList<QGraphicsItem*> rendered;
        QList<XSDItem *> itemsRendered;
        QVector<QRectF> currBounds;
        QRectF initialRect(0, -1, initialOverallBounds.width() + 10000, 1);
        currBounds << initialRect ;
        QXMLEDIT_LAYOUT_DEBUG3(TimeLapse dtPlacement("placement"));
        updateObjectPlacementNew0(this, context, currBounds, rendered, itemsRendered, chain, 0);
        QXMLEDIT_LAYOUT_DEBUG3(dtPlacement.stopAndPrintElapsed());
    }
    QXMLEDIT_LAYOUT_DEBUG3(TimeLapse dtFinalPos("final pos"));
    QRectF overallBounds(0, 0, 0, 0);
    finalPos(overallBounds, true);
    QXMLEDIT_LAYOUT_DEBUG3(dtFinalPos.stopAndPrintElapsed());
    QXMLEDIT_LAYOUT_DEBUG3(TimeLapse dtFinalOffset("final offset"));
    finalOffset(overallBounds);
    QXMLEDIT_LAYOUT_DEBUG3(dtFinalOffset.stopAndPrintElapsed());
    afterPositionChange();
    recalcDispose(context);
    drawChildrenPort(context);
    const qreal overallFinalHeight = overallBounds.height();
    //----------------------------------------------
    QXMLEDIT_LAYOUT_DEBUG3(dtTotal.stopAndPrintElapsed());
    //----------------------------------------------
    return overallFinalHeight ;
}

void XSDItem::finalPos(QRectF &bounds, const bool isFirst)
{
    QGraphicsItem * graphicsItem = graphicItem();
    graphicsItem->setPos(_bounds.topLeft());
    QXMLEDIT_LAYOUT_DEBUG(dump_layout_notice(" Final position ", this, _bounds, 0));
    if(isFirst) {
        bounds = _bounds;
    } else {
        bounds = bounds.united(_bounds);
    }
    foreach(RChild * rchild, _children.children()) {
        XSDItem *xsdItem = rchild->item();
        xsdItem->finalPos(bounds, false);
    }
}

void XSDItem::finalOffset(const QRectF &bounds)
{
    const qreal topElementPos = bounds.top();
    if(topElementPos > FinalOffsetFromTop) {
        const qreal delta = topElementPos - FinalOffsetFromTop;
        foreach(QGraphicsItem * item, graphicItem()->scene()->items()) {
            if(NULL != item->parentItem()) {
                continue;
            }
            QRectF itemBounds = item->boundingRect();
            if(!itemBounds.isEmpty() && item->isVisible()) {
                item->setY(item->y() - delta);
            }
        }
    }
}

qreal XSDItem::calcChildrenHeight(XSDItemContext *context, const bool isRecursive)
{
    Utils::TODO_THIS_RELEASE("delete forse non serve");
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
            lastChildBounds = xsdItem->calcChildrenHeight(context, true) ;
        } else {
            lastChildBounds = xsdItem->_bounds.height();
        }
        directChildrenHeight += lastChildBounds;
    }
    _childrenSizeInvalid = false;

    // now center the children in the bounding rect
    qreal thisHeight = _bounds.height();
    if(directChildrenHeight < thisHeight) {
        directChildrenHeight = thisHeight ;
    }
    directChildrenHeight += offsetHeight();
    directChildrenHeight += marginBottom();
    //QString str = QString("DEBUG CODE: %3\nChildren count:%1, height:%2").arg(_children.children().size()).arg(_childrenHeight).arg(graphicItem()->toolTip());
    //graphicItem()->setToolTip(str);
    return directChildrenHeight ;
}

void XSDItem::placeObjectNew0(XSDItemContext *context, const int level, const qreal xPos, const qreal yPos, QRectF &overallBounds)
{
    /*if(!strcmp(this->metaObject()->className(), "ContainerItem")) {
        int debug = 1;
        debug++;
    }*/
    qreal allChildrenHeight = calcChildrenHeight(context, false) ;
    const qreal yPosThis = yPos;
    _bounds.moveTopLeft(QPointF(xPos, yPosThis));
    QXMLEDIT_LAYOUT_DEBUG3(printf("o:%d pos at: %d, %d (w:%d h:%d)", _id, (int)xPos, (int)yPos, (int)_bounds.width(), (int)_bounds.height()); fflush(stdout););
    QXMLEDIT_LAYOUT_DEBUG3(printf(" r: %d, %d\n", (int)_bounds.left(), (int)_bounds.top()); fflush(stdout););
    QRectF bounds(_bounds);
    qreal xOffset = xPos + 2 * bounds.width() + context->stemLength() / 4 + extraSpace();
    Utils::TODO_THIS_RELEASE("qreal yFirstPos = 0;");
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
        QRectF childBounds(xsdItem->_bounds);
        int origYPos = itemYPos ;
        int childHeight  = childBounds.height();

        xsdItem->placeObjectNew0(context, level + 1, xOffset, itemYPos, overallBounds);

        maxYPosChildren = origYPos + childBounds.height() / 2 ;
        if(isFirst) {
            isFirst = false ;
            minYPosChildren = maxYPosChildren;
            Utils::TODO_THIS_RELEASE("yFirstPos = origYPos + childBounds.height() / 2;");
        } else {
            itemYPos += context->gapBetweenChildren();
        }
        //xsdItem->afterDispose(origYPos, childHeight);
        itemYPos += childHeight ;
    }
    if(0 == level) {
        overallBounds = _bounds ;
    } else {
        overallBounds = overallBounds.united(_bounds) ;
    }
    Utils::TODO_THIS_RELEASE("fare poi le linee");
#if 0
    QGraphicsLineItem *line = _children.secondLine(this);
    if(NULL != line) {
        QXMLEDIT_LAYOUT_DEBUG1(line->setPen(QPen(QColor::fromRgb(0, 255, 255))));
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
        QXMLEDIT_LAYOUT_DEBUG1(secondLine->setPen(QPen(QColor::fromRgb(0, 128, 0))));
        if(_children.children().size() > 1) {
            secondLine->show();
            qreal lastXParent = xPos + bounds.width() ;
            qreal posX = lastXParent + (xOffset - lastXParent) / 2 ;
            qreal yPosLine = yPosThis + bounds.height() / 2 + offsetHeight();
            if(_children.children().size() == 1) {
                yPosLine = yFirstPos ;
            }
            secondLine->setLine(lastXParent, yPosLine, posX, yPosLine);
            _children.showChildLine();
            //secondLine->setPen(QPen(QColor::fromRgb(255,255,255)));
        } else if(_children.children().size() == 1) {
            secondLine->show();
            qreal lastXParent = xPos + bounds.width() ;
            qreal posX = lastXParent + (xOffset - lastXParent) ;
            qreal yPosLine = yPosThis + bounds.height() / 2 + offsetHeight();
            if(_children.children().size() == 1) {
                yPosLine = yFirstPos ;
            }
            secondLine->setLine(lastXParent, yPosLine, posX, yPosLine);
            _children.suppressChildLine();
            //secondLine->setPen(QPen(QColor::fromRgb(255,255,255)));
        } else {
            secondLine->hide();
        }
    }
    //afterDisposeAllChildren();
#endif
}

void XSDItem::updateSummaryLineBounds(const qreal gapThis, const qreal gap, const bool isEnlarging)
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
        QXMLEDIT_LAYOUT_DEBUG(line->setPen(QPen(QColor::fromRgb(255, 0, 0))););
    }
    QGraphicsLineItem *secondLine = _children._line;
    if((NULL != secondLine) && secondLine->isVisible()) {
        QLineF qline = secondLine->line();
        QPointF p1 = qline.p1();
        QPointF p2 = qline.p2();
        p1.setY(p1.y() + gapThis);
        p2.setY(p2.y() + gapThis);
        qline.setP1(p1);
        qline.setP2(p2);
        secondLine->setLine(qline);
        QXMLEDIT_LAYOUT_DEBUG(secondLine->setPen(QPen(QColor::fromRgb(0, 0xC0, 0))););
    }
}

void XSDItem::moveDownBy(const qreal gap, const bool isRecursive, const bool isEnlarging)
{
    Utils::TODO_THIS_RELEASE("fare");
    Q_UNUSED(isRecursive);
    Q_UNUSED(isEnlarging);
    _bounds.moveTop(_bounds.top() + gap);
    QXMLEDIT_LAYOUT_DEBUG(
        printf("%s\n", QString("      ** moved name %1 by %2 enlarging %3")
               .arg(item()->name())
               .arg(gap)
               .arg(isEnlarging)
               .toLatin1().data());
        fflush(0);
    );

    _yToAdd += gap ;
    Utils::TODO_THIS_RELEASE("fare");
}

qreal XSDItem::updateAnObjectPlacementNew0(XSDItemContext * /*context*/, XSDItem *target, QVector<QRectF> &currBounds, const qreal thisGap, const qreal gapValue, const bool isFirst, const int index)
{
    Utils::TODO_THIS_RELEASE("eliminare");
    Q_UNUSED(index);
    Q_UNUSED(isFirst);
    bool targetEngaged = false;
    QXMLEDIT_LAYOUT_DEBUG(bool existsBefore = false);
    bool firstChildIsTarget = false;
    int childIndex = 0;
    foreach(RChild * rchild, _children.children()) {
        XSDItem *xsdItem = rchild->item();
        if(!targetEngaged) {
            if(xsdItem == target) {
                if(0 == childIndex) {
                    firstChildIsTarget = true ;
                }
                targetEngaged = true ;
            } else {
                QXMLEDIT_LAYOUT_DEBUG(existsBefore = true);
            }
            // target and before: already taken care of
        } else {
            // all after to the target is to move
            Utils::TODO_THIS_RELEASE("xsdItem->moveDownBy(gapValue, true, false);");
        }
        childIndex ++ ;
    }
    const bool isOnlyOne = (_children.children().size() == 1);
    const qreal realThisGap = firstChildIsTarget || isOnlyOne ? thisGap : thisGap / 2;
    QXMLEDIT_LAYOUT_DEBUG(do {
        printf("%s\n", QString("index %1 name %2 real this gap: %3 exists %4 children size:%5 inputgapthis: %6")
               .arg(index).arg(item()->name()).arg(realThisGap)
               .arg(existsBefore).arg(_children.children().size())
               .arg(thisGap)
               .toLatin1().data());
        fflush(0);
    } while(false););

    Utils::TODO_THIS_RELEASE("Se target e' primo, il solo, move down di tutto o mezzo, poi");

    QXMLEDIT_LAYOUT_DEBUG(dump_layout_notice("Adding offset", this, _bounds, realThisGap));
    _yToAdd += gapValue ;
    _bounds.moveTop(_bounds.top() + realThisGap);
    Utils::TODO_THIS_RELEASE("Questo rettangolo non e' estatto e non tiene conto dello stem");
    QRectF testRect(QPointF(_bounds.left() - 50, -1), QPointF(_bounds.right() + 50, _bounds.bottom()));
    int size = currBounds.size();
    for(int i = 0 ; i < size ; i ++) {
        QRectF outerRect = currBounds.at(i);
        if(outerRect.intersects(testRect)) {
            QRectF newRect(outerRect);
            if((_bounds.bottom() + 1) > newRect.bottom()) {
                newRect.setBottom(_bounds.bottom() + 1);
                currBounds.replace(i, newRect);
            } else {
                Utils::TODO_THIS_RELEASE("alert");
            }
        }
    }

    QXMLEDIT_LAYOUT_DEBUG(dump_layout_notice(" moved Down To ", this, _bounds, realThisGap));
    return realThisGap ;
}

/*
se esiste precedente allarga:true;
gap se 1! gap else gap/2

se gap da livello precedente, se 1! figli usa gap precedente
se >1 figli e' gap prec/2
*/

QXMLEDIT_LAYOUT_DEBUG1(static int nogo = 0 ;)
bool XSDItem::updateObjectPlacementNew0(XSDItem* root, XSDItemContext *context, QVector<QRectF> &currBounds, QList<QGraphicsItem*> &rendered, QList<XSDItem *> &itemsRendered, QStack<XSDItem*> chain, const qreal extraGapValue)
{
    Utils::TODO_THIS_RELEASE("scrivi algoritmo");
    bool collisionFound = false;
    bool existsCollision = false ;
    qreal extens = 0;
    if(NULL != _chain) {
        extens = _chain->line()->boundingRect().width();
    }
    QXMLEDIT_LAYOUT_DEBUG(if(6 == _id) {
    dump_layout_notice("enter", this, _bounds, extraGapValue);
    });
    QXMLEDIT_LAYOUT_DEBUG(dump_layout_notice("pre enter", this, _bounds, 0));
    _bounds.moveTop(_bounds.top() + extraGapValue);
    Utils::TODO_THIS_RELEASE("valore extra");
    Q_UNUSED(extens);
    qreal beforeExtens = extens ;
    Q_UNUSED(beforeExtens);
    if((_bounds.x() - extens) < 0) {
        beforeExtens = _bounds.x();
    }
    //QRectF thisBounds(_bounds.x() - beforeExtens, _bounds.y(), _bounds.width() + beforeExtens + extens, _bounds.height());
    QRectF thisBounds(_bounds.x() - 10, _bounds.y(), _bounds.width() + 10, _bounds.height());
    QXMLEDIT_LAYOUT_DEBUG(dump_layout_notice("enter", this, thisBounds, extraGapValue));
    QXMLEDIT_LAYOUT_DEBUG(dump_layout_situation("Before work", currBounds));

    int index = 0 ;
    int lastItemLessThan =  -1 ;
    int itemToBeSplitBefore =  -1 ;
    int firstItemToBeSuppressed =  -1 ;
    int lastItemToBeSuppressed =  -1 ;
    int itemToBeSplitAfter =  -1 ;
    int firstItemGreaterThan =  -1 ;
    bool isIntersectBeforeAndAfter = false ;

    qreal gapValue = 0 ;
    foreach(const QRectF &outerRect, currBounds) {
        QXMLEDIT_LAYOUT_DEBUG(dump_layout_intersect(index, intersectionType(outerRect, thisBounds)));
        switch(intersectionType(outerRect, thisBounds)) {
        case IntersectNoneBefore:
            lastItemLessThan = index;
            break;
        case IntersectBefore:
            gapValue = checkVerticalCollision(outerRect, thisBounds, gapValue);
            itemToBeSplitBefore = index;
            break;
        case IntersectIncluded:
            gapValue = checkVerticalCollision(outerRect, thisBounds, gapValue);
            lastItemToBeSuppressed =  index ;
            if(firstItemToBeSuppressed < 0) {
                firstItemToBeSuppressed = index ;
            }
            break;
        case IntersectAfter:
            gapValue = checkVerticalCollision(outerRect, thisBounds, gapValue);
            itemToBeSplitAfter = index ;
            goto escape_from_loop;
            break;
        case IntersectBeforeAndAfter:
            gapValue = checkVerticalCollision(outerRect, thisBounds, gapValue);
            itemToBeSplitBefore = index;
            itemToBeSplitAfter = index ;
            isIntersectBeforeAndAfter = true ;
            goto escape_from_loop;
            break;
        case IntersectNoneAfter:
            firstItemGreaterThan = index ;
            goto escape_from_loop;
            break;
        default:
            Utils::error(NULL, "Invalid intersection value.");
            break;
        }
        index ++ ;
    }

    // simple solution
escape_from_loop:

    int insertAtPosition = -1 ;

    if(isIntersectBeforeAndAfter) {
        // This case takes over the whole processing.
        // 1- split the before
        QRectF itemRect = currBounds.at(itemToBeSplitBefore);
        QRectF beforeRect = XSDItem::splitRectBefore(itemRect, thisBounds);
        currBounds.replace(itemToBeSplitBefore, beforeRect);
        // 2- insert the new part
        insertAtPosition = itemToBeSplitBefore + 1;
        // 3- split the after part
        QRectF afterRect = XSDItem::splitRectAfter(itemRect, thisBounds);
        currBounds.insert(itemToBeSplitBefore + 1, afterRect);
    } else {
        // apply change
        if(itemToBeSplitBefore >= 0) {
            QRectF itemRect = currBounds.at(itemToBeSplitBefore);
            QRectF beforeRect = XSDItem::splitRectBefore(itemRect, thisBounds);
            currBounds.replace(itemToBeSplitBefore, beforeRect);
            insertAtPosition = itemToBeSplitBefore + 1;
        }
        if(firstItemToBeSuppressed >= 0) {
            if(insertAtPosition < 0) {
                insertAtPosition = firstItemToBeSuppressed;
            }
            for(int indexForDelete = firstItemToBeSuppressed ; indexForDelete <= lastItemToBeSuppressed ; indexForDelete ++) {
                currBounds.remove(firstItemToBeSuppressed);
                if(itemToBeSplitAfter >= 0) {
                    itemToBeSplitAfter -- ;
                }
            }
        }
        if(itemToBeSplitAfter >= 0) {
            QRectF itemRect = currBounds.at(itemToBeSplitAfter);
            QRectF afterRect = XSDItem::splitRectAfter(itemRect, thisBounds);
            currBounds.replace(itemToBeSplitAfter, afterRect);
            if(insertAtPosition < 0) {
                insertAtPosition = itemToBeSplitAfter;
            }
        }
        if((itemToBeSplitBefore >= 0) || (firstItemToBeSuppressed >= 0) || (itemToBeSplitAfter >= 0)) {
            if(insertAtPosition < 0) {
                Utils::error(NULL, "Invalid position for insert index.");
            }
        } else {
            if(firstItemGreaterThan >= 0) {
                insertAtPosition = firstItemGreaterThan ;
            } else {
                // insert at the end, no other way out, this can be 0 if no lastItemLessThan
                insertAtPosition = lastItemLessThan + 1;
            }
        }
    }
    QXMLEDIT_LAYOUT_DEBUG(dump_layout_indexes(insertAtPosition, lastItemLessThan,
                          itemToBeSplitBefore, firstItemToBeSuppressed, lastItemToBeSuppressed,
                          itemToBeSplitAfter, firstItemGreaterThan));

    if(gapValue > 0) {
        existsCollision = true ;
        QXMLEDIT_LAYOUT_DEBUG(dump_layout_info("Collision detected"));
        //QGraphicsColorizeEffect *r = new QGraphicsColorizeEffect(); r->setColor(QColor::fromRgb(255,0,0));other->setGraphicsEffect(r);
        _bounds.moveTop(_bounds.top() + gapValue);
        QXMLEDIT_LAYOUT_DEBUG(dump_layout_notice(" movedTo ", this, _bounds, gapValue));
        _yToAdd += gapValue ;
    }
    QRectF thisRect(QPointF(thisBounds.left(), -1), QPointF(thisBounds.right(), _bounds.bottom() + 1));

    // set the new position
    currBounds.insert(insertAtPosition, thisRect);
    //--

    QXMLEDIT_LAYOUT_DEBUG(dump_layout_situation("After work", currBounds));
    QXMLEDIT_LAYOUT_DEBUG(dump_layout_check_congruence(currBounds));
    Utils::TODO_THIS_RELEASE("modifica questo rettangolo se collisione");
    if(existsCollision) {
        //if(existsCollision && (nogo==1)) {
        QXMLEDIT_LAYOUT_DEBUG(QGraphicsItem *thisItem = graphicItem());
        QXMLEDIT_LAYOUT_DEBUG(thisItem->setGraphicsEffect(new QGraphicsColorizeEffect()));
        QXMLEDIT_LAYOUT_DEBUG(dump_layout_info("starting moving down"));
        /*
        * per ogni padre, segna da quale figlio spostare ( da qui in basso).
        * poi partendo dal padre, aggiungi l'offset ad ogni figlio e da qui in poi, in modo ricorsivo:
        * sposta il padre al centro
        */
        const int size = chain.size();
        bool first = true ;
        int counter = 0 ;
        qreal thisGap = gapValue ;
        XSDItem *target = this ;
        for(int index = size - 1 ; index >= 0 ; index --) {
            XSDItem *item = chain.at(index);
            QXMLEDIT_LAYOUT_DEBUG(dump_layout_notice(" Before moving down chain ", item, item->_bounds, thisGap));
            thisGap = item->updateAnObjectPlacementNew0(context, target, currBounds, thisGap, gapValue, first, counter);
            first = false;
            target = item ;
            counter ++ ;
        }
    }
    QXMLEDIT_LAYOUT_DEBUG(root->dump_layout_items());
    chain.push(this);
    //do it for each child
    foreach(RChild * rchild, _children.children()) {
        XSDItem *xsdItem = rchild->item();
        if(xsdItem->updateObjectPlacementNew0(root, context, currBounds, rendered, itemsRendered, chain, _yToAdd + extraGapValue)) {
            collisionFound = true ;
        }
    }
    chain.pop();
    return existsCollision || collisionFound ;
}


qreal XSDItem::checkVerticalCollision(const QRectF &candidate, const QRectF &source, const qreal originalGapValue)
{
    const qreal gapValue = candidate.bottom() - source.top() ;
    if(gapValue <= 0) {
        return originalGapValue;
    }
    if(gapValue > originalGapValue) {
        return gapValue ;
    }
    return originalGapValue ;
}


XSDItem::EIntersectType XSDItem::intersectionType(const QRectF &candidate, const QRectF &source)
{
    const qreal sourceMinX = source.left();
    const qreal sourceMaxX = source.right();
    const qreal candidateMinX = candidate.left();
    const qreal candidateMaxX = candidate.right();

    if(candidateMaxX <= sourceMinX) {
        return IntersectNoneBefore ;
    }
    if(candidateMinX >= sourceMaxX) {
        return IntersectNoneAfter ;
    }
    // now, intersect
    if((sourceMinX <= candidateMinX) && (sourceMaxX >= candidateMaxX)) {
        return IntersectIncluded;
    }

    if(sourceMinX > candidateMinX) {
        if(sourceMaxX >= candidateMaxX) {
            return IntersectBefore;
        } else {
            return IntersectBeforeAndAfter;
        }
    } else {
        return IntersectAfter;
    }
}


void XSDItem::recalcDispose(XSDItemContext *context)
{
    foreach(RChild * rchild, _children.children()) {
        XSDItem *xsdItem = rchild->item();
        xsdItem->recalcDispose(context);
    }
    afterDispose();
    afterDisposeAllChildren();
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


void XSDItem::resetLayoutData()
{
    _yToAdd = 0 ;
    QGraphicsItem *item = graphicItem();
    _bounds.setRect(0, 0, item->boundingRect().width(), item->boundingRect().height());
    foreach(RChild * rchild, _children.children()) {
        XSDItem *xsdItem = rchild->item();
        xsdItem->resetLayoutData();
    }
}

QRectF XSDItem::splitRectBefore(const QRectF &current, const QRectF &source)
{
    Utils::TODO_THIS_RELEASE("");
    QRectF opRect(current);
    opRect.setRight(source.left());
    return opRect;
}

QRectF XSDItem::splitRectAfter(const QRectF &current, const QRectF &source)
{
    Utils::TODO_THIS_RELEASE("");
    QRectF opRect(current);
    opRect.setLeft(source.right());
    return opRect;
}


void XSDItem::drawChildrenPort(XSDItemContext *context)
{
    qreal minYPosChildren = 0;
    qreal maxYPosChildren = 0 ;
    const qreal xOffset = _bounds.left() + 2 * _bounds.width() + context->stemLength() / 4 + extraSpace();
    bool isFirst = true;

    qreal yFirstPos = 0 ;

    foreach(RChild * rchild, _children.children()) {
        XSDItem *xsdItem = rchild->item();
        const QRectF childBounds(xsdItem->_bounds);
        const qreal yPos = childBounds.top() ;

        maxYPosChildren = yPos + childBounds.height() / 2 ;
        yFirstPos = maxYPosChildren ;
        if(isFirst) {
            isFirst = false ;
            minYPosChildren = maxYPosChildren;
        }
        xsdItem->drawChildrenPort(context);
    }

    QGraphicsLineItem *line = _children.secondLine(this);
    if(NULL != line) {
        QXMLEDIT_LAYOUT_DEBUG1(line->setPen(QPen(QColor::fromRgb(0, 255, 255))));
        if(_children.children().size() > 1) {
            line->show();
            qreal lastXParent = _bounds.left() + _bounds.width() ;
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
        QXMLEDIT_LAYOUT_DEBUG1(secondLine->setPen(QPen(QColor::fromRgb(0, 128, 0))));
        if(_children.children().size() > 1) {
            secondLine->show();
            qreal lastXParent = _bounds.left() + _bounds.width() ;
            qreal posX = lastXParent + (xOffset - lastXParent) / 2 ;
            qreal yPosLine = _bounds.top() + _bounds.height() / 2 + offsetHeight();
            if(_children.children().size() == 1) {
                yPosLine = yFirstPos ;
            }
            secondLine->setLine(lastXParent, yPosLine, posX, yPosLine);
            _children.showChildLine();
            QXMLEDIT_LAYOUT_DEBUG1(secondLine->setPen(QPen(QColor::fromRgb(255, 255, 255))));
        } else if(_children.children().size() == 1) {
            secondLine->show();
            qreal lastXParent = _bounds.left() + _bounds.width() ;
            qreal posX = lastXParent + (xOffset - lastXParent) ;
            qreal yPosLine = _bounds.top() + _bounds.height() / 2 + offsetHeight();
            if(_children.children().size() == 1) {
                yPosLine = yFirstPos ;
            }
            secondLine->setLine(lastXParent, yPosLine, posX, yPosLine);
            _children.suppressChildLine();
            QXMLEDIT_LAYOUT_DEBUG1(secondLine->setPen(QPen(QColor::fromRgb(255, 255, 255))));
        } else {
            secondLine->hide();
        }
    }
}
