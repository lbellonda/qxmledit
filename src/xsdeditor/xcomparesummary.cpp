/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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


#include "xcomparesummary.h"
#include "ui_xcomparesummary.h"
#include "utils.h"
#include "xsdeditor/xsdwindow.h"

XCompareSummary::XCompareSummary(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::XCompareSummary)
{
    ui->setupUi(this);
    setupData();
}

XCompareSummary::~XCompareSummary()
{
    delete ui;
}


void XCompareSummary::setReference(const QString &path)
{
    ui->referenceFile->setText(path);
    ui->referenceFile->setToolTip(path);
}


void XCompareSummary::setTarget(const QString &path)
{
    ui->targetPath->setText(path);
    ui->targetPath->setToolTip(path);
}

void XCompareSummary::setupData()
{
    ui->treeWidget->setColumnCount(3);
    QStringList headers;
    headers << tr("Type");
    headers << tr("Name");
    headers << tr("Id");
    ui->treeWidget->setHeaderLabels(headers);
    ui->treeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
}


void XCompareSummary::setSummaryData(const QList<XSchemaObject*> &added, const QList<XSchemaObject*> &modified, const QList<XSchemaObject*> &deleted)
{
    ui->treeWidget->setUpdatesEnabled(false);
    ui->treeWidget->clear();
    if(added.isEmpty() && modified.isEmpty() && deleted.isEmpty()) {
        ui->stackedWidget->setCurrentWidget(ui->noDataPage);
    } else {
        loadList(tr("Added"), added, XSDItem::colorAddStart);
        loadList(tr("Modified"), modified, XSDItem::colorModStart);
        loadList(tr("Deleted"), deleted, XSDItem::colorDelStart);
        ui->stackedWidget->setCurrentWidget(ui->treePage);
    }
    ui->treeWidget->expandAll();
    ui->treeWidget->resizeColumnToContents(0);
    ui->treeWidget->resizeColumnToContents(1);
    ui->treeWidget->resizeColumnToContents(2);
    ui->treeWidget->setUpdatesEnabled(true);
}


void XCompareSummary::loadList(const QString & title, const QList<XSchemaObject*> &items, const QColor bkColor)
{
    if(items.isEmpty()) {
        return ;
    }
    QStringList titleList;
    titleList.append(title);
    QTreeWidgetItem *itemTitle = new QTreeWidgetItem(titleList);
    itemTitle->setData(0, Qt::UserRole, qVariantFromValue((void*)NULL));
    ui->treeWidget->addTopLevelItem(itemTitle);
    ui->treeWidget->setFirstItemColumnSpanned(itemTitle, true);
    itemTitle->setBackgroundColor(0, bkColor);
    foreach(XSchemaObject * item, items) {
        QTreeWidgetItem *itemElement = new QTreeWidgetItem(itemTitle);
        itemElement->setText(0, item->tagName());
        itemElement->setText(1, item->name());
        itemElement->setText(2, item->id());
        itemElement->setData(0, Qt::UserRole, qVariantFromValue((void*)item));
        itemTitle->addChild(itemElement);
    }
}

void XCompareSummary::on_treeWidget_itemDoubleClicked(QTreeWidgetItem * item, int /*column*/)
{
    QVariant data = item->data(0, Qt::UserRole);
    XSchemaObject *object = (XSchemaObject *)data.value<void*>();
    if(NULL != object) {
        emit objectDoubleClicked(object);
    }
}
