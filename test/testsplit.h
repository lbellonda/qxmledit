/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
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

#ifndef TESTSPLIT_H
#define TESTSPLIT_H

#include <QString>
#include "../src/extraction/extractionoperation.h"
#include "testbase.h"

class TestSplit : public TestBase
{
    QString _lastTimeStamp;
    bool _showXML;
    bool compareData(const QString &dataXML, const QString &fileName);
    bool checkSplit(ExtractResults &results, const int id, const QString &referenceFile);
    bool splitAndNavigateFilter(const bool isReverseRange, const QString &fileReference, const QString &fileResult, const int minDoc = 2, const int MaxDoc = 4);
    bool checkFilter(const QString &file1, const QString &file2);
    void setupFilterParameters(ExtractionOperation *operation, const bool isReverseRange, const QString &extractFolder, const QString &timeStamp, const QString &fileInput, const int minDoc, const int MaxDoc);
    void setupFilterParametersCfr(ExtractionOperation *operation, const QString &extractFolder,
                                  const QString &timeStamp, const QString &fileInput,
                                  const QString &attrName, const ExtractionOperation::ECfrOp op, const QString &cfrTerm);
    bool splitAndNavigateFilterCfr(const QString &fileReference, const QString &fileResult,
                                   const QString &attrName, const ExtractionOperation::ECfrOp op, const QString &cfrTerm);
    QString newTS();

    bool testSplitByDepth0();
    bool testSplitByDepth1();
    bool testSplitByDepth2();
    bool checkResetParametersFilterText(ExtractionOperation &opresults);
    bool checkSetParametersFilterText(ExtractionOperation &opresults, const QString &path);
    bool testFilterTextRelative1Split();
    bool testFilterTextRelative2Split();
    bool testFilterTextRelative1Filter();
    bool testFilterTextRelative2Filter();
    bool testFilterTextRelative2FilterFalse();
    bool testFilterTextRelative1SplitWrongPath();
    bool testFilterTextRelativeFilterWrongPath();
    bool testFilterTextAbsoluteSplit();
    bool testFilterTextAbsoluteFilter();
    bool testFilterTextAbsoluteSplitWrongPath();
    bool testFilterTextAbsoluteFilterWrongPath();
    bool testFilterTextRelative2FilterExpectedFalse();

    void setupFilterParametersFilterText(ExtractionOperation *operation,
                                                    const QString &extractFolder,
                                                  const QString &timeStamp, const QString &fileInput,
                                                    const QString &pathForText, const bool isFilter);

    bool splitAndNavigateFilterText(const QString &fileReference, const bool isFilter,
                                                   const QString &fileResult1, const QString &fileResult2,
                                                   const QString &relativePath);
    bool splitAndNavigateFilterText(const QString &fileReference, const bool isFilter,
                                                   const QString &fileResult1, const QString &fileResult2,
                                                   const QString &relativePath, const bool expectPositiveResult );

    using TestBase::error;
    bool error();

public:
    TestSplit();

    bool testParameters();
    bool testParametersNoExtract();
    bool testSplit();
    bool testSplitAndNavigate();
    bool testSplitByDepth();
    bool testSplitFilterTextAbsolute();
    bool testSplitFilterTextRelative();

    bool testSplitAndNavigateFilter();
    bool testSplitAndNavigateFilterMoreComplex();
    bool testSplitAndNavigateFilterReverse();
    bool testSplitAndNavigateFilterMore();
    bool testSplitAndNavigateFilterIdDifferent();
    bool testSplitAndNavigateFilterIdEqual();

};

#endif // TESTSPLIT_H
