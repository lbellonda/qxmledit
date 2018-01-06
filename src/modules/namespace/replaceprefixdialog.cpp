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

#include "replaceprefixdialog.h"
#include "ui_replaceprefixdialog.h"
#include "utils.h"

//----------------------------------------

bool ReplacePrefixDialog::getReplacePrefixInfo(QWidget *window, ReplacePrefixInfo *info)
{
    ReplacePrefixDialog replacePrefixDialog(info, window);
    if(replacePrefixDialog.exec() == QDialog::Accepted)  {
        return true;
    }
    return false;
}

//----------------------------------------

ReplacePrefixDialog::ReplacePrefixDialog(ReplacePrefixInfo *info, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReplacePrefixDialog)
{
    _info = info ;
    _selWidget = NULL ;
    ui->setupUi(this);
    finishSetup();
}

ReplacePrefixDialog::~ReplacePrefixDialog()
{
    delete ui;
}

void ReplacePrefixDialog::finishSetup()
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
    Utils::updateEditableCombo(ui->replacePrefix, _info->prefixInfo.allPrefixes.toList());
    if(!_info->isAllPrefixes) {
        ui->onePrefix->setChecked(true);
    } else {
        ui->allPrefixes->setChecked(true);
    }
    enableChoices();
}


void ReplacePrefixDialog::updateSelection()
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


void ReplacePrefixDialog::onSelectionTypeChanged(TargetSelection::Type selectionType)
{
    if(NULL != _info) {
        _info->targetSelection = selectionType ;
        updateSelection();
    }
}

void ReplacePrefixDialog::on_allPrefixes_clicked()
{
    enableChoices();
}

void ReplacePrefixDialog::on_onePrefix_clicked()
{
    enableChoices();
}

void ReplacePrefixDialog::enableChoices()
{
    _info->isAllPrefixes = !ui->onePrefix->isChecked();
    ui->prefixes->setEnabled(!_info->isAllPrefixes);
}

void ReplacePrefixDialog::on_prefixes_currentTextChanged(const QString &)
{
    _info->toReplacePrefix = ui->prefixes->currentText().trimmed();
    enableOk();
}

void ReplacePrefixDialog::on_prefixes_editTextChanged(const QString &)
{
    _info->toReplacePrefix = ui->prefixes->currentText().trimmed();
    enableOk();
}

void ReplacePrefixDialog::on_replacePrefix_editTextChanged(const QString &)
{
    _info->setPrefix = ui->replacePrefix->currentText().trimmed();
    enableOk();
}

bool ReplacePrefixDialog::checkOK()
{
    if(!_info->isAllPrefixes) {
        if(!Utils::checkNsPrefix(_info->toReplacePrefix)) {
            return false;
        }
    }
    if(!Utils::checkNsPrefix(_info->setPrefix)) {
        return false;
    }
    if(_info->setPrefix == _info->toReplacePrefix) {
        return false;
    }

    return true ;
}

void ReplacePrefixDialog::enableOk()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(checkOK());
}

void ReplacePrefixDialog::accept()
{
    if(!checkOK()) {
        Utils::error(this, tr("Invalid parameters."));
        return ;
    }
    QDialog::accept();
}

