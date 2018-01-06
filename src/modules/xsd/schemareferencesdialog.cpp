/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2018 by Luca Bellonda and individual contributors  *
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


#include "schemareferencesdialog.h"
#include "ui_schemareferencesdialog.h"
#include "modules/namespace/namespacereferenceentry.h"
#include "modules/namespace/namespaceresult.h"
#include "modules/namespace/choosenamespacedialog.h"
#include "utils.h"

SchemaReferencesDialog::SchemaReferencesDialog(QWidget *parent, NamespaceManager *namespaceManager, NamespaceReferenceEntry *entry) :
    QDialog(parent),
    ui(new Ui::SchemaReferencesDialog)
{
    _namespaceManager = namespaceManager ;
    ui->setupUi(this);
    setupData(entry);
    enableButtons();
}

SchemaReferencesDialog::~SchemaReferencesDialog()
{
    delete ui;
}

void SchemaReferencesDialog::enableButtons()
{
    int row = ui->otherNs->currentRow();
    bool enableRow = row >= 0 ;
    ui->cmdDelNs->setEnabled(enableRow);
    ui->cmdModNs->setEnabled(enableRow);
    enableOK();
}

void SchemaReferencesDialog::enableOK()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(okEnabled());
}

bool SchemaReferencesDialog::okEnabled()
{
    bool enableOk = true;
    if(ui->enableNoNsSchemaLocation->isChecked() && ui->noNsSchemaLocation->text().trimmed().isEmpty()) {
        enableOk = false;
    }
    int rowCount = ui->otherNs->rowCount();
    FORINT(row, rowCount) {
        NamespaceResult setValues;
        valueForRow(ui->otherNs, row, &setValues);
        if(setValues.uri.isEmpty() || setValues.schemaLocation.isEmpty()) {
            enableOk = false;
            break;
        }
    }
    return enableOk;
}


void SchemaReferencesDialog::setupTable(QTableWidget *widget)
{
    widget->setColumnCount(2);
    QStringList labels;
    labels << QObject::tr("Namespace") << QObject::tr("Location") ;
    widget->setHorizontalHeaderLabels(labels);
}

void SchemaReferencesDialog::setupData(NamespaceReferenceEntry *entry)
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    ui->noNsSchemaLocation->setClearButtonEnabled(true);
#endif
    setupTable(ui->otherNs);
    ui->enableNoNsSchemaLocation->setChecked(entry->isUseNoNamespaceSchemaLocation());
    ui->noNsSchemaLocation->setText(entry->noNamespaceSchemaLocation());
    // all the other entries
    foreach(SingleNamespaceReferenceEntry * item, entry->references()) {
        setRow(-1, item->uri(), item->schemaLocation()) ;
    }
    ui->otherNs->resizeColumnsToContents();
}

int SchemaReferencesDialog::setRow(const int desiredRow, const QString &uri, const QString &location)
{
    QTableWidget *table = ui->otherNs;
    int row = desiredRow;
    if(row >= 0) {
        if(table->rowCount() > row) {
            table->removeRow(row);
        }
        table->insertRow(row);
    } else {
        row = table->rowCount();
        table->setRowCount(row + 1);
    }
    QString toolTip = QString("Uri: %1\nLocation: %2").arg(uri).arg(location);
    _tableUtils.insNsItem(table, row, 0, uri, NULL, toolTip);
    _tableUtils.insNsItem(table, row, 1, location, NULL, toolTip);
    return row ;
}

void SchemaReferencesDialog::on_cmdSelectNs_clicked()
{
    NamespaceResult setValues;
    setValues.uri = ui->noNsSchemaLocation->text().trimmed();
    ChooseNamespaceDialog dlg(this, _namespaceManager, &setValues);
    dlg.setModal(true);
    dlg.enablePrefix(false);
    if(dlg.exec() == QDialog::Accepted) {
        NamespaceResult result;
        dlg.nsChosen(&result);
        if(!isValidReference(result)) {
            Utils::error(this, tr("Invalid values for location or namespace."));
            return ;
        }
        ui->noNsSchemaLocation->setText(result.uri);
    }
    enableOk();
}

void SchemaReferencesDialog::enableOk()
{
    // nothing
}

void SchemaReferencesDialog::on_cmdDelNs_clicked()
{
    int selRow = ui->otherNs->currentRow();
    if(selRow >= 0) {
        if(Utils::askYN(this, tr("Do you really want to delete this declaration?"))) {
            ui->otherNs->removeRow(selRow);
        }
    }
}

void SchemaReferencesDialog::on_cmdAddNs_clicked()
{
    ChooseNamespaceDialog dlg(this, _namespaceManager, NULL);
    dlg.setModal(true);
    dlg.enablePrefix(false);
    if(dlg.exec() == QDialog::Accepted) {
        NamespaceResult result;
        dlg.nsChosen(&result);
        if(!isValidReference(result)) {
            Utils::error(this, tr("Invalid values for location or namespace."));
            return ;
        }
        int row = setRow(-1, result.uri, result.schemaLocation);
        ui->otherNs->selectRow(row);
        enableButtons();
    }
}

void SchemaReferencesDialog::on_cmdModNs_clicked()
{
    modCurrentRow();
}

void SchemaReferencesDialog::modCurrentRow()
{
    int row = ui->otherNs->currentRow();
    if(row >= 0) {
        NamespaceResult setValues;
        valueForRow(ui->otherNs, row, &setValues);
        ChooseNamespaceDialog dlg(this, _namespaceManager, &setValues);
        dlg.setModal(true);
        dlg.enablePrefix(false);
        if(dlg.exec() == QDialog::Accepted) {
            NamespaceResult result;
            dlg.nsChosen(&result);
            if(!isValidReference(result)) {
                Utils::error(this, tr("Invalid values for location or namespace."));
                return ;
            }
            setRow(row, result.uri, result.schemaLocation);
            ui->otherNs->resizeColumnsToContents();
            enableButtons();
        }
    }
}

bool SchemaReferencesDialog::valueForRow(QTableWidget *table, const int row, NamespaceResult *setValues)
{
    if(row >= 0) {
        setValues->uri = table->item(row, 0)->text().trimmed();
        setValues->schemaLocation = table->item(row, 1)->text().trimmed();
        return true;
    }
    return false;
}

void SchemaReferencesDialog::getResults(NamespaceReferenceEntry *result)
{
    result->setUseNoNamespaceSchemaLocation(ui->enableNoNsSchemaLocation->isChecked());
    result->setNoNamespaceSchemaLocation(ui->noNsSchemaLocation->text().trimmed());
    // all the other entries
    QList<SingleNamespaceReferenceEntry *>items;
    int rows = ui->otherNs->rowCount();
    FORINT(row, rows) {
        NamespaceResult setValues;
        valueForRow(ui->otherNs, row, &setValues);
        items.append(new SingleNamespaceReferenceEntry(setValues.uri, setValues.schemaLocation));
    }
    result->setReferences(items);
}


void SchemaReferencesDialog::accept()
{
    if(!okEnabled()) {
        return ;
    }
    QDialog::accept();
}

/*
 * Action for dialog:
 * if unselect cb -> disable edit box
 * when select row -> enable buttons
 * when modify or double click on row -> open dialof
 */
void SchemaReferencesDialog::on_enableNoNsSchemaLocation_clicked()
{
    enableButtons();
}

void SchemaReferencesDialog::on_otherNs_itemSelectionChanged()
{
    enableButtons();
}

void SchemaReferencesDialog::on_otherNs_cellDoubleClicked(int row, int /*column*/)
{
    if(row == ui->otherNs->currentRow()) {
        modCurrentRow();
    }
}

void SchemaReferencesDialog::on_noNsSchemaLocation_textChanged(const QString & /*text*/)
{
    QString val = ui->noNsSchemaLocation->text().trimmed();
    bool isEnabled = !val.isEmpty();
    if(ui->enableNoNsSchemaLocation->isChecked() != isEnabled) {
        ui->enableNoNsSchemaLocation->setChecked(isEnabled);
    }
    enableButtons();
}

bool SchemaReferencesDialog::isValidReference(NamespaceResult &result)
{
    if(result.schemaLocation.isEmpty() || result.uri.isEmpty()) {
        return false;
    }
    return true ;
}
