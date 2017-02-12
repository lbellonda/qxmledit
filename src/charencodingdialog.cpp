/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2016 by Luca Bellonda and individual contributors  *
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

#include "charencodingdialog.h"
#include "ui_charencodingdialog.h"
#include <QBuffer>
#include <QTextStream>
#include "appconfig.h"
#include "modules/encoding/codepagedialog.h"
#include "qxmleditdata.h"
#include "modules/encoding/unicodehelper.h"

#include "utils.h"

#define UTF8_CODEC  "UTF-8"
#define WIN_1250_CODEC  "windows-1250"
#define WIN_1252_CODEC  "windows-1252"
#define WIN_1251_CODEC  "windows-1251"
#define ISO8859_15_CODEC  "ISO-8859-15"

CharEncodingDialog::CharEncodingDialog(QXmlEditData *theAppData, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CharEncodingDialog)
{
    _isAutoDelete = false;
    _inSetup = true;
    _appData = theAppData ;
    _enc1 = NULL;
    _enc2 = NULL;
    _enc3 = NULL;
    ui->setupUi(this);
    initialSetup();
    if(0 == _usedEncodingNames.size()) {
        if(Utils::askYN(this, tr("No encodings have been configured. Do you want add automatically some common encoding?"))) {
            addEncoding(UTF8_CODEC);
            addEncoding(WIN_1252_CODEC);
            addEncoding(WIN_1250_CODEC);
            addEncoding(WIN_1251_CODEC);
            addEncoding(ISO8859_15_CODEC);
            makeEncodingLabels();
            saveConfiguration();
        }
    }
    _inSetup = false ;
    on_enc1_currentIndexChanged(ui->enc1->currentIndex());
    on_enc2_currentIndexChanged(ui->enc2->currentIndex());
    on_enc3_currentIndexChanged(ui->enc3->currentIndex());
    textChanged();
}

CharEncodingDialog::~CharEncodingDialog()
{
    delete ui;
}

void CharEncodingDialog::setAutoDelete()
{
    _isAutoDelete = true ;
}

void CharEncodingDialog::closeEvent(QCloseEvent * event)
{
    event->accept();
    if(_isAutoDelete) {
        deleteLater();
    }
}

void CharEncodingDialog::initEncodings()
{
    Utils::setupComboEncoding(ui->encodingCombo);
    QStringList usedEncodings ;
    Config::loadStringArray(AppConfig::KEY_ENCODING_USEDCODECS, usedEncodings);
    foreach(QString encodingName, usedEncodings) {
        addEncoding(encodingName);
    }
    makeEncodingLabels();
}

void CharEncodingDialog::initialSetup()
{
    //--------------------------------------
    _useHexNumbers = Config::getBool(AppConfig::KEY_ENCODING_USEHEXVALUES, true) ;
    _showNames = Config::getBool(Config::KEY_CODEPAGE_VISNAMES, true) ;
    ui->hexNumericValues->setCheckState(_useHexNumbers ? Qt::Checked : Qt::Unchecked);
    ui->chkShowCharNames->setCheckState(_showNames ? Qt::Checked : Qt::Unchecked);
    //-----------------1 -------------------
    _textCharsCount = 0 ;
    initEncodings();
    connect(ui->sourceText, SIGNAL(textChanged()), this, SLOT(textChanged()));
    //---2 -----------------------------------
    Utils::setupComboEncoding(ui->enc1);
    Utils::setupComboEncoding(ui->enc2);
    Utils::setupComboEncoding(ui->enc3);
    Utils::selectComboValue(ui->enc1, Config::getInt(AppConfig::KEY_ENCODING_TRADENC1, -1));
    Utils::selectComboValue(ui->enc2, Config::getInt(AppConfig::KEY_ENCODING_TRADENC2, -1));
    Utils::selectComboValue(ui->enc3, Config::getInt(AppConfig::KEY_ENCODING_TRADENC3, -1));
    //--------------------------------------
    initCharDetails();
    //--------------------------------------
    ui->comboExport->clear();
    ui->comboExport->addItem(tr("Export to clipboard"), ExportTitle);
    ui->comboExport->addItem(tr("As UTF-8 bytes"), ExportUtf8Bytes);
    ui->comboExport->addItem(tr("As \"C\" UTF-8 bytes"), ExportUtf8CBytes);
    ui->comboExport->addItem(tr("As \"Java\" UTF-8 charactes"), ExportUtf8JavaChars);
}

void CharEncodingDialog::initCharDetails()
{
    _charButtonGroup.addButton(ui->radioCharByChar, CharByChar);
    _charButtonGroup.addButton(ui->radioCharByEncoding, CharByEncoding);
    _charButtonGroup.addButton(ui->radioCharByUnicode, CharByUnicode);
    _charButtonGroup.setExclusive(true);
    ui->charSelected->setFont(_charFont);
    Utils::setupComboEncoding(ui->comboCharEncodings);
    QString family = Config::getString(AppConfig::KEY_ENCODING_FONTFAMILY, "");
    if(!family.isEmpty()) {
        QFont::Style style = (QFont::Style)Config::getInt(AppConfig::KEY_ENCODING_FONTSTYLE, QFont::StyleNormal);
        int weight = Config::getInt(AppConfig::KEY_ENCODING_FONTWHEIGHT, QFont::Normal);
        qreal ptSize = Config::getReal(AppConfig::KEY_ENCODING_FONTPTSIZE, 10);
        QFont font;
        font.setFamily(family);
        font.setStyle(style);
        font.setWeight(weight);
        font.setPointSizeF(ptSize);
        _charFont = font ;
    }
    charSetFontTitle();
    characterSelect();
}

void CharEncodingDialog::saveConfiguration()
{
    // gets the codec names
    QStringList usedEncodings = _usedEncodingNames.toList();
    Config::saveStringArray(AppConfig::KEY_ENCODING_USEDCODECS, usedEncodings);
}

void CharEncodingDialog::addEncoding(const QString &name)
{
    if(_usedEncodingNames.contains(name)) {
        Utils::error(tr("The selected encoding is already present"));
        return ;
    }
    QTextCodec *codec = QTextCodec::codecForName(name.toLatin1());
    if(NULL != codec) {
        _usedEncodingNames.insert(name);
        _encoders.append(codec);
        int nowRows = ui->encodedTextGrid->rowCount();
        ui->encodedTextGrid->setRowCount(nowRows + 1);
        insertEncodedRow(nowRows, codec, ui->sourceText->toPlainText());
        makeEncodingLabels();
    } else {
        Utils::error(tr("The codec '%1' cannot be found").arg(name));
    }
}

void CharEncodingDialog::deleteRows()
{
    ui->encodedTextGrid->setRowCount(0);
}

void CharEncodingDialog::insertEncodedRow(const int row, QTextCodec *codec, const QString &text)
{
    _textCharsCount = text.length();
    ui->encodedTextGrid->setColumnCount(_textCharsCount);
    for(int i = 0 ; i < _textCharsCount ; i ++) {
        QChar srcChar = text.at(i);
        QString itemText ;
        if(!codec->canEncode(srcChar)) {
            itemText = tr("fail");
        } else {
            QByteArray encodedString = codec->fromUnicode(srcChar);
            int lenOfStr = encodedString.length();
            itemText = QString("%1: ").arg(lenOfStr);
            for(int j = 0 ; j < lenOfStr ; j++) {
                char charValue = encodedString.at(j);
                QString unicodeString = Utils::fmtNumber(_useHexNumbers, charValue, 2);
                if(j > 0) {
                    itemText += ", ";
                }
                itemText += unicodeString;
            }
        }
        QTableWidgetItem *itemUnicode = new QTableWidgetItem(itemText);
        setTItemAttrib(itemUnicode, codec->name());
        ui->encodedTextGrid->setItem(row, i, itemUnicode);
    }
}

void CharEncodingDialog::setTItemAttrib(QTableWidgetItem *item, const QString &data)
{
    item->setFlags(item->flags() & (~Qt::ItemIsEditable));
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item->setData(Qt::UserRole, data);
}

void CharEncodingDialog::insertText(const QString &text)
{
    _textCharsCount = text.length();
    ui->encodedTextGrid->setColumnCount(_textCharsCount);
    for(int i = 0 ; i < _textCharsCount ; i ++) {
        QChar theChar = text.at(i);
        // 0: the literal
        QString theTextOfItem = QString("%1").arg(theChar);
        if(_showNames) {
            QString name = _appData->unicodeHelper()->nameForChar(theChar);
            if(!name.isEmpty()) {
                theTextOfItem += "\n";
                theTextOfItem += name ;
            }
        }

        QTableWidgetItem *itemText = new QTableWidgetItem(theTextOfItem);
        {
            QString tooltipText = QString("%1").arg(theChar);
            QString name = _appData->unicodeHelper()->nameForChar(theChar);
            if(!name.isEmpty()) {
                tooltipText += " (";
                tooltipText += name ;
                tooltipText += " )";
            }
            itemText->setToolTip(tooltipText);
        }

        setTItemAttrib(itemText, "");
        ui->encodedTextGrid->setItem(0, i, itemText);
        // 1: the unicode value
        ushort unicodeValue = theChar.unicode();
        QString unicodeString = Utils::fmtNumber(_useHexNumbers, unicodeValue, 4);
        QTableWidgetItem *itemUnicode = new QTableWidgetItem(unicodeString);
        setTItemAttrib(itemUnicode, "");
        ui->encodedTextGrid->setItem(1, i, itemUnicode);
    }
    ui->encodedTextGrid->resizeRowToContents(0);
}

void CharEncodingDialog::textChanged()
{
    ui->encodedTextGrid->setUpdatesEnabled(false);
    deleteRows();
    ui->encodedTextGrid->setRowCount(2 + _encoders.size());
    QString text = ui->sourceText->toPlainText();
    insertText(text);
    int index = 2 ;
    foreach(QTextCodec * codec, _encoders) {
        insertEncodedRow(index, codec, text);
        index ++;
    }
    makeEncodingLabels();
    ui->encodedTextGrid->setUpdatesEnabled(true);
    redisplayChain();
}

void CharEncodingDialog::makeEncodingLabelsInternal(QTableWidget *grid, const bool insertSource)
{
    QStringList headerLabels;
    if(insertSource) {
        headerLabels.append(tr("Source text"));
        headerLabels.append(tr("Unicode value"));
    }
    foreach(QTextCodec * codec, _encoders) {
        headerLabels.append(codec->name());
    }
    grid->setVerticalHeaderLabels(headerLabels);
}

void CharEncodingDialog::makeEncodingLabels()
{
    makeEncodingLabelsInternal(ui->encodedTextGrid, true);
}

void CharEncodingDialog::on_hexNumericValues_stateChanged(int state)
{
    _useHexNumbers = (state == Qt::Checked) ? true : false;
    Config::saveBool(AppConfig::KEY_ENCODING_USEHEXVALUES, _useHexNumbers);
    updateAllViews();
}

void CharEncodingDialog::on_chkShowCharNames_stateChanged(int state)
{
    _showNames = (state == Qt::Checked) ? true : false;
    Config::saveBool(Config::KEY_CODEPAGE_VISNAMES, _showNames);
    updateAllViews();
}


void CharEncodingDialog::updateAllViews()
{
    textChanged();
}

void CharEncodingDialog::on_addEncodingCmd_clicked()
{
    int newIndex = ui->encodingCombo->currentIndex();
    if(newIndex >= 0) {
        QVariant data = ui->encodingCombo->itemData(newIndex);
        int mib = data.toInt();
        QTextCodec *currentCodec = QTextCodec::codecForMib(mib);
        addEncoding(currentCodec->name());
        saveConfiguration();
    }
}

void CharEncodingDialog::on_removeEncodingCmd_clicked()
{
    bool isOk = false;
    int rowIndex = ui->encodedTextGrid->currentRow();
    if(rowIndex >= 0) {
        QTableWidgetItem *item = ui->encodedTextGrid->item(rowIndex, 0);
        if(NULL != item) {
            QVariant data = item->data(Qt::UserRole);
            QString name = data.toString();
            if(!name.isEmpty()) {
                if(_usedEncodingNames.contains(name)) {
                    _usedEncodingNames.remove(name);
                    // remove encoding
                    foreach(QTextCodec * codec, _encoders) {
                        if(codec->name() == name) {
                            _encoders.removeAll(codec);
                            ui->encodedTextGrid->removeRow(rowIndex);
                            isOk = true ;
                            saveConfiguration();
                        } //if
                    } // foreach
                }
            } else {
                // this row is not really an encoding
                isOk = true;
            }
        }
    }
    if(!isOk) {
        Utils::error(this, tr("Error removing the selected encoding."));
    }
}

void CharEncodingDialog::insertChainEncodedRow(QTableWidget *table, QTextCodec *codec, QByteArray inputData, const QString &text)
{
    table->setRowCount(2);
    QStringList headerLabels;
    headerLabels.append(tr("Source text"));
    headerLabels.append(tr("Binary Values"));
    table->setVerticalHeaderLabels(headerLabels);

    int textLength = inputData.length();
    table->setColumnCount(1);
    QString binaryText ;
    for(int i = 0 ; i < textLength ; i ++) {
        char srcChar = inputData.at(i);
        if(i > 0) {
            binaryText += ", ";
        }
        binaryText += Utils::fmtNumber(_useHexNumbers, srcChar, 2);
    }

    QTableWidgetItem *itemText = new QTableWidgetItem(text);
    setTItemAttrib(itemText, text);
    table->setItem(0, 0, itemText);

    QTableWidgetItem *itemBinary = new QTableWidgetItem(binaryText);
    setTItemAttrib(itemBinary, codec->name());
    table->setItem(1, 0, itemBinary);
}

QByteArray CharEncodingDialog::doGrid(QTableWidget *grid, QTextCodec *codec, QByteArray source, const bool isFirst, const QString &src)
{
    if(NULL != codec) {
        QString translated ;
        if(isFirst) {
            translated = src ;
        } else {
            QTextStream inputBuffer(source, QIODevice::ReadOnly);
            inputBuffer.setCodec(codec);
            translated = inputBuffer.readAll();
        }

        QByteArray outArray;
        QTextStream outputStream(&outArray, QIODevice::WriteOnly);
        outputStream.setCodec(codec);
        outputStream << translated ;
        outputStream.flush();

        insertChainEncodedRow(grid, codec, outArray, translated);
        grid->resizeColumnsToContents();
        return outArray;
    } else {
        grid->setRowCount(0);
        return QByteArray();
    }
}


void CharEncodingDialog::redisplayChain()
{
    QByteArray inputData =  ui->sourceText->toPlainText().toUtf8();
    QByteArray a1 = doGrid(ui->grid1, _enc1, inputData, true, ui->sourceText->toPlainText());
    QByteArray a2 = doGrid(ui->grid2, _enc2, a1, false, "");
    doGrid(ui->grid3, _enc3, a2, false, "");
}

QTextCodec *CharEncodingDialog::encCurrentIndexChanged(int index, QComboBox *combo, const QString &key, QTextCodec** enc)
{
    if(index != -1) {
        QVariant data = combo->itemData(index);
        int mib = data.toInt();
        *enc = QTextCodec::codecForMib(mib);
        Config::saveInt(key, mib);
        redisplayChain();
        return *enc;
    }
    return NULL ;
}

void CharEncodingDialog::on_enc1_currentIndexChanged(int index)
{
    if(_inSetup) {
        return ;
    }
    encCurrentIndexChanged(index, ui->enc1, AppConfig::KEY_ENCODING_TRADENC1, &_enc1);
}

void CharEncodingDialog::on_enc2_currentIndexChanged(int index)
{
    if(_inSetup) {
        return ;
    }
    encCurrentIndexChanged(index, ui->enc2, AppConfig::KEY_ENCODING_TRADENC2, &_enc2);
}

void CharEncodingDialog::on_enc3_currentIndexChanged(int index)
{
    if(_inSetup) {
        return ;
    }
    encCurrentIndexChanged(index, ui->enc3, AppConfig::KEY_ENCODING_TRADENC3, &_enc3);
}

void CharEncodingDialog::on_numericEdit_textChanged()
{
    if(_inSetup) {
        return ;
    }
    QString entry = ui->numericEdit->text();
    entry = entry.replace(",", " ");
    QStringList entries = entry.split(" ", QString::SkipEmptyParts);
    QByteArray binaryData;
    foreach(QString anEntry, entries) {
        bool ok = false ;
        int value = anEntry.toShort(&ok, 0);
        if(ok) {
            value &= 0x00FF ;
            binaryData.append((char)value);
        } else {
            binaryData.append('?');
        }
    }
    // display encodings
    ui->charExplorerGrid->setUpdatesEnabled(false);
    ui->charExplorerGrid->setColumnCount(1);
    ui->charExplorerGrid->setRowCount(0);
    ui->charExplorerGrid->setRowCount(_encoders.size());
    int index = 0 ;
    foreach(QTextCodec * codec, _encoders) {
        insertEncodedRowValue(index, codec, binaryData);
        index ++;
    }
    makeEncodingLabelsInternal(ui->charExplorerGrid, false);
    ui->charExplorerGrid->setUpdatesEnabled(true);
}

void CharEncodingDialog::insertEncodedRowValue(const int row, QTextCodec *codec, const QByteArray &source)
{
    QString translated = codec->toUnicode(source.data());
    QTableWidgetItem *itemText = new QTableWidgetItem(translated);
    setTItemAttrib(itemText, "");
    itemText->setTextAlignment(Qt::AlignLeft);
    ui->charExplorerGrid->setItem(row, 0, itemText);
}


void CharEncodingDialog::on_cmdGenerateSequence_clicked()
{
    QString str ;
    int startPos = 128 ;
    if((QApplication::keyboardModifiers() & Qt::ControlModifier) ==  Qt::ControlModifier) {
        startPos = 32 ;
    }
    for(int i = startPos ; i < 256 ; i ++) {
        str += Utils::fmtNumber(_useHexNumbers, i, 2);
        str += " ";
    }
    ui->numericEdit->setText(str);
}

void CharEncodingDialog::on_cmdCodePages_clicked()
{
    CodePageDialog::showCodePage(_appData, this);
}

void CharEncodingDialog::characterSelect()
{
    bool existsChar = false;
    QChar selChar;
    switch(_charButtonGroup.checkedId()) {
    default:
        break;
    case CharByChar: {
        QString result = ui->charCharEntry->text();//trimmed();
        if(!result.isEmpty()) {
            selChar = result.at(0);
            existsChar = true;
        }
    }
    break;
    case CharByUnicode: {
        bool isOk = false;
        QString valString = ui->charUnicodeValue->text().trimmed();
        int value = valString.toInt(&isOk, 0);
        if(isOk) {
            existsChar = true ;
            selChar = QChar(value);
        }
    }
    break;
    case CharByEncoding: {
        QString codecName = ui->comboCharEncodings->currentText();
        QTextCodec *codec = QTextCodec::codecForName(codecName.toLatin1());
        if(NULL != codec) {
            // find the character by the value
            QString entry = ui->charEncodingValue->text().trimmed();
            entry = entry.replace(",", " ");
            QStringList entries = entry.split(" ", QString::SkipEmptyParts);
            QByteArray binaryData;
            foreach(QString anEntry, entries) {
                bool ok = false ;
                int value = anEntry.toShort(&ok, 0);
                if(ok) {
                    value &= 0x00FF ;
                    binaryData.append((char)value);
                } else {
                    binaryData.append('?');
                }
            }
            QString translated = codec->toUnicode(binaryData.data());
            if(translated.length() > 0) {
                existsChar = true ;
                selChar = QChar(translated.at(0));
            }
        }
    }
    break;
    }

    if(existsChar) {
        setCharDetails(selChar);
    } else {
        setCharDetailsInfoSel(tr("<No match>"));
    }
    ui->charDetails->setVisible(existsChar);
}

void CharEncodingDialog::on_charCharEntry_textEdited(const QString & /*text*/)
{
    ui->radioCharByChar->setChecked(true);
}

void CharEncodingDialog::on_charUnicodeValue_textEdited(const QString & /*text*/)
{
    ui->radioCharByUnicode->setChecked(true);
}

void CharEncodingDialog::on_charEncodingValue_textEdited(const QString & /*text*/)
{
    ui->radioCharByEncoding->setChecked(true);
}

void CharEncodingDialog::on_comboCharEncodings_activated(int /*index*/)
{
    ui->radioCharByEncoding->setChecked(true);
}

void CharEncodingDialog::on_cmdCharFont_clicked()
{
    bool isOk = false ;
    QFont newFont = QFontDialog::getFont(&isOk, _charFont, this);
    if(isOk) {
        _charFont = newFont ;
        ui->charSelected->setFont(_charFont);
        charSetFontTitle();
        Config::saveString(AppConfig::KEY_ENCODING_FONTFAMILY, _charFont.family());
        Config::saveInt(AppConfig::KEY_ENCODING_FONTSTYLE, _charFont.style());
        Config::saveInt(AppConfig::KEY_ENCODING_FONTWHEIGHT, _charFont.weight());
        Config::saveReal(AppConfig::KEY_ENCODING_FONTPTSIZE, _charFont.pointSizeF());
    }
}

void CharEncodingDialog::charSetFontTitle()
{
    QString title = tr("Change Font (%1 %2 %3)").arg(_charFont.family()).arg(_charFont.bold() ? tr("bold") : "").arg(_charFont.pointSizeF());
    ui->cmdCharFont->setText(title);
}

void CharEncodingDialog::setCharDetailsInfoSel(const QString &ch)
{
    QString label = QString(" %1 ").arg(ch);
    ui->charSelected->setText(label);
}

void CharEncodingDialog::setCharDetails(const QChar &ch)
{
    setCharDetailsInfoSel(ch);

    QStringList labels;
    ui->charDetails->reset();
    ui->charDetails->setRowCount(0);
    ui->charDetails->setColumnCount(1);
    //----
    QString name = _appData->unicodeHelper()->nameForChar(ch);
    charDetailInsertRow(tr("Name"), name, labels);
    //---
    ushort unicodeValue = ch.unicode();
    QString unicodeString = Utils::fmtNumber(_useHexNumbers, unicodeValue, 4);
    charDetailInsertRow(tr("Unicode Code Point"), unicodeString, labels);
    //--
    QString cat = charCategory(ch);
    charDetailInsertRow(tr("Unicode category"), cat, labels);
    //--
    QString decomp = charDecomposition(ch);
    charDetailInsertRowNotNull(tr("Decomposition"), decomp, labels);
    //--
    charDetailInsertRowBool(ch.hasMirrored(), "", tr("hasMirrored"), labels);
    charDetailInsertRowBool(ch.isDigit(), "", tr("isDigit"), labels);
    charDetailInsertRowBool(ch.isHighSurrogate(), "", tr("isHighSurrogate"), labels);
    charDetailInsertRowBool(ch.isLetter(), "", tr("isLetter"), labels);
    charDetailInsertRowBool(ch.isLetterOrNumber(), "", tr("isLetterOrNumber"), labels);
    charDetailInsertRowBool(ch.isLowSurrogate(), "", tr("isLowSurrogate"), labels);
    charDetailInsertRowBool(ch.isLower(), "", tr("isLower"), labels);
    charDetailInsertRowBool(ch.isMark(), "", tr("isMark"), labels);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
    charDetailInsertRowBool(ch.isNonCharacter(), "", tr("isNonCharacter"), labels);
#endif
    charDetailInsertRowBool(ch.isNull(), "", tr("isNull"), labels);
    charDetailInsertRowBool(ch.isNumber(), "", tr("isNumber"), labels);
    charDetailInsertRowBool(ch.isPrint(), "", tr("isPrint"), labels);
    charDetailInsertRowBool(ch.isPunct(), "", tr("isPunct"), labels);
    charDetailInsertRowBool(ch.isSpace(), "", tr("isSpace"), labels);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
    charDetailInsertRowBool(ch.isSurrogate(), "", tr("isSurrogate"), labels);
#endif
    charDetailInsertRowBool(ch.isSymbol(), "", tr("isSymbol"), labels);
    charDetailInsertRowBool(ch.isTitleCase(), "", tr("isTitleCase"), labels);
    charDetailInsertRowBool(ch.isUpper(), "", tr("isUpper"), labels);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
    charDetailInsertRow(tr("joining"), findJoining(ch), labels);
    charDetailInsertRow(tr("Unicode script"), findScript(ch), labels);
#endif
    //--
    foreach(QTextCodec * codec, _encoders) {
        QString encodedText ;
        if(codec->canEncode(ch)) {
            QByteArray encodedString = codec->fromUnicode(ch);
            int lenOfStr = encodedString.length();
            encodedText = QString("%1: ").arg(lenOfStr);
            for(int j = 0 ; j < lenOfStr ; j++) {
                char charValue = encodedString.at(j);
                QString unicodeString = Utils::fmtNumber(_useHexNumbers, charValue, 2);
                if(j > 0) {
                    encodedText += ", ";
                }
                encodedText += unicodeString;
            }
        }
        charDetailInsertRow(codec->name(), encodedText, labels);
    }
    //---
    ui->charDetails->setVerticalHeaderLabels(labels);
    ui->charDetails->resizeColumnsToContents();
}

void CharEncodingDialog::charDetailInsertRowNotNull(const QString &rowLabel, const QString &itemText, QStringList &labels)
{
    if(!itemText.isEmpty()) {
        charDetailInsertRow(rowLabel, itemText, labels);
    }
}

void CharEncodingDialog::charDetailInsertRowBool(const bool condition, const QString &rowLabel, const QString &itemText, QStringList &labels)
{
    if(condition) {
        charDetailInsertRow(rowLabel, itemText, labels);
    }
}
void CharEncodingDialog::charDetailInsertRow(const QString &rowLabel, const QString &itemText, QStringList &labels)
{
    int nowRows = ui->charDetails->rowCount();
    ui->charDetails->setRowCount(nowRows + 1);
    labels.append(rowLabel);
    QTableWidgetItem *item = new QTableWidgetItem(itemText);
    setTItemAttrib(item, itemText);
    ui->charDetails->setItem(nowRows, 0, item);
}

void CharEncodingDialog::on_cmdCharSearch_clicked()
{
    characterSelect();
}

QString CharEncodingDialog::charCategory(const QChar &ch)
{
    QString res;
    bool found = true;
    switch(ch.category()) {
    default:
        found = false;
        break;
    case QChar::Mark_NonSpacing: res = "Mark_NonSpacing ( normative Mn)"; break;
    case QChar::Mark_SpacingCombining: res = "Mark_SpacingCombining ( normative Mc)"; break;
    case QChar::Mark_Enclosing: res = "Mark_Enclosing ( normative Me)"; break;
    case QChar::Number_DecimalDigit: res = "Number_DecimalDigit ( normative Nd)"; break;
    case QChar::Number_Letter: res = "Number_Letter ( normative Nl)"; break;
    case QChar::Number_Other: res = "Number_Other ( normative No)"; break;
    case QChar::Separator_Space: res = "Separator_Space ( normative Zs)"; break;
    case QChar::Separator_Line: res = "Separator_Line ( normative Zl)"; break;
    case QChar::Separator_Paragraph: res = "Separator_Paragraph ( normative Zp)"; break;
    case QChar::Other_Control: res = "Other_Control ( normative Cc)"; break;
    case QChar::Other_Format: res = "Other_Format ( normative Cf)"; break;
    case QChar::Other_Surrogate: res = "Other_Surrogate ( normative Cs)"; break;
    case QChar::Other_PrivateUse: res = "Other_PrivateUse ( normative Co)"; break;
    case QChar::Other_NotAssigned: res = "Other_NotAssigned ( normative Cn)"; break;
    case QChar::Letter_Uppercase: res = "Letter_Uppercase ( informative Lu)"; break;
    case QChar::Letter_Lowercase: res = "Letter_Lowercase ( informative Ll)"; break;
    case QChar::Letter_Titlecase: res = "Letter_Titlecase ( informative Lt)"; break;
    case QChar::Letter_Modifier: res = "Letter_Modifier ( informative Lm)"; break;
    case QChar::Letter_Other: res = "Letter_Other ( informative Lo)"; break;
    case QChar::Punctuation_Connector: res = "Punctuation_Connector ( informative Pc)"; break;
    case QChar::Punctuation_Dash: res = "Punctuation_Dash ( informative Pd)"; break;
    case QChar::Punctuation_Open: res = "Punctuation_Open ( informative Ps)"; break;
    case QChar::Punctuation_Close: res = "Punctuation_Close ( informative Pe)"; break;
    case QChar::Punctuation_InitialQuote: res = "Other_Control ( informative Pi)"; break;
    case QChar::Punctuation_FinalQuote: res = "Punctuation_InitialQuote ( informative Pf)"; break;
    case QChar::Punctuation_Other: res = "Punctuation_Other ( informative Po)"; break;
    case QChar::Symbol_Math: res = "Symbol_Math ( informative Sm)"; break;
    case QChar::Symbol_Currency: res = "Symbol_Currency ( informative Sc)"; break;
    case QChar::Symbol_Modifier: res = "Symbol_Modifier ( informative Sk)"; break;
    case QChar::Symbol_Other: res = "Symbol_Other ( informative So)"; break;
    }
    if(found) {
        return res;
    } else {
        return tr("<not found>");
    }
}

QString CharEncodingDialog::charDecomposition(const QChar &ch)
{
    switch(ch.category()) {
    default:
        return tr("<unknown>");
    case QChar::NoDecomposition: return "";
    case QChar::Canonical: return tr("Canonical");
    case QChar::Circle: return tr("Circle");
    case QChar::Compat: return tr("Compat");
    case QChar::Final: return tr("Final");
    case QChar::Font: return tr("Font");
    case QChar::Fraction: return tr("Fraction");
    case QChar::Initial: return tr("Initial");
    case QChar::Isolated: return tr("Isolated");
    case QChar::Medial: return tr("Medial");
    case QChar::Narrow: return tr("Narrow");
    case QChar::NoBreak: return tr("NoBreak");
    case QChar::Small: return tr("Small");
    case QChar::Square: return tr("Square");
    case QChar::Sub: return tr("Sub");
    case QChar::Super: return tr("Super");
    case QChar::Vertical: return tr("Vertical");
    case QChar::Wide: return tr("Wide");
    }
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
QString CharEncodingDialog::findJoining(const QChar &ch)
{
    switch(ch.joiningType()) {
    default: return "" ;
    case QChar::Joining_None: return "None";
    case QChar::Joining_Causing: return "Causing";
    case QChar::Joining_Dual: return "Dual";
    case QChar::Joining_Right: return "Right";
    case QChar::Joining_Left: return "Left";
    case QChar::Joining_Transparent: return "Transparent";
    }
}


#define M_SCR(token) case QChar::Script_##token: return "" #token "" ;

QString CharEncodingDialog::findScript(const QChar &ch)
{
    switch(ch.script()) {
        M_SCR(Unknown)
        M_SCR(Inherited)
        M_SCR(Common)

        M_SCR(Latin)
        M_SCR(Greek)
        M_SCR(Cyrillic)
        M_SCR(Armenian)
        M_SCR(Hebrew)
        M_SCR(Arabic)
        M_SCR(Syriac)
        M_SCR(Thaana)
        M_SCR(Devanagari)
        M_SCR(Bengali)
        M_SCR(Gurmukhi)
        M_SCR(Gujarati)
        M_SCR(Oriya)
        M_SCR(Tamil)
        M_SCR(Telugu)
        M_SCR(Kannada)
        M_SCR(Malayalam)
        M_SCR(Sinhala)
        M_SCR(Thai)
        M_SCR(Lao)
        M_SCR(Tibetan)
        M_SCR(Myanmar)
        M_SCR(Georgian)
        M_SCR(Hangul)
        M_SCR(Ethiopic)
        M_SCR(Cherokee)
        M_SCR(CanadianAboriginal)
        M_SCR(Ogham)
        M_SCR(Runic)
        M_SCR(Khmer)
        M_SCR(Mongolian)
        M_SCR(Hiragana)
        M_SCR(Katakana)
        M_SCR(Bopomofo)
        M_SCR(Han)
        M_SCR(Yi)
        M_SCR(OldItalic)
        M_SCR(Gothic)
        M_SCR(Deseret)
        M_SCR(Tagalog)
        M_SCR(Hanunoo)
        M_SCR(Buhid)
        M_SCR(Tagbanwa)
        M_SCR(Coptic)

        // Unicode 4.0 additions
        M_SCR(Limbu)
        M_SCR(TaiLe)
        M_SCR(LinearB)
        M_SCR(Ugaritic)
        M_SCR(Shavian)
        M_SCR(Osmanya)
        M_SCR(Cypriot)
        M_SCR(Braille)

        // Unicode 4.1 additions
        M_SCR(Buginese)
        M_SCR(NewTaiLue)
        M_SCR(Glagolitic)
        M_SCR(Tifinagh)
        M_SCR(SylotiNagri)
        M_SCR(OldPersian)
        M_SCR(Kharoshthi)

        // Unicode 5.0 additions
        M_SCR(Balinese)
        M_SCR(Cuneiform)
        M_SCR(Phoenician)
        M_SCR(PhagsPa)
        M_SCR(Nko)

        // Unicode 5.1 additions
        M_SCR(Sundanese)
        M_SCR(Lepcha)
        M_SCR(OlChiki)
        M_SCR(Vai)
        M_SCR(Saurashtra)
        M_SCR(KayahLi)
        M_SCR(Rejang)
        M_SCR(Lycian)
        M_SCR(Carian)
        M_SCR(Lydian)
        M_SCR(Cham)

        // Unicode 5.2 additions
        M_SCR(TaiTham)
        M_SCR(TaiViet)
        M_SCR(Avestan)
        M_SCR(EgyptianHieroglyphs)
        M_SCR(Samaritan)
        M_SCR(Lisu)
        M_SCR(Bamum)
        M_SCR(Javanese)
        M_SCR(MeeteiMayek)
        M_SCR(ImperialAramaic)
        M_SCR(OldSouthArabian)
        M_SCR(InscriptionalParthian)
        M_SCR(InscriptionalPahlavi)
        M_SCR(OldTurkic)
        M_SCR(Kaithi)

        // Unicode 6.0 additions
        M_SCR(Batak)
        M_SCR(Brahmi)
        M_SCR(Mandaic)

        // Unicode 6.1 additions
        M_SCR(Chakma)
        M_SCR(MeroiticCursive)
        M_SCR(MeroiticHieroglyphs)
        M_SCR(Miao)
        M_SCR(Sharada)
        M_SCR(SoraSompeng)
        M_SCR(Takri)
    default:
        return "";

    }
}
#endif

void CharEncodingDialog::on_comboExport_activated(int index)
{
    if(index >= 0) {
        QString src = ui->sourceText->toPlainText();
        QVariant var = ui->comboExport->itemData(index);
        ExportType et = (ExportType)var.toInt();
        switch(et) {
        default:
        case ExportTitle:
            break;
        case ExportUtf8Bytes:
            exportToUtf8Bytes(src);
            break;
        case ExportUtf8CBytes:
            exportToUtf8CBytes(src);
            break;
        case ExportUtf8JavaChars:
            exportToJavaUnicodeChars(src);
            break;
        }
    }
    if(index != 0) {
        ui->comboExport->setCurrentIndex(0);
    }
}

void CharEncodingDialog::setClipboardText(const QString &dataString)
{
    bool ok = false;
    if(!dataString.isEmpty()) {
        QClipboard *clipboard = QApplication::clipboard();
        if(NULL != clipboard) {
            clipboard->setText(dataString);
            ok = true;
        }
    }
    if(!ok) {
        Utils::error(this, tr("Impossible copy to the clipboard."));
    }
}

void CharEncodingDialog::exportToUtf8CBytes(const QString &src)
{
    QByteArray inputData = src.toUtf8();
    QString str = "\"";
    int len = inputData.length() ;
    for(int i = 0 ; i < len ; i ++) {
        unsigned char ch = inputData.at(i);
        str += QString("\\x%1").arg(Utils::fmtNumber(true, ch, 2, false));
    }
    str += "\"";
    setClipboardText(str);
}

void CharEncodingDialog::exportToUtf8Bytes(const QString &src)
{
    QByteArray inputData = src.toUtf8();
    QString str ;
    int len = inputData.length() ;
    for(int i = 0 ; i < len ; i ++) {
        unsigned char ch = inputData.at(i);
        if(i != 0) {
            str += " ";
        }
        str += Utils::fmtNumber(true, ch, 2, true);
    }
    setClipboardText(str);
}

void CharEncodingDialog::exportToJavaUnicodeChars(const QString &src)
{
    QString inputData = src ;
    QString str = "\"";
    int len = inputData.length() ;
    for(int i = 0 ; i < len ; i ++) {
        QChar ch = inputData.at(i);

        ushort unicodeCode = ch.unicode();
        if(ch > 127) {
            str += "\\u";
            QString value = Utils::fmtNumber(true, unicodeCode, 4, false);
            str += value;
        } else {
            str += ch ;
        }
    }
    str += "\"";
    setClipboardText(str);
}

