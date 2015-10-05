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

#include "testelement.h"
#include "app.h"
#include "config.h"

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
    return testSingleTooltip("7", 7, false);
}


bool TestElement::testUnit()
{
    _testName = "testUnit";
    if(!testTooltip() ) {
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
    _testName = "testTooltip" ;
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
    return true;
}

static bool probeForImageData(const QString &data)
{
    if(!data.startsWith("<html>")) {
        return false;
    }
    if(!data.indexOf("<img src=\"data:image")<0) {
        return false;
    }
    return true;
}

bool TestElement::testSingleTooltip(const QString &testCase, const int selElement, const bool expected )
{
    _testName = QString("testTooltip %1").arg(testCase);
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
