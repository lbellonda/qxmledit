/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2019 by Luca Bellonda and individual contributors       *
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
#include "extraction/scripting/events/extractionscripttext.h"
#include "helpers/testsplitscriptingoperationhelper.h"

bool TestSplit::unitTestFilterEventText()
{
    _testName = "unitTestFilterEventText" ;
    if(!unitTestFilterEventTextProperties()) {
        return false;
    }
    if(!unitTestFilterEventTextAccessibility()) {
        return false;
    }
    if(!unitTestFilterEventTextMultipleEvents()) {
        return false;
    }
    return true ;
}


bool TestSplit::unitTestFilterEventTextProperties()
{
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventTextProperties/empty";
        ExtractionScriptTextEvent a, b;
        if(!a.compareTo(b)) {
            return error("a not empty 1");
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventTextProperties/CDATA";
        ExtractionScriptTextEvent a, b;
        if(a.isCDATA()) {
            return error("0");
        }
        a.setCDATA(true);
        if(!a.isCDATA()) {
            return error("1");
        }
        if(a.compareTo(b)) {
            return error("3");
        }
        b.setCDATA(true);
        if(!a.compareTo(b)) {
            return error("4");
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventTextProperties/text";
        ExtractionScriptTextEvent a, b;
        if(!a.text().isEmpty()) {
            return error("0");
        }
        a.setText("34");
        if(a.text()!= "34") {
            return error("1");
        }
        if(a.compareTo(b)) {
            return error("1");
        }
        b.setText("34");
        if(!a.compareTo(b)) {
            return error("2");
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventTextProperties/modified";
        ExtractionScriptTextEvent a;
        if(a.isModified()) {
            return error("0");
        }
        a.setText("34");
        if(!a.isModified()) {
            return error("1");
        }
        a.resetModifed();
        if(a.isModified()) {
            return error("2");
        }
        a.setText("36");
        if(!a.isModified()) {
            return error("3");
        }
        a.resetModifed();
        if(a.text()!= "36") {
            return error("3.1");
        }
        // same value
        a.setText("36");
        if(a.isModified()) {
            return error("3.2");
        }
        a.setCDATA(true);
        if(!a.isModified()) {
            return error("4");
        }
        a.resetModifed();
        if(a.isModified()) {
            return error("5");
        }
        // same value
        a.setCDATA(true);
        if(a.isModified()) {
            return error("6");
        }
        a.resetModifed();
        a.setCDATA(false);
        if(!a.isModified()) {
            return error("7");
        }
        a.resetModifed();
        a.setCDATA(false);
        if(a.isModified()) {
            return error("8");
        }
        a.resetModifed();
        a.setIgnored(false);
        if(a.isModified()) {
            return error("10");
        }
        a.setIgnored(true);
        if(!a.isModified()) {
            return error("11");
        }
        a.resetModifed();
        a.setIgnored(true);
        if(a.isModified()) {
            return error("12");
        }
        a.resetModifed();
        a.setError(true);
        if(!a.isModified()) {
            return error("13");
        }
        a.setError(false);
        if(!a.isModified()) {
            return error("14");
        }
        a.resetModifed();
        a.setErrorMessage("xxx");
        if(!a.isModified()) {
            return error("15");
        }

    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventTextProperties/ignored";
        ExtractionScriptTextEvent a, b;
        if(a.isIgnored()) {
            return error("0");
        }
        a.setIgnored(true);
        if(!a.isIgnored()) {
            return error("1");
        }
        if(a.compareTo(b)) {
            return error("2");
        }
        b.setIgnored(true);
        if(!a.compareTo(b)) {
            return error("4");
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventTextProperties/error";
        ExtractionScriptTextEvent a, b;
        if(a.isError()) {
            return error("0");
        }
        a.setError(true);
        if(!a.isError()) {
            return error("1");
        }
        if(a.compareTo(b)) {
            return error("2");
        }
        b.setError(true);
        if(!a.compareTo(b)) {
            return error("4");
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventTextProperties/errorMessage";
        ExtractionScriptTextEvent a, b;
        if(!a.errorMessage().isEmpty()) {
            return error("0");
        }
        a.setErrorMessage("x");
        if(a.errorMessage()!="x") {
            return error("1");
        }
        if(a.compareTo(b)) {
            return error("2");
        }
        b.setErrorMessage("x");
        if(!a.compareTo(b)) {
            return error("4");
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventTextProperties/whitespace";
        ExtractionScriptTextEvent a, b;
        if(a.isWhitespace()) {
            return error("0");
        }
        a.setWhiteSpaceFlag(true);
        if(!a.isWhitespace()) {
            return error("1");
        }
        a.setWhiteSpaceFlag(false);
        if(a.isWhitespace()) {
            return error("2");
        }
        // no compare
    }
    return true ;
}

bool TestSplit::checkText(const int code, ExtractionScriptTextEvent *textEvent, const bool expectedCDATA, const QString &expectedText, const bool expectedIgnored, const bool expectedModified)
{
    if(textEvent->isIgnored() != expectedIgnored) {
        return error(QString("Text event %1 ignored expected %2, found %3 ").arg(code).arg(expectedIgnored).arg(textEvent->isIgnored()));
    }
    if(textEvent->isModified() != expectedModified) {
        return error(QString("Text event %1 modified expected %2, found %3 ").arg(code).arg(expectedModified).arg(textEvent->isModified()));
    }
    if(textEvent->isCDATA() != expectedCDATA) {
        return error(QString("Text event %1 CDATA expected %2, found %3 ").arg(code).arg(expectedCDATA).arg(textEvent->isCDATA()));
    }
    if(textEvent->text() != expectedText) {
        return error(QString("Text event %1 text expected '%2', found '%3' ").arg(code).arg(expectedText).arg(textEvent->text()));
    }
    return true ;
}

bool TestSplit::unitTestFilterEventTextAccessibility()
{
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventTextAccessibility/empty";
        TestExtractionExecutorHelper helper ;
        if(!helper.executeText("var a =1;", true, "a")) {
            return error("1");
        }
        if(!checkText(0, helper.textEvent, true, "a", false, false)) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventTextAccessibility/one";
        TestExtractionExecutorHelper helper ;
        if(!helper.executeText("event.text = 'vbb'", false, "a")) {
            return error("0");
        }
        if(!checkText(0, helper.textEvent, false, "vbb", false, true)) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventTextAccessibility/two";
        TestExtractionExecutorHelper helper ;
        if(!helper.executeText("event.isCDATA = false", true, "a")) {
            return error("0");
        }
        if(!checkText(0, helper.textEvent, false, "a", false, true)) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventTextAccessibility/three";
        TestExtractionExecutorHelper helper ;
        if(!helper.executeText("event.isCDATA = true; event.text='xcc'", false, "a")) {
            return error("0");
        }
        if(!checkText(0, helper.textEvent, true, "xcc", false, true)) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventTextAccessibility/Four";
        TestExtractionExecutorHelper helper ;
        if(!helper.executeText("event.ignored = true; ", false, "a")) {
            return error("0");
        }
        if(!checkText(0, helper.textEvent, false, "a", true, true)) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventTextAccessibility/Four1";
        TestExtractionExecutorHelper helper ;
        if(!helper.executeText("event.ignored = true; event.ignored = false; ", false, "a")) {
            return error("0");
        }
        if(!checkText(0, helper.textEvent, false, "a", false, true)) {
            return false;
        }
    }
    return true ;
}

bool TestSplit::checkTextMultiple(const int code, TestSplitScriptingOperationHelper &helper, ExtractionScriptManager::EEventResult expectedResult, ExtractionScriptTextEvent *textEvent, const bool expectedCDATA, const QString &expectedText, const bool expectedIgnored, const bool expectedModified, const int expectedTextEventsCount)
{
    if(helper.opResult != expectedResult) {
        return error(QString("Text event %1 result expected %2, found %3").arg(code).arg(expectedResult).arg(helper.opResult));
    }
    if(helper.op.scriptManager()->eventsCount() != expectedTextEventsCount ) {
        return error(QString("Events fired %1 result expected %2, found %3").arg(code).arg(expectedTextEventsCount).arg(helper.op.scriptManager()->eventsCount()));
    }
    if(helper.op.scriptManager()->textEventsCount() != expectedTextEventsCount ) {
        return error(QString("Text events fired %1 result expected %2, found %3").arg(code).arg(expectedTextEventsCount).arg(helper.op.scriptManager()->textEventsCount()));
    }
    if(textEvent->isIgnored() != expectedIgnored) {
        return error(QString("Text event %1 ignored expected %2, found %3 ").arg(code).arg(expectedIgnored).arg(textEvent->isIgnored()));
    }
    if(textEvent->isModified() != expectedModified) {
        return error(QString("Text event %1 modified expected %2, found %3 ").arg(code).arg(expectedModified).arg(textEvent->isModified()));
    }
    if(textEvent->isCDATA() != expectedCDATA) {
        return error(QString("Text event %1 CDATA expected %2, found %3 ").arg(code).arg(expectedCDATA).arg(textEvent->isCDATA()));
    }
    if(textEvent->text() != expectedText) {
        return error(QString("Text event %1 text expected '%2', found '%3' ").arg(code).arg(expectedText).arg(textEvent->text()));
    }
    return true ;
}

const QString StartText("abc d");

bool TestSplit::singleEventTextMultipleEvents(const QString & testName, const int code, const QString & codeEvent1, const QString & codeEvent2, ExtractionScriptManager::EEventResult expectedResult, const bool expectedCDATA, const QString &expectedText, const bool expectedIgnored, const bool expectedModified, const int expectedTextEventsCount)
{
    _subTestName = testName;
    TestSplitScriptingOperationHelper helper ;
    helper.executeMultipleText(codeEvent1, codeEvent2, 1, "/root/a", StartText);
    if( !checkTextMultiple(code, helper, expectedResult, &helper.textEvent, expectedCDATA,
                           expectedText, expectedIgnored, expectedModified, expectedTextEventsCount) ) {
        return false;
    }
    return true ;
}

bool TestSplit::unitTestFilterEventTextMultipleEvents()
{
    // err, err:0.
    // go, go:1.
    // ignore, ignore:2.
    // go, err: 3.
    // err, go: 4
    // err, ignore: 5
    // ignore, err: 6
    // go, ignore : 6
    // ignore, go: 7
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventTextMultipleEvents/err-err";
        if( !singleEventTextMultipleEvents(_subTestName, 0, "event.isError = true ;", "event.isError = true ;", ExtractionScriptManager::EventResult_Error, false, StartText, false, true, 1) ) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventTextMultipleEvents/go-go";
        if( !singleEventTextMultipleEvents(_subTestName, 1, "event.text = 'a' ;event.isCDATA= true", "event.text = 'b' ;", ExtractionScriptManager::EventResult_WriteModifiedData, true, "b", false, true, 2) ) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventTextMultipleEvents/ignore-ignore";
        if( !singleEventTextMultipleEvents(_subTestName, 2, "event.ignored = true ;", "event.ignored = true ;", ExtractionScriptManager::EventResult_IgnoreEvent, false, StartText, true, true, 1) ) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventTextMultipleEvents/go-err";
        if( !singleEventTextMultipleEvents(_subTestName, 3, "event.text = 'a' ;", "event.isError = true ;", ExtractionScriptManager::EventResult_Error, false, "a", false, true, 2) ) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventTextMultipleEvents/err-go";
        if( !singleEventTextMultipleEvents(_subTestName, 4, "event.isError = true ;", "event.text = 'a' ;", ExtractionScriptManager::EventResult_Error, false, StartText, false, true, 1) ) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventTextMultipleEvents/err-ignore";
        if( !singleEventTextMultipleEvents(_subTestName, 5, "event.isError = true ;", "event.ignored = true ;", ExtractionScriptManager::EventResult_Error, false, StartText, false, true, 1) ) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventTextMultipleEvents/ignore-err";
        if( !singleEventTextMultipleEvents(_subTestName, 6, "event.ignored = true ;", "event.isError = true ;", ExtractionScriptManager::EventResult_IgnoreEvent, false, StartText, true, true, 1) ) {
            return false;
        }
    }
    // go, ignore : 6
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventTextMultipleEvents/go-ignore";
        if( !singleEventTextMultipleEvents(_subTestName, 6, "event.text = 'a' ;", "event.ignored = true ;", ExtractionScriptManager::EventResult_IgnoreEvent, false, "a", true, true, 2) ) {
            return false;
        }
    }
    // ignore, go: 7
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventTextMultipleEvents/ignore-go";
        if( !singleEventTextMultipleEvents(_subTestName, 7, "event.ignored = true ;", "event.text = 'a' ;", ExtractionScriptManager::EventResult_IgnoreEvent, false, StartText, true, true, 1) ) {
            return false;
        }
    }
    return true ;
}
#endif
