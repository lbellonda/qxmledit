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

#ifndef SCXMLAUTOMODEDIALOG_H
#define SCXMLAUTOMODEDIALOG_H

#include "xmlEdit.h"
#include "libQXmlEdit_global.h"
#include "qxmleditdata.h"


namespace Ui
{
class SCXMLAutoModeDialog;
}

class LIBQXMLEDITSHARED_EXPORT SCXMLAutoModeDialog : public QDialog
{
    Q_OBJECT
public:
    enum ERetCode {
        ENTER_SCXMLMODE,
        DONOTENTER_SCXMLMODE,
        ALWAYS_SCXMLMODE
    };
#ifdef QXMLEDIT_TEST
    class PrivateTest;
    PrivateTest *d;

#endif
    ERetCode _result;
    bool _enterSCXML;
    QXmlEditData *_data;

    void setAnswer(const ERetCode retCode, const bool enterSCXML);

public:
    explicit SCXMLAutoModeDialog(QWidget *parent, QXmlEditData *data);
    ~SCXMLAutoModeDialog();

    bool resultSCXML();

    static bool showToUser(QWidget *parent, QXmlEditData *data);

private:
    Ui::SCXMLAutoModeDialog *ui;
private slots:
    void on_cmdYes_clicked();
    void on_cmdNo_clicked();
    void on_cmdAlways_clicked();
};

#endif // SCXMLAUTOMODEDIALOG_H
