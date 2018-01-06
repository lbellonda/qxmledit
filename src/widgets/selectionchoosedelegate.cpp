/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2018 by Luca Bellonda and individual contributors  *
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

#include "selectionchoosedelegate.h"
#include <QComboBox>
#include "utils.h"
#include "xsdeditor/xschema.h"

SelectionChooseDelegate::SelectionChooseDelegate(QWidget *parent) : QStyledItemDelegate(parent)
{
}

SelectionChooseDelegate::~SelectionChooseDelegate()
{
}


QWidget *SelectionChooseDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const

{
    if(index.column() == ColumnValue) {
        QComboBox *editor = new QComboBox(parent);
        editor->setEditable(true);
        return editor;
    } else {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

void SelectionChooseDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.column() == ColumnValue) {
        QVariant data = index.data(Qt::UserRole);
        QString value = index.data().toString();
        AttrCollectInfo *info = (AttrCollectInfo *)data.value<void*>();
        QComboBox *combo = qobject_cast<QComboBox *>(editor);
        Utils::loadComboTextArrays(combo, value, info->enums, info->enums);
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void SelectionChooseDelegate::setModelData(QWidget * editor, QAbstractItemModel *model,
        const QModelIndex &index) const
{
    QStyledItemDelegate::setModelData(editor, model, index);
    if(index.column() == ColumnValue) {
        QModelIndex checkIndex = model->index(index.row(), 0);
        model->setData(checkIndex, Qt::Checked, Qt::CheckStateRole);
    }
}
