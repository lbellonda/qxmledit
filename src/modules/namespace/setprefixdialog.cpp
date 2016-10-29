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


#include "setprefixdialog.h"
#include "ui_setprefixdialog.h"
#include "utils.h"

//----------------------------------------

bool SetPrefixDialog::getSetPrefixInfo(QWidget *window, SetPrefixInfo *info)
{
    SetPrefixDialog setPrefixDialog(info, window);
    if(setPrefixDialog.exec() == QDialog::Accepted)  {
        return true;
    }
    return false;
}

//----------------------------------------

SetPrefixDialog::SetPrefixDialog(SetPrefixInfo *info, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetPrefixDialog)
{
    _info = info ;
    _selWidget = NULL ;
    ui->setupUi(this);
    finishSetup();
}

SetPrefixDialog::~SetPrefixDialog()
{
    delete ui;
}

void SetPrefixDialog::finishSetup()
{
    _selWidget = new ElementSelectionTypeWidget();
    Utils::replaceWidget(ui->layoutOfPlaceholder, ui->placeholder, _selWidget, true, Qt::AlignTop | Qt::AlignLeft, true);
    connect(_selWidget, SIGNAL(selectionTypeChanged(TargetSelection::Type)), this, SLOT(onSelectionTypeChanged(TargetSelection::Type)));
    if(NULL == _info->element) {
        _selWidget->setSelectionType(TargetSelection::AllItems);
    } else {
        _selWidget->setSelectionType(TargetSelection::SelectedItemAndChildren);
    }

    Utils::updateEditableCombo(ui->prefixes, _info->prefixInfo.allPrefixes.toList());
}


void SetPrefixDialog::onSelectionTypeChanged(TargetSelection::Type selectionType)
{
    _info->targetSelection = selectionType ;
}

void SetPrefixDialog::on_prefixes_currentTextChanged(const QString &)
{
    _info->setPrefix = ui->prefixes->currentText().trimmed();
    enableOk();
}

void SetPrefixDialog::on_prefixes_editTextChanged(const QString &)
{
    _info->setPrefix = ui->prefixes->currentText().trimmed();
    enableOk();
}

bool SetPrefixDialog::checkOK()
{
    return Utils::checkNsPrefix(_info->setPrefix);
}

void SetPrefixDialog::enableOk()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(checkOK());
}

void SetPrefixDialog::accept()
{
    if(!checkOK()) {
        Utils::error(this, tr("Invalid parameters."));
        return ;
    }
    QDialog::accept();
}
