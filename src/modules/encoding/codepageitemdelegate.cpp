/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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

#include "xmlEdit.h"
#include "codepageitemdelegate.h"
#include <QPainter>
#include "utils.h"


CodePageItemDelegate::CodePageItemDelegate(QObject *parent): QStyledItemDelegate(parent)
{
}

CodePageItemDelegate::~CodePageItemDelegate()
{
}

void CodePageItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    STYLEVIEWITEM_OPTION4(option);

    bool isReverse = option4.widget->layoutDirection() == Qt::RightToLeft ? true : false ;
    bool isSelected = option.state & QStyle::State_Selected;

    QString data = index.model()->data(index, Qt::UserRole).toString();

#if defined(ENVIRONMENT_WINDOWS)
    data = data.replace("$BODY$", isSelected ? "transparent" : "white");
#else
    data = data.replace("$BODY$", isSelected ? "blue" : "white");
#endif
    data = data.replace("$BLUE$", isSelected ? "white" : "blue");
    data = data.replace("$GREEN$", isSelected ? "white" : "green");
    data = data.replace("$TEXT$", isSelected ? "white" : "black");
    data = data.replace("$RED$", isSelected ? "white" : "red");

    if(!data.isEmpty()) {
        painter->save();

#if defined(ENVIRONMENT_WINDOWS)
        painter->setBackgroundMode(Qt::TransparentMode);
#endif
        int currentPosX = option.rect.x();
        if(isReverse) {
            currentPosX = option.rect.right() - option.decorationSize.width();
        }

        _document.setHtml(data);
        int offsetX = 0 ;
        if(isReverse) {
            offsetX = - _document.idealWidth();
        }
#if defined(ENVIRONMENT_WINDOWS)
        if(isSelected) {
            painter->fillRect(option.rect, QBrush(QColor::fromRgb(0, 0, 255)));
        } else {
            painter->fillRect(option.rect, QBrush(QColor::fromRgb(255, 255, 255)));
        }
#endif
        painter->translate(currentPosX + offsetX, option.rect.y());
        _document.setTextWidth(option.rect.width());
        _document.drawContents(painter);
        painter->translate(-(currentPosX + offsetX), -option.rect.y());
        painter->restore();
    }
}

QSize CodePageItemDelegate::sizeHint(const QStyleOptionViewItem & /*option*/, const QModelIndex & index) const
{
    QString data = index.model()->data(index, Qt::UserRole).toString();

    data = data.replace("$TEXT$", "black");
    data = data.replace("$FG$", "black");

    if(!data.isEmpty()) {
        _document.setHtml(data);
        QSizeF sizeF = _document.size();
        return QSize(sizeF.width(), sizeF.height());
    }
    return QSize(0, 0);
}
