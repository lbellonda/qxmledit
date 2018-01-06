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


#ifndef TESTXSDCOPY_H
#define TESTXSDCOPY_H

#include "testbase.h"
#include <QDomDocument>
class XSchemaElement ;
class XSchemaObject;
class TestXSDWindow;
class Regola;
class Element;
class MainWindow;

class TestXSDCopy : public TestBase
{
    TestXSDWindow * xsdEditor;


    void deleteEditor();
    bool testCopyNoattrs();
    bool openXsdViewer(MainWindow *wnd);
    QString applyResults(XSchemaObject *object);
    bool checkResults(const QString &result, QDomDocument domDocument, const QString &attrName );
    Element *findCompareElement(Regola *regola, const QString &value);
    Element *findCompareElement(Element *element, const QString &value);
    bool execCopyDefTests(Regola *reference);
    bool execCopyDefTestsFacets(Regola *reference);
    bool hasTestAttr(XSchemaObject *object, const QString &attrName, const QString &attrValue);
    XSchemaElement *getTestElement( XSchemaObject *parentObject, const QString &attrName, const QString &attrValue);
    XSchemaObject *getTestObject( XSchemaObject *parentObject, const QString &attrName, const QString &attrValue);
    bool singleTest( const int id, const QString &newTestName, const QString &attrName, const QString &testTag, const bool isInnerTest, Regola *reference);
    bool singleTestFacets( const int id, const QString &newTestName, const QString &attrName, const QString &testTag, Regola *reference);
    bool testCopyFacetsInt();

public:
    TestXSDCopy();
    ~TestXSDCopy();

    bool testCopy();
    bool testCopyFacets();
};

#endif // TESTXSDCOPY_H
