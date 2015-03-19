/***************************************************************************
 *   This file is part of qaccessorsgen                                    *
 *   Copyright (C) 2011 by Luca Bellonda                                   *
 *   lbellonda _at_ gmail.com                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *    Boston, MA  02110-1301, USA.                                         *
 ***************************************************************************/


#include "testdataaccess.h"

TestDataAccess::TestDataAccess(QObject *parent) :
    QObject(parent)
{
    _alwaysFail = false;
}

TestDataAccess::~TestDataAccess()
{
}

bool TestDataAccess::init(const QString & /*configuration*/)
{
    if(_alwaysFail) {
        return false;
    }
    // empty method
    return true;
}

SessionSummary *TestDataAccess::getSummary(SessionOperationStatus &context)
{
    if(_alwaysFail) {
        return NULL;
    }
    context.ok = false ;
    SessionSummary *summary = new SessionSummary();
    if(NULL != summary) {
        SessionFileAccess * fileAccess = new SessionFileAccess();
        if(NULL != fileAccess) {
            fileAccess->setPath("../test/data/test1.xml");
            fileAccess->setLastAccess(QDateTime());
            fileAccess->setSize(100);
            fileAccess->setTimes(5);
            summary->addRecentFile(fileAccess);
            fileAccess = new SessionFileAccess();
            if(NULL != fileAccess) {
                fileAccess->setPath("/home/lbell/Desktop");
                fileAccess->setLastAccess(QDateTime());
                fileAccess->setSize(200);
                fileAccess->setTimes(4);
                summary->addRecentFile(fileAccess);
                context.ok = true ;
            }
        }
    }
    return summary;
}

void TestDataAccess::closeAndDispose()
{
    delete this;
}

bool TestDataAccess::enrollFile(SessionOperationStatus &context, SessionModel */*model*/, const QString &/*filePath*/)
{
    context.ok = true ;
    if(_alwaysFail) {
        context.ok = false ;
    }
    return context.ok ;
}

bool TestDataAccess::newSession(SessionOperationStatus &context, SessionModel *model)
{
    context.ok = false;
    if(_alwaysFail) {
        return false;
    }
    model->id = 1;
    context.ok = true;
    return context.ok ;
}

bool TestDataAccess::readSession(SessionOperationStatus &context, SessionModel *model)
{
    Q_ASSERT(NULL != model);
    context.ok = false;
    if(_alwaysFail) {
        return false ;
    }
    model->id = 1;
    model->name = "testSessionName";
    model->description = "testSessionDescription";
    model->enabled = true ;
    context.ok = true;
    return true;
}

bool TestDataAccess::readSessionData(SessionOperationStatus &context, SessionModel */*sessionModel*/)
{
    context.ok = false;
    return false;
}

QVector<SessionListModel*> TestDataAccess::readSessionList(SessionOperationStatus &context)
{
    context.ok = false;
    return QVector<SessionListModel*>() ;
}

bool TestDataAccess::deleteSession(SessionOperationStatus &context, SessionModel * /*model*/)
{
    context.ok = true ;
    if(_alwaysFail) {
        context.ok = false ;
    }
    return context.ok ;
}

bool TestDataAccess::touchSession(SessionOperationStatus &context, SessionModel * /*model*/)
{
    context.ok = true ;
    if(_alwaysFail) {
        context.ok = false ;
    }
    return context.ok ;
}

bool TestDataAccess::updateSession(SessionOperationStatus &context, SessionModel * /*model*/)
{
    context.ok = true ;
    if(_alwaysFail) {
        context.ok = false ;
    }
    return context.ok ;
}

bool TestDataAccess::deleteAllSessionData(SessionOperationStatus &context)
{
    context.ok = true ;
    if(_alwaysFail) {
        context.ok = false ;
    }
    return context.ok ;
}

bool TestDataAccess::deleteSessionDataWithDateFilter(SessionOperationStatus &context, const EDateInterval /*dateRange*/, const QDateTime & /*reference*/, const bool /*isFilesToo*/)
{
    context.ok = true ;
    if(_alwaysFail) {
        context.ok = false ;
    }
    return context.ok ;
}

QList<AttrFilterProfile*> TestDataAccess::attributeNamesFilterReadProfiles(DataResult &result)
{
    result.setOk(false);
    return QList<AttrFilterProfile*>();
}

AttrFilterDetail *TestDataAccess::attributeNamesFilterReadDetail(DataResult &result, const int /*idProfile*/)
{
    result.setOk(false);
    return NULL;
}

void TestDataAccess::attributeNamesFilterSaveProfile(DataResult &result, AttrFilterProfile * /*profile*/, AttrFilterDetail * /*detail*/)
{
    result.setOk(false);
}

void TestDataAccess::attributeNamesFilterDeleteProfile(DataResult &result, const int /*idProfile*/)
{
    result.setOk(false);
}

OperationStatus* TestDataAccess::insertGenericData(GenericPersistentData * /*model*/)
{
    return NULL;
}
OperationStatus* TestDataAccess::updateGenericData(GenericPersistentData * /*model*/)
{
    return NULL;
}
OperationStatus* TestDataAccess::deleteGenericData(GenericPersistentData * /*sessionModel*/)
{
    return NULL;
}
OperationStatus* TestDataAccess::readAllGenericData(const QString & /*type*/ , QList<GenericPersistentData*> & /*resultList*/)
{
    return NULL;
}
GenericPersistentData *TestDataAccess::newPersistentDatum(const QString & /*type*/)
{
    return NULL ;
}
