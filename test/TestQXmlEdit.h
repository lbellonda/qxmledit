/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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


#ifndef TESTQXMLEDIT_H
#define TESTQXMLEDIT_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QtCore/QMap>
#include <QTest>

#include "searchinfiles.h"
#include "utils.h"
#include "comparexml.h"
#include "testpaste.h"
#include "testcomment.h"
#include "testsplit.h"
#include "testxsd.h"
#include "testmixedcontent.h"
#include "testbase64.h"
#include "testeditingcommands.h"
#include "testvalidation.h"
#include "testuifile.h"
#include "testsessions.h"
#include "testsqlaccess.h"
#include "testeditelements.h"
#include "testvis.h"
#include "testundoredo.h"
#include "testdoctype.h"
#include "testxsddiff.h"
#include "exportcsvtest.h"
#include "testfilterattributes.h"
#include "testcomparexml.h"
#include "testspringandforces.h"
#include "testsearch.h"
#include "testcopyattrs.h"
#include "testxsdnavigation.h"
#include "testnewfromtemplate.h"
#include "testclipboard.h"
#include "testxsltmode.h"
#include "teststyle.h"
#include "testxsdcopy.h"
#include "testbinaryviewer.h"
#include "testsearchlets.h"
#include "testreadonly.h"
#include "testmetadata.h"
#include "testexport.h"
#include "testprolog.h"
#include "testencoding.h"
#include "testapp.h"
#include "testlengthattributes.h"
#include "testcolor.h"
#include "testindent.h"
#include "testcontainer.h"
#include "testxsdview.h"
#include "testxsdload.h"
#include "testxsdvalidation.h"
#include "testencodingutils.h"
#include "testselectxsd.h"
#include "testxsdmode.h"
#include "testanonymize.h"
#include "testreplace.h"
#include "testnillable.h"
#include "testxsitype.h"
#include "testusernamespaces.h"
#include "testxmlutils.h"
#include "testinsertxsdreference.h"
#include "testsortattributes.h"
#include "testopeninnewwindow.h"
#include "testelement.h"
#include "testloadfile.h"
#include "testreplica.h"
#include "testutils.h"
#include "testconfig.h"
#include "testdeletesiblings.h"
#include "testcommandline.h"
#include "testeditenum.h"
#include "testnamespaces.h"
#include "testspecials.h"
#include "testscxml.h"
#include "testexecxslt.h"
#include "testspecialview.h"
#include "testformattinginfo.h"
#include "teststartupandbehavior.h"
#include "testhelp.h"
#include "testtestxmlfile.h"
#include "testloadsample.h"
#include "testxmlbeans.h"
#include "testabout.h"

class TestQXmlEdit : public QObject
{
    Q_OBJECT
public:
    TestQXmlEdit();
    bool verifyMaps(QMap<int, int> &reference, QMap<int, int> &candidate);

// for valgrind
//private Q_SLOTS:
    void vt1();
    void vt2();
    void vt3();
    void vt4();
    void vt5();
    void vt6();
    void vt7();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testNew();
    void testLoop();
    //
    void testApp();
    void testSpringAndForces();
    void testSearch();
    void testCompareXml();
    void testExportCSV();
    void testXSDDiff12();
    void testXSDDiff2();
    void testXSDDiff3();
    void testXSDDiff4();
    void testXSDDiff5();
    void testXSDDiff6();
    void testXSDDiff7();
    void testXSDDiff8();
    void testXSDDiff9();
    void testXSDDiff10();
    void testXSDDiff11();

    void testDocType();
    void testBase64();
    void testUndoRedo();
    void testVis();
    void testEditing();
    void testSql();
    void testSessions();
    void testFileUI();
    void testValidation();
    void testSplit();
    void testEditingCommands();
    void test2();
    void test();
    void test1();
    void testComment();
    void testXsd();
    void test_data();
    void testPaste();
    void testMixed();
    void testMixed2();
    void testMixedContent();
    void testFilterAttributes();
    void testCopyAttrs();
    void testXSDNavigation();
    void testClipboard();
    void testNewFromTemplate();
    void testXSLTMode();
    void testStyle();
    void testXSDCopy();
    void testBinaryViewer();
    void testSearchlets();
    void testReadOnly();
    void testMetadata();
    void testExport();
    void testProlog();
    void testEncoding();
    void testLengthAttributes();
    void testColors();
    void testIndent();
    void testContainer();
    void testXsdView();
    void testXsdLoad();
    void testXsdValidation();
    void testEncodingUtils();
    void testSelectXsd();
    void testXSDMode();
    void testAnonymize();
    void testReplace();
    void testNillable();
    void testXSIType();
    void testNamespace();
    void testXmlUtils();
    void testInsertXsdReference();
    void testSortAttributes();
    void testOpenInNewWindow();
    void testElement();
    void testLoadFile();
    void testReplica();
    void testUtils();
    void testConfig();
    void testDeleteSiblings();
    void testCommandLine();
    void testEditEnum();
    void testNamespaces();
    void testSpecials();
    void testSCXML();
    void testExecXSLT();
    void testSpecialView();
    void testFormattingInfo();
    void testStartupAndBehavior();
    void testHelp();
    void testTestXMLFile();
    void testLoadSample();
    void testXMLBeans();
    void testAbout();
};


#if QT_VERSION < 0x050000
    #define QXMLEDIT_SKIP(msg) QSKIP(msg)
#else
    #define QXMLEDIT_SKIP(msg) QSKIP(msg, SkipSingle)
#endif

#if QT_VERSION < 0x050000
    #define QXMLEDIT_SKIP_V(result, msg) QSKIP(msg)
#else
    #define QXMLEDIT_SKIP_V(result, msg) QSKIP(msg, SkipSingle)
#endif

#endif // TESTQXMLEDIT_H
