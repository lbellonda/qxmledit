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


#include "licensedialog.h"
#include "ui_licensedialog.h"
#include "xmlEdit.h"

LicenseDialog::LicenseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LicenseDialog)
{
    ui->setupUi(this);
    QString textLicense = readLicense(":/license/lgplv2.txt");
    if(textLicense.isEmpty()) {
        reject();
    }
    ui->textBrowser->setPlainText(textLicense);
    ui->radioNo->setChecked(true);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

LicenseDialog::~LicenseDialog()
{
    delete ui;
}

QString LicenseDialog::readLicense(const QString & filePath)
{
    QString result ;
    QFile file(filePath);
    if(file.open(QIODevice::ReadOnly)) {
        QTextStream streamIn(&file);
        streamIn.setAutoDetectUnicode(false);
        streamIn.setCodec("utf-8");
        result = streamIn.readAll();
        if(file.error() != QFile::NoError) {
            result = "" ;
        }
        file.close();
    }
    return result ;
}


void LicenseDialog::on_radioNo_clicked(bool /*clicked*/)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

void LicenseDialog::on_radioYes_clicked(bool /*clicked*/)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ui->radioYes->isChecked());
}

