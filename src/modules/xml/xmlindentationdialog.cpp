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


#include "xmlindentationdialog.h"
#include "regola.h"
#include "qxmleditdata.h"
#include "utils.h"
#include "ui_xmlindentationdialog.h"

XmlIndentationDialog::XmlIndentationDialog(QWidget *parent, Regola *newRegola, QXmlEditData *newAppData) :
    QDialog(parent),
    ui(new Ui::XmlIndentationDialog)
{
    _regola = newRegola;
    _appData = newAppData ;
    ui->setupUi(this);
    ui->xmlIndent->setMinimum(0);
    ui->xmlIndent->setMaximum(16);
    init();
}

XmlIndentationDialog::~XmlIndentationDialog()
{
    delete ui;
}

void XmlIndentationDialog::init()
{
    bool checkNoIndent = false;

    ui->indentSettings->setChecked(_regola->useIndentation());
    int indentValue = _regola->indentation();
    if(indentValue >= 0) {
        ui->xmlIndent->setValue(indentValue);
    } else {
        ui->xmlIndent->setValue(0);
        checkNoIndent = true ;
    }
    ui->chkNoIndent->setChecked(checkNoIndent);
    _started = true ;
}

void XmlIndentationDialog::on_chkNoIndent_stateChanged(int /*state*/)
{
    ui->xmlIndent->setEnabled(!ui->chkNoIndent->isChecked());
}

void XmlIndentationDialog::accept()
{
    _regola->setUseIndentation(ui->indentSettings->isChecked());
    if(ui->indentSettings->isChecked()) {
        if(ui->chkNoIndent->isChecked()) {
            _regola->setIndentationForce(-1);
        } else {
            _regola->setIndentationForce(ui->xmlIndent->value());
        }
        _regola->setSaveAttributesMethod(ui->cbSortAttributesAlpha->isChecked() ? Regola::SaveAttributesSortingAlphabetically : Regola::SaveAttributesNoSort);
    } else {
        _regola->setIndentationForce(_appData->xmlIndent());
        _regola->setSaveAttributesMethod(Regola::SaveAttributesUsingDefault);
    }
    QDialog::accept();
}
