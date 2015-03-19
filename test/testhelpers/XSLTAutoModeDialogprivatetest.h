/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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


#ifndef XSLTAUTOMODEDIALOG_XSLTAUTOMODEDIALOGPRIVATETEST_H
#define XSLTAUTOMODEDIALOG_XSLTAUTOMODEDIALOGPRIVATETEST_H

#include <QObject>
#include "modules/xslt/xsltautomodedialog.h"

class XSLTAutoModeDialog::PrivateTest : public QObject
{
    Q_OBJECT
    XSLTAutoModeDialog *_dlg;

public:

    enum Tests {
        TEST_SHOWPANEL_XML_FILE,
        TEST_SHOWPANEL_XSL_FILE,
        //---
        TEST_SHOW_NOHIDE_NOAUTO,
        TEST_SHOW_HIDE_NOAUTO,
        TEST_SHOW_NOHIDE_AUTO,
        TEST_SHOW_HIDE_AUTO,
        //
        TEST_BEHAVIOUR
    };

    explicit PrivateTest(XSLTAutoModeDialog *dlg);
    ~PrivateTest();

    //--------------------------------------------
    static Tests testToExecute();
    static void setTestToExecute(const Tests test);
    static bool testPassed();
    static void setTestPassed(const bool passed);
    static QString testMessage();
    static void setTestMessage(const QString &msg);
    static bool panelOpen();
    static void setPanelOpen(const bool open);
    static bool answerShowXSLTPanel();
    static void setAnswerShowXSLTPanel(const bool how);
    static XSLTAutoModeDialog::ERetCode answer();
    static void setAnswer(const XSLTAutoModeDialog::ERetCode how);

    //--------------------------------------------

signals:
    
public slots:
    void testStart();
};

#endif // XSLTAUTOMODEDIALOG_XSLTAUTOMODEDIALOGPRIVATETEST_H
