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

#ifndef ALLOWEDITEMSINSERT_H
#define ALLOWEDITEMSINSERT_H

#include <QDialog>
#include <QTreeWidgetItem>
#include <QStack>
#include <QSet>

#include "xsdeditor/validator/xelementcontent.h"

namespace Ui
{
class AllowedItemsInsert;
}

class AllowedItemsInsert : public QDialog
{
    Q_OBJECT

    /****************** future releases ***********
    bool _redoing;
    QStack< QSet<int> > _undo;
    QStack< QSet<int> > _redo;
    *******************************************/
    XElementContent *_content;
    QList<XSchemaObject*> *_selection;

    void collectSelected(QTreeWidgetItem *parentItem);
    void addItem(XSingleElementContent *target, QTreeWidgetItem *parentItem);
    void unselectRecursive(QTreeWidgetItem * item);
    bool isSameItem(QTreeWidgetItem * item, XSingleElementContent *reference);
    void saveState();
    void enableControls();
    void restoreState();

public:
    explicit AllowedItemsInsert(XElementContent *content, QList<XSchemaObject*> *result, QWidget *parent = 0);
    ~AllowedItemsInsert();


private:
    Ui::AllowedItemsInsert *ui;

    void accept();
private slots:
    void on_allowedItems_itemClicked(QTreeWidgetItem * item, int column)    ;
    void on_undo_clicked();
    void on_redo_clicked();

};

bool ChooseItemsBySchema(QWidget *parent, XElementContent *content, QList<XSchemaObject*> *result);

#endif // ALLOWEDITEMSINSERT_H
