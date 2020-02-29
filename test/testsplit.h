/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2020 by Luca Bellonda and individual contributors  *
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

#include "xmlEdit.h"
#include "../src/extraction/extractionoperation.h"
#include "../src/extraction/extractionscriptingprovider.h"
#include "testbase.h"
#include "helpers/testsplitscriptinghelper.h"
#include "helpers/testsplitscriptingoperationhelper.h"
#include "helpers/testextractionexecutorhelper.h"
#include "extraction/extractfragmentsdialog.h"
#include "extraction/extractionadavancedoptionsdialog.h"

class ExtractionScriptFilterModel ;
class ExtractionScriptEventModel ;
class ExtractionScriptEventHandler;
class ExtractionScriptElementEvent;

class TestExtractionHolder {
public:
    ExtractResults results;
    ExtractionOperation op;
    QString fileName;

    TestExtractionHolder();
    ~TestExtractionHolder();
};

class TestSplit : public TestBase
{
    QString _lastTimeStamp;
    bool _showXML;

    //---------
    bool testUnitOperation();
    bool compareAttributes(const int code, ExtractionScriptElementEvent *elementEvent, QList<QString> expectedAttributesSorted, QHash<QString,QString> expectedAttributes);
    bool compareAttributesNS(const int code, ExtractionScriptElementEvent *elementEvent,
                                        QList<QString> expectedAttributeNamespaces,
                                        QList<QString> expectedAttributeNames,
                                        QList<QString> expectedAttributeValues);
    bool testScriptEventElement();
    bool unitTestFilterEventElementProperties();
    void setupElementEvent(ExtractionOperation *operation, const QString &code);
    bool testScriptEventElementTemplate(TestExtractionHolder *holder, const QString &sourceFile, const QString &code1, const QString &code2);
    bool testScriptEventElementAndTextTemplate(TestExtractionHolder *holder, const QString &sourceFile, const QString &codeText, const QString &codeElement);
    bool checkTextEvent(const QString &sourceFile, const QString &code, const bool expectedError, const ExtractionOperation::EXMLErrors expectedErrorCode, const QString &fileToCheck);
    bool executeOperationForScripting(TestExtractionHolder *holder, const bool expectedError, const ExtractionOperation::EXMLErrors expectedErrorCode, const QString &fileToCheck);
    void setupTextEvent(ExtractionOperation *operation, const QString &code);
    bool testScriptEventTextTemplate(TestExtractionHolder *holder, const QString &sourceFile, const QString &code);
    bool testScriptEventText();
    void initOperationForScripting(TestExtractionHolder *holder, const QString &inputFile);
    bool unitTestScriptingBaseCheck(ExtractionScriptFilter *filter, const bool expectedEnabled, QList<ExtractionScriptEventHandler*> expectedHandlers);
    bool unitTestScriptingBaseCheckHelper(TestSplitScriptingHelper *helper, const bool expectedEnabled);
    bool unitTestScriptingBase();
    bool unitTestFilterDefault();
    bool unitTestFilterOneEvent();
    bool unitTestFilterEvents();
    bool unitTestFilterInterface();
    bool unitTestFilterInterfaceAccessibility();
    bool unitTestFilterInterfaceProperties();
    bool unitTestFilterEventChain();
    bool unitAllEventsChainOnScriptManager();
    //--
    bool performScriptSelectDialogAction(ExtractionAdavancedOptionsDialog *dialog, const int selectionAction);
    bool testScriptingAdvancedOptions();
    bool testSelectUseNamespaces();
    bool setOptionUseNS(const QString &code, const bool expected, const bool initialValue);
    void addAttributesToList(QList<QPair<QString, QString> > &translatesAttributes, QXmlStreamAttributes inputAttributes);
#ifdef QXMLEDIT_JS_SCRIPT
    bool testScriptWithCase0();
    bool testScriptWithCase0Filter();
    bool testScriptWithCase0FilterMulti();
    bool testScriptWithCase0FilterMultiNoScript();
    bool testScriptWithCase0Split();
    bool testScriptWithCase1Split();
    bool testScriptWithCase1NoScriptSplit();
    bool testScriptingJS();
    void setupPredFiltersValues(ExtractionOperation *op, const QStringList &filtersList);
    bool verifyScriptList(const QString &code, ExtractionOperation *op, const QStringList &expectedFiltersList);
    bool loadCheckOptionsPredFilters(const QString &code, const QStringList &initialValues, const QStringList &expectedValues, const int selectionAction);
    bool setOptionIntScript(const QString &code, const bool expected, const bool initialValue);
    bool testSelectChooseScriptFromList();
    bool unitTestFilterEventElement();
    bool unitTestFilterEventElementAccessibility();
    bool unitTestFilterEventElementMultipleEvents();
    bool unitTestOperationBase();
    bool checkElementEventBase(const int code, ExtractionScriptElementEvent *elementEvent, QList<QPair<QString,QString> > inputAttributes, const bool expectedModified);
    bool checkElementEventWithEngine(const int code, QJSEngine &engine, ExtractionScriptElementEvent *elementEvent, QList<QPair<QString,QString> > inputAttributes, const bool expectedModified, const QString &expectedVariableValue);
    bool checkElementEventWithReturnCode(const int code, TestSplitScriptingOperationHelper &helper, ExtractionScriptElementEvent *elementEvent, QList<QPair<QString,QString> > inputAttributes, const bool expectedModified, const ExtractionScriptManager::EEventResult expectedResult,const int expectedElementEventsCount );
    bool checkElementEvent(const QString &sourceFile, const QString &code1, const QString &code2, const bool expectedError, const ExtractionOperation::EXMLErrors expectedErrorCode, const QString &fileToCheck);
    bool checkElementAndTextEvent(const QString &sourceFile, const QString &codeText, const QString &codeElement, const bool expectedError, const ExtractionOperation::EXMLErrors expectedErrorCode, const QString &fileToCheck);
    bool checkAttributeStream(QPair<QString,QString> attribute, const int index, const QString &expectedName, const QString &expectedValue);
#endif
    //--
    bool checkFilterEvent(const QString &code, ExtractionScriptEventModel *event, const EExtractionEventType expectedType, const bool expectedEnabled, const QString &expectedHandler, const QString &expectedDescription, const QString &expectedCode);
    bool loadEventOne();
    bool loadEventTwo();
    bool loadEventMalformed();
    bool unitTestFilterLoad();
    //--
    bool testScriptWithPredefinedScriptingFilterAll();
    bool testPredefinedScriptNoNsFilterAll();
    bool testPredefinedScriptNsFilterAll();
    bool testScriptWithPredefinedScripting();
    bool testScriptWithPredefinedScriptingSplit();
    bool testScriptWithPredefinedScriptingFilter();
    bool testPredefinedScriptTrimAttributesNoNsSplit();
    bool testPredefinedScriptTrimAttributesNsSplit();
    bool testPredefinedScriptRemoveEmptyAttributesNoNsSplit();
    bool testPredefinedScriptRemoveEmptyAttributesNsSplit();
    bool testPredefinedScriptTrimAttributesNoNsFilter();
    bool testPredefinedScriptTrimAttributesNsFilter();
    bool testPredefinedScriptRemoveEmptyAttributesNoNsFilter();
    bool testPredefinedScriptRemoveEmptyAttributesNsFilter();
    bool testPredefinedScriptExecute(const bool useNamespaces, const bool isFilter, const QString &fileReference, const QString &fileResult, QList<ExtractionScriptingProvider::EPredefinedScripts> scripts);
    bool testPredefinedScriptExecute(const bool useNamespaces, const bool isFilter, const QString &fileReference, const QString &fileResult, QList<ExtractionScriptingProvider::EPredefinedScripts> scripts, const int depth, const bool isAllDocuments, const unsigned int minDoc, const unsigned int maxDoc);
    bool testPredefinedScriptExecuteList(const bool useNamespaces, const bool isFilter, const QString &fileReference,
                                                    const QStringList referenceFiles, QList<ExtractionScriptingProvider::EPredefinedScripts> scripts,
                                                    const int depth, const bool isAllDocuments, const unsigned int minDoc, const unsigned int maxDoc);

    //--
    bool unitTestFilterEventText();
    bool unitTestFilterEventTextProperties();
    bool unitTestFilterEventTextAccessibility();
    bool unitTestFilterEventTextMultipleEvents();
    bool checkText(const int code, ExtractionScriptTextEvent *textEvent, const bool expectedCDATA, const QString &expectedText, const bool expectedIgnored, const bool expectedModified);
    bool checkTextMultiple(const int code, TestSplitScriptingOperationHelper &helper, ExtractionScriptManager::EEventResult expectedResult, ExtractionScriptTextEvent *textEvent, const bool expectedCDATA, const QString &expectedText, const bool expectedIgnored, const bool expectedModified, const int expectedTextEventsCount);
    bool singleEventTextMultipleEvents(const QString & testName, const int code, const QString & codeEvent1, const QString & codeEvent2, ExtractionScriptManager::EEventResult expectedResult, const bool expectedCDATA, const QString &expectedText, const bool expectedIgnored, const bool expectedModified, const int expectedTextEvents);
    //---------
    bool groupCSVXML(const bool isXMLOrCSV, const QString &fileReference, const QString &fileResult, const bool isPath);
    bool checkCSV(const QString &file1, const QString &fileReference);
    bool group(const QString &fileReference, const QString &fileResultCSV, const QString &fileResultXML , const QString &fileResultCSVN, const QString &fileResultXMLN);

    bool compareData(const QString &dataXML, const QString &fileName);
    bool checkSplit(ExtractResults &results, const int id, const QString &referenceFile);
    bool splitAndNavigateFilter(const bool isReverseRange, const QString &fileReference, const QString &fileResult, const int minDoc = 2, const int MaxDoc = 4);
    bool checkFilter(const QString &file1, const QString &file2);
    bool checkFilter(const QString &file1, const QString &file2, const bool expectedPositiveResult);
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
    bool testSplitByDepth0_1();
    bool testSplitByDepth1_1();
    bool testSplitByDepth2_1();
    bool testSplitByDepth0_2();
    bool testSplitByDepth1_2();
    bool testSplitByDepth2_2();
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

    bool testScriptActivation();
    bool testScriptEmptyActivation();

public:
    TestSplit();
    ~TestSplit();

    bool testFast();
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
    bool testSplitGroup();
    bool testScripting();
};

#endif // TESTSPLIT_H
