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

#include "scxmlautomodedialogprivatetest.h"
#include "ui_scxmlautomodedialog.h"
#include "qxmleditconfig.h"

SCXMLAutoModeDialog::PrivateTest::Tests SCXMLAutoModeDialog::PrivateTest::testToExecute()
{
    return (Tests)Config::getInt(Config::KEY_TEST+"/scxmlAutoPanel/testToExecute",-1);
}

void SCXMLAutoModeDialog::PrivateTest::setTestToExecute(const Tests test)
{
    Config::saveInt(Config::KEY_TEST+"/scxmlAutoPanel/testToExecute",test);
}

bool SCXMLAutoModeDialog::PrivateTest::testPassed()
{
    return Config::getBool(Config::KEY_TEST+"/scxmlAutoPanel/testPassed",false);
}

void SCXMLAutoModeDialog::PrivateTest::setTestPassed(const bool passed)
{
    Config::saveBool(Config::KEY_TEST+"/scxmlAutoPanel/testPassed",passed);
}

QString SCXMLAutoModeDialog::PrivateTest::testMessage()
{
    return Config::getString(Config::KEY_TEST+"/scxmlAutoPanel/testMessage","");
}

void SCXMLAutoModeDialog::PrivateTest::setTestMessage(const QString &msg)
{
    Config::saveString(Config::KEY_TEST+"/scxmlAutoPanel/testMessage",msg);
}

bool SCXMLAutoModeDialog::PrivateTest::panelOpen()
{
    return Config::getBool(Config::KEY_TEST+"/scxmlAutoPanel/panelOpen",false);
}

void SCXMLAutoModeDialog::PrivateTest::setPanelOpen(const bool open)
{
    Config::saveBool(Config::KEY_TEST+"/scxmlAutoPanel/panelOpen",open);
}

bool SCXMLAutoModeDialog::PrivateTest::answerShowSCXMLPanel()
{
    return Config::getBool(Config::KEY_TEST+"/scxmlAutoPanel/answerShow",false);
}

void SCXMLAutoModeDialog::PrivateTest::setAnswerShowSCXMLPanel(const bool how)
{
    Config::saveBool(Config::KEY_TEST+"/scxmlAutoPanel/answerShow",how);
}

SCXMLAutoModeDialog::ERetCode SCXMLAutoModeDialog::PrivateTest::answer()
{
    return (SCXMLAutoModeDialog::ERetCode)Config::getInt(Config::KEY_TEST+"/scxmlAutoPanel/answer",SCXMLAutoModeDialog::ENTER_SCXMLMODE);
}

void SCXMLAutoModeDialog::PrivateTest::setAnswer(const SCXMLAutoModeDialog::ERetCode how)
{
    Config::saveInt(Config::KEY_TEST+"/scxmlAutoPanel/answer",(int)how);
}


//---------------------------------------------------------

SCXMLAutoModeDialog::PrivateTest::PrivateTest(SCXMLAutoModeDialog *dlg) :
    QObject(dlg)
{
    _dlg = dlg;
}

SCXMLAutoModeDialog::PrivateTest::~PrivateTest()
{
}

void SCXMLAutoModeDialog::PrivateTest::testStart()
{
    setPanelOpen(true);
    setTestPassed(false);
    setTestMessage("");
    switch(testToExecute()) {
    case TEST_SHOWPANEL_XML_FILE: // show panel
        setTestPassed(false);
        setTestMessage("panel open");
        break;
    case TEST_SHOWPANEL_SCXML_FILE: // show panel
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
        _dlg->ui->cbDoNotShow->setChecked(!answerShowSCXMLPanel());
        // use the appropriate answer
        setTestPassed(true);
        switch(answer()) {
        case ENTER_SCXMLMODE:
            _dlg->on_cmdYes_clicked();
            break;
        case DONOTENTER_SCXMLMODE:
            _dlg->on_cmdNo_clicked();
            break;
        case ALWAYS_SCXMLMODE:
            _dlg->on_cmdAlways_clicked();
            break;
        default:
            setTestPassed(false);
            setTestMessage(QString("unknown answer %1").arg(answerShowSCXMLPanel()));
            break;
        }
        return ;

    default:
        break;
    }
    _dlg->accept();
}


