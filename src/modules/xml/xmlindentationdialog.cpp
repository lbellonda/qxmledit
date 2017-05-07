/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2016 by Luca Bellonda and individual contributors  *
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
    _attributeHelper.init(ui->attrNoIndendation, ui->attrNewLineAt, ui->attrCharacters);

    ui->indentSettings->setChecked(_regola->overrideGlobalIndentation());
    int indentValue = _regola->indentation();
    if(indentValue >= 0) {
        ui->xmlIndent->setValue(indentValue);
    } else {
        ui->xmlIndent->setValue(0);
        checkNoIndent = true ;
    }
    ui->chkNoIndent->setChecked(checkNoIndent);
    _attributeHelper.setUp(_regola->xmlIndentAttributesType(), _regola->xmlIndentAttributesColumns());
    ui->cbFormattingInfo->setChecked(_regola->hasFormattingInfo());

    _started = true ;
}

void XmlIndentationDialog::on_chkNoIndent_stateChanged(int /*state*/)
{
    ui->xmlIndent->setEnabled(!ui->chkNoIndent->isChecked());
}

void XmlIndentationDialog::doAccept()
{
    accept();
}

void XmlIndentationDialog::accept()
{
    _regola->setOverrideGlobalIndentation(ui->indentSettings->isChecked());
    if(ui->indentSettings->isChecked()) {
        if(ui->chkNoIndent->isChecked()) {
            _regola->setIndentationForce(-1);
        } else {
            _regola->setIndentationForce(ui->xmlIndent->value());
        }
        _regola->setSaveAttributesMethod(ui->cbSortAttributesAlpha->isChecked() ? Regola::SaveAttributesSortingAlphabetically : Regola::SaveAttributesNoSort);
        _regola->setIndentAttributesSettings(true, _attributeHelper.type(), _attributeHelper.columns());
        _regola->setUseXmlIndentAttributesSettings(true);
    } else {
        _regola->setIndentationForce(_appData->xmlIndent());
        _regola->setSaveAttributesMethod(Regola::SaveAttributesUsingDefault);
        _regola->setIndentAttributesSettings(true, _appData->xmlIndentAttributesType(), _appData->xmlIndentAttributes());
        _regola->setUseXmlIndentAttributesSettings(false);
    }
    _regola->setFormattingInfo(ui->cbFormattingInfo->isChecked());
    QDialog::accept();
}

void XmlIndentationDialog::on_cmdPredefinedAttributes_clicked()
{
    _attributeHelper.doPredefined();
}

void XmlIndentationDialog::on_attrNoIndendation_clicked(bool /*checked*/)
{
    _attributeHelper.onSelection();
}

void XmlIndentationDialog::on_attrNewLineAt_clicked(bool /*checked*/)
{
    _attributeHelper.onSelection();
}
