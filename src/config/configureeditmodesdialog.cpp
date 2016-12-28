/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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


#include "configureeditmodesdialog.h"
#include "ui_configureeditmodesdialog.h"

ConfigureEditModesDialog::ConfigureEditModesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigureEditModesDialog)
{
    _started = false ;
    ui->setupUi(this);
}

ConfigureEditModesDialog::~ConfigureEditModesDialog()
{
    delete ui;
}

void ConfigureEditModesDialog::init(ApplicationData* data)
{
    _data = data ;
    ui->cbShowXSLTPanel->setChecked(data->isShowXSLTPanel());
    ui->cbAutomaticallySwitchToXSLTMode->setChecked(data->isAutoXSLTMode());
    ui->cbShowSCXMLPanel->setChecked(data->isShowSCXMLPanel());
    ui->cbAutomaticallySwitchToSCXMLMode->setChecked(data->isAutoSCXMLMode());
    doEnable();
    _started = true ;
}

void ConfigureEditModesDialog::saveIfChanged()
{
    // do nothing
}

void ConfigureEditModesDialog::on_cbShowXSLTPanel_clicked(bool /* */)
{
    if(_started) {
        _data->setShowXSLTPanel(ui->cbShowXSLTPanel->isChecked());
    }
}

void ConfigureEditModesDialog::on_cbShowSCXMLPanel_clicked(bool /* */)
{
    if(_started) {
        _data->setShowSCXMLPanel(ui->cbShowSCXMLPanel->isChecked());
    }
}

void ConfigureEditModesDialog::on_cbAutomaticallySwitchToXSLTMode_clicked(bool /* */)
{
    if(_started) {
        _data->setAutoXSLTMode(ui->cbAutomaticallySwitchToXSLTMode->isChecked());
    }
    doEnable();
}

void ConfigureEditModesDialog::on_cbAutomaticallySwitchToSCXMLMode_clicked(bool /* */)
{
    if(_started) {
        _data->setAutoSCXMLMode(ui->cbAutomaticallySwitchToSCXMLMode->isChecked());
    }
    doEnable();
}

void ConfigureEditModesDialog::doEnable()
{
    ui->cbShowXSLTPanel->setEnabled(!ui->cbAutomaticallySwitchToXSLTMode->isChecked());
    ui->cbShowSCXMLPanel->setEnabled(!ui->cbAutomaticallySwitchToSCXMLMode->isChecked());
}
