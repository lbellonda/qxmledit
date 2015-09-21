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


#include "choosenamespacedialog.h"
#include "ui_choosenamespacedialog.h"
#include "modules/xsd/namespacemanager.h"
#include "modules/namespace/usernamespaceloader.h"
#include "modules/namespace/editnamespacedialog.h"
#include "utils.h"

ChooseNamespaceDialog::ChooseNamespaceDialog(QWidget *parent, NamespaceManager *nsManager, NamespaceResult *base) :
    QDialog(parent),
    ui(new Ui::ChooseNamespaceDialog)
{
    _nsManager = nsManager ;
    ui->setupUi(this);
    loadData(base);
    enableButtons();
    enableUNButtons();
}

ChooseNamespaceDialog::~ChooseNamespaceDialog()
{
    int rows = ui->userNamespaces->rowCount();
    for(int row = 0 ; row < rows ; row ++) {
        UserNamespace *ns = namespaceForRow(row);
        delete ns;
    }
    delete ui;
}

void ChooseNamespaceDialog::enableButtons()
{
    bool isOk = nsIsLegal(ui->prefix->text().trimmed(), ui->uri->text().trimmed());
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isOk);
}

bool ChooseNamespaceDialog::nsIsLegal(const QString &prefix, const QString &uri)
{
    if(!Utils::checkNsPrefix(prefix)) {
        return false;
    }
    if(uri.isEmpty()) {
        return false;
    }
    return true;
}

void ChooseNamespaceDialog::enablePrefix(const bool isEnabled)
{
    ui->prefix->setEnabled(isEnabled);
}

void ChooseNamespaceDialog::nsChosen(NamespaceResult *returnValue)
{
    returnValue->prefix = ui->prefix->text().trimmed();
    returnValue->uri = ui->uri->text().trimmed();
    returnValue->description = ui->nsDescription->text().trimmed();
    returnValue->schemaLocation = ui->schemaLocation->text().trimmed() ;
}

void ChooseNamespaceDialog::loadData(NamespaceResult *base)
{
    if(NULL == _nsManager) {
        Utils::error(this, tr("Parameters error"));
        return;
    }
    loadPredefinedNs();
    loadUserNs();
    if(NULL != base) {
        QString schemaLocation;
        QString description = retrieveUriDescription(base->uri, schemaLocation);
        setValues(base->prefix, base->uri, schemaLocation, description);
    }
}

void ChooseNamespaceDialog::loadUserNs()
{
    _tableUtils.setupTable(ui->userNamespaces);
    UserNamespaceLoader loader;
    bool isOk = false;
    QList<UserNamespace*> userNamespaces = loader.loadUserNamespaces(_nsManager->dataInterface(), isOk);
    if(!isOk) {
        Utils::error(this, tr("Error loading user namespaces"));
    } else {
        // load namespaces into view
        foreach(UserNamespace * ns, userNamespaces) {
            setUserNamespace(-1, ns);
        }
    }
}


void ChooseNamespaceDialog::setUserNamespace(const int row, UserNamespace *ns)
{
    _tableUtils.insNsInList(ui->userNamespaces, row, ns->preferredPrefix(), ns->uri(),
                            ns->schemaLocation(),
                            ns->description(), ns);

}

void ChooseNamespaceDialog::on_cmdIns_clicked()
{
    UserNamespaceLoader loader;
    UserNamespace *ns = loader.createUserNamespace(_nsManager->dataInterface());
    if(NULL == ns) {
        Utils::error(this, tr("Unable to create new namespace."));
        return ;
    }
    if(editNamespace(ns)) {
        if(!persistNamespace(ns)) {
            Utils::error(this, tr("Error saving namespace. Data not saved."));
        }
        setUserNamespace(-1, ns);
    }
}

void ChooseNamespaceDialog::on_cmdMod_clicked()
{
    int row = ui->userNamespaces->currentRow();
    if(row >= 0) {
        UserNamespace *ns = namespaceForRow(row);
        if(NULL != ns) {
            if(editNamespace(ns)) {
                if(!persistNamespace(ns)) {
                    Utils::error(this, tr("Error saving namespace. Data not saved."));
                }
                setUserNamespace(row, ns);
            }
        }
    }
}

void ChooseNamespaceDialog::on_cmdDel_clicked()
{
    int row = ui->userNamespaces->currentRow();
    if(row >= 0) {
        UserNamespace *ns = namespaceForRow(row);
        if(NULL != ns) {
            if(Utils::askYN(this, tr("Do you really want to delete this declaration?"))) {
                UserNamespaceLoader loader;
                if(!loader.deleteUserNamespace(_nsManager->dataInterface(), ns)) {
                    Utils::error(this, tr("Error deleting the definition."));
                    return ;
                }
                UserNamespace *ns = _tableUtils.userDataType<UserNamespace>(ui->userNamespaces, row, 0);
                delete ns;
                ui->userNamespaces->removeRow(row);
            }
        }
    }
}

bool ChooseNamespaceDialog::editNamespace(UserNamespace *ns)
{
    EditNamespaceDialog dlg(this, ns);
    dlg.setModal(true);
    if(dlg.exec() == QDialog::Accepted) {
        return true;
    }
    return false;
}

void ChooseNamespaceDialog::loadPredefinedNs()
{
    // setup widget
    _tableUtils.setupTable(ui->predefNamespaces);
    QList<NamespaceDef*> allNamespaces = _nsManager->allNamespaces();
    foreach(NamespaceDef * nsDef, allNamespaces) {
        _tableUtils.insNsInList(ui->predefNamespaces, -1, nsDef->_defaultPrefix, nsDef->_namespace, nsDef->schemaLocation(), nsDef->_description, nsDef);
    }
}

bool ChooseNamespaceDialog::persistNamespace(UserNamespace *ns)
{
    UserNamespaceLoader loader;
    return loader.saveUserNamespace(_nsManager->dataInterface(), ns);
}

UserNamespace *ChooseNamespaceDialog::namespaceForRow(const int row)
{
    return _tableUtils.userDataType<UserNamespace>(ui->userNamespaces, row, 0);
}

NamespaceDef *ChooseNamespaceDialog::predefNamespaceForRow(const int row)
{
    return _tableUtils.userDataType<NamespaceDef>(ui->predefNamespaces, row, 0);
}

void ChooseNamespaceDialog::on_predefNamespaces_cellClicked(int row, int /*column*/)
{
    NamespaceDef *ns = predefNamespaceForRow(row);
    if(NULL != ns) {
        setValues(ns->defaultPrefix(), ns->uri(), ns->schemaLocation(), ns->description());
    }
}

void ChooseNamespaceDialog::on_predefNamespaces_cellDoubleClicked(int row, int /*column*/)
{
    NamespaceDef *ns = predefNamespaceForRow(row);
    if(NULL != ns) {
        setValues(ns->defaultPrefix(), ns->uri(), ns->schemaLocation(), ns->description());
        accept();
    }
}

void ChooseNamespaceDialog::on_userNamespaces_cellClicked(int row, int /*column*/)
{
    UserNamespace *ns = namespaceForRow(row);
    if(NULL != ns) {
        setValues(ns->preferredPrefix(), ns->uri(), ns->schemaLocation(), ns->description());
    }
    enableButtons();
}

void ChooseNamespaceDialog::on_userNamespaces_cellDoubleClicked(int row, int /*column*/)
{
    UserNamespace *ns = namespaceForRow(row);
    if(NULL != ns) {
        setValues(ns->preferredPrefix(), ns->uri(), ns->schemaLocation(), ns->description());
        accept();
    }
}

void ChooseNamespaceDialog::setValues(const QString &prefix, const QString &uri, const QString &schemaLocation, const QString &description)
{
    ui->uri->setText(uri);
    ui->prefix->setText(prefix);
    ui->nsDescription->setText(description);
    ui->schemaLocation->setText(schemaLocation);
}

void ChooseNamespaceDialog::on_prefix_textChanged(const QString & /*text*/)
{
    enableButtons();
    ui->nsDescription->setText("");
}

void ChooseNamespaceDialog::on_uri_textChanged(const QString & text)
{
    enableButtons();
    QString schemaLoc;
    QString descr = retrieveUriDescription(text, schemaLoc);
    ui->nsDescription->setText(descr);
    if(!descr.isEmpty()) {
        ui->schemaLocation->setText(schemaLoc);
    }
}

QString ChooseNamespaceDialog::retrieveUriDescription(const QString & text, QString &schemaLocation)
{
    NamespaceDef* nsDef = _nsManager->namespacesForUri(text.trimmed());
    if(NULL != nsDef) {
        schemaLocation = nsDef->schemaLocation();
        return nsDef->description();
    } else {
        // Look for user ns
        int rows = ui->userNamespaces->rowCount();
        for(int i = 0 ; i < rows ; i++) {
            UserNamespace * uns = namespaceForRow(i);
            if(uns->uri() == text) {
                schemaLocation = uns->schemaLocation();
                return uns->description();
            }
        }
        return "" ;
    }
}

void ChooseNamespaceDialog::on_userNamespaces_itemSelectionChanged()
{
    enableUNButtons();
}

void ChooseNamespaceDialog::enableUNButtons()
{
    bool enable = false;
    if(ui->userNamespaces->currentRow() >= 0) {
        enable = true;
    }
    ui->cmdMod->setEnabled(enable);
    ui->cmdDel->setEnabled(enable);
}

