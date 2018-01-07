/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2018 by Luca Bellonda and individual contributors  *
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


#include "anonsettingwidget.h"
#include "ui_anonsettingwidget.h"
#include "utils.h"

AnonSettingsWidget::AnonSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnonSettingsWidget)
{
    _isText = false;
    ui->setupUi(this);
    ui->exclude->setChecked(true);
    ui->methodDefault->setChecked(true);
    if(_isText) {
        ui->chkRecursive->setEnabled(false);
    }
    enableItems();
}

AnonSettingsWidget::~AnonSettingsWidget()
{
    delete ui;
}

void AnonSettingsWidget::setText(const bool isText)
{
    _isText = isText ;
    ui->chkRecursive->setEnabled(!_isText);
    enableItems();
}

void AnonSettingsWidget::enableItems()
{
    bool enableGroupMethods = false ;
    if(ui->include->isChecked()) {
        enableGroupMethods = true ;
    }
    ui->methodGroup->setEnabled(enableGroupMethods);
}


//--------
void AnonSettingsWidget::on_include_clicked()
{
    enableItems();
}

void AnonSettingsWidget::on_exclude_clicked()
{
    enableItems();
}


void AnonSettingsWidget::on_methodDefault_clicked()
{
    enableItems();
}

void AnonSettingsWidget::on_methodFixed_clicked()
{
    enableItems();
}

void AnonSettingsWidget::on_FixedValue_textEdited(const QString & /*text*/)
{
    ui->methodFixed->setChecked(true);
    enableItems();
}

///---------
///
bool AnonSettingsWidget::isIncludeChecked()
{
    return ui->include->isChecked();
}

bool AnonSettingsWidget::isRecursiveChecked()
{
    return ui->chkRecursive->isChecked();
}

bool AnonSettingsWidget::isFixedChecked()
{
    return ui->methodFixed->isChecked();
}

QString AnonSettingsWidget::fixedValueText()
{
    return ui->FixedValue->text();
}

bool AnonSettingsWidget::isUseNamespace()
{
    return ui->processNamespaces->isChecked();
}

void AnonSettingsWidget::setUseNamespace(const bool how)
{
    ui->processNamespaces->setChecked(how);
}

///---------

void AnonSettingsWidget::setExcludeChecked(const bool how)
{
    ui->exclude->setChecked(how);
}

void AnonSettingsWidget::setMethodDefaultChecked(const bool how)
{
    ui->methodDefault->setChecked(how);
}

void AnonSettingsWidget::enableRecursive(const bool how)
{
    ui->chkRecursive->setEnabled(how);
}
