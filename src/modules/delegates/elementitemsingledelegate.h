/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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


#ifndef ELEMENTITEMSINGLEDELEGATE_H
#define ELEMENTITEMSINGLEDELEGATE_H

#include <QStyledItemDelegate>
#include <QTextDocument>

class PaintInfo;
class QTreeWidget ;
class QTreeWidgetItem ;
class Element;
struct ElementDisplayInfo;

class ElementItemSingleDelegate : public QStyledItemDelegate
{
    Q_OBJECT

    static const int HGap = 2 ;
    static const int EditStateBandWidth ;

    static const QBrush normalBrush;
    static const QBrush editedBrush;
    static const QBrush savedBrush;

    mutable QTextDocument _document;
    PaintInfo *_paintInfo ;
    QColor _colorInfo;
    QBrush _commentBrush;
    QBrush _cyanBrush;
    QBrush _infoBrush;
    QBrush _textDeclBrush;
    QColor _attrSelectedText;
    QColor _attrNamesColor;
    QColor _attrValuesColor;
    QColor _hilitedTextColor;
    QColor _tagColor;
    bool _inited ;
    bool _hasColorInverted ;
    bool _isAnonPreview;
    QColor _attrAnonValuesColor;
    QColor _textAnonColor;
    QHash<void *, QString> *_origDataForAnonPreview;

    void calcTextColor(const QStyleOptionViewItem & option);
    bool diffColorOverThr(QColor &c1, QColor &c2, const int threshold);
    bool diffLightnessThr(QColor &c1, QColor &c2, const int threshold);
public:
    explicit ElementItemSingleDelegate(PaintInfo * paramPaintInfo, QObject *parent = 0);
    ~ElementItemSingleDelegate();

    virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;

    void reset();

    bool isAnonPreview() const;
    void setIsAnonPreview(bool isAnonPreview);
    QHash<void *, QString> *origDataForAnonPreview() const;
    void setOrigDataForAnonPreview(QHash<void *, QString> *origDataForAnonPreview);

    static void findRects(QTreeWidget * widget, QTreeWidgetItem *item, QRect &itemRect, Element *element, ElementDisplayInfo *elementDisplayInfo) ;

};

#endif // ELEMENTITEMSINGLEDELEGATE_H
