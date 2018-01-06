/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2018 by Luca Bellonda and individual contributors  *
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

#include "testsqlaccess.h"
#include "sqlliteaccess.h"

TestSQLAccess::TestSQLAccess()
{
}

TestSQLAccess::~TestSQLAccess()
{
}

//------------------------------------------------

QString TestSQLAccess::msg()
{
    return _msg;
}

bool TestSQLAccess::error(const QString &msg)
{
    _msg = msg ;
    return false;
}

bool TestSQLAccess::error()
{
    return false;
}

bool TestSQLAccess::insertSession()
{
    SQLLiteTestAccess access;
    if(!access.init()) {
        return error();
    }
    bool isOk = true ;
    SessionOperationStatus status;
    SessionModel modelToIns;
    if( modelToIns.starred != 0 ) {
        isOk = error();
    }
    modelToIns.name = "test";
    modelToIns.description = "testdescr";
    modelToIns.starred= 5;
    if(!access.access->newSession(status, &modelToIns)) {
        isOk = error();
    }
    if(!status.ok) {
        isOk = error();
    }
    if(modelToIns.id == 0) {
        isOk = error();
    }
    SessionOperationStatus status1;
    SessionModel modelToCheck;
    modelToCheck.id = modelToIns.id ;
    if(!access.access->readSession(status1, &modelToCheck)) {
        isOk = error() ;
    }
    if(!status1.ok) {
        isOk = error() ;
    } else {
        if(modelToCheck.name != QString("test")) {
            isOk = error();
        }
        if(modelToCheck.description != QString("testdescr")) {
            isOk = error() ;
        }
        if(!modelToCheck.enabled) {
            isOk = error();
        }
        if(modelToCheck.id != modelToIns.id) {
            isOk = error() ;
        }
        if(modelToCheck.name != modelToIns.name) {
            isOk = error();
        }
        if(modelToCheck.description != modelToIns.description) {
            isOk = error();
        }
        if(modelToCheck.starred != modelToIns.starred) {
            isOk = error();
        }
    }
    return isOk;
}

bool TestSQLAccess::testFile()
{
    SQLLiteTestAccess access;
    if(!access.init()) {
        return error();
    }
    bool isOk = true ;
    if(!access.access->privatePart()->openTrans()) {
        isOk = error();
    }
    FileModel modelToIns;
    if( modelToIns.starred != 0 ) {
        isOk = error();
    }
    modelToIns.path = "filePath";
    modelToIns.description = "fileDescr";
    modelToIns.starred= 8;
    if(!access.access->privatePart()->insertFile(&modelToIns)) {
        isOk = error();
    }
    if(modelToIns.id == 0) {
        isOk = error();
    }
    if(!access.access->privatePart()->commitTrans()) {
        isOk = error();
    }

    FileModel modelToCheck;
    modelToCheck.id = modelToIns.id ;
    bool found = false;
    if(!access.access->privatePart()->getFile(&modelToCheck, "filePath", found)) {
        isOk = error() ;
    } else {
        if(! found) {
            isOk = error() ;
        }
        if(modelToCheck.path != QString("filePath")) {
            isOk = error() ;
        }
        if(modelToCheck.description != QString("fileDescr")) {
            isOk = error() ;
        }
        if(modelToCheck.id != modelToIns.id) {
            isOk = error() ;
        }
        if(modelToCheck.path != modelToIns.path) {
            isOk = error() ;
        }
        if(modelToCheck.description != modelToIns.description) {
            isOk = error() ;
        }
        if(modelToCheck.starred != modelToIns.starred) {
            isOk = error();
        }
    }
    return isOk;
}

bool TestSQLAccess::testFileNotFound()
{
    SQLLiteTestAccess access;
    if(!access.init()) {
        return error();
    }
    bool isOk = true ;
    if(!access.access->privatePart()->openTrans()) {
        isOk = error();
    }
    FileModel modelToCheck;
    bool found = false;
    if(!access.access->privatePart()->getFile(&modelToCheck, "filePath", found)) {
        isOk = error() ;
    } else {
        if(found) {
            isOk = error() ;
        }
        if(modelToCheck.id != 0) {
            isOk = error() ;
        }
    }
    return isOk;
}

bool TestSQLAccess::testTransCommands()
{
    SQLLiteTestAccess access;
    if(!access.init()) {
        return error();
    }
    bool isOk = true ;
    if(!access.access->privatePart()->openTrans()) {
        isOk = error();
    }
    if(!access.access->privatePart()->rollbackTrans()) {
        isOk = error();
    }
    return isOk;
}

//--------------------------------------------------------------------------------
bool TestSQLAccess::insertFileAccess()
{
    SQLLiteTestAccess access;
    if(!access.init()) {
        return error();
    }
    bool isOk = true ;
    SessionOperationStatus status;
    SessionModel sessionModel;
    sessionModel.name = "test1";
    sessionModel.description = "testdescr1";
    if(!access.access->newSession(status, &sessionModel)) {
        isOk = error();
    }
    if(!status.ok) {
        isOk = error();
    }
    if(sessionModel.id == 0) {
        isOk = error();
    }
    SessionOperationStatus context;
    QString filePath = "/1/2/3" ;
    if(!access.access->enrollFile(context, &sessionModel, filePath)) {
        isOk = error();
    }
    FileModel fileModel;
    bool found = false;
    if(!access.access->privatePart()->getFile(&fileModel, filePath, found)) {
        isOk = error();
    }
    if(!found) {
        isOk = error();
    }
    if(fileModel.path != filePath) {
        isOk = error();
    }
    if(fileModel.id == 0) {
        isOk = error();
    }
    int result =  0;
    if(!access.access->privatePart()->countSessionFileAccesses(&sessionModel, &fileModel, result)) {
        isOk = error();
    }
    if(result != 1) {
        isOk = error();
    }
    return isOk;
}

//--------------------------------------------------------------------------------
bool TestSQLAccess::insertFileAccessLow()
{
    SQLLiteTestAccess access;
    if(!access.init()) {
        return error();
    }
    bool isOk = true ;
    SessionOperationStatus status;
    SessionModel sessionModel;
    sessionModel.name = "test1";
    sessionModel.description = "testdescr1";
    if(!access.access->newSession(status, &sessionModel)) {
        isOk = error();
    }
    if(!status.ok) {
        isOk = error();
    }
    if(sessionModel.id == 0) {
        isOk = error();
    }
    if(!access.access->privatePart()->openTrans()) {
        isOk = error();
    }
    FileModel fileModel;
    fileModel.path = "filePath";
    fileModel.description = "fileDescr";
    if(!access.access->privatePart()->insertFile(&fileModel)) {
        isOk = error();
    }
    // insert another entity if the two id are equals
    if(fileModel.id == sessionModel.id) {
        fileModel.id = 0 ;
        fileModel.path = "filePath2";
        fileModel.description = "fileDescr2";
        if(!access.access->privatePart()->insertFile(&fileModel)) {
            isOk = error();
        }
    }
    if(!access.access->privatePart()->insertFileAccess(&sessionModel, &fileModel)) {
        isOk = error();
    }
    if(!access.access->privatePart()->commitTrans()) {
        isOk = error();
    }
    int result =  0;
    if(!access.access->privatePart()->countSessionFileAccesses(&sessionModel, &fileModel, result)) {
        isOk = error();
    }
    if(result != 1) {
        isOk = error();
    }
    return isOk;
}

//--------------------------------------------------------------------------------

/** \brief 2 files, one with 3 accesses, one with 1
  */
bool TestSQLAccess::readFileAccess()
{
    SQLLiteTestAccess access;
    if(!access.init()) {
        return error();
    }
    bool isOk = true ;
    SessionOperationStatus status;
    SessionModel sessionModel;
    sessionModel.name = "test1";
    sessionModel.description = "testdescr1";
    if(!access.access->newSession(status, &sessionModel)) {
        isOk = error();
    }
    if(!status.ok) {
        isOk = error();
    }
    if(sessionModel.id == 0) {
        isOk = error();
    }
    SessionOperationStatus context;
    QString filePath1 = "/1/2/3" ;
    //access 1
    if(!access.access->enrollFile(context, &sessionModel, filePath1)) {
        isOk = error();
    }
    //access 2
    if(!access.access->enrollFile(context, &sessionModel, filePath1)) {
        isOk = error();
    }
    //access 3
    if(!access.access->enrollFile(context, &sessionModel, filePath1)) {
        isOk = error();
    }
    QString filePath2 = "/3/4/5" ;
    //access 1
    if(!access.access->enrollFile(context, &sessionModel, filePath2)) {
        isOk = error();
    }
    if(!access.access->readSessionData(context, &sessionModel)) {
        isOk = error();
    }
    if(sessionModel.accesses.size() != 4) {
        isOk = error();
    }
    if(sessionModel.files.size() != 2) {
        isOk = error();
    }
    foreach(FileModel * file, sessionModel.files) {
        if(file->path == filePath1) {
            if(file->accesses.size() != 3) {
                isOk = error();
            }
        } else if(file->path == filePath2) {
            if(file->accesses.size() != 1) {
                isOk = error();
            }
        }
    }
    return isOk;
}


//-------------------------------------
bool TestSQLAccess::deleteSession()
{
    SQLLiteTestAccess access;
    if(!access.init()) {
        return error();
    }
    bool isOk = true ;
    SessionOperationStatus status;
    SessionModel modelToIns;
    modelToIns.name = "test";
    modelToIns.description = "testdescr";
    if(!access.access->newSession(status, &modelToIns)) {
        isOk = error();
    }
    if(!status.ok) {
        isOk = error();
    }
    if(modelToIns.id == 0) {
        isOk = error();
    }
    SessionOperationStatus status1;
    SessionModel modelToCheck;
    modelToCheck.id = modelToIns.id ;
    if(!access.access->readSession(status1, &modelToCheck)) {
        isOk = error() ;
    }
    SessionOperationStatus status2;
    if( !access.access->deleteSession(status2, &modelToCheck)) {
        isOk = error() ;
    }
    if(access.access->readSession(status1, &modelToCheck)) {
        isOk = error() ;
    }
    return isOk;
}

//-------------------------------------
bool TestSQLAccess::updateSession()
{
    SQLLiteTestAccess access;
    if(!access.init()) {
        return error();
    }
    bool isOk = true ;
    SessionOperationStatus status;
    SessionModel modelToIns;
    if( modelToIns.starred != 0 ) {
        isOk = error();
    }
    modelToIns.name = "test";
    modelToIns.description = "testdescr";
    modelToIns.starred = 10;
    if(!access.access->newSession(status, &modelToIns)) {
        isOk = error();
    }
    if(!status.ok) {
        isOk = error();
    }
    if(modelToIns.id == 0) {
        isOk = error();
    }
    SessionOperationStatus status1;
    SessionModel modelToCheck;
    modelToCheck.id = modelToIns.id ;
    if(!access.access->readSession(status1, &modelToCheck)) {
        isOk = error() ;
    }
    SessionOperationStatus status2;
    SessionModel modelUpdate;
    modelUpdate.id = modelToIns.id ;
    modelUpdate.name = "newname";
    modelUpdate.description= "newdescr";
    modelToIns.starred = 6;
    if( !access.access->updateSession(status2, &modelUpdate)) {
        isOk = error() ;
    }
    if(!access.access->readSession(status1, &modelToCheck)) {
        isOk = error() ;
    }
    if( isOk ) {
        if( modelToCheck.name != modelUpdate.name ) {
            isOk = error() ;
        }
        if( modelToCheck.id != modelUpdate.id ) {
            isOk = error() ;
        }
        if( modelToCheck.description != modelUpdate.description ) {
            isOk = error() ;
        }
        // beware: the update date must be different from the last one
        if( modelToCheck.updateDate == modelUpdate.updateDate ) {
            isOk = error() ;
        }
        if( modelToCheck.starred != modelUpdate.starred ) {
            isOk = error() ;
        }
    }
    return isOk;
}

//-------------------------------------
bool TestSQLAccess::touchSession()
{
    SQLLiteTestAccess access;
    if(!access.init()) {
        return error();
    }
    bool isOk = true ;
    SessionOperationStatus status;
    SessionModel modelToIns;
    modelToIns.name = "test";
    modelToIns.description = "testdescr";
    if(!access.access->newSession(status, &modelToIns)) {
        isOk = error();
    }
    if(!status.ok) {
        isOk = error();
    }
    if(modelToIns.id == 0) {
        isOk = error();
    }
    SessionOperationStatus status1;
    SessionModel modelToCheck;
    modelToCheck.id = modelToIns.id ;
    if(!access.access->readSession(status1, &modelToCheck)) {
        isOk = error() ;
    }
    SessionOperationStatus status2;
    SessionModel modelUpdate;
    modelUpdate.id = modelToIns.id ;
    modelUpdate.name = "newname";
    modelUpdate.description= "newdescr";
    if( !access.access->touchSession(status2, &modelUpdate)) {
        isOk = error() ;
    }
    if(!access.access->readSession(status2, &modelToCheck)) {
        isOk = error() ;
    }
    if( isOk ) {
        if( modelToCheck.name != modelToIns.name ) {
            isOk = error() ;
        }
        if( modelToCheck.id != modelToIns.id ) {
            isOk = error() ;
        }
        if( modelToCheck.description != modelToIns.description ) {
            isOk = error() ;
        }
        // beware: the access date must be different from the last one
        if( modelToCheck.lastAccess <= modelUpdate.lastAccess ) {
            isOk = error() ;
        }
    }
    return isOk;
}

/**********************************************************************
test: delete per data

data 1 mese
data 6 mesi
data scelta da me
inserisci un tipo non esistente e controlla errore


inserisci accessi opportuni
scrivi funzione conteggio


dati inseriti:

S1 A1 F1 d
   A2 F2 d
   A3 F2
S2 A4 F3 d
   A5 F1 d

devo trovare:

S1 A3 F2 (F1 F3)


********************************************************************/


#define SQL_UPDATE_ALL_SESSION_ACCESS_DATE  "update SESSIONS set lastaccess= date(lastaccess, '%1' ) "
#define SQL_UPDATE_ALL_ACCESSES_DATE "update FILE_SESSION_ACCESSES set accessDate= date(accessDate, '%1' )"


bool TestSQLAccess::deleteByDate1Month()
{
    return deleteByDateIntervalInternal("-1 month", SessionDataInterface::DATE_INTERVAL_1_MONTH, false );
}

bool TestSQLAccess::deleteByDate6Months()
{
    return deleteByDateIntervalInternal("-6 month", SessionDataInterface::DATE_INTERVAL_6_MONTHS, false );
}

bool TestSQLAccess::deleteByGivenDate()
{
    return deleteByDateIntervalInternal("-1 day", SessionDataInterface::DATE_INTERVAL_LITERAL, false );
}

bool TestSQLAccess::deleteByDate1MonthFiles()
{
    return deleteByDateIntervalInternal("-1 month", SessionDataInterface::DATE_INTERVAL_1_MONTH, true );
}

bool TestSQLAccess::deleteByDate6MonthsFiles()
{
    return deleteByDateIntervalInternal("-6 month", SessionDataInterface::DATE_INTERVAL_6_MONTHS, true );
}

bool TestSQLAccess::deleteByGivenDateFiles()
{
    return deleteByDateIntervalInternal("-1 day", SessionDataInterface::DATE_INTERVAL_LITERAL, true );
}

bool TestSQLAccess::deleteByDateWrongType()
{
    return deleteByDateIntervalInternal("+1 month", (SessionDataInterface::EDateInterval)-1, false, true );
}


bool TestSQLAccess::deleteByDateIntervalInternal(const QString &sqlTimeData, const SessionDataInterface::EDateInterval intervalType, const bool isFiles, const bool isErrorExpected )
{
    SQLLiteTestAccess access;
    if(!access.init()) {
        return error("1");
    }
    bool isOk = true ;
    SessionOperationStatus status;
    SessionModel sessionModel1;
    sessionModel1.name = "test1";
    sessionModel1.description = "testdescr1";
    if(!access.access->newSession(status, &sessionModel1)) {
        isOk = error("2");
    }
    SessionModel sessionModel2;
    sessionModel2.name = "test2";
    sessionModel2.description = "testdescr2";
    if(!access.access->newSession(status, &sessionModel2)) {
        isOk = error("3");
    }
    SessionOperationStatus context;
    QString filePath1 = "/1/2/1" ;
    //access 1
    if(!access.access->enrollFile(context, &sessionModel1, filePath1)) {
        isOk = error("4");
    }
    QString filePath2 = "/1/2/2" ;
    //access 2
    if(!access.access->enrollFile(context, &sessionModel1, filePath2)) {
        isOk = error("5");
    }
    QString filePath3 = "/3/4/3" ;
    //access 4
    if(!access.access->enrollFile(context, &sessionModel2, filePath3)) {
        isOk = error("6");
    }
    //access 5
    if(!access.access->enrollFile(context, &sessionModel2, filePath1)) {
        isOk = error("7");
    }
    // update the access/session date
    if(!access.access->execQuery(context, QString(SQL_UPDATE_ALL_SESSION_ACCESS_DATE).arg(sqlTimeData) )) {
        isOk = error("8");
    }
    if(!access.access->execQuery(context, QString(SQL_UPDATE_ALL_ACCESSES_DATE).arg(sqlTimeData) )) {
        isOk = error("9");
    }

    //access 3 inserted here
    if(!access.access->touchSession(context, &sessionModel1)) {
        isOk = error("10");
    }

    //access 3 inserted here
    if(!access.access->enrollFile(context, &sessionModel1, filePath2)) {
        isOk = error("11");
    }
    // clear data
    QDateTime dateTime = QDateTime::currentDateTime();
    dateTime = dateTime.addDays(-1);
    if( isErrorExpected ) {
        if(access.access->deleteSessionDataWithDateFilter(context, intervalType, dateTime, isFiles )) {
            isOk = error("12");
        }
        return isOk;
    }
    if(!access.access->deleteSessionDataWithDateFilter(context, intervalType, dateTime, isFiles )) {
        isOk = error("13");
    }
    // Now data are ready
    sessionModel1.clear();
    sessionModel2.clear();
    if(!access.access->readSessionData(context, &sessionModel1)) {
        isOk = error("14");
    }
    if(access.access->readSessionData(context, &sessionModel2)) {
        isOk = error("15");
    }
    if(sessionModel1.accesses.size() != 1) {
        isOk = error(QString("16: expected 1 entry, found: %1").arg(sessionModel1.accesses.size()));
    }
    // watch this
    if(sessionModel1.files.size() != 1) {
        isOk = error(QString("17: expected 1 entry, found: %1").arg(sessionModel1.files.size()));
    } else {
        FileModel *file = sessionModel1.files.at(0);
        if( file->path != filePath2 ) {
            isOk = error("18");
        }
    }
    SessionOperationStatus contextFiles;
    int cnt = access.access->countFiles(contextFiles);
    if( !contextFiles.ok) {
        isOk = error("19");
    }
    int filesExpected = 3 ;
    if( isFiles ) {
        filesExpected = 1 ;
    }

    if( filesExpected != cnt ) {
        isOk = error("20");
    }

    return isOk;
}
