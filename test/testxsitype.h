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


#ifndef TESTXSITYPE_H
#define TESTXSITYPE_H

#include "testbase.h"

class App;

class TestXSIType : public TestBase
{

    bool testInsert();
    bool testRemove();

    bool cfr(Regola *regola, const QString &step, const QString &fileResult);
    bool testSkeleton(const QString &fileStart, const QString &fileResult,
                                             bool (TestXSIType::*functPtr)(App *appData) );

    bool doRemove(App *appData);
    bool doInsert(App *appData);

    bool testRemove1();
    bool testRemove2();
    bool testRemove3();
    bool testRemove4();
    bool testRemove5();
    bool testRemove6();
    bool testRemove7();
    bool testRemove8();
    bool testRemove9();
    bool testRemove10();
    bool testRemove11();
    bool testRemove12();

    bool testInsert1();
    bool testInsert2();
    bool testInsert3();
    bool testInsert4();
    bool testInsert5();
    bool testInsert6();
    bool testInsert7();
    bool testInsert8();
    bool testInsert9();
    bool testInsert10();

public:
    TestXSIType();
    ~TestXSIType();

    bool testUnit();
    bool testFast();
};

#endif // TESTXSITYPE_H
