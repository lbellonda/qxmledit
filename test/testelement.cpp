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

#include "testelement.h"
#include "app.h"
#include "qxmleditconfig.h"

#define BASE_PATH "../test/data/element/"
#define TOOLTIP  BASE_PATH "tooltip.xml"


TestElement::TestElement()
{
}

TestElement::~TestElement()
{
}

bool TestElement::testFast()
{
    return testConfigTooltip();
}


bool TestElement::testUnit()
{
    _testName = "testUnit";
    if(!testTooltip() ) {
        return false;
    }
    if(!testHasText()) {
        return false;
    }
    if(!testParentPath()) {
        return false;
    }
    return true;
}

/*!
 * \brief TestElement::testTooltip
 * \return
 *
 * tests to come:
 * Elements
 * Attribute xlink Empty
 * Attribute xlink Filled
 * Attribute xlink Missing
 * Attribute xlink with data
 * ---------
 *  Text with data
 *  Text no data
 *  No Text
 * --------
 *  Text
 *   Text with data
 *   Text no data
 *
 *
 * Repeat with disabled settings
 *
 * #  | Attr  |  Inner Tx | Result
 * 0     E          D        Y
 * 1     F          D        Y
 * 2     M          D        Y
 * 3     E          N        Y
 * 4     F          N        Y
 * 5     M          N        Y
 * 6     E          M        N
 * 7     F          M        N
 * 8     M          M        N
 * 9     D          M        Y
 *
 * 10   Text with data       Y
 * 11   Text no data         Y
 */
bool TestElement::testTooltip()
{
    if(!testSingleTooltip("0", 0, true)) {
        return false;
    }
    if(!testSingleTooltip("1", 1, true)) {
        return false;
    }
    if(!testSingleTooltip("2", 2, true)) {
        return false;
    }
    if(!testSingleTooltip("3", 3, true)) {
        return false;
    }
    if(!testSingleTooltip("4", 4, true)) {
        return false;
    }
    if(!testSingleTooltip("5", 5, true)) {
        return false;
    }
    if(!testSingleTooltip("6", 6, false)) {
        return false;
    }
    if(!testSingleTooltip("7", 7, false)) {
        return false;
    }
    if(!testSingleTooltip("8", 8, false)) {
        return false;
    }
    if(!testSingleTooltip("9", 9, true)) {
        return false;
    }
    //----
    if(!testSingleTooltip("10", 10, true)) {
        return false;
    }
    if(!testSingleTooltip("11", 11, true)) {
        return false;
    }
    if(!testConfigTooltip()) {
        return false;
    }
    return true;
}

bool TestElement::testConfigTooltip()
{
    if(!testConfigTooltipInner(false, false, true)) {
        return false;
    }

    if(!testConfigTooltipInner(true, false, false)) {
        return false;
    }
    if(!testConfigTooltipInner(true, true, true)) {
        return false;
    }
    return true;
}

bool TestElement::testConfigTooltipInner(const bool setValue, const bool usePath, const bool expectedFoundPath)
{
    App app;
    app.initNoWindow();
    // setup data
    Element element("A", "", NULL, NULL);
    Element *elementB = new Element("B", "", NULL, &element);
    element.addChild(elementB);
    // setup config
    if(setValue) {
        Config::saveBool(Config::KEY_ELEMENT_TEXT_TOOLTIP_PATH, usePath);
    }
    QVariant toolTip = elementB->columnViewTooltipData(NULL);
    const bool foundPath = toolTip.toString().contains("/A/B", Qt::CaseSensitive);
    if(foundPath!=expectedFoundPath) {
        return error(QString("Error tooltip inner for setValue:%1 usePath:%2 expectedFoundPath:%3")
                     .arg(setValue).arg(usePath).arg(expectedFoundPath));
    }
    return true ;
}


static bool probeForImageData(const QString &data)
{
    if(!data.startsWith("<html>")) {
        return false;
    }
    if(data.indexOf("<img src=\"data:image")<0) {
        return false;
    }
    return true;
}

bool TestElement::testSingleTooltip(const QString &testCase, const int selElement, const bool expected )
{
    _subTestName = QString("testTooltip %1").arg(testCase);
    App app;
    if(!app.init()) {
        return error("init app ");
    }
    if( !app.mainWindow()->loadFile(TOOLTIP) ) {
        return error(QString("unable to load input file: '%1' ").arg(TOOLTIP));
    }
    Regola *regola = app.mainWindow()->getRegola();
    QList<int> selection;
    selection << 0 << selElement ;
    Element *selectedElement = regola->findElementByArray(selection);
    if( NULL == selectedElement ) {
        return error("Null element");
    }
    Config::saveBool(Config::KEY_ELEMENT_TEXT_TOOLTIP_IMAGE, true);
    QString toolTip = selectedElement->columnViewTooltipData(NULL).toString();
    bool result = probeForImageData(toolTip);
    if( result != expected ) {
        return error(QString("Expected %1, found %2").arg(expected).arg(result));
    }

    Config::saveBool(Config::KEY_ELEMENT_TEXT_TOOLTIP_IMAGE, false);
    toolTip = selectedElement->columnViewTooltipData(NULL).toString();
    result = probeForImageData(toolTip);
    if( result ) {
        return error(QString("With disabled, found %1").arg(result));
    }
    return true ;
}


bool TestElement::testHasText()
{
    _subTestName = "testHasText";
    if(!testNotHasTextSingle()) {
        return false;
    }
    if(!testNotHasTextComplex()) {
        return false;
    }
    if(!testHasTextSingle()) {
        return false;
    }
    if(!testHasTextComplex()) {
        return false;
    }
    return true;
}

bool TestElement::testNotHasTextSingle()
{
    _subTestName = "testNotHasTextSingle";
    Element element("A", "", NULL, NULL);
    if(element.hasText()) {
        return error("Expected no");
    }
    return true;
}

bool TestElement::testNotHasTextComplex()
{
    _subTestName = "testNotHasTextComplex";
    Element element("A", "", NULL, NULL);
    Element *elementB = new Element("B", "", NULL, &element);
    Element *elementC = new Element("C", "", NULL, &element);
    element.addChild(elementB);
    element.addChild(elementC);
    if(element.hasText()) {
        return error("Expected no");
    }
    return true;
}

bool TestElement::testHasTextSingle()
{
    _subTestName = "testHasTextSingle";
    Element element("A", "xx", NULL, NULL);
    if(!element.hasText()) {
        return error("Expected yes");
    }
    return true;
}

bool TestElement::testHasTextComplex()
{
    _subTestName = "testHasTextComplex";
    {
        Element element("A", "", NULL, NULL);
        Element *elementB = new Element("B", "", NULL, &element);
        Element *elementC = new Element("C", "", NULL, &element);
        element.addChild(elementB);
        element.addChild(elementC);
        element.addTextNode(new TextChunk(false, "test"));
        if(!element.hasText()) {
            return error("Expected yes 1");
        }
    }
    {
        Element element("A", "", NULL, NULL);
        element.addTextNode(new TextChunk(false, "test"));
        if(!element.hasText()) {
            return error("Expected yes 2");
        }
    }
    {
        Element element("A", "", NULL, NULL);
        Element *elementB = new Element("B", "", NULL, &element);
        Element *elementC = new Element("C", "", NULL, &element);
        element.addChild(elementB);
        element.addChild(elementC);
        element.addTextNode(new TextChunk(true, "test"));
        if(!element.hasText()) {
            return error("Expected yes 3");
        }
    }
    {
        Element element("A", "", NULL, NULL);
        element.addTextNode(new TextChunk(true, "test"));
        if(!element.hasText()) {
            return error("Expected yes 4");
        }
    }
    {
        Element element("A", "", NULL, NULL);
        Element *elementB = new Element("B", "", NULL, &element);
        Element *elementC = new Element("C", "", NULL, &element);
        element.addChild(elementB);
        element.addChild(elementC);
        Element *elementT = new Element(NULL, Element::ET_TEXT, &element) ;
        elementT->setTextOfTextNode("text", false);
        element.addChild(elementT);
        if(!element.hasText()) {
            return error("Expected yes 5");
        }
    }
    {
        Element element("A", "", NULL, NULL);
        Element *elementT = new Element(NULL, Element::ET_TEXT, &element) ;
        elementT->setTextOfTextNode("text", false);
        element.addChild(elementT);
        if(!element.hasText()) {
            return error("Expected yes 6");
        }
    }
    return true;
}

bool TestElement::testParentPath()
{
    _subTestName = "testParentPath";
    {
        const QString expected = "/abc";
        const QString value = Utils::pathFromParent(NULL, "abc");
        if(value != expected) {
            return error(QString("1 expected: %1, found: %2").arg(expected).arg(value));
        }
    }
    {
        const QString expected = "/";
        const QString value = Utils::pathFromParent(NULL, "");
        if(value != expected) {
            return error(QString("1 expected: %1, found: %2").arg(expected).arg(value));
        }
    }
    {
        const QString expected = "/a/b";
        Element element("a", "", NULL, NULL);
        const QString value = Utils::pathFromParent(&element, "b");
        if(value != expected) {
            return error(QString("1 expected: %1, found: %2").arg(expected).arg(value));
        }
    }
    return true ;
}
