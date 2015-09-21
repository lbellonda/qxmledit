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


#include "configurexmlmanagementdialog.h"
#include "ui_configurexmlmanagementdialog.h"
#include "utils.h"

ConfigureXMLManagementDialog::ConfigureXMLManagementDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigureXMLManagementDialog)
{
    _started = false ;
    ui->setupUi(this);
    ui->xmlIndent->setMinimum(0);
    ui->xmlIndent->setMaximum(16);
}

ConfigureXMLManagementDialog::~ConfigureXMLManagementDialog()
{
    delete ui;
}


void ConfigureXMLManagementDialog::init(ApplicationData* data)
{
    _data = data ;
    ui->chkMetadata->setChecked(_data->isUpdateMetadata());
    Utils::setupComboEncoding(ui->comboEncoding, true, _data->autoInsertPrologEncoding());
    ui->chkInsProlog->setChecked(_data->isAutoInsertProlog());
    bool checkNoIndent = false;
    int indentValue = _data->xmlIndent();
    if(indentValue >= 0) {
        ui->xmlIndent->setValue(_data->xmlIndent());
    } else {
        ui->xmlIndent->setValue(QXmlEditData::XmlIndentDefault);
        checkNoIndent = true ;
    }
    ui->chkNoIndent->setChecked(checkNoIndent);
    calcEnableProlog();
    enableIndent();
    ui->cbSaveStream->setChecked(Regola::isSaveUsingStream());
    ui->chkSortAttributes->setChecked(Regola::isSaveSortAlphaAttribute());
    _started = true ;
}


void ConfigureXMLManagementDialog::saveIfChanged()
{
}

void ConfigureXMLManagementDialog::on_chkMetadata_clicked(bool /*state*/)
{
    _data->setUpdateMetadata(ui->chkMetadata->isChecked());
}

void ConfigureXMLManagementDialog::on_chkInsProlog_clicked(bool /*state*/)
{
    _data->setAutoInsertProlog(ui->chkInsProlog->isChecked());
    calcEnableProlog();
}


void ConfigureXMLManagementDialog::calcEnableProlog()
{
    bool enableEncoding = ui->chkInsProlog->isChecked();
    ui->comboEncoding->setEnabled(enableEncoding);
}

void ConfigureXMLManagementDialog::on_comboEncoding_activated(int /*index*/)
{
    QString encoding;
    int index = ui->comboEncoding->currentIndex();
    if(index >= 0) {
        QVariant data = ui->comboEncoding->itemData(index);
        QTextCodec *codec = QTextCodec::codecForMib(index) ;
        if(NULL != codec) {
            encoding = codec->name();
        }
    }
    _data->setAutoInsertPrologEncoding(encoding);
}

void ConfigureXMLManagementDialog::on_xmlIndent_valueChanged(int newValue)
{
    if(_started && !ui->chkNoIndent->isChecked()) {
        _data->setXmlIndent(newValue);
    }
}


void ConfigureXMLManagementDialog::on_cmdIndentReset_clicked()
{
    ui->chkNoIndent->setChecked(false);
    enableIndent();
    ui->xmlIndent->setValue(QXmlEditData::XmlIndentDefault);
}


void ConfigureXMLManagementDialog::enableIndent()
{
    ui->xmlIndent->setEnabled(!ui->chkNoIndent->isChecked());
}

void ConfigureXMLManagementDialog::on_chkNoIndent_stateChanged(int /*state*/)
{
    if(_started) {
        enableIndent();
        if(ui->chkNoIndent->isChecked()) {
            _data->setXmlIndent(-1);
        } else {
            ui->xmlIndent->setValue(ui->xmlIndent->value());
            _data->setXmlIndent(ui->xmlIndent->value());
        }
    }
}

void ConfigureXMLManagementDialog::on_cbSaveStream_stateChanged(int /*state*/)
{
    if(_started) {
        Config::saveBool(Config::KEY_XML_SAVE_STREAM, ui->cbSaveStream->isChecked());
    }
}

void ConfigureXMLManagementDialog::on_chkSortAttributes_stateChanged(int /*state*/)
{
    if(_started) {
        Config::saveBool(Config::KEY_XML_SAVE_SORTATTRIBUTES, ui->chkSortAttributes->isChecked());
    }
}



