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


#include "copyattributesdialog.h"
#include "ui_copyattributesdialog.h"
#include "utils.h"
#include "modules/copyattr/copiedattribute.h"

void CopyAttributesDialog::showDialog(Element *element, CopyAttributesManager *manager, QWidget *parent)
{
    CopyAttributesDialog dialog(element, manager, parent) ;
    dialog.exec();
}

CopyAttributesDialog::CopyAttributesDialog(Element *element, CopyAttributesManager *manager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CopyAttributesDialog)
{
    _manager = manager ;
    _element = element ;
    ui->setupUi(this);
    loadData();
    on_cmdSelectAll_clicked();
    setupUi();
}

CopyAttributesDialog::~CopyAttributesDialog()
{
    delete ui;
}

void CopyAttributesDialog::setupUi()
{
    ui->attributes->horizontalHeader()->setStretchLastSection(true);
    ui->attributes->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->attributes->setSelectionMode(QAbstractItemView::SingleSelection);

    UITableCheckWidgetHelper *helper = new UITableCheckWidgetHelper(ui->attributes);
    helper->addSomeWidget(ui->buttonBox->button(QDialogButtonBox::Ok));
    helper->addSomeWidget(ui->cmdDeselectAll);
    _uiManager.addHelper(helper);
    _uiManager.fireEvent();
    ui->attributes->sortItems(1);
}

void CopyAttributesDialog::loadData()
{
    ui->attributes->setUpdatesEnabled(false);
    ui->attributes->setColumnCount(3);
    QStringList labels;
    labels << "" << tr("Name") << tr("value");
    ui->attributes->setHorizontalHeaderLabels(labels);
    bool error = false;

    int rows = _element->getAttributesList().size();
    ui->attributes->setRowCount(rows);
    int row = 0 ;
    foreach(Attribute * attribute, _element->getAttributesList()) {
        QTableWidgetItem *item0 = new QTableWidgetItem();
        if(NULL == item0) {
            error = true ;
        } else {
            item0->setFlags(((item0->flags() | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable)) & (~Qt::ItemIsEditable));
            item0->setCheckState(Qt::Unchecked);
            item0->setTextAlignment(Qt::AlignCenter | Qt::AlignHCenter);
            ui->attributes->setItem(row, 0, item0);
        }
        QTableWidgetItem *item1 = new QTableWidgetItem();
        if(NULL == item1) {
            error = true ;
        } else {
            item1->setText(attribute->name);
            item1->setFlags(item1->flags() & (~Qt::ItemIsEditable));
            ui->attributes->setItem(row, 1, item1);
        }
        QTableWidgetItem *item2 = new QTableWidgetItem();
        if(NULL == item2) {
            error = true ;
        } else {
            item2->setText(attribute->value);
            item2->setFlags(item2->flags() & (~Qt::ItemIsEditable));
            ui->attributes->setItem(row, 2, item2);
        }
        row++;
    }
    if(error) {
        Utils::error(this, tr("Fatal error"));
        close();
    }
    ui->attributes->resizeColumnsToContents();
    ui->attributes->setUpdatesEnabled(true);
}


void CopyAttributesDialog::retrieveSelection(QList<Attribute*> &resList)
{
    int rows = ui->attributes->rowCount();
    for(int row = 0 ; row < rows ; row ++) {
        QTableWidgetItem *item0 = ui->attributes->item(row, 0);
        if(item0->checkState() == Qt::Checked) {
            Attribute *attribute = new Attribute();
            if(NULL != attribute) {
                QTableWidgetItem *itemName = ui->attributes->item(row, 1);
                QTableWidgetItem *itemValue = ui->attributes->item(row, 2);
                attribute->name = itemName->text();
                attribute->value = itemValue->text();
                resList.append(attribute);
            } else {
                Utils::errorOutOfMem(this);
            }
        }
    }
}

bool CopyAttributesDialog::saveData()
{
    QList<Attribute*> resList;
    retrieveSelection(resList);
    if(resList.isEmpty()) {
        return false;
    }
    CopyAttributesSession *session = new CopyAttributesSession();
    session->setAttributes(resList);
    QString title ;
    bool isFirst = true;
    QString clipboardData;
    foreach(Attribute * attribute, resList) {
        if(isFirst) {
            isFirst = false;
        } else {
            title += ", ";
            clipboardData += " ";
        }
        title += attribute->name;
        clipboardData += QString("%1=\"%2\"").arg(attribute->name).arg(attribute->value);
    }
    // set data to the clipboard
    QClipboard *clipboard = QApplication::clipboard();
    if(NULL != clipboard) {
        clipboard->setText(clipboardData);
    }

    session->setName(title);
    _manager->addSession(session);
    return true;
}


void CopyAttributesDialog::accept()
{
    if(saveData()) {
        QDialog::accept();
    }
}


void CopyAttributesDialog::selectAllItems(const bool select)
{
    ui->attributes->setUpdatesEnabled(false);
    int rows = ui->attributes->rowCount();
    Qt::CheckState state = select ? Qt::Checked : Qt::Unchecked ;
    for(int row = 0 ; row < rows ; row ++) {
        QTableWidgetItem *item0 = ui->attributes->item(row, 0);
        item0->setCheckState(state);
    }
    ui->attributes->setUpdatesEnabled(true);
}

void CopyAttributesDialog::on_cmdSelectAll_clicked()
{
    selectAllItems(true);
}

void CopyAttributesDialog::on_cmdDeselectAll_clicked()
{
    selectAllItems(false);
}
