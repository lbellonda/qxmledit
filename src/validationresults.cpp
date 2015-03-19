/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
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


#include "validationresults.h"
#include "texteditutils.h"
#include "ui_validationresults.h"

void showValidationResults(const QString text, ValidatorMessageHandler &validator, QWidget *parent)
{
    ValidationResults dialog(text, validator, parent);
    dialog.setModal(true);
    dialog.exec();
}

ValidationResults::ValidationResults(const QString text, ValidatorMessageHandler &validator, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ValidationResults),
    _validator(validator)
{
    ui->setupUi(this);
    ui->textBrowser->setText(text);
    TextEditUtils::gotoPos(ui->textBrowser, validator.column(), validator.line());
    TextEditUtils::hiliteCurrentPos(ui->textBrowser);
    ui->error->setText(QString(tr("Line:%1, Col:%2 :%3"))
                       .arg(validator.line())
                       .arg(validator.column())
                       .arg(validator.description()));
}

ValidationResults::~ValidationResults()
{
    delete ui;
}

void ValidationResults::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch(e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
