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

#include "teststyle.h"
#include "style.h"
#include "stylepersistence.h"
#include "app.h"

//----------------------------------------
#define TEST_FILE_STYLE "../test/data/style/test_load_style.style"
#define TEST_XML_BASE_STYLE "../test/data/style/test.xml"
#define TEST_XML_ALT_STYLE "../test/data/style/test.alt.xml"
#define TEST_FILE_STYLE_1 "../test/data/style/style1.style"
#define TEST_FILE_STYLE_2 "../test/data/style/style2.style"
#define FILE_STYLE_BASE "../test/data/style"
//----------------------------------------

#define STYLE_OR    "styleOr"
#define FILE_STYLE_OR   "../test/data/style/style.or.style"
#define STYLE_DEFAULT   "default"
#define STYLE_AND   "styleAnd"
#define FILE_STYLE_AND  "../test/data/style/style.and.style"

//------- axis
#define STYLE_AXIS_PARENT   "axisParent"
#define FILE_STYLE_AXIS_PARENT  "../test/data/style/style.axisparent.style"
#define STYLE_AXIS_GRANDPARENT  "axisGrandParent"
#define FILE_STYLE_AXIS_GRANDPARENT "../test/data/style/style.axisgrandparent.style"
#define STYLE_AXIS_GRANDPARENTNAME  "axisGrandParentName"
#define FILE_STYLE_AXIS_GRANDPARENTNAME "../test/data/style/style.axisgrandparentname.style"

#define STYLE_POSITION_FIRST    "posFirst"
#define FILE_STYLE_POSITION_FIRST   "../test/data/style/pos.first.style"

#define STYLE_POSITION_LAST "posLast"
#define FILE_STYLE_POSITION_LAST "../test/data/style/pos.last.style"

#define STYLE_POSITION  "pos"
#define FILE_STYLE_POSITION "../test/data/style/pos.style"

//--------

TestStyle::TestStyle()
{
}

bool TestStyle::checkFirstIndex(QList<StyleRuleSet*> &rules, int firstIndex)
{
    if( rules.size()<= firstIndex ) {
        return error(QString("Expecting %1 high level items, found %2").arg(firstIndex).arg(rules.size()));
    }
    return true ;
}

bool TestStyle::checkSecondIndex(QList<StyleCalc*> rules, int index)
{
    if( rules.size()<= index ) {
        return error(QString("Expecting %1 second level items, found %2").arg(index).arg(rules.size()));
    }
    return true ;
}

StyleRule* TestStyle::getRuleItem(QList<StyleRuleSet*> &rules, int firstIndex, int secondIndex )
{
    if(!checkFirstIndex(rules, firstIndex) ) {
        return NULL;
    }
    StyleRuleSet *rs = rules.at(firstIndex);
    if(!checkSecondIndex(rs->children(), secondIndex) ) {
        return NULL;
    }
    StyleCalc *calc = rs->children().at(secondIndex);
    if( NULL == calc ) {
        error(QString("Null item at indexes:%1,%2").arg(firstIndex).arg(secondIndex));
        return NULL ;
    }

    if( calc->tp() != "r" ) {
        error(QString("Not a rule at indexes:%1,%2").arg(firstIndex).arg(secondIndex));
        return NULL ;
    }
    StyleRule *rule = (StyleRule *)rs->children().at(secondIndex);
    return rule;
}

StyleRuleSet* TestStyle::getRuleSetItem(QList<StyleRuleSet*> &rules, int firstIndex, int secondIndex )
{
    if(!checkFirstIndex(rules, firstIndex) ) {
        return NULL;
    }
    StyleRuleSet *rs = rules.at(firstIndex);
    if(!checkSecondIndex(rs->children(), secondIndex) ) {
        return NULL;
    }
    StyleCalc *calc = rs->children().at(secondIndex);
    if( NULL == calc ) {
        error(QString("Null item at indexes:%1,%2").arg(firstIndex).arg(secondIndex));
        return NULL ;
    }

    if( calc->tp() != "s" ) {
        error(QString("Not a rule set at indexes:%1,%2").arg(firstIndex).arg(secondIndex));
        return NULL ;
    }
    StyleRuleSet *ruleSet = (StyleRuleSet *)rs->children().at(secondIndex);
    return ruleSet;
}

StyleRuleSet* TestStyle::getRuleSetItem(QList<StyleRuleSet*> &rules, int firstIndex)
{
    if(!checkFirstIndex(rules, firstIndex) ) {
        return NULL;
    }
    StyleRuleSet *ruleSet = rules.at(firstIndex);
    if( NULL == ruleSet ) {
        error(QString("Null item at index:%1").arg(firstIndex));
        return NULL ;
    }
    return ruleSet;
}

//----------------------------------------

bool TestStyle::checkName(QList<StyleRuleSet*> &rules, int firstIndex, int secondIndex, const QString testValue )
{
    _testName = "testLoadCalcStyle/checkName";
    StyleRule* rule = getRuleItem(rules, firstIndex, secondIndex );
    if( NULL == rule ) {
        return false ;
    }
    if( rule->name() != testValue ) {
        return error(QString("Name expected '%1' found '%2' at indexes:%3,%4").arg(testValue).arg(rule->name()).arg(firstIndex).arg(secondIndex));
    }
    return true;
}

bool TestStyle::checkEntity(QList<StyleRuleSet*> &rules, int firstIndex, int secondIndex, const bool testValue )
{
    _testName = "testLoadCalcStyle/checkEntity";
    StyleRule* rule = getRuleItem(rules, firstIndex, secondIndex );
    if( NULL == rule ) {
        return false ;
    }
    if( rule->isEntityElement() != testValue ) {
        return error(QString("Entity expected '%1' found '%2' at indexes:%3,%4").arg(testValue).arg(rule->isEntityElement()).arg(firstIndex).arg(secondIndex));
    }
    return true;
}

bool TestStyle::checkType(QList<StyleRuleSet*> &rules, int firstIndex, int secondIndex, const StyleRule::ECfrType testValue )
{
    _testName = "testLoadCalcStyle/checkType";
    StyleRule* rule = getRuleItem(rules, firstIndex, secondIndex );
    if( NULL == rule ) {
        return false ;
    }
    if( rule->type() != testValue ) {
        return error(QString("Type expected '%1' found '%2' at indexes:%3,%4").arg(testValue).arg(rule->type()).arg(firstIndex).arg(secondIndex));
    }
    return true;
}


bool TestStyle::checkAxis(QList<StyleRuleSet*> &rules, int firstIndex, int secondIndex, const QString &testValue )
{
    _testName = "testLoadCalcStyle/axis";
    StyleRule* rule = getRuleItem(rules, firstIndex, secondIndex );
    if( NULL == rule ) {
        return false ;
    }
    if( rule->axis() != testValue ) {
        return error(QString("Axis expected '%1' found '%2' at indexes:%3,%4").arg(testValue).arg(rule->axis()).arg(firstIndex).arg(secondIndex));
    }
    return true;
}

bool TestStyle::checkValue(QList<StyleRuleSet*> &rules, int firstIndex, int secondIndex, const QString testValue )
{
    _testName = "testLoadCalcStyle/checkName";
    StyleRule* rule = getRuleItem(rules, firstIndex, secondIndex );
    if( NULL == rule ) {
        return false ;
    }
    if( rule->value() != testValue ) {
        return error(QString("value expected '%1' found '%2' at indexes:%3,%4").arg(testValue).arg(rule->value()).arg(firstIndex).arg(secondIndex));
    }
    return true;
}

bool TestStyle::checkCase(QList<StyleRuleSet*> &rules, int firstIndex, int secondIndex, const bool testValue )
{
    _testName = "testLoadCalcStyle/checkCase";
    StyleRule* rule = getRuleItem(rules, firstIndex, secondIndex );
    if( NULL == rule ) {
        return false ;
    }
    if( rule->isCaseSensitive() != testValue ) {
        return error(QString("value expected '%1' found '%2' at indexes:%3,%4").arg(testValue).arg(rule->isCaseSensitive()).arg(firstIndex).arg(secondIndex));
    }
    return true;
}

bool TestStyle::checkOp(QList<StyleRuleSet*> &rules, int firstIndex, int secondIndex, const StyleRule::oper testValue )
{
    _testName = "testLoadCalcStyle/checkOp";
    StyleRule* rule = getRuleItem(rules, firstIndex, secondIndex );
    if( NULL == rule ) {
        return false ;
    }
    if( rule->op() != testValue ) {
        return error(QString("value expected '%1' found '%2' at indexes:%3,%4").arg(testValue).arg(rule->op()).arg(firstIndex).arg(secondIndex));
    }
    return true;
}


bool TestStyle::checkConnector(QList<StyleRuleSet*> &rules, int firstIndex, int secondIndex, const bool andConnector )
{
    _testName = "testLoadCalcStyle/checkConnector";
    StyleRuleSet* rule = getRuleSetItem(rules, firstIndex, secondIndex );
    if( NULL == rule ) {
        return false ;
    }
    if( rule->isConnectorAnd() != andConnector ) {
        return error(QString("value expected '%1' found '%2' at indexes:%3,%4").arg(andConnector).arg(rule->isConnectorAnd()).arg(firstIndex).arg(secondIndex));
    }
    return true;
}

bool TestStyle::checkRSStyleId(QList<StyleRuleSet*> &rules, int firstIndex, const QString &idExpected )
{
    _testName = "testLoadCalcStyle/checkRSStyleId";
    StyleRuleSet* rule = getRuleSetItem(rules, firstIndex );
    if( NULL == rule ) {
        return false ;
    }
    if( rule->idStyle() != idExpected ) {
        return error(QString("value expected '%1' found '%2' at indexes:%3").arg(idExpected).arg(rule->idStyle()).arg(firstIndex));
    }
    return true;
}

bool TestStyle::checkChildrenNum(QList<StyleRuleSet*> &rules, int firstIndex, const int numChildren )
{
    _testName = "testLoadCalcStyle/checkChildrenNum";
    StyleRuleSet* rule = getRuleSetItem(rules, firstIndex);
    if( NULL == rule ) {
        return false ;
    }
    if( rule->children().size() != numChildren ) {
        return error(QString("value expected '%1' found '%2' at index:%3").arg(numChildren).arg(rule->children().size()).arg(firstIndex));
    }
    return true;
}

//----------------------------------------


bool TestStyle::testLoadCalcStyle()
{
    _testName = "testLoadCalcStyle";
    StylePersistence persistence;
    VStyle vStyle("test", "test");
    if( !persistence.readStyleFile(&vStyle, TEST_FILE_STYLE) ) {
        return error(QString("error loading style file %1").arg(TEST_FILE_STYLE));
    }
    // check the styles
    QList<StyleRuleSet*> rules = vStyle.ruleSets();

    if(!checkEntity(rules, 0, 0, true ) ) {
        return false;
    }
    if(!checkEntity(rules, 0, 1, false ) ) {
        return false;
    }
    if(!checkEntity(rules, 0, 2, false ) ) {
        return false;
    }
    if(checkEntity(rules, 0, 2, true ) ) {
        return error(QString("Entity test check found true at indexes:%1,%2").arg(0).arg(2));
    }
    //---
    if(!checkName(rules, 1, 0, "level" ) ) {
        return false;
    }
    if(!checkName(rules, 1, 1, "" ) ) {
        return false;
    }
    if(checkName(rules, 1, 1, "true" ) ) {
        return error(QString("name test check found 'true'' at indexes:%1,%2").arg(1).arg(1));
    }
    //---
    if(!checkType(rules, 2, 0, StyleRule::CT_STRING ) ) {
        return false;
    }
    if(!checkType(rules, 2, 1, StyleRule::CT_NUM ) ) {
        return false;
    }
    if(!checkType(rules, 2, 2, StyleRule::CT_POSITION ) ) {
        return false;
    }
    if(!checkType(rules, 2, 3, StyleRule::CT_STRING ) ) {
        return false;
    }
    if(checkType(rules, 2, 3, StyleRule::CT_NUM ) ) {
        return error(QString("type test check found 'true'' at indexes:%1,%2").arg(2).arg(3));
    }
    //---
    if(!checkValue(rules, 3, 0, "12" ) ) {
        return false;
    }
    if(!checkValue(rules, 3, 1, "abc" ) ) {
        return false;
    }
    if(!checkValue(rules, 3, 2, "" ) ) {
        return false;
    }
    if(!checkValue(rules, 3, 3, "ABCd" ) ) {
        return false;
    }
    if(checkValue(rules, 3, 2, "true" ) ) {
        return error(QString("name test check found 'true'' at indexes:%1,%2").arg(3).arg(2));
    }
    //---
    if(!checkCase(rules, 4, 0, false ) ) {
        return false;
    }
    if(!checkCase(rules, 4, 1, true ) ) {
        return false;
    }
    if(!checkCase(rules, 4, 2, true ) ) {
        return false;
    }
    //---
    if(!checkConnector(rules, 6, 0, true ) ) {
        return false;
    }
    if(!checkConnector(rules, 6, 1, false ) ) {
        return false;
    }
    if(!checkConnector(rules, 6, 2, true ) ) {
        return false;
    }
    //---
    if(!checkChildrenNum(rules, 7, 2 ) ) {
        return false;
    }
    if(!checkChildrenNum(rules, 8, 4 ) ) {
        return false;
    }
    if(checkChildrenNum(rules, 8, 10 ) ) {
        return error(QString("checkChildrenNum found '10'' at index:%1").arg(8));
    }
    //---
    if(!checkOp(rules, 5, 0, StyleRule::EQ ) ) {
        return false;
    }
    if(!checkOp(rules, 5, 1, StyleRule::EQ ) ) {
        return false;
    }
    if(!checkOp(rules, 5, 2, StyleRule::NE) ) {
        return false;
    }
    if(!checkOp(rules, 5, 3, StyleRule::EX) ) {
        return false;
    }
    if(!checkOp(rules, 5, 4, StyleRule::NX) ) {
        return false;
    }
    if(!checkOp(rules, 5, 5, StyleRule::GT) ) {
        return false;
    }
    if(!checkOp(rules, 5, 6, StyleRule::LT) ) {
        return false;
    }
    if(!checkOp(rules, 5, 7, StyleRule::GE) ) {
        return false;
    }
    if(!checkOp(rules, 5, 8, StyleRule::LE) ) {
        return false;
    }
    if(!checkOp(rules, 5, 9, StyleRule::SS) ) {
        return false;
    }
    if(!checkOp(rules, 5, 10, StyleRule::ST) ) {
        return false;
    }
    if(checkOp(rules, 5, 10, StyleRule::SS) ) {
        return error(QString("checkOp returns true when expecting false index:5,10"));
    }

    //-----
    if(!checkAxis(rules, 9, 0, ".." ) ) {
        return false;
    }
    if(!checkAxis(rules, 9, 1, "../../minMAX" ) ) {
        return false;
    }
    if(!checkAxis(rules, 9, 2, "parent" ) ) {
        return false;
    }
    if(!checkAxis(rules, 9, 3, "" ) ) {
        return false;
    }
    if(!checkAxis(rules, 9, 4, "" ) ) {
        return false;
    }
    if(checkAxis(rules, 9, 2, ".." ) ) {
        return error(QString("checkAxis returns true when expecting false index:8,2"));
    }
    //-------------------------

    //---

    return true;
}


//-----

bool TestStyle::testFunctional(int id, const QString &testName, const QString &styleName, const QString &elementName, const QString &attrName, const QString &attrValue, const QString &entity, const QString &op, const QString &type, const QString &caseStr )
{
    _testName = QString("testCalcStyle/%1").arg(testName);

    StylePersistence persistence;
    VStyle vStyle("test", "test");
    QString styleFileName = QString("%1/style_%5_%2_%3_%4.style").arg(FILE_STYLE_BASE).arg(op).arg(type).arg(caseStr).arg(entity);
    if( !persistence.readStyleFile(&vStyle, styleFileName) ) {
        return error(QString("opening style file: '%1'").arg(styleFileName));
    }
    Element *element = new Element( elementName, "", NULL );
    if( NULL == element ) {
        return error(QString("error creating element '%1'").arg(elementName));
    }
    if(!element->addAttribute(attrName, attrValue) ) {
        return error(QString("error setting the attribute '%1'").arg(attrName));
    }
    StyleEntry *entry = element->chooseStyle(&vStyle);
    delete element;
    if( NULL == entry ) {
        return error(QString("NULL style selected, expecting '%1'").arg(styleName));
    } else {
        if( entry->id() != styleName ) {
            return error(QString("Wrong style selected, id=%3 expecting '%1', found '%2'").arg(styleName).arg(entry->id()).arg(id));
        }
    }
    return true ;
}
/*
bool TestStyle::testFunctional(const QString &testName, const QString &styleName, const QString &fileStyle, const int numElement )
{
    _testName = QString("testCalcStyle/%1").arg(testName);
    App app;

    if(!app.initNoWindow() ) {
        return error("init window");
    }
    MainWindow mainWindow(false, qApp, app.data());
    if( !mainWindow.loadFile(QString(TEST_XML_BASE_STYLE)) ) {
        return error(QString("opening test file: '%1'").arg(TEST_XML_BASE_STYLE));
    }

    StylePersistence persistence;
    VStyle vStyle("test", "test");
    if( !persistence.readStyleFile(&vStyle, fileStyle) ) {
        return error(QString("opening style file: '%1'").arg(fileStyle));
    }
    QList<int> selection;
    selection << 0; //0
    selection << 0; //1
    selection << 0; // 2
    selection << numElement;
    Element *element = mainWindow.getRegola()->findElementByArray(selection);
    if( NULL == element ) {
        return error(QString("error selecting element %1").arg(numElement));
    }
    StyleEntry *entry = element->chooseStyle(&vStyle);
    if( NULL == entry ) {
        return error(QString("NULL style selected, expecting '%1'").arg(styleName));
    } else {
        if( entry->id() != styleName ) {
            return error(QString("Wrong style selected, expecting '%1', found '%2'").arg(styleName).arg(entry->id()));
        }
    }
    return true ;
}
*/
bool TestStyle::testFunctionalForRuleInner(const QString &testName, const QString &dataFileName, const QString &styleName, const QString &fileStyle, const int numElement )
{
    _testName = QString("testCalcStyle/%1").arg(testName);
    App app;

    if(!app.initNoWindow() ) {
        return error("init window");
    }
    MainWindow mainWindow(false, qApp, app.data());
    if( !mainWindow.loadFile(QString(dataFileName)) ) {
        return error(QString("opening test file: '%1'").arg(dataFileName));
    }

    StylePersistence persistence;
    VStyle vStyle("test", "test");
    if( !persistence.readStyleFile(&vStyle, fileStyle) ) {
        return error(QString("opening style file: '%1'").arg(fileStyle));
    }
    QList<int> selection;
    selection << 0; //0
    selection << 0; //1
    selection << 0; // 2
    selection << 0; // 3
    selection << numElement;
    Element *element = mainWindow.getRegola()->findElementByArray(selection);
    if( NULL == element ) {
        return error(QString("error selecting element %1").arg(numElement));
    }
    StyleEntry *entry = element->chooseStyle(&vStyle);
    if( NULL == entry ) {
        return error(QString("NULL style selected, expecting '%1'").arg(styleName));
    } else {
        if( entry->id() != styleName ) {
            return error(QString("Wrong style selected, expecting '%1', found '%2' for index: %3").arg(styleName).arg(entry->id()).arg(numElement));
        }
    }
    return true ;
}

bool TestStyle::testFunctionalForRule(const QString &testName, const QString &styleName, const QString &fileStyle, const int numElement )
{
    return testFunctionalForRuleInner(testName, TEST_XML_BASE_STYLE, styleName, fileStyle, numElement );
}

bool TestStyle::testFunctionalForRuleAlt(const QString &testName, const QString &styleName, const QString &fileStyle, const int numElement )
{
    return testFunctionalForRuleInner(testName, TEST_XML_ALT_STYLE, styleName, fileStyle, numElement );
}

struct FTTestData {
    QString testName;
    QString styleName;
    QString elementName;
    QString attrName;
    QString attrValue;
    QString op;
    QString type;
    QString caseStr;
};



bool TestStyle::testCalcStyle()
{
    _testName = "testCalcStyle";

    // string only
    FTTestData dataE [] = {
        // element
        //eq case
        {"equalElement", "1",       "test",  "aName", "abc", "eq", "s", "t" },
        {"equalElement", "default", "test1", "aName", "abc", "eq", "s", "t" },
        {"equalElement", "default", "Test",  "aName", "abc", "eq", "s", "t" },
/*

        EQ,
        NE,
        PRES,
        ABS,
        GT,
        LT,
        GTE,
        LTE
*/
        {"*", "", "", "", "", "", "", "" }
    };
    FTTestData dataA [] = {
        // element
        //eq case
        {"equalAttribute", "1",       "test",  "aName", "ccc", "eq", "s", "t" },//0
        {"equalAttribute", "default", "test",  "aName", "Ccc", "eq", "s", "t" },//1
        {"equalAttribute", "default", "Test",  "aName", "asc", "eq", "s", "t" },//2

        //eq no case
        {"equalAttribute", "1",       "test",  "aName", "ccc", "eq", "s", "f" },//3
        {"equalAttribute", "1",       "test",  "aName", "Ccc", "eq", "s", "f" },//4
        {"equalAttribute", "default", "test",  "aName", "dcc", "eq", "s", "f" },//5
        {"equalAttribute", "default", "test",  "aName", "ccc1", "eq", "s", "f" },//6

        //ne case
        {"notEqualAttribute", "1",       "test",  "aName", "Ccc", "ne", "s", "t" },//7
        {"notEqualAttribute", "default", "test",  "aName", "ccc", "ne", "s", "t" },//8
        {"notEqualAttribute", "1",       "Test",  "aName", "asc", "ne", "s", "t" },//9

        //eq - n
        {"equalAttributeN", "1",       "test",  "aName", "1", "eq", "n", "n" },//10
        {"equalAttributeN", "default", "test",  "aName", "2", "eq", "n", "n" },//11

        //ne - n
        {"notEqualAttributeN", "1",       "test",  "aName", "2", "ne", "n", "n" },//12
        {"notEqualAttributeN", "default", "test",  "aName", "1", "ne", "n", "n" },//13

        //gt case
        {"gtAttribute", "1",       "test",  "aName", "dcc", "gt", "s", "t" },//14
        {"gtAttribute", "default", "test",  "aName", "ccc", "gt", "s", "t" },//15
        {"gtAttribute", "default", "test",  "aName", "acc", "gt", "s", "t" },//16
        {"gtAttribute", "default", "test",  "aName", "Ccc", "gt", "s", "t" },//17

        //lt case
        {"ltAttribute", "1",       "test",  "aName", "acc", "lt", "s", "t" },//18
        {"ltAttribute", "default", "test",  "aName", "ccc", "lt", "s", "t" },//19
        {"ltAttribute", "default", "test",  "aName", "dcc", "lt", "s", "t" },//20
        {"ltAttribute", "1",       "test",  "aName", "Ccc", "lt", "s", "t" },//21

        //pres
        {"pAttribute", "1",       "test",  "aName", "acc", "p", "s", "" },//22
        {"pAttribute", "default", "test",  "aNam1", "ccc", "p", "s", "" },//23

        //abs
        {"pAttribute", "1",       "test",  "aNam1", "acc", "a", "s", "" },//24
        {"pAttribute", "default", "test",  "aName", "ccc", "a", "s", "" },//25


        //ge case
        {"geAttribute", "1",       "test",  "aName", "dcc", "ge", "s", "t" },//26
        {"geAttribute", "1",       "test",  "aName", "ccc", "ge", "s", "t" },//27
        {"geAttribute", "default", "test",  "aName", "acc", "ge", "s", "t" },//28
        {"geAttribute", "default", "test",  "aName", "Ccc", "ge", "s", "t" },//29


        //le case
        {"leAttribute", "1",       "test",  "aName", "acc", "le", "s", "t" },//30
        {"leAttribute", "1",       "test",  "aName", "ccc", "le", "s", "t" },//31
        {"leAttribute", "default", "test",  "aName", "dcc", "le", "s", "t" },//32
        {"leAttribute", "1",       "test",  "aName", "Ccc", "le", "s", "t" },//33

        //gt no case
        {"gtAttribute", "1",       "test",  "aName", "dcc", "gt", "s", "f" },//34
        {"gtAttribute", "default", "test",  "aName", "ccc", "gt", "s", "f" },//35
        {"gtAttribute", "default", "test",  "aName", "acc", "gt", "s", "f" },//36
        {"gtAttribute", "default", "test",  "aName", "Ccc", "gt", "s", "f" },//37

        //lt no case
        {"ltAttribute", "1",       "test",  "aName", "acc", "lt", "s", "f" },//38
        {"ltAttribute", "default", "test",  "aName", "ccc", "lt", "s", "f" },//39
        {"ltAttribute", "default", "test",  "aName", "dcc", "lt", "s", "f" },//40
        {"ltAttribute", "default", "test",  "aName", "Ccc", "lt", "s", "f" },//41

        //ge no case
        {"geAttribute", "1",       "test",  "aName", "dcc", "ge", "s", "f" },//42
        {"geAttribute", "1",       "test",  "aName", "ccc", "ge", "s", "f" },//43
        {"geAttribute", "default", "test",  "aName", "acc", "ge", "s", "f" },//44
        {"geAttribute", "1",       "test",  "aName", "Ccc", "ge", "s", "f" },//45

        //le no case
        {"leAttribute", "1",       "test",  "aName", "acc", "le", "s", "f" },//46
        {"leAttribute", "1",       "test",  "aName", "ccc", "le", "s", "f" },//47
        {"leAttribute", "default", "test",  "aName", "dcc", "le", "s", "f" },//48
        {"leAttribute", "1",       "test",  "aName", "Ccc", "le", "s", "f" },//49
//---------------------- value is 123
        //gt numeric
        {"gtnAttribute","default",  "test",  "aName", "122",    "gt", "n", "" },//50
        {"gtnAttribute","default",  "test",  "aName", "123",    "gt", "n", "" },//51
        {"gtnAttribute","1",        "test",  "aName", "124",    "gt", "n", "" },//52
        {"gtnAttribute","1",        "test",  "aName", "1067.3", "gt", "n", "" },//53

        //lt numeric
        {"ltnAttribute","1",        "test",  "aName", "122",    "lt", "n", "" },//54
        {"ltnAttribute","default",  "test",  "aName", "123",    "lt", "n", "" },//55
        {"ltnAttribute","default",  "test",  "aName", "124",    "lt", "n", "" },//56
        {"ltnAttribute","default",  "test",  "aName", "1067.3", "lt", "n", "" },//57

        //ge numeric
        {"genAttribute","default",   "test",  "aName", "122",    "ge", "n", "" },//58
        {"genAttribute","1",         "test",  "aName", "123",    "ge", "n", "" },//59
        {"genAttribute","1",         "test",  "aName", "124",    "ge", "n", "" },//60
        {"genAttribute","1",         "test",  "aName", "1067.3", "ge", "n", "" },//61

        //le numeric
        {"lenAttribute","1",         "test",  "aName", "122",    "le", "n", "" },//62
        {"lenAttribute","1",         "test",  "aName", "123",    "le", "n", "" },//63
        {"lenAttribute","default",   "test",  "aName", "124",    "le", "n", "" },//64
        {"lenAttribute","default",   "test",  "aName", "1067.3", "le", "n", "" },//65

        // in case
        {"inAttribute", "1",         "test",  "aName", "cccc",   "in", "s", "t" },//66
        {"inAttribute", "default",   "test",  "aName", "CCCC",   "in", "s", "t" },//67
        {"inAttribute", "1",         "test",  "aName", "ccc",    "in", "s", "t" },//68
        {"inAttribute", "default",   "test",  "aName", "CCC",    "in", "s", "t" },//69
        {"inAttribute", "default",   "test",  "aName", "aCCCc",  "in", "s", "t" },//70
        {"inAttribute", "default",   "test",  "aName", "a",      "in", "s", "t" },//71
        {"inAttribute", "default",   "test",  "aName", "ac",     "in", "s", "t" },//72

        // in no case
        {"inAttribute", "1",         "test",  "aName", "cccc",   "in", "s", "f" },//73
        {"inAttribute", "1",         "test",  "aName", "CCCC",   "in", "s", "f" },//74
        {"inAttribute", "1",         "test",  "aName", "ccc",    "in", "s", "f" },//75
        {"inAttribute", "1",         "test",  "aName", "CCC",    "in", "s", "f" },//76
        {"inAttribute", "1",         "test",  "aName", "aCCCc",  "in", "s", "f" },//77
        {"inAttribute", "default",   "test",  "aName", "a",      "in", "s", "f" },//78
        {"inAttribute", "default",   "test",  "aName", "ac",     "in", "s", "f" },//79

        // start case
        {"stAttribute", "1",         "test",  "aName", "cccd",   "st", "s", "t" },//80
        {"stAttribute", "default",   "test",  "aName", "CCCC",   "st", "s", "t" },//81
        {"stAttribute", "1",         "test",  "aName", "ccc",    "st", "s", "t" },//82
        {"stAttribute", "default",   "test",  "aName", "CCC",    "st", "s", "t" },//83
        {"stAttribute", "default",   "test",  "aName", "aCCCc",  "st", "s", "t" },//84
        {"stAttribute", "default",   "test",  "aName", "a",      "st", "s", "t" },//85
        {"stAttribute", "default",   "test",  "aName", "ac",     "st", "s", "t" },//86

        // start no case
        {"stAttribute", "1",         "test",  "aName", "cccd",   "st", "s", "f" },//87
        {"stAttribute", "1",         "test",  "aName", "CCCC",   "st", "s", "f" },//88
        {"stAttribute", "1",         "test",  "aName", "ccc",    "st", "s", "f" },//89
        {"stAttribute", "1",         "test",  "aName", "Ccc",    "st", "s", "f" },//90
        {"stAttribute", "default",   "test",  "aName", "aCCCc",  "st", "s", "f" },//91
        {"stAttribute", "default",   "test",  "aName", "a",      "st", "s", "f" },//92
        {"stAttribute", "default",   "test",  "aName", "ac",     "st", "s", "f" },//93

        // existst
        {"exAttribute", "1",         "test",  "aName", "cccd",   "ex", "s", "" },//94
        {"exAttribute", "default",   "test",  "pName", "cccd",   "ex", "s", "" },//95
        // does not existst
        {"nxAttribute", "1",         "test",  "pName", "cccd",   "nx", "s", "" },//94
        {"nxAttribute", "default",   "test",  "aName", "cccd",   "nx", "s", "" },//95

        // TODO: OR AND
        {"*", "", "", "", "", "", "", "" }
    };
    FTTestData *test = &dataE[0];
    int i = 0;
    while(test->testName != "*" ) {
        if( !testFunctional( i, test->testName, test->styleName, test->elementName, test->attrName, test->attrValue, "e", test->op, test->type, test->caseStr ) ) {
            return false;
        }
        test++;
        i++;
    }

    test = &dataA[0];
    i = 0;
    while(test->testName != "*" ) {
        if( !testFunctional(i, test->testName, test->styleName, test->elementName, test->attrName, test->attrValue, "a", test->op, test->type, test->caseStr ) ) {
            return false;
        }
        test++;
        i++;
    }

    //------- and-or
    if(! testFunctionalForRule("orTrue", STYLE_OR, FILE_STYLE_OR, 1 ) ) {
        return false;
    }
    if(! testFunctionalForRule("orFalse", STYLE_DEFAULT, FILE_STYLE_OR, 2 ) ) {
        return false;
    }
    if(! testFunctionalForRule("andTrue", STYLE_AND, FILE_STYLE_AND, 3 ) ) {
        return false;
    }
    if(! testFunctionalForRule("andFalse", STYLE_DEFAULT, FILE_STYLE_AND, 4 ) ) {
        return false;
    }
    //------- axis
    if(! testFunctionalForRule("axisParent", STYLE_AXIS_PARENT, FILE_STYLE_AXIS_PARENT, 0 ) ) {
        return false;
    }
    if(! testFunctionalForRuleAlt("axisParent", STYLE_DEFAULT, FILE_STYLE_AXIS_PARENT, 0 ) ) {
        return false;
    }
    if(! testFunctionalForRule("axisGrandParent", STYLE_AXIS_GRANDPARENT, FILE_STYLE_AXIS_GRANDPARENT, 0 ) ) {
        return false;
    }
    if(! testFunctionalForRuleAlt("axisGrandParent", STYLE_DEFAULT, FILE_STYLE_AXIS_GRANDPARENT, 0 ) ) {
        return false;
    }
    if(! testFunctionalForRule("axisGrandParentName", STYLE_AXIS_GRANDPARENTNAME, FILE_STYLE_AXIS_GRANDPARENTNAME, 0 ) ) {
        return false;
    }
    if(! testFunctionalForRuleAlt("axisGrandParentName", STYLE_DEFAULT, FILE_STYLE_AXIS_GRANDPARENTNAME, 0 ) ) {
        return false;
    }
    //-- position
    if(! testFunctionalForRule("posFirst", STYLE_POSITION_FIRST, FILE_STYLE_POSITION_FIRST, 0 ) ) {
        return false;
    }
    if(!testFunctionalForRule("posFirst", STYLE_DEFAULT, FILE_STYLE_POSITION_FIRST, 1 ) ) {
        return false;
    }
    if(!testFunctionalForRule("posLast", STYLE_POSITION_LAST, FILE_STYLE_POSITION_LAST, 4 ) ) {
        return false;
    }
    if(!testFunctionalForRule("posLast", STYLE_DEFAULT, FILE_STYLE_POSITION_LAST, 2 ) ) {
        return false;
    }
    if(! testFunctionalForRule("posNumTrue", STYLE_POSITION, FILE_STYLE_POSITION, 1 ) ) {
        return false;
    }
    if( !testFunctionalForRule("posNumFalse", STYLE_DEFAULT, FILE_STYLE_POSITION, 3 ) ) {
        return false;
    }
    //---
    return true;

}

