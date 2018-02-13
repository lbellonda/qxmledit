/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016-2018 by Luca Bellonda and individual contributors  *
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

#include "testconfig.h"
#include "app.h"

TestConfig::TestConfig()
{
}

TestConfig::~TestConfig()
{
}

bool TestConfig::testUnit()
{
    _testName = "testUnit" ;
    if(!testSaveAndReadWithoutLimit()) {
        return false;
    }
    if(!testSaveAndReadWithLimit()) {
        return false;
    }
    if(!testSaveWithLimitAndReadWithoutLimit()) {
        return false;
    }
    if(!testSaveWithoutLimitAndReadWithLimit()) {
        return false;
    }
    if(!testVerifySaveAndRead()) {
        return false;
    }
    if(!testResetData()) {
        return false;
    }
    return true ;
}

bool TestConfig::testSaveAndReadWithoutLimit()
{
    _testName = "testSaveAndReadWithoutLimit" ;
    App app;
    if(!app.init()) {
        return error("init app");
    }
    QStringList referenceList ;
    referenceList << "1" << "2" << "3" << "4" << "5";
    Config::saveStringArray("a", referenceList);
    QStringList newList ;
    if(!Config::loadStringArray("a", newList)) {
        return error("error loading list");
    }
    if(!compareStringList("lists differ", referenceList, newList)) {
        return false;
    }
    return true;
}

bool TestConfig::testSaveAndReadWithLimit()
{
    _testName = "testSaveAndReadWithLimit" ;
    App app;
    if(!app.init()) {
        return error("init app");
    }
    QStringList saveList ;
    saveList << "1" << "2" << "3" << "4" << "5";
    Config::saveStringArray("a", saveList, 3);
    QStringList newList ;
    if(!Config::loadStringArray("a", newList, 3)) {
        return error("error loading list");
    }
    QStringList referenceList ;
    referenceList << "1" << "2" << "3" ;
    if(!compareStringList("lists differ", referenceList, newList)) {
        return false;
    }
    return true;
}

bool TestConfig::testSaveWithLimitAndReadWithoutLimit()
{
    _testName = "testSaveWithLimitAndReadWithoutLimit" ;
    App app;
    if(!app.init()) {
        return error("init app");
    }
    QStringList saveList ;
    saveList << "1" << "2" << "3" << "4" << "5";
    Config::saveStringArray("a", saveList, 3);
    QStringList newList ;
    if(!Config::loadStringArray("a", newList)) {
        return error("error loading list");
    }
    QStringList referenceList ;
    referenceList << "1" << "2" << "3" ;
    if(!compareStringList("lists differ", referenceList, newList)) {
        return false;
    }
    return true;
}

bool TestConfig::testSaveWithoutLimitAndReadWithLimit()
{
    _testName = "testSaveWithoutLimitAndReadWithLimit" ;
    App app;
    if(!app.init()) {
        return error("init app");
    }
    QStringList saveList ;
    saveList << "1" << "2" << "3" << "4" << "5";
    Config::saveStringArray("a", saveList);
    QStringList newList ;
    if(!Config::loadStringArray("a", newList, 3)) {
        return error("error loading list");
    }
    QStringList referenceList ;
    referenceList << "1" << "2" << "3" ;
    if(!compareStringList("lists differ", referenceList, newList)) {
        return false;
    }
    return true;
}

bool TestConfig::testVerifySaveAndRead()
{
    _testName = "testVerifySaveAndRead" ;
    App app;
    if(!app.init()) {
        return error("init app");
    }
    QStringList saveList ;
    saveList << "1" << "2" << "3" << "4" << "5";
    Config::saveStringArray("a", saveList);
    QStringList newList ;
    if(!Config::loadStringArray("a", newList, 3)) {
        return error("error loading list");
    }
    QStringList referenceList ;
    referenceList << "1" << "2" << "3" << "4" ;
    if(compareStringList("lists differ", referenceList, newList)) {
        return error("test does not recognize a difference");
    }
    return true;
}

bool TestConfig::testResetBeahviorData()
{
    _subTestName = "testResetBeahviorData" ;
    App app;
    if(!app.init()) {
        return error("init app");
    }
    Config::saveBool(Config::KEY_ELEMENT_EDIT_SHORTCUT_DIALOG_SHOWN, true);
    Config::saveBool(Config::KEY_ELEMENT_EDIT_SHORTCUT_USED, true);
    Config::saveBool(Config::KEY_ELEMENT_EDIT_MODEFORM_MODIFIED, true);
    Config::saveBool(Config::KEY_ELEMENT_EDIT_TYPE_DIALOG_SHOWN, true);

    app.data()->resetBehaviorData();
    if(!checkBoolSetting(Config::KEY_ELEMENT_EDIT_SHORTCUT_DIALOG_SHOWN, false) ) {
        return false;
    }
    if(!checkBoolSetting(Config::KEY_ELEMENT_EDIT_SHORTCUT_USED, false) ) {
        return false;
    }
    if(!checkBoolSetting(Config::KEY_ELEMENT_EDIT_MODEFORM_MODIFIED, false) ) {
        return false;
    }
    if(!checkBoolSetting(Config::KEY_ELEMENT_EDIT_TYPE_DIALOG_SHOWN, false) ) {
        return false;
    }
    return true;
}

bool TestConfig::testShortcutDialog()
{
    _subTestName = "testShortcutDialog" ;
    App app;
    if(!app.init()) {
        return error("init app");
    }
    Config::saveBool(Config::KEY_ELEMENT_EDIT_SHORTCUT_DIALOG_SHOWN, false);
    app.data()->setShortcutUsedDialogShown();
    if(!checkBoolSetting(Config::KEY_ELEMENT_EDIT_SHORTCUT_DIALOG_SHOWN, true) ) {
        return false;
    }
    Config::saveBool(Config::KEY_ELEMENT_EDIT_SHORTCUT_DIALOG_SHOWN, true);
    app.data()->setShortcutUsedDialogShown();
    if(!checkBoolSetting(Config::KEY_ELEMENT_EDIT_SHORTCUT_DIALOG_SHOWN, true) ) {
        return false;
    }
    return true;
}


bool TestConfig::evalC(const int iBaseEdit)
{
    return iBaseEdit != 0 ;
}

bool TestConfig::setupSingleChooseEditTypeDialogSetup(App &app, const bool expected, const bool baseEditModeFormModified, const bool editTypeDialogShown, const uint editElementAsFormUsageCount, const uint editElementAsTextUsageCount)
{
    app.data()->setBaseEditModeFormModified(baseEditModeFormModified);
    if(editTypeDialogShown) {
        app.data()->setEditTypeDialogShown();
    } else {
        app.data()->resetEditTypeDialogShown();
    }
    const bool result = app.mainWindow()->evaluateIfShowEditingTypeDialog(editElementAsFormUsageCount, editElementAsTextUsageCount);
    if(result != expected) {
        return error(QString("1 Expected %1 for %2,%3,%4,%5").arg(expected).arg(baseEditModeFormModified).arg(editTypeDialogShown).arg(editElementAsFormUsageCount).arg(editElementAsTextUsageCount));
    }
    const bool result2 = app.mainWindow()->baseEvaluateIfShowEditingTypeDialog(baseEditModeFormModified, editTypeDialogShown, editElementAsFormUsageCount, editElementAsTextUsageCount);
    if(result2 != expected) {
        return error(QString("2 Expected %1 for %2,%3,%4,%5").arg(expected).arg(baseEditModeFormModified).arg(editTypeDialogShown).arg(editElementAsFormUsageCount).arg(editElementAsTextUsageCount));
    }
    return true;
}


bool TestConfig::testOpenChooseEditTypeDialog()
{
    _subTestName = "testOpenChooseEditTypeDialog" ;
    App app;
    if(!app.init()) {
        return error("init app");
    }
    const int values[][5]  = {

//expected, baseEditModeFormModified, editTypeDialogShown editElementAsFormUsageCount, editElementAsTextUsageCount)
        { 0, 0, 0, 0, 0 },
        { 0, 1, 0, 0, 0 },
        { 0, 0, 1, 0, 0 },
        { 0, 1, 1, 0, 0 },

        { 0, 0, 0, MainWindow::MaxTimesElementEditedWithoutText, 0 },
        { 0, 1, 0, MainWindow::MaxTimesElementEditedWithoutText, 0 },
        { 0, 0, 1, MainWindow::MaxTimesElementEditedWithoutText, 0 },
        { 0, 1, 1, MainWindow::MaxTimesElementEditedWithoutText, 0 },

        { 0, 0, 0, MainWindow::MaxTimesElementEditedWithoutText, 1 },
        { 0, 1, 0, MainWindow::MaxTimesElementEditedWithoutText, 1 },
        { 0, 0, 1, MainWindow::MaxTimesElementEditedWithoutText, 1 },
        { 0, 1, 1, MainWindow::MaxTimesElementEditedWithoutText, 1 },

        { 0, 0, 0, MainWindow::MaxTimesElementEditedWithoutText-1, 0 },
        { 0, 1, 0, MainWindow::MaxTimesElementEditedWithoutText-1, 0 },
        { 0, 0, 1, MainWindow::MaxTimesElementEditedWithoutText-1, 0 },
        { 0, 1, 1, MainWindow::MaxTimesElementEditedWithoutText-1, 0 },

        { 0, 0, 0, MainWindow::MaxTimesElementEditedWithoutText-1, 1 },
        { 0, 1, 0, MainWindow::MaxTimesElementEditedWithoutText-1, 1 },
        { 0, 0, 1, MainWindow::MaxTimesElementEditedWithoutText-1, 1 },
        { 0, 1, 1, MainWindow::MaxTimesElementEditedWithoutText-1, 1 },

        { 1, 0, 0, MainWindow::MaxTimesElementEditedWithoutText+1, 0 },
        { 0, 1, 0, MainWindow::MaxTimesElementEditedWithoutText+1, 0 },
        { 0, 0, 1, MainWindow::MaxTimesElementEditedWithoutText+1, 0 },
        { 0, 1, 1, MainWindow::MaxTimesElementEditedWithoutText+1, 0 },

        { 0, 0, 0, MainWindow::MaxTimesElementEditedWithoutText+1, 1 },
        { 0, 1, 0, MainWindow::MaxTimesElementEditedWithoutText+1, 1 },
        { 0, 0, 1, MainWindow::MaxTimesElementEditedWithoutText+1, 1 },
        { 0, 1, 1, MainWindow::MaxTimesElementEditedWithoutText+1, 1 },


        { 1, 0, 0, MainWindow::MaxTimesElementEditedWithoutText+100, 0 },
        { 0, 0, 0, MainWindow::MaxTimesElementEditedWithoutText+100, 1 },
        { -1, 0, 0, 0, 0 },

    };


    const int *p = values[0];
    while(p[0] > 0) {
        if(!setupSingleChooseEditTypeDialogSetup(app, evalC(p[0]), evalC(p[1]), evalC(p[2]), p[3], p[4])) {
            return false;
        }
        p++;
    }
    return true;
}

bool TestConfig::testResetData()
{
    _testName = "testResetData" ;
    if( !testResetBeahviorData()) {
        return false;
    }
    if( !testShortcutDialog()) {
        return false;
    }
    if(!testOpenChooseEditTypeDialog()) {
        return false;
    }
    return error("nyi");
}

