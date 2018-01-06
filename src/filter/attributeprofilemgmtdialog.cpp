/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2018 by Luca Bellonda and individual contributors  *
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


#include "attributeprofilemgmtdialog.h"
#include "ui_attributeprofilemgmtdialog.h"
#include "attributefilterdetaildialog.h"
#include "utils.h"

AttributeProfileMgmtDialog::AttributeProfileMgmtDialog(UIDelegate *newUiDelegate, AttributeFilterManagement *newManagement, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AttributeProfileMgmtDialog)
{
    _uiDelegate = newUiDelegate ;
    _mgmt = newManagement;
    _selectedProfileCode = -1 ;
    _selectedProfile = NULL ;
    ui->setupUi(this);
    loadData();
    enableUI();
}

AttributeProfileMgmtDialog::~AttributeProfileMgmtDialog()
{
    delete ui;
    deleteProfiles();
}


int AttributeProfileMgmtDialog::selectedProfileCode()
{
    return _selectedProfileCode ;
}

void AttributeProfileMgmtDialog::deleteProfiles()
{
    foreach(AttrFilterProfile * profile, _profiles) {
        delete profile;
    }
    _profiles.clear();
}

void AttributeProfileMgmtDialog::loadData()
{
    DataResult op;
    QList<AttrFilterProfile*> newProfiles = _mgmt->readProfiles(op);
    if(!op.isOk()) {
        fatalError(tr("Error retrieving profile data."));
    }
    deleteProfiles();
    ui->profilesList->clear();
    _profiles = newProfiles ;
    foreach(AttrFilterProfile * profile, _profiles) {
        QListWidgetItem *newItem = new QListWidgetItem(QString(tr("%1 - %2")).arg(profile->name()).arg(profile->description()));
        newItem->setData(Qt::UserRole, qVariantFromValue((void*)profile));
        ui->profilesList->addItem(newItem);
    }
}

void AttributeProfileMgmtDialog::fatalError(const QString &message)
{
    _uiDelegate->error(this, message);
    reject();
}

void AttributeProfileMgmtDialog::on_profilesList_currentItemChanged()
{
    enableUI();
}

void AttributeProfileMgmtDialog::enableUI()
{
    QListWidgetItem *currentItem = ui->profilesList->currentItem();
    bool isEnabled = false;
    if(NULL != currentItem) {
        isEnabled = true ;
    }
    ui->editButton->setEnabled(isEnabled);
    ui->deleteButton->setEnabled(isEnabled);
    ui->activateButton->setEnabled(isEnabled);
}

void AttributeProfileMgmtDialog::on_activateButton_clicked()
{
    QListWidgetItem *newItem = ui->profilesList->currentItem();
    if(NULL != newItem) {
        QVariant data = newItem->data(Qt::UserRole);
        AttrFilterProfile* profile = (AttrFilterProfile*)data.value<void*>();
        _selectedProfileCode = profile->id();
        _selectedProfile = profile ;
        accept();
    }
}

void AttributeProfileMgmtDialog::on_editButton_clicked()
{
    QListWidgetItem *item = ui->profilesList->currentItem();
    if(NULL != item) {
        QVariant data = item->data(Qt::UserRole);
        AttrFilterProfile* profile = (AttrFilterProfile*)data.value<void*>();
        DataResult op;
        AttrFilterDetail *detail = _mgmt->readDetail(op, profile->id());
        if(!op.isOk()) {
            _uiDelegate->error(this, tr("Error executing the command."));
        } else {
            showDetails(profile, detail);
        }
        delete detail ;
        loadData();
        enableUI();
    } else {
        _uiDelegate->errorNoSel(this);
    }
}

void AttributeProfileMgmtDialog::on_newButton_clicked()
{
    AttrFilterProfile profile ;
    AttrFilterDetail detail ;
    profile.setName(tr("New Profile"));
    profile.setDescription(tr("New Profile"));
    showDetails(&profile, &detail);
    loadData();
    enableUI();
}

void AttributeProfileMgmtDialog::on_profilesList_itemDoubleClicked(QListWidgetItem * /*item*/)
{
    on_editButton_clicked();
}

void AttributeProfileMgmtDialog::showDetails(AttrFilterProfile *profile, AttrFilterDetail *detail)
{
    AttributeFilterDetailDialog *dlg = getDetailDialog(profile, detail);
    if(NULL != dlg) {
        dlg->go();
        delete dlg;
    } else {
        _uiDelegate->errorOutOfMem(this);
    }
}

AttributeFilterDetailDialog *AttributeProfileMgmtDialog::getDetailDialog(AttrFilterProfile *profile, AttrFilterDetail *detail)
{
    AttributeFilterDetailDialog *dlg = new AttributeFilterDetailDialog(this, _uiDelegate, _mgmt, profile, detail);
    return dlg;
}


void AttributeProfileMgmtDialog::on_deleteButton_clicked()
{
    QListWidgetItem *item = ui->profilesList->currentItem();
    if(NULL != item) {
        if(_uiDelegate->askYN(this, tr("Really delete the item?"))) {
            AttrFilterProfile* profile = profileFromItem(item);
            DataResult op;
            _mgmt->deleteProfile(op, profile->id());
            if(!op.isOk()) {
                _uiDelegate->error(this, tr("Error executing the command."));
            }
            loadData();
            enableUI();
        }
    } else {
        _uiDelegate->errorNoSel(this);
    }
}

AttrFilterProfile* AttributeProfileMgmtDialog::profileFromItem(QListWidgetItem *item)
{
    AttrFilterProfile* profile = NULL ;
    if(NULL != item) {
        QVariant data = item->data(Qt::UserRole);
        profile = (AttrFilterProfile*)data.value<void*>();
    }
    return profile;
}

AttributeFilter *AttributeProfileMgmtDialog::buildFilter()
{
    if(_selectedProfile == NULL) {
        return NULL ;
    }
    DataResult op;
    AttrFilterDetail *detail = _mgmt->readDetail(op, _selectedProfileCode);
    AttributeFilter *newFilter = NULL ;

    if(!op.isOk()) {
        fatalError(tr("Error retrieving profile data."));
    } else {
        newFilter = new AttributeFilter();
        if(NULL != newFilter) {
            foreach(QString name, detail->names()) {
                newFilter->addName(name);
            }
            newFilter->setIsWhiteList(_selectedProfile->isWhiteList());
            newFilter->setTemporary(false);
        }
    }
    if(NULL != detail) {
        delete detail;
    }
    return newFilter;
}

