/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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


#include "testsearch.h"
#include "app.h"
#include "findtextparams.h"

#define FILE_SEARCH "../test/data/search/base.xml"
#define FILE_SEARCH_XQUERY "../test/data/search/base_xquery.xml"


TestSearch::TestSearch()
{
}

TestSearch::~TestSearch()
{
}

//---------------------------------------------- UTILITIES
class TestSearchHelper {

    void initSearch(const QString &textToSearch);

public:
    App app;
    FindTextParams findArgs;
    Element *selectedItem ;
    bool isXQuery;
    QString fileToLoad;

    TestSearchHelper(const bool isXQuery = false);
    ~TestSearchHelper();
    bool init();
    void selectRoot();
    void find(const QString &textToSearch);
    void initFind(const QString &textToSearch);
    void search();
};

TestSearchHelper::TestSearchHelper(const bool newIsXQuery)
{
    isXQuery = newIsXQuery ;
    if(isXQuery) {
        fileToLoad = FILE_SEARCH_XQUERY ;
    } else {
        fileToLoad = FILE_SEARCH ;
    }
    selectedItem = NULL ;
}

TestSearchHelper::~TestSearchHelper()
{

}

bool TestSearchHelper::init()
{
    app.init();
    return app.mainWindow()->loadFile(fileToLoad);
}

void TestSearchHelper::selectRoot()
{
    selectedItem = NULL ;
    QTreeWidget *tree = app.mainWindow()->getMainTreeWidget();
    tree->setCurrentItem(app.mainWindow()->getRegola()->root()->getUI());
}

void TestSearchHelper::initSearch(const QString &textToSearch)
{
    selectedItem = NULL ;
    findArgs.init(textToSearch, false, true, false, false,
                            false, FindTextParams::FIND_ALL, true,
                  true, true, "", isXQuery);
}

void TestSearchHelper::find(const QString &textToSearch)
{
    initSearch(textToSearch);
    app.mainWindow()->getEditor()->setUpdatesEnabled(false);
    app.mainWindow()->getRegola()->findText(findArgs, selectedItem);
    app.mainWindow()->getEditor()->setUpdatesEnabled(true);
}

void TestSearchHelper::initFind(const QString &textToSearch)
{
    initSearch(textToSearch);
}

void TestSearchHelper::search()
{
    app.mainWindow()->getEditor()->getRegola()->clearBookmarks();
    app.mainWindow()->getEditor()->setUpdatesEnabled(false);
    app.mainWindow()->getRegola()->findText(findArgs, selectedItem);
    app.mainWindow()->getEditor()->setUpdatesEnabled(true);
}


//---------------------------------------------- LITERAL SEARCH

void TestSearch::init(const QString &nameTest, TestSearchHelper &helper)
{
    _testName = nameTest ;
    helper.init();
    helper.selectRoot();
}

void TestSearch::testAStdSearchWithParamsInit(const QString &nameTest, TestSearchHelper &helper)
{
    _testName = nameTest ;
    helper.init();
    helper.selectRoot();
}

bool TestSearch::checkResults(TestSearchHelper &helper, const int expectedBookmarks, const int expectedCount)
{
    return checkResults(helper, expectedBookmarks, expectedCount, NULL );
}

bool TestSearch::checkResults(TestSearchHelper &helper, const int expectedBookmarks, const int expectedCount, QStringList *expectedElementTags)
{
    if(helper.app.mainWindow()->getRegola()->bookmarkSize()!= expectedBookmarks)
    {
        return error(QString("Expected %1 bookmarks match, but found:%2").arg(expectedBookmarks).arg(helper.app.mainWindow()->getRegola()->bookmarkSize()));
    }
    if( helper.findArgs.getOccurrences() != expectedCount ) {
        return error(QString("Expected %1 occurrence, but found:%2").arg(expectedCount).arg(helper.findArgs.getOccurrences()));
    }
    if( NULL != expectedElementTags) {
        int index = 0 ;
        QVector<Bookmark*> &bmkA = helper.app.mainWindow()->getRegola()->bookmarks.getBookmarks();
        foreach( Bookmark* bmk , bmkA ) {
            Element *element = bmk->getElement();
            if( element->tag() != expectedElementTags->at(index)) {
                return error(QString("The bookmark %1 differs. Expected '%2', but found '%3'\n").arg(index).arg(expectedElementTags->at(index)).arg(element->tag()));
            }
            index++;
        }
    }

    return true;
}

bool TestSearch::checkResultsByPath(TestSearchHelper &helper, const int expectedBookmarks, const int expectedCount, QList<QList<int> > &expectedPathList)
{
    if(helper.app.mainWindow()->getRegola()->bookmarkSize()!= expectedBookmarks)
    {
        return error(QString("Expected %1 bookmarks match, but found:%2").arg(expectedBookmarks).arg(helper.app.mainWindow()->getRegola()->bookmarkSize()));
    }
    if( helper.findArgs.getOccurrences() != expectedCount ) {
        return error(QString("Expected %1 occurrence, but found:%2").arg(expectedCount).arg(helper.findArgs.getOccurrences()));
    }
    int index = 0 ;
    QVector<Bookmark*> &bmkA = helper.app.mainWindow()->getRegola()->bookmarks.getBookmarks();
    if( bmkA.size() != expectedPathList.size() ) {
        return error(QString("Path List differs from boomakes count. bookmarks count: %1 path count:%2").arg(bmkA.size()).arg(expectedPathList.size()));
    }
    foreach( Bookmark* bmk , bmkA ) {
        Element *element = bmk->getElement();
        QList<int> thisPath = expectedPathList.at(index);
        QList<int> elPath = element->indexPath();
        QString message = QString("The bookmark %1 differs by path.").arg(index);
        if( !compareListInts(message, thisPath, elPath)) {
            return false;
        }
        index++;
    }

    return true;
}


bool TestSearch::checkTestSearch(TestSearchHelper &helper, const QString &textToSearch, const int expectedBookmarks, const int expectedCount)
{
    helper.find(textToSearch);
    return checkResults(helper, expectedBookmarks, expectedCount);
}

bool TestSearch::checkTestSearch( TestSearchHelper &helper, const QString &textToSearch, const int expected)
{
    return checkTestSearch(helper, textToSearch, expected, expected);
}

bool TestSearch::testAStdSearch(TestSearchHelper &helper, const QString &nameTest, const int expected, const QString &textToSearch)
{
    testAStdSearchWithParamsInit(nameTest, helper );
    return checkTestSearch(helper, textToSearch, expected);
}

//---------------------------------------------- LITERAL SEARCH

bool TestSearch::literalSearchNoItemFound()
{
    TestSearchHelper helper;
    return testAStdSearch( helper, "literalSearchNoItemFound", 0, "wwww");
}

bool TestSearch::literalSearchNonExact()
{
    TestSearchHelper helper;
    return testAStdSearch( helper, "literalSearchNonExact", 8, "aaaa");
}

bool TestSearch::literalSearchExact()
{
    TestSearchHelper helper;
    testAStdSearchWithParamsInit("literalSearchExact", helper);
    helper.initFind("aaaa");
    helper.findArgs.setMatchExact(true);
    helper.search();
    return checkResults(helper, 4, 4);
}

bool TestSearch::literalSearchExactInChildren()
{
    TestSearchHelper helper;
    testAStdSearchWithParamsInit("literalSearchExactInChildren", helper);
    helper.initFind("aaaa");
    helper.findArgs.setMatchExact(true);
    helper.findArgs.setOnlyChildren(true);
    QList<int> selList;
    selList.append(0);
    selList.append(1);
    Element *element = helper.app.mainWindow()->getRegola()->findElementByArray(selList);
    helper.selectedItem = element ;
    helper.search();
    return checkResults(helper, 1, 1);
}

bool TestSearch::literalSearchNonExactInChildren()
{
    TestSearchHelper helper;
    testAStdSearchWithParamsInit("literalSearchNonExactInChildren", helper);
    helper.initFind("aaaa");
    helper.findArgs.setMatchExact(false);
    helper.findArgs.setOnlyChildren(true);
    QList<int> selList;
    selList.append(0);
    selList.append(1);
    Element *element = helper.app.mainWindow()->getRegola()->findElementByArray(selList);
    helper.selectedItem = element ;
    helper.search();
    return checkResults(helper, 3, 3);
}

bool TestSearch::literalSearchExactInChildrenNoMatch()
{
    TestSearchHelper helper;
    testAStdSearchWithParamsInit("literalSearchExactInChildren", helper);
    helper.initFind("bbbb");
    helper.findArgs.setMatchExact(true);
    helper.findArgs.setOnlyChildren(true);
    QList<int> selList;
    selList.append(0);
    selList.append(1);
    Element *element = helper.app.mainWindow()->getRegola()->findElementByArray(selList);
    helper.selectedItem = element ;
    helper.search();
    return checkResults(helper, 0, 0);
}

bool TestSearch::literalSearchCountInChildren()
{
    TestSearchHelper helper;
    testAStdSearchWithParamsInit("literalSearchCountInChildren", helper);
    helper.initFind("aaaa");
    helper.findArgs.setMatchExact(true);
    helper.findArgs.setOnlyChildren(true);
    helper.findArgs.setCountOnly(true);
    QList<int> selList;
    selList.append(0);
    selList.append(1);
    Element *element = helper.app.mainWindow()->getRegola()->findElementByArray(selList);
    helper.selectedItem = element ;
    helper.search();
    return checkResults(helper, 0, 1);
}

bool TestSearch::literalSearchInElementTags()
{
    TestSearchHelper helper;
    testAStdSearchWithParamsInit("literalSearchInElementTags", helper);
    helper.initFind("hzz");
    helper.findArgs.setMatchExact(true);
    helper.findArgs.setScope(FindTextParams::FIND_TAG);
    helper.search();
    QStringList expected;
    expected << "hzz" ;
    return checkResults(helper, 1, 1, &expected);
}

bool TestSearch::literalSearchInAttributeNames()
{
    TestSearchHelper helper;
    testAStdSearchWithParamsInit("literalSearchInAttributeNames", helper);
    helper.initFind("hzz");
    helper.findArgs.setMatchExact(true);
    helper.findArgs.setScope(FindTextParams::FIND_ATTRIBUTE_NAME);
    helper.search();
    QStringList expected;
    expected << "bhzz" ;
    return checkResults(helper, 1, 1, &expected);
}

bool TestSearch::literalSearchInAttributes()
{
    TestSearchHelper helper;
    testAStdSearchWithParamsInit("literalSearchInAttributes", helper);
    helper.initFind("hzz");
    helper.findArgs.setMatchExact(true);
    helper.findArgs.setScope(FindTextParams::FIND_ATTRIBUTE_VALUE);
    helper.search();
    QStringList expected;
    expected << "ahzz" ;
    return checkResults(helper, 1, 1, &expected);
}

bool TestSearch::literalSearchInText()
{
    TestSearchHelper helper;
    testAStdSearchWithParamsInit("literalSearchInText", helper);
    helper.initFind("h1zz");
    helper.findArgs.setMatchExact(false);
    helper.findArgs.setScope(FindTextParams::FIND_TEXT);
    helper.search();
    QStringList expected;
    expected << "chzz" ;
    return checkResults(helper, 1, 1, &expected);
}

bool TestSearch::literalSearchInComment()
{
    TestSearchHelper helper;
    testAStdSearchWithParamsInit("literalSearchInCommnet", helper);
    helper.initFind("in CDATA not ");
    helper.findArgs.setMatchExact(false);
    helper.findArgs.setScope(FindTextParams::FIND_COMMENT);
    helper.search();
    QList<QList<int> > results;
    QList<int> expected;
    expected << 0 << 14 ;
    results << expected;
    return checkResultsByPath(helper, 1, 1, results);
}

bool TestSearch::literalSearchInTextBase64()
{
    TestSearchHelper helper;
    testAStdSearchWithParamsInit("literalSearchInTextBase64", helper);
    helper.initFind("text not exact");
    helper.findArgs.setMatchExact(false);
    helper.findArgs.setScope(FindTextParams::FIND_TEXT_BASE64);
    helper.search();
    QList<QList<int> > results;
    QList<int> expectedText;
    expectedText << 0 << 19 ;
    results << expectedText;
    QList<int> expectedAttributes;
    expectedAttributes << 0 << 20 ;
    results << expectedAttributes ;
    return checkResultsByPath(helper, 2, 2, results);
}

bool TestSearch::literalSearchInScopedPath()
{
    TestSearchHelper helper;
    testAStdSearchWithParamsInit("literalSearchInScopedPath", helper);
    helper.initFind("ppp");
    helper.findArgs.setMatchExact(true);
    helper.findArgs.setScope(FindTextParams::FIND_TAG);
    helper.findArgs.setScopePath("one/ppp");
    helper.search();
    QStringList expected;
    expected << "ppp" ;
    return checkResults(helper, 1, 1, &expected);
}

bool TestSearch::literalSearchInScopedPathAttribute()
{
    TestSearchHelper helper;
    testAStdSearchWithParamsInit("literalSearchInScopedPathAttribute", helper);
    helper.initFind("ppp");
    helper.findArgs.setMatchExact(true);
    helper.findArgs.setScopePath("one/@a");
    helper.findArgs.setScope(FindTextParams::FIND_ATTRIBUTE_VALUE );
    helper.search();
    QStringList expected;
    expected << "one" ;
    return checkResults(helper, 1, 1, &expected);
}

//---------------------------------------------- XQUERY SEARCH

bool TestSearch::xquerySearchAttributesExact()
{
    TestSearchHelper helper(true);
    testAStdSearchWithParamsInit("xquerySearchAttributesExact", helper);
    helper.initFind("//*[@data eq 'wwww']");
    helper.search();
    QStringList expected;
    expected << "h1" << "h2" << "h3";
    return checkResults(helper, 3, 3, &expected);
}

bool TestSearch::xquerySearchAttributesNotExact()
{
    TestSearchHelper helper(true);
    testAStdSearchWithParamsInit("xquerySearchAttributesExact", helper);
    helper.initFind("//*[contains(@data, 'www') eq true()]");
    helper.search();
    QStringList expected;
    expected << "h1" << "h2" << "h3" << "h4" << "h5";
    return checkResults(helper, 5, 5, &expected);
}

bool TestSearch::xquerySearchAttributeNameExact()
{
    TestSearchHelper helper(true);
    testAStdSearchWithParamsInit("xquerySearchAttributesExact", helper);
    helper.initFind("//*/@*[name() eq 'att1']");
    helper.search();
    QStringList expected;
    expected << "child1" << "child2" << "child2" ;
    return checkResults(helper, 3, 3, &expected);
}

bool TestSearch::xquerySearchAttributeNameNotExact()
{
    TestSearchHelper helper(true);
    testAStdSearchWithParamsInit("xquerySearchAttributeNameNotExact", helper);
    helper.initFind("//*/@*[ starts-with( name() , 'at')]");
    helper.search();
    QStringList expected;
    expected << "child1" << "child2" << "child2" << "rimpl";
    return checkResults(helper, 4, 4, &expected);
}

bool TestSearch::xquerySearchElementsExact()
{
    TestSearchHelper helper(true);
    testAStdSearchWithParamsInit("xquerySearchElementsExact", helper);
    helper.initFind("//child1");
    helper.search();
    QStringList expected;
    expected << "child1" ;
    return checkResults(helper, 1, 1, &expected);
}

bool TestSearch::xquerySearchPathExact()
{
    TestSearchHelper helper(true);
    testAStdSearchWithParamsInit("xquerySearchElementsExact", helper);
    helper.initFind("/root/h1");
    helper.search();
    QStringList expected;
    expected << "h1" ;
    return checkResults(helper, 1, 1, &expected);
}

bool TestSearch::xquerySearchElementsNotExact()
{
    TestSearchHelper helper(true);
    testAStdSearchWithParamsInit("xquerySearchElementsNotExact", helper);
    helper.initFind("//*[starts-with( name(), 'child')]");
    helper.search();
    QStringList expected;
    expected << "child1" << "child2" << "child2" << "child" << "child" << "child" << "child";
    return checkResults(helper, 6, 6, &expected);
}

bool TestSearch::xquerySearchTextNotExact()
{
    TestSearchHelper helper(true);
    testAStdSearchWithParamsInit("xquerySearchTextNotExact", helper);
    helper.initFind("//*[ contains( text(), 'aaaabc') ]");
    helper.search();
    QStringList expected;
    expected << "b" << "child" ;
    return checkResults(helper, 2, 2, &expected);
}

// CDATA, text, mixed, etc...
bool TestSearch::xquerySearchText()
{
    TestSearchHelper helper(true);
    testAStdSearchWithParamsInit("xquerySearchText", helper);
    helper.initFind("//*[ text() eq 'aaaa' ]");
    helper.search();
    QStringList expected;
    expected << "b" << "child" ;
    return checkResults(helper, 2, 2, &expected);
}

bool TestSearch::xquerySearchTextOnlyCount()
{
    TestSearchHelper helper(true);
    testAStdSearchWithParamsInit("xquerySearchTextOnlyCount", helper);
    helper.initFind("//*[ contains( text(), 'aaaa') ]");
    helper.findArgs.setCountOnly(true);
    helper.search();
    return checkResults(helper, 0, 4, NULL);
}

//--
bool TestSearch::xquerySearchAttributesExactInChildren()
{
    TestSearchHelper helper(true);
    testAStdSearchWithParamsInit("xquerySearchAttributesExactInChildren", helper);
    helper.initFind("//*[@data eq 'www']");
    QList<int> selList;
    selList.append(0); //root
    selList.append(0); //h1
    selList.append(0); //h11
    selList.append(1); //h21
    selList.append(1); //h31
    Element *element = helper.app.mainWindow()->getRegola()->findElementByArray(selList);
    helper.selectedItem = element ;
    if( NULL == element ) {
        return error("No element");
    }
    helper.findArgs.setOnlyChildren(true);
    helper.search();
    QStringList expected;
    expected << "h4" << "h5";
    return checkResults(helper, 2, 2, &expected);
}

bool TestSearch::xquerySearchAttributesNotExactInChildren()
{
    TestSearchHelper helper(true);
    testAStdSearchWithParamsInit("xquerySearchAttributesNotExactInChildren", helper);
    helper.initFind("//*[contains(@data, 'www') eq true()]");
    QList<int> selList;
    selList.append(0); //root
    selList.append(0); //h1
    selList.append(0); //h11
    selList.append(1); //h21
    selList.append(1); //h31
    Element *element = helper.app.mainWindow()->getRegola()->findElementByArray(selList);
    helper.selectedItem = element ;
    if( NULL == element ) {
        return error("No element");
    }
    helper.findArgs.setOnlyChildren(true);
    helper.search();
    QStringList expected;
    expected << "h4" << "h5";
    return checkResults(helper, 2, 2, &expected);
}

bool TestSearch::xquerySearchElementsExactInChildren()
{
    TestSearchHelper helper(true);
    testAStdSearchWithParamsInit("xquerySearchElementsExactInChildren", helper);
    helper.initFind("//child1");
    QList<int> selList;
    selList.append(0); //root
    selList.append(2); //aaaabb
    Element *element = helper.app.mainWindow()->getRegola()->findElementByArray(selList);
    if( NULL == element ) {
        return error("No element");
    }
    helper.selectedItem = element ;
    helper.findArgs.setOnlyChildren(true);
    helper.search();
    QStringList expected;
    expected << "child1" ;
    return checkResults(helper, 1, 1, &expected);
}
bool TestSearch::xquerySearchElementsNotExactInChildren()
{
    TestSearchHelper helper(true);
    testAStdSearchWithParamsInit("xquerySearchElementsNotExactInChildren", helper);
    helper.initFind("//*[starts-with( name(), 'rpr')]");
    QList<int> selList;
    selList.append(0); //root
    selList.append(19); //rpr
    selList.append(0); //rpr
    Element *element = helper.app.mainWindow()->getRegola()->findElementByArray(selList);
    helper.selectedItem = element ;
    if( NULL == element ) {
        return error("No element");
    }
    helper.findArgs.setOnlyChildren(true);
    helper.search();
    QStringList expected;
    expected << "rpr" << "rpr1";
    return checkResults(helper, 2, 2, &expected);
}

bool TestSearch::xquerySearchTextNotExactInChildren()
{

    TestSearchHelper helper(true);
    testAStdSearchWithParamsInit("xquerySearchTextNotExactInChildren", helper);
    helper.initFind("//*[ contains( text(), 'kjj') ]");
    QList<int> selList;
    selList.append(0); //root
    selList.append(20); //rpr1
    selList.append(0); //rpr0
    Element *element = helper.app.mainWindow()->getRegola()->findElementByArray(selList);
    helper.selectedItem = element ;
    if( NULL == element ) {
        return error("No element");
    }
    helper.findArgs.setOnlyChildren(true);
    helper.search();
    QStringList expected;
    expected << "rpr2" << "rpr3" ;
    return checkResults(helper, 2, 2, &expected);
}

bool TestSearch::xquerySearchTextInChildren()
{
    TestSearchHelper helper(true);
    testAStdSearchWithParamsInit("xquerySearchTextInChildren", helper);
    helper.initFind("//*[ text() eq 'kjjz' ]");
    QList<int> selList;
    selList.append(0); //root
    selList.append(20); //rpr1
    selList.append(0); //rpr0
    Element *element = helper.app.mainWindow()->getRegola()->findElementByArray(selList);
    helper.selectedItem = element ;
    if( NULL == element ) {
        return error("No element");
    }
    helper.findArgs.setOnlyChildren(true);
    helper.search();
    QStringList expected;
    expected << "rpr2" ;
    return checkResults(helper, 1, 1, &expected);
}

bool TestSearch::xquerySearchNamespaces(){return true;}

//----------------------------------------------

bool TestSearch::testLiteralSearch()
{
    if(!literalSearchNoItemFound() ){
        return false;
    }

   if(!literalSearchNonExact() ){
        return false;
    }

    if(!literalSearchExact() ){
        return false;
    }

    if(!literalSearchExactInChildren() ){
        return false;
    }

    if(!literalSearchExactInChildrenNoMatch() ){
        return false;
    }

    if(!literalSearchNonExactInChildren() ){
        return false;
    }

    if(!literalSearchCountInChildren() ){
        return false;
    }
    //----------------------------------------
    if(!literalSearchInElementTags() ){
        return false;
    }
    if(!literalSearchInText() ){
        return false;
    }
    if(!literalSearchInAttributes() ){
        return false;
    }
    if(!literalSearchInAttributeNames() ){
        return false;
    }
    if(!literalSearchInScopedPath() ){
        return false;
    }
    if(!literalSearchInScopedPathAttribute() ){
        return false;
    }

    if(!literalSearchInComment()) {
        return false;
    }

    if(!literalSearchInTextBase64()) {
        return false;
    }

    //----------------------------------------
    return true;
}

bool TestSearch::testXQuerySearch()
{
    if( !xquerySearchPathExact()) {
        return false;
    }

    if( !xquerySearchAttributesExact()) {
        return false;
    }
    if( !xquerySearchAttributesNotExact()) {
        return false;
    }
    if( !xquerySearchAttributeNameExact()) {
        return false;
    }
    if( !xquerySearchAttributeNameNotExact()) {
        return false;
    }
    if( !xquerySearchElementsExact()) {
        return false;
    }
    if( !xquerySearchElementsNotExact()) {
        return false;
    }
    if( !xquerySearchTextNotExact()) {
        return false;
    }
    if( !xquerySearchText()) {
        return false;
    }
    if( !xquerySearchTextOnlyCount()) {
        return false;
    }

    if( !xquerySearchNamespaces()) {
        return false;
    }
    //--
    if( !xquerySearchAttributesExactInChildren()) {
        return false;
    }
    if( !xquerySearchAttributesNotExactInChildren()) {
        return false;
    }
    if( !xquerySearchElementsExactInChildren()) {
        return false;
    }
    if( !xquerySearchElementsNotExactInChildren()) {
        return false;
    }
    if( !xquerySearchTextNotExactInChildren()) {
        return false;
    }
    if( !xquerySearchTextInChildren()) {
        return false;
    }
    //---
    return true;
}
