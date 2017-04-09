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

#include "qattributedelegate.h"
#include "element.h"
#include <QPixmapCache>
#include <QPainter>
#include <QApplication>
#include "utils.h"

//TODO: cache measures in the item, then clear them at the item variation

QAttributeDelegate::QAttributeDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
    Utils::TODO_THIS_RELEASE("rimuovi se non serve");
}

QAttributeDelegate::~QAttributeDelegate()
{
}

void QAttributeDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QStyledItemDelegate::paint(painter, option, index) ;
    QString html = index.model()->data(index, Element::HtmlTextRole).toString();

    painter->save();
    if(html.startsWith("<html")) {
        _document.setHtml(html);
    } else {
        _document.setPlainText(html);
    }
    int iconWidth = 0;
    QIcon icon = qvariant_cast<QIcon>(index.model()->data(index, Qt::DecorationRole));
    if(!icon.isNull()) {
        QSize size = option.decorationSize;
        iconWidth += size.width();
        icon.paint(painter, option.rect.x(), option.rect.y(), size.width(), size.height());
    }
    painter->translate(option.rect.x() + iconWidth, option.rect.y());
    _document.drawContents(painter);
    painter->restore();
}


QSize QAttributeDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString html = index.model()->data(index, Element::HtmlTextRole).toString();
    _document.setDefaultFont(option.font);
    _document.setHtml(html);
    QSizeF sizeF = _document.size();
    return QSize(sizeF.width(), sizeF.height());
}

