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


/*
 *Tests to do:
 *1-create object and test db access
 *2-set type
 *3-verify equals of object
 *4-verify different of object
 *5-create tables and indexes
 *6-insert manually some record and check count with contstraints unique
 *7-insert object
 *8-check count from db
 *9-read data
 *10-compare inserted values with read data
 *11-update data varying some field
 *12-insert and update data checking that only updated data are varied
 *13-delete data
 *14-test duplicate tags and case handling
 *15-delete, insert, update all toghether
 */

#include "testsearchlets.h"
#include "app.h"
#include "sqlliteaccess.h"
#include "modules/search/searchwidget.h"

#define TYPE_TEST   QXMLE_PDATA_TypeSearchlet "XX"

TestSearchlets::TestSearchlets()
{
}


TestSearchlets::~TestSearchlets()
{
}

//----------------

class TestSearchlets::Data {
    SQLLiteTestAccess _access;
public:
    QString status;
    App app;
    bool init();
    Data():_access(false)
    {
    }
    SQLLiteDataAccess *access()
    {
        return static_cast<SQLLiteDataAccess *>(app.data()->storageManager());
    }
    DataInterface *storageManager()
    {
        return app.data()->storageManager();
    }
};

bool TestSearchlets::Data::init()
{
    status = "init access";
    if(!_access.init()) {
        return false;
    }
    status = "init app";
    app.sessionDBPath = _access.dbFileName;
    return app.init1();
}

bool TestSearchlets::testUnitBase()
{
    _testName = "testUnitBase";
    TestSearchlets::Data data;
    if(!data.init()) {
        return error("init");
    }
    if( NULL == data.app.data()->storageManager()) {
        return error("no storage manager");
    }
    GenericPersistentData *object = data.app.data()->storageManager()->newPersistentDatum(QXMLE_PDATA_TypeSearchlet);
    if( NULL == object ) {
        return error("No generic object");
    }
    if( object->type() != QXMLE_PDATA_TypeSearchlet) {
        error(QString("Wrong object type, expecting '%1', found:'%2'").arg(QXMLE_PDATA_TypeSearchlet).arg(object->type()));
    }
    delete object;
    if( isError()){
        return false;
    }
    return true;
}

void TestSearchlets::fillObject(GenericPersistentData *o1, int id)
{
    o1->setId(id);
    o1->setCreationDate(_baseDate);
    o1->setDescription("description");
    o1->setType("type");
    o1->setCreationUser("setCreationUser");
    o1->setName("name");
    o1->setPayload("payload");
    o1->setReadOnly(false);
    QSet<QString> tags;
    tags << "one";
    tags << "two";
    o1->setTags(tags);
    o1->setUpdateDate(_baseDate);
    o1->setUuid("uuid");
}

/*void TestSearchlets::fillObject1(GenericPersistentData *o1, int id)
{
    o1->setId(id);
    o1->setCreationDate(_baseDate);
    o1->setDescription("tttt");
    o1->setType("type");
    o1->setCreationUser("rrrrr");
    o1->setName("gggg");
    o1->setPayload("hhhh");
    o1->setReadOnly(false);
    QStringList tags;
    tags << "wwww";
    tags << "eeee";
    o1->setTags(tags);
    o1->setUpdateDate(_baseDate);
    o1->setUuid("tttt");
}*/

#define CHKF(EXPECTING,FUNC,VAL) \
    if(!isError()) {\
        fillObject(o2,1);\
        o2->FUNC(VAL);\
        QString fieldName ;\
        if(o1->isEqual(o2, &fieldName) ) {\
            error(QString("o1 is equal to 2 but expecting a difference for field: '%1'").arg(EXPECTING));\
        } else {\
            if( fieldName != EXPECTING ) {\
                error(QString("o1 is different to 2 for field:'%1' but expecting a difference for field: '%2'").arg(fieldName).arg(EXPECTING));\
            }\
        }\
    }\

bool TestSearchlets::testUnitObject()
{
    _testName = "testUnitObject";
    TestSearchlets::Data data;
    if(!data.init()) {
        return error(QString(" data init :%1").arg(data.status));
    }
    GenericPersistentData *o1 = data.app.data()->storageManager()->newPersistentDatum(QXMLE_PDATA_TypeSearchlet);
    if( NULL == o1 ) {
        error("No generic object 1");
    }
    GenericPersistentData *o2 = data.app.data()->storageManager()->newPersistentDatum(QXMLE_PDATA_TypeSearchlet);
    if( NULL == o2 ) {
        error("No generic object 2");
    }

    if(!isError()) {
        if( o1->id() != 0 ) {
            error(QString("New objects have id not 0 :%1").arg(o1->id()));
        }
    }

    if(!isError()) {
        QString fieldName ;
        fillObject(o1, 1);
        fillObject(o2, 1);
        if( o1->isEqual(NULL, &fieldName) ) {
            error("object equals to NULL");
        }
    }
    if(!isError()) {
        QString fieldName ;
        if(!o1->isEqual(o2, &fieldName) ) {
            error(QString("o1 not equal to 2 for field: %1").arg(fieldName));
        }
    }
    CHKF("id",setId,0);
    CHKF("uuid",setUuid,"xxxx");
    CHKF("name",setName,"0000");
    CHKF("description",setDescription,"0000");
    CHKF("payload",setPayload,"0000");
    CHKF("type",setType,"0000");
    CHKF("tags",setTags,QSet<QString>());

    CHKF("updateDate",setUpdateDate,QDateTime::currentDateTime());
    CHKF("creationUser",setCreationUser,"0000");
    CHKF("creationDate",setCreationDate,QDateTime::currentDateTime());
    CHKF("tags",setTags,QSet<QString>());
    CHKF("isReadOnly",setReadOnly,true);

    if(NULL!=o1){
        delete o1;
    }
    if(NULL!=o2){
        delete o2;
    }
    if( isError() ) {
        return false;
    }
    return true;
}


bool TestSearchlets::testAccessTable()
{
    _testName = "testAccessTable";
    TestSearchlets::Data data;
    if(!data.init()) {
        return error(QString(" data init :%1").arg(data.status));
    }

    if(!data.access()->privatePart()->openTrans()) {
        return error("open transaction failed ");
    }

    // test table existence
    // test insert into tags

    QString insTag1 = "insert into TAGS ( tag ) VALUES ('a') ";
    QString insTag2 = "insert into TAGS ( tag ) VALUES ('b') ";
    if(!isError()) {
        if(!data.access()->privatePart()->execLiteralInternal(insTag1)) {
            error("error creating tag");
        }
    }
    if(!isError()) {
        if(data.access()->privatePart()->execLiteralInternal(insTag1)) {
            error("error inserting double tag");
        }
    }
    if(!isError()) {
        if(!data.access()->privatePart()->execLiteralInternal(insTag2)) {
            error("error creating second tag");
        }
    }
    if(!isError()) {
        if(data.access()->privatePart()->execLiteralInternal(insTag2)) {
            error("Expecting error creating second tag twice, but was ok");
        }
    }
    if(!isError()) {
        int values =-1 ;
        if( ! data.access()->privatePart()->countInternal(values, "TAGS") ) {
            error("Error counting tags");
        } else {
            if( values != 2 ) {
                error(QString("Mismatch counting tags, expecting 2, found:%1").arg(values));
            }
        }
    }

    if(!data.access()->privatePart()->rollbackTrans()) {
        return error("rollback transaction failed ");
    }

    if(isError()) {
        return false;
    }
    return true;
}

bool TestSearchlets::testInsert()
{
    _testName = "testInsert";
    TestSearchlets::Data data;
    if(!data.init()) {
        return error(QString(" data init :%1").arg(data.status));
    }

    GenericPersistentData *o1 = data.storageManager()->newPersistentDatum(QXMLE_PDATA_TypeSearchlet);
    if( NULL == o1 ) {
        error("No generic object 1");
    } else {
        o1->tags().insert("one");
        o1->tags().insert("two");
    }
    GenericPersistentData *o2 = data.storageManager()->newPersistentDatum(QXMLE_PDATA_TypeSearchlet);
    if( NULL == o2 ) {
        error("No generic object 2");
    } else {
        o2->tags().insert("one");
        o2->tags().insert("two");
    }
    if(!isError()) {
        OperationStatus *status = data.access()->insertGenericData(o1);
        if( (NULL == status ) || !status->isOk() ) {
            error(QString("error inserting object :%1").arg((NULL!=status?status->message():"")));
        }
        if( NULL != status ) {
            delete status;
        }
    }
    if(!isError()) {
        if( o1->id() == 0 ) {
            error(QString("Expecting id not null after insert"));
        }
    }
    if(!isError()) {
        o2->setId(o1->id());
        o2->setUuid(o1->uuid());
        QString fieldName ;
        if( !o2->isEqual(o1, &fieldName) ) {
            error(QString("after insert o1 is different to 2 for field:'%1'").arg(fieldName));
        }
    }
    if(NULL!=o1)delete o1;
    if(NULL!=o2)delete o2;
    if(isError()) {
        return false;
    }
    return true;
}


bool TestSearchlets::testInsertAndRead()
{
    _testName = "testInsertAndRead";
    TestSearchlets::Data data;
    if(!data.init()) {
        return error(QString(" data init :%1").arg(data.status));
    }

    GenericPersistentData *o1 = data.storageManager()->newPersistentDatum(QXMLE_PDATA_TypeSearchlet);
    if( NULL == o1 ) {
        error("No generic object 1");
    }

    if(!isError()) {
        OperationStatus *status = data.access()->insertGenericData(o1);
        if( (NULL == status ) || !status->isOk() ) {
            error(QString("error inserting object :%1").arg((NULL!=status?status->message():"")));
        }
        if( NULL != status ) {
            delete status;
        }
    }
    if(!isError()) {
        QList<GenericPersistentData*> resultList;
        OperationStatus *status = data.access()->readAllGenericData(QXMLE_PDATA_TypeSearchlet, resultList) ;
        if( (NULL == status ) || !status->isOk() ) {
            error(QString("error reading data :%1").arg((NULL!=status?status->message():"")));
        }

        if( NULL != status ) {
            delete status;
        }
        if(!isError()) {
            if(resultList.size()!= 1) {
                error(QString("Reading items expected 1, found :%1").arg(resultList.size()));
            }
        }
        if(!isError()) {
            GenericPersistentData* gd = resultList.at(0) ;
            QString field;
            if(!gd->isEqual(o1, &field)) {
                error(QString("Reading after insert differs for field :%1").arg(field));
            }
        }

        foreach(GenericPersistentData* gData, resultList) {
            delete gData;
        }

    }
    if(NULL!=o1)delete o1;

    if(isError()) {
        return false;
    }
    return true;
}

bool TestSearchlets::testInsertAndReadOther()
{
    _testName = "testInsertAndReadOther";
    TestSearchlets::Data data;
    if(!data.init()) {
        return error(QString(" data init :%1").arg(data.status));
    }

    GenericPersistentData *o1 = data.storageManager()->newPersistentDatum(QXMLE_PDATA_TypeSearchlet);
    if( NULL == o1 ) {
        error("No generic object 1");
    }
    GenericPersistentData *o2 = data.storageManager()->newPersistentDatum(TYPE_TEST);
    if( NULL == o2 ) {
        error("No generic object 2");
    }
    GenericPersistentData *o3 = data.storageManager()->newPersistentDatum(TYPE_TEST);
    if( NULL == o3 ) {
        error("No generic object 3");
    }
    if(!isError()) {
        OperationStatus *status = data.access()->insertGenericData(o1);
        if( (NULL == status ) || !status->isOk() ) {
            error(QString("error inserting object 1:%1").arg((NULL!=status?status->message():"")));
        }
        if( NULL != status ) {
            delete status;
        }
    }
    if(!isError()) {
        OperationStatus *status = data.access()->insertGenericData(o2);
        if( (NULL == status ) || !status->isOk() ) {
            error(QString("error inserting object 2:%1").arg((NULL!=status?status->message():"")));
        }
        if( NULL != status ) {
            delete status;
        }
    }
    if(!isError()) {
        OperationStatus *status = data.access()->insertGenericData(o3);
        if( (NULL == status ) || !status->isOk() ) {
            error(QString("error inserting object 3:%1").arg((NULL!=status?status->message():"")));
        }
        if( NULL != status ) {
            delete status;
        }
    }
    if(!isError()) {
        QList<GenericPersistentData*> resultList;
        OperationStatus *status = data.access()->readAllGenericData(QXMLE_PDATA_TypeSearchlet, resultList) ;
        if( (NULL == status ) || !status->isOk() ) {
            error(QString("error reading data 1:%1").arg((NULL!=status?status->message():"")));
        }

        if( NULL != status ) {
            delete status;
        }
        if(!isError()) {
            if(resultList.size()!= 1) {
                error(QString("Reading items 1 expected 1, found :%1").arg(resultList.size()));
            }
        }
        if(!isError()) {
            GenericPersistentData* gd = resultList.at(0) ;
            QString field;
            if(!gd->isEqual(o1, &field)) {
                error(QString("Reading after insert 1 differs for field :%1").arg(field));
            }
        }

        foreach(GenericPersistentData* gData, resultList) {
            delete gData;
        }

    }
    if(!isError()) {
        QList<GenericPersistentData*> resultList;
        OperationStatus *status = data.access()->readAllGenericData(TYPE_TEST, resultList) ;
        if( (NULL == status ) || !status->isOk() ) {
            error(QString("error reading data 2 :%1").arg((NULL!=status?status->message():"")));
        }

        if( NULL != status ) {
            delete status;
        }
        if(!isError()) {
            if(resultList.size()!= 2) {
                error(QString("Reading items 2 expected 2, found :%1").arg(resultList.size()));
            }
        }
        bool eq2 = false;
        bool eq3 = false;
        if(!isError()) {
            foreach( GenericPersistentData* gd, resultList) {
                QString field;
                if(gd->id() == o2->id()) {
                    eq2 = true;
                    if(!gd->isEqual(o2, &field)) {
                        error(QString("Reading after insert 2,1 differs for field :%1").arg(field));
                    }
                 } else {
                    if(gd->id() == o3->id()) {
                        eq3 = true;
                        if(!gd->isEqual(o3, &field)) {
                            error(QString("Reading after insert 2,2 differs for field :%1").arg(field));
                        }
                    }
                }
            }
        }
        if(!isError()) {
            if( ! (eq2 && eq3) ) {
                error(QString("Object missing 2:%1 3:%2").arg(eq2).arg(eq3));
            }
        }

        foreach(GenericPersistentData* gData, resultList) {
            delete gData;
        }

    }

    if(NULL!=o1)delete o1;
    if(NULL!=o2)delete o2;
    if(NULL!=o3)delete o3;

    if(isError()) {
        return false;
    }
    return true;
}

bool TestSearchlets::testUpdate()
{
    _testName = "testUpdate";
    TestSearchlets::Data data;
    if(!data.init()) {
        return error(QString(" data init :%1").arg(data.status));
    }

    GenericPersistentData *o1 = data.storageManager()->newPersistentDatum(QXMLE_PDATA_TypeSearchlet);
    if( NULL == o1 ) {
        error("No generic object 1");
    }else {
        o1->tags().insert("1111");
    }
    GenericPersistentData *o2 = data.storageManager()->newPersistentDatum(QXMLE_PDATA_TypeSearchlet);
    if( NULL == o2 ) {
        error("No generic object 2");
    }else {
        o2->tags().insert("2222");
    }
    GenericPersistentData *o3 = data.storageManager()->newPersistentDatum(QXMLE_PDATA_TypeSearchlet);
    if( NULL == o3 ) {
        error("No generic object 3");
    }else {
        o3->tags().insert("3333");
    }

    if(!isError()) {
        OperationStatus *status = data.access()->insertGenericData(o1);
        if( (NULL == status ) || !status->isOk() ) {
            error(QString("error inserting object 1:%1").arg((NULL!=status?status->message():"")));
        }
        if( NULL != status ) {
            delete status;
        }
    }
    if(!isError()) {
        OperationStatus *status = data.access()->insertGenericData(o2);
        if( (NULL == status ) || !status->isOk() ) {
            error(QString("error inserting object 2:%1").arg((NULL!=status?status->message():"")));
        }
        if( NULL != status ) {
            delete status;
        }
    }
    if(!isError()) {
        OperationStatus *status = data.access()->insertGenericData(o3);
        if( (NULL == status ) || !status->isOk() ) {
            error(QString("error inserting object 3:%1").arg((NULL!=status?status->message():"")));
        }
        if( NULL != status ) {
            delete status;
        }
    }
    o1->setName("aaa");
    o1->setDescription("bbb");
    o1->setPayload("ggg");
    o1->tags().insert("kkk");
    o1->tags().insert("eee1");
    o1->setUuid("ffff");
    if(!isError()) {
        OperationStatus *status = data.access()->updateGenericData(o1);
        if( (NULL == status ) || !status->isOk() ) {
            error(QString("error updating object :%1").arg((NULL!=status?status->message():"")));
        }
        if( NULL != status ) {
            delete status;
        }
    }

    if(!isError()) {
        QList<GenericPersistentData*> resultList;
        OperationStatus *status = data.access()->readAllGenericData(QXMLE_PDATA_TypeSearchlet, resultList) ;
        if( (NULL == status ) || !status->isOk() ) {
            error(QString("error reading data 2 :%1").arg((NULL!=status?status->message():"")));
        }

        if( NULL != status ) {
            delete status;
        }

        bool eq = false;
        if(!isError()) {
            foreach( GenericPersistentData* gd, resultList) {
                QString field;
                if(gd->id() == o1->id()) {
                    eq = true;
                    if(!gd->isEqual(o1, &field)) {
                        error(QString("Reading after update differs for field :%1").arg(field));
                    }
                }
            }
        }
        if(!isError()) {
            if( !eq ) {
                error(QString("Object missing in update"));
            }
        }

        foreach(GenericPersistentData* gData, resultList) {
            delete gData;
        }

    }

    if(NULL!=o1)delete o1;
    if(NULL!=o2)delete o2;
    if(NULL!=o3)delete o3;

    if(isError()) {
        return false;
    }
    return true;
}

bool TestSearchlets::testDelete()
{
    _testName = "testDelete";
    TestSearchlets::Data data;
    if(!data.init()) {
        return error(QString(" data init :%1").arg(data.status));
    }

    GenericPersistentData *o1 = data.storageManager()->newPersistentDatum(QXMLE_PDATA_TypeSearchlet);
    if( NULL == o1 ) {
        error("No generic object 1");
    }
    GenericPersistentData *o2 = data.storageManager()->newPersistentDatum(QXMLE_PDATA_TypeSearchlet);
    if( NULL == o2 ) {
        error("No generic object 2");
    }
    if(!isError()) {
        OperationStatus *status = data.access()->insertGenericData(o1);
        if( (NULL == status ) || !status->isOk() ) {
            error(QString("error inserting object 1:%1").arg((NULL!=status?status->message():"")));
        }
        if( NULL != status ) {
            delete status;
        }
    }
    if(!isError()) {
        OperationStatus *status = data.access()->insertGenericData(o2);
        if( (NULL == status ) || !status->isOk() ) {
            error(QString("error inserting object 2:%1").arg((NULL!=status?status->message():"")));
        }
        if( NULL != status ) {
            delete status;
        }
    }
    if(!isError()) {
        OperationStatus *status = data.access()->deleteGenericData(o1) ;
        if( (NULL == status ) || !status->isOk() ) {
            error(QString("error deleteing data 1:%1").arg((NULL!=status?status->message():"")));
        }

        if( NULL != status ) {
            delete status;
        }
    }

    if(!isError()) {
        QList<GenericPersistentData*> resultList;
        OperationStatus *status = data.access()->readAllGenericData(QXMLE_PDATA_TypeSearchlet, resultList) ;
        if( (NULL == status ) || !status->isOk() ) {
            error(QString("error reading data :%1").arg((NULL!=status?status->message():"")));
        }

        if( NULL != status ) {
            delete status;
        }
        if(!isError()) {
            if(resultList.size()!= 1) {
                error(QString("Reading items expected 1, found :%1").arg(resultList.size()));
            }
        }
        if(!isError()) {
            GenericPersistentData* gd = resultList.at(0) ;
            QString field;
            if(!gd->isEqual(o2, &field)) {
                error(QString("Reading after delete differs for field :%1").arg(field));
            }
        }

        foreach(GenericPersistentData* gData, resultList) {
            delete gData;
        }

    }


    if(NULL!=o1)delete o1;
    if(NULL!=o2)delete o2;

    if(isError()) {
        return false;
    }
    return true;
}

//---------------------------------

bool TestSearchlets::testUnitTables()
{
    _testName = "testUnitTables" ;
    if(!testAccessTable()) {
        return false;
    }
    if(!testInsert()) {
        return false;
    }
    if(!testInsertAndRead()) {
        return false;
    }
    if(!testInsertAndReadOther()) {
        return false;
    }
    if(!testUpdate()) {
        return false;
    }
    if(!testDelete()) {
        return false;
    }
    return true;
}

bool TestSearchlets::testUnit()
{
    _testName = "testUnit" ;
    //*1-create object and test db access
    if(!testUnitBase()) {
        return false;
    }
    if(!testUnitObject()) {
        return false;
    }
    if(!testUnitTables()) {
        return false;
    }
    return true;
}

//-------------------

class TestSearchletManagerFactory;
class TestSearchletManager;

class TestSearchletManagerFactory : public SearchletManagerFactory
{
public:
    QString start;
    QString args;
    QString result;


    TestSearchletManagerFactory(const QString &newStart, const QString &newResult)
    {
        start = newStart;
        result = newResult;
    }
    ~TestSearchletManagerFactory(){}

    SearchletManager* newSearchletManager();
};

class TestSearchletManager : public SearchletManager
{
public:
    TestSearchletManagerFactory *_parent;

    TestSearchletManager(TestSearchletManagerFactory *newParent){ _parent = newParent ;}
    virtual ~TestSearchletManager(){}

    virtual QString chooseSearchlets(QXmlEditData * /*data*/, QWidget * /*parent*/) {
        return _parent->result;
    }

    virtual void insertSearchlet(QWidget * /*parent*/, QXmlEditData * /*data*/, const QString &text)
    {
        _parent->args = text;
    }

};

SearchletManager* TestSearchletManagerFactory::newSearchletManager()
{
    return new TestSearchletManager(this);
}

//---

bool TestSearchlets::testInvokeSearchlet()
{
    _testName = "testInvokeSearchlet" ;
    App app;
    if(!app.init() ) {
        return error("init");
    }
    XmlEditWidget *editor = app.mainWindow()->getEditor();
    if( NULL == editor ) {
        return error("no editor");
    }
    SearchWidget *searchWidget = editor->findChild<SearchWidget*>("searchWidget");
    if( NULL == searchWidget ) {
        return error("no search widget");
    }
    QString testString = "abc123" ;
    QString testStringResult = "xxxGGG" ;
    searchWidget->setSearchletManagerFactory(new TestSearchletManagerFactory(testString, testStringResult));
    QComboBox *searchBox = searchWidget->findChild<QComboBox*>("searchBox");
    if( NULL == searchBox ) {
        return error("no searchBox");
    }
    searchBox->setEditText(testString);
    QToolButton *cmdSearchlets = searchWidget->findChild<QToolButton *>("cmdSearchlets");
    if( NULL == cmdSearchlets ) {
        return error("no cmdSearchlets");
    }
    cmdSearchlets->click();
    if( searchBox->currentText() != testStringResult ) {
        return error(QString("Expecting :'%1', found:'%2'").arg(testStringResult).arg(searchBox->currentText()));
    }
   return true;
}


// test the save command
bool TestSearchlets::testSaveSearchlet()
{
    _testName = "testSaveSearchlet" ;
    App app;
    if(!app.init() ) {
        return error("init");
    }
    XmlEditWidget *editor = app.mainWindow()->getEditor();
    if( NULL == editor ) {
        return error("no editor");
    }
    SearchWidget *searchWidget = editor->findChild<SearchWidget*>("searchWidget");
    if( NULL == searchWidget ) {
        return error("no search widget");
    }
    QString testString = "abc123" ;
    QString testStringResult = "xxxGGG" ;
    TestSearchletManagerFactory * testSearchletManagerFactory = new TestSearchletManagerFactory(testString, testStringResult);
    searchWidget->setSearchletManagerFactory(testSearchletManagerFactory);
    QComboBox *searchBox = searchWidget->findChild<QComboBox*>("searchBox");
    if( NULL == searchBox ) {
        return error("no searchBox");
    }
    searchBox->setEditText(testString);
    QToolButton *cmdSearchlets = searchWidget->findChild<QToolButton *>("cmdSearchlets");
    if( NULL == cmdSearchlets ) {
        return error("no cmdSearchlets");
    }
    QAction *actionInsSearch = NULL ;
    foreach( QAction *action, cmdSearchlets->actions() ) {
        if( action->objectName() == "saveSearchletsAction" ) {
            actionInsSearch = action ;
            break;
        }
    }
    if( NULL == actionInsSearch ) {
        return error("no actionInsSearch");
    }
    actionInsSearch->trigger();
    if( testSearchletManagerFactory->args != testString ) {
        return error(QString("Expecting :'%1', found:'%2'").arg(testString).arg(testSearchletManagerFactory->args));
    }
   return true;
}


bool TestSearchlets::testFunctional()
{
    _testName = "testFunctional" ;
    if(!testInvokeSearchlet()) {
        return false;
    }
    if(!testSaveSearchlet()) {
        return false;
    }
    return true;
}
