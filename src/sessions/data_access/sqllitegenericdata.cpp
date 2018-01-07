/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013-2018 by Luca Bellonda and individual contributors  *
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

#include "sqllitedataaccessprivate.h"
#include "model/genericpersistentdbdata.h"
#include <QSqlError>
#include <QSqlQuery>


//-------------------- GENERIC DATA ----
#define GENERIC_COLUMNS_NOCREAT  "uuid, name, description, payload, type, updatedate"
#define GENERIC_COLUMNS  GENERIC_COLUMNS_NOCREAT ", creationuser, creationdate"
#define GENERIC_COLUMNS_WITH_ID "id," GENERIC_COLUMNS
#define SELECT_GENERIC_COLUMNS_WITH_ID "select id," GENERIC_COLUMNS " from " TABLE_GENERIC_OBJECTS " where type=:type"
#define SELECT_GENERIC_COLUMNS_FOR_ID_WITH_ID "select id," GENERIC_COLUMNS " from " TABLE_GENERIC_OBJECTS " where type=:type and id=:id"

#define SQL_INSERT_GENERIC_OBJECT ""\
    "insert into " TABLE_GENERIC_OBJECTS " ( "\
    GENERIC_COLUMNS \
    ")values ("\
    ":uuid, :name, :description, :payload, :type, :updatedate, :creationuser, :creationdate "\
    ")"

#define SQL_UPDATE_GENERIC_OBJECT ""\
    "update " TABLE_GENERIC_OBJECTS " set "\
    "uuid=:uuid, name=:name, description=:description, payload=:payload, type=:type, updatedate=:updatedate where id=:id"

#define SQL_DELETE_GENERIC_OBJECT "delete from " TABLE_GENERIC_OBJECTS " where id=:id"

//-----------

SQLLiteDataAccess::Private::GenericObjectInsert::GenericObjectInsert(GenericPersistentData * model)
{
    _model = model ;
}

QString SQLLiteDataAccess::Private::GenericObjectInsert::name()
{
    return "generic object insert" ;
}

void SQLLiteDataAccess::Private::GenericObjectBase::prepareGenericWrite(QSqlQuery &query, const bool isInsert, GenericPersistentData *model)
{
    query.bindValue(":uuid", model->uuid());
    query.bindValue(":name", model->name());
    query.bindValue(":description", model->description());
    QByteArray payloadBA  = model->payload().toUtf8();
    query.bindValue(":payload", payloadBA);
    query.bindValue(":type", model->type());
    query.bindValue(":updatedate", model->updateDate());

    if(isInsert) {
        query.bindValue(":creationdate", model->creationDate());
        query.bindValue(":creationuser", model->creationUser());
    } else {
        query.bindValue(":id", model->id());
    }
}

QSet<QString> SQLLiteDataAccess::Private::GenericObjectBase::makeUniqueTags(const QSet<QString> &tags)
{
    return tags;
}

#define INSERT_TAG  "insert into " TABLE_TAGS " (tag) values(:tag)"
#define SELECT_TAG  "select id from " TABLE_TAGS " where tag = :tag"
#define DELETE_TAG_RELATION "delete from " TABLE_TAGS_RELATIONS " where objectId =:objectId"
#define INSERT_TAG_RELATION  "insert into " TABLE_TAGS_RELATIONS " (objectId, tagId) values(:objectId, :tagId)"


int SQLLiteDataAccess::Private::GenericObjectBase::insertTag(DataResult &dataResult, QSqlQuery &query, const QString &tag)
{
    query.clear();
    query.prepare(INSERT_TAG);
    query.bindValue(":tag", tag);

    int tagId = -1 ;
    bool ok = query.exec();
    if(ok) {
        tagId = query.lastInsertId().toInt();
    }
    query.finish();
    if(!ok) {
        dataResult.setError(QString("insert tag error: %1:").arg(query.lastError().databaseText()));
        return -1;
    }
    return tagId ;
}

int SQLLiteDataAccess::Private::GenericObjectBase::tagId(DataResult &dataResult, QSqlQuery &query, const QString &tag)
{
    query.clear();
    query.prepare(SELECT_TAG);
    query.bindValue(":tag", tag);

    bool isOk = false;
    int tagId = -1 ;
    if(query.exec()) {
        isOk = true;
        if(query.next()) {
            tagId = query.value(0).toInt();
        }
        query.finish();
    }
    if(!isOk) {
        dataResult.setError(QString("retrieving tag error: %1:").arg(query.lastError().databaseText()));
        return -1;
    }
    return tagId ;
}

bool SQLLiteDataAccess::Private::GenericObjectBase::insertRelationships(DataResult &dataResult, QSqlQuery &query, const QList<int> &tags, const int idObject)
{
    foreach(int tag, tags) {
        query.clear();
        query.prepare(INSERT_TAG_RELATION);
        query.bindValue(":tagId", tag);
        query.bindValue(":objectId", idObject);
        bool ok = query.exec();
        if(!ok) {
            dataResult.setError(QString("insert tag relation error: %1:").arg(query.lastError().databaseText()));
            return false;
        }
    }
    return true;
}

bool SQLLiteDataAccess::Private::GenericObjectBase::deleteRelationships(DataResult &dataResult, QSqlQuery &query, const int idObject)
{
    query.clear();
    query.prepare(DELETE_TAG_RELATION);
    query.bindValue(":objectId", idObject);
    bool ok = query.exec();
    if(!ok) {
        dataResult.setError(QString("delete tag relation error: %1:").arg(query.lastError().databaseText()));
        return false;
    }
    return true;
}


void SQLLiteDataAccess::Private::GenericObjectInsert::doOper(DataResult &dataResult, SQLLiteDataAccess::Private *db)
{
    bool isDebugLog = false;
    if(db->logger() != NULL) {
        isDebugLog = db->logger()->isLoggable(FrwLogger::DEBUG);
    }

    if(isDebugLog) {
        db->logger()->debug("generic object insert enter");
    }
    QSqlQuery query(db->db()) ;
    query.clear();

    QSet<QString> tags = makeUniqueTags(_model->tags());
    QList<int> tagsIdList ;
    foreach(QString tag, tags.values()) {
        int tagIdNumber = -1 ;
        tagIdNumber = tagId(dataResult, query, tag);
        if(dataResult.isOk()) {
            if(tagIdNumber <= 0) {
                tagIdNumber = insertTag(dataResult, query, tag);
            }
        }
        //-----
        if(dataResult.isOk()) {
            tagsIdList.append(tagIdNumber);
        } else {
            break;
        }
    }
    if(dataResult.isOk()) {
        if(isDebugLog) {
            db->logger()->debug(QString("Found %1 tags").arg(tagsIdList.size()));
        }
    }

    if(dataResult.isOk()) {
        query.prepare(SQL_INSERT_GENERIC_OBJECT);
        prepareGenericWrite(query, true, _model);
        if(!query.exec()) {
            dataResult.setError(QString("insert error: %1:").arg(query.lastError().databaseText()));
        }
        int modelId = query.lastInsertId().toInt();
        _model->setId(modelId);
    }
    if(dataResult.isOk()) {
        insertRelationships(dataResult, query, tagsIdList, _model->id());
    }

    if(db->logger()) {
        db->logger()->debug(QString("Insert object exit %1").arg(dataResult.isOk() ? "ok" : "ko"), NULL);
    }
}

//------------------------------------------------------------------------------------------
SQLLiteDataAccess::Private::GenericObjectUpdate::GenericObjectUpdate(GenericPersistentData * model)
{
    _model = model ;
}

QString SQLLiteDataAccess::Private::GenericObjectUpdate::name()
{
    return "generic object update" ;
}

void SQLLiteDataAccess::Private::GenericObjectUpdate::doOper(DataResult &dataResult, SQLLiteDataAccess::Private *db)
{
    bool isDebugLog = false;
    if(db->logger() != NULL) {
        isDebugLog = db->logger()->isLoggable(FrwLogger::DEBUG);
    }

    if(isDebugLog) {
        db->logger()->debug("generic object update enter");
    }
    QSqlQuery query(db->db()) ;
    query.clear();

    QSet<QString> tags = makeUniqueTags(_model->tags());
    QList<int> tagsIdList ;
    foreach(QString tag, tags.values()) {
        int tagIdNumber = -1 ;
        tagIdNumber = tagId(dataResult, query, tag);
        if(dataResult.isOk()) {
            if(tagIdNumber <= 0) {
                tagIdNumber = insertTag(dataResult, query, tag);
            }
        }
        //-----
        if(dataResult.isOk()) {
            tagsIdList.append(tagIdNumber);
        } else {
            break;
        }
    }
    if(dataResult.isOk()) {
        if(isDebugLog) {
            db->logger()->debug(QString("Found %1 tags").arg(tagsIdList.size()));
        }
    }

    if(dataResult.isOk()) {
        query.prepare(SQL_UPDATE_GENERIC_OBJECT);
        prepareGenericWrite(query, false, _model);
        if(!query.exec()) {
            dataResult.setError(QString("update error: %1:").arg(query.lastError().databaseText()));
        }
    }
    if(dataResult.isOk()) {
        deleteRelationships(dataResult, query, _model->id());
    }

    if(dataResult.isOk()) {
        insertRelationships(dataResult, query, tagsIdList, _model->id());
    }

    if(db->logger()) {
        db->logger()->debug(QString("Update object exit %1").arg(dataResult.isOk() ? "ok" : "ko"), NULL);
    }
}


//------------------------------------------------------------------------------------------------------

SQLLiteDataAccess::Private::GenericObjectRead::GenericObjectRead(const QString &newType, const int newId)
{
    id = newId;
    type = newType;
}

void SQLLiteDataAccess::Private::GenericObjectRead::onData(DataResult &dataResult, QSqlQuery &query)
{
    GenericPersistentDBData *model = new GenericPersistentDBData();
    if(NULL == model) {
        dataResult.setOk(false);
        dataResult.setMessage(tr("Out of memory."));
        return ;
    }

    model->setId(query.value(0).toInt());
    model->setUuid(query.value(1).toString());
    model->setName(query.value(2).toString());
    model->setDescription(query.value(3).toString());
    QByteArray payloadBA = query.value(4).toByteArray();
    model->setPayload(QString(payloadBA));
    model->setType(query.value(5).toString());
    model->setUpdateDate(query.value(6).toDateTime());
    model->setCreationUser(query.value(7).toString());
    model->setCreationDate(query.value(8).toDateTime());
    list.append(model);
}

#define QRY_READ_TAGS_TYPE "select tr.objectId, t.tag from " TABLE_TAGS_RELATIONS " tr join " TABLE_GENERIC_OBJECTS " o on o.type=:type and o.id=tr.objectId join " TABLE_TAGS " t on t.id=tr.tagId "
#define QRY_READ_TAGS_TYPE_AND_ID "select tr.objectId, t.tag from " TABLE_TAGS_RELATIONS " tr join " TABLE_GENERIC_OBJECTS " o on o.type=:type and o.id=tr.objectId and o.id=:id join " TABLE_TAGS " t on t.id=tr.tagId "

void SQLLiteDataAccess::Private::GenericObjectRead::readTagsForObjects(DataResult &dataResult, SQLLiteDataAccess::Private *db)
{
    QSqlQuery query(db->db()) ;
    QMap<int, GenericPersistentData*> modelMap;
    foreach(GenericPersistentData * d, list) {
        modelMap.insert(d->id(), d);
    }
    query.clear();
    if(id != 0) {
        query.prepare(QRY_READ_TAGS_TYPE_AND_ID);
        query.bindValue(":id", id);
    } else {
        query.prepare(QRY_READ_TAGS_TYPE);
    }
    query.bindValue(":type", type);

    if(query.exec()) {
        while(query.next()) {
            int objectId = query.value(0).toInt();
            QString tag = query.value(1).toString();
            GenericPersistentData *td = modelMap[objectId];
            if((NULL == td) || query.lastError().isValid()) {
                dataResult.setError(QString("tag without model error: %1:").arg(query.lastError().databaseText()));
                break;
            }
            td->tags().insert(tag);
        }
        query.finish();
    } else {
        dataResult.setError(QString("retrieving tag error: %1:").arg(query.lastError().databaseText()));
    }
}

void SQLLiteDataAccess::Private::GenericObjectRead::doOper(DataResult &dataResult, SQLLiteDataAccess::Private *db)
{
    QSqlQuery query(db->db()) ;
    if(id != 0) {
        query.prepare(SELECT_GENERIC_COLUMNS_FOR_ID_WITH_ID);
        query.bindValue(":id", id);
    } else {
        query.prepare(SELECT_GENERIC_COLUMNS_WITH_ID);
    }
    query.bindValue(":type", type);
    db->execEnum(dataResult, this, query);
    if(!dataResult.isOk()) {
        foreach(GenericPersistentData * d, list) {
            delete d;
        }
        list.clear();
    } else {
        readTagsForObjects(dataResult, db);
    }
}

QString SQLLiteDataAccess::Private::GenericObjectRead::name()
{
    return "read generic objects" ;
}

//------------------------------------------------------------
SQLLiteDataAccess::Private::GenericObjectDelete::GenericObjectDelete(GenericPersistentData * model)
{
    _model = model ;
}

QString SQLLiteDataAccess::Private::GenericObjectDelete::name()
{
    return "generic object delete" ;
}

void SQLLiteDataAccess::Private::GenericObjectDelete::doOper(DataResult &dataResult, SQLLiteDataAccess::Private *db)
{
    bool isDebugLog = false;
    if(db->logger() != NULL) {
        isDebugLog = db->logger()->isLoggable(FrwLogger::DEBUG);
    }

    if(isDebugLog) {
        db->logger()->debug("generic object delete enter");
    }
    QSqlQuery query(db->db()) ;
    query.clear();

    deleteRelationships(dataResult, query, _model->id());

    if(dataResult.isOk()) {
        query.prepare(SQL_DELETE_GENERIC_OBJECT);
        query.bindValue(":id", _model->id());
        if(!query.exec()) {
            dataResult.setError(QString("delete error: %1:").arg(query.lastError().databaseText()));
        }
    }

    if(db->logger()) {
        db->logger()->debug(QString("Delete object exit %1").arg(dataResult.isOk() ? "ok" : "ko"), NULL);
    }
}



