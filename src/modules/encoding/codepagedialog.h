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


#ifndef CODEPAGEDIALOG_H
#define CODEPAGEDIALOG_H

#include <QDialog>
#include "libQXmlEdit_global.h"

#include "qxmleditdata.h"

namespace Ui
{
class CodePageDialog;
}

class LIBQXMLEDITSHARED_EXPORT CodePageDialog : public QDialog
{
    Q_OBJECT

    void setup();
    void repaint();
    void readOptions();
    void search();
    void enableSearch();

    QVector<QChar> _chars;
    QXmlEditData *_appData;
    bool _isAutoDelete;

public:
    explicit CodePageDialog(QXmlEditData *theData, QWidget *parent = 0);
    ~CodePageDialog();
    void setAutoDelete();

    static void showCodePage(QXmlEditData *theData, QWidget *parent);
    static void showCodePageWindow(QXmlEditData *theData, QWidget *parent);

private:
    Ui::CodePageDialog *ui;

    void closeEvent(QCloseEvent * event);

private slots:
    void on_cmbEncodings_activated(const QString & text);
    void on_chkUTF8_clicked();
    void on_chkValue_clicked();
    void on_chkName_clicked();
    void on_searchChar_textEdited(const QString & text);
    void on_searchUnicode_textEdited(const QString & text);
    void on_radioSearchChar_clicked(bool);
    void on_radioSearchUnicode_clicked(bool);

};

#endif // CODEPAGEDIALOG_H
