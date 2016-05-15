/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016 by Luca Bellonda and individual contributors       *
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


#ifndef TESTEDITENUM_H
#define TESTEDITENUM_H

#include "testbase.h"

class XSDFacet;
class MainWindow;

class TestEditEnum : public TestBase
{
    bool testGetSameEditData();
    bool compareFacetsLists(QList<XSDFacet*> reference, QList<XSDFacet*> compare);
    //---
    bool testEditEnum();
    //-----
    bool testAdd();
    bool testModify();
    bool testDelete();
    bool testMoveUp();
    bool testMoveDown();
    //---
    void emptyList(QList<XSDFacet*> reference);
    //--
    bool testInsertSimple();
    bool testInsertComplex();
    bool testInsertComplexMultiple();
    //--
    bool testUpdateSimple();
    bool testUpdateComplex();
    bool testDeleteSimple();
    //--
    bool testIdentity();
    bool applyTestInsertSimple(MainWindow *, Element *);
    bool applyTestInsertComplex(MainWindow * window, Element *selection);
    bool applyTestInsertComplexMultiple(MainWindow * window, Element *selection);
    bool applyIdentity(MainWindow * window, Element *selection);
    bool applyTestUpdateSimple(MainWindow * window, Element *selection);
    bool applyTestUpdateComplex(MainWindow * window, Element *selection);
    bool applyTestDeleteSimple(MainWindow * window, Element *selection);
    bool testSkeleton( const QString &id, const QString &fileStart, const QString &fileFinal, bool (TestEditEnum::*apply)(MainWindow *, Element *) );

public:
    TestEditEnum();
    ~TestEditEnum();

    bool testFast();
    bool testUnit();
    bool testEdit();
};

#endif // TESTEDITENUM_H
