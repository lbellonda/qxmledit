/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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


#include "testloadfile.h"
#include "app.h"
#include "testhelpers/testmainwindow.h"

#define TEST_DATA TEST_BASE_DATA "/xml/loading/"

#define FILE_OK TEST_DATA "ok.xml"
#define FILE_KO TEST_DATA "ko.xml"

TestLoadFile::TestLoadFile()
{
}

TestLoadFile::~TestLoadFile()
{
}

bool TestLoadFile::testFast()
{
    _testName = "testFastt" ;
    return testLoadWithModifications();
}

bool TestLoadFile::testUnit()
{
    _testName = "testUnit" ;

    if(!loadFileOK()) {
        return false;
    }
    if(!loadFileKO()) {
        return false;
    }
    if(!testLoadWithModifications()) {
        return false;
    }
#if QT_VERSION >= QT_VERSION_CHECK(5,6,2)
    if(!testVerifyLoad1()) {
        return false;
    }

    if(!testVerifyLoad2()) {
        return false;
    }
#endif

    return true;
}

bool TestLoadFile::loadFileOK()
{
    _testName = "loadFileOK";
    App app;
    if(!app.init() ) {
        return error("init");
    }
    Config::saveBool(Config::KEY_GENERAL_OPEN_NEWWINDOW, true);
    app.getUiDelegate()->resetErrorCount();
    if(!app.mainWindow()->loadFile(QString(FILE_OK))) {
        return error("load file");
    }
    if(app.getUiDelegate()->errorCount()>0) {
        return error(QString("Expected no errors, but found:%1").arg(app.getUiDelegate()->errorCount()));
    }
    return true ;
}

bool TestLoadFile::loadFileKO()
{
    _testName = "loadFileKO";
    App app;
    if(!app.init() ) {
        return error("init");
    }
    Config::saveBool(Config::KEY_GENERAL_OPEN_NEWWINDOW, true);
    app.getUiDelegate()->resetErrorCount();
    if(app.mainWindow()->loadFile(QString(FILE_KO))) {
        return error("Loading file was unxpected successfully.");
    }
    if(app.getUiDelegate()->errorCount()==0) {
        return error(QString("Expected errors, but found:%1").arg(app.getUiDelegate()->errorCount()));
    }
    return true ;
}

bool TestLoadFile::testVerifyLoad1()
{
    _testName = "testVerifyLoad1";
    App app;
    if(!app.init() ) {
        return error("init");
    }
    Config::saveBool(Config::KEY_GENERAL_OPEN_NEWWINDOW, true);
    app.getUiDelegate()->resetErrorCount();
    const char *sz = "<?xml version='1.0' standalone='yes'?><root a='a&#1A;'/>";
    QString strTest = sz ;
    QBuffer ioDevice;
    ioDevice.setData(strTest.toUtf8());
    if(ioDevice.open(QIODevice::ReadOnly)) {
        if( app.mainWindow()->loadFileInnerStream(&ioDevice, "a", false, true)) {
            return error("Invalid xml loaded.");
        }
        if(app.getUiDelegate()->errorCount()==0) {
            return error(QString("Expected errors, but found:%1").arg(app.getUiDelegate()->errorCount()));
        }
    } else {
        return error("Unable to load test data.");
    }
    return true ;
}

bool TestLoadFile::testVerifyLoad2()
{
    _testName = "testVerifyLoad2";
    App app;
    if(!app.init() ) {
        return error("init");
    }
    Config::saveBool(Config::KEY_GENERAL_OPEN_NEWWINDOW, true);
    app.getUiDelegate()->resetErrorCount();
    const char *sz = "<?xml version='1.0' standalone='yes'?><root a='a\x1A'/>";
    QString strTest = sz ;
    QBuffer ioDevice;
    ioDevice.setData(strTest.toUtf8());
    if(ioDevice.open(QIODevice::ReadOnly)) {
        if( app.mainWindow()->loadFileInnerStream(&ioDevice, "a", false, true)) {
            return error("Invalid xml loaded.");
        }
        if(app.getUiDelegate()->errorCount()==0) {
            return error(QString("Expected errors, but found:%1").arg(app.getUiDelegate()->errorCount()));
        }
    } else {
        return error("Unable to load test data.");
    }
    return true ;
}


/* Logic for ask for modifications (to be replicated in tests)
 * Operation: before:  1! n  after: 1! n
 * Reload              a  a         -  -
 * Drop                a  -         -  s
 * Session             a  -         -  s
 * Recent              a  -         -  s
 * Preferred           a  -         -  s
 * New Clip            a  -         -  s
 * New Spec            a  -         -  s
 * Load                a  -         -  s
 *
 * a: ask unconditionally.
 * s: ask if the file is in one of the editor windows.
 *
 * Ask:
 * -if single + mod -> ask
 * -if force + mod -> ask
 */

bool TestLoadFile::testLoadWithModifications()
{
    _testName = "testLoadWithModifications" ;
    if(!loadDrop()) {
        return false;
    }
    if(!loadReload()) {
        return false;
    }
    if(!loadSession()) {
        return false;
    }
    if(!loadRecent()) {
        return false;
    }
    if(!loadPreferredDir()) {
        return false;
    }
    /*if(!loadOpen()) {
        return false;
    }*/
    if(!newSpec()) {
        return false;
    }
    /*if(!newFromClp()) {
        return false;
    }
    if(!newNew()) {
        return false;
    }*/
    return true ;
}

bool TestLoadFile::loadFileForMod(const bool openOtherWindow,
                                  const QString &filePath, const bool isModification,
                                  const int expectingAskingFirst, const int expectingAskingLater,
                                  const int expectedWindows,
                                  bool (TestLoadFile::*method)(App *, const QString &) )
{
    App app;
    app.setUseTestWindow(true);
    if(!app.init(true) ) {
        return error("init");
    }
    Config::saveBool(Config::KEY_GENERAL_OPEN_NEWWINDOW, openOtherWindow);
    app.getUiDelegate()->resetErrorCount();
    if(!app.mainWindow()->loadFile(QString(filePath))) {
        return error(QString("Loading file:'%1'").arg(filePath));
    }
    if( isModification ) {
        app.mainWindow()->getRegola()->setModified(true);
    }
    app.getCurrentUIDelegate()->resetErrorCount();
    if( !(this->*method)(&app, filePath) ) {
        return error("Method returned false");
    }
    if( isModification ) {
        if(app.getCurrentUIDelegate()->askCountBeforeLoad()!=expectingAskingFirst) {
            return error(QString("Expected %1 questions before, but found %2")
                         .arg(expectingAskingFirst).arg(app.getCurrentUIDelegate()->askCountBeforeLoad()));
        }
        if(app.getCurrentUIDelegate()->askCountAfterLoad()!=expectingAskingLater) {
            return error(QString("Expected %1 questions after, but found %2")
                         .arg(expectingAskingLater).arg(app.getCurrentUIDelegate()->askCountAfterLoad()));
        }
    } else {
        if(app.getCurrentUIDelegate()->askTotalCount()!=0) {
            return error(QString("Expected no ask message, but found:%1").arg(app.getUiDelegate()->askTotalCount()));
        }
    }
    if( app.data()->windowsCount() != expectedWindows ) {
        return error(QString("Windows count error: expected %1, but found:%2").arg(app.data()->windowsCount()).arg(expectedWindows));
    }

    return true ;
}

bool TestLoadFile::loadFileMod(
                                  const bool openOtherWindow,
                                  const int expectingAskingFirst, const int expectingAskingLater,
                                  const int expectedWindows,
                                  bool (TestLoadFile::*method)(App *, const QString &) )
{
    return loadFileForMod(openOtherWindow, FILE_OK, true,
                                expectingAskingFirst, expectingAskingLater,
                                expectedWindows,
                                method );
}

bool TestLoadFile::loadFileNoMod(
                                  const bool openOtherWindow,
                                  const int expectingAskingFirst, const int expectingAskingLater,
                                  const int expectedWindows,
                                  bool (TestLoadFile::*method)(App *, const QString &) )
{
    return loadFileForMod(openOtherWindow, FILE_OK, false,
                                expectingAskingFirst, expectingAskingLater,
                                expectedWindows,
                                method );
}

bool TestLoadFile::actionDrop(App *app, const QString & filePath)
{
    return app->mainWindow()->openDroppedFile(filePath);
}

bool TestLoadFile::loadDrop()
{
    _testName = "loadDrop" ;
    // no mod
    if( !loadFileNoMod(true, 0, 0, 1, &TestLoadFile::actionDrop ) ) {
        return false;
    }
    // mod
    if( !loadFileMod(true, 0, 1, 1, &TestLoadFile::actionDrop ) ) {
        return false;
    }
    if( !loadFileNoMod(false, 0, 0, 1, &TestLoadFile::actionDrop ) ) {
        return false;
    }
    if( !loadFileMod(false, 1, 0, 1, &TestLoadFile::actionDrop ) ) {
        return false;
    }
    return true;
}

bool TestLoadFile::actionReload(App *app, const QString & /*filePath*/)
{
    return app->mainWindow()->reload();
}

bool TestLoadFile::loadReload()
{
    _testName = "loadReload" ;
    if( !loadFileNoMod(true, 0, 0, 1, &TestLoadFile::actionReload ) ) {
        return false;
    }
    if( !loadFileMod(true, 1, 0, 1, &TestLoadFile::actionReload ) ) {
        return false;
    }
    if( !loadFileNoMod(false, 0, 0, 1, &TestLoadFile::actionReload ) ) {
        return false;
    }
    if( !loadFileMod(false, 1, 0, 1, &TestLoadFile::actionReload ) ) {
        return false;
    }
    return true;
}

bool TestLoadFile::actionLoadSession(App *app, const QString & filePath)
{
    return app->mainWindow()->onSessionfileLoadRequest(filePath);
}

bool TestLoadFile::loadSession()
{
    _testName = "loadSession" ;
    if( !loadFileNoMod(true, 0, 0, 1, &TestLoadFile::actionLoadSession ) ) {
        return false;
    }
    if( !loadFileMod(true, 0, 1, 1, &TestLoadFile::actionLoadSession ) ) {
        return false;
    }
    if( !loadFileNoMod(false, 0, 0, 1, &TestLoadFile::actionLoadSession ) ) {
        return false;
    }
    if( !loadFileMod(false, 1, 0, 1, &TestLoadFile::actionLoadSession ) ) {
        return false;
    }
    return true;
}

bool TestLoadFile::actionLoadRecent(App *app, const QString & filePath)
{
    return app->mainWindow()->recentFile(filePath);
}

bool TestLoadFile::loadRecent()
{
    _testName = "loadRecent" ;
    if( !loadFileNoMod(true, 0, 0, 1, &TestLoadFile::actionLoadRecent ) ) {
        return false;
    }
    if( !loadFileMod(true, 0, 1, 1, &TestLoadFile::actionLoadRecent ) ) {
        return false;
    }
    if( !loadFileNoMod(false, 0, 0, 1, &TestLoadFile::actionLoadRecent ) ) {
        return false;
    }
    if( !loadFileMod(false, 1, 0, 1, &TestLoadFile::actionLoadRecent ) ) {
        return false;
    }
    return true;
}

bool TestLoadFile::actionLoadPreferredDir(App *app, const QString & filePath)
{
    return app->mainWindow()->preferredDirLoadFile(filePath);
}

bool TestLoadFile::loadPreferredDir()
{
    _testName = "loadPreferredDir" ;
    if( !loadFileNoMod(true, 0, 0, 1, &TestLoadFile::actionLoadPreferredDir ) ) {
        return false;
    }
    if( !loadFileMod(true, 0, 1, 1, &TestLoadFile::actionLoadPreferredDir ) ) {
        return false;
    }
    if( !loadFileNoMod(false, 0, 0, 1, &TestLoadFile::actionLoadPreferredDir ) ) {
        return false;
    }
    if( !loadFileMod(false, 0, 0, 1, &TestLoadFile::actionLoadPreferredDir ) ) {
        return false;
    }
    return true;
}

bool TestLoadFile::actionNewSpecialized(App *app, const QString & /*filePath*/)
{
    app->mainWindow()->on_actionNewMavenPOM_triggered();
    return true;
}

bool TestLoadFile::newSpec()
{
    _testName = "newSpec" ;
    if( !loadFileNoMod(true, 0, 0, 2, &TestLoadFile::actionNewSpecialized ) ) {
        return false;
    }
    if( !loadFileMod(true, 0, 0, 2, &TestLoadFile::actionNewSpecialized ) ) {
        return false;
    }
    if( !loadFileNoMod(false, 0, 0, 1, &TestLoadFile::actionNewSpecialized ) ) {
        return false;
    }
    if( !loadFileMod(false, 1, 0, 1, &TestLoadFile::actionNewSpecialized ) ) {
        return false;
    }
    return true;
}

