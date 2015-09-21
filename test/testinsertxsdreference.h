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


#ifndef TESTINSERTXSDREFERENCE_H
#define TESTINSERTXSDREFERENCE_H

#include "testbase.h"
class NamespaceReferenceEntry;

class TestInsertXsdReference : public TestBase
{
    bool cfr(Regola *regola, const QString &step, const QString &fileResult);
    bool testSkeleton(const QString &fileStart, const QString &fileResult, NamespaceReferenceEntry *command );
    bool testSkeletonEntry(const QString &fileStart, NamespaceReferenceEntry *reference );
    bool testSkeletonEntryDialog(NamespaceReferenceEntry *reference);
    //
    bool test1NoData();
    bool test1_1NoData();
    bool test1_2NoData();
    bool test2();
    bool test3();
    bool test4();
    bool test4_1();
    bool test4_2();
    bool test4_3();
    bool test7();
    bool test8();
    bool test9();
    //--
    bool testFillDataNegFromRegola();
    bool testFillDataFromRegola();
    bool testDialogFromEmpty();
    bool testDialogFromData();

public:
    TestInsertXsdReference();
    ~TestInsertXsdReference();

    bool testFast();
    bool testInsertXsdReference();
    bool testDialogInsert();
};

#endif // TESTINSERTXSDREFERENCE_H
