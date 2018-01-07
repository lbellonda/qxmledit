/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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


#include "anonaddexcelementdialog.h"
#include "ui_anonaddexcelementdialog.h"
#include "utils.h"
#include "element.h"
#include "modules/xml/elmpath.h"

AnonAddExcElementDialog::AnonAddExcElementDialog(Element *element, const bool isElement, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnonAddExcElementDialog)
{
    _element = element ;
    _isText = !isElement;
    ui->setupUi(this);
    ui->settings->setText(_isText);
}

AnonAddExcElementDialog::~AnonAddExcElementDialog()
{
    delete ui;
}

AnonSettingsWidget *AnonAddExcElementDialog::settingsWidget()
{
    return ui->settings;
}

AnonException *AnonAddExcElementDialog::getException()
{
    AnonException *exception = new AnonException();
    exception->setUseNamespace(ui->settings->isUseNamespace());
    if(exception->isUseNamespace()) {
        ElmPathResolver resolver;
        resolver.select(_element);
        if(_isText) {
            exception->setPath(resolver.textPath());
        } else {
            exception->setPath(resolver.path());
        }
    } else {
        if(_isText) {
            exception->setPath(_element->pathStringAsText());
        } else {
            exception->setPath(_element->pathString());
        }
    }
    if(ui->settings->isIncludeChecked()) {
        if(ui->settings->isRecursiveChecked()) {
            exception->setCriteria(AnonInclusionCriteria::IncludeWithChildren);
        } else {
            exception->setCriteria(AnonInclusionCriteria::Include);
        }
    } else {
        if(ui->settings->isRecursiveChecked()) {
            exception->setCriteria(AnonInclusionCriteria::ExcludeWithChildren);
        } else {
            exception->setCriteria(AnonInclusionCriteria::Exclude);
        }
    }
    if(ui->settings->isFixedChecked()) {
        exception->setAnonType(AnonType::FixedValue);
        exception->setFixedValue(ui->settings->fixedValueText());
    } else {
        exception->setAnonType(AnonType::UseDefault);
    }
    return  exception ;
}

void AnonAddExcElementDialog::accept()
{
    QDialog::accept();
}

