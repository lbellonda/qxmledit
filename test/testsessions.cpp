/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2018 by Luca Bellonda and individual contributors  *
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

#include "testsessions.h"
#include <QTest>
#include "fakeuidelegate.h"
#include "app.h"
#include "../src/mainwindow.h"
#include "../src/qxmleditconfig.h"
#include "sqlliteaccess.h"

#define FILE_1  "../test/data/test1.xml"
#define FILE_2  "../test/data/test2.xml"

TestSessions::TestSessions()
{
}

bool TestSessions::testUI1()
{
    App app;
    if(!app.init()) {
        return errorInit();
    }
    app.data()->sessionManager()->setEnabled(false);
    if(app.mainWindow()->sessionStateWidget()->isVisible()) {
        fprintf(stderr, "session widget is visible\n");
        return false;
    }
    return true ;
}

bool TestSessions::testStartSessionDisabled()
{
    App app;
    if(!app.initNoWindow()) {
        return errorInit();
    }
    Config::saveBool(Config::KEY_SESSIONS_ENABLED, false) ;
    MainWindow mainWindow(false, app.data());
    if( app.data()->sessionManager()->isEnabled() ) {
        return false;
    }
    return true ;
}

bool TestSessions::testStartWithoutSessions()
{
    App app;
    if(!app.initNoWindow()) {
        return errorInit();
    }
    Config::saveBool(Config::KEY_SESSIONS_ENABLED, true) ;
    app.data()->setActiveSessionState(Session::Active);
    MainWindow mainWindow(false, app.data());
    if( app.data()->sessionManager()->currentSession() != NULL ) {
        return errorT();
    }
    return true ;
}

bool TestSessions::testStartWithLastSessionIdStateActive()
{
    SQLLiteTestAccess access;
    if(!access.init()) {
        return errorT();
    }
    App app;
    app.sessionDBPath = access.dbFileName;
    app.init1();
    SessionOperationStatus status;
    SessionModel modelToIns;
    modelToIns.name = "test";
    modelToIns.description = "testdescr";
    if(!access.access->newSession(status, &modelToIns)) {
        return errorT();
    }
    app.data()->setActiveSessionState(Session::Active);
    app.data()->setActiveSessionCode(modelToIns.id);
    Config::saveBool(Config::KEY_SESSIONS_ENABLED, true) ;
    if(!app.init2()) {
        return errorInit();
    }
    MainWindow mainWindow(false, app.data());
    if( app.data()->sessionManager()->currentSession() == NULL ) {
        return errorT();
    }
    if( app.data()->sessionManager()->currentSession()->id() != modelToIns.id ) {
        return errorT();
    }
    return true ;
}

bool TestSessions::testStartWithLastSessionIdStatePaused()
{
    SQLLiteTestAccess access;
    if(!access.init()) {
        return errorT();
    }
    App app;
    app.sessionDBPath = access.dbFileName;
    app.init1();
    Config::saveBool(Config::KEY_SESSIONS_ENABLED, true) ;
    SessionOperationStatus status;
    SessionModel modelToIns;
    modelToIns.name = "test";
    modelToIns.description = "testdescr";
    if(!access.access->newSession(status, &modelToIns)) {
        return errorT();
    }
    app.data()->setActiveSessionState(Session::Paused);
    app.data()->setActiveSessionCode(modelToIns.id);
    if(!app.init2()) {
        return false;
    }
    MainWindow mainWindow(false, app.data());
    if( app.data()->sessionManager()->currentSession() == NULL ) {
        return errorT();
    }
    if( app.data()->sessionManager()->currentSession()->state() != Session::Paused ) {
        return errorT();
    }
    return true ;
}

bool TestSessions::testStartWithLastSessionIllegalState()
{
    SQLLiteTestAccess access;
    if(!access.init()) {
        return errorInit();
    }
    App app;
    app.sessionDBPath = access.dbFileName;
    app.init1();
    Config::saveBool(Config::KEY_SESSIONS_ENABLED, true) ;
    SessionOperationStatus status;
    SessionModel modelToIns;
    modelToIns.name = "test";
    modelToIns.description = "testdescr";
    if(!access.access->newSession(status, &modelToIns)) {
        return errorT();
    }
    app.data()->setActiveSessionState((Session::SessionState)-2);
    app.data()->setActiveSessionCode(modelToIns.id);
    if(!app.init2()) {
        return false;
    }
    MainWindow mainWindow(false, app.data());
    if( app.data()->sessionManager()->currentSession() == NULL ) {
        return errorT();
    }
    Session::SessionState state = app.data()->sessionManager()->currentSession()->state() ;
    if( state != Session::NoSession) {
        return errorT();
    }
    return true ;
}

bool TestSessions::testCmdPauseCheck()
{
    SQLLiteTestAccess access;
    if(!access.init()) {
        return errorInit();
    }
    App app;
    app.sessionDBPath = access.dbFileName;
    app.init1();
    Config::saveBool(Config::KEY_SESSIONS_ENABLED, true) ;
    SessionOperationStatus status;
    SessionModel modelToIns;
    modelToIns.name = "test";
    modelToIns.description = "testdescr";
    if(!access.access->newSession(status, &modelToIns)) {
        return errorT();
    }
    app.data()->setActiveSessionState(Session::Active);
    app.data()->setActiveSessionCode(modelToIns.id);
    if(!app.init2()) {
        return errorT();
    }
    MainWindow mainWindow(false, app.data());
    if( app.data()->sessionManager()->currentSession() == NULL ) {
        return errorT();
    }
    if( app.data()->sessionManager()->currentSession()->state() != Session::Active ) {
        return errorT();
    }
    mainWindow.getUI()->actionPauseSession->trigger();
    if( app.data()->sessionManager()->currentSession()->state() != Session::Paused ) {
        return errorT();
    }
    if( app.data()->activeSessionState() != Session::Paused) {
        return errorT();
    }
    return true ;
}

bool TestSessions::testCmdResume()
{
    SQLLiteTestAccess access;
    if(!access.init()) {
        return errorInit();
    }
    App app;
    app.sessionDBPath = access.dbFileName;
    app.init1();
    Config::saveBool(Config::KEY_SESSIONS_ENABLED, true) ;
    SessionOperationStatus status;
    SessionModel modelToIns;
    modelToIns.name = "test";
    modelToIns.description = "testdescr";
    if(!access.access->newSession(status, &modelToIns)) {
        return errorT();
    }
    app.data()->setActiveSessionState(Session::Paused);
    app.data()->setActiveSessionCode(modelToIns.id);
    if(!app.init2()) {
        return errorT();
    }
    MainWindow mainWindow(false, app.data());
    if( app.data()->sessionManager()->currentSession() == NULL ) {
        return errorT();
    }
    if( app.data()->sessionManager()->currentSession()->state() != Session::Paused ) {
        return errorT();
    }
    mainWindow.getUI()->actionResumeSession->trigger();
    if( app.data()->sessionManager()->currentSession()->state() != Session::Active ) {
        return errorT();
    }
    if( app.data()->activeSessionState() != Session::Active) {
        return errorT();
    }
    return true ;
}

bool TestSessions::testCmdClose()
{
    SQLLiteTestAccess access;
    if(!access.init()) {
        return errorInit();
    }
    App app;
    app.sessionDBPath = access.dbFileName;
    app.init1();
    Config::saveBool(Config::KEY_SESSIONS_ENABLED, true) ;
    SessionOperationStatus status;
    SessionModel modelToIns;
    modelToIns.name = "test";
    modelToIns.description = "testdescr";
    if(!access.access->newSession(status, &modelToIns)) {
        return errorT();
    }
    app.data()->setActiveSessionState(Session::Active);
    app.data()->setActiveSessionCode(modelToIns.id);
    if(!app.init2()) {
        return errorT();
    }
    MainWindow mainWindow(false, app.data());
    if( app.data()->sessionManager()->currentSession() == NULL ) {
        return errorT();
    }
    if( app.data()->sessionManager()->currentSession()->state() != Session::Active ) {
        return errorT();
    }
    mainWindow.getUI()->actionCloseSession->trigger();
    if( !( (app.data()->sessionManager()->state() == Session::Active) &&  app.data()->sessionManager()->isDefaultSession() ) ) {
        return errorT();
    }
    if( !( ( app.data()->sessionManager()->currentSession() != NULL ) && app.data()->sessionManager()->isDefaultSession() ) ){
        return errorT();
    }
    if( app.data()->activeSessionCode() != 0 ) {
        return errorT();
    }
    if( (app.data()->activeSessionState() == Session::Active) && !app.data()->sessionManager()->isDefaultSession() ) {
        return errorT();
    }
    return true ;
}

bool TestSessions::testRegisterAccess()
{
    SQLLiteTestAccess access;
    if(!access.init()) {
        return errorInit();
    }
    App app;
    app.sessionDBPath = access.dbFileName;
    app.init1();
    Config::saveBool(Config::KEY_SESSIONS_ENABLED, true) ;
    SessionOperationStatus status;
    SessionModel modelToIns;
    modelToIns.name = "test";
    modelToIns.description = "testdescr";
    if(!access.access->newSession(status, &modelToIns)) {
        return errorT();
    }
    app.data()->setActiveSessionState(Session::Active);
    app.data()->setActiveSessionCode(modelToIns.id);
    if(!app.init2()) {
        return errorT();
    }
    MainWindow mainWindow(false, app.data());
    if( app.data()->sessionManager()->currentSession() == NULL ) {
        return errorT();
    }

    SessionOperationStatus status2;
    SessionModel modelToCheck;
    modelToCheck.id = modelToIns.id ;

    if(!access.access->readSession(status2, &modelToCheck)) {
        return errorT() ;
    }
    if( modelToCheck.id != modelToIns.id ) {
        return errorT() ;
    }
    // beware: the access date must be different from the last one
    if( modelToCheck.lastAccess <= modelToIns.lastAccess ) {
        return errorT() ;
    }
    return true ;
}

bool TestSessions::errorInit()
{
    return error("init");
}

bool TestSessions::errorT()
{
    return error("unspecified");
}

bool TestSessions::testSummaryData()
{
    SQLLiteTestAccess access;
    if(!access.init()) {
        return errorInit();
    }
    App app;
    app.sessionDBPath = access.dbFileName;
    app.init1();
    Config::saveBool(Config::KEY_SESSIONS_ENABLED, true) ;
    SessionOperationStatus status;
    SessionModel modelToIns;
    modelToIns.name = "test";
    modelToIns.description = "testdescr";
    if(!access.access->newSession(status, &modelToIns)) {
        return error("1");
    }
    app.data()->setActiveSessionState(Session::Active);
    app.data()->setActiveSessionCode(modelToIns.id);
    if(!app.init2()) {
        return error("2");
    }
    MainWindow mainWindow(false, app.data());
    if( app.data()->sessionManager()->currentSession() == NULL ) {
        return error("3");
    }
    mainWindow.loadFile(FILE_1);
    mainWindow.loadFile(FILE_1);
    //wait one second at least
    QTest::qSleep(2000);
    mainWindow.loadFile(FILE_2);
    SessionOperationStatus context;
    SessionSummary* summary = app.data()->sessionManager()->getSummary(context);
    if( !context.ok ){
        return error("4");
    }
    if( summary->mostRecent()->getChildItemsNumber() != 2 ) {
        return error("5");
    }
    SessionFileAccess *sfAccess10 = static_cast<SessionFileAccess*>(summary->mostRecent()->getChildAt(0));
    SessionFileAccess *sfAccess11 = static_cast<SessionFileAccess*>(summary->mostRecent()->getChildAt(1));
    if( ! ( ( sfAccess10->path() == FILE_2 ) && ( sfAccess11->path() == FILE_1 ) ) ) {
        return error("6");
    }
    if( summary->mostUsed()->getChildItemsNumber() != 2 ) {
        return error("7");
    }
    SessionFileAccess *sfAccess0 = static_cast<SessionFileAccess*>(summary->mostRecent()->getChildAt(0));
    SessionFileAccess *sfAccess1 = static_cast<SessionFileAccess*>(summary->mostRecent()->getChildAt(1));
    if( sfAccess0->path() == FILE_1 ) {
        if( sfAccess1->path() != FILE_2 ) {
            return error("8");
        }
    } else if( sfAccess0->path() == FILE_2 ) {
        if( sfAccess1->path() != FILE_1 ) {
            return error("9");
        }
    } else {
        return error("10");
    }

    if( summary->mostRecentFolders()->getChildItemsNumber() != 1 ) {
        return error("11");
    }

    return true ;
}


bool TestSessions::testAccessFileWithSessionsDisabled()
{
    SQLLiteTestAccess access;
    if(!access.init()) {
        return errorInit();
    }
    App app;
    app.sessionDBPath = access.dbFileName;
    app.init1();
    Config::saveBool(Config::KEY_SESSIONS_ENABLED, false) ;
    SessionOperationStatus status;
    SessionModel modelToIns;
    modelToIns.name = "test";
    modelToIns.description = "testdescr";
    if(!access.access->newSession(status, &modelToIns)) {
        return errorT();
    }
    if(!app.init2()) {
        return errorT();
    }
    MainWindow mainWindow(false, app.data());
    mainWindow.loadFile(FILE_1);
    SessionOperationStatus contextFiles;
    int cnt = access.access->countFiles(contextFiles);
    if( !contextFiles.ok) {
        return errorT();
    }
    if( cnt > 0 ) {
        return errorT();
    }
    return true ;
}

/**************** TODO:
per i dati, leggere il session model, tranne che per i test dell'albero, ricorrere a ?

ricordarsi di utilizzare il finto config

start sess disabled

test: start senza sessioni

start con sessione id, stato attivo

start con sessione id, stato paused

start con sessione id illegale

trigger pausa

trigger resume

trigger close


-- controlla valori

ins accesso 1
    ins accesso 1

    ins 2
controlla touch e lista oggetti mostrati

    assicutrati di avere inserito starred in file e cartelle

    pensa a gestire le cartele come starred serve una tabella vcartelle?


accedi a file con load
    controlla accesso sessione abilitata

disabilita sessione
    sccedi a file
    non trovare il file in sessione


fai sessione 1
    accesi
    fai asessione 2
    accedi a 2
    controlla file accresso in sessione 2
    controlla file acceduto sessione 1, devono essere disgiunti

*********************************/
