/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2018 by Luca Bellonda and individual contributors  *
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

#ifndef COMPARESIDEBYSIDEVIEW_H
#define COMPARESIDEBYSIDEVIEW_H

#include "diffresult.h"
#include "comparechrome.h"

/*
 *this class builds the side by side view
 */

class CompareSideBySideView : public QObject
{
    Q_OBJECT

    static const int OffsetLastDiff;
    static const int OffsetThisDiff;

    QFont   _itemsFonts;
    bool    _useItemsFonts;

    enum {
        TYPE_ELEMENT,
        TYPE_ATTRIBUTE_CONSTANT
    };

    CompareChrome _chrome;

    void setupItem(DiffSingleNodeResult *node, QTreeWidgetItem *currentNodeItem, const bool isReference, QList<QTreeWidgetItem*> &itemsList, QList<DiffInfo*> &diffList, const bool isChildrenBlocked);
    void loadSinglePart(DiffSingleNodeResult *node, QTreeWidgetItem *parentItem, QTreeWidget *tree, const bool isReference, QList<QTreeWidgetItem*> &itemsList, QList<DiffInfo*> &diffList, const bool isChildrenBlocked);
    void setGraphic(QTreeWidgetItem *currentNodeItem, const EDiff::KDiff state, const bool isEmpty);
    bool isNodeEmpty(DiffSingleNodeResult *node, const bool isReference);
    SourceElementDiffOperation *getSource(DiffSingleNodeResult *node, const bool isReference);
    QString getText(SourceElementDiffOperation *source, const EDiff::KDiff state, const bool isEmpty);

    void appendAttributes(DiffSingleNodeResult *node, QTreeWidgetItem *attributeTextItem, const EDiff::KDiff state, const bool isReference, const bool isEmpty, QList<Attribute*> &attributes, QList<QTreeWidgetItem*> &itemsList, QList<DiffInfo*> &diffList, const bool isChildrenBlocked);
    void appendAttribute(DiffSingleNodeResult *node, QTreeWidgetItem *attributeTextItem, Attribute *attribute, const EDiff::KDiff state, const bool isReference, const bool isEmpty, QList<QTreeWidgetItem*> &itemsList, QList<DiffInfo*> &diffList, const bool isChildrenBlocked);

    DiffInfo* createNewDiff(QList<DiffInfo*> &diffList, const EDiff::KDiff diffType, const bool isElementOrAttribute, DiffSingleNodeResult *newDiff, Attribute* attribute);
    void setLastItemDiff(const bool isReference, QList<QTreeWidgetItem*> &itemsList, QList<DiffInfo*> &diffList, DiffInfo *info);
    QTreeWidgetItem *newTreeItem(QList<QTreeWidgetItem*> &itemsList);

    static QString getElidedText(const QString &text);

public:
    CompareSideBySideView();
    ~CompareSideBySideView();

    void loadSinglePart(QList<DiffSingleNodeResult *>& nodes, QTreeWidget *tree, const bool isReference, QList<QTreeWidgetItem*> &itemsList, QList<DiffInfo*> &diffList);
    void setItemsFonts(const QFont &newFont);

    static int lastDiffIndexFromItem(QTreeWidgetItem *item);
    static int thisDiffIndexFromItem(QTreeWidgetItem *item);

};

#endif // COMPARESIDEBYSIDEVIEW_H
