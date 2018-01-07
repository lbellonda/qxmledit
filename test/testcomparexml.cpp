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


#include "testcomparexml.h"
#include "helpers/testcomparexmlunithelper.h"
#include "modules/compare/comparesidebysideview.h"
#include "utils.h"


#define REFERENCE_FILE  "../test/data/compare/reference1.xml"
#define COMPARE_FILE  "../test/data/compare/compare1.xml"


//--------------------------


TestCompareXml::TestCompareXml()
{
}

TestCompareXml::~TestCompareXml()
{
}

bool TestCompareXml::testUnit()
{
    QString testName = "testUnit" ;
    if( !testUnitBase() ) {
        return false;
    }
    return true;
}

bool TestCompareXml::ckeckItem(QTreeWidgetItem *item, const QString &text, const QColor expextedColor)
{
    if(NULL==item){
        return False();
    }
    QString itemText = item->text(0) ;
    if(itemText != text){
        return False();
    }
    QColor bkColor = item->backgroundColor(0);
    if( expextedColor != bkColor ) {
        return False();
    }
    return true ;
}

bool TestCompareXml::testUnitBase(const EDiff::KDiff diffType, Element::ElType type, const QString &texta1, const QString &texta2, const QString &textb1, const QString &textb2, const QString &expectedTextL, const QString &expectedTextR, const bool isCData, const QColor colorLeft, const QColor colorRight )
{
    TestCompareXmlUnitHelper helper;
    helper.startTest(diffType, type, texta1, texta2, textb1, textb2, isCData );

    if(!ckeckItem(helper.leftItemD, expectedTextL, colorLeft)){
        return False();
    }

    if(!ckeckItem(helper.rightItemD, expectedTextR, colorRight )){
        return False();
    }
    return true ;
}


bool TestCompareXml::testEqualsBase(const EDiff::KDiff diffType, Element::ElType type, const QString &texta1, const QString &texta2, const QString &textb1, const QString &textb2, const QString &expectedTextL, const QString &expectedTextR, const bool isCData )
{
    CompareChrome chrome;
    return testUnitBase(diffType, type, texta1, texta2, textb1, textb2, expectedTextL, expectedTextR,
                        isCData, chrome.equalsColor(), chrome.equalsColor() );
}

bool TestCompareXml::testAddedBase(const EDiff::KDiff diffType, Element::ElType type, const QString &texta1, const QString &texta2, const QString &textb1, const QString &textb2, const QString &expectedTextL, const QString &expectedTextR, const bool isCData )
{
    CompareChrome chrome;
    return testUnitBase(diffType, type, texta1, texta2, textb1, textb2, expectedTextL, expectedTextR,
                        isCData, chrome.addedColor(), chrome.emptyColor() );
}

bool TestCompareXml::testDeletedBase(const EDiff::KDiff diffType, Element::ElType type, const QString &texta1, const QString &texta2, const QString &textb1, const QString &textb2, const QString &expectedTextL, const QString &expectedTextR, const bool isCData )
{
    CompareChrome chrome;
    return testUnitBase(diffType, type, texta1, texta2, textb1, textb2, expectedTextL, expectedTextR,
                        isCData, chrome.emptyColor(), chrome.deletedColor() );
}

bool TestCompareXml::testModifyBase(const EDiff::KDiff diffType, Element::ElType type, const QString &texta1, const QString &texta2, const QString &textb1, const QString &textb2, const QString &expectedTextL, const QString &expectedTextR, const bool isCData )
{
    CompareChrome chrome;
    return testUnitBase(diffType, type, texta1, texta2, textb1, textb2, expectedTextL, expectedTextR,
                        isCData, chrome.modifiedColor(), chrome.modifiedColor() );
}

#define EQUAL_TEXT_EL "test"
#define EQUAL_TEXT_EL_EXP "<test>"

#define EQUAL_TEXT_COMM "01234567890123456789a"
#define EQUAL_TEXT_COMM_EXP "Comment: 01234567890123456789..."

#define EQUAL_TEXT_PI_T "target"
#define EQUAL_TEXT_PI_D "data"
#define EQUAL_TEXT_PI_EXP   "<? target data ?>"

#define EQUAL_TEXT_TEXT "01234567890123456789a"
#define EQUAL_TEXT_TEXT_EXP "Text: 01234567890123456789..."

#define EQUAL_TEXT_CDATA "01234567890123456789a"
#define EQUAL_TEXT_CDATA_EXP "Text CDATA: [[01234567890123456789...]]"

#define DIFF_L_TEXT_EL "<test1>"
#define DIFF_R_TEXT_EL "<test2>"


//-----------------------------------------------------
bool TestCompareXml::testEqualsElements()
{
    return testEqualsBase(EDiff::ED_EQUAL, Element::ET_ELEMENT, EQUAL_TEXT_EL, "", EQUAL_TEXT_EL, "", EQUAL_TEXT_EL_EXP, EQUAL_TEXT_EL_EXP );
}

bool TestCompareXml::testEqualsComment()
{
    return testEqualsBase(EDiff::ED_EQUAL, Element::ET_COMMENT, EQUAL_TEXT_COMM, "", EQUAL_TEXT_COMM, "", EQUAL_TEXT_COMM_EXP, EQUAL_TEXT_COMM_EXP);
}

bool TestCompareXml::testEqualsPI()
{
    return testEqualsBase(EDiff::ED_EQUAL, Element::ET_PROCESSING_INSTRUCTION, EQUAL_TEXT_PI_T, EQUAL_TEXT_PI_D, EQUAL_TEXT_PI_T, EQUAL_TEXT_PI_D, EQUAL_TEXT_PI_EXP, EQUAL_TEXT_PI_EXP);
}

bool TestCompareXml::testEqualsText()
{
    return testEqualsBase(EDiff::ED_EQUAL, Element::ET_TEXT, EQUAL_TEXT_TEXT, "", EQUAL_TEXT_TEXT, "", EQUAL_TEXT_TEXT_EXP, EQUAL_TEXT_TEXT_EXP);
}

bool TestCompareXml::testEqualsTextCData()
{
    return testEqualsBase(EDiff::ED_EQUAL, Element::ET_TEXT, EQUAL_TEXT_CDATA, "", EQUAL_TEXT_CDATA, "", EQUAL_TEXT_CDATA_EXP, EQUAL_TEXT_CDATA_EXP, true);
}

//-----------------------------------------------------
#define ADDED_TEXT_EL1 "test"
#define ADDED_TEXT_EL2 "test2"
#define ADDED_TEXT_EL1_EXP "<test>"
#define ADDED_TEXT_EL2_EXP ""

#define ADDED_TEXT_COMM1 "01234567890123456789a"
#define ADDED_TEXT_COMM2 "01234567890123456789a"
#define ADDED_TEXT_COMM1_EXP "Comment: 01234567890123456789..."
#define ADDED_TEXT_COMM2_EXP ""

#define ADDED_TEXT_PI1_T "target"
#define ADDED_TEXT_PI1_D "data"
#define ADDED_TEXT_PI2_T "target"
#define ADDED_TEXT_PI2_D "data"
#define ADDED_TEXT_PI1_EXP   "<? target data ?>"
#define ADDED_TEXT_PI2_EXP   ""

#define ADDED_TEXT_TEXT1 "01234567890123456789a"
#define ADDED_TEXT_TEXT2 "0123456789a"
#define ADDED_TEXT_TEXT1_EXP "Text: 01234567890123456789..."
#define ADDED_TEXT_TEXT2_EXP ""

#define ADDED_TEXT_CDATA1 "01234567890123456789a"
#define ADDED_TEXT_CDATA2 "01234567890123456789a"
#define ADDED_TEXT_CDATA1_EXP "Text CDATA: [[01234567890123456789...]]"
#define ADDED_TEXT_CDATA2_EXP ""

bool TestCompareXml::testAddedElements()
{
    return testAddedBase(EDiff::ED_ADDED, Element::ET_ELEMENT, ADDED_TEXT_EL1, "", ADDED_TEXT_EL2, "", ADDED_TEXT_EL1_EXP, ADDED_TEXT_EL2_EXP );
}

bool TestCompareXml::testAddedComment()
{
    return testAddedBase(EDiff::ED_ADDED, Element::ET_COMMENT, ADDED_TEXT_COMM1, "", ADDED_TEXT_COMM2, "", ADDED_TEXT_COMM1_EXP, ADDED_TEXT_COMM2_EXP);
}

bool TestCompareXml::testAddedPI()
{
    return testAddedBase(EDiff::ED_ADDED, Element::ET_PROCESSING_INSTRUCTION, ADDED_TEXT_PI1_T, ADDED_TEXT_PI1_D, ADDED_TEXT_PI2_T, ADDED_TEXT_PI2_D, ADDED_TEXT_PI1_EXP, ADDED_TEXT_PI2_EXP);
}

bool TestCompareXml::testAddedText()
{
    return testAddedBase(EDiff::ED_ADDED, Element::ET_TEXT, ADDED_TEXT_TEXT1, "", ADDED_TEXT_TEXT2, "", ADDED_TEXT_TEXT1_EXP, ADDED_TEXT_TEXT2_EXP);
}

bool TestCompareXml::testAddedTextCData()
{
    return testAddedBase(EDiff::ED_ADDED, Element::ET_TEXT, ADDED_TEXT_CDATA1, "", ADDED_TEXT_CDATA2, "", ADDED_TEXT_CDATA1_EXP, ADDED_TEXT_CDATA2_EXP, true);
}

//-----------------------------------------------------

bool TestCompareXml::testDeletedElements()
{
    return testDeletedBase(EDiff::ED_DELETED, Element::ET_ELEMENT, ADDED_TEXT_EL2, "", ADDED_TEXT_EL1, "", ADDED_TEXT_EL2_EXP, ADDED_TEXT_EL1_EXP );
}

bool TestCompareXml::testDeletedComment()
{
    return testDeletedBase(EDiff::ED_DELETED, Element::ET_COMMENT, ADDED_TEXT_COMM2, "", ADDED_TEXT_COMM1, "", ADDED_TEXT_COMM2_EXP, ADDED_TEXT_COMM1_EXP);
}

bool TestCompareXml::testDeletedPI()
{
    return testDeletedBase(EDiff::ED_DELETED, Element::ET_PROCESSING_INSTRUCTION, ADDED_TEXT_PI2_T, ADDED_TEXT_PI2_D, ADDED_TEXT_PI1_T, ADDED_TEXT_PI1_D, ADDED_TEXT_PI2_EXP, ADDED_TEXT_PI1_EXP);
}

bool TestCompareXml::testDeletedText()
{
    return testDeletedBase(EDiff::ED_DELETED, Element::ET_TEXT, ADDED_TEXT_TEXT2, "", ADDED_TEXT_TEXT1, "", ADDED_TEXT_TEXT2_EXP, ADDED_TEXT_TEXT1_EXP);
}

bool TestCompareXml::testDeletedTextCData()
{
    return testDeletedBase(EDiff::ED_DELETED, Element::ET_TEXT, ADDED_TEXT_CDATA2, "", ADDED_TEXT_CDATA1, "", ADDED_TEXT_CDATA2_EXP, ADDED_TEXT_CDATA1_EXP, true);
}

//-----------------------------------------------------

#define MOD_TEXT_EL1 "test1"
#define MOD_TEXT_EL2 "test2"
#define MOD_TEXT_EL1_EXP "<test1>"
#define MOD_TEXT_EL2_EXP "<test2>"

#define MOD_TEXT_COMM1 "1012345678910123456789a"
#define MOD_TEXT_COMM2 "2012345678920123456789a"
#define MOD_TEXT_COMM1_EXP "Comment: 10123456789101234567..."
#define MOD_TEXT_COMM2_EXP "Comment: 20123456789201234567..."

#define MOD_TEXT_PI1_T "target1"
#define MOD_TEXT_PI1_D "data1"
#define MOD_TEXT_PI2_T "target2"
#define MOD_TEXT_PI2_D "data2"
#define MOD_TEXT_PI1_EXP   "<? target1 data1 ?>"
#define MOD_TEXT_PI2_EXP   "<? target2 data2 ?>"

#define MOD_TEXT_TEXT1 "10123456789234567890a"
#define MOD_TEXT_TEXT2 "20123456789234567890a"
#define MOD_TEXT_TEXT1_EXP "Text: 10123456789234567890..."
#define MOD_TEXT_TEXT2_EXP "Text: 20123456789234567890..."

#define MOD_TEXT_CDATA1 "10123456789234567890a"
#define MOD_TEXT_CDATA2 "20123456789234567890a"
#define MOD_TEXT_CDATA1_EXP "Text CDATA: [[10123456789234567890...]]"
#define MOD_TEXT_CDATA2_EXP "Text CDATA: [[20123456789234567890...]]"

bool TestCompareXml::testModifiedElements()
{
    return testModifyBase(EDiff::ED_MODIFIED, Element::ET_ELEMENT, MOD_TEXT_EL1, "", MOD_TEXT_EL2, "", MOD_TEXT_EL1_EXP, MOD_TEXT_EL2_EXP );
}

bool TestCompareXml::testModifiedComment()
{
    return testModifyBase(EDiff::ED_MODIFIED, Element::ET_COMMENT, MOD_TEXT_COMM1, "", MOD_TEXT_COMM2, "", MOD_TEXT_COMM1_EXP, MOD_TEXT_COMM2_EXP);
}

bool TestCompareXml::testModifiedPI()
{
    return testModifyBase(EDiff::ED_MODIFIED, Element::ET_PROCESSING_INSTRUCTION, MOD_TEXT_PI1_T, MOD_TEXT_PI1_D, MOD_TEXT_PI2_T, MOD_TEXT_PI2_D, MOD_TEXT_PI1_EXP, MOD_TEXT_PI2_EXP);
}

bool TestCompareXml::testModifiedText()
{
    return testModifyBase(EDiff::ED_MODIFIED, Element::ET_TEXT, MOD_TEXT_TEXT1, "", MOD_TEXT_TEXT2, "", MOD_TEXT_TEXT1_EXP, MOD_TEXT_TEXT2_EXP);
}

bool TestCompareXml::testModifiedTextCData()
{
    return testModifyBase(EDiff::ED_MODIFIED, Element::ET_TEXT, MOD_TEXT_CDATA1, "", MOD_TEXT_CDATA2, "", MOD_TEXT_CDATA1_EXP, MOD_TEXT_CDATA2_EXP, true);
}

//-----------------------------------------------------

bool TestCompareXml::compareQuick(OperationResult *results, Regola *one, Regola *two, const bool isDenormalizeEOL)
{
    DiffNodesChangeList changeList;
    CompareOptions options;
    options.setDenormalizeEOL(isDenormalizeEOL);
    results->setMessage("Engine started");
    CompareEngine engine;
    engine.doCompare(results, one, two, &changeList, options);
    return changeList.isReferenceEqualToCompare();
}

Regola *TestCompareXml::loadRegola(QByteArray *dataIn)
{
    QBuffer file(dataIn);
    if(file.open(QIODevice::ReadOnly)) {
        QDomDocument document;
        if(document.setContent(&file)) {
            return new Regola(document, "", true);
        }
    }
    return NULL ;
}

#define S1_TEST "<root><a><![CDATA[\n   a cdata section\n   ]]></a></root>"
#define S2_TEST "<root><a><![CDATA[\r\n   a cdata section\r\n   ]]></a></root>"

bool TestCompareXml::testOptionDenormalizeImpl(const bool isDenormalizeEOL, const bool expectedResult)
{
    bool result = false;
    OperationResult results;

    QByteArray array1 = QString(S1_TEST).toLatin1();
    QByteArray array2 = QString(S2_TEST).toLatin1();

    Regola *one = loadRegola(&array1);
    Regola *two= loadRegola(&array2);
    if( (NULL != one) && (NULL != two) ) {
        bool areDifferent = compareQuick( &results, one, two, isDenormalizeEOL);
        if( results.isOk() ) {
            if(areDifferent == expectedResult) {
                result = true ;
            }
        }
    }
    if( NULL != one ) {
        delete one ;
    }
    if( NULL != two ) {
        delete two ;
    }
    return result;
}

bool TestCompareXml::testOptionDenormalize()
{
    if( !testOptionDenormalizeImpl(false, true)) {
        return error("denorm 1");
    }
    if( !testOptionDenormalizeImpl(true, false)) {
        return error("denorm 2");
    }
    return true ;
}

//-----------------------------------------------------

bool TestCompareXml::testUnitBase()
{
    QString testName = "testUnitBase" ;

    //---------
    if( !testEqualsElements()) {
        return error(testName, "equals elements");
    }
    if( !testEqualsComment()) {
        return error(testName, "equals comment");
    }
    if( !testEqualsPI()) {
        return error(testName, "equals PI");
    }
    if( !testEqualsText()) {
        return error(testName, "equals text");
    }
    if( !testEqualsTextCData()) {
        return error(testName, "equals text CDATA");
    }

    //---------
    if( !testAddedElements()) {
        return error(testName, "added elements");
    }
    if( !testAddedComment()) {
        return error(testName, "added comment");
    }
    if( !testAddedPI()) {
        return error(testName, "added PI");
    }
    if( !testAddedText()) {
        return error(testName, "added text");
    }
    if( !testAddedTextCData()) {
        return error(testName, "added text CData");
    }
    //---------
    if( !testDeletedElements()) {
        return error(testName, "deleted elements");
    }
    if( !testDeletedComment()) {
        return error(testName, "deleted comment");
    }
    if( !testDeletedPI()) {
        return error(testName, "deleted PI");
    }
    if( !testDeletedText()) {
        return error(testName, "deleted text");
    }
    if( !testDeletedTextCData()) {
        return error(testName, "deleted text CData");
    }
    //---------
    if( !testModifiedElements()) {
        return error(testName, "modified elements");
    }
    if( !testModifiedComment()) {
        return error(testName, "modified comment");
    }
    if( !testModifiedPI()) {
        return error(testName, "modified PI");
    }
    if( !testModifiedText()) {
        return error(testName, "modified text");
    }
    if( !testModifiedTextCData()) {
        return error(testName, "modified text CData");
    }
    //---------
    if(!testOptionDenormalize()) {
        return error(testName, "denormalize option");
    }
    return true;
}

bool TestCompareXml::helperTestFileLoad( const bool isExpectedError, const QString &filePath, const CompareModule::Errors errorCode, const QString &errorText)
{
    FakeUIDelegate uiDelegate;
    QStringList files;
    Regola dummy;
    dummy.setRootElement(new Element("root", "", &dummy, NULL ));
    dummy.setFileName(DModule::defaultFileName);
    DModule dModule( NULL, &dummy, &uiDelegate, files);
    bool isOk = dModule.loadAFileForTest(filePath);
    if( isExpectedError ) {
        if( isOk ) {
            return error(errorText);
        }
        if( !uiDelegate.isError ) {
            return error(QString("%1 : error not raised."));
        }
        if( uiDelegate.lastErrorMsg != dModule.textForError(errorCode)) {
            return error(QString("%1 : mesage different than expected."));
        }
    } else {
        if( !isOk ) {
            return error(QString("%1 : returning unexpectedtcly error."));
        }
    }
    return true;
}


bool TestCompareXml::testFileLoadPathEmpty()
{
    return helperTestFileLoad( true, "", CompareModule::ERR_FILEPATHEMPTY, "file path empty") ;
}

bool TestCompareXml::testFileLoadSameFile()
{
    return helperTestFileLoad( true, DModule::defaultFileName, CompareModule::ERR_SAMEFILE, "same file loaded") ;
}

bool TestCompareXml::testFileLoadNoSuchFile()
{
    return helperTestFileLoad( true, ":?:A?A:?\\????   ??", CompareModule::ERR_CANTLOAD, "non existent file") ;
}

bool TestCompareXml::testFileLoadFileOK()
{
    return helperTestFileLoad( false, REFERENCE_FILE, CompareModule::ERR_FILEPATHEMPTY, "valid file loading") ;
}


bool TestCompareXml::testLoadFile()
{
    _testName = "testLoadFile";
    if( !testFileLoadPathEmpty()) {
        return false;
    }
    if( !testFileLoadSameFile()) {
        return false;
    }
    if( !testFileLoadNoSuchFile()) {
        return false;
    }
    if( !testFileLoadFileOK()) {
        return false;
    }
    return true;
}

//----------------------------------------------------------------------------
#define FILE_COMPARE_OPTIONS_TEXT_EQ_COMM_EQ_REF    "../test/data/compare/textcomm.xml"
#define FILE_COMPARE_OPTIONS_TEXT_EQ_COMM_D_REF    "../test/data/compare/textcommd.xml"
#define FILE_COMPARE_OPTIONS_TEXT_D_COMM_EQ_REF    "../test/data/compare/textdcomm.xml"
#define FILE_COMPARE_OPTIONS_TEXT_D_COMM_D_REF    "../test/data/compare/textdcommd.xml"

bool TestCompareXml::testWithOptions(const QString &name, const bool isCompareText, const bool isCompareComments, const bool expected, const QString &file2)
{
    FakeUIDelegateYes uiDelegate;
    Regola regola;
    DModule dModule( NULL, &regola, &uiDelegate, QStringList());
    //test compare with options
    RegolaHolder holder;
    holder.ref = dModule.loadTestRegola(FILE_COMPARE_OPTIONS_TEXT_EQ_COMM_EQ_REF);
    holder.source = dModule.loadTestRegola(file2);
    if( !holder.isOk() ) {
        return error(name+": load files");
    }
    dModule.setCompareOptions(isCompareText, isCompareComments);
    dModule.startCompare(holder.ref, holder.source);
    bool result = dModule.areEquals() ;
    if( result != expected ) {
        return error(name);
    }
    return true;
}

bool TestCompareXml::testWithAllOptions( const QString &name, const bool *expected, const QString &file2)
{
    if( !testWithOptions(name+" t, t", true, true, expected[0], file2) ) {
        return false;
    }
    if( !testWithOptions(name+" t, f", true, false, expected[1], file2) ) {
        return false;
    }
    if( !testWithOptions(name+" f, t", false, true, expected[2], file2) ) {
        return false;
    }
    if( !testWithOptions(name+" f, f", false, false, expected[3], file2) ) {
        return false;
    }
    return true;
}


bool TestCompareXml::testWithOptionsConditions()
{
    {
        bool testFlags1[4] = {true, true, true, true};
        if( !testWithAllOptions("options eq, eq", testFlags1, FILE_COMPARE_OPTIONS_TEXT_EQ_COMM_EQ_REF) ) {
            return false;
        }
    }
    //----
    {
        bool testFlags2[4] = {false, true, false, true};
        if( !testWithAllOptions("options diff comm", testFlags2, FILE_COMPARE_OPTIONS_TEXT_EQ_COMM_D_REF) ) {
            return false;
        }
    }
    //----
    {
        bool testFlags3[4] = {false, false, true, true};
        if( !testWithAllOptions("options diff text", testFlags3, FILE_COMPARE_OPTIONS_TEXT_D_COMM_EQ_REF) ) {
            return false;
        }
    }
    //----
    {
        bool testFlags4[4] = {false, false, false, true};
        if( !testWithAllOptions("options diff diff", testFlags4, FILE_COMPARE_OPTIONS_TEXT_D_COMM_D_REF) ) {
            return false;
        }
    }
    //--
    return true;
}

//---------------------------------------------

bool TestCompareXml::compareInfoItems(const QString &name, QList<DiffInfo*> &items, QList<DiffInfo*> &compare)
{
    if( items.size() != compare.size() ) {
        return error(QString("%3 compareInfoItems size differ %1 vs %2").arg(items.size() ).arg(compare.size()).arg(name));
    }
    int index = 0 ;
    foreach( DiffInfo *info, items) {
        DiffInfo *c = compare.at(index);
        if( c->isElementOrAttribute != info->isElementOrAttribute ) {
            return error(QString("%4 compareInfoItems isElementOrAttribute fails at %1 (%2 vs %3").arg(index).arg(c->isElementOrAttribute).arg(info->isElementOrAttribute).arg(name));
        }
        if( c->diffType != info->diffType ) {
            return error(QString("%4 compareInfoItems diffType fails at %1 (%2 vs %3").arg(index).arg(c->diffType).arg(info->diffType).arg(name));
        }
        index ++ ;
    }
    return true ;
}

bool TestCompareXml::compareTreeItems(const QString &name, QList<QTreeWidgetItem*> &items, QList<TreeItemInfo*> &compareItems )
{
    if( items.size() != compareItems.size() ) {
        return error(QString("%3 compareTreeItems size differ %1 vs %2").arg(items.size() ).arg(compareItems.size()).arg(name));
    }
    int index = 0;
    foreach( QTreeWidgetItem *item, items) {
        TreeItemInfo *compareItem = compareItems[index];

        int lastDiffIndex = CompareSideBySideView::lastDiffIndexFromItem(item);
        int currDiffIndex = CompareSideBySideView::thisDiffIndexFromItem(item);

        if( lastDiffIndex != compareItem->prevDiff ) {
            return error(QString("%4 compareTreeItems lastDiffIndex fails at %1 (%2 vs %3").arg(index).arg(lastDiffIndex).arg(compareItem->prevDiff).arg(name));
        }

        if( currDiffIndex != compareItem->currDiff ) {
            return error(QString("%4 compareTreeItems currDiffIndex fails at %1 (%2 vs %3").arg(index).arg(currDiffIndex).arg(compareItem->currDiff).arg(name));
        }
        index ++ ;
    }
    return true ;
}

bool TestCompareXml::testCompareDifferences(const QString &name, const QString &file1, const QString &file2, QList<TreeItemInfo*> &compareItems, QList<DiffInfo*> &diffInfos )
{
    for( int i = 0 ; i < 3 ; i ++ ) {
        FakeUIDelegateYes uiDelegate;
        Regola regola;
        DModule dModule( NULL, &regola, &uiDelegate, QStringList());
        //test compare with options
        RegolaHolder holder;
        holder.ref = dModule.loadTestRegola(file1);
        holder.source = dModule.loadTestRegola(file2);
        if( !holder.isOk() ) {
            return error("testCompareDifferences load files");
        }
        dModule.setCompareOptions(true, true);
        dModule.startCompare(holder.ref, holder.source);
        if( !compareInfoItems(name, dModule.diffInfos(), diffInfos) ) {
            return false ;
        }
        if( !compareTreeItems(name, dModule.referenceItemsForTest(), compareItems ) ) {
            return false ;
        }
    }
    return true;
}


#define ATTR_ADD1   "../test/data/compare/attradd1.xml"
#define ATTR_ADD2   "../test/data/compare/attradd2.xml"
#define ATTR_MOD1   "../test/data/compare/attrmod1.xml"
#define ATTR_MOD2   "../test/data/compare/attrmod2.xml"
#define ATTR_EQ1   "../test/data/compare/attreq1.xml"
#define ATTR_EQ2   "../test/data/compare/attreq2.xml"
#define ATTR_DEL1   "../test/data/compare/attrdel1.xml"
#define ATTR_DEL2   "../test/data/compare/attrdel2.xml"

#define TII(a,b)   new TreeItemInfo(a,b)

/*
Root
Element
Attrib
+A
*/
bool TestCompareXml::testElemWithAttributeAdd()
{
    DiffListInfoHolder h;
    h.compareItems << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,0);
    h.infos << new DiffInfo( 0, EDiff::ED_ADDED, false, NULL, NULL );

    bool result = testCompareDifferences("testElemWithAttributeAdd", ATTR_ADD1, ATTR_ADD2, h.compareItems, h.infos );

    return result;
}

bool TestCompareXml::testElemWithAttributeMod()
{
    DiffListInfoHolder h;
    h.compareItems << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,0);
    h.infos << new DiffInfo( 0, EDiff::ED_MODIFIED, false, NULL, NULL );

    bool result = testCompareDifferences("testElemWithAttributeMod", ATTR_MOD1, ATTR_MOD2, h.compareItems, h.infos );

    return result;
}

bool TestCompareXml::testElemWithAttributeEq()
{
    DiffListInfoHolder h;
    h.compareItems << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,-1);

    bool result = testCompareDifferences("testElemWithAttributeEq", ATTR_EQ1, ATTR_EQ2, h.compareItems, h.infos );

    return result;
}

bool TestCompareXml::testElemWithAttributeDel()
{
    DiffListInfoHolder h;
    h.compareItems << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,0);
    h.infos << new DiffInfo( 0, EDiff::ED_DELETED, false, NULL, NULL );

    bool result = testCompareDifferences("testElemWithAttributeDel", ATTR_DEL1, ATTR_DEL2, h.compareItems, h.infos );

    return result;
}

#define ATTR_ADDEQ1  "../test/data/compare/attraddeq1.xml"
#define ATTR_ADDEQ2  "../test/data/compare/attraddeq2.xml"
#define ATTR_ADDDEL1    "../test/data/compare/attradddel1.xml"
#define ATTR_ADDDEL2    "../test/data/compare/attradddel2.xml"
#define ATTR_EQDEL1    "../test/data/compare/attraeqdel1.xml"
#define ATTR_EQDEL2    "../test/data/compare/attraeqdel2.xml"
#define ATTR_ADDMOD1    "../test/data/compare/attraddmod1.xml"
#define ATTR_ADDMOD2    "../test/data/compare/attraddmod2.xml"
#define ATTR_DELMOD1    "../test/data/compare/attrdelmod1.xml"
#define ATTR_DELMOD2    "../test/data/compare/attrdelmod2.xml"
#define ATTR_EQMOD1    "../test/data/compare/attreqmod1.xml"
#define ATTR_EQMOD2    "../test/data/compare/attreqmod2.xml"
#define ATTR_ALL1    "../test/data/compare/attrall1.xml"
#define ATTR_ALL2    "../test/data/compare/attrall2.xml"

bool TestCompareXml::testElemWithAttributeAddEq()
{
    DiffListInfoHolder h;
    h.compareItems << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,0) << TII(0,-1) ;
    h.infos << new DiffInfo( 0, EDiff::ED_ADDED, false, NULL, NULL );

    bool result = testCompareDifferences("testElemWithAttributeAddEq", ATTR_ADDEQ1, ATTR_ADDEQ2, h.compareItems, h.infos );

    return result;
}

bool TestCompareXml::testElemWithAttributeAddDel()
{
    DiffListInfoHolder h;
    h.compareItems << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,0) << TII(0,1) ;
    h.infos << new DiffInfo( 0, EDiff::ED_ADDED, false, NULL, NULL );
    h.infos << new DiffInfo( 0, EDiff::ED_DELETED, false, NULL, NULL );

    bool result = testCompareDifferences("testElemWithAttributeAddDel", ATTR_ADDDEL1, ATTR_ADDDEL2, h.compareItems, h.infos );

    return result;
}

bool TestCompareXml::testElemWithAttributeEqDel()
{
    DiffListInfoHolder h;
    h.compareItems << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,0) ;
    h.infos << new DiffInfo( 0, EDiff::ED_DELETED, false, NULL, NULL );

    bool result = testCompareDifferences("testElemWithAttributeEqDel", ATTR_EQDEL1, ATTR_EQDEL2, h.compareItems, h.infos );

    return result;
}

bool TestCompareXml::testElemWithAttributeAddMod()
{
    DiffListInfoHolder h;
    h.compareItems << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,0) << TII(0,1) ;
    h.infos << new DiffInfo( 0, EDiff::ED_ADDED, false, NULL, NULL );
    h.infos << new DiffInfo( 0, EDiff::ED_MODIFIED, false, NULL, NULL );

    bool result = testCompareDifferences("testElemWithAttributeAddMod", ATTR_ADDMOD1, ATTR_ADDMOD2, h.compareItems, h.infos );

    return result;
}

bool TestCompareXml::testElemWithAttributeDelMod()
{
    DiffListInfoHolder h;
    h.compareItems << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,0) << TII(0,1) ;
    h.infos << new DiffInfo( 0, EDiff::ED_MODIFIED, false, NULL, NULL );
    h.infos << new DiffInfo( 0, EDiff::ED_DELETED, false, NULL, NULL );

    bool result = testCompareDifferences("testElemWithAttributeDelMod", ATTR_DELMOD1, ATTR_DELMOD2, h.compareItems, h.infos );

    return result;
}

bool TestCompareXml::testElemWithAttributeEqMod()
{
    DiffListInfoHolder h;
    h.compareItems << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,0) << TII(0,-1) ;
    h.infos << new DiffInfo( 0, EDiff::ED_MODIFIED, false, NULL, NULL );

    bool result = testCompareDifferences("testElemWithAttributeEqMod", ATTR_EQMOD1, ATTR_EQMOD2, h.compareItems, h.infos );

    return result;
}

bool TestCompareXml::testElemWithAllAttributes()
{
    DiffListInfoHolder h;
    h.compareItems << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,0) << TII(0,1) << TII(1,-1) << TII(1,2) ;
    h.infos << new DiffInfo( 0, EDiff::ED_ADDED, false, NULL, NULL );
    h.infos << new DiffInfo( 0, EDiff::ED_MODIFIED, false, NULL, NULL );
    h.infos << new DiffInfo( 0, EDiff::ED_DELETED, false, NULL, NULL );

    bool result = testCompareDifferences("testElemWithAllAttributes", ATTR_ALL1, ATTR_ALL2, h.compareItems, h.infos );

    return result;
}

bool TestCompareXml::testCompareAttributes()
{
    if( !testElemWithAttributeAdd()) {
        return false;
    }
    if( !testElemWithAttributeMod()) {
        return false;
    }
    if( !testElemWithAttributeEq()) {
        return false;
    }
    if( !testElemWithAttributeDel()) {
        return false;
    }
    if( !testElemWithAttributeAddEq()) {
        return false;
    }
    if( !testElemWithAttributeAddDel()) {
        return false;
    }
    if( !testElemWithAttributeEqDel()) {
        return false;
    }
    if( !testElemWithAttributeAddMod()) {
        return false;
    }
    if( !testElemWithAttributeDelMod()) {
        return false;
    }
    if( !testElemWithAttributeEqMod()) {
        return false;
    }
    if( !testElemWithAllAttributes()) {
        return false;
    }
    return true;
}

//------------------------------------------------------------------------

#define ELEM_EQ1  "../test/data/compare/elemeq1.xml"
#define ELEM_EQ2  "../test/data/compare/elemeq2.xml"

#define ELEM_ADD1  "../test/data/compare/elemadd1.xml"
#define ELEM_ADD2  "../test/data/compare/elemadd2.xml"

#define ELEM_DEL1  "../test/data/compare/elemdel1.xml"
#define ELEM_DEL2  "../test/data/compare/elemdel2.xml"

#define ELEM_MOD1  "../test/data/compare/elemmod1.xml"
#define ELEM_MOD2  "../test/data/compare/elemmod2.xml"

/*
0 root
1 elem
2 <<attrib>>
3 a1
4 a2
5 target
6 <<attrib>>
7 a1
*/
bool TestCompareXml::testElemEq()
{
    DiffListInfoHolder h;
    h.compareItems << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) ;

    bool result = testCompareDifferences("testElemEq", ELEM_EQ1, ELEM_EQ2, h.compareItems, h.infos );

    return result;
}

bool TestCompareXml::testElemAdd()
{
    DiffListInfoHolder h;
    h.compareItems << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,-1)
                      << TII(-1,0) << TII(-1,0) << TII(-1,0) ;
    h.infos << new DiffInfo( 0, EDiff::ED_ADDED, true, NULL, NULL );

    bool result = testCompareDifferences("testElemAdd", ELEM_ADD1, ELEM_ADD2, h.compareItems, h.infos );

    return result;
}

bool TestCompareXml::testElemDel()
{
    DiffListInfoHolder h;
    h.compareItems << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,-1)
                      << TII(-1,0) << TII(-1,0) << TII(-1,0) ;
    h.infos << new DiffInfo( 0, EDiff::ED_DELETED, true, NULL, NULL );
    bool result = testCompareDifferences("testElemDel", ELEM_DEL1, ELEM_DEL2, h.compareItems, h.infos );

    return result;
}

bool TestCompareXml::testElemMod()
{
    DiffListInfoHolder h;
    h.compareItems << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,0) ;
    h.infos << new DiffInfo( 0, EDiff::ED_MODIFIED, true, NULL, NULL );
    bool result = testCompareDifferences("testElemMod", ELEM_MOD1, ELEM_MOD2, h.compareItems, h.infos );

    return result;
}

#define ELEM_ADDDEL1  "../test/data/compare/elemadddel1.xml"
#define ELEM_ADDDEL2  "../test/data/compare/elemadddel2.xml"
#define ELEM_ADDDEQ1  "../test/data/compare/elemaddeq1.xml"
#define ELEM_ADDDEQ2  "../test/data/compare/elemaddeq2.xml"
#define ELEM_ADDDMOD1  "../test/data/compare/elemaddmod1.xml"
#define ELEM_ADDDMOD2  "../test/data/compare/elemaddmod2.xml"
#define ELEM_EQMOD1  "../test/data/compare/elemeqmod1.xml"
#define ELEM_EQMOD2  "../test/data/compare/elemeqmod2.xml"
#define ELEM_EQDEL1  "../test/data/compare/elemeqdel1.xml"
#define ELEM_EQDEL2  "../test/data/compare/elemeqdel2.xml"
#define ELEM_ALL1  "../test/data/compare/elemeall1.xml"
#define ELEM_ALL2  "../test/data/compare/elemeall2.xml"

bool TestCompareXml::testElemWithAddDel()
{
    DiffListInfoHolder h;
    h.compareItems << TII(-1,-1) << TII(-1, 0) << TII(-1, 0) << TII(0,1) << TII(0,1) ;

    h.infos << new DiffInfo( 0, EDiff::ED_ADDED, true, NULL, NULL );
    h.infos << new DiffInfo( 1, EDiff::ED_DELETED, true, NULL, NULL );

    bool result = testCompareDifferences("testElemddDe", ELEM_ADDDEL1, ELEM_ADDDEL2, h.compareItems, h.infos );

    return result;
}

bool TestCompareXml::testElemWithAddEq()
{
    DiffListInfoHolder h;
    h.compareItems << TII(-1,-1) << TII(-1, 0) << TII(-1, 0) << TII(0,-1) << TII(0,-1) ;

    h.infos << new DiffInfo( 0, EDiff::ED_ADDED, true, NULL, NULL );

    bool result = testCompareDifferences("testElemAddEq", ELEM_ADDDEQ1, ELEM_ADDDEQ2, h.compareItems, h.infos );

    return result;
}

bool TestCompareXml::testElemWithAddMod()
{
    DiffListInfoHolder h;
    h.compareItems << TII(-1,-1) << TII(-1, 0) << TII(0, 1) << TII(0, 1) ;

    h.infos << new DiffInfo( 0, EDiff::ED_MODIFIED, true, NULL, NULL );
    h.infos << new DiffInfo( 1, EDiff::ED_ADDED, true, NULL, NULL );

    bool result = testCompareDifferences("testElemAddMod", ELEM_ADDDMOD1, ELEM_ADDDMOD2, h.compareItems, h.infos );

    return result;
}

bool TestCompareXml::testElemWithEqMod()
{
    DiffListInfoHolder h;
    h.compareItems << TII(-1,-1) << TII(-1, 0) << TII(0, -1) << TII(0, -1) ;

    h.infos << new DiffInfo( 0, EDiff::ED_MODIFIED, true, NULL, NULL );

    bool result = testCompareDifferences("testElemWithEqMod", ELEM_EQMOD1, ELEM_EQMOD2, h.compareItems, h.infos );

    return result;
}

bool TestCompareXml::testElemWithEqDel()
{
    DiffListInfoHolder h;
    h.compareItems << TII(-1,-1) << TII(-1, -1) << TII(-1, -1) << TII(-1, 0) << TII(-1, 0) ;

    h.infos << new DiffInfo( 0, EDiff::ED_DELETED, true, NULL, NULL );

    bool result = testCompareDifferences("testElemWithEqDel", ELEM_EQDEL1, ELEM_EQDEL2, h.compareItems, h.infos );

    return result;
}

bool TestCompareXml::testElemWithAllElementsAndTypes()
{
    DiffListInfoHolder h;
    h.compareItems << TII(-1,-1) << TII(-1, 0) << TII(0, -1) << TII(0, -1)
                      << TII(0,1) << TII(0, 1)
                      << TII(1,2) << TII(1, 2) ;

    h.infos << new DiffInfo( 0, EDiff::ED_MODIFIED, true, NULL, NULL );
    h.infos << new DiffInfo( 1, EDiff::ED_ADDED, true, NULL, NULL );
    h.infos << new DiffInfo( 2, EDiff::ED_DELETED, true, NULL, NULL );

    bool result = testCompareDifferences("testElemWithAllElementsAndTypes", ELEM_ALL1, ELEM_ALL2, h.compareItems, h.infos );

    return result;
}


bool TestCompareXml::testCompareElements()
{
    if( !testElemAdd()) {
        return false;
    }
    if( !testElemEq()) {
        return false;
    }
    if( !testElemDel()) {
        return false;
    }
    if( !testElemMod()) {
        return false;
    }

    if( !testElemWithAddDel()) {
        return false;
    }
    if( !testElemWithAddEq()) {
        return false;
    }
    if( !testElemWithAddMod()) {
        return false;
    }
    if( !testElemWithEqMod()) {
        return false;
    }
    if( !testElemWithEqDel()) {
        return false;
    }

    if( !testElemWithAllElementsAndTypes()) {
        return false;
    }
    return true;

}

//---------------------------------------------
bool TestCompareXml::testCompareDifferenceList()
{
    //compara l'elenco e la lista di vari casi
    DiffListInfoHolder h;
    h.compareItems  << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) //0..3
                      << TII(-1, 0) << TII(0,1) << TII(1,-1) << TII(1,2)   //4..7
                        << TII(2, 3) << TII(2,3) << TII(2,3) << TII(2,3)    //8..11
                        << TII(3, 4) << TII(3,4) << TII(3,4) << TII(3,4); //12..15
    h.infos << new DiffInfo( 0, EDiff::ED_ADDED, false, NULL, NULL );
    h.infos << new DiffInfo( 1, EDiff::ED_MODIFIED, false, NULL, NULL );
    h.infos << new DiffInfo( 2, EDiff::ED_DELETED, false, NULL, NULL );
    h.infos << new DiffInfo( 3, EDiff::ED_ADDED, true, NULL, NULL );
    h.infos << new DiffInfo( 4, EDiff::ED_DELETED, true, NULL, NULL );

    bool result = testCompareDifferences("testElemWithAttributeAdd", REFERENCE_FILE, COMPARE_FILE, h.compareItems, h.infos );

    return result;
}

bool TestCompareXml::checkSelectItemDiff( DModule *module, const int itemToSelect,
                                          const bool expectedFirstDiff, const bool expectedPrevDiff,
                                          const bool expectedNextDiff, const bool expectedLastDiff )
{
    module->selectItem(itemToSelect);
    if( module->isFirstDiffButtonEnabled() != expectedFirstDiff) {
        return error(QString("first diff at %1").arg(itemToSelect));
    }
    if( module->isPrevDiffButtonEnabled() != expectedPrevDiff) {
        return error(QString("prev diff at %1").arg(itemToSelect));
    }
    if( module->isNextDiffButtonEnabled() != expectedNextDiff) {
        return error(QString("next diff at %1").arg(itemToSelect));
    }
    if( module->isLastDiffButtonEnabled() != expectedLastDiff) {
        return error(QString("last diff at %1").arg(itemToSelect));
    }
    return true ;
}

#define NTESTS_X    (16)
bool TestCompareXml::testCompareUI()
{
    _testName = "testCompareUI";
    TestCompareXmlUnitHelper helper;
    // load diff
    RegolaHolder holder;
    holder.ref = helper.dModule.loadTestRegola(REFERENCE_FILE);
    holder.source = helper.dModule.loadTestRegola(COMPARE_FILE);
    if( !holder.isOk() ) {
        return error("load files");
    }
    helper.dModule.setCompareOptions(true, true);
    helper.dModule.startCompare(holder.ref, holder.source);

    /*h.compareItems  << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) << TII(-1,-1) //0..3
                      << TII(-1, 0) << TII(0,1) << TII(1,-1) << TII(1,2)   //4..7
                        << TII(2, 3) << TII(2,3) << TII(2,3) << TII(2,3)    //8..11
                        << TII(3, 4) << TII(3,4) << TII(3,4) << TII(3,4); //12..15*/

    bool firstDiffEnabled [NTESTS_X] =  {true,  true,  true,  true,  false, true, true, true,
        true, true, true, true, true, true, true, true };
    bool prevDiffEnabled [NTESTS_X] =   {false, false, false, false, false, true, true, true,
        true, true, true, true, true, true, true, true };
    bool nextDiffEnabled [NTESTS_X] =   {true,  true,  true,  true,  true,  true, true, true,
        true, true, true, true, false, false, false, false };
    bool lastDiffEnabled [NTESTS_X] =   {true,  true,  true,  true,  true,  true, true, true,
        true, true, true, true, false, false, false, false };

    for( int i = 0 ; i < NTESTS_X ; i ++ ) {
        // select the first difference, then the others
        if( !checkSelectItemDiff( &helper.dModule, i, firstDiffEnabled[i], prevDiffEnabled[i], nextDiffEnabled[i], lastDiffEnabled[i] ) ) {
            return false;
        }
    }
    // other tests here

    for( int i = 0 ; i < NTESTS_X ; i ++ ) {
        helper.dModule.selectDiff(i);
        QTreeWidgetItem * item = helper.dModule.testLeftTree()->currentItem();
        if( NULL != item ) {
            //
            int lastDiffIndex = CompareSideBySideView::lastDiffIndexFromItem(item);
            int currDiffIndex = CompareSideBySideView::thisDiffIndexFromItem(item);
            if( currDiffIndex != (lastDiffIndex+1)) {
                return error(QString("Selected diff index:%1 found:%2 and %3").arg(i).arg(lastDiffIndex).arg(currDiffIndex));
            }

        }
    }

    // jump to first, next, prev and last
    helper.dModule.moveToFirst();
    if( helper.dModule.selectedDiff()!= 0 ){
        return error(QString("Move to first expected 0 found %1").arg(helper.dModule.selectedDiff()));
    }
    helper.dModule.moveToNext();
    if( helper.dModule.selectedDiff()!= 1 ){
        return error(QString("Move to first expected 1 found %1").arg(helper.dModule.selectedDiff()));
    }

    helper.dModule.moveToPrev();
    if( helper.dModule.selectedDiff()!= 0 ){
        return error(QString("Move to first expected 0 found %1").arg(helper.dModule.selectedDiff()));
    }
    helper.dModule.moveToLast();
    if( helper.dModule.selectedDiff()!= 4 ){
        return error(QString("Move to first expected 4 found %1").arg(helper.dModule.selectedDiff()));
    }
    helper.dModule.moveToPrev();
    if( helper.dModule.selectedDiff()!= 3 ){
        return error(QString("Move to first expected 3 found %1").arg(helper.dModule.selectedDiff()));
    }
    return true;
}

//---------------------------------------------

bool TestCompareXml::testResults()
{
    _testName = "testResults";
    // dialog options
    if( !testWithOptionsConditions()) {
        return false;
    }

    // check the difference list
    if( !testCompareDifferenceList()) {
        return false;
    }

    // compare a set of files for case
    if( !testCompareElements()) {
        return false;
    }

    // compare a set of files for case
    if( !testCompareAttributes()) {
        return false;
    }

    // Check the UI.
    if( !testCompareUI()) {
        return false;
    }

    return true;
}
