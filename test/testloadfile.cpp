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


bool TestLoadFile::testLoadWithModifications()
{
    _testName = "testLoadWithModifications" ;
    if(!loadDrop()) {
        return false;
    }
    /*if(!loadReload()) {
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
    if(!loadOpen()) {
        return false;
    }
    if(!newSpec()) {
        return false;
    }
    if(!newFromClp()) {
        return false;
    }
    if(!newNew()) {
        return false;
    }*/
    return error("nyi");
}

bool TestLoadFile::loadFileForMod(const QString &filePath, const bool isModification,
                                  const int expectingAskingFirst, const int expectingAskingLater,
                                  const int expectedWindows,
                                  bool (TestLoadFile::*method)(App *, const QString &) )
{
    App app;
    app.setUseTestWindow(true);
    if(!app.init(true) ) {
        return error("init");
    }
    Config::saveBool(Config::KEY_GENERAL_OPEN_NEWWINDOW, true);
    app.getUiDelegate()->resetErrorCount();
    if(!app.mainWindow()->loadFile(QString(filePath))) {
        return error(QString("Loading file:'%1'").arg(filePath));
    }
    if( isModification ) {
        app.mainWindow()->getRegola()->setModified(true);
    }
    if( !(this->*method)(&app, filePath) ) {
        return error("Method returned false");
    }
    if( isModification ) {
        if(app.getCurrentUIDelegate()->askCountBeforeLoad()!=expectingAskingFirst) {
            return error(QString("Expected %1 questions before, but found %2")
                         .arg(expectingAskingFirst).arg(app.getUiDelegate()->askCountBeforeLoad()));
        }
        if(app.getCurrentUIDelegate()->askCountAfterLoad()!=expectingAskingLater) {
            return error(QString("Expected %1 questions after, but found %2")
                         .arg(expectingAskingLater).arg(app.getUiDelegate()->askCountAfterLoad()));
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

bool TestLoadFile::actionDrop(App *app, const QString & filePath)
{
    return app->mainWindow()->openDroppedFile(filePath);
}

bool TestLoadFile::loadDrop()
{
    _testName = "loadDrop" ;
    // no mod
    if( !loadFileForMod(FILE_OK, false, 0, 0, 1, &TestLoadFile::actionDrop ) ) {
        return false;
    }
    // mod
    if( !loadFileForMod(FILE_OK, true, 0, 1, 1, &TestLoadFile::actionDrop ) ) {
        return false;
    }
    return true;
}
