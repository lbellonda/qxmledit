/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2018 by Luca Bellonda and individual contributors  *
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

#ifndef TESTXSDDIFF_H
#define TESTXSDDIFF_H

#include "app.h"
#include "xsdeditor/xsdgraphicsconfiguration.h"

class TestXSDDiff
{
    enum ETTType {
        ORDERED,
        UNORDERED,
        ATTRIBUTES
    };

    QMap<QString, QVariant> _configBackend;

    bool one;

    bool testForChildren( const QString &testName, QList<XSchemaObject*> &children, QList<XSDCompareState::EXSDCompareState>expected);
    QString _errorString;
    bool _isError ;

    bool testEquals();
    bool testEqualsUnordered();
    bool testAddBefore();
    bool testAddMiddle();
    bool testAddAfter();
    bool testDelBefore();
    bool testDelMiddle();
    bool testDelAfter();
    bool testAddBeforeUnordered();
    bool testAddMiddleUnordered();
    bool testAddAfterUnordered();
    bool testDelBeforeUnordered();
    bool testDelMiddleUnordered();
    bool testDelAfterUnordered();

    bool testInnerMixed();
    bool testInnerMixedUnordered();
    bool testOpAttributes();
    bool testBaseSettings();
    bool compareBkConfig(XSDGraphicsBackgroundConfiguration &one, XSDGraphicsBackgroundConfiguration &two);
    bool compareColors(const QColor &one, const QColor &two );
    bool errorCompare();

    void putConfigToSleep();
    QColor updateColor( const QColor & inputColor );


    bool error(const QString &testName, const QString &msg);

    bool verifyCollectionOrdered( const QString &testName, XSDSchema *schema, QList<int> &expected);
    bool verifyCollectionUnordered( const QString &testName, XSDSchema *schema, QList<int> &expected);
    bool verifyAttributesUnordered( const QString &testName, XSDSchema *schema, QList<int> &expected);

    void collectTestObject( QList<XSchemaObject*> &outList, QList<XSchemaObject*> &objList, const XSDCompareState::EXSDCompareState stateToLook);
    bool compareResults( const QString &testName, QList<XSchemaObject*> &children, int countExpected, const QString &attrTest);
    bool doCompare(const QString &testName, const QString &fileNameIn, const QString &fileNameOut, const int expAdd, const int expMod, const int expDel, const int expUn);

    bool testDiffx1();

    bool hasTestAttr( XSchemaObject *object);
    XSchemaObject *getTestObject( QList<XSchemaObject*> &list);

    bool doTest( const ETTType type, const QString &testName, const QString &fileNameIn, const QString &fileNameOut, QList<int> &expected, const bool areDifferent );
    bool goOrdered( const QString &testName, const QString &fileNameIn, const QString &fileNameOut, QList<int> &expected, const bool areDifferent );
    bool goAttributes( const QString &testName, const QString &fileNameIn, const QString &fileNameOut, QList<int> &expected, const bool areDifferent );
    bool goUnordered( const QString &testName, const QString &fileNameIn, const QString &fileNameOut, QList<int> &expected, const bool areDifferent );
    bool testOrderedForChildren( const QString &testName, QList<XSchemaObject*> &children, QList<int>&expected);
    bool testUnorderedForChildren( const QString &testName, QList<XSchemaObject*> &children, QHash<XSDCompareState::EXSDCompareState,int>&expected);

    XSDCompareObject::EXSDCompareObject testBaseDifferent();
    XSDCompareObject::EXSDCompareObject testBaseEquals();

    bool testDiffElementsAndTypes();
    bool testDiffChoice();
    bool testDiffSequence();
    bool testDiffAttributes();
    bool testObjects();

public:
    TestXSDDiff();
    ~TestXSDDiff();
    QString errorString();

    bool testAdd();
    bool testEqual();
    bool testDel();
    bool testMixed();
    bool testCompareBase();
    bool testAddUnordered();
    bool testDelUnordered();
    bool testEqualUnordered();
    bool testMixedUnordered();
    bool testDiffObjects();
    bool testDiffOpAttributes();
};

#endif // TESTXSDDIFF_H
