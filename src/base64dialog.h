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


#ifndef BASE64DIALOG_H
#define BASE64DIALOG_H

#include <QDialog>
#include <QTextCodec>

namespace Ui
{
class Base64Dialog;
}

class Base64Dialog : public QDialog
{
    Q_OBJECT

    static const int InputSizeLimit = 1024 * 1024 ;

    QTextCodec *_currentCodec;
    bool _isConverting;
    QString _fileDataPath;

    void setupOther();

public:
    explicit Base64Dialog(QWidget *parent = 0);
    ~Base64Dialog();

    static void showDialog();
    void loadFromBinaryFile(const QString &filePath);
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
protected:
    void loadTextFile(const QString &filePath);

private:
    Ui::Base64Dialog *ui;

private slots:
    void base64textChanged();
    void textChanged();
    void onCurrentEncodingChanged(int newIndex);
    void on_cmdLoadFromFile_clicked();
    void on_cmdLoadFromTextFile_clicked();
#ifdef  QXMLEDIT_TEST
    friend class TestBase64;
#endif
};

#endif // BASE64DIALOG_H
