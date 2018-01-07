/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016-2018 by Luca Bellonda and individual contributors  *
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


#include "normalizenamespacedialog.h"
#include "ui_normalizenamespacedialog.h"
#include "modules/namespace/choosenamespacedialog.h"
#include "utils.h"

//----------------------------------------

bool NormalizeNamespaceDialog::getNormalizeNamespaceInfo(QWidget *window, NormalizeNamespaceInfo *info, NamespaceManager *namespaceManager)
{
    NormalizeNamespaceDialog normalizeNamespaceDialog(info, window);
    normalizeNamespaceDialog.setNamespaceManager(namespaceManager);
    if(normalizeNamespaceDialog.exec() == QDialog::Accepted)  {
        return true;
    }
    return false;
}

//----------------------------------------

NormalizeNamespaceDialog::NormalizeNamespaceDialog(NormalizeNamespaceInfo *info, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NormalizeNamespaceDialog)
{
    _info = info ;
    _selWidget = NULL ;
    _namespaceManager = NULL ;
    ui->setupUi(this);
    finishSetup();
}

NormalizeNamespaceDialog::~NormalizeNamespaceDialog()
{
    delete ui;
}

void NormalizeNamespaceDialog::finishSetup()
{
    ui->declareOnRoot->setChecked(_info->declareOnRoot);
    _selWidget = new ElementSelectionTypeWidget();
    Utils::replaceWidget(ui->layoutOfPlaceholder, ui->placeholder, _selWidget, true, Qt::AlignTop | Qt::AlignLeft, true);
    connect(_selWidget, SIGNAL(selectionTypeChanged(TargetSelection::Type)), this, SLOT(onSelectionTypeChanged(TargetSelection::Type)));
    if(NULL == _info->element) {
        _selWidget->setSelectionType(TargetSelection::AllItems);
    } else {
        _selWidget->setSelectionType(TargetSelection::SelectedItemAndChildren);
    }
    mergePrefixesInfo();
    mergeNamespacesInfo();
}

void NormalizeNamespaceDialog::mergePrefixesInfo()
{
    QSet<QString> set;
    foreach(const QSet<QString> &values, _info->namespacesInfo.prefixesForNamespaces.values()) {
        set.unite(values);
    }
    set.unite(_prefixesCited);
    Utils::updateEditableCombo(ui->prefix, set.toList());
}

void NormalizeNamespaceDialog::mergeNamespacesInfo()
{
    QSet<QString> set;
    set.unite(_info->namespacesInfo.allNamespaces);
    set.unite(_prefixesCited);
    Utils::updateEditableCombo(ui->namespaces, set.toList());
}

void NormalizeNamespaceDialog::on_select_clicked()
{
    if(NULL != _namespaceManager) {
        ChooseNamespaceDialog dlg(this, _namespaceManager, NULL);
        dlg.setModal(true);
        if(dlg.exec() == QDialog::Accepted) {
            NamespaceResult result;
            dlg.nsChosen(&result);
            _namespacesCited.insert(result.uri);
            _prefixesCited.insert(result.prefix);
            ui->namespaces->setEditText(result.uri);
            ui->prefix->setEditText(result.prefix);
        }
    }
    mergePrefixesInfo();
    mergeNamespacesInfo();
}

void NormalizeNamespaceDialog::onSelectionTypeChanged(TargetSelection::Type selectionType)
{
    _info->targetSelection = selectionType ;
}

void NormalizeNamespaceDialog::on_prefix_currentTextChanged(const QString &)
{
    _info->setPrefix = ui->prefix->currentText().trimmed();
    enableOk();
}

void NormalizeNamespaceDialog::on_prefix_editTextChanged(const QString &)
{
    _info->setPrefix = ui->prefix->currentText().trimmed();
    enableOk();
}

void NormalizeNamespaceDialog::on_namespaces_currentTextChanged(const QString &)
{
    _info->nsURI = ui->namespaces->currentText().trimmed();
    enableOk();
}

void NormalizeNamespaceDialog::on_namespaces_editTextChanged(const QString &)
{
    _info->nsURI = ui->namespaces->currentText().trimmed();
    enableOk();
}

bool NormalizeNamespaceDialog::checkOK()
{
    if(!Utils::checkNsPrefix(_info->setPrefix)) {
        return false;
    }
    if(_info->nsURI.isEmpty()) {
        return false;
    }
    return true;
}

void NormalizeNamespaceDialog::enableOk()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(checkOK());
}

void NormalizeNamespaceDialog::accept()
{
    _info->declareOnRoot = ui->declareOnRoot->isChecked();
    if(!checkOK()) {
        Utils::error(this, tr("Invalid parameters."));
        return ;
    }
    if(_info->namespacesInfo.isUsedPrefixForOtherNamespace(_info->nsURI, _info->setPrefix)) {
        if(!Utils::askYNRemapNamespaces(this)) {
            return ;
        }
        _info->remapPrefixes = true ;
    }
    QDialog::accept();
}

NamespaceManager *NormalizeNamespaceDialog::namespaceManager() const
{
    return _namespaceManager;
}

void NormalizeNamespaceDialog::setNamespaceManager(NamespaceManager *namespaceManager)
{
    _namespaceManager = namespaceManager;
}

