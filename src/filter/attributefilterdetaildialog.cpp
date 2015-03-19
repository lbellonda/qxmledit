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


#include "attributefilterdetaildialog.h"
#include "ui_attributefilterdetaildialog.h"

AttributeFilterDetailDialog::AttributeFilterDetailDialog(QWidget *parent, UIDelegate *newUiDelegate, AttributeFilterManagement *mgmt, AttrFilterProfile *profile, AttrFilterDetail *detail) :
    QDialog(parent),
    ui(new Ui::AttributeFilterDetailDialog)
{
    _mgmt = mgmt ;
    _profile = profile;
    _detail  = detail;
    _uiDelegate = newUiDelegate ;
    ui->setupUi(this);
    loadData();
    enableUI();
}

AttributeFilterDetailDialog::~AttributeFilterDetailDialog()
{
    delete ui;
}

int AttributeFilterDetailDialog::go()
{
    return exec();
}

void AttributeFilterDetailDialog::enableUI()
{
    bool isEnabledOK = true ;
    if(ui->txtName->text().trimmed().length() == 0) {
        isEnabledOK = false;
    }
    if(ui->namesList->count() == 0) {
        isEnabledOK = false;
    }

    bool oneEnabled = false;
    if(ui->namesList->currentItem() != NULL) {
        oneEnabled = true ;
    }
    ui->editCmd->setEnabled(oneEnabled);
    ui->deleteCmd->setEnabled(oneEnabled);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isEnabledOK);
}

void AttributeFilterDetailDialog::loadData()
{
    ui->txtName->setText(_profile->name());
    ui->txtDescription->setText(_profile->description());
    if(_profile->isWhiteList()) {
        ui->radioShow->setChecked(true);
    } else {
        ui->radioHide->setChecked(true);
    }
    QMap<QString, QString> sortedMap;
    foreach(QString name, _detail->names()) {
        sortedMap.insert(name, name);
    }
    foreach(QString name, sortedMap) {
        QListWidgetItem *item = new QListWidgetItem(name);
        if(NULL == item) {
            fatalError(_uiDelegate->msgOutOfMem());
        }
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        ui->namesList->addItem(item);
    }
}

void AttributeFilterDetailDialog::fatalError(const QString &message)
{
    _uiDelegate->error(this, message);
    reject();
}

void AttributeFilterDetailDialog::accept()
{
    AttrFilterProfile profile ;
    AttrFilterDetail detail ;

    profile.setId(_profile->id());
    // validation
    if(!validate(&profile, &detail)) {
        return ;
    }
    if(!save(&profile, &detail)) {
        _uiDelegate->error(this, tr("Error saving the profile"));
        return ;
    }
    QDialog::accept();
}

bool AttributeFilterDetailDialog::validate(AttrFilterProfile *profile, AttrFilterDetail *detail)
{
    QString name = ui->txtName->text().trimmed();
    if(name.length() == 0) {
        _uiDelegate->error(this, tr("Insert a valid name."));
        return false;
    }
    profile->setName(name);
    profile->setDescription(ui->txtDescription->text().trimmed());
    profile->setWhiteList(ui->radioShow->isChecked());

    int itemsCount = ui->namesList->count();
    for(int row = 0 ; row < itemsCount ; row ++) {
        bool isValid = false;
        QListWidgetItem *item = ui->namesList->item(row);
        if(NULL != item) {
            QString value = item->text().trimmed();
            if(value.length() > 0) {
                isValid = true ;
                detail->addName(value);
            }
        }
        if(!isValid) {
            _uiDelegate->error(this, tr("The item at position %1 is not valid.").arg(row + 1));
            return false ;
        }
    }
    return true;
}

bool AttributeFilterDetailDialog::save(AttrFilterProfile *profile, AttrFilterDetail *detail)
{
    DataResult op;
    _mgmt->saveProfile(op, profile, detail);
    return op.isOk();
}

void AttributeFilterDetailDialog::on_newCmd_clicked()
{
    QListWidgetItem *item = new QListWidgetItem("");
    if(NULL == item) {
        fatalError(_uiDelegate->msgOutOfMem());
    }
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    ui->namesList->addItem(item);
}

void AttributeFilterDetailDialog::on_deleteCmd_clicked()
{
    QListWidgetItem *item = ui->namesList->currentItem() ;
    if(item != NULL) {
        if(_uiDelegate->askYN(tr("Do you really want to delete the item?"))) {
            QListWidgetItem *dItem = ui->namesList->takeItem(ui->namesList->row(item));
            if(NULL != dItem) {
                delete dItem;
            }
        }
    } else {
        _uiDelegate->errorNoSel(this);
    }
}

void AttributeFilterDetailDialog::on_editCmd_clicked()
{
    QListWidgetItem *item = ui->namesList->currentItem() ;
    if(item != NULL) {
        ui->namesList->editItem(item);
    } else {
        _uiDelegate->errorNoSel(this);
    }
}


void AttributeFilterDetailDialog::on_txtName_textChanged(const QString & /*text*/)
{
    enableUI();
}

void AttributeFilterDetailDialog::on_namesList_itemSelectionChanged()
{
    enableUI();
}
