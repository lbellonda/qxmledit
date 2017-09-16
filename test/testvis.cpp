/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2017 by Luca Bellonda and individual contributors  *
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

#include "testvis.h"
#include "app.h"
#include "visualization/vismapdialog.h"
#include "modules/graph/nodesrelationsdialog.h"
#include "visualization/datawidget.h"

#define BASE_PATH_ATTR "../test/data/vis/attributes/"

#define DATA_FILE_1 "../test/data/vis/testvis.xml"
#define DATA_FILE_2 "../test/data/vis/testvis2.xml"
#define DATA_FILE_COUNT "../test/data/vis/testvis_count.xml"

TestVis::TestVis()
{
}

bool TestVis::testFast()
{
    return testAttributeBaseBigData();
}

bool TestVis::test()
{
    if( ! testBaseElement() ) {
        return error("error");
    }

    if( ! testDialog() ) {
        return error("error");
    }
    return true;
}



/** \brief test list mechanism
  */

bool TestVis::testBaseElement()
{
    ElementBase *root = new ElementBase( NULL, "root") ;
    ElementBase *base = new ElementBase( root, "base") ;
    /*ElementBase *child1 = */new ElementBase( base, "child1") ;
    ElementBase *child2 = new ElementBase( base, "child2") ;
    /*ElementBase *child3 = */new ElementBase( base, "child3") ;
    /*ElementBase *child4 = */new ElementBase( child2, "child4") ;

    // 1 - count
    int total = 0 ;
    count(root, total );
    if( total != 6 ) {
        return error("error");
    }
    // 2 - ordered scan
    ElementBase *child = root->firstChild->firstChild;
    if( (NULL == child ) || (child->name !=  QString("child1")) ) {
        return error("error");
    }
    child = child->next;
    if( (NULL == child ) || (child->name !=  QString("child2")) ) {
        return error("error");
    }
    child = child->next;
    if( (NULL == child ) || (child->name !=  QString("child3")) ) {
        return error("error");
    }
    child = child->next;
    if( NULL != child ) {
        return error("error");
    }
    delete root;
    return true;
}

void TestVis::count(ElementBase *e, int &total )
{
    total++;
    ElementBase * child = e->firstChild;
    while( child != NULL ) {
       count(child, total );
       child = child->next;
    }
}

//-------------------------------------------------- Test dialog.

bool TestVis::testDialog()
{
    App app;
    if(!app.init() ) {
        return error("error");
    }
    VisMapDialog visMapDialog(app.data(), app.mainWindow(), app.mainWindow(), "");
    // ckeck data.
    visMapDialog.loadFile(DATA_FILE_1);

    // check data
    if( visMapDialog._summary.totalSize != (87+15+4*3) ) {
        return error("error");
    }
    if( visMapDialog._summary.levels != 3 ) {
        return error("error");
    }
    if( visMapDialog._summary.totalElements != 6 ) {
        return error("error");
    }
    if( visMapDialog._summary.totalAttributes != 3) {
        return error("error");
    }
    if( visMapDialog._summary.maxAttributes != 2) {
        return error("error");
    }
    if( visMapDialog._summary.maxChildren != 3 ) {
        return error("error");
    }
    if( visMapDialog._summary.maxSize != 54) {
        return error("error");
    }
    if( visMapDialog._summary.maxText != 50) {
        return error("error");
    }
    if( visMapDialog._summary.totalText != 50) {
        return error("error");
    }
    if( visMapDialog._summary.totalAttributesSize != 9) {
        return error(QString("summary.totalAttributesSize, expected %1 found %2").arg(9).arg(visMapDialog._summary.totalAttributesSize));
    }
    if( visMapDialog._summary.maxAttributeSizePerElement != 6) {
        return error(QString("summary.maxAttributeSizePerElement, expected %1 found %2").arg(6).arg(visMapDialog._summary.maxAttributeSizePerElement));
    }
    if( visMapDialog._summary.meanAttributesSize() != 3) {
        return error(QString("summary.meanAttributesSize, expected %1 found %2").arg(3).arg(visMapDialog._summary.meanAttributesSize()));
    }
    // change level and check the recalc
    visMapDialog.calcSlice(0);
    if( visMapDialog._summary.totalFragments != 1) {
        return error("error");
    }
    visMapDialog.calcSlice(1);
    if( visMapDialog._summary.totalFragments != 3) {
        return error("error");
    }
    visMapDialog.calcSlice(2);
    if( visMapDialog._summary.totalFragments != 2) {
        return error("error");
    }
    // loads a smaller file and checks if data are changing
    visMapDialog.loadFile(DATA_FILE_2);

    if( visMapDialog._summary.totalSize != (30+5+4) ) {
        return error("error");
    }
    if( visMapDialog._summary.levels != 3 ) {
        return error("error");
    }
    if( visMapDialog._summary.totalElements != 4) {
        return error("error");
    }
    if( visMapDialog._summary.totalAttributes != 1) {
        return error("error");
    }
    if( visMapDialog._summary.maxAttributes != 1) {
        return error("error");
    }
    if( visMapDialog._summary.maxChildren != 2) {
        return error("error");
    }
    if( visMapDialog._summary.maxSize != (15) ) {
        return error("error");
    }
    if( visMapDialog._summary.maxText != 10) {
        return error("error");
    }
    if( visMapDialog._summary.totalText != 10) {
        return error("error");
    }
    if( visMapDialog._summary.totalAttributesSize != 4) {
        return error(QString("2 summary.totalAttributesSize, expected %1 found %2").arg(4).arg(visMapDialog._summary.totalAttributesSize));
    }
    if( visMapDialog._summary.maxAttributeSizePerElement != 4) {
        return error(QString("2 summary.maxAttributeSizePerElement, expected %1 found %2").arg(4).arg(visMapDialog._summary.maxAttributeSizePerElement));
    }
    if( visMapDialog._summary.meanAttributesSize() != 4) {
        return error(QString("2 summary.meanAttributesSize, expected %1 found %2").arg(4).arg(visMapDialog._summary.meanAttributesSize()));
    }
    return true;
}

bool TestVis::testSummary()
{
    _testName = "testSummary";

    App app;
    if(!app.init()) {
        return error("app init");
    }
    // 1 - Use default: check
    if( !app.mainWindow()->loadFile(DATA_FILE_COUNT) ) {
        return error(QString("Opening test file: '%1' ").arg(DATA_FILE_COUNT));
    }
    Regola *regola = app.mainWindow()->getRegola();
    QByteArray regolaData = regola->writeMemory();
    QBuffer regolaBuffer(&regolaData);
    QList<TagNode*> nodesList ;
    NodesRelationsDialog dialog(false, nodesList, NULL);
    dialog.loadNodesFromFile(&regolaBuffer, regola->fileName());
    QBuffer data;
    if(data.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream outStream(&data);
        dialog.saveStatisticsToStream(outStream);
        outStream.flush();
        data.close();
        if( outStream.status() != QTextStream::Ok ) {
            return error("Error writing data");
        }
    }
    QString result(data.data());
    // do all checks
    if(!result.contains(",a,b,c,root\n")) {
        return error(QString("failed header, is:'%1'").arg(result));
    }
    // check data
    if(!result.contains("\n1,a,1,        20 %,1,1,,,1,\n")) {
        return error(QString("failed row 1, is:'%1'").arg(result));
    }
    if(!result.contains("\n2,b,1,        20 %,1,1,,,1,\n")) {
        return error(QString("failed row 2, is:'%1'").arg(result));
    }
    if(!result.contains("\n3,c,2,        40 %,2,0,,,,\n")) {
        return error(QString("failed row 3, is:'%1'").arg(result));
    }
    if(!result.contains("\n4,root,1,        20 %,0,2,1,1,,\n")) {
        return error(QString("failed row 4, is:'%1'").arg(result));
    }

    // no contains other data
    if(result.contains("\n5,")) {
        return error(QString("failed non existent row, is:'%1'").arg(result));
    }

    return true ;
}


bool TestVis::testVisData()
{
    _testName = "testVisData" ;
    if( ! testSliceElements() ) {
        return false;
    }

    if( ! testDataThreading(true) ) {
        return false;
    }
    if( ! testDataThreading(false) ) {
        return false;
    }
    return true;
}

bool TestVis::testSliceElements()
{
    _testName = "testSliceElements" ;
    {
        QList<int> expected ;
        expected << 10 << 10 << 10 << 12;
        DataWidget dw;
        QList<int> compare = dw.computeSlices(4, 42);
        if(!compareListInts("one", expected, compare)) {
            return false;
        }
    }
    {
        QList<int> expected ;
        expected << 3 << 3 << 3;
        DataWidget dw;
        QList<int> compare = dw.computeSlices(3, 9);
        if(!compareListInts("one", expected, compare)) {
            return false;
        }
    }
    {
        QList<int> expected ;
        expected << 9;
        DataWidget dw;
        QList<int> compare = dw.computeSlices(1, 9);
        if(!compareListInts("one", expected, compare)) {
            return false;
        }
    }
    {
        QList<int> expected ;
        expected << 12 << 12 << 12 << 13;
        DataWidget dw;
        QList<int> compare = dw.computeSlices(4, 49);
        if(!compareListInts("one", expected, compare)) {
            return false;
        }
    }
    return true;
}

ElementBase *TestVis::buildElm(ElementBase *parent, const int level)
{
    if( level == 0 ) {
        return NULL ;
    }
    ElementBase *elem = new ElementBase(parent, "name") ;
    int attrCount = 5 ;
    for(int index = 0 ; index < attrCount ; index ++) {
        elem->size += 10 ;
        elem->size += 10 ;
    }
    elem->attributesCount = attrCount;
    FORINT(i, 4) {
        buildElm(elem, level-1);
    }
    return elem ;
}

bool TestVis::testDataThreading(const bool useStandard)
{
    _testName = "testVisData" ;
    if( useStandard) {
        _testName += "/standard";
    } else {
        _testName += "/multithreading";
    }
    DataWidget dwReference;
    {
        StdColorMap colorMap("a map");
        dwReference.setMtEnabled(false);
        dwReference.setColorMap(&colorMap);
        VisDataMap dataMap;
        ElementBase *e = buildElm(NULL, 4);
        VisMapDialog::calcSize(e, dataMap);
        // build map
        dataMap.calculate(e);
        dwReference.setVisType(DataWidget::Elements);
        dwReference.setGeometry(0,0,600,600);
        dwReference.setData(e);
        dwReference.setDataMap(&dataMap);
        dwReference.setCumulative(true);
        dwReference.recalc();
        dwReference.generateImage();
    }
    DataWidget dwTest;
    {
        StdColorMap colorMap("a map");
        dwTest.setMtEnabled(true);
        dwTest.setColorMap(&colorMap);
        VisDataMap dataMap;
        ElementBase *e = buildElm(NULL, 4);
        VisMapDialog::calcSize(e, dataMap);
        // build map
        dataMap.calculate(e);
        dwTest.setVisType(DataWidget::Elements);
        dwTest.setGeometry(0,0,600,600);
        dwTest.setData(e);
        dwTest.setDataMap(&dataMap);
        dwTest.setCumulative(true);
        dwTest.recalc();
        dwTest.generateImage(useStandard);
    }
    QImage reference = dwReference._cachedImage;
    QImage candidate = dwTest._cachedImage;

    /* debug code
    if(!reference.save("/tmp/reference.png", "PNG")) {
        return error("could not save reference");
    }
    if(!candidate.save("/tmp/candidate.png", "PNG") ) {
        return error("could not save candidate");
    }*/

    // compare the images

    if( (reference.width() != candidate.width()) || (reference.height() != candidate.height()) ){
        return error(QString("Dimensions: reference:(%1 x %2), candidate:(%3 x %4)")
                     .arg(reference.width()).arg(reference.height())
                     .arg(candidate.width()).arg(candidate.height()));
    }
    const int bytesPerLine = candidate.bytesPerLine();
    FORINT(y,candidate.height()) {
        const uchar *rb = reference.constScanLine(y);
        const uchar *rc = candidate.constScanLine(y);
        FORINT(b, bytesPerLine) {
            if(*rb!=*rc) {
                return error(QString("At line:%1 at byte:%2, ref:%3, candidate:%4").arg(y).arg(b).arg(*rb).arg(*rc));
            }
            rb++;
            rc++;
        }
    }
    return true;
}

#define FILE_DATA_ATTRIB BASE_PATH_ATTR "data_base.xml"
#define FILE_ATTRIB_BLACKLIST BASE_PATH_ATTR "blacklist.csv"
#define FILE_ATTRIB_WHITELIST BASE_PATH_ATTR "whitelist.csv"
#define FILE_DATA_ATTRIB_CSV BASE_PATH_ATTR "attrcsv.xml"
#define FILE_CSV_REFERENCE BASE_PATH_ATTR "reference.csv"

bool TestVis::testAttributeCount()
{
    _testName = "testAttributeCount" ;
    if( !testAttributeCountUnit()) {
        return false;
    }
    if( !testAttributeCountLoadingFile()) {
        return false;
    }
    if(!testAttributeBaseBigData()) {
        return false ;
    }
    return true ;
}

bool TestVis::testAttributeCountUnit()
{
    if(!testAttributeCountNoNo(false)) {
        return false;
    }
    if(!testAttributeCountYesNo(false)) {
        return false;
    }
    if(!testAttributeCountNoYes(false)) {
        return false;
    }
    if(!testAttributeCountYesYes(false)) {
        return false;
    }
    if(!testAttributeCountSaveCSV()) {
        return false;
    }
    return true;
}

bool TestVis::testAttributeCountLoadingFile()
{
    _testName = "testAttributeCountLoadingFile";
    if(!testAttributeCountNoNo(true)) {
        return false;
    }
    if(!testAttributeCountYesNo(true)) {
        return false;
    }
    if(!testAttributeCountNoYes(true)) {
        return false;
    }
    if(!testAttributeCountYesYes(true)) {
        return false;
    }
    return true;
}

bool TestVis::testAttributeCountNoNo(const bool isLoad)
{
    _testName = "testAttributeCountNoNo";
    if(!testAttributeBase( isLoad, false, false ) ) {
        return false;
    }
    return true;
}

bool TestVis::testAttributeCountYesNo(const bool isLoad)
{
    _testName = "testAttributeCountYesNo";
    if(!testAttributeBase( isLoad, true, false ) ) {
        return false;
    }
    return true;
}

bool TestVis::testAttributeCountNoYes(const bool isLoad)
{
    _testName = "testAttributeCountNoYes";
    if(!testAttributeBase( isLoad, false, true ) ) {
        return false;
    }
    return true;
}

bool TestVis::testAttributeCountYesYes(const bool isLoad)
{
    _testName = "testAttributeCountYesYes";
    if(!testAttributeBase( isLoad, true, true ) ) {
        return false;
    }
    return true;
}

static qreal perc(const long num, const long den)
{
    const long l = (num*1000)/den;
    return (qreal)l / 10.0;
}

/*
<a a="aa" e="" >
    <b bbb="bbbb"/>
    <c cccc="cccccc" z="z"/>

    WL:/root/a/c/@z  z
    BL:/root/a/c/@cccc  c
*/

AttributesSummaryData* TestVis::buildReferenceAttributesSummaryData( AttributesSummaryData* a, AttributesSummaryTotal &totalTotal, AttributesSummaryTotal &totalUsed, AttributesSummaryTotal &totalNotUsed, const bool isWhitelist, const bool isBlackList)
{
    const int Attributes = 5;
    const int Times = 2;
    AttributeSummaryData *ad = NULL;
    ad = a->attributeSummaryData("/root/a/@a", "a");
    ad->setData(2, 2*2, 0);
    ad = a->attributeSummaryData("/root/a/@e", "e");
    ad->setData(2, 0, 2);
    ad = a->attributeSummaryData("/root/a/b/@bbb", "bbb");
    ad->setData(2, 4*2, 0);
    ad = a->attributeSummaryData("/root/a/c/@cccc", "cccc");
    ad->setData(2, 6*2, 0);
    ad = a->attributeSummaryData("/root/a/c/@z", "z");
    ad->setData(2, 1*2, 0);

    const long SizeA = (Times * (1 + 4) ) + Times*2;
    const long SizeE = (Times * (1 + 4) ) + Times*0;
    const long SizeB = (Times * (3 + 4) ) + Times*4;
    const long SizeC = (Times * (4 + 4) ) + Times*6;
    const long SizeZ = (Times * (1 + 4) ) + Times*1;

    const long SizeAMem = (2 * 1) + (Times * 16) + (Times*2 * 2);
    const long SizeEMem = (2 * 1) + (Times * 16) + (0);
    const long SizeBMem = (2 * 3) + (Times * 16) + (Times*4 * 2);
    const long SizeCMem = (2 * 4) + (Times * 16) + (Times*6 * 2);
    const long SizeZMem = (2 * 1) + (Times * 16) + (Times*1* 2);

    const long SizeAll = SizeA+SizeE+SizeB+SizeC+SizeZ;
    const long SizeMemAll = SizeAMem+SizeEMem+SizeBMem+SizeCMem+SizeZMem;

    const long SizeEEmpty = Times * (1+4);
    const long SizeEMemEmpty = Times * 8;

    if(isWhitelist && isBlackList) {
        totalTotal.setData(Attributes,  Times*Attributes,     SizeMemAll,          SizeAll,          SizeAll/(Times*Attributes),            Times*1,  SizeEEmpty, SizeEMemEmpty, 100 );
        totalNotUsed.setData( 1,        Times,                SizeCMem,            SizeC,            SizeC/Times,                            0,       0,          0,             perc(SizeC,SizeAll) );
        totalUsed.setData(Attributes-1, Times*(Attributes-1), SizeMemAll-SizeCMem, SizeAll-SizeC,    (SizeAll-SizeC)/(Times*(Attributes-1)), Times*1, SizeEEmpty, SizeEMemEmpty, perc(SizeAll-SizeC,SizeAll) );
    } else if(!isWhitelist && isBlackList) {
        totalTotal.setData(Attributes,  Times*Attributes,     SizeMemAll,          SizeAll,          SizeAll/(Times*Attributes),            Times*1,  SizeEEmpty, SizeEMemEmpty, 100 );
        totalNotUsed.setData( 1,        Times,                SizeCMem,            SizeC,            SizeC/Times,                            0,       0,          0,             perc(SizeC,SizeAll) );
        totalUsed.setData(Attributes-1, Times*(Attributes-1), SizeMemAll-SizeCMem, SizeAll-SizeC,    (SizeAll-SizeC)/(Times*(Attributes-1)), Times*1, SizeEEmpty, SizeEMemEmpty, perc(SizeAll-SizeC,SizeAll) );
    } else if(isWhitelist && !isBlackList) {
        totalTotal.setData(Attributes,     Times*Attributes,     SizeMemAll,          SizeAll,          SizeAll/(Times*Attributes),            Times*1,  SizeEEmpty, SizeEMemEmpty, 100 );
        totalUsed.setData( 1,              Times,                SizeZMem,            SizeZ,            SizeZ/Times,                            0,       0,          0,             perc(SizeZ,SizeAll) );
        totalNotUsed.setData(Attributes-1, Times*(Attributes-1), SizeMemAll-SizeZMem, SizeAll-SizeZ,    (SizeAll-SizeZ)/(Times*(Attributes-1)), Times*1, SizeEEmpty, SizeEMemEmpty, perc(SizeAll-SizeZ,SizeAll) );
    } else { // if(!isWhitelist && !isBlackList) {
        totalTotal.setData(Attributes, Times*Attributes, SizeMemAll, SizeAll, SizeAll/(Times*Attributes), Times*1, SizeEEmpty, SizeEMemEmpty, 100 );
        totalUsed.setData(Attributes, Times*Attributes, SizeMemAll, SizeAll, SizeAll/(Times*Attributes), Times*1, SizeEEmpty, SizeEMemEmpty, 100 );
    }
    return a;
}

bool TestVis::testAttributeBase( const bool isLoad, const bool isWhiteList, const bool isBlackList )
{
    App app;
    if(!app.init() ) {
        return error("error");
    }
    AttributesSummaryData *attributesSummaryData = NULL ;
    if(isLoad) {
        QList<TagNode*> dataList;
        NodesRelationsDialog dialog(true, dataList, NULL, NULL );
        // ckeck data.
        dialog.loadFile(FILE_DATA_ATTRIB);
        if(isWhiteList) {
            if(!dialog.innerLoadAttributesList(FILE_ATTRIB_WHITELIST, true)) {
                return error("Opening whitelist");
            }
        }
        if(isBlackList) {
            if(!dialog.innerLoadAttributesList(FILE_ATTRIB_BLACKLIST, false)) {
                return error("Opening black list");
            }
        }
        attributesSummaryData = dialog.attributesSummaryData();
        return testAttributeBaseInner(attributesSummaryData, isWhiteList, isBlackList);
    } else {
        VisMapDialog visMapDialog(app.data(), app.mainWindow(), app.mainWindow(), "");
        // ckeck data.
        visMapDialog.loadFile(FILE_DATA_ATTRIB);
        if(isWhiteList) {
            if(!visMapDialog.loadAttributeWhiteList(FILE_ATTRIB_WHITELIST)) {
                return error("Opening whitelist");
            }
        }
        if(isBlackList) {
            if(!visMapDialog.loadAttributeBlackList(FILE_ATTRIB_BLACKLIST)) {
                return error("Opening black list");
            }
        }
        attributesSummaryData = visMapDialog.attributesSummaryData();
        return testAttributeBaseInner(attributesSummaryData, isWhiteList, isBlackList);
    }
    return error("logic error");
}

bool TestVis::testAttributeBaseInner(AttributesSummaryData *attributesSummaryData, const bool isWhiteList, const bool isBlackList)
{
    // compare data
    AttributesSummaryData referenceAttributesSummaryData ;
    AttributesSummaryTotal totalTotal, totalUsed, totalNotUsed;
    buildReferenceAttributesSummaryData(&referenceAttributesSummaryData, totalTotal, totalUsed, totalNotUsed, isWhiteList, isBlackList );
    QString reason;
    if(!referenceAttributesSummaryData.compareTo(attributesSummaryData, reason)) {
        return error(QString("Data differ: %1").arg(reason));
    }
    AttributesSummarySummary ast;
    ast.calculate(attributesSummaryData);
    if(!ast.totalTotal.compareTo(&totalTotal, reason)) {
        return error(QString("totalTotal differs: %1").arg(reason));
    }
    if(!ast.totalUsed.compareTo(&totalUsed, reason)) {
        return error(QString("totalUsed differs: %1").arg(reason));
    }
    if(!ast.totalNotUsed.compareTo(&totalNotUsed, reason)) {
        return error(QString("totalNotUsed differs: %1").arg(reason));
    }
    return true;
}

bool TestVis::buildReferenceAttributesBigData( AttributesSummaryData* attributesSummaryData, AttributesSummarySummary *ast)
{
    AttributeSummaryData *ad = NULL;
    ad = attributesSummaryData->attributeSummaryData("/root/yes/@a", "a");
    ad->setData(2, 5000000000ull, 1);
    ad = attributesSummaryData->attributeSummaryData("/root/yes/@b", "b");
    ad->addHit(1234567800ull);
    ad = attributesSummaryData->attributeSummaryData("/root/yes/@b", "b");
    ad->addHit(1111111111ull);
    //
    ad = attributesSummaryData->attributeSummaryData("/root/no/@a", "a");
    ad->setData(30, 5000000000ull, 3);
    ad = attributesSummaryData->attributeSummaryData("/root/no/@b", "b");
    ad->addHit(1234567800ull);
    ad = attributesSummaryData->attributeSummaryData("/root/no/@b", "b");
    ad->addHit(1111111111ull);
    //
    attributesSummaryData->insertInBlackList("/root/no/@a");
    attributesSummaryData->insertInBlackList("/root/no/@b");
    attributesSummaryData->insertInWhiteList("/root/yes/@a");
    attributesSummaryData->insertInWhiteList("/root/yes/@b");
    //
    if(!ast->calculate(attributesSummaryData)) {
        return error("calculating");
    }
    return true;
}

bool TestVis::testAttributeBaseBigData()
{
    _testName = "testAttributeCountBigData";

    // compare data
    AttributesSummarySummary  ast;
    AttributesSummaryData attributesSummaryData ;
    if(!buildReferenceAttributesBigData(&attributesSummaryData, &ast)) {
        return false ;
    }
    quint64 expected = 0 ;
    AttributesSummaryTotal *total = &ast.totalNotUsed ;
    {
        expected = 14691358338ull ;
        if(total->sizeInMemory != expected ) {
            return error(QString("TNU sizeMem exp.:%1 found:%2").arg(expected).arg(total->sizeInMemory));
        }
        expected = 7345679071ull ;
        if(total->sizeCharacters != expected ) {
            return error(QString("TNU sizeCharacters exp.:%1 found:%2").arg(expected).arg(total->sizeCharacters));
        }
        expected = 229552470ull;
        if(total->meanSize != expected ) {
            return error(QString("TNU meanSize exp.:%1 found:%2").arg(expected).arg(total->meanSize));
        }
        expected = 15 ;
        if(total->sizeEmptyData != expected ) {
            return error(QString("TNU sizeEmptyData exp.:%1 found:%2").arg(expected).arg(total->sizeEmptyData));
        }
        expected = 24 ;
        if(total->sizeMemoryEmptyData != expected ) {
            return error(QString("TNU sizeMemoryEmptyData exp.:%1 found:%2").arg(expected).arg(total->sizeMemoryEmptyData));
        }
        qreal expectedReal = 50 ;
        if(total->percTotalSize != expectedReal ) {
            return error(QString("TNU percTotalSize exp.:%1 found:%2").arg(expectedReal).arg(total->percTotalSize));
        }
    }
    //
    total = &ast.totalUsed ;
    {
        expected = 14691357890ull ;
        if(total->sizeInMemory != expected ) {
            return error(QString("TU sizeMem exp.:%1 found:%2").arg(expected).arg(total->sizeInMemory));
        }
        expected = 7345678931ull ;
        if(total->sizeCharacters != expected ) {
            return error(QString("TU sizeCharacters exp.:%1 found:%2").arg(expected).arg(total->sizeCharacters));
        }
        expected = 1836419732ull ;
        if(total->meanSize != expected ) {
            return error(QString("TU meanSize exp.:%1 found:%2").arg(expected).arg(total->meanSize));
        }
        expected = 5 ;
        if(total->sizeEmptyData != expected ) {
            return error(QString("TU sizeEmptyData exp.:%1 found:%2").arg(expected).arg(total->sizeEmptyData));
        }
        expected = 8 ;
        if(total->sizeMemoryEmptyData != expected ) {
            return error(QString("TU sizeMemoryEmptyData exp.:%1 found:%2").arg(expected).arg(total->sizeMemoryEmptyData));
        }
        qreal expectedReal = 49.9 ;
        if(total->percTotalSize != expectedReal ) {
            return error(QString("TU percTotalSize exp.:%1 found:%2").arg(expectedReal).arg(total->percTotalSize));
        }
    }
    //
    total = &ast.totalTotal ;
    {
        expected = 29382716228ull ;
        if(total->sizeInMemory != expected ) {
            return error(QString("TT sizeMem exp.:%1 found:%2").arg(expected).arg(total->sizeInMemory));
        }
        expected = 14691358002ull;
        if(total->sizeCharacters != expected ) {
            return error(QString("TT sizeCharacters exp.:%1 found:%2").arg(expected).arg(total->sizeCharacters));
        }
        expected = 408093277ull;
        if(total->meanSize != expected ) {
            return error(QString("TT meanSize exp.:%1 found:%2").arg(expected).arg(total->meanSize));
        }
        expected = 20 ;
        if(total->sizeEmptyData != expected ) {
            return error(QString("TT sizeEmptyData exp.:%1 found:%2").arg(expected).arg(total->sizeEmptyData));
        }
        expected = 32 ;
        if(total->sizeMemoryEmptyData != expected ) {
            return error(QString("TT sizeMemoryEmptyData exp.:%1 found:%2").arg(expected).arg(total->sizeMemoryEmptyData));
        }
        qreal expectedReal = 100 ;
        if(total->percTotalSize != expectedReal ) {
            return error(QString("TT percTotalSize exp.:%1 found:%2").arg(expectedReal).arg(total->percTotalSize));
        }
    }

    return true ;
}

//-------------------------------------------------------------------------------------

bool TestVis::testAttributeCountSaveCSV()
{
    _testName = "testAttributeCountSaveCSV";
    App app;
    if(!app.init() ) {
        return error("error");
    }
    QList<TagNode*> dataList;
    NodesRelationsDialog dialog(true, dataList, NULL, NULL );
    // ckeck data.
    dialog.loadFile(FILE_DATA_ATTRIB_CSV);
    QBuffer buffer;
    if( !dialog.exportAttributesCSVOnDevice(buffer)) {
        return error("Unable to write data");
    }
    buffer.close();
    QByteArray data = buffer.data();
    QString csv = QString::fromUtf8(data);
    QString reference ;
    if(!loadFileAsString(FILE_CSV_REFERENCE, "utf-8", &reference) ) {
        return error("loading reference");
    }
    if(csv != reference) {
        return error(QString("CSV differ: calc\n%1").arg(csv));
    }
    return true;
}

