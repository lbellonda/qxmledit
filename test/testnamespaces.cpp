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

#include "testnamespaces.h"

#define BASE_PATH "../test/data/namespace/edit/"

TestNamespaces::TestNamespaces()
{
}

TestNamespaces::~TestNamespaces()
{
}

bool TestNamespaces::testFast()
{
    return testNormalizeNamespace();
}

bool TestNamespaces::testNamespaces()
{
    _testName = "testNamespaces";
    if(!testRemoveNamespace()) {
        return false;
    }
    if(!testSetNamespace()) {
        return false;
    }
    if(!testReplaceNamespace()) {
        return false;
    }
    if(!testNormalizeNamespace()) {
        return false;
    }
    return true ;
}

bool TestNamespaces::testUnit()
{
    _testName = "testUnit";
    if(!testChildOf()) {
        return false;
    }
    if(!testDetectPrefixes()){
        return false;
    }
    if(!testBookmarks()) {
        return false;
    }
    if(!testUnitNamespaces()) {
        return false;
    }
    return true ;
}

#define FILE_CHILD_0_START   BASE_PATH "prefix/prefixstartwith.xml"

bool TestNamespaces::testChildOf()
{
    _testName = "testChildOf";
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(FILE_CHILD_0_START) ) {
        return error(QString("unable to load input file: '%1' ").arg(FILE_CHILD_0_START));
    }
    Regola *regola = app.mainWindow()->getRegola();
    {
        QList<int> first, second;
        first << 1 << 0;
        second << 1 << 1;
        Element *firstElement = regola->findElementByArray(first);
        Element *secondElement = regola->findElementByArray(second);
        if( (NULL == firstElement ) || (NULL == secondElement ) ) {
            return error("1 elements not found");
        }
        if(firstElement->isChildOf(secondElement)) {
            return error("1 first/second");
        }
        if(secondElement->isChildOf(firstElement)) {
            return error("1 second/first");
        }
    }
    {
        QList<int> first, second;
        first << 1 << 0;
        second << 1 << 0 << 1 ;
        Element *firstElement = regola->findElementByArray(first);
        Element *secondElement = regola->findElementByArray(second);
        if( (NULL == firstElement ) || (NULL == secondElement ) ) {
            return error("2 elements not found");
        }
        if(firstElement->isChildOf(secondElement)) {
            return error("2 first/second");
        }
        if(!secondElement->isChildOf(firstElement)) {
            return error("2 second/first");
        }
    }

    {
        QList<int> first, second;
        first << 1 ;
        second << 1 << 0 << 0 ;
        Element *firstElement = regola->findElementByArray(first);
        Element *secondElement = regola->findElementByArray(second);
        if( (NULL == firstElement ) || (NULL == secondElement ) ) {
            return error("3 elements not found");
        }
        if(firstElement->isChildOf(secondElement)) {
            return error("3 first/second");
        }
        if(!secondElement->isChildOf(firstElement)) {
            return error("3 second/first");
        }
    }
    return true;
}

bool TestNamespaces::testBookmarks()
{
    _testName = "testBookmarks";
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(FILE_CHILD_0_START) ) {
        return error(QString("unable to load input file: '%1' ").arg(FILE_CHILD_0_START));
    }
    Regola *regola = app.mainWindow()->getRegola();
    {
        regola->clearBookmarks();
        QList<int> first, second;
        first << 1 << 0;
        second << 1 << 1;
        Element *firstElement = regola->findElementByArray(first);
        Element *secondElement = regola->findElementByArray(second);
        if( (NULL == firstElement ) || (NULL == secondElement ) ) {
            return error("1 elements not found");
        }
        regola->addBookmark(firstElement);
        regola->addBookmark(secondElement);
        QList<Element*> r = regola->getUniqueBookmarksElements(TargetSelection::Bookmarks);
        if(r.size() != 2 ) {
            return error(QString("1: expected 2, found: %1").arg(r.size()));
        }
        r = regola->getUniqueBookmarksElements(TargetSelection::BookmarksAndChildren);
        if(r.size() != 2 ) {
            return error(QString("1: expected 2, found: %1").arg(r.size()));
        }
    }
    {
        regola->clearBookmarks();
        QList<int> first, second;
        first << 1 << 0;
        second << 1 << 0 << 1 ;
        Element *firstElement = regola->findElementByArray(first);
        Element *secondElement = regola->findElementByArray(second);
        if( (NULL == firstElement ) || (NULL == secondElement ) ) {
            return error("2 elements not found");
        }
        regola->addBookmark(firstElement);
        regola->addBookmark(secondElement);
        QList<Element*> r = regola->getUniqueBookmarksElements(TargetSelection::Bookmarks);
        if(r.size() != 2 ) {
            return error(QString("2: expected 2, found: %1").arg(r.size()));
        }
        r = regola->getUniqueBookmarksElements(TargetSelection::BookmarksAndChildren);
        if(r.size() != 1 ) {
            return error(QString("2: expected 1, found: %1").arg(r.size()));
        }
    }

    {
        regola->clearBookmarks();
        QList<int> first, second;
        first << 1 ;
        second << 1 << 0 << 0 ;
        Element *firstElement = regola->findElementByArray(first);
        Element *secondElement = regola->findElementByArray(second);
        if( (NULL == firstElement ) || (NULL == secondElement ) ) {
            return error("3 elements not found");
        }
        regola->addBookmark(firstElement);
        regola->addBookmark(secondElement);
        QList<Element*> r = regola->getUniqueBookmarksElements(TargetSelection::Bookmarks);
        if(r.size() != 2 ) {
            return error(QString("3: expected 2, found: %1").arg(r.size()));
        }
        r = regola->getUniqueBookmarksElements(TargetSelection::BookmarksAndChildren);
        if(r.size() != 1 ) {
            return error(QString("3: expected 1, found: %1").arg(r.size()));
        }
    }
    return true;
}

class TestPrefixInternal {
    public:

    static bool idoRemovePrefixAllElemOnePrefix(TestBase *aptr, MainWindow * window, Element *selection)
    {
        return static_cast<TestNamespaces*>(aptr)->doRemovePrefixAllElemOnePrefix(window, selection);
    }

    static bool idoRemovePrefixSelNoRecElemOnePrefix(TestBase *aptr, MainWindow * window, Element *selection)
    {
        return static_cast<TestNamespaces*>(aptr)->doRemovePrefixSelNoRecElemOnePrefix(window, selection);
    }

    static bool idoRemovePrefixSelNoRecElemAllPrefixes(TestBase *aptr, MainWindow * window, Element *selection)
    {
        return static_cast<TestNamespaces*>(aptr)->doRemovePrefixSelNoRecElemAllPrefixes(window, selection);
    }

    static bool idoRemovePrefixSelRecElemOnePrefix(TestBase *aptr, MainWindow * window, Element *selection)
    {
        return static_cast<TestNamespaces*>(aptr)->doRemovePrefixSelRecElemOnePrefix(window, selection);
    }

    static bool idoRemovePrefixSelRecElemAllPrefixes(TestBase *aptr, MainWindow * window, Element *selection)
    {
        return static_cast<TestNamespaces*>(aptr)->doRemovePrefixSelRecElemAllPrefixes(window, selection);
    }

    static bool idoRemovePrefixAllElemAllPrefixes(TestBase *aptr, MainWindow * window, Element *selection)
    {
        return static_cast<TestNamespaces*>(aptr)->doRemovePrefixAllElemAllPrefixes(window, selection);
    }
    //--

    static bool idoRemovePrefixBkmRecElemOnePrefix(TestBase *, MainWindow * window, Element *)
    {
        window->getEditor()->prefixRemove("a", NULL, TargetSelection::BookmarksAndChildren, false);
        return window->errorCount()==0;
    }

    static bool idoRemovePrefixBkmNoRecElemOnePrefix(TestBase *, MainWindow * window, Element *)
    {
        window->getEditor()->prefixRemove("a", NULL, TargetSelection::Bookmarks, false);
        return window->errorCount()==0;
    }

    //---
    //tutti, sel, selrec, book, book rec:
    static bool idoSetPrefixAll(TestBase *, MainWindow * window, Element *)
    {
        window->getEditor()->prefixSet("c", NULL, TargetSelection::AllItems);
        return window->errorCount()==0;
    }
    //
    static bool idoSetPrefixSelNoRec(TestBase *, MainWindow * window, Element * element)
    {
        window->getEditor()->prefixSet("c", element, TargetSelection::SelectedItem);
        return window->errorCount()==0;
    }
    static bool idoSetPrefixSelRec(TestBase *, MainWindow * window, Element *element)
    {
        window->getEditor()->prefixSet("c", element, TargetSelection::SelectedItemAndChildren);
        return window->errorCount()==0;
    }
    static bool idoSetPrefixBkmNoRec(TestBase *, MainWindow * window, Element *element)
    {
        window->getEditor()->prefixSet("c", element, TargetSelection::Bookmarks);
        return window->errorCount()==0;
    }
    static bool idoSetPrefixBkmRec(TestBase *, MainWindow * window, Element *element)
    {
        window->getEditor()->prefixSet("c", element, TargetSelection::BookmarksAndChildren);
        return window->errorCount()==0;
    }
    //--
    static const QString OldPrefix;
    static const QString NewPrefix;
    static bool idoReplacePrefixAllElemAllPrefixes(TestBase *, MainWindow * window, Element *element)
    {
        window->getEditor()->prefixReplace(OldPrefix, NewPrefix, element, TargetSelection::AllItems, true);
        return window->errorCount()==0;
    }
    //
    static bool idoReplacePrefixAllElemOnePrefix(TestBase *, MainWindow * window, Element *element)
    {
        window->getEditor()->prefixReplace(OldPrefix, NewPrefix, element, TargetSelection::AllItems, false);
        return window->errorCount()==0;
    }
    static bool idoReplacePrefixSelNoRecElemOnePrefix(TestBase *, MainWindow * window, Element *element)
    {
        window->getEditor()->prefixReplace(OldPrefix, NewPrefix, element, TargetSelection::SelectedItem, false);
        return window->errorCount()==0;
    }
    static bool idoReplacePrefixSelRecElemOnePrefix(TestBase *, MainWindow * window, Element *element)
    {
        window->getEditor()->prefixReplace(OldPrefix, NewPrefix, element, TargetSelection::SelectedItemAndChildren, false);
        return window->errorCount()==0;
    }
    static bool idoReplacePrefixSelNoRecElemAllPrefixes(TestBase *, MainWindow * window, Element *element)
    {
        window->getEditor()->prefixReplace(OldPrefix, NewPrefix, element, TargetSelection::SelectedItem, true);
        return window->errorCount()==0;
    }
    static bool idoReplacePrefixSelRecElemAllPrefixes(TestBase *, MainWindow * window, Element *element)
    {
        window->getEditor()->prefixReplace(OldPrefix, NewPrefix, element, TargetSelection::SelectedItemAndChildren, true);
        return window->errorCount()==0;
    }
    // Test for bookmark, only one case.
    static bool idoReplacePrefixBkmNoRecElemOnePrefix(TestBase *, MainWindow * window, Element *element)
    {
        window->getEditor()->prefixReplace(OldPrefix, NewPrefix,  element, TargetSelection::Bookmarks, false);
        return window->errorCount()==0;
    }
    static bool idoReplacePrefixBkmRecElemOnePrefix(TestBase *, MainWindow * window, Element *element)
    {
        window->getEditor()->prefixReplace(OldPrefix, NewPrefix, element, TargetSelection::BookmarksAndChildren, false);
        return window->errorCount()==0;
    }
};

const QString TestPrefixInternal::OldPrefix="a";
const QString TestPrefixInternal::NewPrefix ="newp";

#define FILE_PREFIX_0_START   BASE_PATH "prefix/prefixstartwith.xml"
#define FILE_PREFIX_REMOVE_ALL_ALL   BASE_PATH "prefix/prefixremoveallall.xml"
#define FILE_PREFIX_REMOVE_ALL_ONE BASE_PATH "prefix/prefixremoveallone.xml"
#define FILE_PREFIX_REMOVE_SELNOR_ONE BASE_PATH "prefix/prefixremoveselnorone.xml"
#define FILE_PREFIX_REMOVE_SELR_ONE BASE_PATH "prefix/prefixremoveselrone.xml"
#define FILE_PREFIX_REMOVE_SELNOR_ALL BASE_PATH "prefix/prefixremoveselnorall.xml"
#define FILE_PREFIX_REMOVE_SELR_ALL BASE_PATH "prefix/prefixremoveselrall.xml"
//--
#define FILE_PREFIX_SET_ALL BASE_PATH "prefix/setall.xml"
#define FILE_PREFIX_SET_SELNOR BASE_PATH "prefix/setselnor.xml"
#define FILE_PREFIX_SET_SELR BASE_PATH "prefix/setselr.xml"
//--
#define FILE_PREFIX_REPLACE_ALL_ALL BASE_PATH "prefix/repl_a_a.xml"
#define FILE_PREFIX_REPLACE_ALL_ONE BASE_PATH "prefix/repl_a_o.xml"
#define FILE_PREFIX_REPLACE_SELNOR_ONE BASE_PATH "prefix/repl_s_n.xml"
#define FILE_PREFIX_REPLACE_SELR_ONE BASE_PATH "prefix/repl_s_r.xml"
#define FILE_PREFIX_REPLACE_SELNOR_ALL BASE_PATH "prefix/repl_snr.xml"
#define FILE_PREFIX_REPLACE_SELR_ALL BASE_PATH "prefix/repl_sra.xml"


bool TestNamespaces::doRemovePrefixAllElemAllPrefixes(MainWindow * window, Element * selection)
{
    window->getEditor()->prefixRemove("a", selection, TargetSelection::AllItems, true);
    return window->errorCount()==0;
}

bool TestNamespaces::doRemovePrefixAllElemOnePrefix(MainWindow * window, Element * selection)
{
    window->getEditor()->prefixRemove("a", selection, TargetSelection::AllItems, false);
    return window->errorCount()==0;
}

bool TestNamespaces::doRemovePrefixSelNoRecElemOnePrefix(MainWindow * window, Element * selection)
{
    window->getEditor()->prefixRemove("a", selection, TargetSelection::SelectedItem, false);
    return window->errorCount()==0;
}

bool TestNamespaces::doRemovePrefixSelRecElemOnePrefix(MainWindow * window, Element * selection)
{
    window->getEditor()->prefixRemove("a", selection, TargetSelection::SelectedItemAndChildren, false);
    return window->errorCount()==0;
}

bool TestNamespaces::doRemovePrefixSelNoRecElemAllPrefixes(MainWindow * window, Element * selection)
{
    window->getEditor()->prefixRemove("", selection, TargetSelection::SelectedItem, true);
    return window->errorCount()==0;
}

bool TestNamespaces::doRemovePrefixSelRecElemAllPrefixes(MainWindow * window, Element * selection)
{
    window->getEditor()->prefixRemove("", selection, TargetSelection::SelectedItemAndChildren, true);
    return window->errorCount()==0;
}

bool TestNamespaces::testPrefixes()
{
    _testName = "testPrefixes";

    if(!testRemovePrefix()) {
        return false ;
    }
    if(!testSetPrefix()) {
        return false ;
    }
    if(!testReplacePrefix()) {
        return false ;
    }
    return true ;
}

bool TestNamespaces::testRemovePrefix()
{
    _testName = "testRemovePrefix";
    QList<int> selPath ;
    selPath << 1 << 1 ;

    // as is
    if(!testSkeletonBase( "0", FILE_PREFIX_0_START, FILE_PREFIX_REMOVE_ALL_ALL, TestPrefixInternal::idoRemovePrefixAllElemAllPrefixes, selPath) ) {
        return false;
    }
    //
    if(!testSkeletonBase( "1", FILE_PREFIX_0_START, FILE_PREFIX_REMOVE_ALL_ONE, TestPrefixInternal::idoRemovePrefixAllElemOnePrefix, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "2", FILE_PREFIX_0_START, FILE_PREFIX_REMOVE_SELNOR_ONE, TestPrefixInternal::idoRemovePrefixSelNoRecElemOnePrefix, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "3", FILE_PREFIX_0_START, FILE_PREFIX_REMOVE_SELR_ONE, TestPrefixInternal::idoRemovePrefixSelRecElemOnePrefix, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "4", FILE_PREFIX_0_START, FILE_PREFIX_REMOVE_SELNOR_ALL, TestPrefixInternal::idoRemovePrefixSelNoRecElemAllPrefixes, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "5", FILE_PREFIX_0_START, FILE_PREFIX_REMOVE_SELR_ALL, TestPrefixInternal::idoRemovePrefixSelRecElemAllPrefixes, selPath) ) {
        return false;
    }
    // Test for bookmark, only one case.
    if(!testSkeletonBase( "6", FILE_PREFIX_0_START, FILE_PREFIX_REMOVE_SELNOR_ONE, TestPrefixInternal::idoRemovePrefixBkmNoRecElemOnePrefix, selPath, SelOnlyBookmark) ) {
        return false;
    }
    if(!testSkeletonBase( "7", FILE_PREFIX_0_START, FILE_PREFIX_REMOVE_SELR_ONE, TestPrefixInternal::idoRemovePrefixBkmRecElemOnePrefix, selPath, SelOnlyBookmark) ) {
        return false;
    }
    return true;
}

bool TestNamespaces::testSetPrefix()
{
    _testName = "testSetPrefix";
    QList<int> selPath ;
    selPath << 1 << 1 ;

    // as is
    if(!testSkeletonBase( "0", FILE_PREFIX_0_START, FILE_PREFIX_SET_ALL, TestPrefixInternal::idoSetPrefixAll, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "1", FILE_PREFIX_0_START, FILE_PREFIX_SET_SELNOR, TestPrefixInternal::idoSetPrefixSelNoRec, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "2", FILE_PREFIX_0_START, FILE_PREFIX_SET_SELR, TestPrefixInternal::idoSetPrefixSelRec, selPath) ) {
        return false;
    }
    // Test for bookmark, only one case.
    if(!testSkeletonBase( "3", FILE_PREFIX_0_START, FILE_PREFIX_SET_SELNOR, TestPrefixInternal::idoSetPrefixBkmNoRec, selPath, SelOnlyBookmark) ) {
        return false;
    }
    if(!testSkeletonBase( "4", FILE_PREFIX_0_START, FILE_PREFIX_SET_SELR, TestPrefixInternal::idoSetPrefixBkmRec, selPath, SelOnlyBookmark) ) {
        return false;
    }
    return true;
}

bool TestNamespaces::testReplacePrefix()
{
    _testName = "testReplacePrefix";
    QList<int> selPath ;
    selPath << 1 << 1 ;

    // as is
    if(!testSkeletonBase( "0", FILE_PREFIX_0_START, FILE_PREFIX_REPLACE_ALL_ALL, TestPrefixInternal::idoReplacePrefixAllElemAllPrefixes, selPath) ) {
        return false;
    }
    //
    if(!testSkeletonBase( "1", FILE_PREFIX_0_START, FILE_PREFIX_REPLACE_ALL_ONE, TestPrefixInternal::idoReplacePrefixAllElemOnePrefix, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "2", FILE_PREFIX_0_START, FILE_PREFIX_REPLACE_SELNOR_ONE, TestPrefixInternal::idoReplacePrefixSelNoRecElemOnePrefix, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "3", FILE_PREFIX_0_START, FILE_PREFIX_REPLACE_SELR_ONE, TestPrefixInternal::idoReplacePrefixSelRecElemOnePrefix, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "4", FILE_PREFIX_0_START, FILE_PREFIX_REPLACE_SELNOR_ALL, TestPrefixInternal::idoReplacePrefixSelNoRecElemAllPrefixes, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "5", FILE_PREFIX_0_START, FILE_PREFIX_REPLACE_SELR_ALL, TestPrefixInternal::idoReplacePrefixSelRecElemAllPrefixes, selPath) ) {
        return false;
    }
    // Test for bookmark, only one case.
    if(!testSkeletonBase( "6", FILE_PREFIX_0_START, FILE_PREFIX_REPLACE_SELNOR_ONE, TestPrefixInternal::idoReplacePrefixBkmNoRecElemOnePrefix, selPath, SelOnlyBookmark) ) {
        return false;
    }
    if(!testSkeletonBase( "7", FILE_PREFIX_0_START, FILE_PREFIX_REPLACE_SELR_ONE, TestPrefixInternal::idoReplacePrefixBkmRecElemOnePrefix, selPath, SelOnlyBookmark) ) {
        return false;
    }
    return true;
}


///------------- region(namespace)

#define FILE_NS_0_START   BASE_PATH "namespace/start0.xml"
#define FILE_NS_1_START   BASE_PATH "namespace/start1.xml"
//--
#define FILE_NS_REMOVE_ALL_ALL   BASE_PATH "namespace/removeallall.xml"
#define FILE_NS_REMOVE_ALL_ALL_RD   BASE_PATH "namespace/removeallallrd.xml"
#define FILE_NS_REMOVE_ALL_ONE_RD   BASE_PATH "namespace/removeallonerd.xml"
#define FILE_NS_REMOVE_ALL_ONE      BASE_PATH "namespace/removeallone.xml"
#define FILE_NS_REMOVE_SELNOR_ONE   BASE_PATH "namespace/removeselnorone.xml"
#define FILE_NS_REMOVE_SELNOR_ONE_RD  BASE_PATH "namespace/removeselnoronerd.xml"
#define FILE_NS_REMOVE_SELR_ONE   BASE_PATH "namespace/removeselrone.xml"
#define FILE_NS_REMOVE_SELR_ONE_RD  BASE_PATH "namespace/removeselronerd.xml"
//--
#define FILE_NS_SET_ALL_ALL   BASE_PATH "namespace/setallall.xml"
#define FILE_NS_SET_SELNOR_ONE   BASE_PATH "namespace/setselnorone.xml"
#define FILE_NS_SET_SELNOR_ONE_RD  BASE_PATH "namespace/setselnoronerd.xml"
#define FILE_NS_SET_SELR_ONE   BASE_PATH "namespace/setselrone.xml"
#define FILE_NS_SET_SELR_ONE_RD  BASE_PATH "namespace/setselronerd.xml"
//--
#define FILE_NS_CASE0_START   BASE_PATH "namespace/normalize/case0start.xml"
#define FILE_NS_CASE0_END   BASE_PATH "namespace/normalize/case0end.xml"
#define FILE_NS_CASE1_END   BASE_PATH "namespace/normalize/case0end.xml"
//
#define FILE_NS_CASE2_START   BASE_PATH "namespace/normalize/case2start.xml"
#define FILE_NS_CASE2_END   BASE_PATH "namespace/normalize/case2end.xml"
#define FILE_NS_CASE3_END   BASE_PATH "namespace/normalize/case2end.xml"
//
#define FILE_NS_CASE4_START   BASE_PATH "namespace/normalize/case4start.xml"
#define FILE_NS_CASE4_END   BASE_PATH "namespace/normalize/case4end.xml"
#define FILE_NS_CASE5_END   BASE_PATH "namespace/normalize/case4end.xml"
#define FILE_NS_CASE6_START   BASE_PATH "namespace/normalize/case6start.xml"
#define FILE_NS_CASE6_END   BASE_PATH "namespace/normalize/case6end.xml"
#define FILE_NS_CASE7_END   BASE_PATH "namespace/normalize/case7end.xml"
#define FILE_NS_CASE8_END   BASE_PATH "namespace/normalize/case8end.xml"
#define FILE_NS_CASE9_END   BASE_PATH "namespace/normalize/case9end.xml"
#define FILE_NS_CASE12_START   BASE_PATH "namespace/normalize/case12start.xml"
#define FILE_NS_CASE12_END   BASE_PATH "namespace/normalize/case12end.xml"
#define FILE_NS_CASE13_START   BASE_PATH "namespace/normalize/case13start.xml"
#define FILE_NS_CASE13_END   BASE_PATH "namespace/normalize/case13end.xml"
#define FILE_NS_CASE14_START   BASE_PATH "namespace/normalize/case14start.xml"
#define FILE_NS_CASE14_END   BASE_PATH "namespace/normalize/case14end.xml"
#define FILE_NS_CASE15_START   BASE_PATH "namespace/normalize/case15start.xml"
#define FILE_NS_CASE15_END   BASE_PATH "namespace/normalize/case15end.xml"
#define FILE_NS_CASE16_START   BASE_PATH "namespace/normalize/case16start.xml"
#define FILE_NS_CASE16_END   BASE_PATH "namespace/normalize/case16end.xml"
#define FILE_NS_CASE17_START   BASE_PATH "namespace/normalize/case17start.xml"
#define FILE_NS_CASE17_END   BASE_PATH "namespace/normalize/case17end.xml"
#define FILE_NS_CASE18_START   BASE_PATH "namespace/normalize/case18start.xml"
#define FILE_NS_CASE18_END   BASE_PATH "namespace/normalize/case18end.xml"
#define FILE_NS_CASE19_START   BASE_PATH "namespace/normalize/case19start.xml"
#define FILE_NS_CASE19_END   BASE_PATH "namespace/normalize/case19end.xml"
#define FILE_NS_CASE20_END   BASE_PATH "namespace/normalize/case20end.xml"
#define FILE_NS_CASE21_END   BASE_PATH "namespace/normalize/case21end.xml"
#define FILE_NORMALIZE_DECLARESTART BASE_PATH "namespace/normalize/declare_start.xml"
#define FILE_NORMALIZE_DECLAREEND BASE_PATH "namespace/normalize/declare_end.xml"
//--
#define FILE_NS_REPLACE_ALL_ALL   BASE_PATH "namespace/replaceallall.xml"
#define FILE_NS_REPLACE_ALL_ALL_RD   BASE_PATH "namespace/replaceallallrd.xml"
#define FILE_NS_REPLACE_ALL_ONE_RD   BASE_PATH "namespace/replaceallonerd.xml"
#define FILE_NS_REPLACE_ALL_ONE      BASE_PATH "namespace/replaceallone.xml"
#define FILE_NS_REPLACE_SELNOR_ONE   BASE_PATH "namespace/replaceselnorone.xml"
#define FILE_NS_REPLACE_SELNOR_ONE_RD  BASE_PATH "namespace/replaceselnoronerd.xml"
#define FILE_NS_REPLACE_SELR_ONE   BASE_PATH "namespace/replaceselrone.xml"
//--
#define FILE_NS_STANDARD_  BASE_PATH "namespace/unit/ris_"
#define FILE_NS_START_ BASE_PATH "namespace/unit/src_"

#define FILE_NS_CHANGEPREFIX0_START  BASE_PATH "unit/change_prefix0_start.xml"
#define FILE_NS_CHANGEPREFIX0_END    BASE_PATH "unit/change_prefix0_end.xml"
#define FILE_NS_CHANGEPREFIX1_START  BASE_PATH "unit/change_prefix1_start.xml"
#define FILE_NS_CHANGEPREFIX1_END    BASE_PATH "unit/change_prefix1_end.xml"
#define FILE_NS_CHANGEPREFIX2_START  BASE_PATH "unit/change_prefix2_start.xml"
#define FILE_NS_CHANGEPREFIX2_END    BASE_PATH "unit/change_prefix2_end.xml"
#define FILE_NS_CHANGEPREFIX3_START  BASE_PATH "unit/change_prefix3_start.xml"
#define FILE_NS_CHANGEPREFIX3_END    BASE_PATH "unit/change_prefix3_end.xml"
#define FILE_NS_CHANGEPREFIX4_START  BASE_PATH "unit/change_prefix4_start.xml"
#define FILE_NS_CHANGEPREFIX4_END    BASE_PATH "unit/change_prefix4_end.xml"
//----
#define FILE_NS_DETECTPREFIX_1  BASE_PATH "unit/detect_1.xml"
#define FILE_NS_DETECTPREFIX_2  BASE_PATH "unit/detect_2.xml"
#define FILE_NS_DETECTPREFIX_3  BASE_PATH "unit/detect_3.xml"
#define FILE_NS_DETECTPREFIX_4  BASE_PATH "unit/detect_4.xml"
#define FILE_NS_DETECTPREFIX_5  BASE_PATH "unit/detect_5.xml"
#define FILE_NS_DETECTPREFIX_6  BASE_PATH "unit/detect_6.xml"
#define FILE_NS_DETECTPREFIX_7  BASE_PATH "unit/detect_7.xml"
#define FILE_NS_DETECTPREFIX_8  BASE_PATH "unit/detect_8.xml"
#define FILE_NS_DETECTPREFIX_9  BASE_PATH "unit/detect_9.xml"
//----
#define FILE_NSCLASH_0_START  BASE_PATH "namespace/unit/clash_0_start.xml"
#define FILE_NSCLASH_0_END  BASE_PATH "namespace/unit/clash_set_end.xml"
#define FILE_NSCLASH_1_END  BASE_PATH "namespace/unit/clash_replace_end.xml"
#define FILE_NSCLASH_2_END  BASE_PATH "namespace/unit/clash_normalize_end.xml"
//----

class TestNamespaceRemoveInternal {
    public:

    static bool idoRemoveNamespaceAllElemAllNamespacesRemoveDeclarations(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceRemove("abca", selection, TargetSelection::AllItems, true, true);
        return window->errorCount()==0;
    }

    static bool idoRemoveNamespaceAllElemAllNamespaces(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceRemove("abca", selection, TargetSelection::AllItems, true, false);
        return window->errorCount()==0;
    }

    static bool idoRemoveNamespaceAllElemOneNamespaceRemoveDeclarations(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceRemove("abca", selection, TargetSelection::AllItems, false, true);
        return window->errorCount()==0;
    }

    static bool idoRemoveNamespaceAllElemOneNamespace(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceRemove("abca", selection, TargetSelection::AllItems, false, false);
        return window->errorCount()==0;
    }

    static bool idoRemoveNamespaceOneElemNoROneNamespace(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceRemove("abca", selection, TargetSelection::SelectedItem, false, false);
        return window->errorCount()==0;
    }
    static bool idoRemoveNamespaceOneElemNoROneNamespaceRD(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceRemove("abca", selection, TargetSelection::SelectedItem, false, true);
        return window->errorCount()==0;
    }
    static bool idoRemoveNamespaceOneElemROneNamespace(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceRemove("abca", selection, TargetSelection::SelectedItemAndChildren, false, false);
        return window->errorCount()==0;
    }
    static bool idoRemoveNamespaceOneElemROneNamespaceRD(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceRemove("abca", selection, TargetSelection::SelectedItemAndChildren, false, true);
        return window->errorCount()==0;
    }
    static bool idoRemoveNamespaceBmkElemOneNamespace(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceRemove("abca", selection, TargetSelection::Bookmarks, false, false);
        return window->errorCount()==0;
    }
    static bool idoRemoveNamespaceBmkROneNamespace(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceRemove("abca", selection, TargetSelection::BookmarksAndChildren, false, false);
        return window->errorCount()==0;
    }
};

class TestNamespaceSetInternal {
    public:

    //-------------
    static const QString NewNS ;
    static const QString NewPr ;
    static bool idoSetNamespaceAllElemAllNamespaces(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceSet(NewNS, NewPr, selection, TargetSelection::AllItems);
        return window->errorCount()==0;
    }
    static bool idoSetNamespaceOneElemNoROneNamespace(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceSet(NewNS, NewPr, selection, TargetSelection::SelectedItem);
        return window->errorCount()==0;
    }
    static bool idoSetNamespaceOneElemROneNamespace(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceSet(NewNS, NewPr, selection, TargetSelection::SelectedItemAndChildren);
        return window->errorCount()==0;
    }
    static bool idoSetNamespaceBmkElemOneNamespace(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceSet(NewNS, NewPr, selection, TargetSelection::Bookmarks);
        return window->errorCount()==0;
    }
    static bool idoSetNamespaceBmkROneNamespace(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceSet(NewNS, NewPr, selection, TargetSelection::BookmarksAndChildren);
        return window->errorCount()==0;
    }
};

const QString TestNamespaceSetInternal::NewNS = "newns";
const QString TestNamespaceSetInternal::NewPr = "pr";

class TestNamespaceReplaceInternal {
    public:

    //-------------
    static const QString OldNS ;
    static const QString NewNS ;
    static const QString NewPr ;
    static bool idoReplaceNamespaceAllElemAllNamespaces(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceReplace(OldNS, NewNS, NewPr, selection, TargetSelection::AllItems);
        return window->errorCount()==0;
    }

    static bool idoReplaceNamespaceAllElemOneNamespace(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceReplace(OldNS, NewNS, NewPr, selection, TargetSelection::AllItems);
        return window->errorCount()==0;
    }

    static bool idoReplaceNamespaceOneElemNoROneNamespace(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceReplace(OldNS, NewNS, NewPr, selection, TargetSelection::SelectedItem);
        return window->errorCount()==0;
    }
    static bool idoReplaceNamespaceOneElemROneNamespace(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceReplace(OldNS, NewNS, NewPr, selection, TargetSelection::SelectedItemAndChildren);
        return window->errorCount()==0;
    }
    static bool idoReplaceNamespaceBmkElemOneNamespace(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceReplace(OldNS, NewNS, NewPr, selection, TargetSelection::Bookmarks);
        return window->errorCount()==0;
    }
    static bool idoReplaceNamespaceBmkROneNamespace(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceReplace(OldNS, NewNS, NewPr, selection, TargetSelection::BookmarksAndChildren);
        return window->errorCount()==0;
    }
};

const QString TestNamespaceReplaceInternal::OldNS = "abca";
const QString TestNamespaceReplaceInternal::NewNS = "newns";
const QString TestNamespaceReplaceInternal::NewPr = "pr";

class TestNamespaceNormalizeInternal {
    public:

    //-------------
    static const QString NewNS ;
    static const QString NewPr ;
    static bool idoNormalizeNamespaceDeclareOnlyRoot(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceNormalize(NewNS, NewPr, selection, TargetSelection::AllItems, true);
        return window->errorCount()==0;
    }

    static bool idoNormalizeNamespace(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceNormalize(NewNS, NewPr, selection, TargetSelection::AllItems, false);
        return window->errorCount()==0;
    }
    static bool idoNormalizeNamespaceInnerDeclareOnlyRoot(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceNormalize(NewNS, NewPr, selection, TargetSelection::SelectedItemAndChildren, true);
        return window->errorCount()==0;
    }

    static bool idoNormalizeNamespaceInner(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceNormalize(NewNS, NewPr, selection, TargetSelection::SelectedItemAndChildren, false);
        return window->errorCount()==0;
    }

    static bool idoNormalizeNamespaceInnerUsingBookmarks(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceNormalize(NewNS, NewPr, selection, TargetSelection::BookmarksAndChildren, false);
        return window->errorCount()==0;
    }
    static bool idoNormalizeNamespaceInnerDeclareOnlyRootUsingBookmarks(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceNormalize(NewNS, NewPr, selection, TargetSelection::BookmarksAndChildren, true);
        return window->errorCount()==0;
    }
};

const QString TestNamespaceNormalizeInternal::NewNS = "abca";
const QString TestNamespaceNormalizeInternal::NewPr = "a";


bool TestNamespaces::testRemoveNamespace()
{
    _testName = "testRemoveNamespace";
    QList<int> selPath ;
    selPath << 1 << 1 ;
    // as is
    if(!testSkeletonBase( "0", FILE_NS_0_START, FILE_NS_REMOVE_ALL_ALL_RD, TestNamespaceRemoveInternal::idoRemoveNamespaceAllElemAllNamespacesRemoveDeclarations, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "1", FILE_NS_0_START, FILE_NS_REMOVE_ALL_ALL, TestNamespaceRemoveInternal::idoRemoveNamespaceAllElemAllNamespaces, selPath) ) {
        return false;
    }
    //
    if(!testSkeletonBase( "2", FILE_NS_0_START, FILE_NS_REMOVE_ALL_ONE_RD, TestNamespaceRemoveInternal::idoRemoveNamespaceAllElemOneNamespaceRemoveDeclarations, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "3", FILE_NS_0_START, FILE_NS_REMOVE_ALL_ONE, TestNamespaceRemoveInternal::idoRemoveNamespaceAllElemOneNamespace, selPath) ) {
        return false;
    }

    if(!testSkeletonBase( "4", FILE_NS_0_START, FILE_NS_REMOVE_SELNOR_ONE, TestNamespaceRemoveInternal::idoRemoveNamespaceOneElemNoROneNamespace, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "5", FILE_NS_0_START, FILE_NS_REMOVE_SELNOR_ONE_RD, TestNamespaceRemoveInternal::idoRemoveNamespaceOneElemNoROneNamespaceRD, selPath) ) {
        return false;
    }

    if(!testSkeletonBase( "6", FILE_NS_0_START, FILE_NS_REMOVE_SELR_ONE, TestNamespaceRemoveInternal::idoRemoveNamespaceOneElemROneNamespace, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "7", FILE_NS_0_START, FILE_NS_REMOVE_SELR_ONE_RD, TestNamespaceRemoveInternal::idoRemoveNamespaceOneElemROneNamespaceRD, selPath) ) {
        return false;
    }
    // now the recursive, remone bs and all has been tested, test only remaining selections
    // Test for bookmark, only one case.
    if(!testSkeletonBase( "8", FILE_NS_0_START, FILE_NS_REMOVE_SELNOR_ONE, TestNamespaceRemoveInternal::idoRemoveNamespaceBmkElemOneNamespace, selPath, SelOnlyBookmark) ) {
        return false;
    }
    if(!testSkeletonBase( "9", FILE_NS_0_START, FILE_NS_REMOVE_SELR_ONE, TestNamespaceRemoveInternal::idoRemoveNamespaceBmkROneNamespace, selPath, SelOnlyBookmark) ) {
        return false;
    }
    return true;
}

bool TestNamespaces::testSetNamespace()
{
    _testName = "testSetNamespace";
    QList<int> selPath ;
    selPath << 1 << 1 ;
    // as is
    if(!testSkeletonBase( "1", FILE_NS_0_START, FILE_NS_SET_ALL_ALL, TestNamespaceSetInternal::idoSetNamespaceAllElemAllNamespaces, selPath) ) {
        return false;
    }
    //
    if(!testSkeletonBase( "4", FILE_NS_0_START, FILE_NS_SET_SELNOR_ONE, TestNamespaceSetInternal::idoSetNamespaceOneElemNoROneNamespace, selPath) ) {
        return false;
    }

    if(!testSkeletonBase( "6", FILE_NS_0_START, FILE_NS_SET_SELR_ONE, TestNamespaceSetInternal::idoSetNamespaceOneElemROneNamespace, selPath) ) {
        return false;
    }
    // now the recursive, remone bs and all has been tested, test only remaining selections
    // Test for bookmark, only one case.
    if(!testSkeletonBase( "8", FILE_NS_0_START, FILE_NS_SET_SELNOR_ONE, TestNamespaceSetInternal::idoSetNamespaceBmkElemOneNamespace, selPath, SelOnlyBookmark) ) {
        return false;
    }
    if(!testSkeletonBase( "9", FILE_NS_0_START, FILE_NS_SET_SELR_ONE, TestNamespaceSetInternal::idoSetNamespaceBmkROneNamespace, selPath, SelOnlyBookmark) ) {
        return false;
    }
    return true;
}

bool TestNamespaces::testReplaceNamespace()
{
    _testName = "testReplaceNamespace";

    QList<int> selPath ;
    selPath << 1 << 1 ;
    // as is
    if(!testSkeletonBase( "1", FILE_NS_0_START, FILE_NS_REPLACE_ALL_ALL, TestNamespaceReplaceInternal::idoReplaceNamespaceAllElemAllNamespaces, selPath) ) {
        return false;
    }
    //
    if(!testSkeletonBase( "3", FILE_NS_0_START, FILE_NS_REPLACE_ALL_ONE, TestNamespaceReplaceInternal::idoReplaceNamespaceAllElemOneNamespace, selPath) ) {
        return false;
    }

    if(!testSkeletonBase( "4", FILE_NS_0_START, FILE_NS_REPLACE_SELNOR_ONE, TestNamespaceReplaceInternal::idoReplaceNamespaceOneElemNoROneNamespace, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "6", FILE_NS_0_START, FILE_NS_REPLACE_SELR_ONE, TestNamespaceReplaceInternal::idoReplaceNamespaceOneElemROneNamespace, selPath) ) {
        return false;
    }
    // now the recursive, remone bs and all has been tested, test only remaining selections
    // Test for bookmark, only one case.
    if(!testSkeletonBase( "8", FILE_NS_0_START, FILE_NS_REPLACE_SELNOR_ONE, TestNamespaceReplaceInternal::idoReplaceNamespaceBmkElemOneNamespace, selPath, SelOnlyBookmark) ) {
        return false;
    }
    if(!testSkeletonBase( "9", FILE_NS_0_START, FILE_NS_REPLACE_SELR_ONE, TestNamespaceReplaceInternal::idoReplaceNamespaceBmkROneNamespace, selPath, SelOnlyBookmark) ) {
        return false;
    }
    return true;
}

class TestNamespaceClashInternal {
    public:

    //-------------
    static const QString OldNS ;
    static const QString NewNS ;
    static const QString NewPr ;
    static bool idoClashSet(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        NamespacesInfo info;
        Element *element = window->getEditor()->getSelectedItem();
        window->getRegola()->collectNamespaces(info, element);
        window->getEditor()->namespaceSet(NewNS, NewPr, selection, TargetSelection::SelectedItem, true, &info);
        return window->errorCount()==0;
    }

    static bool idoClashReplace(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        NamespacesInfo info;
        Element *element = window->getEditor()->getSelectedItem();
        window->getRegola()->collectNamespaces(info, element);
        window->getEditor()->namespaceReplace(OldNS, NewNS, NewPr, selection, TargetSelection::AllItems, true, &info);
        return window->errorCount()==0;
    }

    static bool idoClashNormalize(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        NamespacesInfo info;
        Element *element = window->getEditor()->getSelectedItem();
        window->getRegola()->collectNamespaces(info, element);
        window->getEditor()->namespaceNormalize(OldNS, NewPr, selection, TargetSelection::SelectedItemAndChildren, false, true, &info);
        return window->errorCount()==0;
    }
};

const QString TestNamespaceClashInternal::OldNS = "abca";
const QString TestNamespaceClashInternal::NewNS = "newns";
const QString TestNamespaceClashInternal::NewPr = "a";

bool TestNamespaces::testUnitNamespaceClash()
{
    _testName = "testUnitNamespaceClash";

    QList<int> selPath ;
    selPath << 1 << 1 ;
    if(!testSkeletonBase( "Set", FILE_NSCLASH_0_START, FILE_NSCLASH_0_END, TestNamespaceClashInternal::idoClashSet, selPath) ) {
        return false;
    }
    //
    if(!testSkeletonBase( "Replace", FILE_NSCLASH_0_START, FILE_NSCLASH_1_END, TestNamespaceClashInternal::idoClashReplace, selPath) ) {
        return false;
    }

    if(!testSkeletonBase( "Normalize", FILE_NSCLASH_0_START, FILE_NSCLASH_2_END, TestNamespaceClashInternal::idoClashNormalize, selPath) ) {
        return false;
    }

    return true ;
}

/***
 * Keys:
 * 0: root 1ns, root false
 * 1: root 1ns, root true
 * 2: root+a, 1ns root false
 * 3: root+a, 1ns root true
 * 4: root+a, 2ns root false
 * 5: root+a, 2ns root true
 *
 * the root declaration is at higher level:
 * if applied on root root, else it is the root of the selection
 * so you can normalize a tree without being obliged to insert the
 * ns delcaration at the very root of the tree
 *
 * 6: root+inner+a root false, sel all
 * 7: root+inner+a root true, sel all
 * 8: root+inner+a root false, sel sel
 * 9: root+inner+a root true, sel sel
 *10: root+inner+a root false, sel bmk
 *11: root+inner+a root true, sel bmk
 *
 *12: inner+a root false, sel all
 *13: inner+a root true, sel all
 *14: inner+a root false, sel sel
 *15: inner+a root true, sel sel
 *16: inner+a root false, sel bmk
 *17: inner+a root true, sel bmk
 *
 * here, simplified
 *18: a, root false sel all
 *19: a, root true sel all
 *20: a, root true sel sel
 *21: a, root true sel bmk
 *
 * selezione interna, dichiarato su root e all'interno (+flag root)
 * selezione interna, dichiarato solo su root  (+flag root)
 * selezione interna, dichiarato solo su interno  (+flag root)
 *
 */

bool TestNamespaces::testNormalizeNamespace()
{
    _testName = "testNormalizeNamespace";
    QList<int> selPath ;
    selPath << 1 << 0 ;
    // as is
    if(!testSkeletonBase( "0", FILE_NS_CASE0_START, FILE_NS_CASE0_END, TestNamespaceNormalizeInternal::idoNormalizeNamespace, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "1", FILE_NS_CASE0_START, FILE_NS_CASE1_END, TestNamespaceNormalizeInternal::idoNormalizeNamespaceDeclareOnlyRoot, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "2", FILE_NS_CASE2_START, FILE_NS_CASE2_END, TestNamespaceNormalizeInternal::idoNormalizeNamespace, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "3", FILE_NS_CASE2_START, FILE_NS_CASE3_END, TestNamespaceNormalizeInternal::idoNormalizeNamespaceDeclareOnlyRoot, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "4", FILE_NS_CASE4_START, FILE_NS_CASE4_END, TestNamespaceNormalizeInternal::idoNormalizeNamespace, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "5", FILE_NS_CASE4_START, FILE_NS_CASE5_END, TestNamespaceNormalizeInternal::idoNormalizeNamespaceDeclareOnlyRoot, selPath) ) {
        return false;
    }
    //----
    if(!testSkeletonBase( "6", FILE_NS_CASE6_START, FILE_NS_CASE6_END, TestNamespaceNormalizeInternal::idoNormalizeNamespace, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "7", FILE_NS_CASE6_START, FILE_NS_CASE7_END, TestNamespaceNormalizeInternal::idoNormalizeNamespaceDeclareOnlyRoot, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "8", FILE_NS_CASE6_START, FILE_NS_CASE8_END, TestNamespaceNormalizeInternal::idoNormalizeNamespaceInner, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "9", FILE_NS_CASE6_START, FILE_NS_CASE9_END, TestNamespaceNormalizeInternal::idoNormalizeNamespaceInnerDeclareOnlyRoot, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "10", FILE_NS_CASE6_START, FILE_NS_CASE8_END, TestNamespaceNormalizeInternal::idoNormalizeNamespaceInnerUsingBookmarks, selPath, SelOnlyBookmark) ) {
        return false;
    }
    if(!testSkeletonBase( "11", FILE_NS_CASE6_START, FILE_NS_CASE9_END, TestNamespaceNormalizeInternal::idoNormalizeNamespaceInnerUsingBookmarks, selPath, SelOnlyBookmark) ) {
        return false;
    }
    //--
    if(!testSkeletonBase( "12", FILE_NS_CASE12_START, FILE_NS_CASE12_END, TestNamespaceNormalizeInternal::idoNormalizeNamespace, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "13", FILE_NS_CASE12_START, FILE_NS_CASE13_END, TestNamespaceNormalizeInternal::idoNormalizeNamespaceDeclareOnlyRoot, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "14", FILE_NS_CASE12_START, FILE_NS_CASE14_END, TestNamespaceNormalizeInternal::idoNormalizeNamespaceInner, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "15", FILE_NS_CASE12_START, FILE_NS_CASE15_END, TestNamespaceNormalizeInternal::idoNormalizeNamespaceInnerDeclareOnlyRoot, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "16", FILE_NS_CASE12_START, FILE_NS_CASE14_END, TestNamespaceNormalizeInternal::idoNormalizeNamespaceInnerUsingBookmarks, selPath, SelOnlyBookmark) ) {
        return false;
    }
    if(!testSkeletonBase( "17", FILE_NS_CASE12_START, FILE_NS_CASE15_END, TestNamespaceNormalizeInternal::idoNormalizeNamespaceInnerUsingBookmarks, selPath, SelOnlyBookmark) ) {
        return false;
    }
    //--
    if(!testSkeletonBase( "18", FILE_NS_CASE18_START, FILE_NS_CASE18_END, TestNamespaceNormalizeInternal::idoNormalizeNamespace, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "19", FILE_NS_CASE18_START, FILE_NS_CASE19_END, TestNamespaceNormalizeInternal::idoNormalizeNamespaceDeclareOnlyRoot, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "20", FILE_NS_CASE18_START, FILE_NS_CASE20_END, TestNamespaceNormalizeInternal::idoNormalizeNamespaceInnerDeclareOnlyRoot, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "21", FILE_NS_CASE18_START, FILE_NS_CASE21_END, TestNamespaceNormalizeInternal::idoNormalizeNamespaceInnerUsingBookmarks, selPath, SelOnlyBookmark) ) {
        return false;
    }
    //---
    QList<int> selPath1 ;
    selPath1 << 1 << 1 ;
    if(!testSkeletonBase( "22", FILE_NORMALIZE_DECLARESTART, FILE_NORMALIZE_DECLAREEND, TestNamespaceNormalizeInternal::idoNormalizeNamespaceInner, selPath1) ) {
        return false;
    }
    return true;
}

/******************************************************************************
 Tests to do:
 NS Target    NS Other    Outcome
0  N              N           SRC
1  Y              N           SRC
2  2              N           SRC
3  N              2           REPL
4  2              2           REPL
******************************************************************************/
class TestNamespaceChangePrefixInternal {
    public:

    static bool idoNamespaceChangePrefix0(TestBase * /*aptr*/, MainWindow * window, Element * /*selection*/)
    {
        NamespacesInfo info ;
        window->getRegola()->collectNamespaces(info, NULL);
        window->getEditor()->namespaceAvoidClash("ns1", "a", &info);
        return window->errorCount()==0;
    }
};

bool TestNamespaces::testUnitChangePrefix()
{
    _testName = "testUnitChangePrefix";
    QList<int> selPath ;
    selPath << 1 << 1 ;
    // as is
    if(!testSkeletonBase( "0", FILE_NS_CHANGEPREFIX0_START, FILE_NS_CHANGEPREFIX0_END, TestNamespaceChangePrefixInternal::idoNamespaceChangePrefix0, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "1", FILE_NS_CHANGEPREFIX1_START, FILE_NS_CHANGEPREFIX1_END, TestNamespaceChangePrefixInternal::idoNamespaceChangePrefix0, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "2", FILE_NS_CHANGEPREFIX2_START, FILE_NS_CHANGEPREFIX2_END, TestNamespaceChangePrefixInternal::idoNamespaceChangePrefix0,selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "3", FILE_NS_CHANGEPREFIX3_START, FILE_NS_CHANGEPREFIX3_END, TestNamespaceChangePrefixInternal::idoNamespaceChangePrefix0, selPath) ) {
        return false;
    }
    if(!testSkeletonBase( "4", FILE_NS_CHANGEPREFIX4_START, FILE_NS_CHANGEPREFIX4_END, TestNamespaceChangePrefixInternal::idoNamespaceChangePrefix0, selPath) ) {
        return false;
    }
    return true;
}


bool TestNamespaces::testUnitNamespaces()
{
    _testName = "testUnitNamespaces";

    if(!testUnitChangePrefix()) {
        return false;
    }
    // no need to test for delete
    if(!testUnitSetNamespace()) {
        return false;
    }
    if(!testUnitReplaceNamespace()) {
        return false;
    }
    if(!testUnitNamespaceClash()) {
        return false ;
    }
    return true ;
}


class TestNamespaceUnitInternal {
    public:

    static bool unitReplaceNS(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceReplace("abca", "newns", "pr", selection, TargetSelection::SelectedItemAndChildren);
        return window->errorCount()==0;
    }
    static bool unitSetNS(TestBase * /*aptr*/, MainWindow * window, Element *selection)
    {
        window->getEditor()->namespaceSet("abca", "a", selection, TargetSelection::SelectedItemAndChildren);
        return window->errorCount()==0;
    }
};

bool TestNamespaces::testUnitSetNamespace()
{
    _testName = "testUnitSetNamespace";
    if(!testStandardNamespace("set", TestNamespaceUnitInternal::unitSetNS)) {
        return false;
    }
    return true ;
}

bool TestNamespaces::testUnitReplaceNamespace()
{
    _testName = "testUnitReplaceNamespace";
    if(!testStandardNamespace("replace", TestNamespaceUnitInternal::unitReplaceNS)) {
        return false;
    }
    return true ;
}

/* Test cases: selection types tested otherwise
 * 0: parent samepr
 * 1: parent other pr
 * 2: self other pr
 * 3: self same pr
 * 4: child: same pr
 * 5: child other pr
 * 6: other ns declared (this test can be added to other)
 * 7: same pr other ns parent
 * 8: same pr other ns self
 * 9: same pr other ns children
 */
bool TestNamespaces::testStandardNamespace(const QString &aCase, bool (*apply)(TestBase *arg, MainWindow *, Element *))
{
    QList<int> selPath ;
    selPath << 1 << 0 ;

    FORINT( index, 10 ) {
        QString resFileName = QString("%1%2%3.xml").arg(FILE_NS_STANDARD_).arg(aCase).arg(index);
        QString startFileName = QString("%1%2.xml").arg(FILE_NS_START_).arg(index);
        if(!testSkeletonBase( QString::number(index), startFileName, resFileName, apply, selPath) ) {
            return false;
        }
    }
    return true ;
}

class TestNamespaceDetectPrefixInternal {
    public:

    static bool idoCheck(TestBase * test, MainWindow * window, Element * /*selection*/, void *aptr)
    {
        NamespacesInfo info;
        const bool expected = *(bool*)aptr;
        window->getRegola()->collectNamespaces(info, NULL);
        bool result = info.isUsedPrefixForOtherNamespace("abcd", "pre");
        if(result != expected ) {
            return test->error(QString("Found %1, expcted:%2").arg(result).arg(expected));
        }
        return true;
    }
};

/*
 * Tests: R:Root I:internal
 * Declaration  Data
 * PS, NS : prefix amd ns, AP:AN other prefixes and namespaces.
 * 1: PS:AN-AP:NS-?
 * 2: ?-PS:AN-?
 * 3: ?-?-PS:AN
 * --
 * 4: PS:AN-PS:NS-?
 * 5: PS:NS-PS:AN-?
 * ---
 * 6: PS:NS-?-?
 * 7: ?-PS:NS-?
 * 8: ?-?-PS:NS
 * ---
 * 9: AP:AN-?-?
 */

bool TestNamespaces::testForDetectPrefixes( const QString &id, const QString &file, const bool newExpected )
{
    QList<int> selPath ;
    selPath << 1 << 0 ;
    bool expected = newExpected ;
    if(!testSkeletonBaseNoOp( id, file, TestNamespaceDetectPrefixInternal::idoCheck, selPath, SelOnlySelection, &expected) ) {
        return false;
    }
    return true;
}

bool TestNamespaces::testDetectPrefixes()
{
    _testName = "testDetectPrefixes";
    // as is
    if(!testForDetectPrefixes( "1", FILE_NS_DETECTPREFIX_1, true) ) {
        return false;
    }
    if(!testForDetectPrefixes( "2", FILE_NS_DETECTPREFIX_2, true) ) {
        return false;
    }
    if(!testForDetectPrefixes( "3", FILE_NS_DETECTPREFIX_3, true) ) {
        return false;
    }
    if(!testForDetectPrefixes( "4", FILE_NS_DETECTPREFIX_4, true) ) {
        return false;
    }
    if(!testForDetectPrefixes( "5", FILE_NS_DETECTPREFIX_5, true) ) {
        return false;
    }
    if(!testForDetectPrefixes( "6", FILE_NS_DETECTPREFIX_6, false) ) {
        return false;
    }
    if(!testForDetectPrefixes( "7", FILE_NS_DETECTPREFIX_7, false) ) {
        return false;
    }
    if(!testForDetectPrefixes( "8", FILE_NS_DETECTPREFIX_8, false) ) {
        return false;
    }
    if(!testForDetectPrefixes( "9", FILE_NS_DETECTPREFIX_9, false) ) {
        return false;
    }
    return true;
}
