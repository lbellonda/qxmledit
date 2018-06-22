/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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


#include "sqllitedataaccess.h"
#include "sqllitedataaccessprivate.h"
#include <QSqlError>
#include <QSqlQuery>
#include "model/genericpersistentdbdata.h"

#define LOG_TK "SQLLiteDataAccess"

#define SQL_DRIVER  "QSQLITE"

#define TABLE_SESSIONS  "SESSIONS"
#define TABLE_FILES     "FILES"
#define TABLE_ACCESSES  "FILE_SESSION_ACCESSES"
#define TABLE_VERSION  "VERSION"
//----------- SQL Commands -----------------

#define SQL_ENABLE_FOREIGN_KEYS "PRAGMA foreign_keys = ON"

#define SQL_CREATE_TABLE_SESSIONS_IF_NOT_EXISTS  ""\
    "create table if not exists SESSIONS ( "\
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"\
    "name varchar(64),"\
    "description varchar(256),"\
    "starred int default 0,"\
    "creationdate varchar(64),"\
    "updatedate varchar(64), " \
    "lastaccess varchar(64), " \
    "enabled int )"

#define SQL_CREATE_TABLE_FILES_IF_NOT_EXISTS  ""\
    "create table if not exists FILES ( "\
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"\
    "path varchar(2048),"\
    "starred int default 0,"\
    "description varchar(256),"\
    "creationdate varchar(64)"\
    ")"

#define SQL_CREATE_INDEX_FILES_IF_NOT_EXISTS  ""\
    "create index if not exists FILES_NAME_INDEX on "\
    " FILES "\
    "(path)"

#define SQL_CREATE_TABLE_SESSION_FILE_ACCESSES_IF_NOT_EXISTS  ""\
    "create table if not exists FILE_SESSION_ACCESSES ( "\
    "fileId integer NOT NULL,"\
    "sessionId integer NOT NULL,"\
    "accessDate varchar(64),"\
    "FOREIGN KEY(fileId) references " TABLE_FILES "(id),"\
    "FOREIGN KEY(sessionId) references " TABLE_SESSIONS"(id)"\
    ")"

#define SQL_CREATE_TABLE_VERSION_IF_NOT_EXISTS  ""\
    "create table if not exists " TABLE_VERSION " ( "\
    "version int,"\
    "versionId varchar(64),"\
    "name varchar(64)"\
    ")"

#define TABLE_ATTRNAMEFILTER_PROFILES  "AF_PROFILES"
#define TABLE_ATTRNAMEFILTER_NAMES "AF_NAMES"

#define SQL_CREATE_TABLE_ATTRNAMEFILTER_PROFILES_IF_NOT_EXISTS  ""\
    "create table if not exists " TABLE_ATTRNAMEFILTER_PROFILES " ( "\
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"\
    "name varchar(64),"\
    "description varchar(256),"\
    "iswhitelist int(1) default 1,"\
    "creationdate varchar(64),"\
    "updatedate varchar(64) ) "

#define SQL_CREATE_TABLE_ATTRNAMEFILTER_PROFILE_DETAIL_IF_NOT_EXISTS  ""\
    "create table if not exists " TABLE_ATTRNAMEFILTER_NAMES " ( "\
    "profileId integer NOT NULL,"\
    "name varchar(2048),"\
    "FOREIGN KEY(profileId) references " TABLE_ATTRNAMEFILTER_PROFILES "(id)"\
    ")"

#define SQL_CREATE_TABLE_GENERICOBJECT_IF_NOT_EXISTS    ""\
    "create table if not exists " TABLE_GENERIC_OBJECTS " ( "\
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"\
    "uuid varchar(256),"\
    "name varchar(64),"\
    "description varchar(1024),"\
    "payload blob,"\
    "type varchar(16),"\
    "creationuser varchar(256),"\
    "creationdate varchar(64),"\
    "updatedate varchar(64) " \
    ")"

#define SQL_CREATE_INDEX_GENERICTYPE_IF_NOT_EXISTS  ""\
    "create index if not exists GENERIC_TYPE_INDEX on "\
    TABLE_GENERIC_OBJECTS \
    " (type)"

#define SQL_CREATE_TABLE_TAGS_IF_NOT_EXISTS "" \
    "create table if not exists " TABLE_TAGS " ( "\
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"\
    "tag varchar(2048) NOT NULL, " \
    " UNIQUE ( tag ) ON CONFLICT FAIL "\
    ")"

#define SQL_CREATE_TABLE_GENERIC_TAGS_IF_NOT_EXISTS "" \
    "create table if not exists " TABLE_TAGS_RELATIONS" ( "\
    "tagId integer NOT NULL,"\
    "objectId varchar(2048),"\
    "FOREIGN KEY(tagId) references " TABLE_TAGS "(id)"\
    "FOREIGN KEY(objectId) references " TABLE_GENERIC_OBJECTS "(id)"\
    ")"

#define SQL_CREATE_INDEX_UNIQUETAGS_IF_NOT_EXISTS  ""\
    "create unique index if not exists TAGS_INDEX on "\
    TABLE_TAGS \
    " (tag)"

#define FILE_FIELDS "f.id, f.path, f.description, f.creationdate, f.starred"
#define OFFSET_ACCESS_FILEACCESS (5)

//--------------------------

SessionDataFactory::~SessionDataFactory()
{
}

//--------------------------

SessionDataInterface::~SessionDataInterface()
{
}

//--------------------------

SQLLiteDataAccess::SQLLiteDataAccess(QObject *parent) :
    QObject(parent),
    d(new SQLLiteDataAccess::Private(this))
{
}

SQLLiteDataAccess::~SQLLiteDataAccess()
{
    QString connectionName ;
    if(NULL != d) {
        connectionName = d->connectionName();
        delete d;
    }
    if(!connectionName.isEmpty()) {
        QSqlDatabase::removeDatabase(connectionName);
    }
}

SQLLiteDataAccess::Private *SQLLiteDataAccess::privatePart()
{
    return d;
}

bool SQLLiteDataAccess::init(const QString &configuration)
{
    return d->init(configuration);
}

void SQLLiteDataAccess::setLogger(FrwLogger *newLogger)
{
    d->setLogger(newLogger);
}

void SQLLiteDataAccess::closeAndDispose()
{
    d->close();
    //deleteLater();
    delete this ;
}

bool SQLLiteDataAccess::newSession(SessionOperationStatus &context, SessionModel *model)
{
    return d->newSession(context, model);
}

bool SQLLiteDataAccess::readSession(SessionOperationStatus &context, SessionModel *model)
{
    return d->readSession(context, model);
}

bool SQLLiteDataAccess::enrollFile(SessionOperationStatus &context, SessionModel *model, const QString &filePath)
{
    return d->enrollFile(context, model, filePath);
}

bool SQLLiteDataAccess::readSessionData(SessionOperationStatus &context, SessionModel *sessionModel)
{
    return d->readSessionData(context, sessionModel);
}

QVector<SessionListModel*> SQLLiteDataAccess::readSessionList(SessionOperationStatus &context)
{
    return d->readSessionList(context);
}

bool SQLLiteDataAccess::deleteSession(SessionOperationStatus &context, SessionModel *model)
{
    return d->deleteSession(context, model);
}

bool SQLLiteDataAccess::touchSession(SessionOperationStatus &context, SessionModel *model)
{
    return d->touchSession(context, model);
}

bool SQLLiteDataAccess::updateSession(SessionOperationStatus &context, SessionModel * model)
{
    return d->updateSession(context, model);
}

bool SQLLiteDataAccess::deleteAllSessionData(SessionOperationStatus &context)
{
    return d->deleteAllSessionData(context);
}

bool SQLLiteDataAccess::deleteSessionDataWithDateFilter(SessionOperationStatus &context, const EDateInterval dateRange, const QDateTime &reference, const bool isFilesToo)
{
    return d->deleteSessionDataWithDateFilter(context, dateRange, reference, isFilesToo);
}

bool SQLLiteDataAccess::execQuery(SessionOperationStatus &context, const QString &queryLiteral)
{
    return d->execQuery(context, queryLiteral);
}

int SQLLiteDataAccess::countFiles(SessionOperationStatus &context)
{
    return d->countFiles(context);
}

QList<AttrFilterProfile*> SQLLiteDataAccess::attributeNamesFilterReadProfiles(DataResult &result)
{
    return d->attributeNamesFilterReadProfiles(result);
}

AttrFilterDetail *SQLLiteDataAccess::attributeNamesFilterReadDetail(DataResult &result, const int idProfile)
{
    return d->attributeNamesFilterReadDetail(result, idProfile);
}

void SQLLiteDataAccess::attributeNamesFilterSaveProfile(DataResult &result, AttrFilterProfile *profile, AttrFilterDetail *detail)
{
    d->attributeNamesFilterSaveProfile(result, profile, detail);
}

void SQLLiteDataAccess::attributeNamesFilterDeleteProfile(DataResult &result, const int idProfile)
{
    d->attributeNamesFilterDeleteProfile(result, idProfile);
}

//------------------------------------------------------------------------------------------------------------

OperationStatus* SQLLiteDataAccess::insertGenericData(GenericPersistentData *model)
{
    return d->insertGenericData(model);
}

OperationStatus* SQLLiteDataAccess::updateGenericData(GenericPersistentData *model)
{
    return d->updateGenericData(model);
}

OperationStatus* SQLLiteDataAccess::deleteGenericData(GenericPersistentData *sessionModel)
{
    return d->deleteGenericData(sessionModel);
}

OperationStatus* SQLLiteDataAccess::readAllGenericData(const QString &type, QList<GenericPersistentData*> &resultList)
{
    return d->readAllGenericData(type, resultList);
}

GenericPersistentData *SQLLiteDataAccess::newPersistentDatum(const QString &type)
{
    return d->newPersistentDatum(type);
}

OperationStatus* SQLLiteDataAccess::readGenericData(const QString &type, const int id, QList<GenericPersistentData*> &resultList)
{
    return d->readGenericData(type, id, resultList);
}


//------------------------------------------------------------------------------------------------------------
SQLLiteDataAccess::Private::Private(SQLLiteDataAccess *publicInterface, QObject *parent) :
    QObject(parent),
    p(publicInterface)
{
    _dbIsOpen = false;
    _errorCode = E_NOERROR ;
    _dbErrorCode = 0 ;
    _isInited = false;
    _logger = NULL ;
    _logInfo.source = LOG_TK;
}

SQLLiteDataAccess::Private::~Private()
{
    close();
}

void SQLLiteDataAccess::Private::close()
{
    if(_db.isOpen() || _dbIsOpen) {
        _dbIsOpen = false;
        _db.close();
        if(NULL != _logger) {
            _logger->info(QString("database closed"), &_logInfo);
        }
    }
}

bool SQLLiteDataAccess::Private::init(const QString &configuration)
{
    if(!_isInited) {
        _isInited = true;
        if(!initDB(configuration)) {
            return false;
        }
    }
    return true;
}

QSqlDatabase SQLLiteDataAccess::Private::db()
{
    return _db;
}

void SQLLiteDataAccess::Private::setLogger(FrwLogger *newLogger)
{
    _logger = newLogger ;
}

FrwLogger *SQLLiteDataAccess::Private::logger()
{
    return _logger ;
}

void SQLLiteDataAccess::Private::setError(const SessionDataInterface::ErrorCode errorCode, const QString &errorMessage)
{
    _errorCode = errorCode;
    _errorMessage = errorMessage ;
    if(NULL != _logger) {
        _logger->error(QString("error %1").arg(errorMessage), &_logInfo);
    }
}

void SQLLiteDataAccess::Private::resetError()
{
    _errorCode = E_NOERROR;
    _errorMessage = "" ;
}

//tested
bool SQLLiteDataAccess::Private::openTrans()
{
    bool isOk = _db.transaction();
    if(!isOk) {
        if(_logger) {
            _logger->error("openTrans(): error starting transaction", &_logInfo);
        }
    }
    return isOk ;
}

//tested
bool SQLLiteDataAccess::Private::commitTrans()
{
    bool isOk = _db.commit();
    if(!isOk) {
        if(_logger) {
            _logger->error("commitTrans(): error committing transaction", &_logInfo);
        }
    }
    return isOk ;
}

//tested
bool SQLLiteDataAccess::Private::rollbackTrans()
{
    bool isOk = _db.rollback();
    if(!isOk) {
        if(_logger) {
            _logger->error("rollbackTrans(): error rolling back transaction", &_logInfo);
        }
    }
    return isOk ;
}

void SQLLiteDataAccess::Private::setError()
{
    QSqlError error = _db.lastError();
    _errorCode = E_UNKNOWN;
    _dbErrorText = error.text() ;
    _dbErrorCode = error.number() ;
    if(_logger) {
        _logger->error(QString("error %1").arg(_dbErrorText), &_logInfo);
    }
}

SQLLiteDataAccess::ErrorCode SQLLiteDataAccess::Private::getErrorCode()
{
    return _errorCode;
}

//tested
bool SQLLiteDataAccess::Private::initDB(const QString &configuration)
{
    if(_logger) {
        _logger->info(QString("%1 Started").arg(LOG_TK), &_logInfo);
    }
    QString newConnetionName = QString("qxmledit-%1").arg((ulong)(void*)this);
    _db = QSqlDatabase::addDatabase(SQL_DRIVER, newConnetionName);
    _db.setDatabaseName(configuration) ;
    if(!_db.open()) {
        QSqlError error = _db.lastError();
        setError(E_CANTOPEN, tr("can't open sessions database:%1").arg(error.text()));
        return false;
    }
    _connectionName = _db.connectionName();
    _dbIsOpen = true;
    bool isOk = createTables();
    if(_logger) {
        _logger->info(QString("%1 end started code %2").arg(LOG_TK).arg(isOk), &_logInfo);
    }
    return isOk ;
}

//tested chain
bool SQLLiteDataAccess::Private::createTables()
{
    QSqlQuery query(_db) ;
    if(_logger) {
        _logger->debug("create tables", &_logInfo);
    }
    if(!query.exec(SQL_ENABLE_FOREIGN_KEYS)) {
        setError();
        if(_logger) {
            _logger->error(QString("%1: error enabling foreign keys").arg(LOG_TK), &_logInfo);
        }
        return false ;
    }
    if(!query.exec(SQL_CREATE_TABLE_SESSIONS_IF_NOT_EXISTS)) {
        setError();
        if(_logger) {
            _logger->error(QString("%1: error creating sessions table").arg(LOG_TK), &_logInfo);
        }
        return false ;
    }
    if(!query.exec(SQL_CREATE_TABLE_FILES_IF_NOT_EXISTS)) {
        setError();
        if(_logger) {
            _logger->error(QString("%1: error creating files table").arg(LOG_TK), &_logInfo);
        }
        return false ;
    }
    if(!query.exec(SQL_CREATE_INDEX_FILES_IF_NOT_EXISTS)) {
        setError();
        if(_logger) {
            _logger->error(QString("%1: error creating index on files").arg(LOG_TK), &_logInfo);
        }
        return false ;
    }

    if(!query.exec(SQL_CREATE_TABLE_VERSION_IF_NOT_EXISTS)) {
        setError();
        if(_logger) {
            _logger->error(QString("%1: error creating version table").arg(LOG_TK), &_logInfo);
        }
        return false ;
    }

    if(!query.exec(SQL_CREATE_TABLE_SESSION_FILE_ACCESSES_IF_NOT_EXISTS)) {
        setError();
        if(_logger) {
            _logger->error(QString("%1: error creating accesses table").arg(LOG_TK), &_logInfo);
        }
        return false ;
    }

    if(!query.exec(SQL_CREATE_TABLE_ATTRNAMEFILTER_PROFILES_IF_NOT_EXISTS)) {
        setError();
        if(_logger) {
            _logger->error(QString("%1: error creating attribute names filter profile table").arg(LOG_TK), &_logInfo);
        }
        return false ;
    }

    if(!query.exec(SQL_CREATE_TABLE_ATTRNAMEFILTER_PROFILE_DETAIL_IF_NOT_EXISTS)) {
        setError();
        if(_logger) {
            _logger->error(QString("%1: error creating attribute names filter details table").arg(LOG_TK), &_logInfo);
        }
        return false ;
    }

    if(!query.exec(SQL_CREATE_TABLE_GENERICOBJECT_IF_NOT_EXISTS)) {
        setError();
        if(_logger) {
            _logger->error(QString("%1: error creating generic objects table").arg(LOG_TK), &_logInfo);
        }
        return false ;
    }

    if(!query.exec(SQL_CREATE_INDEX_GENERICTYPE_IF_NOT_EXISTS)) {
        setError();
        if(_logger) {
            _logger->error(QString("%1: error creating generic type index").arg(LOG_TK), &_logInfo);
        }
        return false ;
    }

    if(!query.exec(SQL_CREATE_TABLE_TAGS_IF_NOT_EXISTS)) {
        setError();
        if(_logger) {
            _logger->error(QString("%1: error creating tags table").arg(LOG_TK), &_logInfo);
        }
        return false ;
    }

    if(!query.exec(SQL_CREATE_TABLE_GENERIC_TAGS_IF_NOT_EXISTS)) {
        setError();
        if(_logger) {
            _logger->error(QString("%1: error creating tags relationship table").arg(LOG_TK), &_logInfo);
        }
        return false ;
    }

    if(!query.exec(SQL_CREATE_INDEX_UNIQUETAGS_IF_NOT_EXISTS)) {
        setError();
        if(_logger) {
            _logger->error(QString("%1: error creating unique index on tags table").arg(LOG_TK), &_logInfo);
        }
        return false ;
    }

    if(_logger) {
        _logger->debug(QString("%1 create tables end ok").arg(LOG_TK), &_logInfo);
    }
    return true ;
}

//tested
bool SQLLiteDataAccess::Private::enrollFile(SessionOperationStatus &context, SessionModel *model, const QString &filePath)
{
    context.ok = false ;
    if(_logger) {
        _logger->debug("SQLLiteDataAccess::Private::enrollFile enter", &_logInfo);
    }

    if(!openTrans()) {
        setError();
        if(_logger) {
            _logger->error("SQLLiteDataAccess::Private::enrollFile error starting transaction", &_logInfo);
        }
        context.ok = false;
    } else {
        if(enrollFileInternal(model, filePath)) {
            if(commitTrans()) {
                context.ok = true ;
            } else {
                setError();
                if(_logger) {
                    _logger->error("SQLLiteDataAccess::Private::enrollFile error committing", &_logInfo);
                }
            }
        } else {
            if(!rollbackTrans()) {
                setError();
                if(_logger) {
                    _logger->error("SQLLiteDataAccess::Private::enrollFile error rollbacking", &_logInfo);
                }
            }
        }
    }
    if(context.ok) {
        if(_logger) {
            _logger->debug("SQLLiteDataAccess::Private::enrollFile end ok", &_logInfo);
        }
    } else {
        if(_logger) {
            _logger->error("SQLLiteDataAccess::Private::enrollFile end ko", &_logInfo);
        }
    }
    return context.ok;
}

//tested chain
bool SQLLiteDataAccess::Private::enrollFileInternal(SessionModel *inputSessionModel, const QString &filePath)
{
    if(_logger) {
        _logger->debug("SQLLiteDataAccess::enrollFileInternal start ", &_logInfo);
    }
    SessionModel sessionModel;
    if(!readSessionModel(&sessionModel, inputSessionModel->id)) {
        if(_logger) {
            _logger->error("SQLLiteDataAccess::enrollFileInternal end ko", &_logInfo);
        }
        return false;
    }
    // look for file in file table
    // if not, insert it and get id
    // insert a relation into the relation table
    FileModel fileModel ;
    bool found = false;
    if(!getFile(&fileModel, filePath, found)) {
        if(_logger) {
            _logger->error("SQLLiteDataAccess::Private::enrollFileInternal end ko", &_logInfo);
        }
        return false;
    }
    if(!found) {
        if(_logger) {
            _logger->debug("enrollFileInternal file not found, inserting", &_logInfo);
        }
        fileModel.path = filePath ;
        if(!insertFile(&fileModel)) {
            if(_logger) {
                _logger->error("enrollFileInternal error inserting file", &_logInfo);
            }
            return false;
        }
    }
    if(!insertFileAccess(&sessionModel, &fileModel)) {
        if(_logger) {
            _logger->error("enrollFileInternal error inserting file access", &_logInfo);
        }
        return false;
    }
    return true;
}

#define SQL_INSERT_FILEACCESS  ""\
    "insert into FILE_SESSION_ACCESSES ( fileId, sessionId, accessDate) "\
    " values ( "\
    " :fileId, :sessionId, datetime('now', 'localtime') )"

//tested
bool SQLLiteDataAccess::Private::insertFileAccess(SessionModel *sessionModel, FileModel *fileModel)
{
    if(_logger) {
        _logger->debug("SQLLiteDataAccess::insertFileAccess enter", &_logInfo);
    }
    QSqlQuery query(_db) ;
    bool isOk = true ;
    query.prepare(SQL_INSERT_FILEACCESS);
    query.bindValue(":fileId", fileModel->id);
    query.bindValue(":sessionId", sessionModel->id);
    if(!query.exec()) {
        setError();
        if(_logger) {
            _logger->error("SQLLiteDataAccess::insertFileAccess exit error", &_logInfo);
        }
        isOk = false;
    } else {
        if(_logger) {
            _logger->debug("SQLLiteDataAccess::insertFileAccess exit ok", &_logInfo);
        }
    }
    return isOk ;
}

#define SQL_SELECT_FILE "select " FILE_FIELDS " from FILES f where f.path = :path"

//tested chain
void SQLLiteDataAccess::Private::readAFileModel(QSqlQuery &query, const int offset, FileModel *model)
{
    model->id = query.value(offset + 0).toInt();
    model->path = query.value(offset + 1).toString();
    model->description = query.value(offset + 2).toString();
    model->creationDate = query.value(offset + 3).toDateTime();
    model->starred = query.value(offset + 4).toInt();
}

//tested
// if the file is not found, its is is zero
bool SQLLiteDataAccess::Private::getFile(FileModel *model, const QString & path, bool &isFound)
{
    if(_logger) {
        _logger->debug("SQLLiteDataAccess::getFile enter", &_logInfo);
    }
    isFound = false;
    QSqlQuery query(_db) ;
    bool isOk = false;
    query.prepare(SQL_SELECT_FILE);
    query.bindValue(":path", path);
    if(query.exec()) {
        if(query.next()) {
            readAFileModel(query, 0, model);
            isFound = true;
            isOk = true;
        } else {
            if(_logger) {
                _logger->warning("SQLLiteDataAccess::getFile path not found", &_logInfo);
            }
            isOk = true;
            model->id = 0 ;
        }
    }
    query.finish();
    if(!isOk) {
        setError();
        if(_logger) {
            _logger->error("SQLLiteDataAccess::getFile ends in error", &_logInfo);
        }
    } else {
        if(_logger) {
            _logger->debug("SQLLiteDataAccess::getFile ends ok", &_logInfo);
        }
    }
    return isOk ;
}

#define SQL_INSERT_FILE  ""\
    "insert into FILES ( path, description, creationdate, starred) "\
    " values ( "\
    " :path, :description, datetime('now', 'localtime'), :starred )"

//tested
bool SQLLiteDataAccess::Private::insertFile(FileModel *model)
{
    if(_logger) {
        _logger->debug("SQLLiteDataAccess::insertFile enter", &_logInfo);
    }
    QSqlQuery query(_db) ;
    bool isOk = true ;
    query.prepare(SQL_INSERT_FILE);
    query.bindValue(":path", model->path);
    query.bindValue(":description", model->description);
    query.bindValue(":starred", model->starred);
    if(!query.exec()) {
        setError();
        isOk = false;
        if(_logger && _logger->isLoggable(FrwLogger::DEBUG)) {
            _logger->debug("SQLLiteDataAccess::insertFile ends in error", &_logInfo);
        }
    } else {
        model->id = query.lastInsertId().toInt();
        if(_logger && _logger->isLoggable(FrwLogger::DEBUG)) {
            _logger->debug(QString("SQLLiteDataAccess::insertFile ok id:%1").arg(model->id), &_logInfo);
        }
    }
    return isOk ;
}

#define SQL_INSERT_SESSION  ""\
    "insert into SESSIONS ( name, description, creationdate, updatedate, lastaccess, enabled, starred) "\
    " values ( "\
    " :name, :description, datetime('now', 'localtime'), datetime('now', 'localtime'), datetime('now', 'localtime'), :enabled, :starred)"

//tested chain
bool SQLLiteDataAccess::Private::insertSessionInternal(SessionModel *model)
{
    QSqlQuery query(_db) ;
    bool isOk = true ;
    query.prepare(SQL_INSERT_SESSION);
    query.bindValue(":name", model->name);
    query.bindValue(":description", model->description);
    query.bindValue(":enabled", model->enabled ? 1 : 0);
    query.bindValue(":starred", model->starred);
    if(!query.exec()) {
        setError();
        if(_logger) {
            _logger->error("error inserting a session");
        }
        isOk = false;
    } else {
        model->id = query.lastInsertId().toInt();
    }
    query.finish();
    return isOk ;
}

#define SQL_SELECT_SESSION "select id, name, description, creationdate, updatedate, lastaccess, enabled, starred from SESSIONS where id = :id"

//tested chain
void SQLLiteDataAccess::Private::readASessionModel(QSqlQuery &query, SessionModel *model)
{
    model->id = query.value(0).toInt();
    model->name = query.value(1).toString();
    model->description = query.value(2).toString();
    model->creationDate = query.value(3).toDateTime();
    model->updateDate = query.value(4).toDateTime();
    model->lastAccess = query.value(5).toDateTime();
    model->enabled = query.value(6).toBool();
    model->starred = query.value(7).toInt();
}

void SQLLiteDataAccess::Private::readAnAccessModel(QSqlQuery &query, const int offset, AccessModel *model)
{
    model->accessDate = query.value(offset).toDateTime();
}

//tested chain
bool SQLLiteDataAccess::Private::readSessionModel(SessionModel *model, const int sessionId)
{
    bool isOk = false ;
    if(_logger) {
        _logger->debug("SQLLiteDataAccess::readSessionModel", &_logInfo);
    }
    QSqlQuery query(_db) ;
    query.prepare(SQL_SELECT_SESSION);
    query.bindValue(":id", sessionId);
    if(query.exec()) {
        if(query.next()) {
            readASessionModel(query, model);
            isOk = true ;
        } else {
            if(_logger) {
                _logger->debug("readSessionModel: no data ", &_logInfo);
            }
        }
    } else {
        if(_logger) {
            _logger->error("readSessionModel: error in query", &_logInfo);
        }
    }
    query.finish();
    if(query.lastError().isValid()) {
        setError();
        isOk = false;
    }
    if(isOk) {
        if(_logger) {
            _logger->debug("readSessionModel: ok", &_logInfo);
        }
    } else {
        if(_logger) {
            _logger->error("readSessionModel: nok", &_logInfo);
        }
    }
    return isOk;
}

#define SQL_SELECT_SESS_ACC_F_SUM "select count(*) from FILE_SESSION_ACCESSES where fileId = :fileId and sessionId = :sessionId"

//tested
bool SQLLiteDataAccess::Private::countSessionFileAccesses(SessionModel *sessionModel, FileModel *fileModel, int &result)
{
    result = -1 ;
    bool isOk = false ;
    if(_logger) {
        _logger->debug("SQLLiteDataAccess::countSessionFileAccesses", &_logInfo);
    }
    QSqlQuery query(_db) ;
    query.prepare(SQL_SELECT_SESS_ACC_F_SUM);
    query.bindValue(":fileId", fileModel->id);
    query.bindValue(":sessionId", sessionModel->id);
    if(query.exec()) {
        if(query.next()) {
            result = query.value(0).toInt();
            isOk = true ;
        } else {
            if(_logger) {
                _logger->debug("countSessionFileAccesses: no data ", &_logInfo);
            }
        }
    } else {
        if(_logger) {
            _logger->error("countSessionFileAccesses: error in query", &_logInfo);
        }
    }
    query.finish();
    if(query.lastError().isValid()) {
        setError();
        isOk = false;
    }
    if(isOk) {
        if(_logger) {
            _logger->debug("countSessionFileAccesses: ok", &_logInfo);
        }
    } else {
        if(_logger) {
            _logger->error("countSessionFileAccesses: nok", &_logInfo);
        }
    }
    return isOk;
}

bool SQLLiteDataAccess::Private::SqlOperNewSession::doOper(SessionOperationStatus &context, SessionModel *model)
{
    if(!obj->insertSessionInternal(model)) {
        context.ok = false;
        return false;
    }
    return true ;
}



//tested
bool SQLLiteDataAccess::Private::newSession(SessionOperationStatus &context, SessionModel *model)
{
    SqlOperNewSession oper(this, "newSession");
    return genericTransaction(context, model, &oper);
}

//tested
bool SQLLiteDataAccess::Private::readSession(SessionOperationStatus &context, SessionModel *model)
{
    resetError();
    bool isOk = true ;
    if(_logger) {
        _logger->debug("SQLLiteDataAccess::Private::readSession", &_logInfo);
    }

    isOk = readSessionModel(model, model->id);
    if(isOk) {
        if(_logger) {
            _logger->debug("read session ok", &_logInfo);
        }
    } else {
        if(_logger) {
            _logger->debug("read session failed", &_logInfo);
        }
    }
    context.ok = isOk ;
    context.message = _errorMessage;
    return isOk;
}

/**
  \brief reads all the session data
  */

bool SQLLiteDataAccess::Private::readSessionData(SessionOperationStatus &context, SessionModel *sessionModel)
{
    resetError();
    bool isOk = true ;
    if(_logger) {
        _logger->debug("SQLLiteDataAccess::Private::readSessionData enter", &_logInfo);
    }
    if(!openTrans()) {
        setError();
        if(_logger) {
            _logger->error("error creating a session", &_logInfo);
        }
        isOk = false;
    } else {
        isOk = readSessionModel(sessionModel, sessionModel->id);
        if(isOk) {
            if(_logger) {
                _logger->debug("read session ok", &_logInfo);
            }
        } else {
            if(_logger) {
                _logger->debug("read session failed", &_logInfo);
            }
        }
        if(isOk) {
            if(readSessionFilesAndAccessesModel(sessionModel)) {
                if(_logger) {
                    _logger->debug("read accesses ok", &_logInfo);
                }
            } else {
                isOk = false ;
                if(_logger) {
                    _logger->debug("read accesses failed", &_logInfo);
                }
            }
        }
        if(isOk) {
            if(!commitTrans()) {
                setError();
                if(_logger) {
                    _logger->error("error committing", &_logInfo);
                }
                isOk = false;
            }
        } else {
            if(!rollbackTrans()) {
                setError();
                if(_logger) {
                    _logger->error("error rollbacking", &_logInfo);
                }
                isOk = false;
            }
        }
    }
    if(isOk) {
        if(_logger) {
            _logger->debug("readSessionData ok", &_logInfo);
        }
    } else {
        if(_logger) {
            _logger->debug("readSessionData failed", &_logInfo);
        }
    }
    context.ok = isOk ;
    context.message = _errorMessage;
    return isOk;
}

#define SQL_SELECT_FILE_AND_ACCESS_BY_SESSION  ""\
    "select " FILE_FIELDS ", a.accessDate from FILE_SESSION_ACCESSES a join FILES f on a.fileId = f.id and a.sessionId=:sessionId "

bool SQLLiteDataAccess::Private::readSessionFilesAndAccessesModel(SessionModel *sessionModel)
{
    bool isOk = true ;
    if(_logger) {
        _logger->debug(QString("SQLLiteDataAccess::readSessionFilesAndAccessesModel id=%1").arg(sessionModel->id), &_logInfo);
    }
    QSqlQuery query(_db) ;
    query.prepare(SQL_SELECT_FILE_AND_ACCESS_BY_SESSION);
    query.bindValue(":sessionId", sessionModel->id);
    QMap<int, FileModel*> filesMap;
    if(query.exec()) {
        while(query.next()) {
            if(_logger) {
                _logger->debug("SQLLiteDataAccess::an access", &_logInfo);
            }
            AccessModel *accessModel = new AccessModel();
            FileModel *fileModel = new FileModel();
            if((NULL == accessModel) || (NULL == fileModel)) {
                isOk = false;
            } else {
                readAFileModel(query, 0, fileModel);
                readAnAccessModel(query, OFFSET_ACCESS_FILEACCESS, accessModel);
                FileModel *theFileModel = fileModel;

                if(!filesMap.contains(fileModel->id)) {
                    filesMap.insert(fileModel->id, fileModel);
                    sessionModel->files.append(fileModel);
                } else {
                    theFileModel = filesMap.value(fileModel->id);
                    delete fileModel ;
                    fileModel = NULL;
                }
                accessModel->fileModel = theFileModel;
                theFileModel->accesses.append(accessModel);
                sessionModel->accesses.append(accessModel);
            }
            if(!isOk) {
                if(NULL != accessModel) {
                    delete accessModel ;
                }
                if(NULL != fileModel) {
                    delete fileModel ;
                }
                if(_logger) {
                    _logger->error("readSessionFilesAndAccessesModel unable to allocate data", &_logInfo);
                }
            }
        }// while
    } else {
        isOk = false;
        if(_logger) {
            _logger->error("readSessionFilesAndAccessesModel: error in query", &_logInfo);
        }
    }
    query.finish();
    if(query.lastError().isValid()) {
        setError();
        isOk = false;
    }
    if(isOk) {
        if(_logger) {
            _logger->debug("readSessionFilesAndAccessesModel: ok", &_logInfo);
        }
    } else {
        if(_logger) {
            _logger->error("readSessionFilesAndAccessesModel: nok", &_logInfo);
        }
    }
    return isOk;
}

#define SQL_SELECT_ALL_ACCESS ""\
    "select  s.id, s.name, s.description, s.creationDate, s.updateDate, s.lastAccess, s.enabled, s.starred from sessions s order by s.name"

#define SQL_SELECT_ACCESS_BY_SESSION  ""\
    "select  distinct s.id, s.name, s.description, s.creationDate, s.updateDate, s.lastAccess, s.enabled, s.starred, max(a.accessDate) as lastacc, "\
    " count(a.accessDate) as cnt from sessions s join file_session_accesses a on s.id=a.sessionId group by s.id, s.name, s.description, "\
    " s.creationDate, s.updateDate, s.lastAccess, s.enabled, s.starred"

QVector<SessionListModel*> SQLLiteDataAccess::Private::readSessionList(SessionOperationStatus &context)
{
    QVector<SessionListModel*> result;
    bool isOk = true ;
    if(_logger) {
        _logger->debug("SQLLiteDataAccess::readSessionList", &_logInfo);
    }
    QSqlQuery query(_db) ;
    query.prepare(SQL_SELECT_ACCESS_BY_SESSION);
    QSet<int> sessionMap;
    if(query.exec()) {
        while(query.next()) {
            if(_logger) {
                _logger->debug("SQLLiteDataAccess::session ", &_logInfo);
            }
            if(query.value(0).toInt() != 0) {
                SessionListModel *listModel = new SessionListModel();
                if(NULL == listModel) {
                    isOk = false;
                } else {
                    readListModelBase(query, listModel, false);
                    result.append(listModel);
                    sessionMap.insert(listModel->session.id);
                }
            }
        }// while
    } else {
        isOk = false;
        if(_logger) {
            _logger->error("readSessionList: error in query", &_logInfo);
        }
    }
    query.finish();
    if(query.lastError().isValid()) {
        setError();
        isOk = false;
    }

    if(! readAndAppendAllSessions(&sessionMap, &result)) {
        isOk = false;
    }

    if(isOk) {
        if(_logger) {
            _logger->debug("readSessionList: ok", &_logInfo);
        }
    } else {
        if(_logger) {
            _logger->error("readSessionList: nok", &_logInfo);
        }
    }
    context.ok = isOk;
    return result;
}

void SQLLiteDataAccess::Private::readListModelBase(QSqlQuery &query, SessionListModel *model, const bool sessionOnly)
{
    model->session.id = query.value(0).toInt();
    model->session.name = query.value(1).toString();
    model->session.description = query.value(2).toString();
    model->session.creationDate = query.value(3).toDateTime();
    model->session.updateDate = query.value(4).toDateTime();
    model->session.lastAccess = query.value(5).toDateTime();
    model->session.enabled = query.value(6).toBool();
    model->session.starred = query.value(7).toInt();
    if(!sessionOnly) {
        model->lastAccess = query.value(8).toDateTime();
        model->count = query.value(9).toInt();
    }
}

bool SQLLiteDataAccess::Private::readAndAppendAllSessions(QSet<int> *sessionMap, QVector<SessionListModel*> *result)
{
    bool isOk = true ;
    if(_logger) {
        _logger->debug("SQLLiteDataAccess::readAndAppendAllSessions", &_logInfo);
    }
    QSqlQuery query(_db) ;
    query.prepare(SQL_SELECT_ALL_ACCESS);
    if(query.exec()) {
        while(query.next()) {
            if(_logger) {
                _logger->debug("SQLLiteDataAccess::session ", &_logInfo);
            }
            int id = query.value(0).toInt();
            if(!sessionMap->contains(id)) {
                if(_logger) {
                    _logger->debug("SQLLiteDataAccess::adding missing session ", &_logInfo);
                }
                SessionListModel *listModel = new SessionListModel();
                if(NULL == listModel) {
                    isOk = false;
                } else {
                    readListModelBase(query, listModel, true);
                    result->append(listModel);
                    sessionMap->insert(listModel->session.id);
                }
            }
        }// while
    } else {
        isOk = false;
        if(_logger) {
            _logger->error("readAndAppendAllSessions: error in query", &_logInfo);
        }
    }
    query.finish();
    if(query.lastError().isValid()) {
        setError();
        isOk = false;
    }

    if(isOk) {
        if(_logger) {
            _logger->debug("readAndAppendAllSessions: ok", &_logInfo);
        }
    } else {
        if(_logger) {
            _logger->error("readAndAppendAllSessions: nok", &_logInfo);
        }
    }
    return isOk;
}

bool SQLLiteDataAccess::Private::SqlOperDeleteSession::doOper(SessionOperationStatus &context, SessionModel *model)
{
    if(!obj->deleteSessionInternal(model)) {
        context.ok = false;
        return false;
    }
    return true ;
}

bool SQLLiteDataAccess::Private::deleteSession(SessionOperationStatus &context, SessionModel *model)
{
    SqlOperDeleteSession oper(this, "deleteSession");
    return genericTransaction(context, model, &oper);
}


#define SQL_DELETE_SESSION_ACCESSES  ""\
    "delete from FILE_SESSION_ACCESSES where sessionId=:sessionId "
#define SQL_DELETE_SESSION  ""\
    "delete from SESSIONS where id=:sessionId "

bool SQLLiteDataAccess::Private::deleteSessionInternal(SessionModel *sessionModel)
{
    bool isOk = true ;
    if(_logger) {
        _logger->debug(QString("SQLLiteDataAccess::deleteSessionInternal id=%1").arg(sessionModel->id), &_logInfo);
    }
    {
        QSqlQuery query(_db) ;
        query.prepare(SQL_DELETE_SESSION_ACCESSES);
        query.bindValue(":sessionId", sessionModel->id);
        if(!query.exec()) {
            isOk = false;
            if(_logger) {
                _logger->error("delete session accesses: error in query", &_logInfo);
            }
        }
        query.finish();
        if(query.lastError().isValid()) {
            setError();
            isOk = false;
        }
    }
    if(isOk) {
        QSqlQuery query2(_db) ;
        query2.prepare(SQL_DELETE_SESSION);
        query2.bindValue(":sessionId", sessionModel->id);
        if(!query2.exec()) {
            isOk = false;
            if(_logger) {
                _logger->error("delete session: error in query", &_logInfo);
            }
        }
        query2.finish();
        if(query2.lastError().isValid()) {
            setError();
            isOk = false;
        }
    }

    if(isOk) {
        if(_logger) {
            _logger->debug("deleteSessionInternal: ok", &_logInfo);
        }
    } else {
        if(_logger) {
            _logger->error("deleteSessionInternal: nok", &_logInfo);
        }
    }
    return isOk;
}

#define SQL_TOUCH_SESSION  ""\
    "update SESSIONS set lastaccess=datetime('now', 'localtime') where id=:sessionId "

bool SQLLiteDataAccess::Private::touchSessionInternal(SessionModel *sessionModel)
{
    bool isOk = true ;
    if(_logger) {
        _logger->debug(QString("SQLLiteDataAccess::touchSessionInternal id=%1").arg(sessionModel->id), &_logInfo);
    }
    {
        QSqlQuery query(_db) ;
        query.prepare(SQL_TOUCH_SESSION);
        query.bindValue(":sessionId", sessionModel->id);
        if(!query.exec()) {
            isOk = false;
            if(_logger) {
                _logger->error("touch session: error in query", &_logInfo);
            }
        }
        query.finish();
        if(query.lastError().isValid()) {
            setError();
            isOk = false;
        }
    }
    if(isOk) {
        if(_logger) {
            _logger->debug("touchSessionInternal: ok", &_logInfo);
        }
    } else {
        if(_logger) {
            _logger->error("touchSessionInternal: nok", &_logInfo);
        }
    }
    return isOk;
}

bool SQLLiteDataAccess::Private::SqlOperTouchSession::doOper(SessionOperationStatus &context, SessionModel *model)
{
    if(!obj->touchSessionInternal(model)) {
        context.ok = false;
        return false;
    }
    return true ;
}


bool SQLLiteDataAccess::Private::touchSession(SessionOperationStatus &context, SessionModel *model)
{
    SqlOperTouchSession oper(this, "touchSession");
    return genericTransaction(context, model, &oper);
}

#define SQL_UPDATE_SESSION  ""\
    "update SESSIONS set name=:name, description=:description, lastaccess=datetime('now', 'localtime'), starred=:starred where id=:sessionId "

bool SQLLiteDataAccess::Private::updateSessionInternal(SessionModel *sessionModel)
{
    bool isOk = true ;
    if(_logger) {
        _logger->debug(QString("SQLLiteDataAccess::updateSessionInternal id=%1").arg(sessionModel->id), &_logInfo);
    }
    {
        QSqlQuery query(_db) ;
        query.prepare(SQL_UPDATE_SESSION);
        query.bindValue(":sessionId", sessionModel->id);
        query.bindValue(":name", sessionModel->name);
        query.bindValue(":description", sessionModel->description);
        query.bindValue(":starred", sessionModel->starred);
        if(!query.exec()) {
            isOk = false;
            if(_logger) {
                _logger->error("updateSession: error in query", &_logInfo);
            }
        }
        query.finish();
        if(query.lastError().isValid()) {
            setError();
            isOk = false;
        }
    }
    if(isOk) {
        if(_logger) {
            _logger->debug("updateSessionInternal: ok", &_logInfo);
        }
    } else {
        if(_logger) {
            _logger->error("updateSessionInternal: nok", &_logInfo);
        }
    }
    return isOk;
}

bool SQLLiteDataAccess::Private::updateSession(SessionOperationStatus &context, SessionModel *model)
{
    SqlOperUpdateSession oper(this, "updateSession");
    return genericTransaction(context, model, &oper);
}

bool SQLLiteDataAccess::Private::SqlOperUpdateSession::doOper(SessionOperationStatus &context, SessionModel *model)
{
    if(!obj->updateSessionInternal(model)) {
        context.ok = false;
        return false ;
    }
    return true;
}

//-------------------------------------------------------

bool SQLLiteDataAccess::Private::genericTransaction(SessionOperationStatus &context, SessionModel *model, SqlOperBase *baseObj)
{
    resetError();
    bool isOk = true ;
    if(_logger) {
        _logger->debug(QString("Enter %1").arg(baseObj->idOperaz), &_logInfo);
    }

    if(!openTrans()) {
        setError();
        if(_logger) {
            _logger->error(QString("error opering transaction %1").arg(baseObj->idOperaz), &_logInfo);
        }
        isOk = false;
    } else {
        isOk = baseObj->doOper(context, model);
        if(isOk) {
            if(!commitTrans()) {
                setError();
                if(_logger) {
                    _logger->error("error committing", &_logInfo);
                }
                isOk = false;
            }
        } else {
            if(!rollbackTrans()) {
                setError();
                if(_logger) {
                    _logger->error("error rollbacking", &_logInfo);
                }
                isOk = false;
            }
        }
    }
    if(isOk) {
        if(_logger) {
            _logger->debug(QString("%1 ok").arg(baseObj->idOperaz), &_logInfo);
        }
    } else {
        if(_logger) {
            _logger->error(QString("%1 failed").arg(baseObj->idOperaz), &_logInfo);
        }
    }
    context.ok = isOk ;
    context.message = _errorMessage;
    return isOk;
}


//-----------------------------------------------------------------
#define SQL_VACUUM "vacuum"
bool SQLLiteDataAccess::Private::deleteAllSessionData(SessionOperationStatus &context)
{
    SqlOperDeleteAllSessionData oper(this, "deleteAllSessionData");
    if(genericTransaction(context, NULL, &oper)) {
        QSqlQuery query(_db) ;
        query.prepare(SQL_VACUUM);
        if(!execQuery(query, "deleteAllSessionData: error in vacuumm")) {
            context.ok = false ;
            return false;
        }
    }
    return true ;
}

bool SQLLiteDataAccess::Private::SqlOperDeleteAllSessionData::doOper(SessionOperationStatus &context, SessionModel * /*model*/)
{
    if(!obj->deleteAllSessionDataInternal()) {
        context.ok = false;
        return false;
    }
    return true ;
}

#define SQL_DELETE_ALL_ACCESSES "delete from " TABLE_ACCESSES
#define SQL_DELETE_ALL_FILES "delete from " TABLE_FILES
#define SQL_DELETE_ALL_SESSIONS "delete from " TABLE_SESSIONS

bool SQLLiteDataAccess::Private::deleteAllSessionDataInternal()
{
    bool isOk = true ;
    if(_logger) {
        _logger->debug(QString("SQLLiteDataAccess::deleteAllSessionDataInternal."), &_logInfo);
    }
    {
        QSqlQuery query(_db) ;
        query.prepare(SQL_DELETE_ALL_ACCESSES);
        if(!query.exec()) {
            isOk = false;
            if(_logger) {
                _logger->error("deleteAllSessionDataInternal: error deleting accesses", &_logInfo);
            }
        }
        query.finish();
        if(query.lastError().isValid()) {
            setError();
            isOk = false;
        }
    }
    if(isOk) {
        QSqlQuery query(_db) ;
        query.prepare(SQL_DELETE_ALL_FILES);
        if(!query.exec()) {
            isOk = false;
            if(_logger) {
                _logger->error("deleteAllSessionDataInternal: error deleting files", &_logInfo);
            }
        }
        query.finish();
        if(query.lastError().isValid()) {
            setError();
            isOk = false;
        }
    }
    if(isOk) {
        QSqlQuery query(_db) ;
        query.prepare(SQL_DELETE_ALL_SESSIONS);
        if(!query.exec()) {
            isOk = false;
            if(_logger) {
                _logger->error("deleteAllSessionDataInternal: error deleting sessions", &_logInfo);
            }
        }
        query.finish();
        if(query.lastError().isValid()) {
            setError();
            isOk = false;
        }
    }
    if(isOk) {
        if(_logger) {
            _logger->debug("deleteAllSessionData: ok", &_logInfo);
        }
    } else {
        if(_logger) {
            _logger->error("deleteAllSessionData: nok", &_logInfo);
        }
    }
    return isOk;
}

//-----------------------------------------------------------------

QDateTime SQLLiteDataAccess::Private::findDate(const SessionDataInterface::EDateInterval dateRange, const QDateTime &reference, bool &isOk)
{
    isOk = true ;
    switch(dateRange) {
    default:
        isOk = false;
        return QDateTime::currentDateTime().addYears(-1000);
    case SessionDataInterface::DATE_INTERVAL_1_MONTH:
        return QDateTime::currentDateTime().addMonths(-1);
    case SessionDataInterface::DATE_INTERVAL_6_MONTHS:
        return QDateTime::currentDateTime().addMonths(-6);
    case SessionDataInterface::DATE_INTERVAL_LITERAL:
        return reference;
    }
}

bool SQLLiteDataAccess::Private::deleteSessionDataWithDateFilter(SessionOperationStatus &context, const EDateInterval dateRange, const QDateTime &reference, const bool isFilesToo)
{
    SqlOperDeleteAllSessionDataWithDateFilter oper(this, "deleteAllSessionDataWithDateFilter", dateRange, reference, isFilesToo);
    return genericTransaction(context, NULL, &oper);
}

bool SQLLiteDataAccess::Private::SqlOperDeleteAllSessionDataWithDateFilter::doOper(SessionOperationStatus &context, SessionModel * /*model*/)
{
    if(!obj->deleteSessionDataInternalDate(dateRange, reference, isFileToo)) {
        context.ok = false;
        return false ;
    }
    return true;
}

#define SQL_DELETE_ALL_ACCESSES_BYDATE "delete from " TABLE_ACCESSES " where accessDate < :dateTarget"
#define SQL_DELETE_ALL_FILES_BYDATE "delete from " TABLE_FILES " where id not in (select fileid from " TABLE_ACCESSES " )"
#define SQL_DELETE_ALL_SESSIONS_BYDATE "delete from " TABLE_SESSIONS "  where (lastAccess < :dateTarget) and id not in (select sessionid from " TABLE_ACCESSES " )"

bool SQLLiteDataAccess::Private::deleteSessionDataInternalDate(const SessionDataInterface::EDateInterval dateRange, const QDateTime &reference, const bool isDeleteFiles)
{
    bool isOk = true ;
    if(_logger) {
        _logger->debug(QString("SQLLiteDataAccess::deleteSessionDataInternalDate."), &_logInfo);
    }
    QDateTime limitDate = findDate(dateRange, reference, isOk);
    if(!isOk) {
        if(_logger) {
            _logger->error("deleteSessionDataInternalDate: invalid date", &_logInfo);
        }
        return false;
    }
    if(_logger) {
        _logger->info(QString("deleteSessionDataInternalDate: date is %1").arg(limitDate.toString(Qt::DefaultLocaleShortDate)), &_logInfo);
    }

    {
        QSqlQuery query(_db) ;
        query.prepare(SQL_DELETE_ALL_ACCESSES_BYDATE);
        query.bindValue(":dateTarget", limitDate);
        if(!execQuery(query, "deleteSessionDataInternalDate: error deleting accesses")) {
            isOk = false;
        }
    }
    if(isOk && isDeleteFiles) {
        if(!execQuery(SQL_DELETE_ALL_FILES_BYDATE, "deleteSessionDataInternalDate: error deleting files")) {
            isOk = false;
        }
    }
    if(isOk) {
        QSqlQuery query(_db) ;
        query.prepare(SQL_DELETE_ALL_SESSIONS_BYDATE);
        query.bindValue(":dateTarget", limitDate);
        if(!execQuery(query, "deleteSessionDataInternalDate: error deleting sessions")) {
            isOk = false;
        }
    }
    if(isOk) {
        if(_logger) {
            _logger->debug("deleteSessionDataInternalDate: ok", &_logInfo);
        }
    } else {
        if(_logger) {
            _logger->error("deleteSessionDataInternalDate: nok", &_logInfo);
        }
    }
    return isOk;
}


bool SQLLiteDataAccess::Private::execQuery(const QString &queryLiteral, const QString &logData)
{
    bool isOk = true ;
    QSqlQuery query(_db) ;
    query.prepare(queryLiteral);
    if(!query.exec()) {
        isOk = false;
        if(_logger) {
            _logger->error(logData, &_logInfo);
        }
    }
    query.finish();
    if(query.lastError().isValid()) {
        setError();
        isOk = false;
    }
    return isOk;
}

bool SQLLiteDataAccess::Private::execLiteralInternal(const QString &literal)
{
    bool isOk = true ;
    if(_logger) {
        _logger->debug(QString("SQLLiteDataAccess::execLiteralInternal"), &_logInfo);
    }
    {
        QSqlQuery query(_db) ;
        query.prepare(literal);
        if(!query.exec()) {
            isOk = false;
            if(_logger) {
                _logger->error("execLiteralInternal: error in query", &_logInfo);
            }
        }
        query.finish();
        if(query.lastError().isValid()) {
            setError();
            isOk = false;
        }
    }
    if(isOk) {
        if(_logger) {
            _logger->debug("execLiteralInternal: ok", &_logInfo);
        }
    } else {
        if(_logger) {
            _logger->error("execLiteralInternal: nok", &_logInfo);
        }
    }
    return isOk;
}

bool SQLLiteDataAccess::Private::execQuery(SessionOperationStatus &context, const QString &queryLiteral)
{
    SqlOperLiteralQuery oper(this, "utilityExecQuery", queryLiteral);
    return genericTransaction(context, NULL, &oper);
}

bool SQLLiteDataAccess::Private::SqlOperLiteralQuery::doOper(SessionOperationStatus &context, SessionModel * /*model*/)
{
    if(!obj->execLiteralInternal(literal)) {
        context.ok = false;
        return false ;
    }
    return true;
}

//------------------------------------------------------------------------
bool SQLLiteDataAccess::Private::execQuery(QSqlQuery &query, const QString &logData)
{
    bool isOk = true ;
    if(!query.exec()) {
        isOk = false;
        if(_logger) {
            _logger->error(logData, &_logInfo);
        }
    }
    query.finish();
    if(query.lastError().isValid()) {
        setError();
        isOk = false;
    }
    return isOk;
}

#define SQL_COUNT_GENERIC "select count(*) from %1"

bool SQLLiteDataAccess::Private::countInternal(int &result, const QString &table)
{
    bool isOk = false ;
    if(_logger) {
        _logger->debug(QString("SQLLiteDataAccess::countInternal %1").arg(table), &_logInfo);
    }
    QSqlQuery query(_db) ;
    query.prepare(QString(SQL_COUNT_GENERIC).arg(table));
    if(query.exec()) {
        if(query.next()) {
            result = query.value(0).toInt();
            isOk = true;
            if(_logger && _logger->isLoggable(FrwLogger::DEBUG)) {
                _logger->debug(QString("countInternal: #:%1").arg(result), &_logInfo);
            }

        }
    }
    query.finish();
    if(query.lastError().isValid()) {
        setError();
        isOk = false;
    }
    if(isOk) {
        if(_logger && _logger->isLoggable(FrwLogger::DEBUG)) {
            _logger->debug("countInternal: ok", &_logInfo);
        }
    } else {
        if(_logger) {
            _logger->error("countInternal: nok", &_logInfo);
        }
    }
    return isOk;
}

int SQLLiteDataAccess::Private::countFiles(SessionOperationStatus &context)
{
    SqlOperCountQuery oper(this, "countFiles", TABLE_FILES);
    genericTransaction(context, NULL, &oper);
    return oper.result;
}

bool SQLLiteDataAccess::Private::SqlOperCountQuery::doOper(SessionOperationStatus &context, SessionModel * /*model*/)
{
    if(!obj->countInternal(result, table)) {
        context.ok = false;
        return false ;
    }
    return true;
}


//------------- generic -------------------------------------

bool SQLLiteDataAccess::Private::genericTransaction(DataResult &result, SQLLiteDataAccess::Private::SqlOperBaseGeneric *baseObj)
{
    resetError();
    bool isOk = true ;
    if(_logger) {
        _logger->debug(QString("Enter %1").arg(baseObj->name()), &_logInfo);
    }

    if(!openTrans()) {
        setError();
        if(_logger) {
            _logger->error(QString("error opening transaction %1").arg(baseObj->name()), &_logInfo);
        }
        isOk = false;
    } else {
        baseObj->doOper(result, this);
        isOk = result.isOk();
        if(isOk) {
            if(!commitTrans()) {
                setError();
                if(_logger) {
                    _logger->error("error committing", &_logInfo);
                }
                isOk = false;
            }
        } else {
            if(!rollbackTrans()) {
                setError();
                if(_logger) {
                    _logger->error("error rollbacking", &_logInfo);
                }
                isOk = false;
            }
        }
    }
    if(isOk) {
        if(_logger) {
            _logger->debug(QString("%1 ok").arg(baseObj->name()), &_logInfo);
        }
    } else {
        if(_logger) {
            _logger->error(QString("%1 failed").arg(baseObj->name()), &_logInfo);
        }
    }
    result.setOk(isOk);
    if(result.message().isEmpty()) {
        result.setMessage(_errorMessage);
    }
    return isOk;
}

bool SQLLiteDataAccess::Private::execEnum(DataResult &dataResult, SQLLiteDataAccess::Private::SqlEnumOper *oper, QSqlQuery &query)
{
    bool isOk = true ;
    if(_logger) {
        _logger->debug(QString("enum %1 start").arg(oper->name()), &_logInfo);
    }
    if(query.exec()) {
        while(query.next()) {
            if(_logger) {
                _logger->debug("enum hit", &_logInfo);
            }
            oper->onData(dataResult, query);
            if(!dataResult.isOk()) {
                isOk = false;
                break;
            }
        }// while
    } else {
        isOk = false;
        if(_logger) {
            _logger->error("enum: error in query", &_logInfo);
        }
    }
    query.finish();
    if(query.lastError().isValid()) {
        setError();
        isOk = false;
    }

    if(isOk) {
        if(_logger) {
            _logger->debug(QString("enum %1 ok").arg(oper->name()), &_logInfo);
        }
    } else {
        if(_logger) {
            _logger->error(QString("enum %1 nok").arg(oper->name()), &_logInfo);
        }
    }
    return isOk;
}


//-----------------------------------------------------------------------

QList<AttrFilterProfile*> SQLLiteDataAccess::Private::attributeNamesFilterReadProfiles(DataResult &result)
{
    result.setOk(true);
    AttributeNamesFilterReadProfiles readProfiles;
    if(!genericTransaction(result, &readProfiles)) {
        result.setOk(false);
    }
    return readProfiles.list;
}

AttrFilterDetail *SQLLiteDataAccess::Private::attributeNamesFilterReadDetail(DataResult &result, const int idProfile)
{
    result.setOk(true);
    AttributeNamesFilterReadDetail readDetail(idProfile);
    if(!genericTransaction(result, &readDetail)) {
        result.setOk(false);
    }
    return readDetail.detail;
}

void SQLLiteDataAccess::Private::attributeNamesFilterSaveProfile(DataResult &result, AttrFilterProfile *profile, AttrFilterDetail *detail)
{
    AttributeNamesFilterSaveProfile saveProfile(profile, detail);
    if(!genericTransaction(result, &saveProfile)) {
        result.setOk(false);
    }
}

void SQLLiteDataAccess::Private::attributeNamesFilterDeleteProfile(DataResult &result, const int idProfile)
{
    result.setOk(true);
    AttributeNamesFilterDelete deleteProfile(idProfile);
    if(!genericTransaction(result, &deleteProfile)) {
        result.setOk(false);
    }
}


//-----------------------------------------------------------------------
OperationStatus* SQLLiteDataAccess::Private::insertGenericData(GenericPersistentData * model)
{
    DataResult *result = new DataResult();
    result->setOk(true);
    GenericObjectInsert writer(model);
    if(!genericTransaction(*result, &writer)) {
        result->setOk(false);
    }
    return result;
}

OperationStatus* SQLLiteDataAccess::Private::updateGenericData(GenericPersistentData * model)
{
    DataResult *result = new DataResult();
    result->setOk(true);
    GenericObjectUpdate writer(model);
    if(!genericTransaction(*result, &writer)) {
        result->setOk(false);
    }
    return result;
}

OperationStatus* SQLLiteDataAccess::Private::deleteGenericData(GenericPersistentData * model)
{
    DataResult *result = new DataResult();
    result->setOk(true);
    GenericObjectDelete writer(model);
    if(!genericTransaction(*result, &writer)) {
        result->setOk(false);
    }
    return result;
}

OperationStatus* SQLLiteDataAccess::Private::readAllGenericData(const QString & type, QList<GenericPersistentData*> &resultList)
{
    DataResult *result = new DataResult();
    result->setOk(true);
    GenericObjectRead readManager(type);
    if(!genericTransaction(*result, &readManager)) {
        result->setOk(false);
    } else {
        foreach(GenericPersistentData * d, readManager.list) {
            resultList.append(d);
        }
    }
    return result;
}

OperationStatus* SQLLiteDataAccess::Private::readGenericData(const QString & type, const int id, QList<GenericPersistentData*> &resultList)
{
    DataResult *result = new DataResult();
    result->setOk(true);
    GenericObjectRead readManager(type, id);
    if(!genericTransaction(*result, &readManager)) {
        result->setOk(false);
    } else {
        foreach(GenericPersistentData * d, readManager.list) {
            resultList.append(d);
        }
    }
    return result;
}


GenericPersistentData *SQLLiteDataAccess::Private::newPersistentDatum(const QString & type)
{
    GenericPersistentDBData *newData = new GenericPersistentDBData();
    if(NULL != newData) {
        newData->setType(type);
    }
    return newData;
}
