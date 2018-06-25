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


#ifndef SESSIONDATAINTERFACE_H
#define SESSIONDATAINTERFACE_H

#include "sessionoperationstatus.h"
#include "sessionsummary.h"
#include "data_access/model/filemodel.h"
#include "data_access/model/sessionlistmodel.h"
#include "data_access/dataresult.h"


class AttrFilterProfile;
class AttrFilterDetail;

class SessionDataInterface
{
public:
    virtual ~SessionDataInterface();
    virtual bool init(const QString &configuration) = 0 ;
    virtual void closeAndDispose() = 0 ;

    virtual bool enrollFile(SessionOperationStatus &context, SessionModel *model, const QString &filePath) = 0;

    virtual bool newSession(SessionOperationStatus &context, SessionModel *model) = 0;
    virtual bool readSession(SessionOperationStatus &context, SessionModel *model) = 0;
    virtual bool readSessionData(SessionOperationStatus &context, SessionModel *sessionModel) = 0;

    virtual bool deleteSession(SessionOperationStatus &context, SessionModel *model) = 0;
    virtual bool updateSession(SessionOperationStatus &context, SessionModel *model) = 0;

    virtual QVector<SessionListModel*> readSessionList(SessionOperationStatus &context) = 0;

    virtual bool touchSession(SessionOperationStatus &context, SessionModel *model) = 0;

    virtual bool deleteAllSessionData(SessionOperationStatus &context) = 0;

    enum EDateInterval {
        DATE_INTERVAL_1_MONTH,
        DATE_INTERVAL_6_MONTHS,
        DATE_INTERVAL_LITERAL
    };

    virtual bool deleteSessionDataWithDateFilter(SessionOperationStatus &context, const EDateInterval dateRange, const QDateTime &reference, const bool isFilesToo) = 0;

    enum ErrorCode {
        E_NOERROR,
        E_CANTOPEN,
        E_UNKNOWN = 1000
    };

    //-----
    virtual QList<AttrFilterProfile*> attributeNamesFilterReadProfiles(DataResult &result) = 0 ;
    virtual AttrFilterDetail *attributeNamesFilterReadDetail(DataResult &result, const int idProfile) = 0 ;
    virtual void attributeNamesFilterSaveProfile(DataResult &result, AttrFilterProfile *profile, AttrFilterDetail *detail) = 0 ;
    virtual void attributeNamesFilterDeleteProfile(DataResult &result, const int idProfile) = 0 ;
};

class QXMLEDITSESSIONSSHARED_EXPORT SessionDataFactory
{
public:
    virtual SessionDataInterface *sessionDataInterface(const QString &configuration) = 0;
    virtual ~SessionDataFactory();
};

#endif // SESSIONDATAINTERFACE_H
