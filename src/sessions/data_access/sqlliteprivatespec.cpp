/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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

// private generic operations

#include "sqllitedataaccessprivate.h"
#include <QSqlError>
#include <QSqlQuery>

void SQLLiteDataAccess::Private::AttributeNamesFilterReadProfiles::onData(DataResult &dataResult, QSqlQuery &query)
{
    AttrFilterProfile *model = new AttrFilterProfile();
    if(NULL == model) {
        dataResult.setOk(false);
        dataResult.setMessage(tr("Out of memory."));
        return ;
    }
    model->setId(query.value(0).toInt());
    model->setName(query.value(1).toString());
    model->setDescription(query.value(2).toString());
    model->setWhiteList(query.value(3).toBool());
    model->setCreationTime(query.value(4).toDateTime());
    model->setUpdateTime(query.value(5).toDateTime());
    list.append(model);
}

#define SQL_READ_ALL_ATTRIBUTEFILTER_PROFILES "select id, name, description, iswhitelist, creationdate, updatedate from " TABLE_ATTRNAMEFILTER_PROFILES " order by name;"

void SQLLiteDataAccess::Private::AttributeNamesFilterReadProfiles::doOper(DataResult &dataResult, SQLLiteDataAccess::Private *db)
{
    QSqlQuery query(db->db()) ;
    query.prepare(SQL_READ_ALL_ATTRIBUTEFILTER_PROFILES);
    db->execEnum(dataResult, this, query);
}

QString SQLLiteDataAccess::Private::AttributeNamesFilterReadProfiles::name()
{
    return "read profiles" ;
}

//---------------------------------------------------------------------------------------

SQLLiteDataAccess::Private::AttributeNamesFilterReadDetail::AttributeNamesFilterReadDetail(const int newIdProfile)
{
    detail = new AttrFilterDetail();
    idProfile = newIdProfile;
}

QString SQLLiteDataAccess::Private::AttributeNamesFilterReadDetail::name()
{
    return "read detail" ;
}

#define SQL_READ_ATTRIBUTE_FILTER_READ_DETAIL " select name from " TABLE_ATTRNAMEFILTER_NAMES  " where profileId=:profileId order by name;"

void SQLLiteDataAccess::Private::AttributeNamesFilterReadDetail::doOper(DataResult &dataResult, SQLLiteDataAccess::Private *db)
{
    QSqlQuery query(db->db()) ;
    query.prepare(SQL_READ_ATTRIBUTE_FILTER_READ_DETAIL);
    query.bindValue(":profileId", idProfile);
    db->execEnum(dataResult, this, query);
    if(!dataResult.isOk()) {
        if(NULL != detail) {
            delete detail;
            detail = NULL;
        }
    }
}


void SQLLiteDataAccess::Private::AttributeNamesFilterReadDetail::onData(DataResult &dataResult, QSqlQuery &query)
{
    if(NULL == detail) {
        dataResult.setOk(false);
        dataResult.setMessage(tr("Out of memory."));
        return ;
    }
    detail->addName(query.value(0).toString());
}

//--------------------------------------------------------------------------

SQLLiteDataAccess::Private::AttributeNamesFilterSaveProfile::AttributeNamesFilterSaveProfile(AttrFilterProfile *newProfile, AttrFilterDetail *newDetail)
{
    profile = newProfile;
    detail = newDetail;
}

QString SQLLiteDataAccess::Private::AttributeNamesFilterSaveProfile::name()
{
    return "attribute filter profile save" ;
}

void SQLLiteDataAccess::Private::AttributeNamesFilterSaveProfile::doOper(DataResult &dataResult, SQLLiteDataAccess::Private *db)
{
    if(NULL == profile) {
        dataResult.setError(tr("No profile"));
        return ;
    }
    if(NULL == detail) {
        dataResult.setError(tr("No detail"));
        return ;
    }
    bool isInsert = false;
    if(profile->id() == 0) {
        isInsert = true ;
        profile->setCreationTime(QDateTime::currentDateTime());
    }
    profile->setUpdateTime(QDateTime::currentDateTime());
    saveProfile(dataResult, db, isInsert);
    deleteProfileDetails(dataResult, db);
    insertProfileDetails(dataResult, db);
}

#define SQL_INSERT_ATTRFILTER_PROFILE " insert into " TABLE_ATTRNAMEFILTER_PROFILES " (id, name, description, iswhitelist, creationdate, updatedate ) values (:id, :name, :description, :isWhiteList, :creationTime, :updateTime) ;"

#define SQL_UPDATE_ATTRFILTER_PROFILE " update " TABLE_ATTRNAMEFILTER_PROFILES " set name=:name, description=:description, iswhitelist=:isWhiteList, updatedate=:updateTime  where id=:id;"

void SQLLiteDataAccess::Private::AttributeNamesFilterSaveProfile::prepareProfileWrite(QSqlQuery &query, const bool isInsert)
{
    query.bindValue(":name", profile->name());
    query.bindValue(":description", profile->description());
    query.bindValue(":isWhiteList", profile->isWhiteList());
    query.bindValue(":updateTime", profile->updateTime());
    if(isInsert) {
        query.bindValue(":creationTime", profile->creationTime());
    } else {
        query.bindValue(":id", profile->id());
    }
}

void SQLLiteDataAccess::Private::AttributeNamesFilterSaveProfile::saveProfile(DataResult &dataResult, SQLLiteDataAccess::Private *db, const bool isInsert)
{
    if(db->logger()) {
        db->logger()->debug("Insert Profile enter", NULL);
    }
    QSqlQuery query(db->db()) ;
    if(isInsert) {
        query.prepare(SQL_INSERT_ATTRFILTER_PROFILE);
    } else {
        query.prepare(SQL_UPDATE_ATTRFILTER_PROFILE);
    }
    prepareProfileWrite(query, isInsert);

    if(!query.exec()) {
        dataResult.setError(query.lastError().databaseText());
    } else {
        if(isInsert) {
            profile->setId(query.lastInsertId().toInt());
        }
    }
    if(db->logger()) {
        db->logger()->debug(QString("Insert Profile exit %1").arg(dataResult.isOk() ? "ok" : "ko"));
    }
}

#define SQL_DELETE_ATTRFILTER_PDETAILS " delete from " TABLE_ATTRNAMEFILTER_NAMES  " where profileId = :profileId;"

void SQLLiteDataAccess::Private::AttributeNamesFilterSaveProfile::deleteProfileDetails(DataResult &dataResult, SQLLiteDataAccess::Private *db)
{
    if(db->logger()) {
        db->logger()->debug("Delete Profile details enter", NULL);
    }
    QSqlQuery query(db->db()) ;
    query.prepare(SQL_DELETE_ATTRFILTER_PDETAILS);
    query.bindValue(":profileId", profile->id());

    if(!query.exec()) {
        dataResult.setError(db->db().lastError().text());
    }
    if(db->logger()) {
        db->logger()->debug(QString("Delete Profile details exit %1").arg(dataResult.isOk() ? "ok" : "ko"), NULL);
    }
}

#define SQL_INSERT_ATTRFILTER_PDETAILS " insert into " TABLE_ATTRNAMEFILTER_NAMES  " (profileId, name) values ( :profileId, :name);"

void SQLLiteDataAccess::Private::AttributeNamesFilterSaveProfile::insertProfileDetails(DataResult &dataResult, SQLLiteDataAccess::Private *db)
{
    if(db->logger()) {
        db->logger()->debug("Insert Profile details enter", NULL);
    }
    QSqlQuery query(db->db()) ;
    query.prepare(SQL_INSERT_ATTRFILTER_PDETAILS);
    foreach(QString aName, detail->names()) {
        query.bindValue(":profileId", profile->id());
        query.bindValue(":name", aName);

        if(!query.exec()) {
            dataResult.setError(QString("name insert error: %1:").arg(query.lastError().databaseText()));
            break;
        }
    }
    if(db->logger()) {
        db->logger()->debug(QString("Insert Profile details exit %1").arg(dataResult.isOk() ? "ok" : "ko"), NULL);
    }
}

//--------------------------------------------------------------------------

#define SQL_DELETE_ATTRFILTER_HEADER_PROFILE " delete from " TABLE_ATTRNAMEFILTER_PROFILES  " where id = :profileId;"

SQLLiteDataAccess::Private::AttributeNamesFilterDelete::AttributeNamesFilterDelete(const int newIdProfile)
{
    idProfile = newIdProfile;
}

QString SQLLiteDataAccess::Private::AttributeNamesFilterDelete::name()
{
    return "attribute filter delete" ;
}

void SQLLiteDataAccess::Private::AttributeNamesFilterDelete::doOper(DataResult &dataResult, SQLLiteDataAccess::Private *db)
{
    if(db->logger()) {
        db->logger()->debug("Delete Profile enter");
    }
    QSqlQuery query(db->db()) ;
    query.prepare(SQL_DELETE_ATTRFILTER_PDETAILS);
    query.bindValue(":profileId", idProfile);

    if(!query.exec()) {
        dataResult.setError(query.lastError().databaseText());
    } else {
        QSqlQuery queryH(db->db()) ;
        queryH.prepare(SQL_DELETE_ATTRFILTER_HEADER_PROFILE);
        queryH.bindValue(":profileId", idProfile);
        if(!queryH.exec()) {
            dataResult.setError(queryH.lastError().databaseText());
        }
    }
    if(db->logger()) {
        db->logger()->debug(QString("Delete Profile exit %1").arg(dataResult.isOk() ? "ok" : "ko"));
    }
}

//--------------------

