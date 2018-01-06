/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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

#ifndef NAVIGATIONTREE_H
#define NAVIGATIONTREE_H

#include <QTreeWidget>

class XSchemaObject;
class XSDSchema;

class NavigationTree : public QTreeWidget
{
    Q_OBJECT

    //---- icons
    QIcon typeComplexIcon;
    QIcon typeSimpleIcon;
    QIcon referenceIcon;
    QIcon elementIcon;
    QIcon attributeIcon;

    QFont   navigationFont;
    QFont   chaptersNavFont;

    QColor tilesColor;
    QBrush titlesBrush;
    QBrush transparentBrush;
    QBrush highlightBrush;

    QTreeWidgetItem *topLevelNavigation ;
    QTreeWidgetItem *topLevelElements ;
    QTreeWidgetItem *topLevelGroups ;
    QTreeWidgetItem *topLevelTypes ;
    QTreeWidgetItem *topLevelAttributes ;
    QTreeWidgetItem *topLevelAttributeGroups ;
    QTreeWidgetItem *topLevelReferences ;
    QTreeWidgetItem *topLevelOtherElements ;

    QTreeWidgetItem *topLevelNavigationSpacer ;
    QTreeWidgetItem *topLevelElementsSpacer ;
    QTreeWidgetItem *topLevelGroupsSpacer ;
    QTreeWidgetItem *topLevelTypesSpacer ;
    QTreeWidgetItem *topLevelAttributesSpacer ;
    QTreeWidgetItem *topLevelAttributeGroupsSpacer ;
    QTreeWidgetItem *topLevelReferencesSpacer ;
    QTreeWidgetItem *topLevelOtherElementsSpacer ;
    QList<QTreeWidgetItem *>allNavigationItems ;
    QList<QTreeWidgetItem *>matchedItems;

    bool useOnlyThisSchemaOrAll;


    QTreeWidgetItem* addSpacer();
    void init();
    QTreeWidgetItem *addTopLevelCategory(const QString &label);
    XSchemaObject *fromItemData(QTreeWidgetItem *item);
    QList<XSchemaObject*> sortObjectsByName(const QList<XSchemaObject*> &objects);

    void loadObjects(QList<XSchemaObject*> source, QTreeWidgetItem *topLevelItem, QIcon icon, QTreeWidgetItem *topLevelItemSpace);
    QTreeWidgetItem *objectItem(XSchemaObject * object, QIcon icon);
    QString mangleName(XSchemaObject * object);


public:
    explicit NavigationTree(QWidget *parent = 0);
    virtual ~NavigationTree();

    void setupNavigationBaseItems();
    void emptyNavigationBox();
    void loadNavigationBox(XSDSchema *schema);
    void setNavigationTargetSelection(XSchemaObject *newSelection);
    void newSearchText(const QString &newText);

signals:

public slots:

};

#endif // NAVIGATIONTREE_H
