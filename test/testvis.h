/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2018 by Luca Bellonda and individual contributors  *
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


#ifndef TESTVIS_H
#define TESTVIS_H

#include "testbase.h"
#include "visualization/elementbase.h"

class AttributesSummaryData;
class AttributesSummaryTotal;
class AttributesSummarySummary;

class TestVis : public TestBase
{
    ElementBase *buildElm(ElementBase *parent, const int level);
    void count(ElementBase *e, int &total );
    bool testBaseElement();
    bool testDialog();
    bool testDataThreading(const bool useStandard);
    bool testSliceElements();
    //----
    bool testAttributeCountUnit();
    bool testAttributeCountNoNo(const bool isLoad);
    bool testAttributeCountNoYes(const bool isLoad);
    bool testAttributeCountYesNo(const bool isLoad);
    bool testAttributeCountYesYes(const bool isLoad);
    bool testAttributeCountSaveCSV();
    bool testAttributeCountLoadingFile();
    AttributesSummaryData* buildReferenceAttributesSummaryData( AttributesSummaryData* a, AttributesSummaryTotal &totalTotal, AttributesSummaryTotal &totalUsed, AttributesSummaryTotal &totalNotUsed, const bool isWhitelist, const bool isBlackList);
    bool testAttributeBase( const bool isLoad, const bool isWhitelist, const bool isBlackList );
    bool testAttributeBaseInner(AttributesSummaryData *attributesSummaryData, const bool isWhiteList, const bool isBlackList);
    bool testAttributeBaseBigData();
    bool buildReferenceAttributesBigData( AttributesSummaryData* attributesSummaryData, AttributesSummarySummary *ast);
    //----
public:
    TestVis();

    bool test();
    bool testSummary();
    bool testFast();
    bool testVisData();
    bool testAttributeCount();
};

#endif // TESTVIS_H
