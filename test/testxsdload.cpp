/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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


#include "testxsdload.h"
#include "xsdeditor/xschema.h"
#include "xsdeditor/io/xschemaloader.h"
#include "testhelpers/fakenetworkaccessmanager.h"
#include "helpers/compareschemaobjects.h"

#include <QTest>

#define FILE_TEST_LOAD_SIMPLE   "../test/data/xsd/load/simple.xsd"
#define TEST_LOAD_SIMPLE_NETWORK   "jshdfjshjkshdfjsfd://ksjfd_bkasfdblkzdbvmzbvcbvc"
#define FILE_TEST_LOAD_INCLUDING   "../test/data/xsd/load/deps/including.xsd"
#define FILE_TEST_LOAD_INCLUDED   "../test/data/xsd/load/deps/included.xsd"
#define FILE_TEST_LOAD_INCLUDING2   "../test/data/xsd/load/deps/including2.xsd"
#define FILE_TEST_LOAD_REDEFINE   "../test/data/xsd/load/deps/redefine.xsd"
#define FILE_TEST_TYPES_NONAMESPACE "../test/data/xsd/load/types/nonamespace.xsd"
#define FILE_TEST_TYPES_NONAMESPACE_INCLUDING "../test/data/xsd/load/types/nonamespace_including.xsd"
#define FILE_TEST_TYPES_NONAMESPACE_INCLUDED "../test/data/xsd/load/types/nonamespace_included.xsd"
#define FILE_TEST_TYPES_NONAMESPACE_INCLUDED2 "../test/data/xsd/load/types/nonamespace_included2.xsd"
#define FILE_TEST_TYPES_NAMESPACE_INCLUDING "../test/data/xsd/load/types/namespace_including.xsd"
#define FILE_TEST_TYPES_NAMESPACE_DEFAULT_INCLUDING "../test/data/xsd/load/types/namespace_def_including.xsd"
#define FILE_TEST_TYPES_NAMESPACE_NONDEFAULT_INCLUDING "../test/data/xsd/load/types/namespace_nodef_including.xsd"
#define FILE_TEST_IMPORT_AND_INCLUDE "../test/data/xsd/load/types/import/a.xsd"
#define FILE_TEST_TYPES_NAMESPACE_REDEFINING "../test/data/xsd/load/types/namespace_redefining.xsd"


//---------------------------------------
TestHelperXLists::TestHelperXLists()
{
}

TestHelperXLists::~TestHelperXLists()
{
}

//---------------------------------------



TestXSDLoad::TestXSDLoad() : QObject(NULL)
{
    _receivedOk = false;
    _receivedKO = false ;
    _go = false;
}

TestXSDLoad::~TestXSDLoad()
{
}


bool TestXSDLoad::testUnits()
{
    _testName = "testUnits";
    if(!testLoadSimpleSync()) {
        return false;
    }
    if(!testLoadSimpleNetworkFileSync()) {
        return false;
    }
    if(!testLoadSimpleNetworkSync()) {
        return false;
    }
    if(!testLoadSimpleASync()) {
        return false;
    }
    if(!testLoadSimpleNetworkFileASync()) {
        return false;
    }
    if(!testLoadSimpleNetworkASync()) {
        return false;
    }
    return true;
}

bool TestXSDLoad::testInclude()
{
    _testName = "testInclude";
    if(!testIncludeSimpleSync()) {
        return false;
    }
    if(!testIncludeSimpleAsync()) {
        return false;
    }
    if(!testIncludeSimpleDoubleSync()) {
        return false;
    }
    return true;
}

bool TestXSDLoad::testRedefine()
{
    _testName = "testRedefine";
    if(!testRedefineSimpleSync()) {
        return false;
    }
    if(!testRedefineSimpleAsync()) {
        return false;
    }
    return true;
}

bool TestXSDLoad::testTypes()
{
    _testName = "testTypes";
    if(!testTypesWithoutNamespace()) {
        return false;
    }
    if(!testTypesWithoutNamespaceInclude()) {
        return false;
    }
    if(!testIncludeSimpleNamespaces()) {
        return false;
    }
    if(!testIncludeSimpleNamespacesDefault()) {
        return false;
    }
    if(!testIncludeSimpleNamespacesNonDefault()) {
        return false;
    }
    if(!testImportAndInclude()) {
        return false;
    }
    if(!testRedefineTypes()) {
        return false;
    }
    return true;
}


bool TestXSDLoad::checkMode(XSchemaLoader *loader, const bool expected)
{
    if( loader->isAsynchMode() != expected ) {
        return error(QString("Async mode problem expected %1, found '%2'").arg(expected).arg(loader->isAsynchMode()));
    }
    return true;
}

bool TestXSDLoad::testLoadSimpleSync()
{
    _testName = "testLoadSimpleSync" ;
    App app;
    if(!app.init()) {
        return error("init app");
    }
    bool asynch = false ;
    XSchemaLoader loader;
    XSDLoadContext loaderContext;
    XSchemaLoader::State state = loader.load(&loaderContext, FILE_TEST_LOAD_SIMPLE, asynch, "");
    if( XSchemaLoader::STATE_READY != state ) {
        return error(QString("expected STATE_READY state, found '%1', file: '%2'").arg(state).arg(FILE_TEST_LOAD_SIMPLE));
    }
    if( XSchemaLoader::SCHEMA_READY != loader.code() ) {
        return error(QString("expected SCHEMA_READY code, found '%1', file: '%2'").arg(loader.code()).arg(FILE_TEST_LOAD_SIMPLE));
    }
    if( NULL == loader.schema()) {
        return error(QString("expected not null schema, file: '%1'").arg(FILE_TEST_LOAD_SIMPLE));
    }
    if( !checkMode(&loader, asynch) ) {
        return false;
    }

    return true;
}

bool TestXSDLoad::testLoadSimpleNetworkFileSync()
{
    _testName = "testLoadSimpleNetworkFileSync" ;
    App app;
    if(!app.init()) {
        return error("init app");
    }
    bool asynch = false ;
    QFile file(FILE_TEST_LOAD_SIMPLE);
    QFileInfo fileInfo(file);
    QUrl url = QUrl::fromLocalFile(fileInfo.absoluteFilePath());
    QString fileUrl = url.toString();
    XSchemaLoader loader;
    FakeNetworkAccessManager mockNetworkManager("");
    XSDLoadContext loaderContext;
    XSchemaLoader::State state = loader.load(&loaderContext, fileUrl, asynch, "", &mockNetworkManager);
    if( XSchemaLoader::STATE_READY != state ) {
        return error(QString("expected STATE_READY state, found '%1', file: '%2'").arg(state).arg(fileUrl));
    }
    if( XSchemaLoader::SCHEMA_READY != loader.code() ) {
        return error(QString("expected SCHEMA_READY code, found '%1', file: '%2'").arg(loader.code()).arg(fileUrl));
    }
    if( NULL == loader.schema()) {
        return error(QString("expected not null schema, file: '%1'").arg(fileUrl));
    }
    if( !checkMode(&loader, asynch) ) {
        return false;
    }

    return true;
}

bool TestXSDLoad::testLoadSimpleNetworkSync()
{
    _testName = "testLoadSimpleNetworkSync" ;
    App app;
    if(!app.init()) {
        return error("init app");
    }
    bool asynch = false ;
    FakeNetworkAccessManager mockNetworkManager(FILE_TEST_LOAD_SIMPLE);
    XSchemaLoader loader;
    XSDLoadContext loaderContext;
    XSchemaLoader::State state = loader.load(&loaderContext, TEST_LOAD_SIMPLE_NETWORK, asynch, "", &mockNetworkManager);
    if( XSchemaLoader::STATE_READY != state ) {
        return error(QString("expected STATE_READY state, found '%1', file: '%2'").arg(state).arg(TEST_LOAD_SIMPLE_NETWORK));
    }
    if( XSchemaLoader::SCHEMA_READY != loader.code() ) {
        return error(QString("expected SCHEMA_READY code, found '%1', file: '%2'").arg(loader.code()).arg(TEST_LOAD_SIMPLE_NETWORK));
    }
    if( NULL == loader.schema()) {
        return error(QString("expected not null schema, file: '%1'").arg(TEST_LOAD_SIMPLE_NETWORK));
    }
    if( !checkMode(&loader, asynch) ) {
        return false;
    }
    return true;
}

//------
bool TestXSDLoad::testLoadSimpleASync()
{
    _testName = "testUnits/testLoadSimpleASync" ;
    App app;
    if(!app.init()) {
        return error("init app");
    }
    bool asynch = true ;
    XSchemaLoader loader;
    FakeNetworkAccessManager mockNetworkManager("");
    XSDLoadContext loaderContext;
    XSchemaLoader::State state = loader.load(&loaderContext, FILE_TEST_LOAD_SIMPLE, asynch, "",  &mockNetworkManager);
    if( XSchemaLoader::STATE_READY != state ) {
        return error(QString("expected STATE_READY state, found '%1', file: '%2'").arg(state).arg(FILE_TEST_LOAD_SIMPLE));
    }
    if( XSchemaLoader::SCHEMA_READY != loader.code() ) {
        return error(QString("expected SCHEMA_READY code, found '%1', file: '%2'").arg(loader.code()).arg(FILE_TEST_LOAD_SIMPLE));
    }
    if( NULL == loader.schema()) {
        return error(QString("expected not null schema, file: '%1'").arg(FILE_TEST_LOAD_SIMPLE));
    }
    if( !checkMode(&loader, asynch) ) {
        return false;
    }

    return true;
}

bool TestXSDLoad::testLoadSimpleNetworkFileASync()
{
    _testName = "testLoadSimpleNetworkFileASync" ;
    App app;
    if(!app.init()) {
        return error("init app");
    }
    bool asynch = true ;
    QFile file(FILE_TEST_LOAD_SIMPLE);
    QFileInfo fileInfo(file);
    QUrl url = QUrl::fromLocalFile(fileInfo.absoluteFilePath());
    QString fileUrl = url.toString();
    XSchemaLoader loader;
    FakeNetworkAccessManager mockNetworkManager("");
    XSDLoadContext loaderContext;
    XSchemaLoader::State state = loader.load(&loaderContext, fileUrl, asynch, "", &mockNetworkManager);
    if( XSchemaLoader::STATE_READY != state ) {
        return error(QString("expected STATE_READY state, found '%1', file: '%2'").arg(state).arg(fileUrl));
    }
    if( XSchemaLoader::SCHEMA_READY != loader.code() ) {
        return error(QString("expected SCHEMA_READY code, found '%1', file: '%2'").arg(loader.code()).arg(fileUrl));
    }
    if( NULL == loader.schema()) {
        return error(QString("expected not null schema, file: '%1'").arg(fileUrl));
    }
    if( !checkMode(&loader, asynch) ) {
        return false;
    }

    return true;
}

bool TestXSDLoad::testLoadSimpleNetworkASync()
{
    _receivedOk = false;
    _receivedKO = false ;
    _go = false;

    _testName = "testLoadSimpleNetworkASync" ;
    App app;
    if(!app.init()) {
        return error("init app");
    }
    bool asynch = true ;
    FakeNetworkAccessManager mockNetworkManager(FILE_TEST_LOAD_SIMPLE);
    XSchemaLoader loader;
    connect(&loader, SIGNAL(finished( XSchemaLoader *, const XSchemaLoader::Code )), this, SLOT(onFinished( XSchemaLoader *, const XSchemaLoader::Code )));
    XSDLoadContext loaderContext;
    XSchemaLoader::State state = loader.load(&loaderContext, TEST_LOAD_SIMPLE_NETWORK, asynch, "", &mockNetworkManager);
    // for qt5 only, use QSignalSpy
    int cnt = 0 ;
    int MAX_TIMEOUT_CNT = 200;
    while(!_go && (cnt<MAX_TIMEOUT_CNT)) {
        QTest::qWait(10);
        cnt++;
    }
    disconnect(&loader, SIGNAL(finished( XSchemaLoader *, const XSchemaLoader::Code )), this, SLOT(onFinished( XSchemaLoader *, const XSchemaLoader::Code )));
    if( XSchemaLoader::STATE_READY != state ) {
        return error(QString("expected STATE_READY state, found '%1', file: '%2'").arg(state).arg(TEST_LOAD_SIMPLE_NETWORK));
    }
    if( XSchemaLoader::SCHEMA_READY != loader.code() ) {
        return error(QString("expected SCHEMA_READY code, found '%1', file: '%2'").arg(loader.code()).arg(TEST_LOAD_SIMPLE_NETWORK));
    }
    if( NULL == loader.schema()) {
        return error(QString("expected not null schema, file: '%1'").arg(TEST_LOAD_SIMPLE_NETWORK));
    }
    if( !checkMode(&loader, asynch) ) {
        return false;
    }
    return true;
}

void TestXSDLoad::onFinished( XSchemaLoader * /*entity*/, const XSchemaLoader::Code code )
{
    if( code == XSchemaLoader::SCHEMA_READY ) {
        _receivedOk = true;
    } else {
        _receivedKO = true;
    }
    _go = true;
}

//--------------------------------- section(INCLUDE) -------

bool TestXSDLoad::testIncludeSimpleSync()
{
    _testName = "testIncludeSimpleSync" ;
    App app;
    if(!app.init()) {
        return error("init app");
    }
    QString fileIn = FILE_TEST_LOAD_INCLUDING;
    bool asynch = false ;
    XSchemaLoader loader;
    XSDLoadContext loaderContext;
    XSchemaLoader::State state = loader.load(&loaderContext, fileIn, asynch, "");
    if( XSchemaLoader::STATE_READY != state ) {
        return error(QString("expected STATE_READY state, found '%1', file: '%2'").arg(state).arg(fileIn));
    }
    if( XSchemaLoader::SCHEMA_READY != loader.code() ) {
        return error(QString("expected SCHEMA_READY code, found '%1', file: '%2'").arg(loader.code()).arg(fileIn));
    }
    if( NULL == loader.schema()) {
        return error(QString("expected not null schema, file: '%1'").arg(fileIn));
    }
    if( !checkMode(&loader, asynch) ) {
        return false;
    }

    return true;
}


bool TestXSDLoad::testIncludeSimpleAsync()
{
    _receivedOk = false;
    _receivedKO = false ;
    _go = false;

    _testName = "testIncludeSimpleAsync" ;
    App app;
    if(!app.init()) {
        return error("init app");
    }
    bool asynch = true ;
    FakeNetworkAccessManager mockNetworkManager(FILE_TEST_LOAD_INCLUDING);
    XSchemaLoader loader;
    connect(&loader, SIGNAL(finished( XSchemaLoader *, const XSchemaLoader::Code )), this, SLOT(onFinished( XSchemaLoader *, const XSchemaLoader::Code )));
    XSDLoadContext loaderContext;
    XSchemaLoader::State state = loader.load(&loaderContext, TEST_LOAD_SIMPLE_NETWORK, asynch, "", &mockNetworkManager);
    // for qt5 only, use QSignalSpy
    int cnt = 0 ;
    int MAX_TIMEOUT_CNT = 200;
    while(!_go && (cnt<MAX_TIMEOUT_CNT)) {
        QTest::qWait(10);
        cnt++;
    }
    disconnect(&loader, SIGNAL(finished( XSchemaLoader *, const XSchemaLoader::Code )), this, SLOT(onFinished( XSchemaLoader *, const XSchemaLoader::Code )));
    if( XSchemaLoader::STATE_READY != state ) {
        return error(QString("expected STATE_READY state, found '%1', file: '%2'").arg(state).arg(TEST_LOAD_SIMPLE_NETWORK));
    }
    if( XSchemaLoader::SCHEMA_READY != loader.code() ) {
        return error(QString("expected SCHEMA_READY code, found '%1', file: '%2'").arg(loader.code()).arg(TEST_LOAD_SIMPLE_NETWORK));
    }
    if( NULL == loader.schema()) {
        return error(QString("expected not null schema, file: '%1'").arg(TEST_LOAD_SIMPLE_NETWORK));
    }
    if( !checkMode(&loader, asynch) ) {
        return false;
    }
    return true;
}

bool TestXSDLoad::testIncludeSimpleDoubleSync()
{
    _testName = "testIncludeSimpleDoubleSync" ;
    App app;
    if(!app.init()) {
        return error("init app");
    }
    QString fileIn = FILE_TEST_LOAD_INCLUDING2;
    bool asynch = false ;
    XSchemaLoader loader;
    XSDLoadContext loaderContext;
    XSchemaLoader::State state = loader.load(&loaderContext, fileIn, asynch, "");
    if( XSchemaLoader::STATE_READY != state ) {
        return error(QString("expected STATE_READY state, found '%1', file: '%2'").arg(state).arg(fileIn));
    }
    if( XSchemaLoader::SCHEMA_READY != loader.code() ) {
        return error(QString("expected SCHEMA_READY code, found '%1', file: '%2'").arg(loader.code()).arg(fileIn));
    }
    if( NULL == loader.schema()) {
        return error(QString("expected not null schema, file: '%1'").arg(fileIn));
    }
    if( !checkMode(&loader, asynch) ) {
        return false;
    }
    if( loader.schema()->infoPool()->includes().size() != 2 ) {
        return error(QString("expected 2 includes, found : '%1'").arg(loader.schema()->infoPool()->includes().size()));
    }

    return true;
}

//--------------------------------- endsection(INCLUDE) -------

//--------------------------------- section(REDEFINE) ---------
bool TestXSDLoad::testRedefineSimpleSync()
{
    _testName = "testRedefineSimpleSync" ;
    App app;
    if(!app.init()) {
        return error("init app");
    }
    QString fileIn = FILE_TEST_LOAD_REDEFINE;
    bool asynch = false ;
    XSchemaLoader loader;
    XSDLoadContext loaderContext;
    XSchemaLoader::State state = loader.load(&loaderContext, fileIn, asynch, "");
    if( XSchemaLoader::STATE_READY != state ) {
        return error(QString("expected STATE_READY state, found '%1', file: '%2'").arg(state).arg(fileIn));
    }
    if( XSchemaLoader::SCHEMA_READY != loader.code() ) {
        return error(QString("expected SCHEMA_READY code, found '%1', file: '%2'").arg(loader.code()).arg(fileIn));
    }
    if( NULL == loader.schema()) {
        return error(QString("expected not null schema, file: '%1'").arg(fileIn));
    }
    if( !checkMode(&loader, asynch) ) {
        return false;
    }

    return true;
}


bool TestXSDLoad::testRedefineSimpleAsync()
{
    _receivedOk = false;
    _receivedKO = false ;
    _go = false;

    _testName = "testRedefineSimpleAsync" ;
    App app;
    if(!app.init()) {
        return error("init app");
    }
    bool asynch = true ;
    FakeNetworkAccessManager mockNetworkManager(FILE_TEST_LOAD_REDEFINE);
    XSchemaLoader loader;
    XSDLoadContext loaderContext;
    connect(&loader, SIGNAL(finished( XSchemaLoader *, const XSchemaLoader::Code )), this, SLOT(onFinished( XSchemaLoader *, const XSchemaLoader::Code )));
    XSchemaLoader::State state = loader.load(&loaderContext, TEST_LOAD_SIMPLE_NETWORK, asynch, "", &mockNetworkManager);
    // for qt5 only, use QSignalSpy
    int cnt = 0 ;
    int MAX_TIMEOUT_CNT = 200;
    while(!_go && (cnt<MAX_TIMEOUT_CNT)) {
        QTest::qWait(10);
        cnt++;
    }
    disconnect(&loader, SIGNAL(finished( XSchemaLoader *, const XSchemaLoader::Code )), this, SLOT(onFinished( XSchemaLoader *, const XSchemaLoader::Code )));
    if( XSchemaLoader::STATE_READY != state ) {
        return error(QString("expected STATE_READY state, found '%1', file: '%2'").arg(state).arg(FILE_TEST_LOAD_REDEFINE));
    }
    if( XSchemaLoader::SCHEMA_READY != loader.code() ) {
        return error(QString("expected SCHEMA_READY code, found '%1', file: '%2'").arg(loader.code()).arg(FILE_TEST_LOAD_REDEFINE));
    }
    if( NULL == loader.schema()) {
        return error(QString("expected not null schema, file: '%1'").arg(FILE_TEST_LOAD_REDEFINE));
    }
    if( !checkMode(&loader, asynch) ) {
        return false;
    }
    return true;
}

//--------------------------------- endsection(REDEFINE) -------

//--------------------------------- section(TYPES) -------
XSDSchema* TestXSDLoad::testLoadTypes(const QString &fileIn)
{
    XSDSchema *result = NULL ;
    App app;
    if(!app.init()) {
        error("init app");
        return NULL;
    }
    XSchemaLoader loader;
    XSDLoadContext loaderContext;
    XSchemaLoader::State state = loader.load(&loaderContext, fileIn, false, "");
    if( XSchemaLoader::STATE_READY != state ) {
        error(QString("expected STATE_READY state, found '%1', file: '%2'").arg(state).arg(fileIn));
        return NULL ;
    }
    if( XSchemaLoader::SCHEMA_READY != loader.code() ) {
        error(QString("expected SCHEMA_READY code, found '%1', file: '%2'").arg(loader.code()).arg(fileIn));
        return NULL;
    }
    if( NULL == loader.schema()) {
        error(QString("expected not null schema, file: '%1'").arg(fileIn));
        return NULL ;
    }
    if( !checkMode(&loader, false) ) {
        return NULL;
    }
    result = loader.getSchemaAndTakeOwnership();
    return result;
}

QString TestXSDLoad::fakeName(const QString &name)
{
    return QString("%1____fake").arg(name);
}

bool TestXSDLoad::errorObjectAndDeleteSchema(XSDSchema* schema, const QString &name, const ESchemaType typeSpec, const bool reverse )
{
    if( NULL != schema ) {
        delete schema;
    }
    if( reverse) {
        return error(QString("Object found but unexpected:'%1' spec:%2").arg(name).arg(typeSpec));
    } else {
        return error(QString("Object not found:'%1' spec:%2").arg(name).arg(typeSpec));
    }
}

bool TestXSDLoad::errorNameAndDeleteSchema(XSDSchema* schema, const QString &realName, const QString &name, const ESchemaType typeSpec, const bool reverse )
{
    if( NULL != schema ) {
        delete schema;
    }
    if( reverse) {
        return error(QString("Element name unexpected:'%1' for:'%2' spec:%3").arg(realName).arg(name).arg(typeSpec));
    } else {
        return error(QString("Element name found:'%1' for:'%2' spec:%3").arg(realName).arg(name).arg(typeSpec));
    }
}

bool TestXSDLoad::errorSpecAndDeleteSchema(XSDSchema* schema, const QString &name, const ESchemaType typeSpec, const bool reverse )
{
    if( NULL != schema ) {
        delete schema;
    }
    if( reverse) {
        return error(QString("Element found unexpected:'%1' spec:%2").arg(name).arg(typeSpec));
    } else {
        return error(QString("Element not found:'%1' spec:%2").arg(name).arg(typeSpec));
    }
}

bool TestXSDLoad::errorSpecAndDeleteSchemaFalse(XSDSchema* schema, const QString &name, const ESchemaType typeSpec )
{
    if( NULL != schema ) {
        delete schema;
    }
    return error(QString("Spec found when not present:'%1' spec:%2").arg(name).arg(typeSpec));
}

bool TestXSDLoad::errorRootAndDeleteSchema(XSDSchema* schema, const QString &name, const ESchemaType typeSpec )
{
    if( NULL != schema ) {
        delete schema;
    }
    return error(QString("Schema root not expected:'%1' spec:%2").arg(name).arg(typeSpec));
}

bool TestXSDLoad::errorObjectAndDeleteSchemaFalse(XSDSchema* schema, const QString &name, const ESchemaType typeSpec )
{
    if( NULL != schema ) {
        delete schema;
    }
    return error(QString("Object found when not expected:'%1' spec:%2").arg(name).arg(typeSpec));
}

bool TestXSDLoad::errorMessageAndDeleteSchema(XSDSchema* schema, const QString &name, const QString &msg )
{
    if( NULL != schema ) {
        delete schema;
    }
    return error(QString("Error %1: %2").arg(name).arg(msg));
}

bool TestXSDLoad::checkSchemaStructure(XSDSchema* schema, const int includesExpected )
{
    int size = schema->infoPool()->includes().size() ;
    if( includesExpected != size ) {
        if( NULL != schema ) {
            delete schema;
        }
        return error(QString("Include size expected:'%1' found:%2").arg(includesExpected).arg(size));
    }
    return true;
}

bool TestXSDLoad::cmpNames(const QString &name, XSchemaObject *object )
{
    if( NULL == object ) {
        return false;
    }
    QString localName = name;
    int colonIndex = name.indexOf(":");
    if(colonIndex >= 0) {
        localName = name.mid(colonIndex + 1);
    }
    if( object->name() == localName ) {
        return true ;
    }
    return false;
}

bool TestXSDLoad::findObject(XSDSchema* schema, const QString &name, const ESchemaType typeSpec, const bool reverse )
{
    XSchemaObject *object = schema->findTopObject(name, typeSpec) ;
    if( (!reverse && (NULL == object)) || (reverse && (NULL != object) ) ) {
        return errorObjectAndDeleteSchema(schema, name, typeSpec, reverse);
    }
    bool isCmpName = cmpNames( name, object );
    if( (!reverse && !isCmpName) || (reverse && isCmpName ) ) {
        return errorNameAndDeleteSchema(schema, object->name(), name, typeSpec, reverse);
    }

    QString theFakeName = fakeName(name);
    if(NULL != schema->findTopObject(theFakeName, typeSpec)) {
        return errorObjectAndDeleteSchemaFalse(schema, name, typeSpec);
    }
    return true;
}

bool TestXSDLoad::findElement(XSDSchema* schema, const QString &name, const bool reverse)
{
    if( (!reverse && (NULL == schema->topLevelElement(name))) || (reverse && (NULL != schema->topLevelElement(name)) ) ) {
        return errorSpecAndDeleteSchema(schema, name, SchemaTypeElement, reverse);
    }
    QString theFakeName = fakeName(name);
    if( NULL != schema->topLevelElement(theFakeName)) {
        return errorSpecAndDeleteSchemaFalse(schema, name, SchemaTypeElement);
    }
    if(!findObject(schema, name, SchemaTypeElement, reverse)) {
        return false;
    }
    if(!findObject(schema, name, SchemaGenericElement, reverse)) {
        return false;
    }
    return true;
}

bool TestXSDLoad::findAttribute(XSDSchema* schema, const QString &name, const bool reverse)
{
    if( (!reverse && (NULL == schema->topLevelAttribute(name))) || (reverse && (NULL != schema->topLevelAttribute(name)) ) ) {
        return errorSpecAndDeleteSchema(schema, name, SchemaTypeAttribute, reverse);
    }
    QString theFakeName = fakeName(name);
    if( NULL != schema->topLevelAttribute(theFakeName)) {
        return errorSpecAndDeleteSchemaFalse(schema, name, SchemaTypeAttribute);
    }
    if(!findObject(schema, name, SchemaTypeAttribute, reverse)) {
        return false;
    }
    return true;
}

bool TestXSDLoad::findAttributeGroup(XSDSchema* schema, const QString &name, const bool reverse)
{
    if( (!reverse && (NULL == schema->topLevelAttributeGroup(name))) || (reverse && (NULL != schema->topLevelAttributeGroup(name)) ) ) {
        return errorSpecAndDeleteSchema(schema, name, SchemaTypeAttributeGroup, reverse);
    }
    QString theFakeName = fakeName(name);
    if( NULL != schema->topLevelAttributeGroup(theFakeName)) {
        return errorSpecAndDeleteSchemaFalse(schema, name, SchemaTypeAttributeGroup);
    }
    if(!findObject(schema, name, SchemaTypeAttributeGroup, reverse)) {
        return false;
    }
    return true;
}

bool TestXSDLoad::findGroup(XSDSchema* schema, const QString &name, const bool reverse)
{
    if( (!reverse && (NULL == schema->topLevelGroup(name))) || (reverse && (NULL != schema->topLevelGroup(name)) ) ) {
        return errorSpecAndDeleteSchema(schema, name, SchemaTypeGroup, reverse);
    }
    QString theFakeName = fakeName(name);
    if( NULL != schema->topLevelGroup(theFakeName)) {
        return errorSpecAndDeleteSchemaFalse(schema, name, SchemaTypeGroup);
    }
    if(!findObject(schema, name, SchemaTypeGroup, reverse)) {
        return false;
    }
    return true;
}


bool TestXSDLoad::findType(XSDSchema* schema, const QString &name, const bool reverse, XSDSchema *expectedSchema )
{
    XSchemaObject* target =schema->topLevelType(name);
    if( (!reverse && (NULL == target)) || (reverse && (NULL != target) ) ) {
        return errorSpecAndDeleteSchema(schema, name, SchemaGenericType, reverse);
    }
    QString theFakeName = fakeName(name);
    if( NULL != schema->topLevelType(theFakeName)) {
        return errorSpecAndDeleteSchemaFalse(schema, name, SchemaGenericType);
    }
    if(!findObject(schema, name, SchemaGenericType, reverse)) {
        return false;
    }
    if(!checkOwner(target, schema, name, reverse, expectedSchema ) ) {
        return false;
    }

    return true;
}


bool TestXSDLoad::checkOwner(XSchemaObject* target, XSDSchema* schema, const QString &name, const bool reverse, XSDSchema *expectedSchema )
{
    if(!reverse && (NULL != expectedSchema ) ) {
        if( target->root() != expectedSchema ) {
            return errorRootAndDeleteSchema(schema, name, SchemaGenericType );
        }
    }

    return true;
}


bool TestXSDLoad::testExpectedAttributes( XSDSchema* schema, QStringList expectedSingleSchema, const bool onlyThisSchemaOrAll)
{
    CompareSchemaObjects c;
    QString msg;
    QList<XSchemaObject*>result ;
    result = schema->topLevelAttributes(onlyThisSchemaOrAll);
    if( !c.compareListXObjects(msg, expectedSingleSchema, result, SchemaTypeAttribute )) {
        return errorMessageAndDeleteSchema(schema, QString("ExpectedAttributes, %1").arg(onlyThisSchemaOrAll?"single":"all"), c.error );
    }
    result = schema->topLevelListByName(SchemaTypeAttribute, onlyThisSchemaOrAll);
    if( !c.compareListXObjects(msg, expectedSingleSchema, result, SchemaTypeAttribute )) {
        return errorMessageAndDeleteSchema(schema, QString("ExpectedAttributes type, %1").arg(onlyThisSchemaOrAll?"single":"all"), c.error );
    }
    return true;
}

bool TestXSDLoad::testExpectedElements( XSDSchema* schema, QStringList expectedSingleSchema, const bool onlyThisSchemaOrAll)
{
    CompareSchemaObjects c;
    QString msg;
    QList<XSchemaObject*>result ;
    result = schema->topLevelElements(onlyThisSchemaOrAll);
    if( !c.compareListXObjects(msg, expectedSingleSchema, result, SchemaTypeElement )) {
        return errorMessageAndDeleteSchema( schema, QString("ExpectedElements, %1").arg(onlyThisSchemaOrAll?"single":"all"), c.error );
    }
    result = schema->getChildrenByType(SchemaGenericElement, onlyThisSchemaOrAll);
    if( !c.compareListXObjects(msg, expectedSingleSchema, result, SchemaTypeElement )) {
        return errorMessageAndDeleteSchema( schema, QString("ExpectedElements type, %1").arg(onlyThisSchemaOrAll?"single":"all"), c.error );
    }
    return true;
}

bool TestXSDLoad::testExpectedTypes( XSDSchema* schema, QStringList expectedSingleSchema, const bool onlyThisSchemaOrAll)
{
    CompareSchemaObjects c;
    QString msg;
    QList<XSchemaObject*>result ;
    result = schema->topLevelTypes(onlyThisSchemaOrAll);
    if( !c.compareListXObjects(msg, expectedSingleSchema, result, SchemaTypeElement )) {
        return errorMessageAndDeleteSchema( schema, QString("ExpectedTypes, %1").arg(onlyThisSchemaOrAll?"single":"all"), c.error );
    }
    result = schema->topLevelListByName(SchemaGenericType, onlyThisSchemaOrAll);
    if( !c.compareListXObjects(msg, expectedSingleSchema, result, SchemaTypeElement )) {
        return errorMessageAndDeleteSchema( schema, QString("ExpectedTypes type, %1").arg(onlyThisSchemaOrAll?"single":"all"), c.error );
    }
    return true;
}

bool TestXSDLoad::testExpectedAttributeGroups( XSDSchema* schema, QStringList expectedSingleSchema, const bool onlyThisSchemaOrAll)
{
    CompareSchemaObjects c;
    QString msg;
    QList<XSchemaObject*>result ;
    result = schema->topLevelAttributeGroups(onlyThisSchemaOrAll);
    if( !c.compareListXObjects(msg, expectedSingleSchema, result, SchemaTypeAttributeGroup )) {
        return errorMessageAndDeleteSchema( schema, QString("ExpectedAttributeGroups, %1").arg(onlyThisSchemaOrAll?"single":"all"), c.error );
    }
    result = schema->topLevelListByName(SchemaTypeAttributeGroup, onlyThisSchemaOrAll);
    if( !c.compareListXObjects(msg, expectedSingleSchema, result, SchemaTypeAttributeGroup )) {
        return errorMessageAndDeleteSchema( schema, QString("ExpectedAttributeGroups type, %1").arg(onlyThisSchemaOrAll?"single":"all"), c.error );
    }
    return true;
}

bool TestXSDLoad::testExpectedGroups( XSDSchema* schema, QStringList expectedSingleSchema, const bool onlyThisSchemaOrAll)
{
    CompareSchemaObjects c;
    QString msg;
    QList<XSchemaObject*>result ;
    result = schema->topLevelGroups(onlyThisSchemaOrAll);
    if( !c.compareListXObjects(msg, expectedSingleSchema, result, SchemaTypeGroup )) {
        return errorMessageAndDeleteSchema( schema, QString("ExpectedGroups, %1").arg(onlyThisSchemaOrAll?"single":"all"), c.error );
    }
    result = schema->topLevelListByName(SchemaTypeGroup, onlyThisSchemaOrAll);
    if( !c.compareListXObjects(msg, expectedSingleSchema, result, SchemaTypeGroup )) {
        return errorMessageAndDeleteSchema( schema, QString("ExpectedGroups type, %1").arg(onlyThisSchemaOrAll?"single":"all"), c.error );
    }
    return true;
}

bool TestXSDLoad::testExpectedReferences( XSDSchema* schema, QList<QPair<QString,ESchemaType> > &reference)
{
    CompareSchemaObjects c;
    QString msg;
    QList<XSchemaObject*>result ;
    result = schema->topLevelReferences();
    if( !c.compareListXReferences(msg, reference, result)) {
        return errorMessageAndDeleteSchema( schema, QString("Expected References"), c.error );
    }
    return true;
}

bool TestXSDLoad::testLists(XSDSchema* schema, const bool onlyThisSchemaOrAll, TestHelperXLists &helper )
{
    if( !testExpectedAttributes(schema, helper.attributes, onlyThisSchemaOrAll) ) {
        return false;
    }
    if( !testExpectedElements(schema, helper.elements, onlyThisSchemaOrAll) ) {
        return false;
    }
    if( !testExpectedGroups(schema, helper.groups, onlyThisSchemaOrAll) ) {
        return false;
    }
    if( !testExpectedAttributeGroups(schema, helper.attributeGroups, onlyThisSchemaOrAll) ) {
        return false;
    }
    if( !testExpectedTypes(schema, helper.types, onlyThisSchemaOrAll) ) {
        return false;
    }
    if( !testExpectedReferences(schema, helper.references) ) {
        return false;
    }
    return true ;
}

bool TestXSDLoad::testTypesWithoutNamespace()
{
    _testName = "testTypesWithoutNamespace" ;
    XSDSchema* schema = testLoadTypes(FILE_TEST_TYPES_NONAMESPACE);
    if( NULL == schema ) {
        return false ;
    }

    if(!checkSchemaStructure(schema, 1)){
        return false;
    }

    if(!findType(schema, "xsd:string")) {
        return false;
    }
    if(!findType(schema, "string", true)) {
        return false;
    }

    if(!findElement(schema, "elemento1")) {
        return false;
    }

    if(!findAttribute(schema, "attr1")) {
        return false;
    }

    if(!findAttributeGroup(schema, "ag1")) {
        return false;
    }

    if(!findGroup(schema, "group1")) {
        return false;
    }
    TestHelperXLists helperThis;
    helperThis.attributes << "attr1" << "attr1.1";
    helperThis.attributeGroups << "ag1"<< "ag1.1";
    helperThis.elements << "elemento1"<< "elemento1.1";
    helperThis.types<< "type1"<< "type1.1";
    helperThis.groups<< "group1"<< "group1.1";
    if( !testLists(schema, true, helperThis ) ) {
        return false;
    }
    if( !testLists(schema, false, helperThis ) ) {
        return false;
    }

    delete schema;
    return true ;
}

bool TestXSDLoad::testForObjects(XSDSchema* schema, const bool isSchemaQualified, const bool isObjectQualified, const bool isObjectUnqualified )
{
    if(!findType(schema, "xsd:string")) {
        return false;
    }

    if(!findType(schema, "string", isSchemaQualified)) {
        return false;
    }

    bool qual = !isObjectQualified;
    bool unqual = !isObjectUnqualified;

    if(!findType(schema, "type1", unqual)) {
        return false;
    }
    if(!findType(schema, "type2", unqual)) {
        return false;
    }
    if(!findType(schema, "type3", unqual)) {
        return false;
    }
    if(!findType(schema, "x:type1", qual)) {
        return false;
    }
    if(!findType(schema, "x:type2", qual)) {
        return false;
    }
    if(!findType(schema, "x:type3", qual)) {
        return false;
    }
    if(!findElement(schema, "x:elemento1", qual)) {
        return false;
    }
    if(!findElement(schema, "x:elemento2", qual)) {
        return false;
    }
    if(!findElement(schema, "x:elemento3", qual)) {
        return false;
    }
    if(!findElement(schema, "elemento1", unqual)) {
        return false;
    }
    if(!findElement(schema, "elemento2", unqual)) {
        return false;
    }
    if(!findElement(schema, "elemento3", unqual)) {
        return false;
    }
    if(!findAttribute(schema, "x:attr1", qual)) {
        return false;
    }

    if(!findAttribute(schema, "x:attr2", qual)) {
        return false;
    }
    if(!findAttribute(schema, "x:attr3", qual)) {
        return false;
    }
    if(!findAttribute(schema, "attr1", unqual)) {
        return false;
    }
    if(!findAttribute(schema, "attr2", unqual)) {
        return false;
    }
    if(!findAttribute(schema, "attr3", unqual)) {
        return false;
    }
    if(!findAttributeGroup(schema, "x:ag1", qual)) {
        return false;
    }

    if(!findAttributeGroup(schema, "x:ag2", qual)) {
        return false;
    }

    if(!findAttributeGroup(schema, "x:ag3", qual)) {
        return false;
    }
    if(!findAttributeGroup(schema, "ag1", unqual)) {
        return false;
    }

    if(!findAttributeGroup(schema, "ag2", unqual)) {
        return false;
    }

    if(!findAttributeGroup(schema, "ag3", unqual)) {
        return false;
    }

    if(!findGroup(schema, "x:group1", qual)) {
        return false;
    }

    if(!findGroup(schema, "x:group2", qual)) {
        return false;
    }

    if(!findGroup(schema, "x:group3", qual)) {
        return false;
    }

    if(!findGroup(schema, "x:group3a", true)) {
        return false;
    }

    if(!findGroup(schema, "group1", unqual)) {
        return false;
    }

    if(!findGroup(schema, "group2", unqual)) {
        return false;
    }

    if(!findGroup(schema, "group3", unqual)) {
        return false;
    }
    return true;
}

bool TestXSDLoad::testForLists(XSDSchema* schema, const QString &included )
{
    {
        TestHelperXLists helperThis;
        helperThis.attributes << "attr1" << "attr1.1" ;
        helperThis.attributeGroups << "ag1" << "ag1.1";
        helperThis.elements << "elemento1" << "elemento1.1";
        helperThis.types<< "type1" << "type1.1";
        helperThis.groups<< "group1" << "group1.1";
        helperThis.references << QPair<QString,ESchemaType>(included,SchemaTypeInclude);
        if( !testLists(schema, true, helperThis ) ) {
            return false;
        }
    }
    {
        TestHelperXLists helperAll;
        helperAll.attributes << "attr1" << "attr2" << "attr3" << "attr2.2" << "attr3.3" << "attr1.1" ;
        helperAll.attributeGroups << "ag1"<< "ag2"<<  "ag3" << "ag2.2" << "ag3.3" << "ag1.1";
        helperAll.elements << "elemento1" << "elemento2"<< "elemento3"<< "elemento1.1";
        helperAll.types<< "type1" << "type2"<< "type3" << "type2.2" << "type3.3"<<"type1.1";
        helperAll.groups<< "group1"<< "group2"<< "group3" << "group3.3" << "group2.2" << "group1.1";
        helperAll.references << QPair<QString,ESchemaType>(included,SchemaTypeInclude);
        if( !testLists(schema, false, helperAll ) ) {
            return false;
        }
    }
    return true ;
}

bool TestXSDLoad::testForListsRedefined(XSDSchema* schema, const QString &included )
{
    {
        TestHelperXLists helperThis;
        helperThis.attributes << "attr1" << "attr1.1" ;
        helperThis.attributeGroups << "ag1" << "ag1.1"<< "ag2" ;
        helperThis.elements << "elemento1" << "elemento1.1";
        helperThis.types<< "type1" << "type1.1"<< "type2" ;
        helperThis.groups<< "group1" << "group1.1"<< "group2" ;
        helperThis.references << QPair<QString,ESchemaType>(included,SchemaTypeRedefine);
        if( !testLists(schema, true, helperThis ) ) {
            return false;
        }
    }
    {
        TestHelperXLists helperAll;
        helperAll.attributes << "attr1" << "attr2" << "attr3" << "attr2.2" << "attr3.3" << "attr1.1" ;
        helperAll.attributeGroups << "ag1"<< "ag2"<<  "ag3" << "ag2.2" << "ag3.3" << "ag1.1";
        helperAll.elements << "elemento1" << "elemento2"<< "elemento3"<< "elemento1.1";
        helperAll.types<< "type1" << "type2"<< "type3" << "type2.2" << "type3.3"<<"type1.1";
        helperAll.groups<< "group1"<< "group2"<< "group3" << "group3.3" << "group2.2" << "group1.1";
        helperAll.references << QPair<QString,ESchemaType>(included,SchemaTypeRedefine);
        if( !testLists(schema, false, helperAll ) ) {
            return false;
        }
    }
    return true ;
}

bool TestXSDLoad::testForListsImported(XSDSchema* schema, const QString &included )
{
    {
        TestHelperXLists helperThis;
        helperThis.attributes << "attra" ;
        helperThis.attributeGroups << "aga"  ;
        helperThis.elements << "elementoa" ;
        helperThis.types<< "typea"  ;
        helperThis.groups<< "groupa" ;
        helperThis.references << QPair<QString,ESchemaType>(included,SchemaTypeInclude);
        helperThis.references << QPair<QString,ESchemaType>("c.p_a.xsd",SchemaTypeImport);
        if( !testLists(schema, true, helperThis ) ) {
            return false;
        }
    }
    {
        TestHelperXLists helperAll;
        helperAll.attributes << "attra" << "attrb" ;
        helperAll.attributeGroups << "aga"<< "agb";
        helperAll.elements << "elementoa" << "elementob";
        helperAll.types<< "typea" << "typeb";
        helperAll.groups<< "groupa"<< "groupb";
        helperAll.references << QPair<QString,ESchemaType>(included,SchemaTypeInclude);
        helperAll.references << QPair<QString,ESchemaType>("c.p_a.xsd",SchemaTypeImport);
        if( !testLists(schema, false, helperAll ) ) {
            return false;
        }
    }
    return true ;
}


bool TestXSDLoad::testTypesWithoutNamespaceInclude()
{
    _testName = "testTypesWithoutNamespaceInclude" ;
    XSDSchema* schema = testLoadTypes(FILE_TEST_TYPES_NONAMESPACE_INCLUDING);
    if( NULL == schema ) {
        return false ;
    }

    if(!checkSchemaStructure(schema, 3)){
        return false;
    }

    if(!testForObjects(schema, true, false, true ) ) {
        return false;
    }

    if(! testForLists(schema, "nonamespace_included.xsd" ) ) {
        return false;
    }
    delete schema;
    return true ;
}

bool TestXSDLoad::testIncludeSimpleNamespaces()
{
    _testName = "testIncludeSimpleNamespaces" ;
    XSDSchema* schema = testLoadTypes(FILE_TEST_TYPES_NAMESPACE_INCLUDING);
    if( NULL == schema ) {
        return false ;
    }

    if(!checkSchemaStructure(schema, 3)) {
        return false;
    }

    if(!testForObjects(schema, true, true, false ) ) {
        return false;
    }

    if(! testForLists(schema, "namespace_included.xsd") ) {
        return false;
    }

    delete schema;
    return true ;
}

bool TestXSDLoad::testIncludeSimpleNamespacesDefault()
{
    _testName = "testIncludeSimpleNamespacesDefault" ;
    XSDSchema* schema = testLoadTypes(FILE_TEST_TYPES_NAMESPACE_DEFAULT_INCLUDING);
    if( NULL == schema ) {
        return false ;
    }
    if(!checkSchemaStructure(schema, 3)){
        return false;
    }

    if(!testForObjects(schema, true, true, true ) ) {
        return false;
    }

    if(! testForLists(schema,"nonamespace_included.xsd" ) ) {
        return false;
    }
    delete schema;
    return true ;
}


bool TestXSDLoad::testIncludeSimpleNamespacesNonDefault()
{
    _testName = "testIncludeSimpleNamespacesNonDefault" ;
    XSDSchema* schema = testLoadTypes(FILE_TEST_TYPES_NAMESPACE_NONDEFAULT_INCLUDING);
    if( NULL == schema ) {
        return false ;
    }

    if(!checkSchemaStructure(schema, 3)){
        return false;
    }

    if(!testForObjects(schema, false, true, false) ) {
        return false;
    }

    if(! testForLists(schema, "nonamespace_included.xsd" ) ) {
        return false;
    }

    delete schema;
    return true ;
}


/**
 * @brief TestXSDLoad::testImportAndInclude
 * a include b.i_a.xsd
 *
 * a import c.p_a.xsd
 *        c include d.p_a_i.xsd
 *                  d import e.p_a_i_p.xsd
 *                           e import f.p_a_i_p_i.xsd
 * @return
 */

bool TestXSDLoad::testImportAndInclude()
{
    _testName = "testImportAndInclude" ;
    XSDSchema* schema = testLoadTypes(FILE_TEST_IMPORT_AND_INCLUDE);
    if( NULL == schema ) {
        return false ;
    }

    if(!findType(schema, "xsd:string")) {
        return false;
    }

    if(!findElement(schema, "x:elementoa")) {
        return false;
    }
    if(!findElement(schema, "x:elementoc", true)) {
        return false;
    }
    if(!findElement(schema, "x:elementod", true)) {
        return false;
    }
    if(!findElement(schema, "x:elementoe", true)) {
        return false;
    }

    /*if(!findElement(schema, "i:elementoc", true)) {
        return false;
    }*/

    if(!checkSchemaStructure(schema, 2)){
        return false;
    }

    if(! testForListsImported(schema, "b.i_a.xsd") ) {
        return false;
    }

    delete schema;
    return true ;
}


bool TestXSDLoad::testRedefineTypes()
{
    _testName = "testRedefineTypes" ;
    XSDSchema* schema = testLoadTypes(FILE_TEST_TYPES_NAMESPACE_REDEFINING);
    if( NULL == schema ) {
        return false ;
    }

    if(!testForObjects(schema, true, true, false) ) {
        return false;
    }
    // check for the parent
    if( !findType(schema, "x:type2", false, schema ) ) {
        return false;
    }
    if(! testForListsRedefined(schema, "namespace_included.xsd") ) {
        return false;
    }
    if(!checkSchemaStructure(schema, 3)){
        return false;
    }

    delete schema;
    return true ;
}
//--------------------------------- endsection(TYPES) -------

bool TestXSDLoad::testFast()
{
    //return testRedefineTypes();
    return testImportAndInclude();
}


/**********************************************************************

tests:

NO NS:   x    Include

NS, nodefault:   a:x    Include

NS, default:   a:x/x    Include

import no ns con z  z:x
import target z z:x
import target con def ns:  z:x


**********************************************************************/
