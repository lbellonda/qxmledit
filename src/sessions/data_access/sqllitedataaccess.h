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


#ifndef SQLLITEDATAACCESS_H
#define SQLLITEDATAACCESS_H

#include <QObject>

#include <sessiondata>
#include "log.h"
#include "../QXmlEditSessions_global.h"
#include <data/DataInterface.h>


#include "model/attrfilterdetail.h"
#include "model/attrfilterprofile.h"


class QXMLEDITSESSIONSSHARED_EXPORT SQLLiteDataAccess : public QObject, public SessionDataInterface, public DataInterface
{
    Q_OBJECT
    class Private;
    Private * const d;

public:
    explicit SQLLiteDataAccess(QObject *parent = 0);
    ~SQLLiteDataAccess();
    Private * privatePart();

    virtual bool init(const QString &configuration)  ;
    virtual void closeAndDispose() ;

    void setLogger(FrwLogger *logger);

    virtual bool enrollFile(SessionOperationStatus &context, SessionModel *model, const QString &filePath);
    virtual bool newSession(SessionOperationStatus &context, SessionModel *model) ;
    virtual bool readSession(SessionOperationStatus &context, SessionModel *model);
    virtual bool readSessionData(SessionOperationStatus &context, SessionModel *sessionModel);
    virtual bool deleteSession(SessionOperationStatus &context, SessionModel *model);
    virtual QVector<SessionListModel*> readSessionList(SessionOperationStatus &context);
    virtual bool touchSession(SessionOperationStatus &context, SessionModel *model) ;
    virtual bool updateSession(SessionOperationStatus &context, SessionModel *model);
    virtual bool deleteAllSessionData(SessionOperationStatus &context) ;
    virtual bool deleteSessionDataWithDateFilter(SessionOperationStatus &context, const EDateInterval dateRange, const QDateTime &reference, const bool isFilesToo) ;

    //-----
    bool execQuery(SessionOperationStatus &context, const QString &queryLiteral);
    int countFiles(SessionOperationStatus &context);

    //-----
    QList<AttrFilterProfile*> attributeNamesFilterReadProfiles(DataResult &result);
    AttrFilterDetail *attributeNamesFilterReadDetail(DataResult &result, const int idProfile);
    void attributeNamesFilterSaveProfile(DataResult &result, AttrFilterProfile *profile, AttrFilterDetail *detail);
    void attributeNamesFilterDeleteProfile(DataResult &result, const int idProfile);
    // --
    virtual OperationStatus* insertGenericData(GenericPersistentData *model) ;
    virtual OperationStatus* updateGenericData(GenericPersistentData *model) ;
    virtual OperationStatus* deleteGenericData(GenericPersistentData *sessionModel) ;
    virtual OperationStatus* readAllGenericData(const QString &type, QList<GenericPersistentData*> &resultList) ;
    virtual OperationStatus* readGenericData(const QString &type, const int id, QList<GenericPersistentData*> &resultList);
    virtual GenericPersistentData *newPersistentDatum(const QString &type);
signals:

public slots:

};

#endif // SQLLITEDATAACCESS_H
