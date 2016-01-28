/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2016 by Luca Bellonda and individual contributors  *
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

#include "TestQXmlEdit.h"
#include <QtGlobal>

const char *APP_TITLE = QT_TR_NOOP("QXmlEditTest");

void testSearchInFileWorking(volatile bool *running, XmlScanInfo *info, const QString &filePath);

TestQXmlEdit::TestQXmlEdit()
{
}

void TestQXmlEdit::initTestCase()
{
    Utils::isUnitTest = true ;
    Utils::silenceMessages1 = true ;
    Utils::silenceMessages2 = true ;
}

void TestQXmlEdit::cleanupTestCase()
{
}

void TestQXmlEdit::test1()
{
    CompareXML  c;
    bool result = c.comparePlainDocuments();
    QVERIFY2(result, QString("compare1").toLatin1().data());
}

void TestQXmlEdit::test2()
{
    CompareXML  c;
    bool result = !c.comparePlainDocumentsFalse();
    QVERIFY2(result, QString("compare2").toLatin1().data());
}

void TestQXmlEdit::testMixed()
{
    CompareXML  c;
    bool result = c.compareMixedDocuments();
    QVERIFY2(result, QString("compareMixed").toLatin1().data());
}

void TestQXmlEdit::testMixed2()
{
    CompareXML  c;
    bool result = !c.compareMixedDocumentsFalse();
    QVERIFY2(result, QString("compareMixed False").toLatin1().data());
}

void TestQXmlEdit::test()
{
    XmlScanInfo info;
    if(!info.init("/a/b/c", true)) {
        QVERIFY2(false, "SFC: Unable to begin the search.");
    }
    if(!info.check()) {
        QVERIFY2(false, "SFC: The pattern to search is not well formed.");
        return ;
    }

    bool running = true ;
    QFuture<void> future = QtConcurrent::run(testSearchInFileWorking, &running, &info, QString("../test/data/testcount.xml"));
    future.waitForFinished();
    QVERIFY2(!info.isError, "SFC: error in method.");
    // check the result
    QVERIFY2(info.size == 3, "SFC: Number of rows");
    // devo verificare l'equivalenza degli hash map.
    // Costruisco le mappe corrispondenti al test
    QMap<int, int> expecteds [3];
    expecteds[0][1] = 1;
    expecteds[1][17] = 1;
    expecteds[2][0] = 1;
    expecteds[2][1] = 2;
    expecteds[2][2] = 6;
    expecteds[2][3] = 3;
    expecteds[2][4] = 5;

    for(int i = 0 ; i < info.size ; i ++) {
        //verify Hash Map equivalence
        if(!verifyMaps(info.groups[i], expecteds[i])) {
            QVERIFY2(false, QString("SFC: Map %1 different").arg(i).toLatin1().data());
        }
    }
}

bool TestQXmlEdit::verifyMaps(QMap<int, int> &reference, QMap<int, int> &candidate)
{
    if(reference.count() != candidate.count()) {
        return false ;
    }
    foreach(int key1, reference.keys()) {
        if(!candidate.contains(key1)) {
            return false;
        }
        if(reference[key1] != candidate[key1]) {
            return false ;
        }
    }
    return true ;
}

void TestQXmlEdit::testComment()
{
    TestComment tc;
    bool result ;
    result = tc.testMakeComment();
    QVERIFY2(result, "Test Comment: make a comment.");
    result = tc.testUndoComment();
    QVERIFY2(result, "Test Comment: undo a comment.");
    result = tc.testUndoUndoableComment();
    QVERIFY2(!result, "Test Comment: undo an undoable comment.");
}

void TestQXmlEdit::testXsd()
{
    TestXsd tx ;
    bool result ;
    result = tx.TestNonamespace();
    QVERIFY2(result, "Test XSD: TestNonamespace.");
    result = tx.TestNonamespaceFalse();
    QVERIFY2(result, "Test XSD: TestNonamespaceFalse.");
    result = tx.TestOneSchema();
    QVERIFY2(result, "Test XSD: TestOneSchema.");
    result = tx.TestMoreSchema();
    QVERIFY2(result, "Test XSD: TestMoreSchema.");
    result = tx.testSameNamespace();
    QVERIFY2(result, "Test XSD: testSameNamespace.");
}

void TestQXmlEdit::testPaste()
{
    TestPaste tp;
    bool result ;
    result = tp.testAPaste();
    QVERIFY2(result, (QString("test Paste 1 '%1'").arg(tp.errorString())).toLatin1().data());
    TestPaste tp2;
    result = tp2.testPasteMultiple();
    QVERIFY2(result, (QString("test Paste 2 '%1'").arg(tp2.errorString())).toLatin1().data());
}


void TestQXmlEdit::test_data()
{
    QTest::addColumn<QString>("data");
    QTest::newRow("0") << QString();
}

void TestQXmlEdit::testSplit()
{
    TestSplit ts;
    bool result ;

    result = ts.testParameters();
    QVERIFY2(result, QString("Test Split: parameters. details: %1").arg(ts.errorString()).toLatin1().data());
    result = ts.testParametersNoExtract();
    QVERIFY2(result, "Test Split: parameters w/o extraction.");
    result = ts.testSplit();
    QVERIFY2(result, "Test Split: split.");
    result = ts.testSplitAndNavigate();
    QVERIFY2(result, "Test Split: split and navigate.");

    //-------- others
    result = ts.testSplitAndNavigateFilter();
    QVERIFY2(result, "Test Split: filter.");
    result = ts.testSplitAndNavigateFilterMoreComplex();
    QVERIFY2(result, "Test Split: more complex filter.");
    result = ts.testSplitAndNavigateFilterReverse();
    QVERIFY2(result, "Test Split: filter reverserange.");
    result = ts.testSplitAndNavigateFilterMore();
    QVERIFY2(result, "Test Split: filter more.");
    //------------------------
    result = ts.testSplitAndNavigateFilterIdDifferent();
    QVERIFY2(result, "Test Split: filter diff.");
    result = ts.testSplitAndNavigateFilterIdEqual();
    QVERIFY2(result, "Test Split: filter equal.");
    //------
    result = ts.testSplitByDepth();
    QVERIFY2(result, "Test Split: by depth");
    //----
    result = ts.testSplitFilterTextAbsolute();
    QVERIFY2(result, "Test Split absolute: filter diff.");
    result = ts.testSplitFilterTextRelative();
    QVERIFY2(result, "Test Split Relative: filter diff.");

}

void TestQXmlEdit::testMixedContent()
{
    TestMixedContent testMixedContent;
    bool result ;
    result = testMixedContent.test_m1();
    QVERIFY2(result, "TestMixedContent: case 1.");
    result = testMixedContent.test_m3();
    QVERIFY2(result, "TestMixedContent: case 3.");
    result = testMixedContent.test_m6();
    QVERIFY2(result, "TestMixedContent: case 6.");
}

void TestQXmlEdit::testBase64()
{
    bool result ;
    {
        TestBase64 testBase64;
        result = testBase64.testUnits();
        QVERIFY2(result, (QString("TestBase64 testUnits: %1").arg(testBase64.errorString())).toLatin1().data());
    }
    {
        TestBase64 testBase64;
        result = testBase64.test_base64();
        QVERIFY2(result, (QString("TestBase64 test_base64: %1").arg(testBase64.errorString())).toLatin1().data());
    }
    {
        TestBase64 testBase64;
        result = testBase64.test_base64_base();
        QVERIFY2(result, (QString("TestBase64 test_base64_base: %1").arg(testBase64.errorString())).toLatin1().data());
    }
    {
        TestBase64 testBase64;
        result = testBase64.test_base64_file();
        QVERIFY2(result, (QString("TestBase64 file: %1").arg(testBase64.errorString())).toLatin1().data());
    }
    {
        TestBase64 testBase64;
        result = testBase64.test_base64_utils();
        QVERIFY2(result, (QString("TestBase64 utils: %1").arg(testBase64.errorString())).toLatin1().data());
    }
}


void TestQXmlEdit::testEditingCommands()
{
    TestEditingCommands te;
    bool result ;
    result = te.testInnerAppendElement();
    QVERIFY2(result, "Test inner append");
}

void TestQXmlEdit::testValidation()
{
    TestValidation tv;
    bool result ;
    result = tv.test();
    QVERIFY2(result, QString("Test validation %1").arg(tv.errorString()).toLatin1().data());
}

void TestQXmlEdit::testFileUI()
{
    TestUIFile tf;
    bool result ;
    result = tf.testNew();
    QVERIFY2(result, "Test FileUI");
}

void TestQXmlEdit::testSessions()
{
    TestSessions ts;
    bool result ;
    result = ts.testUI1();
    QVERIFY2(result, "0 Test Sessions testUI1");
    result = ts.testStartSessionDisabled();
    QVERIFY2(result, "-1 testStartSessionDisabled");
    result = ts.testStartWithoutSessions();
    QVERIFY2(result, "1 testStartWithoutSessions");
    result = ts.testStartWithLastSessionIdStateActive();
    QVERIFY2(result, "2 testStartWithLastSessionIdStateActive");
    result = ts.testStartWithLastSessionIdStatePaused();
    QVERIFY2(result, "3 testStartWithLastSessionIdStatePaused");
    result = ts.testStartWithLastSessionIllegalState();
    QVERIFY2(result, "4 testStartWithLastSessionIllegalState");
    result = ts.testCmdPauseCheck();
    QVERIFY2(result, "5 testCmdPauseCheck");
    result = ts.testCmdResume();
    QVERIFY2(result, "6 testCmdResume");
    result = ts.testCmdClose();
    QVERIFY2(result, "7 testCmdClose");
    result = ts.testRegisterAccess();
    QVERIFY2(result, "8 testRegisterAccess");
    result = ts.testSummaryData();
    QVERIFY2(result, QString("9 testSummaryData: %1").arg(ts.errorString()).toLatin1().data());
    result = ts.testAccessFileWithSessionsDisabled();
    QVERIFY2(result, "11 testAccessFileWithSessionsDisabled");
}

void TestQXmlEdit::testSql()
{
    TestSQLAccess tsa;
    bool result ;

    result = tsa.insertSession();
    QVERIFY2(result, "Test insert Sessions");
    result = tsa.testFile();
    QVERIFY2(result, "Test insert File");
    result = tsa.testTransCommands();
    QVERIFY2(result, "Test transactions");
    result = tsa.testFileNotFound();
    QVERIFY2(result, "Test file not found");
    result = tsa.insertFileAccessLow();
    QVERIFY2(result, "Test file access low");
    result = tsa.insertFileAccess();
    QVERIFY2(result, "Test file access");
    result = tsa.readFileAccess();
    QVERIFY2(result, "Test read file access");
    result = tsa.deleteSession();
    QVERIFY2(result, "Test delete session");
    result = tsa.updateSession();
    QVERIFY2(result, "Test update session");
    result = tsa.touchSession();
    QVERIFY2(result, "Test touch session");

    result = tsa.deleteByDate1Month();
    QVERIFY2(result, QString("deleteByDate1Month %1").arg(tsa.msg()).toLatin1().data());
    result = tsa.deleteByDate6Months();
    QVERIFY2(result, QString("deleteByDate6Months %1").arg(tsa.msg()).toLatin1().data());
    result = tsa.deleteByGivenDate();
    QVERIFY2(result, QString("deleteByGivenDate %1").arg(tsa.msg()).toLatin1().data());
    result = tsa.deleteByDateWrongType();
    QVERIFY2(result, QString("deleteByDateWrongType %1").arg(tsa.msg()).toLatin1().data());
    //-----------------
    result = tsa.deleteByDate1MonthFiles();
    QVERIFY2(result, "deleteByDate1MonthFiles");
    result = tsa.deleteByDate6MonthsFiles();
    QVERIFY2(result, "deleteByDate6MonthsFiles");
    result = tsa.deleteByGivenDateFiles();
    QVERIFY2(result, "deleteByGivenDateFiles");
}

void TestQXmlEdit::testEditing()
{
    bool result ;

    {
        TestEditElements test;
        result = test.testFast();
        QVERIFY2(result, (QString("test TestEditElements: testFast() '%1'").arg(test.errorString())).toLatin1().data());
    }
    {
        TestEditElements test;
        result = test.testInsertElement();
        QVERIFY2(result, (QString("test TestEditElements: testInsertElement() '%1'").arg(test.errorString())).toLatin1().data());
    }
    TestEditElements tee;
    result = tee.test();
    QVERIFY2(result, "TestEditElements test");
}


void TestQXmlEdit::testVis()
{
    TestVis tv;
    bool result ;

    result = tv.test();
    QVERIFY2(result, "TestVis test");
}

void TestQXmlEdit::testUndoRedo()
{
    bool result ;
    TestUndoRedo tu;
    result = tu.test();
    QVERIFY2(result, (QString("undo-redo %1").arg(tu.cause)).toLatin1());
}

void TestQXmlEdit::testDocType()
{
    bool result ;
    {
        TestDocType testDT;
        result = testDT.testFast();
        QVERIFY2(result, QString("document type fast: %1").arg(testDT.errorString()).toLatin1().data());
    }
    {
        TestDocType testDT;
        result = testDT.testUnit();
        QVERIFY2(result, QString("document type testUnit: %1").arg(testDT.errorString()).toLatin1().data());
    }
    {
        TestDocType testDT;
        result = testDT.test();
        QVERIFY2(result, QString("document type tests: %1").arg(testDT.errorString()).toLatin1().data());
    }
    {
        TestDocType testDT;
        result = testDT.testEdit();
        QVERIFY2(result, QString("document type testEdit: %1").arg(testDT.errorString()).toLatin1().data());
    }
}


void TestQXmlEdit::testXSDDiff2()
{
    bool result ;
    TestXSDDiff testXsdEqual;
    result = testXsdEqual.testEqual();
    QVERIFY2(result, QString("xsd diff: %1").arg(testXsdEqual.errorString()).toLatin1().data());
}

void TestQXmlEdit::testXSDDiff3()
{
    bool result ;
    TestXSDDiff testXsdAdd;
    result = testXsdAdd.testAdd();
    QVERIFY2(result, QString("xsd diff: %1").arg(testXsdAdd.errorString()).toLatin1().data());
}

void TestQXmlEdit::testXSDDiff4()
{
    bool result ;
    TestXSDDiff testXsdDel;
    result = testXsdDel.testDel();
    QVERIFY2(result, QString("xsd diff: %1").arg(testXsdDel.errorString()).toLatin1().data());
}

void TestQXmlEdit::testXSDDiff5()
{
    bool result ;
    TestXSDDiff testXsdMixed;
    result = testXsdMixed.testMixed();
    QVERIFY2(result, QString("xsd diff: %1").arg(testXsdMixed.errorString()).toLatin1().data());
}

//-
void TestQXmlEdit::testXSDDiff6()
{
    bool result ;
    TestXSDDiff testXsdEqualUnordered;
    result = testXsdEqualUnordered.testEqualUnordered();
    QVERIFY2(result, QString("xsd diff: %1").arg(testXsdEqualUnordered.errorString()).toLatin1().data());
}

void TestQXmlEdit::testXSDDiff7()
{
    bool result ;
    TestXSDDiff testXsdAddUnordered;
    result = testXsdAddUnordered.testAddUnordered();
    QVERIFY2(result, QString("xsd diff: %1").arg(testXsdAddUnordered.errorString()).toLatin1().data());
}

void TestQXmlEdit::testXSDDiff8()
{
    bool result ;
    TestXSDDiff testXsdDelUnordered;
    result = testXsdDelUnordered.testDelUnordered();
    QVERIFY2(result, QString("xsd diff: %1").arg(testXsdDelUnordered.errorString()).toLatin1().data());
}

void TestQXmlEdit::testXSDDiff9()
{
    bool result ;
    TestXSDDiff testXsdMixedUnordered;
    result = testXsdMixedUnordered.testMixedUnordered();
    QVERIFY2(result, QString("xsd diff: %1").arg(testXsdMixedUnordered.errorString()).toLatin1().data());
}

void TestQXmlEdit::testXSDDiff10()
{
    bool result ;
    TestXSDDiff test;
    result = test.testCompareBase();
    QVERIFY2(result, QString("xsd diff: %1").arg(test.errorString()).toLatin1().data());
}

void TestQXmlEdit::testXSDDiff11()
{
    bool result ;
    TestXSDDiff test;
    result = test.testDiffOpAttributes();
    QVERIFY2(result, QString("xsd diff: %1").arg(test.errorString()).toLatin1().data());
}

void TestQXmlEdit::testXSDDiff12()
{
    bool result ;
    TestXSDDiff test;
    result = test.testDiffObjects();
    QVERIFY2(result, QString("xsd diff: %1").arg(test.errorString()).toLatin1().data());
}

void TestQXmlEdit::testExportCSV()
{
    bool result ;
    ExportCSVTest test1;
    result = test1.testUnitTest();
    QVERIFY2(result, (QString("csv export unit test :%1").arg(test1.errorString())).toLatin1().data());

    ExportCSVTest test2;
    result = test2.testExportSiblingsAttributes();
    QVERIFY2(result, (QString("csv export functional test :%1").arg(test2.errorString())).toLatin1().data());

}

void TestQXmlEdit::testFilterAttributes()
{
    bool result ;
    TestFilterAttributes test1;
    result = test1.testUnit();
    QVERIFY2(result, QString("filter attributes unit test").toLatin1().data());

    TestFilterAttributes tests;
    result = tests.testStorage();
    QVERIFY2(result, QString("filter attributes storage test").toLatin1().data());

    TestFilterAttributes test2;
    result = test2.testFunctionality();
    QVERIFY2(result, QString("filter attributes functional test").toLatin1().data());

    TestFilterAttributes test3;
    result = test3.testDetailFunctionality();
    QVERIFY2(result, QString("filter attributes detail functional test").toLatin1().data());
}

void TestQXmlEdit::testCompareXml()
{
    bool result ;

    TestCompareXml test1;
    result = test1.testUnit();
    QVERIFY2(result, (QString("test compare xml unit test: '%1'").arg(test1.errorString())).toLatin1().data());

    TestCompareXml test2;
    result = test2.testLoadFile();
    QVERIFY2(result, (QString("test compare xml load file: '%1'").arg(test2.errorString())).toLatin1().data());

    TestCompareXml test3;
    result = test3.testResults();
    QVERIFY2(result, (QString("test compare xml result test: '%1'").arg(test3.errorString())).toLatin1().data());
}


void TestQXmlEdit::testSpringAndForces()
{
    bool result ;

    TestSpringAndForces test1;
    result = test1.testUnitForces();
    QVERIFY2(result, (QString("test testUnitForces: '%1'").arg(test1.errorString())).toLatin1().data());

}

void TestQXmlEdit::testSearch()
{
    bool result ;

    TestSearch test1;
    result = test1.testLiteralSearch();
    QVERIFY2(result, (QString("test testLiteralSearch: '%1'").arg(test1.errorString())).toLatin1().data());

    TestSearch test2;
    result = test2.testXQuerySearch();
    QVERIFY2(result, (QString("test testXQuerySearch: '%1'").arg(test2.errorString())).toLatin1().data());
}


void TestQXmlEdit::testReplace()
{
    bool result ;
    {
        TestReplace test1;
        result = test1.testFast();
        QVERIFY2(result, (QString("test testFast: '%1'").arg(test1.errorString())).toLatin1().data());
    }

    {
        TestReplace test1;
        result = test1.testUnitReplace();
        QVERIFY2(result, (QString("test testUnitReplace: '%1'").arg(test1.errorString())).toLatin1().data());
    }
    {
        TestReplace test1;
        result = test1.testBaseReplace();
        QVERIFY2(result, (QString("test testBaseReplace: '%1'").arg(test1.errorString())).toLatin1().data());
    }
}

void TestQXmlEdit::testCopyAttrs()
{
    bool result ;

    TestCopyAttrs test1;
    result = test1.testCopyAttrs();
    QVERIFY2(result, (QString("test TestCopyAttrs: '%1'").arg(test1.errorString())).toLatin1().data());
    TestCopyAttrs test2;
    result = test2.testCopyAttrsExcl();
    QVERIFY2(result, (QString("test TestCopyAttrs Excl: '%1'").arg(test2.errorString())).toLatin1().data());
}

void TestQXmlEdit::testXSDNavigation()
{
    bool result ;

    TestXSDNavigation test1;
    result = test1.testNavigation();
    QVERIFY2(result, (QString("test TestXSDNavigation: '%1'").arg(test1.errorString())).toLatin1().data());
}

void TestQXmlEdit::testClipboard()
{
    bool result ;

    TestClipboard test1;
    result = test1.testCopyElementPathToClipboard();
    QVERIFY2(result, (QString("test CopyElementPathToClipboard: '%1'").arg(test1.errorString())).toLatin1().data());
    TestClipboard test2;
    result = test2.testPasteAsSibling();
    QVERIFY2(result, (QString("test testPasteAsSibling: '%1'").arg(test2.errorString())).toLatin1().data());
    TestClipboard test3;
    result = test3.testCopyAllToClipboard();
    QVERIFY2(result, (QString("test testCopyAllToClipboard: '%1'").arg(test3.errorString())).toLatin1().data());
}


void TestQXmlEdit::testNewFromTemplate()
{
    bool result ;

    {
        TestNewFromTemplate test;
        result = test.testFast();
        QVERIFY2(result, (QString("TestNewFromTemplate testFast: '%1'").arg(test.errorString())).toLatin1().data());
    }
    {
        TestNewFromTemplate test1;
        result = test1.testCreateFromTemplate();
        QVERIFY2(result, (QString("TestNewFromTemplate TestNewFromTemplate: '%1'").arg(test1.errorString())).toLatin1().data());
    }
    {
        TestNewFromTemplate test2;
        result = test2.testCreateFromSnippet();
        QVERIFY2(result, (QString("TestNewFromTemplate TestNewFromSnippet: '%1'").arg(test2.errorString())).toLatin1().data());
    }
}

void TestQXmlEdit::testXSLTMode()
{
    bool result ;

    TestXSLTMode  test1;
    result = test1.testBaseMode();
    QVERIFY2(result, (QString("test TestXsltMode: basic '%1'").arg(test1.errorString())).toLatin1().data());
    TestXSLTMode  test2;
    result = test2.testAdvancedMode();
    QVERIFY2(result, (QString("test TestXsltMode: advanced '%1'").arg(test2.errorString())).toLatin1().data());
    TestXSLTMode  test3;
    result = test3.testNavigation();
    QVERIFY2(result, (QString("test TestXsltMode: navigation '%1'").arg(test3.errorString())).toLatin1().data());
    TestXSLTMode  test4;
    result = test4.testPanel();
    QVERIFY2(result, (QString("test TestXsltMode: panel '%1'").arg(test4.errorString())).toLatin1().data());
    TestXSLTMode  test5;
    result = test5.testInsParameters();
    QVERIFY2(result, (QString("test TestXsltMode: testInsParameters '%1'").arg(test5.errorString())).toLatin1().data());
}

void TestQXmlEdit::testStyle()
{
    bool result ;

    TestStyle  test1;
    result = test1.testLoadCalcStyle();
    QVERIFY2(result, (QString("test Style: testLoadCalcStyle '%1'").arg(test1.errorString())).toLatin1().data());
    TestStyle  test2;
    result = test2.testCalcStyle();
    QVERIFY2(result, (QString("test Style: testCalcStyle '%1'").arg(test2.errorString())).toLatin1().data());
}

void TestQXmlEdit::testXSDCopy()
{
    bool result ;

    TestXSDCopy test1;
    result = test1.testCopy();
    QVERIFY2(result, (QString("test TestXSDCopy: testCopy '%1'").arg(test1.errorString())).toLatin1().data());
    TestXSDCopy test2;
    result = test2.testCopyFacets();
    QVERIFY2(result, (QString("test TestXSDCopy: testCopyFacets '%1'").arg(test2.errorString())).toLatin1().data());
}

//-
void TestQXmlEdit::testLoop()
{
    for( int i = 0 ; i < 10 ; i ++ ) {
        TestSQLAccess tsa;
        bool result = tsa.deleteByDate1Month();
        QVERIFY2(result, QString("deleteByDate1Month %1").arg(tsa.msg()).toLatin1().data());
        QTest::qSleep(100);
    }
}


void TestQXmlEdit::testBinaryViewer()
{
    bool result ;

    TestBinaryViewer test1;
    result = test1.unitTests();
    QVERIFY2(result, (QString("test testBinaryViewer: unitTests '%1'").arg(test1.errorString())).toLatin1().data());
    TestBinaryViewer test2;
    result = test2.functionalTests();
    QVERIFY2(result, (QString("test testBinaryViewer: functionalTests '%1'").arg(test2.errorString())).toLatin1().data());
}

void TestQXmlEdit::testSearchlets()
{
    bool result ;

    TestSearchlets test1;
    result = test1.testUnit();
    QVERIFY2(result, (QString("test TestSearchlets: unitTests '%1'").arg(test1.errorString())).toLatin1().data());
    TestSearchlets test2;
    result = test2.testFunctional();
    QVERIFY2(result, (QString("test TestSearchlets: functionalTests '%1'").arg(test2.errorString())).toLatin1().data());
}

void TestQXmlEdit::testReadOnly()
{
    bool result ;

    TestReadOnly test1;
    result = test1.testUnit();
    QVERIFY2(result, (QString("test TestReadOnly: unitTests '%1'").arg(test1.errorString())).toLatin1().data());
}

void TestQXmlEdit::testMetadata()
{
    bool result ;

    TestMetadata test;
    result = test.testSuiteParsing();
    QVERIFY2(result, (QString("test TestMetadata: testSuiteParsing '%1'").arg(test.errorString())).toLatin1().data());
    TestMetadata test1;
    result = test1.testManageMeta();
    QVERIFY2(result, (QString("test TestMetadata: testManageMeta'%1'").arg(test1.errorString())).toLatin1().data());
    TestMetadata test2;
    result = test2.testModifyMeta();
    QVERIFY2(result, (QString("test TestMetadata: testModifyMeta '%1'").arg(test2.errorString())).toLatin1().data());
}

void TestQXmlEdit::testExport()
{
    bool result ;

    {
        TestExport test;
        result = test.testFast();
        QVERIFY2(result, (QString("test testFast: '%1'").arg(test.errorString())).toLatin1().data());
    }
    {
        TestExport test;
        result = test.testExportJava();
        QVERIFY2(result, (QString("test TestExportJava: testSuiteParsing '%1'").arg(test.errorString())).toLatin1().data());
    }
    {
        TestExport testC;
        result = testC.testExportC();
        QVERIFY2(result, (QString("test TestExportC: testSuiteParsing '%1'").arg(testC.errorString())).toLatin1().data());
    }
    {
        TestExport testC;
        result = testC.testExportSelected();
        QVERIFY2(result, (QString("test testExportSelected: '%1'").arg(testC.errorString())).toLatin1().data());
    }}

void TestQXmlEdit::testEncoding()
{
    bool result ;

    TestEncoding test1;
    result = test1.testUnitTests();
    QVERIFY2(result, (QString("test TestEncoding: testUnitTests '%1'").arg(test1.errorString())).toLatin1().data());
    TestEncoding test2;
    result = test2.testFunctionalTests();
    QVERIFY2(result, (QString("test TestEncoding: testFunctionalTests '%1'").arg(test2.errorString())).toLatin1().data());
}

void TestQXmlEdit::testProlog()
{
    bool result ;

    TestProlog test1;
    result = test1.testUnitTests();
    QVERIFY2(result, (QString("test TestProlog: testUnitTests '%1'").arg(test1.errorString())).toLatin1().data());
    TestProlog test2;
    result = test2.testFunctionalTests();
    QVERIFY2(result, (QString("test TestProlog: testFunctionalTests '%1'").arg(test2.errorString())).toLatin1().data());
}

void TestQXmlEdit::testApp()
{
    bool result ;

    TestApp test1;
    result = test1.testBaseTests();
    QVERIFY2(result, (QString("test TestApp: testBaseTests '%1'").arg(test1.errorString())).toLatin1().data());
}

void TestQXmlEdit::testLengthAttributes()
{
    bool result ;

    TestLengthAttributes test1;
    result = test1.test();
    QVERIFY2(result, (QString("test TestLengthAttributes: test '%1'").arg(test1.errorString())).toLatin1().data());
}


void TestQXmlEdit::testColors()
{
    bool result ;

    TestColor test1;
    result = test1.testExport();
    QVERIFY2(result, (QString("test TestColor: testExport '%1'").arg(test1.errorString())).toLatin1().data());
    TestColor test2;
    result = test2.testImport();
    QVERIFY2(result, (QString("test TestColor: testImport '%1'").arg(test1.errorString())).toLatin1().data());
}

void TestQXmlEdit::testIndent()
{
    bool result ;
    {
        TestIndent test1;
        result = test1.testFast();
        QVERIFY2(result, (QString("test TestIndent: testFast '%1'").arg(test1.errorString())).toLatin1().data());
    }
    {
        TestIndent test1;
        result = test1.testSaving();
        QVERIFY2(result, (QString("test TestIndent: testSaving '%1'").arg(test1.errorString())).toLatin1().data());
    }
    {
        TestIndent test1;
        result = test1.testSettings();
        QVERIFY2(result, (QString("test TestIndent: testSettings '%1'").arg(test1.errorString())).toLatin1().data());
    }
    {
        TestIndent test1;
        result = test1.testIndentAttributes();
        QVERIFY2(result, (QString("test TestIndent: testIndentAttributes '%1'").arg(test1.errorString())).toLatin1().data());
    }
}

void TestQXmlEdit::testContainer()
{
    bool result ;

    TestContainer test0;
    result = test0.testFast();
    QVERIFY2(result, (QString("test TestContainer: testFast() '%1'").arg(test0.errorString())).toLatin1().data());
    TestContainer test1;
    result = test1.testInsertChildContainer();
    QVERIFY2(result, (QString("test TestContainer: testInsertChildContainer() '%1'").arg(test1.errorString())).toLatin1().data());
    TestContainer test2;
    result = test2.testInsertParent();
    QVERIFY2(result, (QString("test TestContainer: testInsertParent() '%1'").arg(test2.errorString())).toLatin1().data());
    TestContainer test3;
    result = test3.testRemoveParent();
    QVERIFY2(result, (QString("test TestContainer: testRemoveParent() '%1'").arg(test3.errorString())).toLatin1().data());
    TestContainer test4;
    result = test4.testEnableInsParent();
    QVERIFY2(result, (QString("test TestContainer: testEnableInsParent() '%1'").arg(test4.errorString())).toLatin1().data());
    TestContainer test5;
    result = test5.testEnableRemoveParent();
    QVERIFY2(result, (QString("test TestContainer: testEnableRemoveParent() '%1'").arg(test5.errorString())).toLatin1().data());
}

void TestQXmlEdit::testXsdView()
{
    bool result ;

    TestXSDView test0;
    result = test0.testBase();
    QVERIFY2(result, (QString("test TestXSDView: testBase() '%1'").arg(test0.errorString())).toLatin1().data());

    TestXSDView test1;
    result = test1.testElement();
    QVERIFY2(result, (QString("test TestXSDView: testElement() '%1'").arg(test1.errorString())).toLatin1().data());

    TestXSDView test2;
    result = test2.testType();
    QVERIFY2(result, (QString("test TestXSDView: testType() '%1'").arg(test2.errorString())).toLatin1().data());

    TestXSDView test3;
    result = test3.testReference();
    QVERIFY2(result, (QString("test TestXSDView: testReference() '%1'").arg(test3.errorString())).toLatin1().data());

    TestXSDView test4;
    result = test4.testExtension();
    QVERIFY2(result, (QString("test TestXSDView: testExtension() '%1'").arg(test4.errorString())).toLatin1().data());

    TestXSDView test5;
    result = test5.testRestriction();
    QVERIFY2(result, (QString("test TestXSDView: testRestriction() '%1'").arg(test5.errorString())).toLatin1().data());

    /*TestXSDView test6;
    result = test6.testAttributeAndGroups();
    QVERIFY2(result, (QString("test TestXSDView: testAttributeAndGroups() '%1'").arg(test6.errorString())).toLatin1().data());*/
}


void TestQXmlEdit::testXsdLoad()
{
    bool result;
    TestXSDLoad testA;
    result = testA.testFast();
    QVERIFY2(result, (QString("test TestXSDLoad: testFast() '%1'").arg(testA.errorString())).toLatin1().data());

    TestXSDLoad test0;
    result = test0.testUnits();
    QVERIFY2(result, (QString("test TestXSDLoad: testUnits() '%1'").arg(test0.errorString())).toLatin1().data());
    TestXSDLoad test1;
    result = test1.testInclude();
    QVERIFY2(result, (QString("test TestXSDLoad: testInclude() '%1'").arg(test1.errorString())).toLatin1().data());
    TestXSDLoad test2;
    result = test2.testRedefine();
    QVERIFY2(result, (QString("test TestXSDLoad: testRedefine() '%1'").arg(test2.errorString())).toLatin1().data());
    TestXSDLoad test3;
    result = test3.testTypes();
    QVERIFY2(result, (QString("test TestXSDLoad: testTypes() '%1'").arg(test3.errorString())).toLatin1().data());
}

void TestQXmlEdit::testXsdValidation()
{
    bool result;
    {
        TestXSDValidation testLast;
        result = testLast.testLast();
        QVERIFY2(result, (QString("test TestXSDValidation: testLast() '%1'").arg(testLast.errorString())).toLatin1().data());
    }
    {
        TestXSDValidation testA;
        result = testA.testAttributes();
        QVERIFY2(result, (QString("test TestXSDValidation: testAttributes() '%1'").arg(testA.errorString())).toLatin1().data());
    }
    {
        TestXSDValidation testB;
        result = testB.testElements();
        QVERIFY2(result, (QString("test TestXSDValidation: testElements() '%1'").arg(testB.errorString())).toLatin1().data());
    }
    {
        TestXSDValidation testC;
        result = testC.testTypes();
        QVERIFY2(result, (QString("test TestXSDValidation: testTypes() '%1'").arg(testC.errorString())).toLatin1().data());
    }
    {
        TestXSDValidation testD;
        result = testD.testValidateSchema();
        QVERIFY2(result, (QString("test TestXSDValidation: testValidateSchema() '%1'").arg(testD.errorString())).toLatin1().data());
    }
}

void TestQXmlEdit::testEncodingUtils()
{
    bool result;
    TestEncodingUtils testA;
    result = testA.testBase();
    QVERIFY2(result, (QString("test TestEncodingUtils: testBase() '%1'").arg(testA.errorString())).toLatin1().data());
}


void TestQXmlEdit::testSelectXsd()
{
    bool result;
    TestSelectXsd testA;
    result = testA.testForward();
    QVERIFY2(result, (QString("test TestSelectXsd: testForward() '%1'").arg(testA.errorString())).toLatin1().data());
    TestSelectXsd testB;
    result = testB.testBack();
    QVERIFY2(result, (QString("test TestSelectXsd: testBack() '%1'").arg(testB.errorString())).toLatin1().data());
}


void TestQXmlEdit::testNillable()
{
    bool result;
    {
        TestNillable testA;
        result = testA.testFast();
        QVERIFY2(result, (QString("test TestNillable: testFast() '%1'").arg(testA.errorString())).toLatin1().data());
    }

    {
        TestNillable testA;
        result = testA.testUnit();
        QVERIFY2(result, (QString("test TestNillable: testUnit() '%1'").arg(testA.errorString())).toLatin1().data());
    }
}

void TestQXmlEdit::testXSIType()
{
    bool result;
    {
        TestXSIType testA;
        result = testA.testFast();
        QVERIFY2(result, (QString("test TestXSIType: testFast() '%1'").arg(testA.errorString())).toLatin1().data());
    }

    {
        TestXSIType testA;
        result = testA.testUnit();
        QVERIFY2(result, (QString("test TestXSIType: testUnit() '%1'").arg(testA.errorString())).toLatin1().data());
    }
}

void TestQXmlEdit::testNamespace()
{
    bool result;
    {
        TestUserNamespaces testA;
        result = testA.testFast();
        QVERIFY2(result, (QString("test TestUserNamespaces: testFast() '%1'").arg(testA.errorString())).toLatin1().data());
    }

    {
        TestUserNamespaces testA;
        result = testA.testSerialization();
        QVERIFY2(result, (QString("test TestUserNamespaces: testSerialization() '%1'").arg(testA.errorString())).toLatin1().data());
    }
    {
        TestUserNamespaces testA;
        result = testA.testVisibility();
        QVERIFY2(result, (QString("test TestUserNamespaces: testVisibility() '%1'").arg(testA.errorString())).toLatin1().data());
    }
}

void TestQXmlEdit::testInsertXsdReference()
{
    bool result;
    {
        TestInsertXsdReference testA;
        result = testA.testFast();
        QVERIFY2(result, (QString("test testInsertXsdReference: testFast() '%1'").arg(testA.errorString())).toLatin1().data());
    }

    {
        TestInsertXsdReference testA;
        result = testA.testInsertXsdReference();
        QVERIFY2(result, (QString("test testInsertXsdReference: testInsertXsdReference() '%1'").arg(testA.errorString())).toLatin1().data());
    }

    {
        TestInsertXsdReference testA;
        result = testA.testDialogInsert();
        QVERIFY2(result, (QString("test testInsertXsdReference: testDialogInsert() '%1'").arg(testA.errorString())).toLatin1().data());
    }
}

void TestQXmlEdit::testXSDMode()
{
    bool result;

    {
        TestXSDMode test0;
        result = test0.testLast();
        QVERIFY2(result, (QString("test TestXSDMode: testLast() '%1'").arg(test0.errorString())).toLatin1().data());
    }

    {
        TestXSDMode testA;
        result = testA.testType();
        QVERIFY2(result, (QString("test TestXSDMode: testType() '%1'").arg(testA.errorString())).toLatin1().data());
    }

    {
        TestXSDMode testB;
        result = testB.testElement();
        QVERIFY2(result, (QString("test TestXSDMode: testElement() '%1'").arg(testB.errorString())).toLatin1().data());
    }

    {
        TestXSDMode testC;
        result = testC.testAttribute();
        QVERIFY2(result, (QString("test TestXSDMode: testAttribute() '%1'").arg(testC.errorString())).toLatin1().data());
    }

    {
        TestXSDMode testD;
        result = testD.testEnabling();
        QVERIFY2(result, (QString("test TestXSDMode: testEnabling() '%1'").arg(testD.errorString())).toLatin1().data());
    }

    {
        TestXSDMode testE;
        result = testE.testDialogTypes();
        QVERIFY2(result, (QString("test TestXSDMode: testDialogTypes() '%1'").arg(testE.errorString())).toLatin1().data());
    }
    {
        TestXSDMode testF;
        result = testF.testEditAnnotations();
        QVERIFY2(result, (QString("test TestXSDMode: testAnnotations() '%1'").arg(testF.errorString())).toLatin1().data());
    }
}

void TestQXmlEdit::testAnonymize()
{
    bool result;
    {
        TestAnonymize test;
        result = test.testFast();
        QVERIFY2(result, (QString("test testAnonymize: testFast() '%1'").arg(test.errorString())).toLatin1().data());
    }
    {
        TestAnonymize test0;
        result = test0.testBase();
        QVERIFY2(result, (QString("test testAnonymize: testBase() '%1'").arg(test0.errorString())).toLatin1().data());
    }
    {
        TestAnonymize test0;
        result = test0.testUnit();
        QVERIFY2(result, (QString("test testAnonymize: testUnit() '%1'").arg(test0.errorString())).toLatin1().data());
    }
    {
        TestAnonymize test1;
        result = test1.testExc();
        QVERIFY2(result, (QString("test testAnonymize: testExc() '%1'").arg(test1.errorString())).toLatin1().data());
    }
    {
        TestAnonymize test2;
        result = test2.testBatch();
        QVERIFY2(result, (QString("test testAnonymize: testBatch() '%1'").arg(test2.errorString())).toLatin1().data());
    }
}


void TestQXmlEdit::testXmlUtils()
{
    bool result;
    {
        TestXmlUtils test;
        result = test.testUnits();
        QVERIFY2(result, (QString("test testXmlUtils: testUnits() '%1'").arg(test.errorString())).toLatin1().data());
    }

}

void TestQXmlEdit::testSortAttributes()
{
    bool result ;

    {
        TestSortAttributes test1;
        result = test1.testFast();
        QVERIFY2(result, (QString("test TestSortAttributes: testFast '%1'").arg(test1.errorString())).toLatin1().data());
    }
    {
        TestSortAttributes test1;
        result = test1.testSaving();
        QVERIFY2(result, (QString("test TestSortAttributes: testSaving '%1'").arg(test1.errorString())).toLatin1().data());
    }
}


void TestQXmlEdit::testOpenInNewWindow()
{
    bool result ;

    {
        TestOpenInNewWindow test1;
        result = test1.testFast();
        QVERIFY2(result, (QString("test TestOpenInNewWindow: testFast '%1'").arg(test1.errorString())).toLatin1().data());
    }
    {
        TestOpenInNewWindow test1;
        result = test1.testUnit();
        QVERIFY2(result, (QString("test TestOpenInNewWindow: testUnit '%1'").arg(test1.errorString())).toLatin1().data());
    }
}

void TestQXmlEdit::testElement()
{
    bool result ;

    {
        TestElement test1;
        result = test1.testFast();
        QVERIFY2(result, (QString("test TestElement: testFast '%1'").arg(test1.errorString())).toLatin1().data());
    }
    {
        TestElement test1;
        result = test1.testUnit();
        QVERIFY2(result, (QString("test TestElement: testUnit '%1'").arg(test1.errorString())).toLatin1().data());
    }
}

void TestQXmlEdit::testLoadFile()
{
    bool result ;

    {
        TestLoadFile test1;
        result = test1.testFast();
        QVERIFY2(result, (QString("test TestLoadFile: testFast '%1'").arg(test1.errorString())).toLatin1().data());
    }
    {
        TestLoadFile test1;
        result = test1.testUnit();
        QVERIFY2(result, (QString("test TestLoadFile: testUnit '%1'").arg(test1.errorString())).toLatin1().data());
    }
}

void TestQXmlEdit::testReplica()
{
    bool result ;

    {
        TestReplica test1;
        result = test1.testFast();
        QVERIFY2(result, (QString("test TestReplica: testFast '%1'").arg(test1.errorString())).toLatin1().data());
    }
    {
        TestReplica test1;
        result = test1.testUnit();
        QVERIFY2(result, (QString("test TestReplica: testUnit '%1'").arg(test1.errorString())).toLatin1().data());
    }
    {
        TestReplica test1;
        result = test1.testAction();
        QVERIFY2(result, (QString("test TestReplica: testAction '%1'").arg(test1.errorString())).toLatin1().data());
    }
    {
        TestReplica test1;
        result = test1.testClone();
        QVERIFY2(result, (QString("test TestReplica: testClone '%1'").arg(test1.errorString())).toLatin1().data());
    }}

void TestQXmlEdit::testUtils()
{
    bool result ;

    {
        TestUtils test1;
        result = test1.testUnit();
        QVERIFY2(result, (QString("test TestUtils: testUnit '%1'").arg(test1.errorString())).toLatin1().data());
    }
}

/*

// This function enabled for debug purposes. DO NOT REMOVE
static void msgHandler(QtMsgType type, const char *msg)
static void msgHandler(QtMsgType, const QMessageLogContext &, const QString &msg)
{
    QString s = msg;
    if( !s.startsWith("Application asked to ")){
        int k = 0;
        k++;
    }
    //printf("%s\n", msg);
    printf("%s\n", msg.toLatin1().data());
}
*/
void TestQXmlEdit::testNew()
{
    //qInstallMsgHandler(msgHandler);
    //qInstallMessageHandler(msgHandler);
    testExport();
    testIndent();
    /*testAnonymize();
    testReplica();
    testSortAttributes();*/
}

QTEST_MAIN(TestQXmlEdit)

//#include "tst_qxmledit.moc"
