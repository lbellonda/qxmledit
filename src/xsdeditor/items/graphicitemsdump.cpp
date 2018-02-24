/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2018 by Luca Bellonda and individual contributors       *
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

void XSDItem::dump_layout_item(XSDItem *target)
{
    QString msg = QString("item (id:%1), def:%2 yToAdd:%3").arg(target->_id).arg(dump_rect_string(target->_bounds)).arg(target->_yToAdd);
    printf("%s\n", msg.toLatin1().data());
    fflush(stdout);
}

void XSDItem::dump_layout_items()
{
    dump_layout_item(this);

    foreach(RChild * rchild, _children.children()) {
        XSDItem *xsdItem = rchild->item();
        xsdItem->dump_layout_items();
    }
}


QString XSDItem::dump_rect_string(const QRectF &thisBounds)
{
    QString str = QString("[(%1,%2)-(%3,%4) w:%5, h:%6]")
                  .arg(thisBounds.left()).arg(thisBounds.top()).arg(thisBounds.right()).arg(thisBounds.bottom())
                  .arg(thisBounds.width()).arg(thisBounds.height());
    return str;
}

void XSDItem::dump_layout_notice(const QString &notice, XSDItem *target, const QRectF &thisBounds, const qreal extraGap)
{
    void *p = target;
    QString msg = QString("%1 for item %2 (id:%3), def:%4 gap:%5").arg(notice).arg((long)p).arg(target->_id).arg(dump_rect_string(thisBounds)).arg(extraGap);
    printf("%s\n", msg.toLatin1().data());
    fflush(stdout);
}

void XSDItem::dump_layout_info(const QString &msg)
{
    printf("%s\n", msg.toLatin1().data());
    fflush(stdout);
}


void XSDItem::dump_layout_situation(const QString &msgPassed, const QVector<QRectF> &currBounds)
{
    QString data;
    foreach(const QRectF &rect, currBounds) {
        data += " ";
        data += dump_rect_string(rect);
    }
    QString msg = QString("%1: items: %2 -> %3").arg(msgPassed).arg(currBounds.size()).arg(data);
    printf("%s\n", msg.toLatin1().data());
    fflush(stdout);
}

void XSDItem::dump_layout_check_congruence(const QVector<QRectF> &currBounds)
{
    QString data;
    qreal posPrev = 0;
    int index = 0 ;
    foreach(const QRectF &rect, currBounds) {
        if(rect.left() != posPrev) {
            QString msg = QString("******** ERROR rect at %1: pos: %2 prev pos: %3").arg(index).arg(rect.left()).arg(posPrev);
            printf("%s\n", msg.toLatin1().data());
            fflush(stdout);
            return ;
        }
        posPrev = rect.right();
        index ++ ;
    }
}


void XSDItem::dump_layout_intersect(const int index, const EIntersectType type)
{
    QString value = "";
    switch(type) {
    case IntersectNoneBefore: value = "IntersectNoneBefore"; break;
    case IntersectBefore: value = "IntersectBefore"; break;
    case IntersectIncluded: value = "IntersectIncluded"; break;
    case IntersectAfter: value = "IntersectAfter"; break;
    case IntersectBeforeAndAfter: value = "IntersectBeforeAndAfter"; break;
    case IntersectNoneAfter: value = "IntersectNoneAfter"; break;
    }
    QString msg = QString("Index: %1 int: %2").arg(index).arg(value);
    printf("%s\n", msg.toLatin1().data());
    fflush(stdout);
}

void XSDItem::dump_layout_indexes(const int insertAtPosition, const int lastItemLessThan,
                                  const int itemToBeSplitBefore, const int firstItemToBeSuppressed, const int lastItemToBeSuppressed,
                                  const int itemToBeSplitAfter, const int firstItemGreaterThan)
{
    QString msg = QString("insertAtPosition %1 lastItemLessThan %2 itemToBeSplitBefore %3, firstItemToBeSuppressed %4 lastItemToBeSuppressed %5\n itemToBeSplitAfter %6 firstItemGreaterThan %7")
                  .arg(insertAtPosition).arg(lastItemLessThan).arg(itemToBeSplitBefore).arg(firstItemToBeSuppressed)
                  .arg(lastItemToBeSuppressed).arg(itemToBeSplitAfter).arg(firstItemGreaterThan);
    printf("%s\n", msg.toLatin1().data());
    fflush(stdout);

}
