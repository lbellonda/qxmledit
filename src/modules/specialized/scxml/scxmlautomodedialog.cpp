/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016 by Luca Bellonda and individual contributors       *
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

#include "scxmlautomodedialog.h"
#include "ui_scxmlautomodedialog.h"
#include "utils.h"
#ifdef QXMLEDIT_TEST
#include "../test/testhelpers/scxmlautomodedialogprivatetest.h"
#endif


bool SCXMLAutoModeDialog::showToUser(QWidget *parent, QXmlEditData *data)
{
    SCXMLAutoModeDialog dlg(parent, data);
    dlg.exec();
    return dlg.resultSCXML();
}

SCXMLAutoModeDialog::SCXMLAutoModeDialog(QWidget *parent, QXmlEditData *data) :
    QDialog(parent),
#ifdef QXMLEDIT_TEST
    d(new PrivateTest(this)),
#endif
    ui(new Ui::SCXMLAutoModeDialog)
{
    _enterSCXML = false ;
    ui->setupUi(this);
    _result = DONOTENTER_SCXMLMODE;
    _data = data;
    QStyle *style = QApplication::style();
    QIcon icon = style->standardIcon(QStyle::SP_MessageBoxQuestion, 0, this);
    ui->questionIcon->setPixmap(icon.pixmap(32));
#ifdef QXMLEDIT_TEST
    QTimer::singleShot(50, d, SLOT(testStart()));
#endif
}

SCXMLAutoModeDialog::~SCXMLAutoModeDialog()
{
    delete ui;
#ifdef QXMLEDIT_TEST
    delete d;
#endif
}

void SCXMLAutoModeDialog::setAnswer(const ERetCode retCode, const bool enterSCXML)
{
    _enterSCXML = enterSCXML ;
    _result = retCode ;
    _data->setShowSCXMLPanel(!ui->cbDoNotShow->isChecked());
    accept();
}


void SCXMLAutoModeDialog::on_cmdYes_clicked()
{
    setAnswer(ENTER_SCXMLMODE, true);
}

void SCXMLAutoModeDialog::on_cmdNo_clicked()
{
    setAnswer(DONOTENTER_SCXMLMODE, false);
}

void SCXMLAutoModeDialog::on_cmdAlways_clicked()
{
    _data->setAutoSCXMLMode(true);
    setAnswer(ALWAYS_SCXMLMODE, true);
}

bool SCXMLAutoModeDialog::resultSCXML()
{
    return _enterSCXML;
}

