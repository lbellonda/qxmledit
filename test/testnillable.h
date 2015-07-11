/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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


#ifndef TESTNILLABLE_H
#define TESTNILLABLE_H

#include "testbase.h"

class App;

class TestNillable : public TestBase
{

    bool testInsert();
    bool testRemove();

    bool cfr(Regola *regola, const QString &step, const QString &fileResult);
    bool testSkeleton(const QString &fileStart, const QString &fileResult,
                                             bool (TestNillable::*functPtr)(App *appData) );

    bool doRemove(App *appData);
    bool doInsert(App *appData);

    bool testRemove1();
    bool testRemove2();
    bool testRemove3();
    bool testRemove4();
    bool testRemove5();
public:
    TestNillable();
    ~TestNillable();

    bool testUnit();
    bool testFast();
};

#endif // TESTNILLABLE_H
