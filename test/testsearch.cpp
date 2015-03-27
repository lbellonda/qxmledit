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

#define FILE_NEXT_ROOT  "../test/data/search/next_root.xml"
#define FILE_NEXT_INNER  "../test/data/search/next_inner.xml"
#define FILE_NEXT_COMPLEX  "../test/data/search/next_complex.xml"


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
    Element *lastMatch;
    bool isXQuery;
    QString fileToLoad;

    TestSearchHelper(const bool isXQuery = false);
    ~TestSearchHelper();
    bool init();
    void selectRoot();
    void selectPath(QList<int> &sel);
    void find(const QString &textToSearch);
    void initFind(const QString &textToSearch);
    void initFindForNext(const QString &textToSearch, const bool isSearchNext, const bool isWrapAround);
    void search();
    void searchLastPos();
    void setFileToLoad(const QString &newFile);
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
    lastMatch = NULL ;
}

TestSearchHelper::~TestSearchHelper()
{

}

bool TestSearchHelper::init()
{
    app.init();
    if(!fileToLoad.isEmpty()) {
        return app.mainWindow()->loadFile(fileToLoad);
    }
    return true ;
}

void TestSearchHelper::selectRoot()
{
    selectedItem = NULL ;
    QTreeWidget *tree = app.mainWindow()->getMainTreeWidget();
    tree->setCurrentItem(app.mainWindow()->getRegola()->root()->getUI());
}

void TestSearchHelper::setFileToLoad(const QString &newFile)
{
    fileToLoad = newFile ;
}

void TestSearchHelper::selectPath(QList<int> &sel)
{
    selectedItem = NULL ;
    if(!sel.isEmpty()) {
        QTreeWidget *tree = app.mainWindow()->getMainTreeWidget();
        Element *foundElm = app.mainWindow()->getRegola()->findElementByArray(sel);
        if( NULL != foundElm ) {
            tree->setCurrentItem(foundElm->getUI());
        }
    }
}

void TestSearchHelper::initSearch(const QString &textToSearch)
{
    selectedItem = NULL ;
    findArgs.init( FindTextParams::FindAllOccurrences, textToSearch, false, true, false, false,
                            false, FindTextParams::FIND_ALL, true,
                  true, true, "", false, isXQuery);
}

void TestSearchHelper::find(const QString &textToSearch)
{
    initSearch(textToSearch);
    app.mainWindow()->getEditor()->setUpdatesEnabled(false);
    lastMatch = app.mainWindow()->getRegola()->findText(findArgs, selectedItem);
    app.mainWindow()->getEditor()->setUpdatesEnabled(true);
}

void TestSearchHelper::initFind(const QString &textToSearch)
{
    initSearch(textToSearch);
}

void TestSearchHelper::initFindForNext(const QString &textToSearch, const bool isSearchNext, const bool isWrapAround)
{
    selectedItem = NULL ;
    findArgs.init( isSearchNext? FindTextParams::FindNext : FindTextParams::FindPrevious,
                   textToSearch, false, true, false, false,
                            false, FindTextParams::FIND_ALL, true,
                   true, true, "", isWrapAround, isXQuery);
}

void TestSearchHelper::search()
{
    app.mainWindow()->getEditor()->getRegola()->clearBookmarks();
    app.mainWindow()->getEditor()->setUpdatesEnabled(false);
    lastMatch = app.mainWindow()->getRegola()->findText(findArgs, selectedItem);
    app.mainWindow()->getEditor()->setUpdatesEnabled(true);
}

void TestSearchHelper::searchLastPos()
{
    app.mainWindow()->getEditor()->getRegola()->clearBookmarks();
    app.mainWindow()->getEditor()->setUpdatesEnabled(false);
    Element *fromHereOn = app.mainWindow()->getEditor()->getSelectedItem();
    lastMatch = app.mainWindow()->getRegola()->findText(findArgs, fromHereOn);
    if( NULL != lastMatch ) {
        app.mainWindow()->getEditor()->setCurrentItem(lastMatch);
    }
    app.mainWindow()->getEditor()->setUpdatesEnabled(true);
}


//---------------------------------------------- LITERAL SEARCH

void TestSearch::init(const QString &nameTest, TestSearchHelper &helper)
{
    _testName = nameTest ;
    helper.init();
    helper.selectRoot();
}

bool TestSearch::testAStdSearchWithParamsInit(const QString &nameTest, TestSearchHelper &helper)
{
    _testName = nameTest ;
    if(!helper.init()) {
        return false;
    }
    helper.selectRoot();
    return true;
}

void TestSearch::initWithParams(const QString &nameTest, TestSearchHelper &helper, QList<int> &selPath)
{
    _testName = nameTest ;
    helper.init();
    helper.selectPath(selPath);
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
        return error(QString("Path List differs from boomarks count. bookmarks count: %1 path count:%2").arg(bmkA.size()).arg(expectedPathList.size()));
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

bool TestSearch::checkSelectionByPath(TestSearchHelper &helper, const int expectedBookmarks, QList<int> &expectedPath, const int index)
{
    if(helper.app.mainWindow()->getRegola()->bookmarkSize()!= expectedBookmarks)
    {
        return error(QString("Index: %1 expected %2 bookmarks match, but found:%3").arg(index).arg(expectedBookmarks).arg(helper.app.mainWindow()->getRegola()->bookmarkSize()));
    }

    Element *sel = helper.app.mainWindow()->getEditor()->getSelectedItem();
    Element *lastMatch = helper.lastMatch ;

    if( ( NULL != lastMatch ) && (sel != lastMatch) ) {
        return error(QString("Index:%1 Last match not null, but was not the selection.").arg(index));
    }
    if( ( NULL == lastMatch ) && !expectedPath.isEmpty() ) {
        return error(QString("Index:%1 Expected selection, but found none path is: %2").arg(index).arg(listIntToString(expectedPath)));
    }
    if( ( NULL != lastMatch ) && expectedPath.isEmpty() ) {
        return error(QString("Index:%1 Expected no selection, but found one, path is: %2, XPath:%3").arg(index).arg(listIntToString(lastMatch->indexPath())).arg(lastMatch->pathString()));
    }
    // compare path
    if( NULL != lastMatch ) {
        QList<int> indexPath = lastMatch->indexPath();
        if(!compareListInts(QString("Index:%1 Match position differs").arg(index), expectedPath, indexPath)){
            return false;
        }
    }
    return true ;
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
    if(!testAStdSearchWithParamsInit(nameTest, helper )) {
        return error("init: testAStdSearchWithParamsInit");
    }
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
    if(!literalSearchNext()) {
        return false;
    }
    if(!literalSearchPrevious()) {
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


//------------------------------------------------------------

bool TestSearch::innerSearchForNext(const QString &testName, const bool isNext)
{
    foreach( bool wrap, boolArray() ) {
        TestSearchHelper helper(false);
        helper.setFileToLoad("");
        QList<int> emptyPath;
        initWithParams(QString("%1.Empty").arg(testName), helper, emptyPath);
        helper.initFindForNext("match", isNext, wrap );
        helper.searchLastPos();
        if(!checkSelectionByPath(helper, 0, emptyPath)) {
            return false;
        }
    }
    return true ;
}

bool TestSearch::innerSearchForRoot(const QString &testName, const bool isNext)
{
    foreach( bool isSel, boolArray() ) {
        foreach( bool wrap, boolArray() ) {
            TestSearchHelper helper(false);
            helper.setFileToLoad(FILE_NEXT_ROOT);
            QList<int> emptyPath;
            QList<int> selPath;
            selPath << 0 ;
            initWithParams(QString("%1.Root").arg(testName), helper, isSel?selPath:emptyPath);
            helper.initFindForNext("match", isNext, wrap );
            helper.searchLastPos();
            if(!checkSelectionByPath(helper, 0, isSel?emptyPath:selPath)) {
                return false;
            }
            helper.initFindForNext("match", isNext, wrap );
            helper.searchLastPos();
            if(!checkSelectionByPath(helper, 0, emptyPath)) {
                return false;
            }
        }
    }
    return true ;
}

bool TestSearch::innerSearchOne(const QString &testName, const bool isNext, QList<int> firstSelPath, QList<QList<int> > selList, const bool isWrap )
{
    TestSearchHelper helper(false);
    helper.setFileToLoad(FILE_NEXT_INNER);
    initWithParams(QString("%1.Inner").arg(testName), helper, firstSelPath);

    int index = 0;
    foreach( QList<int> list, selList ) {
        helper.initFindForNext("match", isNext, isWrap );
        helper.searchLastPos();
        if(!checkSelectionByPath(helper, 0, list, index)) {
            return false;
        }
        index ++ ;
    }
    return true ;
}

bool TestSearch::innerSearchComplex(const QString &testName, const bool isNext, QList<int> firstSelPath, QList<QList<int> > selList, const bool isWrap )
{
    TestSearchHelper helper(false);
    helper.setFileToLoad(FILE_NEXT_COMPLEX);
    initWithParams(QString("%1.Complex").arg(testName), helper, firstSelPath);

    int index = 0;
    foreach( QList<int> list, selList ) {
        helper.initFindForNext("match", isNext, isWrap );
        helper.searchLastPos();
        if(!checkSelectionByPath(helper, 0, list, index)) {
            return false;
        }
        index ++ ;
    }
    return true ;
}

bool TestSearch::innerSearchForInner(const QString &testName, const bool isNext)
{
    // 0 - no selection 1 match, stop
    // 1 - select before, 1 match stop
    // 2 - select element, no match
    // 3 - select next, 1 match only with wraparound
    QList<int> emptyPath;
    //0
    {
        QList<int> oneSel;
        oneSel << 0 << 1 ;

        QList<QList<int> > selList;
        selList << oneSel ;
        selList << emptyPath ;
        selList << emptyPath ;

        if(! innerSearchOne(testName, isNext, emptyPath, selList, false ) ) {
            return false;
        }
        if(! innerSearchOne(testName, isNext, emptyPath, selList, true) ) {
            return false;
        }
    }
    //1
    {
        QList<int> firstSelPath;
        firstSelPath << 0 << 0 ;

        QList<int> oneSel;
        oneSel << 0 << 1 ;

        QList<QList<int> > selList;
        selList << oneSel ;
        selList << emptyPath ;
        selList << emptyPath ;

        if(! innerSearchOne(QString("0:%1").arg(testName), isNext, firstSelPath, selList, false ) ) {
            return false;
        }
        if(! innerSearchOne(QString("1:%1").arg(testName), isNext, firstSelPath, selList, true) ) {
            return false;
        }
    }
    //2
    {
        QList<int> firstSelPath;
        firstSelPath << 0 << 1 ;

        QList<QList<int> > selList;
        selList << emptyPath ;
        selList << emptyPath ;
        selList << emptyPath ;

        if(! innerSearchOne(QString("2:%1").arg(testName), isNext, firstSelPath, selList, false ) ) {
            return false;
        }
        if(! innerSearchOne(QString("3:%1").arg(testName), isNext, firstSelPath, selList, true) ) {
            return false;
        }
    }
    //3
    {
        QList<int> firstSelPath;
        firstSelPath << 0 << 2 ;

        QList<int> oneSel;
        oneSel << 0 << 1 ;
        {
            QList<QList<int> > selList;
            selList << emptyPath ;
            selList << emptyPath ;
            selList << emptyPath ;

            if(! innerSearchOne(QString("4:%1").arg(testName), isNext, firstSelPath, selList, false ) ) {
                return false;
            }
        }
        {
            QList<QList<int> > selList;
            selList << oneSel ;
            selList << emptyPath ;
            selList << emptyPath ;
            if(! innerSearchOne(QString("5:%1").arg(testName), isNext, firstSelPath, selList, true) ) {
                return false;
            }
        }
    }
    return true ;
}

bool TestSearch::innerSearchForInnerReverse(const QString &testName)
{
    // 0 - no selection 1 match, stop
    // 1 - select before, 1 match stop
    // 2 - select element, no match
    // 3 - select next, 1 match only with wraparound
    QList<int> emptyPath;
    //0
    {
        QList<int> oneSel;
        oneSel << 0 << 1 ;

        QList<QList<int> > selList;
        selList << oneSel ;
        selList << emptyPath ;
        selList << emptyPath ;

        if(! innerSearchOne(QString("a:%1").arg(testName), false, emptyPath, selList, false ) ) {
            return false;
        }
        if(! innerSearchOne(QString("b:%1").arg(testName), false, emptyPath, selList, true) ) {
            return false;
        }
    }
    //1
    {
        QList<int> firstSelPath;
        firstSelPath << 0 << 2 ;

        QList<int> oneSel;
        oneSel << 0 << 1 ;

        QList<QList<int> > selList;
        selList << oneSel ;
        selList << emptyPath ;
        selList << emptyPath ;

        if(! innerSearchOne(QString("0:%1").arg(testName), false, firstSelPath, selList, false ) ) {
            return false;
        }
        if(! innerSearchOne(QString("1:%1").arg(testName), false, firstSelPath, selList, true) ) {
            return false;
        }
    }
    //2
    {
        QList<int> firstSelPath;
        firstSelPath << 0 << 1 ;

        QList<QList<int> > selList;
        selList << emptyPath ;
        selList << emptyPath ;
        selList << emptyPath ;

        if(! innerSearchOne(QString("2:%1").arg(testName), false, firstSelPath, selList, false ) ) {
            return false;
        }
        if(! innerSearchOne(QString("3:%1").arg(testName), false, firstSelPath, selList, true) ) {
            return false;
        }
    }
    //3
    {
        QList<int> firstSelPath;
        firstSelPath << 0 << 2 ;

        QList<int> oneSel;
        oneSel << 0 << 1 ;
        {
            QList<QList<int> > selList;
            selList << oneSel ;
            selList << emptyPath ;
            selList << emptyPath ;

            if(! innerSearchOne(QString("4:%1").arg(testName), false, firstSelPath, selList, false ) ) {
                return false;
            }
        }
        {
            QList<QList<int> > selList;
            selList << oneSel ;
            selList << emptyPath ;
            selList << emptyPath ;
            if(! innerSearchOne(QString("5:%1").arg(testName), false, firstSelPath, selList, true) ) {
                return false;
            }
        }
    }
    return true ;
}


/*bool TestSearch::a(const QString &testName, const bool isNext)
{
    // 0 - no selection 1 match, stop
    // 1 - select before, 1 match stop
    // 2 - select element, no match
    // 3 - select next, 1 match only with wraparound
    QList<int> emptyPath;
    //3
    {
        QList<int> firstSelPath;
        firstSelPath << 0 << 2 ;

        QList<int> oneSel;
        oneSel << 0 << 1 ;
        {
            QList<QList<int> > selList;
            selList << oneSel ;
            selList << emptyPath ;
            selList << emptyPath ;
            if(! innerSearchOne(QString("5:%1").arg(testName), isNext, firstSelPath, selList, true) ) {
                return false;
            }
        }
    }
    return true ;
}*/


/**
  Tests:
*/
bool TestSearch::innerSearchForNextComplex(const QString &testName, const bool isNext)
{
    QList<int> emptyPath;
    //0 - no selection 1 match, stop
    {
        {
            QList<QList<int> > selList;
            QList<int> oneSel;
            oneSel << 0 << 0 << 1 ;
            QList<int> twoSel;
            twoSel << 0 << 1 << 0 << 0 << 0;
            QList<int> threeSel;
            threeSel << 0 << 2 << 0 ;
            selList << oneSel ;
            selList << twoSel ;
            selList << threeSel ;
            selList << emptyPath ;
            selList << emptyPath ;

            if(!innerSearchComplex(testName, isNext, emptyPath, selList, false ) ) {
                return false ;
            }
        }

        {
            QList<QList<int> > selList;
            QList<int> oneSel;
            oneSel << 0 << 0 << 1 ;
            QList<int> twoSel;
            twoSel << 0 << 1 << 0 << 0 << 0;
            QList<int> threeSel;
            threeSel << 0 << 2 << 0 ;
            selList << oneSel ;
            selList << twoSel ;
            selList << threeSel ;
            selList << oneSel ;
            selList << twoSel ;
            selList << threeSel ;

            if(!innerSearchComplex(testName, isNext, emptyPath, selList, true ) ) {
                return false ;
            }
        }

        //1 - first selection 2 matches, stop
        {

            QList<int> oneSel;
            oneSel << 0 << 0 << 1 ;
            {
                QList<QList<int> > selList;
                QList<int> twoSel;
                twoSel << 0 << 1 << 0 << 0 << 0;
                QList<int> threeSel;
                threeSel << 0 << 2 << 0 ;
                selList << twoSel ;
                selList << threeSel ;
                selList << emptyPath ;
                selList << emptyPath ;

                if(!innerSearchComplex(testName, isNext, oneSel, selList, false ) ) {
                    return false ;
                }
            }
            {
                QList<QList<int> > selList;
                QList<int> twoSel;
                twoSel << 0 << 1 << 0 << 0 << 0;
                QList<int> threeSel;
                threeSel << 0 << 2 << 0 ;
                selList << twoSel ;
                selList << threeSel ;
                selList << oneSel ;
                selList << twoSel ;
                selList << threeSel ;

                if(!innerSearchComplex(testName, isNext, oneSel, selList, true ) ) {
                    return false ;
                }
            }
        }
        // sel item 2
        {
            QList<int> selPath;
            selPath << 0 << 1 << 0 << 0 << 0 ;
            {
                QList<QList<int> > selList;
                QList<int> lastSel;
                lastSel << 0 << 2 << 0 ;
                selList << lastSel ;
                selList << emptyPath ;
                selList << emptyPath ;

                if(!innerSearchComplex(testName, isNext, selPath, selList, false ) ) {
                    return false ;
                }
            }

            {
                QList<QList<int> > selList;
                QList<int> oneSel;
                oneSel << 0 << 0 << 1 ;
                QList<int> twoSel;
                twoSel << 0 << 1 << 0 << 0 << 0;
                QList<int> threeSel;
                threeSel << 0 << 2 << 0 ;
                selList << threeSel ;
                selList << oneSel ;
                selList << twoSel ;
                selList << threeSel ;
                if(!innerSearchComplex(testName, isNext, selPath, selList, true ) ) {
                    return false ;
                }
            }
        }

        // sel no match
        {
            QList<int> oneSel;
            oneSel << 0 << 0 << 1 ;
            QList<int> twoSel;
            twoSel << 0 << 1 << 0 << 0 << 0;
            QList<int> threeSel;
            threeSel << 0 << 2 << 0 ;
            QList<int> selPath;
            selPath << 0 << 0 << 1 ;
            {
                QList<QList<int> > selList;
                QList<int> lastSel;
                lastSel << 0 << 2 << 0 ;
                selList << twoSel ;
                selList << threeSel ;
                selList << emptyPath ;
                selList << emptyPath ;
                if(!innerSearchComplex(testName, isNext, selPath, selList, false ) ) {
                    return false ;
                }
            }

            {
                QList<QList<int> > selList;

                selList << twoSel ;
                selList << threeSel ;
                selList << oneSel ;
                selList << twoSel ;
                selList << threeSel ;
                if(!innerSearchComplex(testName, isNext, selPath, selList, true ) ) {
                    return false ;
                }
            }
        }
    }

    return true ;
}


bool TestSearch::innerSearchForNextComplexReverse(const QString &testName)
{
    QList<int> emptyPath;
    QList<int> oneSel;
    oneSel << 0 << 0 << 1 ;
    QList<int> twoSel;
    twoSel << 0 << 1 << 0 << 0 << 0;
    QList<int> threeSel;
    threeSel << 0 << 2 << 0 ;
    //0 - no selection 1 match, stop
    {
        QList<QList<int> > selList;
        selList << threeSel ;
        selList << twoSel ;
        selList << oneSel ;
        selList << emptyPath ;
        selList << emptyPath ;

        if(!innerSearchComplex(QString("0:%1").arg(testName), false, emptyPath, selList, false ) ) {
            return false ;
        }
    }

    {
        QList<QList<int> > selList;
        selList << threeSel ;
        selList << twoSel ;
        selList << oneSel ;
        selList << threeSel ;
        selList << twoSel ;
        selList << oneSel ;

        if(!innerSearchComplex(QString("1:%1").arg(testName), false, emptyPath, selList, true ) ) {
            return false ;
        }
    }

    //1 - first selection 2 matches, stop
    {
        QList<QList<int> > selList;
        selList << twoSel ;
        selList << oneSel ;
        selList << emptyPath ;
        selList << emptyPath ;

        if(!innerSearchComplex(QString("2:%1").arg(testName), false, threeSel, selList, false ) ) {
            return false ;
        }
    }
    {
        QList<QList<int> > selList;
        selList << twoSel ;
        selList << oneSel ;
        selList << threeSel ;
        selList << twoSel ;
        selList << oneSel ;
        selList << threeSel ;

        if(!innerSearchComplex(QString("3:%1").arg(testName), false, threeSel, selList, true ) ) {
            return false ;
        }
    }
    // sel item 2
    {
        QList<QList<int> > selList;
        selList << oneSel ;
        selList << emptyPath ;
        selList << emptyPath ;

        if(!innerSearchComplex(QString("4:%1").arg(testName), false, twoSel, selList, false ) ) {
            return false ;
        }
    }

    {
        QList<QList<int> > selList;
        selList << oneSel ;
        selList << threeSel ;
        selList << twoSel ;
        selList << oneSel ;
        selList << threeSel ;
        selList << twoSel ;
        if(!innerSearchComplex(QString("5:%1").arg(testName), false, twoSel, selList, true ) ) {
            return false ;
        }
    }

    // sel no match
    {
        QList<int> selPath;
        selPath << 0 << 1 << 0 ;
        {
            QList<QList<int> > selList;
            selList << oneSel ;
            selList << emptyPath ;
            selList << emptyPath ;
            if(!innerSearchComplex(QString("6:%1").arg(testName), false, selPath, selList, false ) ) {
                return false ;
            }
        }

        {
            QList<QList<int> > selList;
            selList << oneSel ;
            selList << threeSel ;
            selList << twoSel ;
            selList << oneSel ;
            selList << threeSel ;
            selList << twoSel ;
            if(!innerSearchComplex(QString("7:%1").arg(testName), false, selPath, selList, true ) ) {
                return false ;
            }
        }
    }
    return true ;
}

/*
*/

/*
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
*/

bool TestSearch::literalSearchNext()
{
    _testName = "literalSearchNext";
    QString testName = "literalSearchNext";

    if(!innerSearchForNext(testName, true) ) {
        return false;
    }
    if(!innerSearchForRoot(testName, true) ) {
        return false;
    }
    if(!innerSearchForInner(testName, true) ) {
        return false;
    }
    if(!innerSearchForNextComplex(testName, true) ) {
        return false;
    }
    return true;
}

bool TestSearch::literalSearchPrevious()
{
    _testName = "literalSearchPrevious";
    QString testName = "literalSearchPrevious";
    if(!innerSearchForNext(testName, false) ) {
        return false;
    }
    if(!innerSearchForRoot(testName, false) ) {
        return false;
    }
    if(!innerSearchForInnerReverse(testName) ) {
        return false;
    }
    if(!innerSearchForNextComplexReverse(testName) ) {
        return false;
    }
    return true;
}

