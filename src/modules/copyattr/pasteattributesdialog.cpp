/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013-2018 by Luca Bellonda and individual contributors  *
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


#include "pasteattributesdialog.h"
#include "ui_pasteattributesdialog.h"

#include "utils.h"
#include "copiedattribute.h"
#include "regola.h"

PasteAttributesDialog::PasteAttributesDialog(Element *element, CopyAttributesManager *manager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PasteAttributesDialog)
{
    ui->setupUi(this);
    _manager = manager ;
    _element = element ;
    loadStartData();
    setupTable();
    loadData();
    on_cmdSelectAll_clicked();
    setupUI();
}

PasteAttributesDialog::~PasteAttributesDialog()
{
    delete ui;
}

void PasteAttributesDialog::showDialog(Element *element, CopyAttributesManager *manager, QWidget *parent)
{
    PasteAttributesDialog dialog(element, manager, parent) ;
    dialog.exec();
}

void PasteAttributesDialog::setupTable()
{
    ui->attributes->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->attributes->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->attributes->setColumnCount(3);
    ui->attributes->horizontalHeader()->setStretchLastSection(true);
    QStringList labels;
    labels << tr("") << tr("Name") << tr("value");
    ui->attributes->setHorizontalHeaderLabels(labels);
}

void PasteAttributesDialog::setupUI()
{
    UITableCheckWidgetHelper *helper = new UITableCheckWidgetHelper(ui->attributes);
    helper->addSomeWidget(ui->buttonBox->button(QDialogButtonBox::Ok));
    helper->addSomeWidget(ui->cmdDeselectAll);
    _uiManager.addHelper(helper);
    _uiManager.fireEvent();
    ui->attributes->sortByColumn(1);
}

void PasteAttributesDialog::on_cmdClearClipboard_clicked()
{
    _manager->resetExcept(_currentSession);
    ui->sessions->clear();
    loadStartData();
    loadData();
}

void PasteAttributesDialog::on_sessions_currentIndexChanged(int index)
{
    if(index >= 0) {
        QVariant variant = ui->sessions->itemData(index);
        _currentSession = (CopyAttributesSession *)variant.value<void*>();
        loadData();
    }
}

void PasteAttributesDialog::loadStartData()
{
    // load session data into combo box
    foreach(CopyAttributesSession * session, _manager->sessions()) {
        ui->sessions->addItem(session->name(), qVariantFromValue((void*)session));
    }
    ui->sessions->setCurrentIndex(0);
}

void PasteAttributesDialog::loadData()
{
    _uiManager.pause();
    ui->attributes->setUpdatesEnabled(false);
    bool error = false;

    ui->attributes->clearContents();

    int rows = 0 ;
    if(NULL != _currentSession) {
        rows = _currentSession->attributes().size();
        ui->attributes->setRowCount(rows);
        int row = 0 ;
        foreach(Attribute * attribute, _currentSession->attributes()) {
            QTableWidgetItem *item0 = new QTableWidgetItem();
            if(NULL == item0) {
                error = true ;
            } else {
                item0->setFlags(((item0->flags() | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable)) & (~Qt::ItemIsEditable));
                item0->setCheckState(Qt::Checked);
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
    }
    if(error) {
        Utils::error(this, tr("Fatal error"));
        close();
    }
    ui->attributes->resizeColumnsToContents();
    ui->attributes->setUpdatesEnabled(true);
    ui->attributes->horizontalHeader()->setStretchLastSection(true);
    _uiManager.fireEvent();
}

void PasteAttributesDialog::on_cmdSelectAll_clicked()
{
    selectAllItems(true);
}

void PasteAttributesDialog::on_cmdDeselectAll_clicked()
{
    selectAllItems(false);
}

void PasteAttributesDialog::selectAllItems(const bool select)
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

void PasteAttributesDialog::accept()
{
    if(saveData()) {
        QDialog::accept();
    }
}


bool PasteAttributesDialog::saveData()
{
    QList<Attribute*> resList;
    retrieveSelection(resList);
    if(resList.isEmpty()) {
        return false;
    }
    foreach(Attribute * attribute, resList) {
        delete attribute ;
    }
    _element->getParentRule()->pasteAttributes(this, _element->getUI()->treeWidget(), _currentSession);
    return true;
}


void PasteAttributesDialog::retrieveSelection(QList<Attribute*> &resList)
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
