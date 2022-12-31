/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2022 by Luca Bellonda and individual contributors       *
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

#include "testtestmainwindow.h"
#include "testhelpers/testmainwindowfile.h"

#define DATA_DIR    "../test/data/mainwindow/"
#define DATA_DIR_VERIFY    "../test/data/mainwindow"
#define FILE_1 DATA_DIR "file1.xml"

TestTestMainWindow::TestTestMainWindow()
{
    //
}

TestTestMainWindow::~TestTestMainWindow()
{
    //
}

bool TestTestMainWindow::testFast()
{
    return testSaveFileName();
}

bool TestTestMainWindow::testUnit() {
    _testName = "testUnit";
    if(!testSaveMessage()) {
        return false;
    }
    if(!testFileNameOrFolder()) {
        return false;
    }
    if(!testSaveFileName()) {
        return false;
    }
    return true;
}

bool TestTestMainWindow::testSaveMessage()
{
    _subTestName = "testSaveMessage";
    App app;
    if(!app.initNoWindow() ) {
        return error("init");
    }
    MainWindow mainWindow(false, false, app.data());
    {
        const QString file1 = "/1/aaa.xml" ;
        const QString msg1 = mainWindow.fileSavedMessage(file1);
        if(!msg1.endsWith(file1)) {
            return error(QString("1: Expecting:%1, found:%2").arg(file1).arg(msg1));
        }
    }
    {
        const QString file1 = "01020304050607080910a" ;
        const QString expected = "...04050607080910a" ;
        const QString msg1 = mainWindow.fileSavedMessage(file1);
        if(!msg1.endsWith(expected)) {
            return error(QString("1: Expecting:%1, found:%2").arg(expected).arg(msg1));
        }
    }
    return true ;
}

bool TestTestMainWindow::testFileNameOrFolder()
{
    _subTestName = "testFileNameOrFolder";
    App app;
    if(!app.init() ) {
        return false;
    }
    const QString expected = "";
    const QString candidate = "";
    QString result = app.mainWindow()->fileNameOrFolder(candidate);
    if(result != expected ) {
        return error(QString("1 Expected: '%1' found: '%2'").arg(expected).arg(result));
    }
    app.mainWindow()->getRegola()->setFileName(FILE_1);
    result = app.mainWindow()->fileNameOrFolder(app.mainWindow()->getRegola()->fileName());
    if(result != DATA_DIR_VERIFY) {
        return error(QString("2 Expected: '%1' found: '%2'").arg(DATA_DIR_VERIFY).arg(result));
    }
    return true ;
}

#define TEST_FN(e) TEST_ORDIE((mainWindow->getRegola()->fileName()==FILE_1), e);

bool TestTestMainWindow::testSaveFileName()
{
    Utils::TODO_THIS_RELEASE("commenti sotto");
    _subTestName = "testSaveFileName";
    App app;
    app.setUseTestWindowFile();
    if(!app.init() ) {
        return error("init");
    }
    if(NULL != app.mainWindow()->getParentMainWindow()) {
        return error(QString("First level not null"));
    }
    TestMainWindowFile *mainWindow = dynamic_cast<TestMainWindowFile*>(app.mainWindow());
    // empty
    mainWindow->resetFileData();
    mainWindow->on_actionSave_triggered();
    if(!mainWindow->verifyEmptySaveTriggered()) {
        return error("verifyEmptySaveTriggered");
    }
    if( !app.mainWindow()->loadFile(FILE_1) ) {
        return error(QString("unable to load input file: '%1' ").arg(FILE_1));
    }
    // prova salva come
    // 1 - base file name does not change
    mainWindow->resetFileData();
    mainWindow->on_actionSave_triggered();
    if(!mainWindow->verifySaveTriggered()) {
        return error("verifySaveTriggered");
    }
    TEST_FN("Re 1");
    mainWindow->resetFileData();
    mainWindow->on_actionSaveAs_triggered();
    if(!mainWindow->verifySaveAsTriggered(FILE_1)) {
        return error("verifySaveAsTriggered");
    }
    TEST_FN("Re 2");
    mainWindow->resetFileData();
    mainWindow->on_actionSaveACopyAs_triggered();
    if(!mainWindow->verifyCopyAsTriggered(FILE_1)) {
        return error("verifyCopyAsTriggered");
    }
    TEST_FN("Re 3");
    // emulate window
    TestMainWindowFile *editWindow = new TestMainWindowFile(true, true, app.data(), mainWindow) ;
    editWindow->setWindowModality(Qt::WindowModal);
    editWindow->ui.editor->loadText("<a/>");
    if(app.mainWindow() != editWindow->getParentMainWindow()) {
        return error(QString("Second level not triggered"));
    }
    // 1 - base file name does not change
    // 2 - new file name is based on old folder
    TEST_FN("Re 4");
    editWindow->resetFileData();
    editWindow->on_actionSave_triggered();
    if(!editWindow->verifyEditSaveTriggered(DATA_DIR_VERIFY)) {
        return error("verifyEditSaveTriggered");
    }
    TEST_FN("Re 5");
    editWindow->resetFileData();
    editWindow->on_actionSaveAs_triggered();
    if(!editWindow->verifyEditSaveTriggered(DATA_DIR_VERIFY)) {
        return error("verifyEditSaveAsTriggered");
    }
    TEST_FN("Re 6");
    editWindow->resetFileData();
    editWindow->on_actionSaveACopyAs_triggered();
    if(!editWindow->verifyEditSaveTriggered(DATA_DIR_VERIFY)) {
        return error("verifyEditSaveACopyAsAsTriggered");
    }
    TEST_FN("Re 7");
    return true;
}
