/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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


#include "anonymizedialog.h"
#include "ui_anonymizedialog.h"

AnonymizeDialog::AnonymizeDialog(QWidget *parent, AnonymizeParameters *theParams) :
    QDialog(parent),
    ui(new Ui::AnonymizeDialog)
{
    _params = theParams ;
    ui->setupUi(this);
    _params->mode = AnonymizeParameters::UsingPatterns ;
    _params->useFixedLetter = false;
    ui->codeChk->setChecked(true);
    ui->threshold->setValue(_params->threshold);
    ui->threshold->setEnabled(ui->codeChk->isChecked());
}

AnonymizeDialog::~AnonymizeDialog()
{
    delete ui;
}

void AnonymizeDialog::accept()
{
    QDialog::accept();
}

void AnonymizeDialog::on_allChk_clicked(bool /*checked*/)
{
    _params->mode = AnonymizeParameters::AllText ;
    ui->threshold->setEnabled(ui->codeChk->isChecked());
}

void AnonymizeDialog::on_codeChk_clicked(bool /*checked*/)
{
    _params->mode = AnonymizeParameters::UsingPatterns ;
    ui->threshold->setEnabled(ui->codeChk->isChecked());
}

void AnonymizeDialog::on_chkFixedPattern_clicked(bool /*checked*/)
{
    _params->useFixedLetter = ui->chkFixedPattern->isChecked();
}

void AnonymizeDialog::on_threshold_valueChanged(int /*i*/)
{
    _params->threshold = ui->threshold->value();
}
