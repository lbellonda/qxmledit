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


#include "codepagedialog.h"
#include "ui_codepagedialog.h"
#include "qxmleditconfig.h"
#include "utils.h"
#include "codepageitemdelegate.h"

#include <QTextCodec>

//----

void CodePageDialog::showCodePage(QXmlEditData *theData, QWidget *parent)
{
    CodePageDialog codePage(theData, parent);
    codePage.setModal(true);
    codePage.exec();
}

void CodePageDialog::showCodePageWindow(QXmlEditData *theData, QWidget *parent)
{
    CodePageDialog *codePage = new CodePageDialog(theData, parent);
    codePage->setAutoDelete();
    codePage->setModal(false);
    codePage->show();
}
//----

CodePageDialog::CodePageDialog(QXmlEditData *theData, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CodePageDialog)
{
    Utils::TODO_NEXT_RELEASE("se names mostra selezionato con nome, manca stato iniziale");
    Utils::TODO_NEXT_RELEASE("colori");
    _isAutoDelete = false ;
    _appData = theData ;
    ui->setupUi(this);
    setup();
    showMaximized();
}

CodePageDialog::~CodePageDialog()
{
    delete ui;
}

void CodePageDialog::setAutoDelete()
{
    _isAutoDelete = true ;
}

void CodePageDialog::closeEvent(QCloseEvent * event)
{
    event->accept();
    if(_isAutoDelete) {
        deleteLater();
    }
}

//------

void CodePageDialog::readOptions()
{
    ui->chkUTF8->setChecked(Config::getBool(Config::KEY_CODEPAGE_VISUTF8, true));
    ui->chkValue->setChecked(Config::getBool(Config::KEY_CODEPAGE_VISVALUE, true));
    ui->chkName->setChecked(Config::getBool(Config::KEY_CODEPAGE_VISNAMES, true)) ;
}

void CodePageDialog::setup()
{
    _chars.reserve(256);
    for(int i = 0 ; i < 256 ; i++) {
        _chars << QChar('\0');
    }
    CodePageItemDelegate *delegate = new CodePageItemDelegate();
    ui->searchUnicode->setInputMask("hhhhhhhh");
    ui->searchUnicode->selectAll();
    ui->searchChar->selectAll();

    ui->codepageWidget->setItemDelegate(delegate);
    readOptions();
    Utils::setupComboEncoding(ui->cmbEncodings);
    ui->codepageWidget->setRowCount(16);
    ui->codepageWidget->setColumnCount(16);
    QStringList hLabels, vLabels;
    for(int i = 0 ; i < 16 ; i ++) {
        hLabels << Utils::fmtNumber(true, i, 2);
        vLabels << Utils::fmtNumber(true, i * 16, 2);
    }
    ui->codepageWidget->setHorizontalHeaderLabels(hLabels);
    ui->codepageWidget->setVerticalHeaderLabels(vLabels);
    ui->cmbEncodings->setCurrentIndex(-1);
    enableSearch();
    // select the default system codepage
    QTextCodec * localeCodec = QTextCodec::codecForLocale();
    if(NULL != localeCodec) {
        int index = ui->cmbEncodings->findText(localeCodec->name());
        ui->cmbEncodings->setCurrentIndex(index);
        repaint();
    }
}

void CodePageDialog::on_cmbEncodings_activated(const QString & /*text*/)
{
    repaint();
    enableSearch();
    search();
}

void CodePageDialog::repaint()
{
    bool visUTF8 = ui->chkUTF8->isChecked();
    bool visValue = ui->chkValue->isChecked();
    bool viewNames = ui->chkName->isChecked();
    QString codecName = ui->cmbEncodings->currentText() ;
    QTextCodec *codec = QTextCodec::codecForName(codecName.toLatin1().data());
    if(NULL == codec) {
        ui->codepageWidget->clearContents();
        Utils::error(this, "No codec found for the selected encoding. Unable to display values.");
        return ;
    }

    int charIndex = 0;
    for(int r = 0 ; r < 16 ; r ++) {
        for(int c = 0 ; c < 16 ; c ++) {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setFlags(item->flags()&~Qt::ItemIsEditable);
            int value = r * 16 + c;
            QByteArray data(1, (char)value);
            bool canEncode = false ;
            QString origString = codec->toUnicode(data);
            if(origString.length() > 0) {
                canEncode = true ;
            }
            item->setText(origString);
            QString valueString = "<html><body bgcolor=\"$BODY$\"><p align='center'>";
            valueString += "<FONT color=\"$BLUE$\"><BIG>";
            valueString += Utils::escapeHTML(canEncode ? origString : tr("<Invalid>"));
            valueString += "</BIG></FONT>";
            if(origString.length() > 0) {
                _chars.replace(charIndex, QChar(origString.at(0)));
                QChar theChar = origString.at(0);
                QString charName ;
                if(viewNames) {
                    QString theCharName = _appData->unicodeHelper()->nameForChar(theChar);
                    if(!theCharName.isEmpty()) {
                        charName = QString("<BR><FONT COLOR='$RED$'><I><small>%1</small></I></FONT>").arg(Utils::escapeHTML(theCharName));
                    }
                }
                if(visValue) {
                    valueString += QString("<BR/><FONT color=\"$TEXT$\">%1 (0x%2)<BR/>U:x%3</FONT>%4")
                                   .arg(QString::number(value, 10).toUpper())
                                   .arg(QString::number(value, 16).toUpper())
                                   .arg(QString::number(theChar.unicode(), 16).toUpper())
                                   .arg(charName);
                } else {
                    if(viewNames) {
                        valueString += QString("%1").arg(charName);
                    }
                }
                if(visUTF8) {
                    valueString += "<BR/><FONT color=\"$GREEN$\">" ;
                    QByteArray utfData = origString.toUtf8();
                    int textLength = utfData.length();
                    for(int i = 0 ; i < textLength ; i ++) {
                        char srcChar = utfData.at(i);
                        if(i > 0) {
                            valueString += ", ";
                        }
                        valueString += Utils::fmtNumber(true, srcChar, 2);
                    }
                    valueString += "</FONT>";
                }
            } else {
                _chars.replace(charIndex, QChar('\0'));
            }
            valueString.append("</p></body><html>");
            item->setData(Qt::UserRole, valueString);
            if(canEncode && (origString.length() > 0)) {
                QChar theChar = origString.at(0);
                QString tooltipText = QString("%1").arg(theChar);
                QString name = _appData->unicodeHelper()->nameForChar(theChar);
                if(!name.isEmpty()) {
                    tooltipText += " (";
                    tooltipText += name ;
                    tooltipText += " )";
                }
                if(!tooltipText.isEmpty()) {
                    item->setToolTip(tooltipText);
                }
            }
            ui->codepageWidget->setItem(r, c, item);
            charIndex ++ ;
        }
    }
    ui->codepageWidget->resizeColumnsToContents();
    ui->codepageWidget->resizeRowsToContents();
}

void CodePageDialog::on_chkUTF8_clicked()
{
    repaint();
}

void CodePageDialog::on_chkValue_clicked()
{
    repaint();
}

void CodePageDialog::on_chkName_clicked()
{
    repaint();
}

void CodePageDialog::enableSearch()
{
    bool enableSearchChar = false;
    bool enableSearchUnicode = false;
    if(ui->radioSearchChar->isChecked()) {
        enableSearchChar = true ;
    }
    if(ui->radioSearchUnicode->isChecked()) {
        enableSearchUnicode = true ;
    }
    ui->searchChar->setEnabled(enableSearchChar);
    ui->searchUnicode->setEnabled(enableSearchUnicode);
}

void CodePageDialog::search()
{
    bool existsResult = false;
    int index = 0 ;
    QChar chSel = 0;
    if(ui->radioSearchChar->isChecked()) {
        QString res = ui->searchChar->text().trimmed();
        if(!res.isEmpty()) {
            QChar ch = res.at(0);
            for(int i = 0 ; i < 256 ; i++) {
                if(_chars[i] == ch) {
                    existsResult = true ;
                    index = i ;
                    chSel = _chars[i];
                    break;
                }
            }
        }
    }
    if(ui->radioSearchUnicode->isChecked()) {
        QString hex = ui->searchUnicode->text().trimmed();
        if(!hex.isEmpty()) {
            bool isOk = false;
            ushort value = hex.toInt(&isOk, 16);
            if(!isOk) {
                ui->searchResults->setText(tr("Hexadecimal number not valid."));
                return ;
            }
            for(int i = 0 ; i < 256 ; i++) {
                if(_chars[i].unicode() == value) {
                    existsResult = true ;
                    index = i ;
                    chSel = _chars[i];
                    break;
                }
            }
        }


    }
    // apply the result
    if(existsResult && (chSel != QChar('\0'))) {

        QString charName ;
        QString theCharName = _appData->unicodeHelper()->nameForChar(chSel);
        if(!theCharName.isEmpty()) {
            charName = tr("<BR>Name:%1").arg(Utils::escapeHTML(theCharName));
        }
        QString res = tr("<html><body><b>%1</b><br/><br/>Position:%2 (%3)<br/>Unicode: 0x%4 %5</body></html>")
                      .arg(Utils::escapeHTML(chSel))
                      .arg(QString::number(index, 10).toUpper())
                      .arg(QString::number(index, 16).toUpper())
                      .arg(QString::number(chSel.unicode(), 16).toUpper())
                      .arg(charName);
        ui->codepageWidget->setCurrentCell(index / 16, index % 16, QItemSelectionModel::SelectCurrent);
        ui->searchResults->setText(res);
    } else {
        ui->searchResults->setText(tr("No results"));
    }
}


void CodePageDialog::on_searchChar_textEdited(const QString & /*text*/)
{
    search();
}

void CodePageDialog::on_searchUnicode_textEdited(const QString & /*text*/)
{
    search();
}

void CodePageDialog::on_radioSearchChar_clicked(bool)
{
    enableSearch();
    search();
    ui->searchChar->setFocus();
    ui->searchChar->setSelection(0, -1);
    ui->searchChar->setCursorPosition(0);
}

void CodePageDialog::on_radioSearchUnicode_clicked(bool)
{
    enableSearch();
    search();
    ui->searchUnicode->setFocus();
    ui->searchUnicode->setSelection(0, -1);
    ui->searchUnicode->setCursorPosition(0);
}

