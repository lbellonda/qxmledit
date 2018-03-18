/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2018 by Luca Bellonda and individual contributors       *
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

#include "configuregeneralindentation.h"
#include "ui_configuregeneralindentation.h"
#include "qxmleditdata.h"
#include "qxmleditconfig.h"
#include "utils.h"


ConfigureGeneralIndentation::ConfigureGeneralIndentation(QWidget *parent, QXmlEditData *newAppData) :
    QDialog(parent),
    ui(new Ui::ConfigureGeneralIndentation)
{
    Utils::TODO_THIS_RELEASE("se guided ordina gli attributi alfabeticamente");
    Utils::TODO_THIS_RELEASE("fare");
    Utils::TODO_THIS_RELEASE("test");
    _appData = newAppData ;
    ui->setupUi(this);
    ui->xmlIndent->setMinimum(0);
    ui->xmlIndent->setMaximum(16);
    init();
}

ConfigureGeneralIndentation::~ConfigureGeneralIndentation()
{
    delete ui;
}

void ConfigureGeneralIndentation::init()
{
    bool checkNoIndent = false;
    _attributeHelper.init(ui->attrNoIndendation, ui->attrNewLineAt, ui->attrCharacters);

    const int indentValue = _appData->xmlIndent();
    if(indentValue >= 0) {
        ui->xmlIndent->setValue(indentValue);
    } else {
        ui->xmlIndent->setValue(QXmlEditData::XmlIndentDefault);
        checkNoIndent = true ;
    }
    ui->chkNoIndent->setChecked(checkNoIndent);
    _attributeHelper.setUp(_appData->xmlIndentAttributesType(), _appData->xmlIndentAttributes());

    _started = true ;
}

void ConfigureGeneralIndentation::on_chkNoIndent_stateChanged(int /*state*/)
{
    ui->xmlIndent->setEnabled(!ui->chkNoIndent->isChecked());
}

void ConfigureGeneralIndentation::doAccept()
{
    accept();
}

void ConfigureGeneralIndentation::accept()
{
    if(ui->chkNoIndent->isChecked()) {
        _appData->setXmlIndent(-1);
    } else {
        _appData->setXmlIndent(ui->xmlIndent->value());
    }
    Config::saveBool(Config::KEY_XML_SAVE_SORTATTRIBUTES, ui->cbSortAttributesAlpha->isChecked());
    _appData->setXmlIndentAttributesType(_attributeHelper.type());
    _appData->setXmlIndentAttributes(_attributeHelper.columns());
    QDialog::accept();
}

void ConfigureGeneralIndentation::on_cmdPredefinedAttributes_clicked()
{
    _attributeHelper.doPredefined();
}

void ConfigureGeneralIndentation::on_attrNoIndendation_clicked(bool /*checked*/)
{
    _attributeHelper.onSelection();
}

void ConfigureGeneralIndentation::on_attrNewLineAt_clicked(bool /*checked*/)
{
    _attributeHelper.onSelection();
}
