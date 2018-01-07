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

#include "removeprefixdialog.h"
#include "ui_removeprefixdialog.h"
#include "utils.h"

//----------------------------------------

bool RemovePrefixDialog::getRemovePrefixInfo(QWidget *window, RemovePrefixInfo *info)
{
    RemovePrefixDialog removePrefixDialog(info, window);
    if(removePrefixDialog.exec() == QDialog::Accepted)  {
        return true;
    }
    return false;
}

//----------------------------------------


RemovePrefixDialog::RemovePrefixDialog(RemovePrefixInfo *info, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemovePrefixDialog)
{
    _info = info ;
    _selWidget = NULL ;
    ui->setupUi(this);
    finishSetup();
}

RemovePrefixDialog::~RemovePrefixDialog()
{
    delete ui;
}

void RemovePrefixDialog::finishSetup()
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
    if(!_info->isAllPrefixes) {
        ui->onePrefix->setChecked(true);
    } else {
        ui->allPrefixes->setChecked(true);
    }
    enableChoices();
}

void RemovePrefixDialog::updateSelection()
{
    switch(_info->targetSelection) {
    default:
        ui->prefixes->clear();
        break;
    case TargetSelection::AllItems:
        Utils::updateEditableCombo(ui->prefixes, _info->prefixInfo.allPrefixes.toList());
        break;
    case TargetSelection::SelectedItem:
        Utils::updateEditableCombo(ui->prefixes, _info->prefixInfo.selectionPrefixes.toList());
        break;
    case TargetSelection::SelectedItemAndChildren:
        Utils::updateEditableCombo(ui->prefixes, _info->prefixInfo.selectionPrefixesRecursive.toList());
        break;
    case TargetSelection::Bookmarks:
        Utils::updateEditableCombo(ui->prefixes, _info->prefixInfo.bookmarksPrefixes.toList());
        break;
    case TargetSelection::BookmarksAndChildren:
        Utils::updateEditableCombo(ui->prefixes, _info->prefixInfo.bookmarksPrefixesRecursive.toList());
        break;
    }
}


void RemovePrefixDialog::onSelectionTypeChanged(TargetSelection::Type selectionType)
{
    if(NULL != _info) {
        _info->targetSelection = selectionType ;
        updateSelection();
    }
}

void RemovePrefixDialog::on_allPrefixes_clicked()
{
    enableChoices();
}

void RemovePrefixDialog::on_onePrefix_clicked()
{
    enableChoices();
}

void RemovePrefixDialog::enableChoices()
{
    _info->isAllPrefixes = !ui->onePrefix->isChecked();
    ui->prefixes->setEnabled(!_info->isAllPrefixes);
}

void RemovePrefixDialog::on_prefixes_currentTextChanged(const QString &)
{
    _info->removedPrefix = ui->prefixes->currentText().trimmed();
    enableOk();
}

void RemovePrefixDialog::on_prefixes_editTextChanged(const QString &)
{
    _info->removedPrefix = ui->prefixes->currentText().trimmed();
    enableOk();
}

bool RemovePrefixDialog::checkOK()
{
    if(!_info->isAllPrefixes) {
        return Utils::checkNsPrefix(_info->removedPrefix, false);
    }
    return true ;
}

void RemovePrefixDialog::enableOk()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(checkOK());
}

void RemovePrefixDialog::accept()
{
    if(!_info->isAllPrefixes) {
        if(!checkOK()) {
            Utils::error(this, tr("Invalid parameters."));
            return ;
        }
    }
    QDialog::accept();
}
