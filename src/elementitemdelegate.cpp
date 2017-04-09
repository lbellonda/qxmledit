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

#include "elementitemdelegate.h"
#include "element.h"
#include "utils.h"

static const int WIDTH_BAND = 4 ;

const QBrush ElementItemDelegate::normalBrush(QColor(0xC0, 0xC0, 0xC0));
const QBrush ElementItemDelegate::editedBrush(QColor(0xFF, 0x80, 0x80));
const QBrush ElementItemDelegate::savedBrush(QColor(0x40, 0xC0, 0x40));


ElementItemDelegate::ElementItemDelegate(QAbstractItemDelegate *newBaseDelegate, QObject *parent) :
    QAbstractItemDelegate(parent)
{
    Utils::TODO_THIS_RELEASE("check se ancora necessario");
    baseDelegate = newBaseDelegate ;
    connect(baseDelegate, SIGNAL(closeEditor(QWidget*, QAbstractItemDelegate::EndEditHint)), this, SLOT(closeEditorBridge(QWidget*, QAbstractItemDelegate::EndEditHint)));
    connect(baseDelegate, SIGNAL(commitData(QWidget*)), this, SLOT(commitDataBridge(QWidget*)));
    connect(baseDelegate, SIGNAL(sizeHintChanged(const QModelIndex &)), this, SLOT(sizeHintChangedBridge(const QModelIndex &)));
}

ElementItemDelegate::~ElementItemDelegate()
{
}

void ElementItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    baseDelegate->paint(painter, option, index);
    Element * element = Element::fromModelIndex(index);
    if(NULL != element) {
        STYLEVIEWITEM_OPTION4(option);
        QRect markRect = option.rect;
        int startPosition = static_cast<const QTreeWidget*>(option4.widget)->columnViewportPosition(0) ;
        markRect.setLeft(startPosition);
        markRect.setWidth(WIDTH_BAND);
        if(element->saved()) {
            painter->fillRect(markRect, savedBrush);
        } else if(element->edited()) {
            painter->fillRect(markRect, editedBrush);
        } else {
            painter->fillRect(markRect, normalBrush);
        }
    }
}


QWidget * ElementItemDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    return baseDelegate->createEditor(parent, option, index);
}

bool ElementItemDelegate::editorEvent(QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index)
{
    return baseDelegate->editorEvent(event, model,  option,  index);
}

void ElementItemDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
    baseDelegate->setEditorData(editor, index);
}

void ElementItemDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
    baseDelegate->setModelData(editor, model, index);
}

QSize ElementItemDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    return baseDelegate->sizeHint(option, index);
}

void ElementItemDelegate::updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    baseDelegate->updateEditorGeometry(editor, option, index);
}

void ElementItemDelegate::closeEditorBridge(QWidget * editor, QAbstractItemDelegate::EndEditHint hint)
{
    emit closeEditor(editor,  hint);
}

void ElementItemDelegate::commitDataBridge(QWidget * editor)
{
    emit commitData(editor);
}

void ElementItemDelegate::sizeHintChangedBridge(const QModelIndex & index)
{
    emit sizeHintChanged(index);
}


