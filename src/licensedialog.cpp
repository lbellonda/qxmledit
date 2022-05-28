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

#include "xmlEdit.h"
#include "licensedialog.h"
#include "qxmleditconfig.h"
#include "ui_licensedialog.h"

void LicenseDialog::licenseAgreement()
{
    if(!Config::getBool(Config::KEY_GENERAL_LICENSE_AGREED, false)) {
        LicenseDialog dlg;
        dlg.exec();
        Config::saveBool(Config::KEY_GENERAL_LICENSE_AGREED, true);
    }
}

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
}

LicenseDialog::~LicenseDialog()
{
    delete ui;
}

#ifdef QXMLEDIT_TEST
bool LicenseDialog::testLicenseValid = false ;
#endif

void LicenseDialog::showEvent(QShowEvent *event)
{
#ifdef QXMLEDIT_TEST
    QString license = ui->textBrowser->toPlainText();
    testLicenseValid = !license.isEmpty();
    QTimer::singleShot(200, this, SLOT(accept()));
#endif
    QDialog::showEvent(event);
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
