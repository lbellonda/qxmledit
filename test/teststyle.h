/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013-2018 by Luca Bellonda and individual contributors  *
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


#ifndef TESTSTYLE_H
#define TESTSTYLE_H

#include "testbase.h"

#include "testbase.h"
#include "style.h"

class TestStyle : public TestBase
{
    bool checkFirstIndex(QList<StyleRuleSet*> &rules, int firstIndex);
    bool checkSecondIndex(QList<StyleCalc *> rules, int index);
    StyleRule* getRuleItem(QList<StyleRuleSet*> &rules, int firstIndex, int secondIndex );
    StyleRuleSet* getRuleSetItem(QList<StyleRuleSet*> &rules, int firstIndex, int secondIndex );
    StyleRuleSet* getRuleSetItem(QList<StyleRuleSet*> &rules, int firstIndex);
    bool checkName(QList<StyleRuleSet*> &rules, int firstIndex, int index, const QString testValue );
    bool checkEntity(QList<StyleRuleSet*> &rules, int firstIndex, int secondIndex, const bool testValue );
    bool checkType(QList<StyleRuleSet*> &rules, int firstIndex, int secondIndex, const StyleRule::ECfrType testValue );
    bool checkAxis(QList<StyleRuleSet*> &rules, int firstIndex, int secondIndex, const QString &testValue );
    bool checkValue(QList<StyleRuleSet*> &rules, int firstIndex, int secondIndex, const QString testValue );
    bool checkCase(QList<StyleRuleSet*> &rules, int firstIndex, int secondIndex, const bool testValue );
    bool checkConnector(QList<StyleRuleSet*> &rules, int firstIndex, int secondIndex, const bool andConnector );
    bool checkRSStyleId(QList<StyleRuleSet*> &rules, int firstIndex, int secondIndex, const QString &idExpected );
    bool checkChildrenNum(QList<StyleRuleSet*> &rules, int firstIndex, const int numChildren );
    bool checkRSStyleId(QList<StyleRuleSet*> &rules, int firstIndex, const QString &idExpected );
    bool checkOp(QList<StyleRuleSet*> &rules, int firstIndex, int secondIndex, const StyleRule::oper testValue );
    bool testFunctional(int id, const QString &testName, const QString &styleName, const QString &elementName, const QString &attrName, const QString &attrValue, const QString &entity, const QString &op, const QString &type, const QString &caseStr );
    //bool testFunctionalForRule(const QString &testName, const QString &styleName, const QString &fileStyle, const int numElement );
    bool testFunctionalForRuleInner(const QString &testName, const QString &dataFileName, const QString &styleName, const QString &fileStyle, const int numElement );
    bool testFunctionalForRule(const QString &testName, const QString &styleName, const QString &fileStyle, const int numElement );
    bool testFunctionalForRuleAlt(const QString &testName, const QString &styleName, const QString &fileStyle, const int numElement );
public:
    TestStyle();

    bool testLoadCalcStyle();
    bool testCalcStyle();
};

#endif // TESTSTYLE_H
