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


#include "XSLTAutoModeDialogprivatetest.h"
#include "ui_xsltautomodedialog.h"
#include "qxmleditconfig.h"

XSLTAutoModeDialog::PrivateTest::Tests XSLTAutoModeDialog::PrivateTest::testToExecute()
{
    return (Tests)Config::getInt(Config::KEY_TEST+"/xslAutoPanel/testToExecute",-1);
}

void XSLTAutoModeDialog::PrivateTest::setTestToExecute(const Tests test)
{
    Config::saveInt(Config::KEY_TEST+"/xslAutoPanel/testToExecute",test);
}

bool XSLTAutoModeDialog::PrivateTest::testPassed()
{
    return Config::getBool(Config::KEY_TEST+"/xslAutoPanel/testPassed",false);
}

void XSLTAutoModeDialog::PrivateTest::setTestPassed(const bool passed)
{
    Config::saveBool(Config::KEY_TEST+"/xslAutoPanel/testPassed",passed);
}

QString XSLTAutoModeDialog::PrivateTest::testMessage()
{
    return Config::getString(Config::KEY_TEST+"/xslAutoPanel/testMessage","");
}

void XSLTAutoModeDialog::PrivateTest::setTestMessage(const QString &msg)
{
    Config::saveString(Config::KEY_TEST+"/xslAutoPanel/testMessage",msg);
}

bool XSLTAutoModeDialog::PrivateTest::panelOpen()
{
    return Config::getBool(Config::KEY_TEST+"/xslAutoPanel/panelOpen",false);
}

void XSLTAutoModeDialog::PrivateTest::setPanelOpen(const bool open)
{
    Config::saveBool(Config::KEY_TEST+"/xslAutoPanel/panelOpen",open);
}

bool XSLTAutoModeDialog::PrivateTest::answerShowXSLTPanel()
{
    return Config::getBool(Config::KEY_TEST+"/xslAutoPanel/answerShow",false);
}

void XSLTAutoModeDialog::PrivateTest::setAnswerShowXSLTPanel(const bool how)
{
    Config::saveBool(Config::KEY_TEST+"/xslAutoPanel/answerShow",how);
}

XSLTAutoModeDialog::ERetCode XSLTAutoModeDialog::PrivateTest::answer()
{
    return (XSLTAutoModeDialog::ERetCode)Config::getInt(Config::KEY_TEST+"/xslAutoPanel/answer",XSLTAutoModeDialog::ENTER_XSLMODE);
}

void XSLTAutoModeDialog::PrivateTest::setAnswer(const XSLTAutoModeDialog::ERetCode how)
{
    Config::saveInt(Config::KEY_TEST+"/xslAutoPanel/answer",(int)how);
}


//---------------------------------------------------------

XSLTAutoModeDialog::PrivateTest::PrivateTest(XSLTAutoModeDialog *dlg) :
    QObject(dlg)
{
    _dlg = dlg;
}

XSLTAutoModeDialog::PrivateTest::~PrivateTest()
{
}

void XSLTAutoModeDialog::PrivateTest::testStart()
{
    setPanelOpen(true);
    setTestPassed(false);
    setTestMessage("");
    switch(testToExecute()) {
    case TEST_SHOWPANEL_XML_FILE: // show panel
        setTestPassed(false);
        setTestMessage("panel open");
        break;
    case TEST_SHOWPANEL_XSL_FILE: // show panel
        setTestPassed(true);
        setTestMessage("panel open");
        break;
     //---
    case TEST_SHOW_NOHIDE_NOAUTO:
    case TEST_SHOW_HIDE_NOAUTO:
    case TEST_SHOW_NOHIDE_AUTO:
    case TEST_SHOW_HIDE_AUTO:
        setTestPassed(true);
        setTestMessage("panel open");
        break;
    case TEST_BEHAVIOUR:
        // check show
        _dlg->ui->cbDoNotShow->setChecked(!answerShowXSLTPanel());
        // use the appropriate answer
        setTestPassed(true);
        switch(answer()) {
        case ENTER_XSLMODE:
            _dlg->on_cmdYes_clicked();
            break;
        case DONOTENTER_XSLMODE:
            _dlg->on_cmdNo_clicked();
            break;
        case ALWAYS_XSLMODE:
            _dlg->on_cmdAlways_clicked();
            break;
        default:
            setTestPassed(false);
            setTestMessage(QString("unknown answer %1").arg(answerShowXSLTPanel()));
            break;
        }
        return ;

    default:
        break;
    }
    _dlg->accept();
}


