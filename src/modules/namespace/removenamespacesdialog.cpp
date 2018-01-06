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


#include "removenamespacesdialog.h"
#include "ui_removenamespacesdialog.h"
#include "utils.h"

//----------------------------------------

bool RemoveNamespacesDialog::getRemoveNamespacesInfo(QWidget *window, RemoveNamespacesInfo *info)
{
    RemoveNamespacesDialog removeNamespacesDialog(info, window);
    if(removeNamespacesDialog.exec() == QDialog::Accepted)  {
        return true;
    }
    return false;
}

//----------------------------------------

RemoveNamespacesDialog::RemoveNamespacesDialog(RemoveNamespacesInfo *info, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemoveNamespacesDialog)
{
    _info = info ;
    _selWidget = NULL ;
    ui->setupUi(this);
    finishSetup();
}

RemoveNamespacesDialog::~RemoveNamespacesDialog()
{
    delete ui;
}

void RemoveNamespacesDialog::finishSetup()
{
    _selWidget = new ElementSelectionTypeWidget();
    Utils::replaceWidget(ui->layoutOfPlaceholder, ui->placeholder, _selWidget, true, Qt::AlignTop | Qt::AlignLeft, true);
    connect(_selWidget, SIGNAL(selectionTypeChanged(TargetSelection::Type)), this, SLOT(onSelectionTypeChanged(TargetSelection::Type)));
    if(NULL == _info->element) {
        _selWidget->setSelectionType(TargetSelection::AllItems);
    } else {
        _selWidget->setSelectionType(TargetSelection::SelectedItemAndChildren);
    }
    updateSelection();
    if(!_info->isAllNamespaces) {
        ui->oneNamespace->setChecked(true);
    } else {
        ui->allNamespaces->setChecked(true);
    }
    ui->removeDeclarations->setChecked(_info->isRemoveDeclarations);
    enableChoices();
}

void RemoveNamespacesDialog::updateSelection()
{
    switch(_info->targetSelection) {
    default:
        ui->namespaces->clear();
        break;
    case TargetSelection::AllItems:
        Utils::updateNonEditableCombo(ui->namespaces, _info->namespacesInfo.allNamespaces.toList());
        break;
    case TargetSelection::SelectedItem:
        Utils::updateNonEditableCombo(ui->namespaces, _info->namespacesInfo.selectionNamespaces.toList());
        break;
    case TargetSelection::SelectedItemAndChildren:
        Utils::updateNonEditableCombo(ui->namespaces, _info->namespacesInfo.selectionNamespacesRecursive.toList());
        break;
    case TargetSelection::Bookmarks:
        Utils::updateNonEditableCombo(ui->namespaces, _info->namespacesInfo.bookmarksNamespaces.toList());
        break;
    case TargetSelection::BookmarksAndChildren:
        Utils::updateNonEditableCombo(ui->namespaces, _info->namespacesInfo.bookmarksNamespacesRecursive.toList());
        break;
    }
}


void RemoveNamespacesDialog::onSelectionTypeChanged(TargetSelection::Type selectionType)
{
    if(NULL != _info) {
        _info->targetSelection = selectionType ;
        updateSelection();
    }
}

void RemoveNamespacesDialog::on_allNamespaces_clicked()
{
    enableChoices();
}

void RemoveNamespacesDialog::on_oneNamespace_clicked()
{
    enableChoices();
}

void RemoveNamespacesDialog::enableChoices()
{
    _info->isAllNamespaces = !ui->oneNamespace->isChecked();
    ui->namespaces->setEnabled(!_info->isAllNamespaces);
    if(!_info->isAllNamespaces) {
        updatePrefixes();
    }
}

void RemoveNamespacesDialog::on_namespaces_currentIndexChanged(const QString &)
{
    _info->removedNamespace = ui->namespaces->currentText().trimmed();
    updatePrefixes();
    enableOk();
}

void RemoveNamespacesDialog::updatePrefixes()
{
    QString prefixes;
    if(_info->namespacesInfo.prefixesForNamespaces.contains(_info->removedNamespace)) {
        QSet<QString> prefixesL = _info->namespacesInfo.prefixesForNamespaces[_info->removedNamespace];
        QList<QString> prefixesList = prefixesL.toList();
        bool first = true ;
        foreach(const QString & str, prefixesList) {
            if(!first) {
                prefixes += ", ";
            }
            prefixes += str;
            first = false ;
        }
    }
    ui->prefixexOfNamespace->setText(prefixes);
}

void RemoveNamespacesDialog::enableOk()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!ui->namespaces->currentText().trimmed().isEmpty());
}

void RemoveNamespacesDialog::accept()
{
    if(!_info->isAllNamespaces) {
        if(_info->removedNamespace.isEmpty()) {
            Utils::error(this, tr("Invalid parameters."));
            return ;
        }
    }
    _info->isRemoveDeclarations = ui->removeDeclarations->isChecked();
    QDialog::accept();
}
