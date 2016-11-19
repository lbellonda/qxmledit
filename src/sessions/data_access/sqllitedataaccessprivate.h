/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
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


#ifndef SQLLITEDATAACCESSPRIVATE_H
#define SQLLITEDATAACCESSPRIVATE_H

#include <QObject>
#include "sqllitedataaccess.h"
#include <QSqlDatabase>
#include "model/filemodel.h"
#include "dataresult.h"
#include "model/attrfilterdetail.h"
#include "model/attrfilterprofile.h"


//-- definitions

#define TABLE_ATTRNAMEFILTER_PROFILES  "AF_PROFILES"
#define TABLE_ATTRNAMEFILTER_NAMES "AF_NAMES"

#define TABLE_TAGS      "TAGS"
#define TABLE_GENERIC_OBJECTS   "GENERIC_OBJECTS"
#define TABLE_TAGS_RELATIONS "OBJECT_AND_TAGS"


class SQLLiteDataAccess::Private : public QObject
{
    Q_OBJECT
    SQLLiteDataAccess * const p;
    friend class TestSQLAccess;
    friend class TestSearchlets;

    class SqlOperBase;

    bool _isInited;
    bool _dbIsOpen;
    QString _connectionName;
    QSqlDatabase _db;
    QString _errorMessage;
    SessionDataInterface::ErrorCode _errorCode;
    int     _dbErrorCode;
    QString _dbErrorText;
    FrwLogger *_logger;
    LogInfo _logInfo;

    bool enrollFileInternal(SessionModel *model, const QString &filePath);

    bool insertFileAccess(SessionModel *sessionModel, FileModel *fileModel);
    bool initDB(const QString &configuration);
    void setError(const SessionDataInterface::ErrorCode errorCode, const QString &errorMessage);
    void setError();
    bool createTables();
    void readAFileModel(QSqlQuery &query, const int offset, FileModel *model);
    void readASessionModel(QSqlQuery &query, SessionModel *model);
    void readAnAccessModel(QSqlQuery &query, const int offset, AccessModel *model);
    bool readSessionModel(SessionModel *model, const int sessionId);
    bool getFile(FileModel *model, const QString & path, bool &isFound);
    bool insertFile(FileModel *model);
    bool insertSessionInternal(SessionModel *model);
    void resetError();
    bool countSessionFileAccesses(SessionModel *sessionModel, FileModel *fileModel, int &result);
    bool readSessionFilesAndAccessesModel(SessionModel *sessionModel);
    void readListModelBase(QSqlQuery &query, SessionListModel *model, const bool sessionOnly);
    bool readAndAppendAllSessions(QSet<int> *sessionMap, QVector<SessionListModel*> *result);
    bool deleteSessionInternal(SessionModel *sessionModel);
    bool touchSessionInternal(SessionModel *sessionModel);
    bool genericTransaction(SessionOperationStatus &context, SessionModel *model, SqlOperBase *baseObj);
    bool updateSessionInternal(SessionModel *sessionModel);
    bool deleteAllSessionDataInternal();
    bool deleteSessionDataInternalDate(const SessionDataInterface::EDateInterval dateRange, const QDateTime &reference, const bool isDeleteFiles);
    QDateTime findDate(const SessionDataInterface::EDateInterval dateRange, const QDateTime &reference, bool &isOk);
    bool execLiteralInternal(const QString &literal);
    bool countInternal(int &result, const QString &table);
    QSqlDatabase db();

    bool execQuery(const QString &queryLiteral, const QString &logData);
    bool execQuery(QSqlQuery &query, const QString &logData);

    //------------------------------------------------------------

    // base of the generic operations
    class SqlOperBaseCommon
    {
    public:
        virtual QString name() = 0 ;
    };

    class SqlOperBaseGeneric : public SqlOperBaseCommon
    {
    public:
        virtual void doOper(DataResult &context, SQLLiteDataAccess::Private *db) = 0;
    };

    // interface for enumerations
    class SqlEnumOper : public SqlOperBaseCommon
    {
    public:
        virtual void onData(DataResult &context, QSqlQuery &query) = 0;
    };

    //------------------------------------------------------------

    bool execEnum(DataResult &dataResult, SqlEnumOper *oper, QSqlQuery &query);
    bool genericTransaction(DataResult &context, SqlOperBaseGeneric *baseObj);


public:
    explicit Private(SQLLiteDataAccess *publicInterface, QObject *parent = 0);
    ~Private();

    bool init(const QString &configuration) ;
    void close();

    virtual bool enrollFile(SessionOperationStatus &context, SessionModel *model, const QString &filePath);
    virtual bool newSession(SessionOperationStatus &context, SessionModel *model) ;
    virtual bool readSession(SessionOperationStatus &context, SessionModel *model);
    virtual bool readSessionData(SessionOperationStatus &context, SessionModel *sessionModel);
    virtual QVector<SessionListModel*> readSessionList(SessionOperationStatus &context);

    virtual bool deleteSession(SessionOperationStatus &context, SessionModel *model);
    virtual bool updateSession(SessionOperationStatus &context, SessionModel *model) ;
    virtual bool touchSession(SessionOperationStatus &context, SessionModel *model);
    virtual bool deleteAllSessionData(SessionOperationStatus &context) ;
    virtual bool deleteSessionDataWithDateFilter(SessionOperationStatus &context, const EDateInterval dateRange, const QDateTime &reference, const bool isFilesToo) ;

    bool execQuery(SessionOperationStatus &context, const QString &queryLiteral);
    bool execQueryNoTrans(SessionOperationStatus &context, const QString &queryLiteral);
    int countFiles(SessionOperationStatus &context);
    QString connectionName()
    {
        return _connectionName;
    }

    SessionDataInterface::ErrorCode getErrorCode();
    void setLogger(FrwLogger *newLogger);
    FrwLogger *logger();
    bool openTrans();
    bool commitTrans();
    bool rollbackTrans();


    QList<AttrFilterProfile*> attributeNamesFilterReadProfiles(DataResult &result);
    AttrFilterDetail *attributeNamesFilterReadDetail(DataResult &result, const int idProfile);
    void attributeNamesFilterSaveProfile(DataResult &result, AttrFilterProfile *profile, AttrFilterDetail *detail);
    void attributeNamesFilterDeleteProfile(DataResult &result, const int idProfile);

    virtual OperationStatus* insertGenericData(GenericPersistentData *model) ;
    virtual OperationStatus* updateGenericData(GenericPersistentData *model) ;
    virtual OperationStatus* deleteGenericData(GenericPersistentData *sessionModel) ;
    virtual OperationStatus* readAllGenericData(const QString &type, QList<GenericPersistentData*> &resultList) ;
    virtual OperationStatus* readGenericData(const QString &type, const int id, QList<GenericPersistentData*> &resultList) ;
    virtual GenericPersistentData *newPersistentDatum(const QString &type);

signals:

public slots:

private slots:

private:
    class SqlOperBase
    {
    public:
        SQLLiteDataAccess::Private *obj ;
        QString idOperaz;
        SqlOperBase(SQLLiteDataAccess::Private *value, const QString &newIdOperaz)
        {
            obj = value ;
            idOperaz = newIdOperaz ;
        }
        virtual bool doOper(SessionOperationStatus &context, SessionModel *model) = 0;
    };
    class SqlOperUpdateSession : public SqlOperBase
    {
    public:
        SqlOperUpdateSession(SQLLiteDataAccess::Private *value, const QString &newIdOperaz) : SqlOperBase(value, newIdOperaz) {}
        virtual bool doOper(SessionOperationStatus &context, SessionModel *model);
    };
    class SqlOperTouchSession : public SqlOperBase
    {
    public:
        SqlOperTouchSession(SQLLiteDataAccess::Private *value, const QString &newIdOperaz) : SqlOperBase(value, newIdOperaz) {}
        virtual bool doOper(SessionOperationStatus &context, SessionModel *model);
    };
    class SqlOperDeleteSession : public SqlOperBase
    {
    public:
        SqlOperDeleteSession(SQLLiteDataAccess::Private *value, const QString &newIdOperaz) : SqlOperBase(value, newIdOperaz) {}
        virtual bool doOper(SessionOperationStatus &context, SessionModel *model);
    };
    class SqlOperNewSession : public SqlOperBase
    {
    public:
        SqlOperNewSession(SQLLiteDataAccess::Private *value, const QString &newIdOperaz) : SqlOperBase(value, newIdOperaz) {}
        virtual bool doOper(SessionOperationStatus &context, SessionModel *model);
    };
    class SqlOperDeleteAllSessionData : public SqlOperBase
    {
    public:
        SqlOperDeleteAllSessionData(SQLLiteDataAccess::Private *value, const QString &newIdOperaz) : SqlOperBase(value, newIdOperaz) {}
        virtual bool doOper(SessionOperationStatus &context, SessionModel *model);
    };
    class SqlOperDeleteAllSessionDataWithDateFilter : public SqlOperBase
    {
        SessionDataInterface::EDateInterval dateRange;
        QDateTime reference;
        bool isFileToo ;
    public:
        SqlOperDeleteAllSessionDataWithDateFilter(SQLLiteDataAccess::Private *value, const QString &newIdOperaz, SessionDataInterface::EDateInterval pDateRange, QDateTime pRreference, const bool pIsFileToo) : SqlOperBase(value, newIdOperaz)
        {
            dateRange = pDateRange;
            reference = pRreference;
            isFileToo = pIsFileToo ;
        }
        virtual bool doOper(SessionOperationStatus &context, SessionModel *model);
    };

    class SqlOperLiteralQuery : public SqlOperBase
    {
        QString literal;
    public:
        SqlOperLiteralQuery(SQLLiteDataAccess::Private *value, const QString &newIdOperaz, const QString &pliteral) : SqlOperBase(value, newIdOperaz)
        {
            literal = pliteral;
        }
        virtual bool doOper(SessionOperationStatus &context, SessionModel *model);
    };
    class SqlOperCountQuery : public SqlOperBase
    {
        QString table;
    public:
        int result;

        SqlOperCountQuery(SQLLiteDataAccess::Private *value, const QString &newIdOperaz, const QString &ptable) : SqlOperBase(value, newIdOperaz)
        {
            result = 0 ;
            table = ptable;
        }
        virtual bool doOper(SessionOperationStatus &context, SessionModel *model);
    };

    //------------------------------------------------------------
    /* TODO
        // base of the generic operations
        class SqlOperBaseCommon
        {
        public:
            virtual QString name() = 0 ;
        };

        class SqlOperBaseGeneric : public SqlOperBaseCommon
        {
        public:
            virtual void doOper(DataResult &context) = 0;
        };

        // interface for enumerations
        class SqlEnumOper : public SqlOperBaseCommon
        {
        public:
            virtual void onData(DataResult &context, QSqlQuery &query ) = 0;
        };
    */
    class AttributeNamesFilterReadProfiles : public SqlOperBaseGeneric, public SqlEnumOper
    {
    public:
        QList<AttrFilterProfile*> list;
        void doOper(DataResult &context, SQLLiteDataAccess::Private *db) ;
        void onData(DataResult &context, QSqlQuery &query);
        QString name();
    };

    class AttributeNamesFilterReadDetail : public SqlOperBaseGeneric, public SqlEnumOper
    {
    public:
        AttrFilterDetail *detail;
        int idProfile;
        AttributeNamesFilterReadDetail(const int newIdProfile);
        void doOper(DataResult &dataResult, SQLLiteDataAccess::Private *db) ;
        void onData(DataResult &dataResult, QSqlQuery &query);
        QString name();
    };

    class AttributeNamesFilterSaveProfile : public SqlOperBaseGeneric
    {
    public:
        AttrFilterProfile *profile;
        AttrFilterDetail *detail;
        AttributeNamesFilterSaveProfile(AttrFilterProfile *profile, AttrFilterDetail *detail);
        void doOper(DataResult &context, SQLLiteDataAccess::Private *db) ;
        QString name();
    private:
        void saveProfile(DataResult &dataResult, SQLLiteDataAccess::Private *db, const bool isInsert);
        void prepareProfileWrite(QSqlQuery &query, const bool isInsert);
        void deleteProfileDetails(DataResult &dataResult, SQLLiteDataAccess::Private *db);
        void insertProfileDetails(DataResult &dataResult, SQLLiteDataAccess::Private *db);
    };

    class AttributeNamesFilterDelete : public SqlOperBaseGeneric
    {
    public:
        AttributeNamesFilterDelete(const int newIdProfile);
        void doOper(DataResult &dataResult, SQLLiteDataAccess::Private *db) ;
        QString name();
    private:
        int idProfile;
    };

    //------------------------- searchlets and generic items -----
    class GenericObjectBase
    {
    protected:
        QSet<QString> makeUniqueTags(const QSet<QString> &tags);
        void prepareGenericWrite(QSqlQuery &query, const bool isInsert, GenericPersistentData *model);
        int insertTag(DataResult &dataResult, QSqlQuery &query, const QString &tag);
        int tagId(DataResult &dataResult, QSqlQuery &query, const QString &tag);
        bool insertRelationships(DataResult &dataResult, QSqlQuery &query, const QList<int> &tags, const int idObject);
        bool deleteRelationships(DataResult &dataResult, QSqlQuery &query, const int idObject);
    };

    class GenericObjectInsert : public SqlOperBaseGeneric, protected GenericObjectBase
    {
        GenericPersistentData * _model;
    public:
        GenericObjectInsert(GenericPersistentData * model);
        void doOper(DataResult &dataResult, SQLLiteDataAccess::Private *db) ;
        QString name();
    };

    class GenericObjectUpdate : public SqlOperBaseGeneric, protected GenericObjectBase
    {
        GenericPersistentData * _model;
    public:
        GenericObjectUpdate(GenericPersistentData * model);
        void doOper(DataResult &dataResult, SQLLiteDataAccess::Private *db) ;
        QString name();
    };

    class GenericObjectRead : public SqlOperBaseGeneric, public SqlEnumOper
    {
    protected:
        void readTagsForObjects(DataResult &dataResult, SQLLiteDataAccess::Private *db);
    public:
        int id;
        QList<GenericPersistentData*> list;
        QString type;
        GenericObjectRead(const QString &type, const int newId = 0);
        void doOper(DataResult &context, SQLLiteDataAccess::Private *db) ;
        void onData(DataResult &context, QSqlQuery &query);
        QString name();
    };

    class GenericObjectDelete : public SqlOperBaseGeneric, protected GenericObjectBase
    {
        GenericPersistentData * _model;
    public:
        GenericObjectDelete(GenericPersistentData * model);
        void doOper(DataResult &dataResult, SQLLiteDataAccess::Private *db) ;
        QString name();
    };
};


#endif // SQLLITEDATAACCESSPRIVATE_H
