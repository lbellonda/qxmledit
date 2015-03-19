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


#ifndef CHARENCODINGDIALOG_H
#define CHARENCODINGDIALOG_H

#include <QDialog>
#include <QSet>
#include <QComboBox>
#include <QTextCodec>
#include <QTableWidgetItem>
#include <QButtonGroup>

namespace Ui
{
class CharEncodingDialog;
}


class QXmlEditData;

class CharEncodingDialog : public QDialog
{
    Q_OBJECT


    enum ExportType {
        ExportTitle,
        ExportUtf8Bytes,
        ExportUtf8CBytes,
        ExportUtf8JavaChars
    };

    enum ECharSearch {
        CharByChar = 0,
        CharByUnicode,
        CharByEncoding
    };

    bool _isAutoDelete ;
    bool _inSetup;
    //---------------------------------------------------
    bool _useHexNumbers;
    bool _showNames;
    //--------- tab string ------------------------------
    QSet<QString> _usedEncodingNames;
    QList<QTextCodec*> _encoders;
    int _textCharsCount;
    //---------------------------------------------------
    QTextCodec  *_enc1;
    QTextCodec  *_enc2;
    QTextCodec  *_enc3;
    //-------
    QButtonGroup _charButtonGroup;
    QFont _charFont;
    //-------
    QXmlEditData *_appData;


public:
    explicit CharEncodingDialog(QXmlEditData *theAppData, QWidget *parent = 0);
    ~CharEncodingDialog();
    void setAutoDelete();

private:
    Ui::CharEncodingDialog *ui;

    void closeEvent(QCloseEvent * event);
    void addEncoding(const QString &name);
    void saveConfiguration();
    void initEncodings();
    void initialSetup();
    void insertEncodedRow(const int row, QTextCodec *codec, const QString &text);
    void deleteRows();
    void insertText(const QString &text);
    void makeEncodingLabels();
    void makeEncodingLabelsInternal(QTableWidget *grid, const bool insertSource);
    void updateAllViews();
    void setTItemAttrib(QTableWidgetItem *item, const QString &data);
    void redisplayChain();
    QByteArray doGrid(QTableWidget *grid, QTextCodec *codec, QByteArray source, const bool isFirst, const QString &src);
    void insertChainEncodedRow(QTableWidget *table, QTextCodec *codec, QByteArray inputData, const QString &text);
    QTextCodec *encCurrentIndexChanged(int index, QComboBox *combo, const QString &key, QTextCodec** enc);
    void insertEncodedRowValue(const int row, QTextCodec *codec, const QByteArray &source);
    void characterSelect();
    void initCharDetails();
    void charSetFontTitle();
    void charDetailInsertRow(const QString &rowLabel, const QString &itemText, QStringList &labels);
    void charDetailInsertRowNotNull(const QString &rowLabel, const QString &itemText, QStringList &labels);
    void charDetailInsertRowBool(const bool condition, const QString &rowLabel, const QString &itemText, QStringList &labels);
    void setCharDetails(const QChar &ch);
    void setCharDetailsInfoSel(const QString &ch);
    QString charCategory(const QChar &ch);
    QString charDecomposition(const QChar &ch);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
    QString findJoining(const QChar &ch);
#endif
    QString findScript(const QChar &ch);
    void setClipboardText(const QString &dataString);
    void exportToUtf8CBytes(const QString &src);
    void exportToUtf8Bytes(const QString &src);
    void exportToJavaUnicodeChars(const QString &src);

private slots:
    void textChanged();
    void on_hexNumericValues_stateChanged(int state);
    void on_chkShowCharNames_stateChanged(int state);
    void on_addEncodingCmd_clicked();
    void on_removeEncodingCmd_clicked();
    void on_enc1_currentIndexChanged(int index);
    void on_enc2_currentIndexChanged(int index);
    void on_enc3_currentIndexChanged(int index);

    void on_numericEdit_textChanged();
    void on_cmdGenerateSequence_clicked();
    void on_cmdCodePages_clicked();
    void on_cmdCharFont_clicked();
    void on_cmdCharSearch_clicked();
    void on_charCharEntry_textEdited(const QString & text);
    void on_charUnicodeValue_textEdited(const QString & text);
    void on_charEncodingValue_textEdited(const QString & text);
    void on_comboCharEncodings_activated(int index);
    void on_comboExport_activated(int index);

    friend class TestEncoding;
};

#endif // CHARENCODINGDIALOG_H
