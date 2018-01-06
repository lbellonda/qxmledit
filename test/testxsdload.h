/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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


#ifndef TESTXSDLOAD_H
#define TESTXSDLOAD_H

#include "testbase.h"
#include "app.h"
#include <xsdeditor/io/xschemaloader.h>

class XSchemaLoader;

class TestHelperXLists {
public:
    TestHelperXLists();
    ~TestHelperXLists();
    QStringList types, elements, attributes, attributeGroups, groups ;
    QList< QPair<QString,ESchemaType> > references;
};


class TestXSDLoad : public QObject, public TestBase
{
    Q_OBJECT
    bool testLoadSimpleSync();
    bool testLoadSimpleNetworkSync();
    bool testLoadSimpleNetworkFileSync();
    bool testLoadSimpleASync();
    bool testLoadSimpleNetworkASync();
    bool testLoadSimpleNetworkFileASync();
    bool checkMode(XSchemaLoader *loader, const bool expected);
    XSDSchema* TesttestLoadTypes(const QString &fileIn);
    bool errorType(XSDSchema* schema, const QString &type);
    bool _receivedOk;
    bool _receivedKO;
    bool _go;
    bool testIncludeSimpleSync();
    bool testIncludeSimpleAsync();
    bool testRedefineSimpleSync();
    bool testRedefineSimpleAsync();
    bool testIncludeSimpleDoubleSync();
    //--------------------------------------------
    bool testTypesWithoutNamespace();
    bool testTypesWithoutNamespaceInclude();
    bool testIncludeSimpleNamespaces();
    bool testImportAndInclude();
    bool testIncludeSimpleNamespacesNonDefault();
    bool testIncludeSimpleNamespacesDefault();
    bool testRedefineTypes();
    //--------------------------------------------
    XSDSchema* testLoadTypes(const QString &fileIn);
    bool findElement(XSDSchema* schema, const QString &type, const bool reverse = false);
    bool findAttribute(XSDSchema* schema, const QString &type, const bool reverse = false);
    bool findAttributeGroup(XSDSchema* schema, const QString &name, const bool reverse = false);
    bool findGroup(XSDSchema* schema, const QString &name, const bool reverse = false);
    bool findObject(XSDSchema* schema, const QString &name, const ESchemaType typeSpec, const bool reverse = false);
    bool findType(XSDSchema* schema, const QString &name, const bool reverse = false, XSDSchema *expectedSchema = NULL );
    bool checkOwner(XSchemaObject* target, XSDSchema* schema, const QString &name, const bool reverse, XSDSchema *expectedSchema );
    //--------------------------------------------
    QString fakeName(const QString &name);
    bool errorObjectAndDeleteSchema(XSDSchema* schema, const QString &name, const ESchemaType typeSpec, const bool reverse );
    bool errorSpecAndDeleteSchema(XSDSchema* schema, const QString &name, const ESchemaType typeSpec, const bool reverse );
    bool errorSpecAndDeleteSchemaFalse(XSDSchema* schema, const QString &name, const ESchemaType typeSpec );
    bool errorRootAndDeleteSchema(XSDSchema* schema, const QString &name, const ESchemaType typeSpec );
    bool errorObjectAndDeleteSchemaFalse(XSDSchema* schema, const QString &name, const ESchemaType typeSpec );
    bool errorNameAndDeleteSchema(XSDSchema* schema, const QString &realName, const QString &name, const ESchemaType typeSpec, const bool reverse );
    bool checkSchemaStructure(XSDSchema* schema, const int includesExpected );
    bool errorMessageAndDeleteSchema(XSDSchema* schema, const QString &name, const QString &msg ) ;
    //--------------------------------------------
    bool testLists(XSDSchema* schema, const bool onlyThisSchemaOrAll, TestHelperXLists &helper );
    bool testExpectedAttributes( XSDSchema* schema, QStringList expectedSingleSchema, const bool onlyThisSchemaOrAll);
    bool testExpectedElements(XSDSchema* schema, QStringList expectedSingleSchema, const bool onlyThisSchemaOrAll);
    bool testExpectedTypes( XSDSchema* schema, QStringList expectedSingleSchema, const bool onlyThisSchemaOrAll);
    bool testExpectedAttributeGroups( XSDSchema* schema, QStringList expectedSingleSchema, const bool onlyThisSchemaOrAll);
    bool testExpectedGroups( XSDSchema* schema, QStringList expectedSingleSchema, const bool onlyThisSchemaOrAll);
    bool testExpectedReferences( XSDSchema* schema, QList<QPair<QString,ESchemaType> > &reference);
    //--------------------------------------------
    bool cmpNames(const QString &name, XSchemaObject *object );
    //--------------------------------------------
    bool testForLists(XSDSchema* schema , const QString &included);
    bool testForListsRedefined(XSDSchema* schema, const QString &included );
    bool testForListsImported(XSDSchema* schema, const QString &included );
    bool testForObjects(XSDSchema* schema, const bool isSchemaQualified, const bool isObjectQualified, const bool isObjectUnqualified );
    //--------------------------------------------


public:
    TestXSDLoad();
    ~TestXSDLoad();

    bool testUnits();
    bool testInclude();
    bool testRedefine();
    bool testTypes();
    bool testFast();

private slots:
    void onFinished( XSchemaLoader *entity, const XSchemaLoader::Code code );
};

#endif // TESTXSDLOAD_H
