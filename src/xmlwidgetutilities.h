/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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


#ifndef XMLWIDGETUTILITIES_H
#define XMLWIDGETUTILITIES_H

#include <QComboBox>
#include <QDomDocument>
#include <QTreeWidgetItem>
#include <QTreeWidget>

#include "libQXmlEdit_global.h"

class QAttributeDelegate;
class PaintInfo;
class Element;
class ElementItemSingleDelegate;

class LIBQXMLEDITSHARED_EXPORT XmlWidgetUtilities : public QObject
{
    Q_OBJECT
    QTreeWidget *_tree;
    ElementItemSingleDelegate *_tagDelegate;
public:
    XmlWidgetUtilities();
    virtual ~XmlWidgetUtilities();

    QAttributeDelegate* setUpTreeXmlDelegates(QWidget *parent, QTreeWidget *treeWidget, PaintInfo *paintInfo);
    void resetTree(QTreeWidget *treeWidget, PaintInfo *paintInfo);
    void setDataColumnTitle(QTreeWidget *treeWidget, PaintInfo *paintInfo, const bool isCompactView);
    void calcColumnState(QTreeWidget *treeWidget, PaintInfo *paintInfo);
    void resizeTreeColumns(QTreeWidget *treeWidget);
    Element *getSelectedItem(QTreeWidget *treeWidget);
    Element *getSelectedItem();
    void setTree(QTreeWidget *tree);
    bool filterEvent(QObject * obj, QEvent * event, QHash<void *, QString> *mapDataAnon = NULL);
    ElementItemSingleDelegate *tagDelegate();
    //---
    static void setTableBehavior(QTableWidget *table);
    static void swapTableRows(QTableWidget *table, const int row1, const int row2);
};

#endif // XMLWIDGETUTILITIES_H
