/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016 by Luca Bellonda and individual contributors       *
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

#include "specificpropertiesdialog.h"
#include "ui_specificpropertiesdialog.h"
#include "modules/xsd/namespacemanager.h"
#include "utils.h"

/**
  In the tree item, the top level is the editor and the item is the code for the selection.
 */

SpecificPropertiesDialog::SpecificPropertiesDialog(QWidget *parent, QList<HandlerForInsert *> *handlers) :
    QDialog(parent),
    ui(new Ui::SpecificPropertiesDialog)
{
    _handler = NULL ;
    _handlers = handlers ;
    ui->setupUi(this);
    setupTree();
    setupData();
}

SpecificPropertiesDialog::~SpecificPropertiesDialog()
{
    delete ui;
}

QString SpecificPropertiesDialog::selectedItemCode()
{
    return _selectedItemCode ;
}

HandlerForInsert *SpecificPropertiesDialog::handler()
{
    return _handler ;
}

void SpecificPropertiesDialog::setupTree()
{
    ui->treeWidget->setColumnCount(1) ;
    QStringList labels;
    labels << tr("Namespace") ;
    ui->treeWidget->setHeaderLabels(labels);
}

void SpecificPropertiesDialog::setupData()
{
    foreach(const HandlerForInsert *handler, *_handlers) {
        QTreeWidgetItem * item = new QTreeWidgetItem();
        item->setText(0, handler->name);
        ui->treeWidget->addTopLevelItem(item);
        item->setFlags(item->flags() & (~Qt::ItemIsEditable));
        item->setData(0, Qt::UserRole, qVariantFromValue((void*)handler));
        foreach(SingleHandlerForInsert* singleHanlder, handler->elements) {
            QTreeWidgetItem * subItem = new QTreeWidgetItem();
            QString theName = QString("%1 %2").arg(singleHanlder->name).arg(singleHanlder->description);
            subItem->setText(0, theName);
            subItem->setFlags(subItem->flags() & (~Qt::ItemIsEditable));
            subItem->setData(0, Qt::UserRole, qVariantFromValue(singleHanlder->id));
            item->addChild(subItem);
        }
    }
    ui->treeWidget->expandAll();
}

bool SpecificPropertiesDialog::checkAndStoreSelection()
{
    QList<QTreeWidgetItem *>selItems =  ui->treeWidget->selectedItems();
    if(selItems.count() < 1) {
        return false ;
    }
    QTreeWidgetItem *item = selItems.at(0);
    QTreeWidgetItem *itemParent = item->parent();
    if(itemParent == NULL) {
        return false;
    }
    QVariant parentData = itemParent->data(0, Qt::UserRole);
    HandlerForInsert *handlerForInsert = static_cast<HandlerForInsert*>(parentData.value<void*>());
    QVariant data = item->data(0, Qt::UserRole);
    _handler = handlerForInsert ;
    _selectedItemCode = data.toString() ;
    return true ;
}

void SpecificPropertiesDialog::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *, int)
{
    accept();
}

void SpecificPropertiesDialog::accept()
{
    if(checkAndStoreSelection()) {
        QDialog::accept();
    }
}
