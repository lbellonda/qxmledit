/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016 by Luca Bellonda and individual contributors       *
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


#include "replacenamespacedialog.h"
#include "ui_replacenamespacedialog.h"
#include "modules/namespace/choosenamespacedialog.h"
#include "utils.h"

//----------------------------------------

bool ReplaceNamespaceDialog::getReplaceNamespaceInfo(QWidget *window, ReplaceNamespaceInfo *info, NamespaceManager *namespaceManager)
{
    ReplaceNamespaceDialog replaceNamespaceDialog(info, window);
    replaceNamespaceDialog.setNamespaceManager(namespaceManager);
    if(replaceNamespaceDialog.exec() == QDialog::Accepted)  {
        return true;
    }
    return false;
}

//----------------------------------------

ReplaceNamespaceDialog::ReplaceNamespaceDialog(ReplaceNamespaceInfo *info, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReplaceNamespaceDialog)
{
    _info = info ;
    _selWidget = NULL ;
    _namespaceManager = NULL ;
    _remapPrefixes = false;
    ui->setupUi(this);
    finishSetup();
}

ReplaceNamespaceDialog::~ReplaceNamespaceDialog()
{
    delete ui;
}

void ReplaceNamespaceDialog::finishSetup()
{
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

void ReplaceNamespaceDialog::mergePrefixesInfo()
{
    QSet<QString> set;
    foreach(const QSet<QString> &values, _info->namespacesInfo.prefixesForNamespaces.values()) {
        set.unite(values);
    }
    set.unite(_prefixesCited);
    Utils::updateEditableCombo(ui->prefix, set.toList());
}

void ReplaceNamespaceDialog::mergeNamespacesInfo()
{
    QSet<QString> set;
    set.unite(_info->namespacesInfo.allNamespaces);
    set.unite(_prefixesCited);
    Utils::updateEditableCombo(ui->namespaces, set.toList());
    Utils::updateNonEditableCombo(ui->namespaceToReplace, set.toList());
}

void ReplaceNamespaceDialog::on_select_clicked()
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

void ReplaceNamespaceDialog::onSelectionTypeChanged(TargetSelection::Type selectionType)
{
    _info->targetSelection = selectionType ;
}

void ReplaceNamespaceDialog::on_prefix_currentTextChanged(const QString &)
{
    _info->setPrefix = ui->prefix->currentText().trimmed();
    enableOk();
}

void ReplaceNamespaceDialog::on_prefix_editTextChanged(const QString &)
{
    _info->setPrefix = ui->prefix->currentText().trimmed();
    enableOk();
}

void ReplaceNamespaceDialog::on_namespaces_currentTextChanged(const QString &)
{
    _info->nsURI = ui->namespaces->currentText().trimmed();
    enableOk();
}

void ReplaceNamespaceDialog::on_namespaces_editTextChanged(const QString &)
{
    _info->nsURI = ui->namespaces->currentText().trimmed();
    enableOk();
}

void ReplaceNamespaceDialog::on_namespaceToReplace_currentIndexChanged(int)
{
    int index = ui->namespaceToReplace->currentIndex();
    if(index >= 0) {
        _info->toReplaceNSUri = ui->namespaceToReplace->itemText(index);
    } else {
        _info->toReplaceNSUri = "" ;
    }
    enableOk();
}

bool ReplaceNamespaceDialog::checkOK()
{
    if(!Utils::checkNsPrefix(_info->setPrefix)) {
        return false;
    }
    if(_info->nsURI.isEmpty()) {
        return false;
    }
    if(_info->toReplaceNSUri.isEmpty()) {
        return false;
    }
    return true;
}

void ReplaceNamespaceDialog::enableOk()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(checkOK());
}

void ReplaceNamespaceDialog::accept()
{
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

NamespaceManager *ReplaceNamespaceDialog::namespaceManager() const
{
    return _namespaceManager;
}

void ReplaceNamespaceDialog::setNamespaceManager(NamespaceManager *namespaceManager)
{
    _namespaceManager = namespaceManager;
}

