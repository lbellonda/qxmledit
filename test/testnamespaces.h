/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016-2018 by Luca Bellonda and individual contributors  *
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


#ifndef TESTNAMESPACES_H
#define TESTNAMESPACES_H

#include "testbase.h"
#include "app.h"

class TestNamespaces : public TestBase
{

    bool testReplaceNamespace();
    bool testSetNamespace();
    bool testRemoveNamespace() ;
    bool testReplacePrefix();
    bool testSetPrefix();
    bool testRemovePrefix() ;
    bool testNormalizeNamespace();
    //------
    bool doRemovePrefixAllElemAllPrefixes(MainWindow * window, Element *selection);
    bool doRemovePrefixAllElemOnePrefix(MainWindow * window, Element * selection);
    bool doRemovePrefixSelNoRecElemOnePrefix(MainWindow * window, Element * selection);
    bool doRemovePrefixSelRecElemOnePrefix(MainWindow * window, Element * selection);
    bool doRemovePrefixSelNoRecElemAllPrefixes(MainWindow * window, Element * selection);
    bool doRemovePrefixSelRecElemAllPrefixes(MainWindow * window, Element * selection);
    bool doRemoveNamespaceAllElemAllNamespaces(MainWindow * window, Element *selection);
    friend class TestNamespaceInternal;
    friend class TestPrefixInternal;
    //---
    bool testChildOf();
    bool testBookmarks();
    bool testUnitNamespaces();
    bool testUnitSetNamespace();
    bool testUnitReplaceNamespace();
    bool testStandardNamespace(const QString &aCase, bool (*apply)(TestBase *arg, MainWindow *, Element *));
    bool testUnitChangePrefix();
    bool testDetectPrefixes();
    bool testForDetectPrefixes( const QString &id, const QString &file, const bool expected );
    bool testUnitNamespaceClash();

public:
    TestNamespaces();
    ~TestNamespaces();

    bool testFast();
    bool testUnit();
    bool testPrefixes();
    bool testNamespaces();

};

#endif // TESTNAMESPACES_H
