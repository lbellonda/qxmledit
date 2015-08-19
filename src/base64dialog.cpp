/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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
#include "base64dialog.h"
#include "ui_base64dialog.h"
#include "qxmleditdata.h"
#include "utils.h"
#include <QTextStream>

void Base64Dialog::showDialog()
{
    Base64Dialog dialog;
    dialog.exec();
}


Base64Dialog::Base64Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Base64Dialog)
{
    _isConverting = false;
    _currentCodec = NULL ;
    ui->setupUi(this);
    setupOther();
    setAcceptDrops(true);
}

Base64Dialog::~Base64Dialog()
{
    delete ui;
}

void Base64Dialog::setupOther()
{
    connect(ui->base64Edit, SIGNAL(textChanged()), this, SLOT(base64textChanged()));
    connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(textChanged()));
    connect(ui->fontEncoding, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentEncodingChanged(int)));
    Utils::setupComboEncoding(ui->fontEncoding);
    int currentCodec = QTextCodec::codecForName("UTF-8")->mibEnum();
    Utils::selectComboValue(ui->fontEncoding, currentCodec);
}

void Base64Dialog::base64textChanged()
{
    if(_isConverting) {
        return ;
    }
    _isConverting = true ;
    QString base64Text = ui->base64Edit->toPlainText();

    QByteArray array(base64Text.toLatin1());
    QByteArray array2 = QByteArray::fromBase64(array);
    QTextStream textStream(array2);
    textStream.setAutoDetectUnicode(false);
    if(NULL != _currentCodec) {
        textStream.setCodec(_currentCodec);
    }
    QString result = textStream.readAll();
    ui->textEdit->setPlainText(result);
    _isConverting = false ;
}

void Base64Dialog::textChanged()
{
    if(_isConverting) {
        return ;
    }
    _isConverting = true ;
    QString text = ui->textEdit->toPlainText();
    QString result = Utils::toBase64(text);
    ui->base64Edit->setPlainText(result);
    _isConverting = false ;
}

void Base64Dialog::onCurrentEncodingChanged(int newIndex)
{
    if(newIndex >= 0) {
        QVariant data = ui->fontEncoding->itemData(newIndex);
        int mib = data.toInt();
        _currentCodec = QTextCodec::codecForMib(mib);
        base64textChanged();
    }
}

void Base64Dialog::dropEvent(QDropEvent *event)
{
    if(event->mimeData()->hasFormat("text/uri-list")) {
        QString filePath = "" ;
        event->acceptProposedAction();
        if(event->mimeData()->hasUrls()) {
            foreach(QUrl url, event->mimeData()->urls()) {
                filePath = url.toLocalFile();
                break;
            }
        }
        if(filePath.length() > 0) {
            loadFromBinaryFile(filePath);
        }
        event->acceptProposedAction();
    }
}

void Base64Dialog::on_cmdLoadFromFile_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                       tr("Choose a File to be Transformed in Base 64 Encoded Text"),
                       QXmlEditData::sysFilePathForOperation(_fileDataPath), tr("All files (*);;XML files (*.xml);;XML Schema files (*.xsd);;"));
    if(!filePath.isEmpty()) {
        _fileDataPath = filePath ;
        loadFromBinaryFile(filePath);
    }
}

void Base64Dialog::loadFromBinaryFile(const QString &filePath)
{
    bool isError = true ;
    bool isAbort = false ;
    QFile file(filePath);
    QByteArray data ;
    if(file.open(QIODevice::ReadOnly)) {
        qint64 fileSize = file.size();
        if(fileSize > InputSizeLimit) {
            if(!Utils::askYN(this, tr("Warning: the size of the file to import is %1. Do you want to continue?").arg(Utils::getSizeForPresentation(fileSize)))) {
                isAbort = true ;
                isError = false ;
            }
        }
        if(!isAbort) {
            data = file.readAll();
            if(file.error() == QFile::NoError) {
                isError = false ;
            }
        }
        file.close();
        if(isError) {
            Utils::error(tr("Error reading file."));
        } else {
            QByteArray converted = data.toBase64();
            QString strBase64 = converted.data();
            _isConverting = true ;
            ui->base64Edit->setPlainText(strBase64);
            ui->textEdit->setPlainText("");
            _isConverting = false ;
        }
    } else {
        Utils::error(QString(tr("Unable to load file.\nError code is '%1'")).arg(file.error()));
    }
}
