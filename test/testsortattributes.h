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


#ifndef TESTSORTATTRIBUTES_H
#define TESTSORTATTRIBUTES_H

#include "testbase.h"

class TestSortAttributes : public TestBase
{
    bool saveDefaultWithSorted();
    bool saveDefaultWithUnsorted();
    bool noSortAndRegolaSort();
    bool noSortAndRegolaUnSort();

    bool baseSetup(const QString &fileIn, const QString &fileCompare,
                   const bool setGlobal, const bool setSortGlobal,
                   const bool setLocal, const bool setSortLocal);
    //----
    bool testUnitOperation();
    bool testSortUnsorted();
    bool testSortSorted();
    bool testSortUndo();
    bool verifyAttributesList(Element *el, QStringList values);
    bool addAttributeVerify(Element *el, const QString &aName, const QString &aValue);
    bool fillDefaultAttributesUnsorted(Element *el);
    bool fillDefaultAttributesSorted(Element *el);
    bool checkAttributesOrder(Element *el, QStringList attr);
    bool checkAttributesOrder(Regola *regola, QStringList attr);

public:
    TestSortAttributes();
    ~TestSortAttributes();

    bool testFast();
    bool testSaving();
    bool testUnitSort();
    bool testSortInEditor();
};

#endif // TESTSORTATTRIBUTES_H
