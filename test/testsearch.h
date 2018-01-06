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


#ifndef TESTSEARCH_H
#define TESTSEARCH_H

#include "testbase.h"

class TestSearchHelper ;

class TestSearch : public TestBase
{
    bool literalSearchNoItemFound();
    bool literalSearchNonExact();
    bool literalSearchExact();
    bool literalSearchExactInChildren();
    bool literalSearchExactInChildrenNoMatch();
    bool literalSearchNonExactInChildren();
    bool literalSearchCountInChildren();
    bool literalSearchInAttributes();
    bool literalSearchInElementTags();
    bool literalSearchInAttributeNames();
    bool literalSearchInScopedPathAttribute();
    bool literalSearchInScopedPath();
    bool literalSearchInText();
    bool literalSearchInComment();
    bool literalSearchInTextBase64();

    //-----------------------------------------------------------
    bool xquerySearchPathExact();
    bool xquerySearchAttributesExact();
    bool xquerySearchAttributesNotExact();
    bool xquerySearchAttributeNameExact();
    bool xquerySearchAttributeNameNotExact();
    bool xquerySearchElementsExact();
    bool xquerySearchElementsNotExact();
    bool xquerySearchTextNotExact();
    bool xquerySearchText();
    bool xquerySearchTextOnlyCount();
    bool xquerySearchNamespaces();
    //--
    bool xquerySearchNamespacesTemplate(const QString &testName, const QString &fileName, const QString &searchPattern, const QString &expected);
    bool testxquerySearchNamespacesNoNS();
    bool testxquerySearchDefaultNamespace();
    bool testxquerySearchOneNamespace();
    bool testxquerySearchDefaultAndOtherNamespace();
    bool testxquerySearchMultiplesNamespace();
    bool testxquerySearchDefaultAndMultiplesNamespace();
    //--
    bool xquerySearchNamespacesTemplateString(const QString &testName, const QString &fileName, const QString &searchPattern, const QString &expectedValue);
    bool xquerySearchNamespacesString();
    bool testxquerySearchNamespacesNoNSString();
    bool testxquerySearchDefaultNamespaceString();
    bool testxquerySearchOneNamespaceString();
    bool testxquerySearchDefaultAndOtherNamespaceString();
    bool testxquerySearchMultiplesNamespaceString();
    bool testxquerySearchDefaultAndMultiplesNamespaceString();
    //--
    bool xquerySearchAttributesExactInChildren();
    bool xquerySearchAttributesNotExactInChildren();
    bool xquerySearchElementsExactInChildren();
    bool xquerySearchElementsNotExactInChildren();
    bool xquerySearchTextNotExactInChildren();
    bool xquerySearchTextInChildren();

    //-----------------------------------------------------------
    bool literalSearchNext();
    bool literalSearchPrevious();
    bool innerSearchForNext(const QString &testName, const bool isNext);
    bool innerSearchForRoot(const QString &testName, const bool isNext);
    bool innerSearchForInner(const QString &testName, const bool isNext);
    bool innerSearchForNextComplex(const QString &testName, const bool isNext);
    bool innerSearchForNextComplexReverse(const QString &testName);
    bool innerSearchOne(const QString &testName, const bool isNext, QList<int> firstSelPath, QList<QList<int> > selList, const bool isWrap );
    bool innerSearchForInnerReverse(const QString &testName);
    bool innerSearchComplex(const QString &testName, const bool isNext, QList<int> firstSelPath, QList<QList<int> > selList, const bool isWrap );
    //-----------------------------------------------------------

    bool testAStdSearch(const QString &nameTest, const int expected, const QString &textToSearch);
    void init(const QString &nameTest, TestSearchHelper &helper);
    bool testAStdSearchWithParamsInit(const QString &nameTest, TestSearchHelper &helper);
    void initWithParams(const QString &nameTest, TestSearchHelper &helper, QList<int> &selPath);
    bool checkResults(TestSearchHelper &helper, const int expectedBookmarks, const int expectedCount);
    bool checkResults(TestSearchHelper &helper, const int expectedBookmarks, const int expectedCount, QStringList *expectedElementTags);
    bool checkTestSearch(TestSearchHelper &helper, const QString &textToSearch, const int expectedBookmarks, const int expectedCount);
    bool checkTestSearch( TestSearchHelper &helper, const QString &textToSearch, const int expected);
    bool testAStdSearch(TestSearchHelper &helper, const QString &nameTest, const int expected, const QString &textToSearch);
    bool checkResultsByPath(TestSearchHelper &helper, const int expectedBookmarks, const int expectedCount, QList<QList<int> > &expectedPathList);
    bool checkSelectionByPath(TestSearchHelper &helper, const int expectedBookmarks, QList<int> &expectedPath, const int index = 0);

public:
    TestSearch();
    ~TestSearch();

    bool testFast();
    bool testXQuerySearch();
    bool testLiteralSearch();
};

#endif // TESTSEARCH_H
