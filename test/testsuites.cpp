/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2018 by Luca Bellonda and individual contributors       *
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
#include "utils.h"

void TestQXmlEdit::vt1(){
    testApp();
    testSpringAndForces();
    testSearch();
    testCompareXml();
    testExportCSV();
    testXSDDiff12();
    testXSDDiff2();
    testXSDDiff3();
    testXSDDiff4();
    testXSDDiff5();
    testXSDDiff6();
    testXSDDiff7();
    testXSDDiff8();
    testXSDDiff9();
    testXSDDiff10();
    testXSDDiff11();
}

void TestQXmlEdit::vt2(){
    testDocType();
    testBase64();
    testUndoRedo();
    testVis();
    testEditing();
    testSql();
    testSessions();
    testFileUI();
    testValidation();
    testSplit();
    testEditingCommands();
    test2();
    test();
    test1();
}

void TestQXmlEdit::vt3(){
    testComment();
    testXsd();
    test_data();
    testPaste();
    testMixed();
    testMixed2();
    testMixedContent();
    testFilterAttributes();
    testCopyAttrs();
    testXSDNavigation();
    testClipboard();
    testNewFromTemplate();
    testXSLTMode();
    testStyle();
}

void TestQXmlEdit::vt4(){
    testXSDCopy();
    testBinaryViewer();
    testSearchlets();
    testReadOnly();
    testMetadata();
    testExport();
    testProlog();
    testEncoding();
    testLengthAttributes();
    testColors();
    testIndent();
    testContainer();
}

void TestQXmlEdit::vt5(){
    testXsdView();
    testXsdLoad();
    testXsdValidation();
    testEncodingUtils();
    testSelectXsd();
    testXSDMode();
    testAnonymize();
    testReplace();
    testNillable();
    testXSIType();
    testNamespace();
    testXmlUtils();
}

void TestQXmlEdit::vt6(){
    testInsertXsdReference();
    testSortAttributes();
    testOpenInNewWindow();
    testElement();
    testLoadFile();
    testReplica();
    testUtils();
    testConfig();
    testDeleteSiblings();
    testCommandLine();
    testEditEnum();
}

void TestQXmlEdit::vt7(){
    testNamespaces();
    testSpecials();
    testSCXML();
    testExecXSLT();
    testSpecialView();
    testFormattingInfo();
    testStartupAndBehavior();
    testHelp();
}
