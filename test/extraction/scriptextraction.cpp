/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2019-2020 by Luca Bellonda and individual contributors  *
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

#include "testsplit.h"
#ifdef  QXMLEDIT_JS_SCRIPT
#include "utils.h"
#include "extraction/scripting/model/extractionscriptfiltermodel.h"
#include "extraction/extractionadavancedoptionsdialog.h"
#include "modules/services/systemservices.h"
#include "comparexml.h"

#define BASE_FOLDER   "../test/data/split/scripting/"

#define TEXT_EVENT_SOURCE_FILE   BASE_FOLDER   "source_text_event.xml"
#define TEXT_EVENT_RES0_FILE   BASE_FOLDER   "res0_text_event.xml"
#define TEXT_EVENT_RES1_FILE   BASE_FOLDER   "res1_text_event.xml"
#define TEXT_EVENT_RES2_FILE   BASE_FOLDER   "res2_text_event.xml"
#define TEXT_EVENT_RES3_FILE   BASE_FOLDER   "res3_text_event.xml"
#define TEXT_EVENT_RES4_FILE   BASE_FOLDER   "res4_text_event.xml"
//-----------------
#define ELEMENT_EVENT_SOURCE_FILE   BASE_FOLDER   "source_element_event.xml"
#define ELEMENT_EVENT_RES1_FILE BASE_FOLDER   "res1_element_event.xml"
#define ELEMENT_EVENT_RES2_FILE BASE_FOLDER   "res2_element_event.xml"
#define ELEMENT_EVENT_RES3_FILE BASE_FOLDER   "res3_element_event.xml"
#define ELEMENT_EVENT_RES4_FILE BASE_FOLDER   "res4_element_event.xml"
#define ELEMENT_EVENT_RES5_FILE BASE_FOLDER   "res5_element_event.xml"
#define MIXED_EVENT_RES1_FILE BASE_FOLDER   "res1_mixed_event.xml"
//-----------------
#define INTEGRATION_BASE BASE_FOLDER "integrated/"
#define INTEGRATION_S_T_0 INTEGRATION_BASE "s_t_0.xml"
#define INTEGRATION_R_T_0 INTEGRATION_BASE "r_t_0.xml"
#define INTEGRATION_S_TN_0 INTEGRATION_BASE "s_tn_0.xml"
#define INTEGRATION_R_TN_0 INTEGRATION_BASE "r_tn_0.xml"
//--
#define INTEGRATION_S_R_0 INTEGRATION_BASE "s_r_0.xml"
#define INTEGRATION_R_R_0 INTEGRATION_BASE "r_r_0.xml"
#define INTEGRATION_S_RN_0 INTEGRATION_BASE "s_rn_0.xml"
#define INTEGRATION_R_RN_0 INTEGRATION_BASE "r_rn_0.xml"
//--
#define INTEGRATION_S_TR_0 INTEGRATION_BASE "s_tr_0.xml"
#define INTEGRATION_R_TR_0 INTEGRATION_BASE "r_tr_0.xml"
#define INTEGRATION_S_TRN_0 INTEGRATION_BASE "s_trn_0.xml"
#define INTEGRATION_R_TRN_0 INTEGRATION_BASE "r_trn_0.xml"
//--
#define CASES_BASE BASE_FOLDER "issues/"
#define CASE0_SOURCE CASES_BASE "case0.xml"
#define CASE0_RESULT CASES_BASE "case0r.xml"
#define CASE0S_RESULT CASES_BASE "case0sr.xml"
#define CASE0MULTI_SOURCE CASES_BASE "case0m.xml"
#define CASE0MULTI_RESULT CASES_BASE "case0mr.xml"
#define CASE0MULTINS_RESULT CASES_BASE "case0mrns.xml"
#define CASE1_SOURCE CASES_BASE "case1.xml"
#define CASE10_RESULT CASES_BASE "case10r.xml"
#define CASE11_RESULT CASES_BASE "case11r.xml"
#define CASE12_RESULT CASES_BASE "case12r.xml"
#define CASE10NS_RESULT CASES_BASE "case10nsr.xml"
#define CASE11NS_RESULT CASES_BASE "case11nsr.xml"
#define CASE12NS_RESULT CASES_BASE "case12nsr.xml"
//--

static const QString EventElHdl("theElementHandler");


TestExtractionHolder::TestExtractionHolder() : op(&results)
{
}

TestExtractionHolder::~TestExtractionHolder()
{
}

//-----------------

bool TestSplit::unitTestFilterEvents()
{
    _testName = "unitTestFilterEvents";
    if(!unitTestFilterLoad()) {
        return false ;
    }
    if(!unitTestFilterEventText()) {
        return false ;
    }
    if(!unitTestFilterEventElement()) {
        return false ;
    }
    return true;
}

//-----------------


bool TestSplit::testScriptEmptyActivation()
{
    _subTestName = "testScriptEmptyActivation" ;
    ExtractResults results;
    ExtractionOperation op(&results);
    if(op.isScriptingEnabled()) {
        return error("Scripting enabled by default");
    }
    return true;
}

bool TestSplit::testScriptActivation()
{
    _subTestName = "testScriptActivation" ;
    ExtractResults results;
    ExtractionOperation op(&results);
    op.addScriptingFilter(TestSplitScriptingHelper::simpleFilter());
    if(!op.isScriptingEnabled()) {
        return error("Scripting not enabled when it should");
    }
    return true;
}

bool TestSplit::unitTestScriptingBaseCheckHelper(TestSplitScriptingHelper *helper, const bool expectedEnabled)
{
    helper->applyModelList();
    return unitTestScriptingBaseCheck(&helper->filter, expectedEnabled, helper->expectedHandlers);
}

bool TestSplit::unitTestScriptingBaseCheck(ExtractionScriptFilter *filter, const bool expectedEnabled, QList<ExtractionScriptEventHandler*> expectedHandlers)
{
    if(filter->isEnabled()!= expectedEnabled) {
        return error(QString("ExtractionScriptFilter enabled:%1 : wrong").arg(expectedEnabled));
    }
    if(filter->eventsCount() != expectedHandlers.size()) {
        return error(QString("ExtractionScriptFilter expectedEventsCount: %1, expected: %2").arg(filter->eventsCount()).arg(expectedHandlers.size()));
    }
    foreach (ExtractionScriptEventHandler *expectedHandler, expectedHandlers) {
        bool found = false;
        foreach(ExtractionScriptEventHandler * handler, filter->allEvents()){
            if(expectedHandler->equalsTo(handler)) {
                found = true ;
                break;
            }
        }
        if(!found) {
            return error(QString("Handler not found: %1").arg(expectedHandler->toString()));
        }
    }

    return true ;
}
/*
aggiungi evento 1
poi 2 in catena e controlla tipo con uno switch
*/
/*
 to test: filter, manager, scripting, extraction
unit test
        1) abilitato:no
        2)          add event disabilitato, 0 eventi
        3)          add event abilitato, 1 evento
        4)          add 2 volte stesso evento: 1 evento
        5)          add 2 eventi diversi 2 eventi 1 abilitato 1 no
        6)          add 2 eventi diversi abilitati
 da testare: ogni tipo di evento aggiunto

 test sul tipo di evento e la sua abilitazione

*/

bool TestSplit::unitTestScriptingBase()
{
    _subTestName = "unitTestScriptingBase";
    if(!unitTestFilterDefault()) {
        return false;
    }
    if(!unitTestFilterOneEvent()) {
        return false;
    }
    if(!unitTestFilterEvents()) {
        return false;
    }
    if(!unitTestFilterInterface()) {
        return false;
    }
    return true ;
}

bool TestSplit::unitTestFilterDefault()
{
    _subTestName = "unitTestScriptingBase/unitTestFilterDefault";
    ExtractionScriptFilter filter;
    QList<ExtractionScriptEventHandler*> expectedHandlers;
    if(!unitTestScriptingBaseCheck(&filter, false, expectedHandlers)) {
        return false;
    }
    return true;
}

bool TestSplit::unitTestFilterOneEvent()
{
    _subTestName = "unitTestScriptingBase/unitTestFilterOneEvent/2";
    // case 2
    {
        TestSplitScriptingHelper helper(true);
        helper.createBaseEventModelList(ExtractionEventText, false, "name", "code");
        if(!unitTestScriptingBaseCheckHelper(&helper, false)) {
            return false;
        }
    }
    _subTestName = "unitTestScriptingBase/unitTestFilterOneEvent/3";
    // case 3
    {
        TestSplitScriptingHelper helper(true);
        helper.createBaseEventModelList(ExtractionEventText, true, "name", "code");
        if(!unitTestScriptingBaseCheckHelper(&helper, true)) {
            return false;
        }
    }
    _subTestName = "unitTestScriptingBase/unitTestFilterOneEvent/4";
    // case 4
    {
        TestSplitScriptingHelper helper(true);
        helper.createBaseEventModelList(ExtractionEventText, true, "name", "code");
        helper.createBaseEventModelList(ExtractionEventText, true, "name", "code");
        if(!unitTestScriptingBaseCheckHelper(&helper, true)) {
            return false;
        }
    }
    _subTestName = "unitTestScriptingBase/unitTestFilterOneEvent/4.1";
    // case 4.1
    {
        TestSplitScriptingHelper helper(true);
        helper.createBaseEventModelList(ExtractionEventText, false, "name", "code");
        helper.createBaseEventModelList(ExtractionEventText, false, "name", "code");
        if(!unitTestScriptingBaseCheckHelper(&helper, false)) {
            return false;
        }
    }
    _subTestName = "unitTestScriptingBase/unitTestFilterOneEvent/5";
    // case 5
    {
        TestSplitScriptingHelper helper(true);
        helper.createBaseEventModelList(ExtractionEventText, true, "name", "code");
        helper.createBaseEventModelList(ExtractionEventNone, false, "name", "code");
        if(!unitTestScriptingBaseCheckHelper(&helper, true)) {
            return false;
        }
    }
    _subTestName = "unitTestScriptingBase/unitTestFilterOneEvent/6";
    // case 6
    {
        TestSplitScriptingHelper helper(true);
        helper.createBaseEventModelList(ExtractionEventText, true, "name", "code");
        helper.createBaseEventModelList(ExtractionEventNone, true, "name", "code");
        if(!unitTestScriptingBaseCheckHelper(&helper, true)) {
            return false;
        }
    }
    return true;
}

void TestSplit::initOperationForScripting(TestExtractionHolder *holder, const QString &inputFile)
{
    QString extractFolder(SystemServices::tempLocation());
    QString timeStamp = newTS();
    holder->op.setInputFile(inputFile);
    holder->op.setSplitDepth(1);
    holder->op.setSplitType(ExtractionOperation::SplitUsingDepth);
    holder->op.setOperationType(ExtractionOperation::OperationFilter);
    holder->op.setExtractDocuments(true);
    holder->op.setExtractRange();
    holder->op.setMaxDoc(1);
    holder->op.setMinDoc(1);
    holder->op.setExtractFolder(SystemServices::tempLocation());
    //
    holder->op.setIsMakeSubFolders(true);
    holder->op.setSubFoldersEachNFiles(1);
    QStringList list;
    list.append(timeStamp);
    list.append("%counter%");
    QStringList listF;
    listF.append(timeStamp);
    listF.append("qxmledit_test");
    holder->op.setSubfolderNamePattern(listF);
    extractFolder.append(QDir::separator());
    extractFolder.append(timeStamp);
    extractFolder.append("qxmledit_test");
    holder->op.setFilesNamePattern(list);
    holder->fileName = holder->op.makeAName(extractFolder, 1, list, 1);
    holder->fileName.append(".xml");
}

bool TestSplit::executeOperationForScripting(TestExtractionHolder *holder, const bool expectedError, const ExtractionOperation::EXMLErrors expectedErrorCode, const QString &fileToCheck)
{
    // do operation
    holder->op.performExtraction();
    if(holder->op.isError() != expectedError ) {
        return error(QString("Execution error: expected %1 vs %2 (%3)").arg(expectedError).arg(holder->op.isError()).arg(holder->op.scriptManager()->errorMessageExtended()));
    }
    if(holder->op.error() != expectedErrorCode ) {
        return error(QString("Execution error code: expected %1 vs %2").arg(expectedErrorCode).arg(holder->op.error()));
    }
    if(!holder->op.isError()) {
        if(!checkFilter(fileToCheck, holder->fileName) ) {
            return false;
        }
    }
    return true ;
}

//--------------------------------------------------

bool TestSplit::testScriptEventTextTemplate(TestExtractionHolder *holder, const QString &sourceFile, const QString &code)
{
    initOperationForScripting(holder, sourceFile);
    setupTextEvent(&holder->op, code);
    holder->op.addScriptingFilter(TestSplitScriptingHelper::simpleFilter());
    if(!holder->op.isScriptingEnabled()) {
        return error("Scripting not enabled when it should");
    }
    return true;
}

void TestSplit::setupTextEvent(ExtractionOperation *operation, const QString &code)
{
    ExtractionScriptFilterModel *filterModel = TestSplitScriptingHelper::newFilterModel();
    ExtractionScriptEventModel *filterEventModel = TestSplitScriptingHelper::baseEventModel(ExtractionEventText, true, "theTextHandler", code);
    filterModel->addEventModel(filterEventModel);
    operation->scriptManager()->addScriptingFilter(filterModel);
}

bool TestSplit::checkTextEvent(const QString &sourceFile, const QString &code, const bool expectedError, const ExtractionOperation::EXMLErrors expectedErrorCode, const QString &fileToCheck)
{
    TestExtractionHolder holder;
    if(!testScriptEventTextTemplate(&holder, sourceFile, code) ) {
        return false;
    }
    if(!executeOperationForScripting(&holder, expectedError, expectedErrorCode, fileToCheck) ) {
        return false;
    }
    return true;
}

bool TestSplit::testScriptEventText()
{
    _subTestName = "testScriptEventText/1ia3" ;
    if(!checkTextEvent(TEXT_EVENT_SOURCE_FILE, "function theTextHandler(context, event){ if(context.path==\"/root/a\") { event.ignored = true;}}", false, ExtractionOperation::EXML_NoError, TEXT_EVENT_RES1_FILE) ) {
        return false;
    }
    //-----------
    _subTestName = "testScriptEventText/0" ;
    if(!checkTextEvent(TEXT_EVENT_SOURCE_FILE, "fsdfunction han**dler(context, event)asdass daas dasd XXX { }", true, ExtractionOperation::EXML_InitScripting, TEXT_EVENT_RES1_FILE) ) {
        return false;
    }
    _subTestName = "testScriptEventText/01" ;
    if(!checkTextEvent(TEXT_EVENT_SOURCE_FILE, "function handlerMissing(context, event) { }", true, ExtractionOperation::EXML_Scripting, TEXT_EVENT_RES1_FILE) ) {
        return false;
    }
    _subTestName = "testScriptEventText/1iall" ;
    if(!checkTextEvent(TEXT_EVENT_SOURCE_FILE, "function theTextHandler(context, event){ event.ignored = true; }", false, ExtractionOperation::EXML_NoError, TEXT_EVENT_RES0_FILE) ) {
        return false;
    }
    _subTestName = "testScriptEventText/1ia" ;
    if(!checkTextEvent(TEXT_EVENT_SOURCE_FILE, "function theTextHandler(context, event){ if(context.path=='/root/a') { event.ignored = true;}}", false, ExtractionOperation::EXML_NoError, TEXT_EVENT_RES1_FILE) ) {
        return false;
    }
    _subTestName = "testScriptEventText/1change" ;
    if(!checkTextEvent(TEXT_EVENT_SOURCE_FILE, "function theTextHandler(context, event){ if(context.path=='/root/a') { event.text = 'aaa';}}", false, ExtractionOperation::EXML_NoError, TEXT_EVENT_RES2_FILE) ) {
        return false;
    }
    _subTestName = "testScriptEventText/1add" ;
    if(!checkTextEvent(TEXT_EVENT_SOURCE_FILE, "function theTextHandler(context, event){ if(context.path=='/root/a') { event.text += 'aaa';}}", false, ExtractionOperation::EXML_NoError, TEXT_EVENT_RES3_FILE) ) {
        return false;
    }
    _subTestName = "testScriptEventText/2add" ;
    if(!checkTextEvent(TEXT_EVENT_SOURCE_FILE, "function theTextHandler(context, event){ if(context.path=='/root/a') { event.text += 'aaa';}if(context.path=='/root/a/b/c') { event.text = 'ccc'; event.isCDATA = true}}", false, ExtractionOperation::EXML_NoError, TEXT_EVENT_RES4_FILE) ) {
        return false;
    }
    return true;
}


//--------------------------------------------------

void TestSplit::setupElementEvent(ExtractionOperation *operation, const QString &code)
{
    ExtractionScriptFilterModel *filterModel = TestSplitScriptingHelper::newFilterModel();
    ExtractionScriptEventModel *filterEventModel = TestSplitScriptingHelper::baseEventModel(ExtractionEventElement, true, EventElHdl, code);
    filterModel->addEventModel(filterEventModel);
    operation->scriptManager()->addScriptingFilter(filterModel);
}

bool TestSplit::testScriptEventElement()
{
    //-----------
    {
        _subTestName = "testScriptEventElement/errsyntax" ;
        const QString Code("fsdfunction han**dler(context, event)asdass daas dasd XXX { }");
        if(!checkElementEvent(ELEMENT_EVENT_SOURCE_FILE, Code, Code, true, ExtractionOperation::EXML_InitScripting, ELEMENT_EVENT_SOURCE_FILE) ) {
            return false;
        }
    }
    {
        _subTestName = "testScriptEventElement/nomod";
        const QString Code("function theElementHandler(context, event) { }");
        if(!checkElementEvent(ELEMENT_EVENT_SOURCE_FILE, Code, Code, false, ExtractionOperation::EXML_NoError, ELEMENT_EVENT_RES1_FILE) ) {
            return false;
        }
    }
    {
        _subTestName = "testScriptEventElement/changeattr";
        const QString Code1("function theElementHandler(context, event) { if(event.elementName=='a') { event.setAttributeValueByName('a', 'xx')}}");
        const QString Code2("function theElementHandler(context, event) { if(event.elementName=='a') { event.setAttributeValueByName('a', event.attributeValueByName('a')+'yy')} }");
        if(!checkElementEvent(ELEMENT_EVENT_SOURCE_FILE, Code1, Code2, false, ExtractionOperation::EXML_NoError, ELEMENT_EVENT_RES2_FILE) ) {
            return false;
        }
    }
    {
        _subTestName = "testScriptEventElement/addattr";
        const QString Code1("function theElementHandler(context, event) { if(event.elementName=='a') { event.setAttributeValueByName('ax', 'xx')}}");
        const QString Code2("function theElementHandler(context, event) { if(event.elementName=='a') { event.setAttributeValueByName('ax1', event.attributeValueByName('a')+'yy')}}");
        if(!checkElementEvent(ELEMENT_EVENT_SOURCE_FILE, Code1, Code2, false, ExtractionOperation::EXML_NoError, ELEMENT_EVENT_RES3_FILE) ) {
            return false;
        }
    }
    {
        _subTestName = "testScriptEventElement/removeattr";
        const QString Code1("function theElementHandler(context, event) { if(event.elementName=='a') { event.removeAttributeByName('at1')}}");
        const QString Code2("function theElementHandler(context, event) { if(event.elementName=='c') { event.removeAttributeByName('at2')}}");
        if(!checkElementEvent(ELEMENT_EVENT_SOURCE_FILE, Code1, Code2, false, ExtractionOperation::EXML_NoError, ELEMENT_EVENT_RES4_FILE) ) {
            return false;
        }
    }
    {
        _subTestName = "testScriptEventElement/addremovechangeattr";
        const QString Code1("function theElementHandler(context, event) { if(event.elementName=='a') { event.removeAttributeByName('at1')}}");
        const QString Code2("function theElementHandler(context, event) { if(event.elementName=='a') { event.setAttributeValueByName('ax1', event.attributeValueByName('a')+'yy')}}");
        if(!checkElementEvent(ELEMENT_EVENT_SOURCE_FILE, Code1, Code2, false, ExtractionOperation::EXML_NoError, ELEMENT_EVENT_RES5_FILE) ) {
            return false;
        }
    }
    return true;
}

bool TestSplit::checkElementEvent(const QString &sourceFile, const QString &code1, const QString &code2, const bool expectedError, const ExtractionOperation::EXMLErrors expectedErrorCode, const QString &fileToCheck)
{
    TestExtractionHolder holder;
    if(!testScriptEventElementTemplate(&holder, sourceFile, code1, code2) ) {
        return false;
    }
    if(!executeOperationForScripting(&holder, expectedError, expectedErrorCode, fileToCheck) ) {
        return false;
    }
    return true;
}

bool TestSplit::testScriptEventElementTemplate(TestExtractionHolder *holder, const QString &sourceFile, const QString &code1, const QString &code2)
{
    initOperationForScripting(holder, sourceFile);
    setupElementEvent(&holder->op, code1);
    setupElementEvent(&holder->op, code2);
    holder->op.addScriptingFilter(TestSplitScriptingHelper::simpleFilter());
    if(!holder->op.isScriptingEnabled()) {
        return error("Scripting not enabled when it should");
    }
    return true;
}

bool TestSplit::testScriptEventElementAndTextTemplate(TestExtractionHolder *holder, const QString &sourceFile, const QString &codeText, const QString &codeElement)
{
    initOperationForScripting(holder, sourceFile);
    setupTextEvent(&holder->op, codeText);
    setupElementEvent(&holder->op, codeElement);
    holder->op.addScriptingFilter(TestSplitScriptingHelper::simpleFilter());
    if(!holder->op.isScriptingEnabled()) {
        return error("Scripting not enabled when it should");
    }
    return true;
}

bool TestSplit::unitAllEventsChainOnScriptManager()
{
    _testName = "unitAllEventsChainOnScriptManager";
    const QString CodeElement("function theElementHandler(context, event) { if(event.elementName=='a') { event.setAttributeValueByName('ax', 'xx')}}");
    const QString CodeText("function theTextHandler(context, event) { if(context.path=='/root/a') { event.text = 'ab' ;}}");
    if(!checkElementAndTextEvent(ELEMENT_EVENT_SOURCE_FILE, CodeText, CodeElement, false, ExtractionOperation::EXML_NoError, MIXED_EVENT_RES1_FILE) ) {
        return false;
    }
    return true ;
}

bool TestSplit::checkElementAndTextEvent(const QString &sourceFile, const QString &codeText, const QString &codeElement, const bool expectedError, const ExtractionOperation::EXMLErrors expectedErrorCode, const QString &fileToCheck)
{
    TestExtractionHolder holder;
    if(!testScriptEventElementAndTextTemplate(&holder, sourceFile, codeText, codeElement) ) {
        return false;
    }
    if(!executeOperationForScripting(&holder, expectedError, expectedErrorCode, fileToCheck) ) {
        return false;
    }
    return true;
}

#define XML_FILTER_MALFORMED_STRING \
    "<root>"\
    " <mmilter enabled='true' >" \
    "   <event type='text' enabled='true' description='descr' handler='xxx'> "\
    "     <code></code>" \
    "   </event> " \
    " </mmilter>" \
    "<root>"

bool TestSplit::loadEventMalformed()
{
    ExtractionScriptFilterModel* filterModel = ExtractionScriptFilterModel::fromXMLString(XML_FILTER_MALFORMED_STRING);
    if(NULL != filterModel ) {
        delete filterModel ;
        return error("Loaded filter from malformed XML");
    }
    return true;
}

bool TestSplit::checkFilterEvent(const QString &code, ExtractionScriptEventModel *event, const EExtractionEventType expectedType, const bool expectedEnabled, const QString &expectedHandler, const QString &expectedDescription, const QString &expectedCode)
{
    if(event->eventType() != expectedType) {
        return error(QString("%1 event expected type %2, found %3").arg(code).arg(expectedType).arg(event->eventType()));
    }
    if(event->enabled() != expectedEnabled) {
        return error(QString("%1 event expected enabled %2, found %3").arg(code).arg(expectedEnabled).arg(event->enabled()));
    }
    if(event->handlerName()!= expectedHandler) {
        return error(QString("%1 event expected handler %2, found %3").arg(code).arg(expectedHandler).arg(event->handlerName()));
    }
    if(event->description()!= expectedDescription) {
        return error(QString("%1 event expectedDescription %2, found %3").arg(code).arg(expectedDescription).arg(event->description()));
    }
    if(event->code()!= expectedCode) {
        return error(QString("%1 event expectedCode %2, found %3").arg(code).arg(expectedCode).arg(event->code()));
    }
    return true;
}

#define CODE_TEST " function xx () {"\
"    aaa() " \
"   "\
"  }"

#define XML_FILTER_ONEEVENT_STRING_START \
" <filter enabled='true' >" \
"   <event type='text' enabled='true' description='descr' handler='xxx'> "\
"     <code><![CDATA["


#define XML_FILTER_ONEEVENT_STRING_END "]]></code>" \
"   </event> "\
" </filter >"

#define XML_FILTER_TWOEVENTS_STRING_START \
" <filter enabled='true' >" \
"   <event type='text' enabled='true' description='descrText' handler='hText'> "\
"     <code><![CDATA["


#define XML_FILTER_TWOEVENTS_STRING_MIDDLE "]]></code>" \
    "   </event> "\
    "   <event type='element' enabled='false' description='descrElem' handler='hElem'> "\
    "     <code><![CDATA["


#define XML_FILTER_TWOEVENTS_STRING_END "]]></code>" \
"   </event> "\
" </filter >"


#define XML_FILTER_ONEEVENT_STRING XML_FILTER_ONEEVENT_STRING_START CODE_TEST XML_FILTER_ONEEVENT_STRING_END
#define XML_FILTER_TWOEVENTS_STRING XML_FILTER_TWOEVENTS_STRING_START CODE_TEST XML_FILTER_TWOEVENTS_STRING_MIDDLE CODE_TEST XML_FILTER_TWOEVENTS_STRING_END



bool TestSplit::loadEventOne()
{
    ExtractionScriptFilterModel* filterModel = ExtractionScriptFilterModel::fromXMLString(XML_FILTER_ONEEVENT_STRING);
    if(NULL != filterModel ) {
        if(!filterModel->enabled()) {
            delete filterModel;
            return error("One event enabled");
        }
        if(filterModel->events().size() != 1 ) {
            delete filterModel;
            return error(QString("One event expected 1 event, found %1").arg(filterModel->events().size()));
        }
        foreach(ExtractionScriptEventModel *event, filterModel->events() ) {
            if(!checkFilterEvent("OneEvent", event, ExtractionEventText, true, "xxx", "descr", CODE_TEST)) {
                delete filterModel;
                return false;
            }
        }
        delete filterModel;
        return true;
    }
    return error("One event null filter");
}

bool TestSplit::loadEventTwo()
{
    _testName = "loadEventTwo";
    ExtractionScriptFilterModel* filterModel = ExtractionScriptFilterModel::fromXMLString(XML_FILTER_TWOEVENTS_STRING);
    if(NULL != filterModel ) {
        if(!filterModel->enabled()) {
            delete filterModel;
            return error("Two events enabled");
        }
        if(filterModel->events().size() != 2 ) {
            delete filterModel;
            return error(QString("Two events expected 2 events, found %1").arg(filterModel->events().size()));
        }
        bool eventText = false;
        bool eventElement = false;
        foreach( ExtractionScriptEventModel* event, filterModel->events() ) {
            if( event->eventType() == ExtractionEventText ) {
                if(!checkFilterEvent("Two events a", event, ExtractionEventText, true, "hText", "descrText", CODE_TEST)) {
                    delete filterModel;
                    return false;
                }
                eventText = true ;
            }
            if( event->eventType() == ExtractionEventElement ) {
                if(!checkFilterEvent("Two events b", event, ExtractionEventElement, false, "hElem", "descrElem", CODE_TEST)) {
                    delete filterModel;
                    return false;
                }
                eventElement = true ;
            }
        }
        delete filterModel;
        if(!eventText) {
            return error("Two events no text event");
        }
        if(!eventElement) {
            return error("Two events no element event");
        }

        return true;
    }
    return error("Two events null filter");
}

bool TestSplit::unitTestFilterLoad()
{
    _testName = "unitTestFilterLoad" ;
    // KO FOR MALFORMED
    if(!loadEventMalformed()) {
        return false;
    }
    // OK 1 event
    if(!loadEventOne()) {
        return false;
    }
    // OK 2 events
    if(!loadEventTwo()) {
        return false;
    }
    return true ;
}

bool TestSplit::testScriptWithPredefinedScripting()
{
    if(!testScriptWithPredefinedScriptingFilter()) {
        return false;
    }
    if(!testScriptWithPredefinedScriptingSplit()) {
        return false;
    }
    if(!testScriptWithPredefinedScriptingFilterAll()) {
        return false;
    }
    return true ;
}

bool TestSplit::testScriptWithPredefinedScriptingFilter()
{
    _testName = "testScriptWithPredefinedScriptingFilter" ;
    if(!testPredefinedScriptTrimAttributesNoNsFilter()) {
        return false ;
    }
    if(!testPredefinedScriptTrimAttributesNsFilter()) {
        return false ;
    }
    if(!testPredefinedScriptRemoveEmptyAttributesNoNsFilter()) {
        return false ;
    }
    if(!testPredefinedScriptRemoveEmptyAttributesNsFilter()) {
        return false ;
    }
    return true ;
}

bool TestSplit::testScriptWithPredefinedScriptingSplit()
{
    _testName = "testScriptWithPredefinedScriptingSplit" ;
    if(!testPredefinedScriptTrimAttributesNoNsSplit()) {
        return false ;
    }
    if(!testPredefinedScriptTrimAttributesNsSplit()) {
        return false ;
    }
    if(!testPredefinedScriptRemoveEmptyAttributesNoNsSplit()) {
        return false ;
    }
    if(!testPredefinedScriptRemoveEmptyAttributesNsSplit()) {
        return false ;
    }
    return true ;
}

bool TestSplit::testScriptWithPredefinedScriptingFilterAll()
{
    _testName = "testScriptWithPredefinedScriptingFilterAll" ;
    if(!testPredefinedScriptNoNsFilterAll()) {
        return false ;
    }
    if(!testPredefinedScriptNsFilterAll()) {
        return false ;
    }
    return true ;
}

bool TestSplit::testPredefinedScriptNoNsFilterAll()
{
    _testName = "testPredefinedScriptNoNsFilterAll" ;
    QList<ExtractionScriptingProvider::EPredefinedScripts> scripts;
    scripts << ExtractionScriptingProvider::PredefinedScriptTrimAttributes << ExtractionScriptingProvider::PredefinedScriptRemoveEmptyAttributes ;
    return testPredefinedScriptExecute(false, true, INTEGRATION_S_TR_0, INTEGRATION_R_TR_0, scripts);
}

bool TestSplit::testPredefinedScriptNsFilterAll()
{
    _testName = "testPredefinedScriptNsFilterAll" ;
    QList<ExtractionScriptingProvider::EPredefinedScripts> scripts;
    scripts << ExtractionScriptingProvider::PredefinedScriptTrimAttributes << ExtractionScriptingProvider::PredefinedScriptRemoveEmptyAttributes ;
    return testPredefinedScriptExecute(true, true, INTEGRATION_S_TRN_0, INTEGRATION_R_TRN_0, scripts);
}

bool TestSplit::testPredefinedScriptTrimAttributesNoNsSplit()
{
    _testName = "testPredefinedScriptTrimAttributesNoNsSplit" ;
    QList<ExtractionScriptingProvider::EPredefinedScripts> scripts;
    scripts << ExtractionScriptingProvider::PredefinedScriptTrimAttributes;
    return testPredefinedScriptExecute(false, false, INTEGRATION_S_T_0, INTEGRATION_R_T_0, scripts);
}

bool TestSplit::testPredefinedScriptTrimAttributesNsSplit()
{
    _testName = "testPredefinedScriptTrimAttributesNsSplit" ;
    QList<ExtractionScriptingProvider::EPredefinedScripts> scripts;
    scripts << ExtractionScriptingProvider::PredefinedScriptTrimAttributes;
    return testPredefinedScriptExecute(true, false, INTEGRATION_S_TN_0, INTEGRATION_R_TN_0, scripts);
}

bool TestSplit::testPredefinedScriptRemoveEmptyAttributesNoNsSplit()
{
    _testName = "testPredefinedScriptRemoveEmptyAttributesNoNsSplit" ;
    QList<ExtractionScriptingProvider::EPredefinedScripts> scripts;
    scripts << ExtractionScriptingProvider::PredefinedScriptRemoveEmptyAttributes;
    return testPredefinedScriptExecute(false, false, INTEGRATION_S_R_0, INTEGRATION_R_R_0, scripts);
}

bool TestSplit::testPredefinedScriptRemoveEmptyAttributesNsSplit()
{
    _testName = "testPredefinedScriptRemoveEmptyAttributesNoSplit" ;
    QList<ExtractionScriptingProvider::EPredefinedScripts> scripts;
    scripts << ExtractionScriptingProvider::PredefinedScriptRemoveEmptyAttributes;
    return testPredefinedScriptExecute(true, false, INTEGRATION_S_RN_0, INTEGRATION_R_RN_0, scripts);
}

bool TestSplit::testPredefinedScriptTrimAttributesNoNsFilter()
{
    _testName = "testPredefinedScriptTrimAttributesNoNsFilter" ;
    QList<ExtractionScriptingProvider::EPredefinedScripts> scripts;
    scripts << ExtractionScriptingProvider::PredefinedScriptTrimAttributes;
    return testPredefinedScriptExecute(false, true, INTEGRATION_S_T_0, INTEGRATION_R_T_0, scripts);
}

bool TestSplit::testPredefinedScriptTrimAttributesNsFilter()
{
    _testName = "testPredefinedScriptTrimAttributesNsFilter" ;
    QList<ExtractionScriptingProvider::EPredefinedScripts> scripts;
    scripts << ExtractionScriptingProvider::PredefinedScriptTrimAttributes;
    return testPredefinedScriptExecute(true, true, INTEGRATION_S_TN_0, INTEGRATION_R_TN_0, scripts);
}

bool TestSplit::testPredefinedScriptRemoveEmptyAttributesNoNsFilter()
{
    _testName = "testPredefinedScriptRemoveEmptyAttributesNoNsFilter" ;
    QList<ExtractionScriptingProvider::EPredefinedScripts> scripts;
    scripts << ExtractionScriptingProvider::PredefinedScriptRemoveEmptyAttributes;
    return testPredefinedScriptExecute(false, true, INTEGRATION_S_R_0, INTEGRATION_R_R_0, scripts);
}

bool TestSplit::testPredefinedScriptRemoveEmptyAttributesNsFilter()
{
    _testName = "testPredefinedScriptRemoveEmptyAttributesNoNsFilter" ;
    QList<ExtractionScriptingProvider::EPredefinedScripts> scripts;
    scripts << ExtractionScriptingProvider::PredefinedScriptRemoveEmptyAttributes;
    return testPredefinedScriptExecute(true, true, INTEGRATION_S_RN_0, INTEGRATION_R_RN_0, scripts);
}

bool TestSplit::testPredefinedScriptExecute(const bool useNamespaces, const bool isFilter, const QString &fileReference, const QString &fileResult, QList<ExtractionScriptingProvider::EPredefinedScripts> scripts)
{
    return testPredefinedScriptExecute(useNamespaces, isFilter, fileReference, fileResult, scripts, 1, true, 0, 0);
}

bool TestSplit::testPredefinedScriptExecute(const bool useNamespaces, const bool isFilter, const QString &fileReference, const QString &fileResult, QList<ExtractionScriptingProvider::EPredefinedScripts> scripts, const int depth, const bool isAllDocuments, const unsigned int minDoc, const unsigned int maxDoc)
{
    ExtractResults results;
    ExtractionOperation op(&results);
    QString extractFolder(SystemServices::tempLocation());

    QString timeStamp = newTS();
    setupFilterParametersFilterText(&op, extractFolder, timeStamp, fileReference, "", isFilter);
    op.setSplitDepth(depth);
    op.setSplitType(ExtractionOperation::SplitUsingDepth);
    op.setOperationType(isFilter?ExtractionOperation::OperationFilter:ExtractionOperation::OperationSplit);
    if(isAllDocuments) {
        op.setExtractAllDocuments();
    } else {
        op.setExtractRange();
        op.setMinDoc(minDoc);
        op.setMaxDoc(maxDoc);
    }

    QStringList list;
    list.append(timeStamp);
    list.append("%counter%");
    QStringList listF;
    listF.append(timeStamp);
    listF.append("qxmledit_test");
    extractFolder.append(QDir::separator());
    extractFolder.append(timeStamp);
    extractFolder.append("qxmledit_test");
    QString fileName1 = op.makeAName(extractFolder, 1, list, 1);
    fileName1.append(".xml");
    // do operation
    op.setUseNamespaces(useNamespaces);
    QStringList filters;
    foreach(const ExtractionScriptingProvider::EPredefinedScripts script, scripts) {
        filters.append(QString("%1").arg(script));
    }
    op.setFiltersId(filters.join(","));

    op.performExtraction();
    if(op.isError()) {
        return error(QString("Split relative Error: %1 %2").arg(op.error()).arg(op.errorMessage()));
    }
    CompareXML compare;
    const bool result = compare.compareFiles(fileResult, fileName1);
    if(!result) {
        return error(QString("Filter is not correct \n%1 found:\n%2\nexpected:\n%3\n")
                     .arg(compare.errorString())
                     .arg(loadTextFile(fileName1))
                     .arg(loadTextFile(fileResult)));
    }
    return true ;
}

bool TestSplit::testPredefinedScriptExecuteList(const bool useNamespaces, const bool isFilter, const QString &fileReference,
                                                const QStringList referenceFiles, QList<ExtractionScriptingProvider::EPredefinedScripts> scripts,
                                                const int depth, const bool isAllDocuments, const unsigned int minDoc, const unsigned int maxDoc)
{
    ExtractResults results;
    ExtractionOperation op(&results);
    QString extractFolder(SystemServices::tempLocation());

    QString timeStamp = newTS();
    setupFilterParametersFilterText(&op, extractFolder, timeStamp, fileReference, "", isFilter);
    op.setSplitDepth(depth);
    op.setSplitType(ExtractionOperation::SplitUsingDepth);
    op.setOperationType(isFilter?ExtractionOperation::OperationFilter:ExtractionOperation::OperationSplit);
    if(isAllDocuments) {
        op.setExtractAllDocuments();
    } else {
        op.setExtractRange();
        op.setMinDoc(minDoc);
        op.setMaxDoc(maxDoc);
    }

    QStringList list;
    list.append(timeStamp);
    list.append("%counter%");
    QStringList listF;
    listF.append(timeStamp);
    listF.append("qxmledit_test");
    extractFolder.append(QDir::separator());
    extractFolder.append(timeStamp);
    extractFolder.append("qxmledit_test");
    QStringList filesCandidate;
    FORINT(index, referenceFiles.size() ) {
        QString fileNameN = op.makeAName(extractFolder, 1+index, list, 1);
        fileNameN.append(".xml");
        filesCandidate.append(fileNameN);
    }
    // do operation
    op.setUseNamespaces(useNamespaces);
    QStringList filters;
    foreach(const ExtractionScriptingProvider::EPredefinedScripts script, scripts) {
        filters.append(QString("%1").arg(script));
    }
    op.setFiltersId(filters.join(","));

    op.performExtraction();
    if(op.isError()) {
        return error(QString("Split relative Error: %1 %2").arg(op.error()).arg(op.errorMessage()));
    }
    FORINT(index, referenceFiles.size() ) {
        const QString candidateFile = filesCandidate.at(index);
        const QString referenceFile = referenceFiles.at(index);
        CompareXML compare;
        const bool result = compare.compareFiles(referenceFile, candidateFile);
        if(!result) {
            return error(QString("Filter is not correct for file %4\n%1 found:\n%2\nexpected:\n%3\n")
                         .arg(compare.errorString())
                         .arg(loadTextFile(candidateFile))
                         .arg(loadTextFile(referenceFile))
                         .arg(referenceFile));
        }
    }
    return true ;
}

#endif

bool TestSplit::testScriptingAdvancedOptions()
{
    _testName = "testScriptingAdvancedOptions" ;
    if(!testSelectUseNamespaces()) {
        return false;
    }
#ifdef QXMLEDIT_JS_SCRIPT
    if(!testSelectChooseScriptFromList()) {
        return false;
    }
#endif
    return true ;
}


bool TestSplit::setOptionUseNS(const QString &code, const bool expected, const bool initialValue)
{
    ExtractResults results;
    ExtractionOperation op(&results);
    op.setUseNamespaces(initialValue);
    ExtractionAdavancedOptionsDialog dialog( &op, NULL);
    QCheckBox* cb = dialog.findChild<QCheckBox*>("cbUseNamespaces");
    if(NULL == cb) {
        return error(QString("%1 Check box ns not found").arg(code));
    }
    if(cb->isChecked()!= initialValue) {
        return error(QString("%1 Initial Expected ns %2, found %3").arg(code).arg(initialValue).arg(cb->isChecked()));
    }
    cb->setChecked(expected);
    dialog.accept();
    if(op.isUseNamespaces()!= expected) {
        return error(QString("%1 Expected result ns %2, found %3").arg(code).arg(expected).arg(cb->isChecked()));
    }
    return  true ;
}

bool TestSplit::testSelectUseNamespaces()
{
    _testName = "testSelectUseNamespaces" ;
    if(!setOptionUseNS("0", false, false)) {
        return false;
    }
    if(!setOptionUseNS("1", false, true)) {
        return false;
    }
    if(!setOptionUseNS("2", true, false)) {
        return false;
    }
    if(!setOptionUseNS("3", true, true)) {
        return false;
    }
    return true ;
}

#ifdef QXMLEDIT_JS_SCRIPT

bool TestSplit::setOptionIntScript(const QString &code, const bool expected, const bool initialValue)
{
    ExtractResults results;
    ExtractionOperation op(&results);
    op.setUseNamespaces(initialValue);
    ExtractionAdavancedOptionsDialog dialog( &op, NULL);
    QCheckBox* cb = dialog.findChild<QCheckBox*>("cbUseNamespaces");
    if(NULL == cb) {
        return error(QString("%1 Check box ns not found").arg(code));
    }
    if(cb->isChecked()!= initialValue) {
        return error(QString("%1 Initial Expected ns %2, found %3").arg(code).arg(initialValue).arg(cb->isChecked()));
    }
    cb->setChecked(expected);
    dialog.accept();
    if(op.isUseNamespaces()!= expected) {
        return error(QString("%1 Expected result ns %2, found %3").arg(code).arg(expected).arg(cb->isChecked()));
    }
    return  true ;
}

/*---- tests
id  start  selected  action   result
1    0        0         0       0
2    ab       ab        0       ab
3    ab       ab        none    0
4    0        0         all     ab
5    0        0         none    0
6    a        a         0       a
7    b        b         0       b
8    a        a         all     ab
9    b        b         all     ab
10   a        a         none    0
11   b        b         none    0
-----*/

bool TestSplit::testSelectChooseScriptFromList()
{
    _testName = "testSelectChooseScriptFromList" ;
    QStringList emptyList;
    QStringList aList;
    QStringList bList;
    QStringList abList;

    aList << "-1";
    bList << "-2";
    abList << "-1" << "-2";

    if(     !loadCheckOptionsPredFilters("1", emptyList, emptyList, 0)
            ||!loadCheckOptionsPredFilters("2", abList, abList, 0)
            ||!loadCheckOptionsPredFilters("2", abList, emptyList, -1)
            ||!loadCheckOptionsPredFilters("4", emptyList, abList, 1)
            ||!loadCheckOptionsPredFilters("5", emptyList, emptyList, -1)
            ||!loadCheckOptionsPredFilters("6", aList, aList, 0)
            ||!loadCheckOptionsPredFilters("7", bList, bList, 0)
            ||!loadCheckOptionsPredFilters("8", aList, abList, 1)
            ||!loadCheckOptionsPredFilters("9", bList, abList, 1)
            ||!loadCheckOptionsPredFilters("10", aList, emptyList, -1)
            ||!loadCheckOptionsPredFilters("11", bList, emptyList, -1)
        ) {
        return false;
    }
    return true;
}

bool TestSplit::loadCheckOptionsPredFilters(const QString &code, const QStringList &initialValues, const QStringList &expectedValues, const int selectionAction)
{
    ExtractResults results;
    ExtractionOperation op(&results);
    setupPredFiltersValues(&op, initialValues);
    ExtractionAdavancedOptionsDialog dialog( &op, NULL);
    if(!performScriptSelectDialogAction(&dialog, selectionAction)) {
        return false;
    }
    dialog.accept();
    if(!verifyScriptList(code, &op, expectedValues)) {
        return false;
    }
    return  true ;
}

void TestSplit::setupPredFiltersValues(ExtractionOperation *op, const QStringList &filtersList)
{
    QStringList newList(filtersList);
    newList.sort();
    const QString filters = newList.join(",");
    op->setFiltersId(filters);
}

bool TestSplit::verifyScriptList(const QString &code, ExtractionOperation *op, const QStringList &expectedFiltersList)
{
    QStringList expectedSortedList(expectedFiltersList);
    expectedSortedList.sort();
    QStringList candidateList = op->filterListAsIdList();
    candidateList.sort();
    if(!compareStringList(QString("Verify filters code:%1").arg(code), expectedSortedList, candidateList)) {
        return false;
    }
    return true;
}

bool TestSplit::performScriptSelectDialogAction(ExtractionAdavancedOptionsDialog *dialog, const int selectionAction)
{
    if(selectionAction > 0 ) {
        QPushButton *cmd = dialog->findChild<QPushButton*>("cmdSelectAll");
        if( NULL == cmd ) {
            return error(QString("Select all button not found"));
        }
        cmd->click();
    } else if(selectionAction < 0 ) {
        QPushButton *cmd = dialog->findChild<QPushButton*>("cmdSelectNone");
        if( NULL == cmd ) {
            return error(QString("Select none button not found"));
        }
        cmd->click();
    }
    return true ;
}

bool TestSplit::testScriptingJS()
{
    _testName = "testScriptingJS";
    if(!unitTestScriptingBase()) {
        return false;
    }
    if(!testScriptEmptyActivation()) {
        return false;
    }
    if(!testScriptActivation()) {
        return false;
    }
    if(!testScriptEventText()) {
        return false;
    }
    if(!testScriptEventElement()) {
        return false;
    }
    if(!testScriptWithPredefinedScripting()) {
        return false;
    }
    if(!testScriptWithCase0()) {
        return false;
    }
    return true;
}

bool TestSplit::testScriptWithCase0()
{
    if(!testScriptWithCase0FilterMulti()) {
        return false;
    }
    if(!testScriptWithCase0FilterMultiNoScript()) {
        return false;
    }
    if(!testScriptWithCase0Filter()) {
        return false;
    }
    if(!testScriptWithCase0Split()) {
        return false;
    }
    if(!testScriptWithCase1Split()) {
        return false;
    }
    if(!testScriptWithCase1NoScriptSplit()) {
        return false;
    }
    return true ;
}

bool TestSplit::testScriptWithCase0Filter()
{
    _testName = "testScriptWithCase0Filter";
    QList<ExtractionScriptingProvider::EPredefinedScripts> scripts;
    scripts << ExtractionScriptingProvider::PredefinedScriptTrimAttributes;
    scripts << ExtractionScriptingProvider::PredefinedScriptRemoveEmptyAttributes;
    return testPredefinedScriptExecute(true, true, CASE0_SOURCE, CASE0_RESULT, scripts);
}

bool TestSplit::testScriptWithCase0FilterMulti()
{
    _testName = "testScriptWithCase0FilterMulti";
    QList<ExtractionScriptingProvider::EPredefinedScripts> scripts;
    scripts << ExtractionScriptingProvider::PredefinedScriptTrimAttributes;
    scripts << ExtractionScriptingProvider::PredefinedScriptRemoveEmptyAttributes;
    return testPredefinedScriptExecute(true, true, CASE0MULTI_SOURCE, CASE0MULTI_RESULT, scripts, 2, false, 2, 3);
}

bool TestSplit::testScriptWithCase0FilterMultiNoScript()
{
    _testName = "testScriptWithCase0FilterMultiNoScript";
    QList<ExtractionScriptingProvider::EPredefinedScripts> scripts;
    return testPredefinedScriptExecute(true, true, CASE0MULTI_SOURCE, CASE0MULTINS_RESULT, scripts, 2, false, 2, 3);
}

bool TestSplit::testScriptWithCase0Split()
{
    _testName = "testScriptWithCase0Split";
    QList<ExtractionScriptingProvider::EPredefinedScripts> scripts;
    scripts << ExtractionScriptingProvider::PredefinedScriptTrimAttributes;
    scripts << ExtractionScriptingProvider::PredefinedScriptRemoveEmptyAttributes;
    return testPredefinedScriptExecute(true, false, CASE0_SOURCE, CASE0S_RESULT, scripts, 2, true, 1, 1);
}

bool TestSplit::testScriptWithCase1Split()
{
    _testName = "testScriptWithCase1Split";
    QList<ExtractionScriptingProvider::EPredefinedScripts> scripts;
    scripts << ExtractionScriptingProvider::PredefinedScriptTrimAttributes;
    scripts << ExtractionScriptingProvider::PredefinedScriptRemoveEmptyAttributes;
    QStringList expectedList;
    expectedList << CASE10_RESULT << CASE11_RESULT << CASE12_RESULT ;
    return testPredefinedScriptExecuteList(true, false, CASE1_SOURCE, expectedList, scripts, 2, true, 1, 1);
}

bool TestSplit::testScriptWithCase1NoScriptSplit()
{
    _testName = "testScriptWithCase1NoScriptSplit";
    QList<ExtractionScriptingProvider::EPredefinedScripts> scripts;
    QStringList expectedList;
    expectedList << CASE10NS_RESULT << CASE11NS_RESULT << CASE12NS_RESULT ;
    return testPredefinedScriptExecuteList(true, false, CASE1_SOURCE, expectedList, scripts, 2, true, 1, 1);
}

#endif

bool TestSplit::testScripting()
{
    _testName = "testScripting";
    if(!testScriptingAdvancedOptions()) {
        return false;
    }
#ifdef QXMLEDIT_JS_SCRIPT
    if(!testScriptingJS()) {
        return false;
    }
#endif
    return true;
}

