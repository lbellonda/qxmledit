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

#ifndef ELEMENTITEMDELEGATE_H
#define ELEMENTITEMDELEGATE_H

#include <QAbstractItemDelegate>

class ElementItemDelegate : public QAbstractItemDelegate
{
    Q_OBJECT

    static const QBrush normalBrush;
    static const QBrush editedBrush;
    static const QBrush savedBrush;

    QAbstractItemDelegate *baseDelegate;
public:
    explicit ElementItemDelegate(QAbstractItemDelegate *newBaseDelegate, QObject *parent = 0);
    virtual ~ElementItemDelegate();

    virtual QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
    virtual bool editorEvent(QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index);
    virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
    virtual void setEditorData(QWidget * editor, const QModelIndex & index) const;
    virtual void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;
    virtual void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const;

private slots:
    void closeEditorBridge(QWidget * editor, QAbstractItemDelegate::EndEditHint hint = NoHint);
    void commitDataBridge(QWidget * editor);
    void sizeHintChangedBridge(const QModelIndex & index);

};

#endif // ELEMENTITEMDELEGATE_H
