/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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


#include "editnamespacedialog.h"
#include "ui_editnamespacedialog.h"
#include "modules/namespace/usernamespace.h"
#include "utils.h"

EditNamespaceDialog::EditNamespaceDialog(QWidget *parent, UserNamespace *ns) :
    QDialog(parent),
    ui(new Ui::EditNamespaceDialog)
{
    _ns = ns ;
    ui->setupUi(this);
    loadData();
    enableButtons();
}

EditNamespaceDialog::~EditNamespaceDialog()
{
    delete ui;
}

bool EditNamespaceDialog::canSave()
{
    bool enableOk = true ;
    if(ui->uri->text().trimmed().isEmpty()
            ||  ui->name->text().trimmed().isEmpty()) {
        enableOk = false;
    }
    if(!Utils::checkNsPrefix(ui->defaultPrefix->text().trimmed())) {
        enableOk = false;
    }
    if(ui->schemaLocation->text().trimmed().isEmpty()) {
        enableOk = false;
    }
    return enableOk;
}

void EditNamespaceDialog::enableButtons()
{
    bool enableOk = canSave() ;
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(enableOk);

    bool enableDelCmd = false ;
    if(ui->prefixes->currentRow() >= 0) {
        enableDelCmd = true ;
    }
    ui->cmdDelP->setEnabled(enableDelCmd);
    ui->cmdModP->setEnabled(enableDelCmd);
}


void EditNamespaceDialog::loadData()
{
    ui->name->setText(_ns->name());
    ui->description->setText(_ns->description());
    QStringList tags = QStringList::fromSet(_ns->data()->tags());
    ui->tags->setText(tags.join(","));
    ui->defaultPrefix->setText(_ns->preferredPrefix());
    ui->uri->setText(_ns->uri());
    ui->schemaLocation->setText(_ns->schemaLocation());
    ui->creationDate->setText(_ns->data()->creationDate().toString());
    ui->updateDate->setText(_ns->data()->updateDate().toString());
    foreach(QString prefix, _ns->prefixes()) {
        addItem(prefix);
    }
}

bool EditNamespaceDialog::getDataFromUI()
{
    // save data into snippet
    QSet<QString> newTags = tagsFromUI();

    //check data consistency
    if(!canSave()) {
        Utils::error(this, tr("Mandatory fields empty or invalid."));
        return false;
    }
    int count = ui->prefixes->count();
    QStringList prf;
    for(int i = 0 ; i < count ; i++) {
        QString str = ui->prefixes->item(i)->text();
        prf.append(str);
        if(!Utils::checkNsPrefix(str)) {
            Utils::error(this, tr("Invalid prefix:'%1'.").arg(str));
            return false;
        }
    }

    _ns->setName(ui->name->text().trimmed());
    _ns->setDescription(ui->description->text().trimmed());

    _ns->data()->setTags(newTags);

    QDateTime now = QDateTime::currentDateTime() ;
    _ns->data()->setUpdateDate(now);
    _ns->setUri(ui->uri->text().trimmed());
    _ns->setPreferredPrefix(ui->defaultPrefix->text().trimmed());
    _ns->setPrefixes(prf);
    _ns->setSchemaLocation(ui->schemaLocation->text().trimmed());
    return true ;
}

void EditNamespaceDialog::accept()
{
    if(!getDataFromUI()) {
        Utils::error(this, tr("Invalid data"));
        return ;
    }
    QDialog::accept();
}

QSet<QString> EditNamespaceDialog::tagsFromUI()
{
    QStringList tags = ui->tags->text().trimmed().split(',');
    QSet<QString> newTags;
    foreach(QString tg, tags) {
        QString tx = tg.trimmed();
        if(!tx.isEmpty()) {
            if(!newTags.contains(tx))
                newTags.insert(tx);
        }
    }
    return newTags;
}

void EditNamespaceDialog::on_cmdAddP_clicked()
{
    addItem(tr("prefix"));
    ui->prefixes->editItem(ui->prefixes->item((ui->prefixes->count() - 1)));
}

void EditNamespaceDialog::addItem(const QString &text)
{
    QListWidgetItem *item = new QListWidgetItem(text);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    ui->prefixes->addItem(item);
}

void EditNamespaceDialog::on_cmdDelP_clicked()
{
    int row = ui->prefixes->currentRow();
    if(row >= 0) {
        ui->prefixes->takeItem(row);
    }
}

void EditNamespaceDialog::on_cmdModP_clicked()
{
    int row = ui->prefixes->currentRow();
    if(row >= 0) {
        ui->prefixes->editItem(ui->prefixes->item(row));
    }
}

void EditNamespaceDialog::on_prefixes_currentRowChanged(int /*currentRow*/)
{
    enableButtons();
}

void EditNamespaceDialog::on_name_textEdited(const QString & /*text*/)
{
    enableButtons();
}

void EditNamespaceDialog::on_description_textEdited(const QString & /*text*/)
{
    enableButtons();
}

void EditNamespaceDialog::on_defaultPrefix_textEdited(const QString & /*text*/)
{
    enableButtons();
}

void EditNamespaceDialog::on_uri_textEdited(const QString & /*text*/)
{
    enableButtons();
}

void EditNamespaceDialog::on_schemaLocation_textEdited(const QString & /*text*/)
{
    enableButtons();
}
