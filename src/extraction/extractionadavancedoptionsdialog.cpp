/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2019 by Luca Bellonda and individual contributors       *
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

#include "extractionadavancedoptionsdialog.h"
#include "ui_extractionadavancedoptionsdialog.h"
#include "extractionscriptingprovider.h"
#include "utils.h"

ExtractionAdavancedOptionsDialog::ExtractionAdavancedOptionsDialog(ExtractionOperation *theOperation, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExtractionAdavancedOptionsDialog)
{
    _operation = theOperation;
    ui->setupUi(this);
    setup();
}

ExtractionAdavancedOptionsDialog::~ExtractionAdavancedOptionsDialog()
{
    delete ui;
}

QSet<QString> ExtractionAdavancedOptionsDialog::setOfScripts()
{
    QStringList idList = _operation->filtersId().split(",");
    return idList.toSet();
}

void ExtractionAdavancedOptionsDialog::setup()
{
    ui->cbUseNamespaces->setChecked(_operation->isUseNamespaces());
    setupScripts();
}

void ExtractionAdavancedOptionsDialog::setupScripts()
{
    ui->cbUseNamespaces->setChecked(_operation->isUseNamespaces());
    ExtractionScriptingProvider provider;
    bool isError = false;
    QList<ExtractionScriptFilterModel*> models = provider.getPredefinedScripts(isError);
    if(isError) {
        Utils::error(this, tr("Unable to show predefined scripts."));
    }
    QSet<QString> scriptIdSelectedSet = setOfScripts();
    QStringList labels;
    labels << "" << tr("Name") << tr("Description");
    ui->scriptTable->setColumnCount(3);
    ui->scriptTable->setHorizontalHeaderLabels(labels);
    ui->scriptTable->setAlternatingRowColors(true);
    ui->scriptTable->horizontalHeader()->setStretchLastSection(true);

    int row = ui->scriptTable->rowCount();
    foreach(ExtractionScriptFilterModel* filter, models) {
        // load a single script info
        ui->scriptTable->setRowCount(row + 1);
        QTableWidgetItem *itemMod = newTableItem("", filter->id());
        QTableWidgetItem *itemName = newTableItem(filter->name(), NULL);
        QTableWidgetItem *itemDescription = newTableItem(filter->description(), NULL);
        itemMod->setCheckState(scriptIdSelectedSet.contains(filter->id()) ? Qt::Checked : Qt::Unchecked);
        itemMod->setFlags(itemMod->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
        ui->scriptTable->setItem(row, 0, itemMod);
        ui->scriptTable->setItem(row, 1, itemName);
        ui->scriptTable->setItem(row, 2, itemDescription);
        row++;
    }
    ui->scriptTable->resizeRowsToContents();
    ui->scriptTable->resizeColumnsToContents();
    EMPTYPTRLIST(models, ExtractionScriptFilterModel);
}

QTableWidgetItem* ExtractionAdavancedOptionsDialog::newTableItem(const QString &text, const QString &userData)
{
    QTableWidgetItem *newItem = new QTableWidgetItem(text);
    newItem->setData(Qt::UserRole, userData);
    newItem->setFlags(newItem->flags() & ~(Qt::ItemIsEditable | Qt::ItemIsUserCheckable));
    return newItem;
}

void ExtractionAdavancedOptionsDialog::accept()
{
    _operation->setUseNamespaces(ui->cbUseNamespaces->isChecked());
    QStringList ids;
    const int rows = ui->scriptTable->rowCount();
    FORINT(row, rows) {
        if(ui->scriptTable->item(row, 0)->checkState() == Qt::Checked) {
            QVariant data = ui->scriptTable->item(row, 0)->data(Qt::UserRole);
            ids.append(data.toString());
        }
    }
    _operation->setFiltersId(ids.join(","));
    QDialog::accept();
}

void ExtractionAdavancedOptionsDialog::on_cmdSelectAll_clicked()
{
    selectAllOperation(true);
}

void ExtractionAdavancedOptionsDialog::on_cmdSelectNone_clicked()
{
    selectAllOperation(false);
}

void ExtractionAdavancedOptionsDialog::selectAllOperation(const bool how)
{
    const int rows = ui->scriptTable->rowCount();
    FORINT(row, rows) {
        ui->scriptTable->item(row, 0)->setCheckState(how ? Qt::Checked : Qt::Unchecked);
    }
}
