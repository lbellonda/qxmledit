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


#include "testreplace.h"
#include "app.h"
#include "comparexml.h"
#include "modules/search/searchmanager.h"

/**
  Tests have this characteristic:
  1- Scope (all/children of the selcted element)
  2- Match (all/partial)
  3- Scope (element/all/attributes)
  4- Path (XPath of a particular element)
  5- Errors (element tags, attributes tags, comments)
  **/

#define REPL_ALL_ELM_EXACT_START    "../test/data/search/replace/all_elm_exact_start.xml"
#define REPL_ALL_ELM_EXACT_END    "../test/data/search/replace/all_elm_exact_end.xml"
#define REPL_ALL_ELM_EXACT_START_CASE    "../test/data/search/replace/all_elm_exact_case_start.xml"
#define REPL_ALL_ELM_EXACT_END_CASE    "../test/data/search/replace/all_elm_exact_case_end.xml"

#define REPL_ALL_ELM_PARTIAL_END    "../test/data/search/replace/all_elm_partial_end.xml"
#define REPL_ALL_ELM_PARTIAL_END_CASE    "../test/data/search/replace/all_elm_partial_case_end.xml"
#define REPL_SINGLE_ELM_EXACT_END    "../test/data/search/replace/single_elm_exact_end.xml"
#define REPL_ALL_ATTR_PARTIAL_START "../test/data/search/replace/all_attr_partial_start.xml"
#define REPL_SINGLE_ATTR_PARTIAL_END    "../test/data/search/replace/all_attr_partial_end.xml"

#define REPL_ALL_ATTR_PARTIAL_VALUE_END   "../test/data/search/replace/all_attr_partial_value_end.xml"
#define REPL_ALL_ATTR_PARTIAL_VALUE_CASE_START    "../test/data/search/replace/all_attr_partial_value_case_start.xml"
#define REPL_ALL_ATTR_PARTIAL_VALUE_CASE_END      "../test/data/search/replace/all_attr_partial_value_case_end.xml"
#define REPL_ALL_ELM_PI_START  "../test/data/search/replace/all_pi_start.xml"
#define REPL_ALL_ELM_PI_END  "../test/data/search/replace/all_pi_end.xml"

#define REPL_ALL_TEXTELM_PARTIAL_START  "../test/data/search/replace/all_elm_text_partial_start.xml"
#define REPL_ALL_TEXTELM_PARTIAL_END    "../test/data/search/replace/all_elm_text_partial_end.xml"

#define REPL_ALL_TEXT_INLINE_PARTIAL_START  "../test/data/search/replace/all_text_inline_partial_start.xml"
#define REPL_ALL_TEXT_INLINE_PARTIAL_END    "../test/data/search/replace/all_text_inline_partial_end.xml"
#define REPL_ALL_TEXT_CDATA_PARTIAL_START  "../test/data/search/replace/all_text_cdata_partial_start.xml"
#define REPL_ALL_TEXT_CDATA_PARTIAL_END    "../test/data/search/replace/all_text_cdata_partial_end.xml"
#define REPL_ALL_COMMENT_END    "../test/data/search/replace/all_comment_end.xml"

//-----------------------

class TestReplaceHelper {

    void initSearch(const QString &textToSearch);

public:
    ReplaceTextParams findArgs;
    QList<int> selection;
    int expectedErrorCount ;
    int expectedReplacementCount;

    TestReplaceHelper();
    ~TestReplaceHelper();
    bool initReplace(const QString &search, const QString &replace, const FindTextParams::EFindTarget op, const bool isExact);
    void setCaseOn();

};

TestReplaceHelper::TestReplaceHelper()
{
    expectedErrorCount = 0 ;
    expectedReplacementCount = 0 ;
}

TestReplaceHelper::~TestReplaceHelper()
{
    //
}

bool TestReplaceHelper::initReplace(const QString &search, const QString &replace, const FindTextParams::EFindTarget op, const bool isExact)
{
    findArgs.setMatchExact(isExact);
    findArgs.setScope(op);
    findArgs.setSearchText(search);
    findArgs.setReplaceText(replace);
    findArgs.setCaseSensitive(false);
    return true;
}

void TestReplaceHelper::setCaseOn()
{
    findArgs.setCaseSensitive(true);
}

//-----------------------

TestReplace::TestReplace()
{
}

TestReplace::~TestReplace()
{

}

//----------------------------------

bool TestReplace::testFast()
{
    _testName = "testFast";
    if(!testReplaceProcessingInstructions()) {
        return false;
    }
    return true;
}

/**
 * @brief TestReplace::testBaseReplace
 * @return
 *
 * test: replace all
 *   11 elements
 *   12 text
 *   13 comments
 *   14 attribute names
 *   15 attribute values
 *   16 all
 *
 *  test with errors.
 *
 * test: replace next, with a replace and a skip
 * next
 * root
 * inner
 * complete
 *
 * all reversed
 * all checking errors
 */
bool TestReplace::testBaseReplace()
{
    _testName = "testBaseReplace";
    if(!testReplaceAll()) {
        return false;
    }
    if(!testReplaceNext()) {
        return false;
    }
    if(!testReplacePrevious()) {
        return false;
    }
    return true;
}

bool TestReplace::testUnitReplace()
{
    _testName = "testUnitReplace";
    if(!testUnitReplaceString()) {
        return false;
    }
    return true;
}

bool TestReplace::testReplaceAll()
{
    if(!testReplaceElements()) {
        return false;
    }
    if(!testReplaceAttributes()) {
        return false;
    }
    if(!testReplaceText()) {
        return false;
    }
    if(!testReplaceComments()) {
        return false;
    }
    if(!testReplaceProcessingInstructions()) {
        return false;
    }
    return true;
}

bool TestReplace::testReplaceElements()
{
    if(!testReplaceAllElementsExact()) {
        return false;
    }
    if(!testReplaceAllElementsExactError()) {
        return false;
    }
    if(!testReplaceAllElementsExactCase()) {
        return false;
    }
    if(!testReplaceAllElementsPartial()) {
        return false;
    }
    if(!testReplaceAllElementsPartialError()) {
        return false;
    }
    if(!testReplaceAllElementsPartialCase()) {
        return false;
    }
    if(!testReplaceChildrenElements()) {
        return false;
    }
    if(!testReplaceChildrenElementsPartialError()) {
        return false;
    }
    if(!testReplaceChildrenElementsPartialCase()) {
        return false;
    }

    return true ;
}

//----------------------------------
bool TestReplace::replaceAStringPartialCase(const QString &source, const QString &search, const QString &replacement, const QString &expected)
{
    return replaceAString(source, search, replacement, expected, true, true);
}

bool TestReplace::replaceAStringPartialNoCase(const QString &source, const QString &search, const QString &replacement, const QString &expected)
{
    return replaceAString(source, search, replacement, expected, true, false);
}

bool TestReplace::replaceAString(const QString &source, const QString &search, const QString &replacement, const QString &expected, const bool usePartialMatch, const bool isCaseSensitive)
{
    ReplaceTextParams params;
    params.setSearchText(search);
    params.setReplaceText(replacement);
    params.setMatchExact(!usePartialMatch);
    params.setCaseSensitive(isCaseSensitive);
    QString result = params.applyReplacement(source);
    if( result != expected ) {
        return error(QString("Repl failed: expected:'%1', found:'%2', params( search:'%3', repl:'%4', partialMatch:%5, case sensitive:%6)")
                     .arg(expected).arg(result).arg(search).arg(replacement).arg(usePartialMatch).arg(isCaseSensitive));
    }
    return true;
}

bool TestReplace::testUnitReplaceString()
{
    _testName = "testUnitReplaceString";

    // Remember that the replacement is all or partial and a match has already been calculated.
    // this replacement operation is a blind substitution. The search test cases check for the
    // match logic.

    // single whole word no case
    if(!replaceAString("abc", "abc", "123", "123", false, false)) {
        return false;
    }
    // single whole word no case, wrong
    if(!replaceAString("ABC", "abc", "123", "123", false, false)) {
        return false;
    }
    // single whole word case
    if(!replaceAString("abc", "abc", "123", "123", false, true)) {
        return false;
    }
    // single whole word case, wrong
    if(!replaceAString("ABC", "abc", "123", "123", false, true)) {
        return false;
    }
    //----
    // partial
    if(!replaceAString("abc", "abc", "123", "123", true, false)) {
        return false;
    }
    // partial word no case, wrong
    if(!replaceAString("ABC", "abc", "123", "123", true, false)) {
        return false;
    }
    // partial whole word case
    if(!replaceAString("abc", "abc", "123", "123", true, true)) {
        return false;
    }
    // partial whole word case, wrong
    if(!replaceAString("ABC", "abc", "123", "ABC", true, true)) {
        return false;
    }
    //------
    // end of string, whole, case ok
    if(!replaceAString("cba abc", "abc", "123", "123", false, true)) {
        return false;
    }
    // end of string, whole, case nok
    if(!replaceAString("cba abc", "ABC", "123", "123", false, true)) {
        return false;
    }
    // same, but without case
    if(!replaceAString("cba abc", "abc", "123", "123", false, false)) {
        return false;
    }
    // continue
    if(!replaceAString("cba abc", "ABC", "123", "123", false, false)) {
        return false;
    }
    // end of string, partial, case ok
    if(!replaceAString("cba abc", "abc", "123", "cba 123", true, true)) {
        return false;
    }
    // end of string, partial, case nok
    if(!replaceAString("cba abc", "ABC", "123", "cba abc", true, true)) {
        return false;
    }
    // same, but without case
    if(!replaceAString("cba abc", "abc", "123", "cba 123", true, false)) {
        return false;
    }
    // continue
    if(!replaceAString("cba abc", "ABC", "123", "cba 123", true, false)) {
        return false;
    }

    // single end string, word partial, case, final space
    if(!replaceAString("cba ABC", "abc", "123", "cba ABC", true, true)) {
        return false;
    }
    if(!replaceAString("cba ABC ", "abc", "123", "cba 123 ", true, false)) {
        return false;
    }
    // multiple matches in partial mode
    if(!replaceAStringPartialNoCase("cbaabc ", "abc", "123", "cba123 ")) {
        return false;
    }
    if(!replaceAStringPartialCase("cbaabc ", "abc", "123", "cba123 " )) {
        return false;
    }
    if(!replaceAStringPartialNoCase("cbaabc ", "ABC", "123", "cba123 ")) {
        return false;
    }
    if(!replaceAStringPartialCase("cbaabc ", "ABC", "123", "cbaabc ")) {
        return false;
    }
    //----
    // single mid string, word
    if(!replaceAStringPartialNoCase("cba abc twa", "abc", "123", "cba 123 twa")) {
        return false;
    }
    if(!replaceAStringPartialNoCase("cba abc twa", "ABC", "123", "cba 123 twa")) {
        return false;
    }
    if(!replaceAStringPartialCase("cba abc twa", "abc", "123", "cba 123 twa")) {
        return false;
    }
    if(!replaceAStringPartialCase("cba abc twa", "ABC", "123", "cba abc twa")) {
        return false;
    }
    // single mid string, word partial
    if(!replaceAStringPartialNoCase("cbaabctwa", "abc", "123", "cba123twa")) {
        return false;
    }
    if(!replaceAStringPartialNoCase("cbaabctwa", "ABC", "123", "cba123twa")) {
        return false;
    }
    if(!replaceAStringPartialCase("cbaabctwa", "abc", "123", "cba123twa")) {
        return false;
    }
    if(!replaceAStringPartialCase("cbaabctwa", "ABC", "123", "cbaabctwa")) {
        return false;
    }

    // repeated partial
    if(!replaceAStringPartialNoCase("abcabcabc", "abc", "123", "123123123")) {
        return false;
    }
    if(!replaceAStringPartialNoCase("abcabcabc", "ABC", "123", "123123123")) {
        return false;
    }
    if(!replaceAStringPartialCase("abcabcabc", "abc", "123", "123123123")) {
        return false;
    }
    if(!replaceAStringPartialCase("abcabcabc", "ABC", "123", "abcabcabc")) {
        return false;
    }
    //
    return true;
}

//----------------------------------

bool TestReplace::cfr(Regola *regola, const QString &step, const QString &fileResult)
{
    QByteArray resultData = regola->writeMemory();
    QDomDocument document1;
    QDomDocument document2;
    CompareXML compare;
    if(!compare.loadFileIntoDocument(fileResult, document1)) {
        return error(QString("step: %1, load file result %2").arg(step).arg(fileResult));
    }
    QBuffer outputData(&resultData);
    if(!compare.loadFileIntoDocument(&outputData, document2)) {
        return error(QString("step %1 load modified data").arg(step));
    }
    bool result = compare.compareDomDocuments(document1, document2);
    if( !result ) {
        compare.dumpErrorCause();
        return error(QString("Step: %1 comparing file with doc: %2").arg(step).arg(compare.errorString()));
    }
    return true ;
}

bool TestReplace::testSkeleton(const QString &fileStart, const QString &fileResult,
                                         bool (TestReplace::*functPtr)(App *appData, TestReplaceHelper &helper),
                                         TestReplaceHelper &helper )
{
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(fileStart) ) {
        return error(QString("unable to load input file: '%1' ").arg(fileStart));
    }
    Regola *regola = app.mainWindow()->getRegola();
    Element *selectedElement = NULL ;
    if(helper.selection.isEmpty()) {
        selectedElement = regola->root();
    } else {
        selectedElement = app.mainWindow()->getRegola()->findElementByArray(helper.selection);
        helper.findArgs.setOnlyChildren(true);
    }
    if(NULL == selectedElement) {
        return error("no element selected");
    }
    app.mainWindow()->getEditor()->setCurrentItem(selectedElement);
    if(!(this->*functPtr)(&app, helper)) {
        return false;
    }
    if(!cfr(regola, "operation", fileResult)){
        return false;
    }
    regola->undo();
    if(!cfr(regola, "undo", fileStart)){
        return false;
    }
    regola->redo();
    if(!cfr(regola, "redo", fileResult)){
        return false;
    }
    return true;
}

bool TestReplace::initHelper(const QString &name, TestReplaceHelper &helper, const int expectedErrorsCount, const int expectedReplacementCount)
{
    _testName = name ;
    helper.expectedReplacementCount = expectedReplacementCount ;
    helper.expectedErrorCount = expectedErrorsCount ;
    return true ;
}

bool TestReplace::funOneStep(App *appData, TestReplaceHelper &helper)
{
    appData->mainWindow()->getEditor()->replaceAll(&helper.findArgs);
    if(helper.expectedReplacementCount != helper.findArgs.replacementCount() ) {
        return error(QString("expected %1 replacements, but performed %2").arg(helper.expectedReplacementCount).arg(helper.findArgs.replacementCount()));
    }
    //check for expected errors
    //
    if( helper.expectedErrorCount != helper.findArgs.replacementErrorsCount() ) {
        return error(QString("Expected %1 errors, found:%2").arg(helper.expectedErrorCount).arg(helper.findArgs.replacementErrorsCount()));
    }
    return true ;
}

bool TestReplace::testReplaceAllElementsExact()
{
    TestReplaceHelper helper;
    if(!initHelper("testReplaceAllElementsExact", helper, 0, 2 ) ) {
        return false;
    }
    helper.initReplace("abc", "ppp", FindTextParams::FIND_TAG, true);
    if(!testSkeleton(REPL_ALL_ELM_EXACT_START, REPL_ALL_ELM_EXACT_END, &TestReplace::funOneStep, helper ))
    {
        return false;
    }
    return true;
}

bool TestReplace::testReplaceAllElementsExactError()
{
    TestReplaceHelper helper;
    if(!initHelper("testReplaceAllElementsExactError", helper, 2, 0 ) ) {
        return false;
    }
    helper.initReplace("abc", "---", FindTextParams::FIND_TAG, true);
    if(!testSkeleton(REPL_ALL_ELM_EXACT_START, REPL_ALL_ELM_EXACT_START, &TestReplace::funOneStep, helper ))
    {
        return false;
    }
    return true;
}

bool TestReplace::testReplaceAllElementsExactCase()
{
    TestReplaceHelper helper;
    if(!initHelper("testReplaceAllElementsExactCase", helper, 0, 1 ) ) {
        return false;
    }
    helper.initReplace("abc", "ppp", FindTextParams::FIND_TAG, true);
    helper.setCaseOn();
    if(!testSkeleton(REPL_ALL_ELM_EXACT_START_CASE, REPL_ALL_ELM_EXACT_END_CASE, &TestReplace::funOneStep, helper ))
    {
        return false;
    }
    return true;
}

//----------------------------------

bool TestReplace::testReplaceAllElementsPartial()
{
    TestReplaceHelper helper;
    if(!initHelper("testReplaceAllElementsPartial", helper, 0, 3 ) ) {
        return false;
    }
    helper.initReplace("ab", "ppp", FindTextParams::FIND_TAG, false);
    if(!testSkeleton(REPL_ALL_ELM_EXACT_START, REPL_ALL_ELM_PARTIAL_END, &TestReplace::funOneStep, helper ))
    {
        return false;
    }
    return true;
}

bool TestReplace::testReplaceAllElementsPartialError()
{
    TestReplaceHelper helper;
    if(!initHelper("testReplaceAllElementsPartialError", helper, 3, 0 ) ) {
        return false;
    }
    helper.initReplace("ab", "---", FindTextParams::FIND_TAG, false);
    if(!testSkeleton(REPL_ALL_ELM_EXACT_START, REPL_ALL_ELM_EXACT_START, &TestReplace::funOneStep, helper ))
    {
        return false;
    }
    return true;
}

bool TestReplace::testReplaceAllElementsPartialCase()
{
    TestReplaceHelper helper;
    if(!initHelper("testReplaceAllElementsPartialCase", helper, 0, 1 ) ) {
        return false;
    }
    helper.initReplace("AB", "ppp", FindTextParams::FIND_TAG, false);
    helper.setCaseOn();
    if(!testSkeleton(REPL_ALL_ELM_EXACT_START_CASE, REPL_ALL_ELM_PARTIAL_END_CASE, &TestReplace::funOneStep, helper ))
    {
        return false;
    }
    return true;
}

//----------------------------------

bool TestReplace::testReplaceChildrenElements()
{
    TestReplaceHelper helper;
    if(!initHelper("testReplaceChildrenElements", helper, 0, 1 ) ) {
        return false;
    }
    helper.initReplace("abc", "ppp", FindTextParams::FIND_TAG, true);
    helper.selection << 1 << 0 ;
    if(!testSkeleton(REPL_ALL_ELM_EXACT_START, REPL_SINGLE_ELM_EXACT_END, &TestReplace::funOneStep, helper ))
    {
        return false;
    }
    return true;
}

bool TestReplace::testReplaceChildrenElementsPartialError()
{
    TestReplaceHelper helper;
    if(!initHelper("testReplaceChildrenElementsPartialError", helper, 1, 0 ) ) {
        return false;
    }
    helper.initReplace("abc", "---", FindTextParams::FIND_TAG, true);
    helper.selection << 1 << 0 ;
    if(!testSkeleton(REPL_ALL_ELM_EXACT_START, REPL_ALL_ELM_EXACT_START, &TestReplace::funOneStep, helper ))
    {
        return false;
    }
    return true;
}

bool TestReplace::testReplaceChildrenElementsPartialCase()
{
    TestReplaceHelper helper;
    if(!initHelper("testReplaceChildrenElementsPartialCase", helper, 0, 0 ) ) {
        return false;
    }
    helper.initReplace("ABC", "ppp", FindTextParams::FIND_TAG, false);
    helper.setCaseOn();
    helper.selection << 1 << 0 ;
    if(!testSkeleton(REPL_ALL_ELM_EXACT_START, REPL_ALL_ELM_EXACT_START, &TestReplace::funOneStep, helper ))
    {
        return false;
    }
    return true;
}

//----------------------------------
bool TestReplace::testReplaceAttributesPartial()
{
    TestReplaceHelper helper;
    if(!initHelper("testReplaceAttributesPartial", helper, 0, 4 ) ) {
        return false;
    }
    helper.initReplace("abc", "ppp", FindTextParams::FIND_ATTRIBUTE_NAME, false);
    helper.selection << 1 << 0 ;
    if(!testSkeleton(REPL_ALL_ATTR_PARTIAL_START, REPL_SINGLE_ATTR_PARTIAL_END, &TestReplace::funOneStep, helper ))
    {
        return false;
    }
    return true;
}

bool TestReplace::testReplaceAttributesPartialError()
{
    TestReplaceHelper helper;
    if(!initHelper("testReplaceAttributesPartialError", helper, 2, 0 ) ) {
        return false;
    }
    helper.initReplace("abc", "---", FindTextParams::FIND_ATTRIBUTE_NAME, true);
    helper.selection << 1 << 0 ;
    if(!testSkeleton(REPL_ALL_ELM_EXACT_START, REPL_ALL_ELM_EXACT_START, &TestReplace::funOneStep, helper ))
    {
        return false;
    }
    return true;
}

bool TestReplace::testReplaceAttributesPartialCase()
{
    TestReplaceHelper helper;
    if(!initHelper("testReplaceAttributesPartialCase", helper, 0, 0 ) ) {
        return false;
    }
    helper.initReplace("ABC", "ppp", FindTextParams::FIND_ATTRIBUTE_NAME, true);
    helper.setCaseOn();
    helper.selection << 1 << 0 ;
    if(!testSkeleton(REPL_ALL_ELM_EXACT_START_CASE, REPL_ALL_ELM_EXACT_START_CASE, &TestReplace::funOneStep, helper ))
    {
        return false;
    }
    return true;
}
//----------------------------------
bool TestReplace::testReplaceAttributesValuesPartial()
{
    TestReplaceHelper helper;
    if(!initHelper("testReplaceAttributesValuesPartial", helper, 0, 4 ) ) {
        return false;
    }
    helper.initReplace("abc", "ppp", FindTextParams::FIND_ATTRIBUTE_VALUE, false);
    helper.selection << 1 << 0 ;
    if(!testSkeleton(REPL_ALL_ATTR_PARTIAL_START, REPL_ALL_ATTR_PARTIAL_VALUE_END, &TestReplace::funOneStep, helper ))
    {
        return false;
    }
    return true;
}

bool TestReplace::testReplaceAttributesValuesPartialCase()
{
    TestReplaceHelper helper;
    if(!initHelper("testReplaceAttributesValuesPartialCase", helper, 0, 1 ) ) {
        return false;
    }
    helper.initReplace("ABC", "ppp", FindTextParams::FIND_ATTRIBUTE_VALUE, true);
    helper.setCaseOn();
    helper.selection << 1 << 0 ;
    if(!testSkeleton(REPL_ALL_ATTR_PARTIAL_VALUE_CASE_START, REPL_ALL_ATTR_PARTIAL_VALUE_CASE_END, &TestReplace::funOneStep, helper ))
    {
        return false;
    }
    return true;
}
//----------------------------------
bool TestReplace::testReplaceAttributes()
{
    if(!testReplaceAttributesPartial()) {
        return false;
    }
    if(!testReplaceAttributesPartialCase()) {
        return false;
    }
    if(!testReplaceAttributesPartialError()) {
        return false;
    }
    //---
    if(!testReplaceAttributesValuesPartial()) {
        return false;
    }
    if(!testReplaceAttributesValuesPartialCase()) {
        return false;
    }
    return true;
}

//---
bool TestReplace::testReplaceText()
{
    if(!testReplaceTextElement()) {
        return false;
    }
    if(!testReplaceTextElementInline()) {
        return false;
    }
    if(!testReplaceTextElementCData()) {
        return false;
    }
    return true ;
}

bool TestReplace::testReplaceComments()
{
    if(!testReplaceCommentsOk()) {
        return false;
    }
    if(!testReplaceCommentsError()) {
        return false;
    }
    return true ;
}

bool TestReplace::testReplaceCommentsOk()
{
    TestReplaceHelper helper;
    if(!initHelper("testReplaceCommentsOk", helper, 0, 1 ) ) {
        return false;
    }
    helper.initReplace("abc", "ppp", FindTextParams::FIND_COMMENT, false);
    helper.setCaseOn();
    helper.selection << 1 ;
    if(!testSkeleton(REPL_ALL_ELM_EXACT_START, REPL_ALL_COMMENT_END, &TestReplace::funOneStep, helper ))
    {
        return false;
    }
    return true;
}

bool TestReplace::testReplaceCommentsError()
{
    TestReplaceHelper helper;
    if(!initHelper("testReplaceCommentsError", helper, 1, 0 ) ) {
        return false;
    }
    helper.initReplace("abc", "--", FindTextParams::FIND_COMMENT, false);
    helper.setCaseOn();
    helper.selection << 1 ;
    if(!testSkeleton(REPL_ALL_ELM_EXACT_START, REPL_ALL_ELM_EXACT_START, &TestReplace::funOneStep, helper ))
    {
        return false;
    }
    return true;
}

bool TestReplace::testReplaceProcessingInstructions()
{
    TestReplaceHelper helper;
    if(!initHelper("testReplaceProcessingInstructions", helper, 0, 1 ) ) {
        return false;
    }
    helper.initReplace("zzz", "ppp", FindTextParams::FIND_ALL, false);
    helper.setCaseOn();
    if(!testSkeleton(REPL_ALL_ELM_PI_START, REPL_ALL_ELM_PI_END, &TestReplace::funOneStep, helper ))
    {
        return false;
    }
    return true;
}

//---

bool TestReplace::testReplaceTextElement()
{
    TestReplaceHelper helper;
    if(!initHelper("testReplaceTextElement", helper, 0, 2 ) ) {
        return false;
    }
    helper.initReplace("zzz", "ppp", FindTextParams::FIND_TEXT, false);
    if(!testSkeleton(REPL_ALL_TEXTELM_PARTIAL_START, REPL_ALL_TEXTELM_PARTIAL_END, &TestReplace::funOneStep, helper ))
    {
        return false;
    }
    return true;
}

bool TestReplace::testReplaceTextElementInline()
{
    TestReplaceHelper helper;
    if(!initHelper("testReplaceTextElementInline", helper, 0, 1 ) ) {
        return false;
    }
    helper.initReplace("zzz", "ppp", FindTextParams::FIND_TEXT, false);
    if(!testSkeleton(REPL_ALL_TEXT_INLINE_PARTIAL_START, REPL_ALL_TEXT_INLINE_PARTIAL_END, &TestReplace::funOneStep, helper ))
    {
        return false;
    }
    return true;
}

bool TestReplace::testReplaceTextElementCData()
{
    TestReplaceHelper helper;
    if(!initHelper("testReplaceTextElementCData", helper, 0, 1 ) ) {
        return false;
    }
    helper.initReplace("xxx", "ppp", FindTextParams::FIND_TEXT, false);
    if(!testSkeleton(REPL_ALL_TEXT_CDATA_PARTIAL_START, REPL_ALL_TEXT_CDATA_PARTIAL_END, &TestReplace::funOneStep, helper ))
    {
        return false;
    }
    return true;
}

//---

//----------------------------------------------------------
#define REPL_NEXT_START "../test/data/search/replace/next.xml"
#define REPL_NEXT_1 "../test/data/search/replace/next1.xml"
#define REPL_NEXT_2 "../test/data/search/replace/next2.xml"
#define REPL_NEXT_3 "../test/data/search/replace/next3.xml"
#define REPL_NEXT_4 "../test/data/search/replace/next4.xml"
//----------------------------------------------------------
#define REPL_PREV_START "../test/data/search/replace/prev.xml"
#define REPL_PREV_1 "../test/data/search/replace/prev1.xml"
#define REPL_PREV_2 "../test/data/search/replace/prev2.xml"
#define REPL_PREV_3 "../test/data/search/replace/prev3.xml"
#define REPL_PREV_4 "../test/data/search/replace/prev4.xml"

//---

bool TestReplace::testReplaceNext()
{
    _testName = "testReplaceNext";

    QStringList files ;
    files << REPL_NEXT_1 << REPL_NEXT_2 << REPL_NEXT_3 << REPL_NEXT_4 ;

    TestReplaceHelper helper;
    if(!initHelper("testReplaceNext", helper, 0, 0 ) ) {
        return false;
    }
    helper.selection << 0 << 1 << 0 ;
    helper.initReplace("abc", "ppp", FindTextParams::FIND_TAG, true);
    if( !testSkeletonPrevious(REPL_NEXT_START, files, helper, false ) ) {
        return false;
    }
    return true;
}

bool TestReplace::testReplacePrevious()
{
    _testName = "testReplacePrevious";

    QStringList files ;
    files << REPL_PREV_1 << REPL_PREV_2 << REPL_PREV_3 << REPL_PREV_4 ;

    TestReplaceHelper helper;
    if(!initHelper("testReplacePrevious", helper, 0, 0 ) ) {
        return false;
    }
    helper.selection << 0 << 2 << 0 ;
    helper.initReplace("abc", "ppp", FindTextParams::FIND_TAG, true);
    if( !testSkeletonPrevious(REPL_PREV_START, files, helper, true ) ) {
        return false;
    }
    return true;
}


/********************************
a
 a  4. repl and next (none)
 a  1.selez, repl,next
 b
   a 2. skip and next
   a 3. skip and next

prev:
a
 a  3. skip and prev
 a  2. skip prev
 b
   a 1. selez, repl and prev
   a 4. repl and prev (none)

**********************************/

bool TestReplace::testSkeletonPrevious(const QString &fileStart, const QStringList fileResult,
                                         TestReplaceHelper &helper, const bool isPrevious )
{
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(fileStart) ) {
        return error(QString("unable to load input file: '%1' ").arg(fileStart));
    }
    Regola *regola = app.mainWindow()->getRegola();
    Element *selectedElement = NULL ;
    if(helper.selection.isEmpty()) {
        selectedElement = regola->root();
    } else {
        selectedElement = app.mainWindow()->getRegola()->findElementByArray(helper.selection);
        helper.findArgs.setOnlyChildren(true);
    }
    if(NULL == selectedElement) {
        return error("no element selected");
    }
    app.mainWindow()->getEditor()->setCurrentItem(selectedElement);

    // if prev, usa previous

    //1
    if( isPrevious ) {
        helper.findArgs.setFindType(FindTextParams::ReplaceAndGotoPrev);
        app.mainWindow()->getEditor()->replace(&helper.findArgs);
    } else {
        helper.findArgs.setFindType(FindTextParams::ReplaceAndGotoNext);
        app.mainWindow()->getEditor()->replace(&helper.findArgs);
    }
    if(!cfr(regola, "op 1", fileResult.at(0))){
        return false;
    }
    //2
    if( isPrevious ) {
        helper.findArgs.setFindType(FindTextParams::SkipAndGotoPrev);
        app.mainWindow()->getEditor()->replace(&helper.findArgs);
    } else {
        helper.findArgs.setFindType(FindTextParams::SkipAndGotoNext);
        app.mainWindow()->getEditor()->replace(&helper.findArgs);
    }
    if(!cfr(regola, "op 2", fileResult.at(1))){
        return false;
    }
    //3
    if( isPrevious ) {
        helper.findArgs.setFindType(FindTextParams::SkipAndGotoPrev);
        app.mainWindow()->getEditor()->replace(&helper.findArgs);
    } else {
        helper.findArgs.setFindType(FindTextParams::SkipAndGotoNext);
        app.mainWindow()->getEditor()->replace(&helper.findArgs);
    }
    if(!cfr(regola, "op 3", fileResult.at(2))){
        return false;
    }
    //4
    if( isPrevious ) {
        helper.findArgs.setFindType(FindTextParams::ReplaceAndGotoPrev);
        app.mainWindow()->getEditor()->replace(&helper.findArgs);
    } else {
        helper.findArgs.setFindType(FindTextParams::ReplaceAndGotoNext);
        app.mainWindow()->getEditor()->replace(&helper.findArgs);
    }
    if(!cfr(regola, "op 4", fileResult.at(3))){
        return false;
    }

    regola->undo();
    if(!cfr(regola, "undo", fileResult.at(2))){
        return false;
    }
    regola->redo();
    if(!cfr(regola, "redo", fileResult.at(3))){
        return false;
    }
    return true;
}


