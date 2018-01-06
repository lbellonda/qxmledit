/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2017-2018 by Luca Bellonda and individual contributors  *
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

#include "showxslerrorsdialog.h"
#include "ui_showxslerrorsdialog.h"
#include "operationresult.h"
#include <QClipboard>
#include "utils.h"

void ShowXSLErrorsDialog::showErrors(QWidget *parent, MessagesOperationResult *messages)
{
    ShowXSLErrorsDialog dialog(parent, messages);
    dialog.exec();
}

ShowXSLErrorsDialog::ShowXSLErrorsDialog(QWidget *parent, MessagesOperationResult *messages) :
    QDialog(parent),
    ui(new Ui::ShowXSLErrorsDialog)
{
    _messages = messages;
    ui->setupUi(this);
    finishSetup();
}

ShowXSLErrorsDialog::~ShowXSLErrorsDialog()
{
    delete ui;
}

void ShowXSLErrorsDialog::finishSetup()
{
    ui->messages->setColumnCount(2);
    QStringList headers ;
    headers << tr("Type") << tr("message");
    ui->messages->setHorizontalHeaderLabels(headers);
    ui->messages->horizontalHeader()->setStretchLastSection(true);
    ui->messages->setRowCount(_messages->messages()->size());
    int row = 0;
    foreach(SourceMessage* message, *_messages->messages()) {
        QTableWidgetItem *itemType = new QTableWidgetItem(decodeType(message->type()));
        ui->messages->setItem(row, 0, itemType);
        QTableWidgetItem *itemText = new QTableWidgetItem(message->description());
        ui->messages->setItem(row, 1, itemText);
        row++;
    }
}

void ShowXSLErrorsDialog::on_cmdCopy_clicked()
{
    QString result ;
    foreach(SourceMessage* message, *_messages->messages()) {
        result += QString("%1 %2\n").arg(decodeType(message->type())).arg(message->description());
    }
    QClipboard *clipboard = QApplication::clipboard();
    if(NULL != clipboard) {
        clipboard->setText(result);
    }

}

QString ShowXSLErrorsDialog::decodeType(const SourceMessage::EType type)
{
    switch(type) {
    case SourceMessage::Debug:
        return tr("Debug");
        break;
    case SourceMessage::Warning:
        return tr("Warning");
        break;
    case SourceMessage::Error:
        return tr("Error");
        break;
    case SourceMessage::Info:
        return tr("Info");
        break;
    default:
        return tr("Error");
        break;
    }
}
