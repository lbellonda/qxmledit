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


#ifndef TESTREPLACE_H
#define TESTREPLACE_H

#include "testbase.h"

class App;
class TestReplaceHelper ;

class TestReplace : public TestBase
{
    bool testSkeleton(const QString &fileStart, const QString &fileResult,
                                             bool (TestReplace::*functPtr)(App *appData, TestReplaceHelper &helper),
                                             TestReplaceHelper &helper );
    bool initHelper(const QString &name, TestReplaceHelper &helper);
    bool cfr(Regola *regola, const QString &step, const QString &fileResult);

    bool funOneStep(App *appData, TestReplaceHelper &helper);
    bool replaceAString(const QString &source, const QString &search, const QString &replacement, const QString &expected, const bool usePartialMatch, const bool isCaseSensitive);
    bool replaceAStringPartialCase(const QString &source, const QString &search, const QString &replacement, const QString &expected);
    bool replaceAStringPartialNoCase(const QString &source, const QString &search, const QString &replacement, const QString &expected);
    bool testUnitReplaceString();

    //---
    bool testReplaceAll();
    bool testReplaceNext();
    bool testReplacePrevious();
    bool testReplaceAllElementsExact();
    bool testReplaceAllElementsExactError();
    bool testReplaceAllElementsExactCase();
    bool testReplaceAllElementsPartial();
    bool testReplaceAllElementsPartialError();
    bool testReplaceAllElementsPartialCase();
    bool testReplaceChildrenElements();
    bool testReplaceChildrenElementsPartialError();
    bool testReplaceChildrenElementsPartialCase();
    //---
    bool testReplaceAttributesPartial();
    bool testReplaceAttributesPartialError();
    bool testReplaceAttributesPartialCase();
    bool testReplaceAttributesValuesPartial();
    bool testReplaceAttributesValuesPartialCase();
    //---
    bool testReplaceElements();
    bool testReplaceAttributes();
    bool testReplaceText();
    bool testReplaceComments();
    bool testReplaceProcessingInstructions();
    //---
    bool testReplaceCommentsOk();
    bool testReplaceCommentsError();
    //---
    bool testReplaceTextElement();
    bool testReplaceTextElementInline();
    bool testReplaceTextElementCData();
    //---
    bool testSkeletonPrevious(const QString &fileStart, const QStringList fileResult,
                                             TestReplaceHelper &helper, const bool isPrevious );
    //---
    bool initHelper(const QString &name, TestReplaceHelper &helper, const int expectedErrorsCount, const int expectedReplacementCount);
public:
    TestReplace();
    ~TestReplace();

    bool testBaseReplace();
    bool testUnitReplace();
    bool testFast();
};

#endif // TESTREPLACE_H
