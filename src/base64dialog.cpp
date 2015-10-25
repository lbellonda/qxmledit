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
#include "config.h"


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
    _type = (Base64Utils::EBase64)Config::getBool(Config::KEY_BASE64_TYPE, Base64Utils::RFC4648Standard);
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
    QStringList base64Labels;
    QList<int> base64Values;
    base64Labels << QString("RFC4648 %1").arg("(Standard)");
    base64Labels << QString("RFC6920 %1").arg("(Url)");
    base64Values << Base64Utils::RFC4648Standard;
    base64Values << Base64Utils::RFC6920Url;
    Utils::loadComboCodedArrays(ui->cbType, _type, base64Labels, base64Values);
}

void Base64Dialog::base64textChanged()
{
    if(_isConverting) {
        return ;
    }
    _isConverting = true ;
    QString base64Text = ui->base64Edit->toPlainText();
    Base64Utils base64;
    QByteArray array2 = base64.fromBase64(_type, base64Text);
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
    Base64Utils base64;
    QByteArray bytes = _currentCodec->fromUnicode(text);
    QString result = base64.toBase64(_type, bytes);
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

void Base64Dialog::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
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
    Base64Utils base64Utils;
    QString strBase64 = base64Utils.loadFromBinaryFile(_type, this, filePath, isError, isAbort);
    if(!(isError || isAbort)) {
        ui->base64Edit->setPlainText(strBase64);
    }
}

void Base64Dialog::on_cmdLoadFromTextFile_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                       tr("Choose a Text File to be Transformed in Base 64 Encoded Text"),
                       QXmlEditData::sysFilePathForOperation(_fileDataPath), tr("All files (*);;Text files (*.txt);;XML files (*.xml);;XML Schema files (*.xsd);;"));
    if(!filePath.isEmpty()) {
        _fileDataPath = filePath ;
        loadTextFile(filePath);
    }
}


void Base64Dialog::loadTextFile(const QString &filePath)
{
    bool isError = true ;
    bool isAbort = false ;
    QString newText = Utils::loadTextFile(this, filePath, isError, isAbort);
    if(!(isError || isAbort)) {
        ui->textEdit->setPlainText(newText);
    }
}

void Base64Dialog::on_cmdSaveBinaryData_clicked()
{
    QString text = ui->base64Edit->toPlainText();
    if(!text.isEmpty()) {
        Base64Utils base64Utils;
        if(!base64Utils.saveBase64ToBinaryFile(_type, this, text, QXmlEditData::sysFilePathForOperation(_fileDataPath))) {
            Utils::error(this, tr("Operation failed"));
        }
    }
}

void Base64Dialog::on_cbType_currentIndexChanged(int index)
{
    if(index >= 0) {
        int value = ui->cbType->itemData(index).toInt();
        _type = (Base64Utils::EBase64) value;
        Config::saveBool(Config::KEY_BASE64_TYPE, _type);
        //reload data
        textChanged();
    }
}
