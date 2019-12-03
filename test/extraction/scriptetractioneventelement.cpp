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
#include "extraction/scripting/events/extractionscriptelement.h"

static const QString EventVariable = "varOfEvent" ;

bool TestSplit::unitTestFilterEventElement()
{
    _testName = "unitTestFilterEventElement" ;
    if(!unitTestFilterEventElementProperties()) {
        return false;
    }
    if(!unitTestFilterEventElementAccessibility()) {
        return false;
    }
    if(!unitTestOperationBase()) {
        return false;
    }
    if(!unitTestFilterEventElementMultipleEvents()) {
        return false;
    }
    return true ;
}

bool TestSplit::checkElementEventBase(const int code, ExtractionScriptElementEvent *elementEvent, QList<QPair<QString,QString> > inputAttributes, const bool expectedModified)
{
    if(elementEvent->isModified() != expectedModified) {
        return error(QString("Element event %1 modified expected %2, found %3 ").arg(code).arg(expectedModified).arg(elementEvent->isModified()));
    }
    QList<QString> expectedAttributesSorted;
    QHash<QString,QString> expectedAttributes;
    QPair<QString,QString> inputAttribute;
    foreach( inputAttribute, inputAttributes ) {
        expectedAttributesSorted << inputAttribute.first ;
        expectedAttributes[inputAttribute.first] = inputAttribute.second;
    }
    if(!compareAttributes(code, elementEvent, expectedAttributesSorted, expectedAttributes)) {
        return false;
    }
    return true;
}

bool TestSplit::checkElementEventWithEngine(const int code, QJSEngine &engine, ExtractionScriptElementEvent *elementEvent, QList<QPair<QString,QString> > inputAttributes, const bool expectedModified, const QString &expectedVariableValue)
{
    if(!checkElementEventBase(code, elementEvent, inputAttributes, expectedModified) ) {
        return false;
    }
    QJSValue variable = engine.evaluate(EventVariable);
    Utils::TODO_NEXT_RELEASE("abilitare variable.errorType() ove presente");
    if(variable.isError()) {
        return error(QString("Error evaluating variable %1")
                     .arg(EventVariable));
    }
    const QString & measuredValue = variable.toString();
    if(measuredValue != expectedVariableValue ) {
        return error(QString("Error evaluating variable %1 expected: '%2' found: '%3'").arg(EventVariable).arg(expectedVariableValue).arg(measuredValue));
    }
    return true ;
}

bool TestSplit::checkElementEventWithReturnCode(const int code, TestSplitScriptingOperationHelper &helper, ExtractionScriptElementEvent *elementEvent, QList<QPair<QString,QString> > inputAttributes, const bool expectedModified, const ExtractionScriptManager::EEventResult expectedResult, const int expectedElementEventsCount )
{
    if(!checkElementEventBase(code, elementEvent, inputAttributes, expectedModified) ) {
        return false;
    }
    if(helper.op.scriptManager()->eventsCount() != expectedElementEventsCount ) {
        return error(QString("Events fired %1 result expected %2, found %3").arg(code).arg(expectedElementEventsCount).arg(helper.op.scriptManager()->eventsCount()));
    }
    if(helper.op.scriptManager()->elementEventsCount() != expectedElementEventsCount ) {
        return error(QString("Element events fired %1 result expected %2, found %3").arg(code).arg(expectedElementEventsCount).arg(helper.op.scriptManager()->elementEventsCount()));
    }
    if( helper.opResult != expectedResult) {
        return error(QString("For code: %1 result expected:%2, found: %3").arg(code).arg(expectedResult).arg(helper.opResult));
    }
    return true ;
}

bool TestSplit::compareAttributes(const int code, ExtractionScriptElementEvent *elementEvent, QList<QString> expectedAttributesSorted, QHash<QString,QString> expectedAttributes)
{
    // input validation
    if(expectedAttributesSorted.size() != expectedAttributes.size()) {
        return error(QString("Input validation: For test %1 expected lists size differs. Expected: %2 found: %3")
                     .arg(code)
                     .arg(expectedAttributesSorted.size()).arg(expectedAttributes.size()));
    }
    foreach( QString key, expectedAttributesSorted ) {
        if(!expectedAttributes.contains(key)) {
            return error(QString("Input validation: For test %1 sorted key '%2' not in map.")
                         .arg(code).arg(key));
        }
    }
    // end input validation
    if(elementEvent->attributesCount() != expectedAttributes.size()) {
        return error(QString("For test %1 attributeCount is %2 expected %3")
                     .arg(code)
                     .arg(elementEvent->attributesCount()).arg(expectedAttributes.size()));
    }

    FORINT(attributeIndex, expectedAttributes.size()) {
        const QString &expectedAttributeName = expectedAttributesSorted.at(attributeIndex);
        const QString &foundAttributeName = elementEvent->attributeNameByIndex(attributeIndex);
        if(expectedAttributeName != foundAttributeName) {
            return error(QString("For test %1 at index %2 expected name '%3', found '%4'")
                         .arg(code).arg(attributeIndex)
                         .arg(expectedAttributeName).arg(foundAttributeName));
        }
        const QString &expectedAttributeValue = expectedAttributes[expectedAttributeName];
        const QString &foundAttributeValue = elementEvent->attributeValueByIndex(attributeIndex);
        if(expectedAttributeValue != foundAttributeValue) {
            return error(QString("For test %1 at index %2 attribute name: '%3' expected value '%4', found '%5'")
                         .arg(code).arg(attributeIndex).arg(expectedAttributeName)
                         .arg(expectedAttributeValue).arg(foundAttributeValue));
        }
    }
    return true;
}

bool TestSplit::compareAttributesNS(const int code, ExtractionScriptElementEvent *elementEvent,
                                    QList<QString> expectedAttributeNamespaces,
                                    QList<QString> expectedAttributeNames,
                                    QList<QString> expectedAttributeValues)
{
    // input validation
    if(expectedAttributeNamespaces.size() != expectedAttributeNames.size()) {
        return error(QString("Input validation: For test %1 expected lists size differs. Expected: %2 found: %3")
                     .arg(code)
                     .arg(expectedAttributeNamespaces.size()).arg(expectedAttributeNames.size()));
    }
    if(expectedAttributeNamespaces.size() != expectedAttributeValues.size()) {
        return error(QString("Input validation: For test %1 expected lists size differs. Expected: %2 found: %3")
                     .arg(code)
                     .arg(expectedAttributeNamespaces.size()).arg(expectedAttributeValues.size()));
    }
    // end input validation
    if(elementEvent->attributesCount() != expectedAttributeNamespaces.size()) {
        return error(QString("For test %1 attributeCount is %2 expected %3")
                     .arg(code)
                     .arg(elementEvent->attributesCount()).arg(expectedAttributeNamespaces.size()));
    }

    FORINT(attributeIndex, expectedAttributeNames.size()) {
        const QString &expectedAttributeName = expectedAttributeNames.at(attributeIndex);
        const QString &expectedAttributeNameSpace = expectedAttributeNamespaces.at(attributeIndex);
        const QString &foundAttributeName = elementEvent->attributeNameByIndex(attributeIndex);
        const QString &foundAttributeNameSpace = elementEvent->attributeNameSpaceByIndex(attributeIndex);
        if(expectedAttributeName != foundAttributeName) {
            return error(QString("For test %1 at index %2 expected name '%3', found '%4'")
                         .arg(code).arg(attributeIndex)
                         .arg(expectedAttributeName).arg(foundAttributeName));
        }
        if(expectedAttributeNameSpace != foundAttributeNameSpace) {
            return error(QString("For test %1 at index %2 expected namespace '%3', found '%4'")
                         .arg(code).arg(attributeIndex)
                         .arg(expectedAttributeNameSpace).arg(foundAttributeNameSpace));
        }
        const QString &expectedAttributeValue = expectedAttributeValues.at(attributeIndex);
        const QString &foundAttributeValue = elementEvent->attributeValueByIndex(attributeIndex);
        if(expectedAttributeValue != foundAttributeValue) {
            return error(QString("For test %1 at index %2 attribute name: '%3' expected value '%4', found '%5'")
                         .arg(code).arg(attributeIndex).arg(expectedAttributeName)
                         .arg(expectedAttributeValue).arg(foundAttributeValue));
        }
    }
    return true;
}

static QList<QPair<QString,QString> > stdElementAttributes()
{
    QList<QPair<QString,QString> > sourceList ;
    sourceList << QPair<QString, QString>("a", "b");
    sourceList << QPair<QString, QString>("a1", "b1");
    sourceList << QPair<QString, QString>("a2", "b2");
    return sourceList ;
}

bool TestSplit::unitTestFilterEventElementProperties()
{
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementProperties/attr/empty";
        ExtractionScriptAttribute a, b;
        if(!a.compareTo(&b)) {
            return error("a not empty 1");
        }
        if(a.name != "" ) {
            return error("e0");
        }
        if(a.nameSpace != "" ) {
            return error("e1");
        }
        if(a.value != "" ) {
            return error("e2");
        }
        a.name = "aa";
        if(a.name != "aa" ) {
            return error("e3");
        }
        if(a.compareTo(&b)) {
            return error("e3.1");
        }
        b.name = "aa";
        a.nameSpace = "bb";
        if(a.nameSpace != "bb" ) {
            return error("e4");
        }
        if(a.compareTo(&b)) {
            return error("e4.1");
        }
        b.nameSpace = "bb";
        a.value = "cc";
        if(a.value != "cc" ) {
            return error("e5");
        }
        if(a.compareTo(&b)) {
            return error("e5.1");
        }
        b.value = "cc";
        if(!a.compareTo(&b)) {
            return error("e5");
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementProperties/empty";
        ExtractionScriptElementEvent a, b;
        if(a.isModified()) {
            return error("p0");
        }
        if(a.attributesCount()>0) {
            return error("c0");
        }
        /*if(!a.attributeNames().isEmpty()) {
            return error("e0");
        }*/
        if(!a.elementName().isEmpty()) {
            return error("f0");
        }
        if(!a.nameSpace().isEmpty()) {
            return error("f1");
        }
        if(!a.localName().isEmpty()) {
            return error("g0");
        }
        if(!a.compareTo(b)) {
            return error("a not empty 1");
        }
        if(a.isError()) {
            return error("isError");
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementProperties/elementName";
        ExtractionScriptElementEvent a, b;
        if(a.isModified()) {
            return error("e0");
        }
        if(a.elementName()!="") {
            return error("e1");
        }
        a.setElementName("aaaa");
        if(a.isModified()) {
            return error("e2");
        }
        if(a.elementName() != "aaaa") {
            return error("e3");
        }
        if(!a.nameSpace().isEmpty()) {
            return error("e3-1");
        }
        if(!a.localName().isEmpty()) {
            return error("e3-2");
        }
        if(a.isModified()) {
            return error("e4");
        }
        if(a.compareTo(b)) {
            return error("e5");
        }
        b.setElementName("ccc");
        if(a.compareTo(b)) {
            return error("e6");
        }
        b.setElementName("aaaa");
        if(!a.compareTo(b)) {
            return error("e7");
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementProperties/localName";
        ExtractionScriptElementEvent a, b;
        if(a.isModified()) {
            return error("e0");
        }
        if(a.localName()!="") {
            return error("e1");
        }
        a.setLocalName("aaaa");
        if(a.isModified()) {
            return error("e2");
        }
        if(a.localName() != "aaaa") {
            return error("e3");
        }
        if(!a.nameSpace().isEmpty()) {
            return error("e3-1");
        }
        if(!a.elementName().isEmpty()) {
            return error("e3-2");
        }
        if(a.isModified()) {
            return error("e4");
        }
        if(a.compareTo(b)) {
            return error("e5");
        }
        b.setLocalName("ccc");
        if(a.compareTo(b)) {
            return error("e6");
        }
        b.setLocalName("aaaa");
        if(!a.compareTo(b)) {
            return error("e7");
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementProperties/namespace";
        ExtractionScriptElementEvent a, b;
        if(a.isModified()) {
            return error("e0");
        }
        if(a.nameSpace()!="") {
            return error("e1");
        }
        a.setNameSpace("aaaa");
        if(a.isModified()) {
            return error("e2");
        }
        if(a.nameSpace() != "aaaa") {
            return error("e3");
        }
        if(!a.elementName().isEmpty()) {
            return error("e3-1");
        }
        if(!a.localName().isEmpty()) {
            return error("e3-2");
        }
        if(a.isModified()) {
            return error("e4");
        }
        if(a.compareTo(b)) {
            return error("e5");
        }
        b.setNameSpace("ccc");
        if(a.compareTo(b)) {
            return error("e6");
        }
        b.setNameSpace("aaaa");
        if(!a.compareTo(b)) {
            return error("e7");
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementProperties/iserror";
        ExtractionScriptElementEvent a, b;
        a.setError(true);
        if(!a.isError()) {
            return error("e0");
        }
        if(a.compareTo(b)) {
            return error("e1");
        }
        a.resetError();
        if(a.isError()) {
            return error("e2");
        }
        a.setError(true);
        b.setError(true);
        if(!a.compareTo(b)) {
            return error("e3");
        }
    }
    FORINT(i, 2)
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementProperties/setAttrByName0";
        ExtractionScriptElementEvent a, b;
        bool useNS = false ;
        if(1==i) {
            useNS = true ;
        }

        if(a.isModified()) {
            return error("m0");
        }
        if(a.attributesCount()) {
            return error("0");
        }
        a.setUseNamespaces(useNS);
        b.setUseNamespaces(useNS);
        a.resetModifed();
        b.resetModifed();
        if(a.attributeValueByName("a")!="") {
            return error("v0");
        }
        a.setAttributeValueByName("a", "b");
        if(!a.isModified()) {
            return error("m");
        }
        if(a.isError()) {
            return error("m,a");
        }
        a.resetModifed();
        if(a.isModified()) {
            return error("m1");
        }
        if(a.attributeValueByName("a")!="b") {
            return error("v1");
        }
        if(a.attributeValueByName("axx")!="") {
            return error("v2");
        }
        if(a.attributesCount()!=1) {
            return error("1");
        }
        if(a.compareTo(b)) {
            return error("2");
        }
        int attrCount = a.removeAttributeByName("a");
        if(!a.isModified()) {
            return error("m3");
        }
        if(a.isError()) {
            return error("m,ab");
        }
        if(attrCount!=0) {
            return error("c0");
        }
        a.resetModifed();
        if(!a.compareTo(b)) {
            return error("4");
        }
        a.setAttributeValueByName("a", "b");
        a.setAttributeValueByName("a1", "b1");
        a.setAttributeValueByName("a2", "b2");
        if(a.isError()) {
            return error("m,abc");
        }
        if(!a.isModified()) {
            return error("m4");
        }
        if(a.attributesCount()!=3) {
            return error("c1");
        }
        if(a.attributeValueByName("a")!="b") {
            return error("v3");
        }
        if(a.attributeValueByName("a1")!="b1") {
            return error("v4");
        }
        if(a.attributeValueByName("a2")!="b2") {
            return error("v5");
        }

        b.setAttributeValueByName("a", "b");
        b.setAttributeValueByName("a1", "b1");
        b.setAttributeValueByName("a2", "b2");
        if(!a.compareTo(b)) {
            return error("5");
        }
        a.resetModifed();
        a.setAttributeValueByName("a2", "b2");
        if(a.isModified()) {
            return error("v5-1");
        }
        a.resetModifed();
        const int attrC2 = a.removeAttributeByName("a1");
        if(a.isError()) {
            return error("m,awe");
        }
        if(attrC2!=2) {
            return error("1-2");
        }
        if(!a.isModified()) {
            return error("1-3");
        }
        a.resetModifed();
        const int attrC2a = a.removeAttributeByName("zzzz");
        if(attrC2a!=2) {
            return error("1-4");
        }
        if(!a.isError()) {
            return error("m,abd");
        }
        if(a.isModified()) {
            return error("1-5");
        }
        a.resetError();
        {
            QList<QString> attrList;
            attrList << "a" << "a2" ;
            QHash<QString,QString> map;
            map["a"] = "b" ;
            map["a2"] = "b2" ;
            if(!compareAttributes(0, &a, attrList, map )) {
                return false;
            }
        }
    }
    FORINT(i, 2)
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementProperties/setAttrByIndex0";
        ExtractionScriptElementEvent a, b;
        bool useNS = false ;
        if(1==i) {
            useNS = true ;
        }
        if(a.isModified()) {
            return error("m0");
        }
        if(a.attributesCount()) {
            return error("0");
        }
        if(a.attributeNameByIndex(0)!= "") {
            return error("1");
        }
        if(a.attributeNameByIndex(1)!= "") {
            return error("2");
        }
        if(a.attributeValueByIndex(0)!= "") {
            return error("3");
        }
        if(a.attributeValueByIndex(1)!= "") {
            return error("4");
        }
        a.setUseNamespaces(useNS);
        b.setUseNamespaces(useNS);
        a.resetModifed();
        b.resetModifed();
        a.setAttributeValueByName("ca", "cb");
        a.setAttributeValueByName("ca1", "cb1");
        a.setAttributeValueByName("ca2", "cb2");
        a.resetModifed();
        if(a.isModified()) {
            return error("5");
        }
        if(a.attributesCount()!=3) {
            return error("6");
        }
        if(a.attributeNameByIndex(0)!= "ca") {
            return error("7");
        }
        if(a.attributeNameByIndex(1)!= "ca1") {
            return error("8");
        }
        if(a.attributeValueByIndex(0)!= "cb") {
            return error("9");
        }
        if(a.attributeValueByIndex(1)!= "cb1") {
            return error("10");
        }
        a.resetModifed();
        a.setAttributeValueByIndex(1, "cb1");
        if(a.isModified()) {
            return error("10-1");
        }
        a.resetModifed();
        a.setAttributeNameByIndex(0, "a");
        if(!a.isModified()) {
            return error("13");
        }
        if(a.attributeNameByIndex(0)!= "a") {
            return error("14");
        }
        a.resetModifed();
        a.setAttributeValueByIndex(0, "b");
        if(!a.isModified()) {
            return error("11");
        }
        if(a.attributeValueByIndex(0)!= "b") {
            return error("12");
        }
        a.resetModifed();
        a.setAttributeNameByIndex(1, "a1");
        if(!a.isModified()) {
            return error("15");
        }
        if(a.attributeNameByIndex(1)!= "a1") {
            return error("16");
        }
        a.resetModifed();
        a.setAttributeValueByIndex(1, "b1");
        if(a.attributeValueByIndex(1)!= "b1") {
            return error("17");
        }
        a.resetModifed();
        a.setAttributeValueByIndex(10, "xx");
        if(a.attributeValueByIndex(10)!= "") {
            return error("18");
        }
        if(!a.isError()) {
            return error("18.1");
        }
        if(a.isModified()) {
            return error("19");
        }
        a.resetError();
        a.setAttributeNameByIndex(10, "xx");
        if(a.attributeNameByIndex(10)!= "") {
            return error("20");
        }
        if(a.isModified()) {
            return error("21");
        }
        b.setAttributeValueByName("a", "b");
        b.setAttributeValueByName("a1", "b1");
        b.setAttributeValueByName("ca1", "cb1");
        a.resetModifed();
        b.resetModifed();
        if(a.compareTo(b)) {
            return error("22");
        }
        a.setAttributeNameByIndex(100, "xx");
        if(!a.isError()) {
            return error("23");
        }
        a.resetError();
        {
            QList<QString> attrList;
            attrList << "a" << "a1" << "ca2";
            QHash<QString,QString> map;
            map["a"] = "b" ;
            map["a1"] = "b1" ;
            map["ca2"] = "cb2" ;
            if(!compareAttributes(0, &a, attrList, map )) {
                return false;
            }
        }
        a.setAttributeNameByIndex(1, "aaa");
        if(a.isError()) {
            return error("24");
        }
        a.setAttributeNameByIndex(0, "aaa");
        if(!a.isError()) {
            return error("25");
        }

    }
    FORINT(i, 2)
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementProperties/removeAttributeByIndex";
        ExtractionScriptElementEvent a, b;
        bool useNS = false ;
        if(1==i) {
            useNS = true ;
        }
        if(a.isModified()) {
            return error("m0");
        }
        a.setUseNamespaces(useNS);
        b.setUseNamespaces(useNS);
        a.resetModifed();
        b.resetModifed();
        a.setAttributeValueByName("a", "b");
        a.setAttributeValueByName("a1", "b1");
        a.setAttributeValueByName("ca1", "cb1");
        if(a.attributesCount()!=3) {
            return error("0");
        }
        int count = a.removeAttributeByIndex(1);
        if(count != 2 ) {
            return error("1");
        }
        if(!a.isModified()) {
            return error("2");
        }
        a.resetModifed();
        count = a.removeAttributeByIndex(20);
        if(count != 2 ) {
            return error("3");
        }
        if(a.isModified()) {
            return error("4");
        }
        if(!a.isError()) {
            return error("4.1");
        }
        count = a.removeAttributeByIndex(-1);
        if(count != 2 ) {
            return error("5");
        }
        if(a.isModified()) {
            return error("6");
        }
        b.setAttributeValueByName("a", "b");
        b.setAttributeValueByName("ca1", "cb1");
        a.resetModifed();
        b.resetModifed();
        a.resetError();
        if(!a.compareTo(b)) {
            return error("7");
        }
        {
            QList<QString> attrList;
            attrList << "a" << "ca1";
            QHash<QString,QString> map;
            map["a"] = "b" ;
            map["ca1"] = "cb1" ;
            if(!compareAttributes(0, &a, attrList, map )) {
                return false;
            }
        }
        count = a.removeAttributeByIndex(2);
        if(count != 2 ) {
            return error("8-1");
        }
        count = a.removeAttributeByIndex(1);
        if(count != 1 ) {
            return error("8");
        }
        if(!a.isModified()) {
            return error("9");
        }
        a.resetModifed();
        count = a.removeAttributeByIndex(0);
        if(count != 0 ) {
            return error("10");
        }
        if(!a.isModified()) {
            return error("11");
        }
        a.resetError();
        a.resetModifed();
        count = a.removeAttributeByIndex(3);
        if(a.isModified()) {
            return error("12");
        }
        if(!a.isError()) {
            return error("13");
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementProperties/nsToNoNs";
        ExtractionScriptElementEvent a;
        a.setUseNamespaces(false);
        const QString res0a = a.attributeNameSpaceByIndex(0);
        if(!a.isError()) {
            return error("0.1");
        }
        a.resetError();
        a.setAttributeValueByNameNS("a", "b", "c");
        if(!a.isError()) {
            return error("0");
        }
        a.setAttributeValueByName("a", "b");
        a.resetError();
        const QString res0 = a.attributeNameSpaceByIndex(0);
        if(!a.isError()) {
            return error("1");
        }
        a.resetError();
        a.setAttributeNameByIndexNS(0, "c", "s");
        if(!a.isError()) {
            return error("2");
        }
        a.resetError();
        a.attributeValueByNameNS("a", "b");
        if(!a.isError()) {
            return error("3");
        }
        a.resetError();
        a.removeAttributeByNameNS("a", "b");
        if(!a.isError()) {
            return error("4");
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementProperties/addAttrNons";
        ExtractionScriptElementEvent a;
        a.setUseNamespaces(false);
        QXmlStreamAttribute attribute1("qnam:e1", "val1");
        a.setAttribute(&attribute1);
        if(a.isError()) {
            return error("0");
        }
        if(!a.isModified()) {
            return error("1");
        }
        QXmlStreamAttribute attribute2("x:e2", "val2");
        a.setAttribute(&attribute2);
        if(a.isError()) {
            return error("2");
        }
        a.resetError();
        QList<QString> attrList;
        attrList << "qnam:e1" << "x:e2" ;
        QHash<QString,QString> map;
        map["qnam:e1"] = "val1" ;
        map["x:e2"] = "val2" ;
        if(!compareAttributes(3, &a, attrList, map )) {
            return false;
        }
        QXmlStreamAttribute attribute3;
        a.setAttribute(&attribute3);
        if(!a.isError()) {
            return error("4");
        }
        if(!compareAttributes(5, &a, attrList, map )) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementProperties/addAttrNs";
        ExtractionScriptElementEvent a;
        a.setUseNamespaces(true);
        QXmlStreamAttribute attribute1("x", "e1", "val1");
        a.setAttribute(&attribute1);
        if(a.isError()) {
            return error("0");
        }
        if(!a.isModified()) {
            return error("1");
        }
        QXmlStreamAttribute attribute2("y", "e2", "val2");
        a.setAttribute(&attribute2);
        if(a.isError()) {
            return error("2");
        }
        a.resetError();
        QList<QString> namesList, nsList, valuesList;
        namesList << "e1" << "e2" ;
        nsList << "x" << "y" ;
        valuesList << "val1" << "val2" ;
        if(!compareAttributesNS(3, &a, nsList, namesList, valuesList )) {
            return false;
        }
        QXmlStreamAttribute attribute3;
        a.setAttribute(&attribute3);
        if(!a.isError()) {
            return error("4");
        }
        if(!compareAttributesNS(5, &a, nsList, namesList, valuesList )) {
            return false;
        }
        a.resetError();
        QXmlStreamAttribute attributenns("qname1", "valnns");
        a.setAttribute(&attributenns);
        if(a.isError()) {
            return error("5");
        }
        namesList << "qname1" ;
        nsList << "";
        valuesList << "valnns" ;
        if(!compareAttributesNS(6, &a, nsList, namesList, valuesList )) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementProperties/sortAttributByName0";
        ExtractionScriptElementEvent a, b;
        a.setAttributeValueByName("z", "1");
        a.setAttributeValueByName("H", "2");
        a.setAttributeValueByName("a", "3");
        a.setAttributeValueByName("ns:A", "4");
        a.setAttributeValueByName("B", "5");
        a.resetModifed();
        a.sortAttributesByName();
        if(!a.isModified()) {
            return error("0");
        }
        {
            QList<QString> attrList;
            attrList << "a" << "B" << "H" << "ns:A" << "z";
            QHash<QString,QString> map;
            map["a"] = "3" ;
            map["ns:A"] = "4" ;
            map["B"] = "5" ;
            map["H"] = "2" ;
            map["z"] = "1" ;
            if(!compareAttributes(0, &a, attrList, map )) {
                return false;
            }
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementProperties/sortAttributesByNameExcludingPrefix";
        ExtractionScriptElementEvent a;
        a.setUseNamespaces(true);
        a.setAttributeValueByNameNS("m", "z", "1");
        a.setAttributeValueByNameNS("m", "H", "2");
        a.setAttributeValueByNameNS("a", "a", "3");
        a.setAttributeValueByNameNS("ns", "b", "4");
        a.setAttributeValueByNameNS("", "C", "5");
        a.resetModifed();
        a.sortAttributesByNamespaceAndName();
        if(!a.isModified()) {
            return error("0");
        }
        {
            QList<QString> attrList;
            attrList << "C" << "a" << "H" << "z" << "b";
            QHash<QString,QString> map;
            map["a"] = "3" ;
            map["b"] = "4" ;
            map["C"] = "5" ;
            map["H"] = "2" ;
            map["z"] = "1" ;
            if(!compareAttributes(0, &a, attrList, map )) {
                return false;
            }
        }
    }
    return true ;
}

bool TestSplit::unitTestFilterEventElementAccessibility()
{
    const QString LocalName("rootLocal");
    const QString Uri("xxx");
    const QString ElementName("root");
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementAccessibility/empty";
        TestExtractionExecutorHelper helper ;
        QList<QPair<QString,QString> > sourceList ;
        sourceList << QPair<QString, QString>("a", "b");
        sourceList << QPair<QString, QString>("a1", "b1");
        if(!helper.executeElement("var varOfEvent =1;", ElementName, Uri, LocalName, sourceList)) {
            return error("1");
        }
        if(!checkElementEventWithEngine(0, helper.filter._engine, helper.elementEvent,
                              sourceList, false, "1") ) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementAccessibility/tag";
        TestExtractionExecutorHelper helper ;
        QList<QPair<QString,QString> > sourceList ;
        sourceList << QPair<QString, QString>("a", "b");
        sourceList << QPair<QString, QString>("a1", "b1");
        if(!helper.executeElement("var varOfEvent =0; if(event.elementName=='root' ) {varOfEvent='root1'}  ", ElementName, Uri, LocalName, sourceList)) {
            return error("2");
        }
        if(!checkElementEventWithEngine(0, helper.filter._engine, helper.elementEvent,
                              sourceList, false, "root1") ) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementAccessibility/localName";
        TestExtractionExecutorHelper helper ;
        QList<QPair<QString,QString> > sourceList ;
        sourceList << QPair<QString, QString>("a", "b");
        sourceList << QPair<QString, QString>("a1", "b1");
        if(!helper.executeElement("var varOfEvent =0; if(event.localName=='rootLocal' ) {varOfEvent='root1'}  ", ElementName, Uri, LocalName, sourceList)) {
            return error("2");
        }
        if(!checkElementEventWithEngine(0, helper.filter._engine, helper.elementEvent,
                              sourceList, false, "root1") ) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementAccessibility/uri";
        TestExtractionExecutorHelper helper ;
        QList<QPair<QString,QString> > sourceList ;
        sourceList << QPair<QString, QString>("a", "b");
        sourceList << QPair<QString, QString>("a1", "b1");
        if(!helper.executeElement("var varOfEvent =0; if(event.nameSpace=='xxx' ) {varOfEvent='root1'}  ", ElementName, Uri, LocalName, sourceList)) {
            return error("2");
        }
        if(!checkElementEventWithEngine(0, helper.filter._engine, helper.elementEvent,
                              sourceList, false, "root1") ) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementAccessibility/count";
        TestExtractionExecutorHelper helper ;
        QList<QPair<QString,QString> > sourceList ;
        sourceList << QPair<QString, QString>("a", "b");
        sourceList << QPair<QString, QString>("a1", "b1");
        if(!helper.executeElement("var varOfEvent = event.attributesCount; ", ElementName, Uri, LocalName, sourceList)) {
            return error("3");
        }
        if(!checkElementEventWithEngine(0, helper.filter._engine, helper.elementEvent,
                              sourceList, false, "2") ) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementAccessibility/removeByName";
        TestExtractionExecutorHelper helper ;
        QList<QPair<QString,QString> > sourceList ;
        sourceList << QPair<QString, QString>("a", "b");
        sourceList << QPair<QString, QString>("a1", "b1");
        QList<QPair<QString,QString> > expectedList ;
        expectedList << QPair<QString, QString>("a", "b");
        if(!helper.executeElement("var varOfEvent = 0 ; event.removeAttributeByName('a1'); varOfEvent = event.attributesCount;", ElementName, Uri, LocalName, sourceList)) {
            return error("4");
        }
        if(!checkElementEventWithEngine(0, helper.filter._engine, helper.elementEvent,
                              expectedList, true, "1") ) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementAccessibility/removeByName";
        TestExtractionExecutorHelper helper ;
        QList<QPair<QString,QString> > sourceList ;
        sourceList << QPair<QString, QString>("a", "b");
        sourceList << QPair<QString, QString>("a1", "b1");
        QList<QPair<QString,QString> > expectedList ;
        expectedList << QPair<QString, QString>("a", "b");
        if(!helper.executeElement("var varOfEvent = 0 ; event.removeAttributeByIndex(1); varOfEvent = event.attributesCount;", ElementName, Uri, LocalName, sourceList)) {
            return error("5");
        }
        if(!checkElementEventWithEngine(0, helper.filter._engine, helper.elementEvent,
                              expectedList, true, "1") ) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementAccessibility/setAByName";
        TestExtractionExecutorHelper helper ;
        QList<QPair<QString,QString> > sourceList = stdElementAttributes() ;
        QList<QPair<QString,QString> > expectedList ;
        expectedList << QPair<QString, QString>("a", "b");
        expectedList << QPair<QString, QString>("a1", "b1");
        expectedList << QPair<QString, QString>("a2", "b22");
        expectedList << QPair<QString, QString>("a4", "b4");
        // setval name
        const QString CODE_SETBYNAME(""\
" var varOfEvent = 0 ; "\
" event.setAttributeValueByName('a2', 'b22'); "\
" event.setAttributeValueByName('a4', 'b4'); "\
"  varOfEvent = '' + event.attributesCount + '/' + event.attributeValueByName('a');");

        if(!helper.executeElement(CODE_SETBYNAME, ElementName, Uri, LocalName, sourceList)) {
            return error("5");
        }
        if(!checkElementEventWithEngine(0, helper.filter._engine, helper.elementEvent,
                              expectedList, true, "4/b") ) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementAccessibility/setAByIndex";
        TestExtractionExecutorHelper helper ;
        QList<QPair<QString,QString> > sourceList = stdElementAttributes() ;
        QList<QPair<QString,QString> > expectedList ;
        expectedList << QPair<QString, QString>("a", "b");
        expectedList << QPair<QString, QString>("a1", "b22");
        expectedList << QPair<QString, QString>("v", "b2");
        // setval name
        const QString CODE_SETBYINDEX(""\
" var varOfEvent = 0 ; "\
" event.setAttributeValueByIndex(1, 'b22'); "\
" event.setAttributeValueByIndex(3, 'b4'); "\
" event.setAttributeValueByIndex(13, 'xx4'); "\
" event.setAttributeNameByIndex(2, 'v'); "\
"  varOfEvent = '' + event.attributesCount + '/' + event.attributeValueByIndex(0)+ '/' + event.attributeValueByIndex(1);");

        if(!helper.executeElement(CODE_SETBYINDEX, ElementName, Uri, LocalName, sourceList)) {
            return error("5");
        }
        if(!checkElementEventWithEngine(0, helper.filter._engine, helper.elementEvent,
                              expectedList, true, "3/b/b22") ) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementAccessibility/removeByIndex";
        TestExtractionExecutorHelper helper ;
        QList<QPair<QString,QString> > sourceList = stdElementAttributes() ;
        QList<QPair<QString,QString> > expectedList ;
        expectedList << QPair<QString, QString>("a", "b");
        expectedList << QPair<QString, QString>("a2", "b2");
        // setval name
        const QString CodeRemoveAttribute(""\
" var varOfEvent = 0 ; "\
" event.removeAttributeByIndex(1); varOfEvent = event.attributesCount");
        if(!helper.executeElement(CodeRemoveAttribute, ElementName, Uri, LocalName, sourceList)) {
            return error("5");
        }
        if(!checkElementEventWithEngine(0, helper.filter._engine, helper.elementEvent,
                              expectedList, true, "2") ) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementAccessibility/removeByName";
        TestExtractionExecutorHelper helper ;
        QList<QPair<QString,QString> > sourceList = stdElementAttributes() ;
        QList<QPair<QString,QString> > expectedList ;
        expectedList << QPair<QString, QString>("a", "b");
        expectedList << QPair<QString, QString>("a2", "b2");
        // setval name
        const QString CodeRemoveAttribute(""\
" var varOfEvent = 0 ; "\
" event.removeAttributeByName('a1');"\
" event.removeAttributeByName('xx'); "\
"varOfEvent = event.attributesCount");
        if(!helper.executeElement(CodeRemoveAttribute, ElementName, Uri, LocalName, sourceList)) {
            return error("5");
        }
        if(!checkElementEventWithEngine(0, helper.filter._engine, helper.elementEvent,
                              expectedList, true, "2") ) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementAccessibility/sortAttribute";
        TestExtractionExecutorHelper helper ;
        QList<QPair<QString,QString> > sourceList ;
        sourceList << QPair<QString, QString>("z", "zz");
        sourceList << QPair<QString, QString>("x", "xx");
        sourceList << QPair<QString, QString>("B", "bb");
        sourceList << QPair<QString, QString>("a", "aa");
        QList<QPair<QString,QString> > expectedList ;
        expectedList << QPair<QString, QString>("a", "aa");
        expectedList << QPair<QString, QString>("B", "bb");
        expectedList << QPair<QString, QString>("x", "xx");
        expectedList << QPair<QString, QString>("z", "zz");
        // setval name
        const QString CodeSortAttribute(
" var varOfEvent = 0 ; "\
" event.sortAttributesByName();");
        if(!helper.executeElement(CodeSortAttribute, ElementName, Uri, LocalName, sourceList)) {
            return error("5");
        }
        if(!checkElementEventWithEngine(0, helper.filter._engine, helper.elementEvent,
                              expectedList, true, "0") ) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementAccessibility/sortAttributeNS";
        TestExtractionExecutorHelper helper ;
        QList<QPair<QString,QString> > sourceList ;
        sourceList << QPair<QString, QString>("z", "zz");
        sourceList << QPair<QString, QString>("a:x", "xx");
        sourceList << QPair<QString, QString>("B", "bb");
        QList<QPair<QString,QString> > expectedList ;
        expectedList << QPair<QString, QString>("a:x", "xx");
        expectedList << QPair<QString, QString>("B", "bb");
        expectedList << QPair<QString, QString>("z", "zz");
        // setval name
        const QString CodeSortAttribute(
" var varOfEvent = 0 ; "\
" event.sortAttributesByNamespaceAndName();");
        if(!helper.executeElement(CodeSortAttribute, ElementName, Uri, LocalName, sourceList)) {
            return error("5");
        }
        if(!checkElementEventWithEngine(0, helper.filter._engine, helper.elementEvent,
                              expectedList, true, "0") ) {
            return false;
        }
    }
    return true ;
}

bool TestSplit::checkAttributeStream(QPair<QString,QString> attribute, const int index, const QString &expectedName, const QString &expectedValue)
{
    if(attribute.first != expectedName) {
        return error(QString("attribute name at index:%1 expected: %2, found %3")
                .arg(index).arg(expectedName).arg(attribute.first));
    }
    if(attribute.second != expectedValue) {
        return error(QString("attribute value at index:%1 expected: %2, found %3")
                .arg(index).arg(expectedValue).arg(attribute.second));
    }
    return true ;
}

void TestSplit::addAttributesToList(QList<QPair<QString, QString> > &translatesAttributes, QXmlStreamAttributes inputAttributes)
{
    const int numAttributes = inputAttributes.size();
    FORINT(index, numAttributes) {
        QXmlStreamAttribute streamAttribute = inputAttributes.at(index);
        QPair<QString, QString> attributePair(streamAttribute.qualifiedName().toString(), streamAttribute.value().toString());
        translatesAttributes.append(attributePair);
    }
}
bool TestSplit::unitTestOperationBase()
{
    _testName = "unitTestOperationBase";
    QXmlStreamAttributes inputAttributes;
    QXmlStreamAttribute attribute1("abc", "123");
    QXmlStreamAttribute attribute2("cde", "456");
    inputAttributes.append(attribute1);
    inputAttributes.append(attribute2);
    QList<QPair<QString,QString> > translatesAttributes;
    addAttributesToList(translatesAttributes, inputAttributes);
    if(translatesAttributes.size()!= 2) {
        return error(QString("Size: expected: %1, found %2").arg(2).arg(translatesAttributes.size()));
    }
    if(!checkAttributeStream(translatesAttributes.at(0), 0, "abc", "123") ) {
        return false;
    }
    if(!checkAttributeStream(translatesAttributes.at(1), 1, "cde", "456") ) {
        return false;
    }
    return true ;
}

bool TestSplit::unitTestFilterEventElementMultipleEvents()
{
    // mod, mod: 0
    // ignore, ignore: 1
    // mod, ignore: 2
    // ignore, mod: 3
    const QString LocalName("rootLocal");
    const QString Uri("xxx");
    const QString ElementName("root");
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementMultipleEvents/go-go";
        TestSplitScriptingOperationHelper helper;
        QList<QPair<QString,QString> > sourceList = stdElementAttributes();
        QList<QPair<QString,QString> > expectedList ;
        expectedList << QPair<QString, QString>("a", "b");
        expectedList << QPair<QString, QString>("a1", "xx");
        expectedList << QPair<QString, QString>("a2", "b2");

        helper.executeMultipleElement("event.setAttributeValueByName('a1', 'yy');",  "event.setAttributeValueByName('a1', 'xx');",
                                          ElementName, Uri, LocalName, sourceList);
        if(!checkElementEventWithReturnCode(0, helper, &helper.elementEvent, expectedList, true, ExtractionScriptManager::EventResult_WriteModifiedData, 2) ) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementMultipleEvents/ignore-ignore";
        TestSplitScriptingOperationHelper helper;
        QList<QPair<QString,QString> > sourceList = stdElementAttributes();
        QList<QPair<QString,QString> > expectedList ;
        expectedList << QPair<QString, QString>("a", "b");
        expectedList << QPair<QString, QString>("a1", "b1");
        expectedList << QPair<QString, QString>("a2", "b2");

        helper.executeMultipleElement(";",  ";",
                                          ElementName, Uri, LocalName, sourceList);
        if(!checkElementEventWithReturnCode(1, helper, &helper.elementEvent, expectedList, false, ExtractionScriptManager::EventResult_WriteOriginalData, 2) ) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementMultipleEvents/go-ignore";
        TestSplitScriptingOperationHelper helper;
        QList<QPair<QString,QString> > sourceList = stdElementAttributes();
        QList<QPair<QString,QString> > expectedList ;
        expectedList << QPair<QString, QString>("a", "b");
        expectedList << QPair<QString, QString>("a1", "yy");
        expectedList << QPair<QString, QString>("a2", "b2");

        helper.executeMultipleElement("event.setAttributeValueByName('a1', 'yy');",  ";",
                                          ElementName, Uri, LocalName, sourceList);
        if(!checkElementEventWithReturnCode(2, helper, &helper.elementEvent, expectedList, true, ExtractionScriptManager::EventResult_WriteModifiedData, 2) ) {
            return false;
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterEventElementMultipleEvents/ignore-go";
        TestSplitScriptingOperationHelper helper;
        QList<QPair<QString,QString> > sourceList = stdElementAttributes();
        QList<QPair<QString,QString> > expectedList ;
        expectedList << QPair<QString, QString>("a", "b");
        expectedList << QPair<QString, QString>("a1", "yy");
        expectedList << QPair<QString, QString>("a2", "b2");

        helper.executeMultipleElement(";", "event.setAttributeValueByName('a1', 'yy');",
                                          ElementName, Uri, LocalName, sourceList);
        if(!checkElementEventWithReturnCode(3, helper, &helper.elementEvent, expectedList, true, ExtractionScriptManager::EventResult_WriteModifiedData, 2) ) {
            return false;
        }
    }
    return true ;
}

#endif
