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

#define DATA_FILE_1 "../test/data/vis/testvis.xml"
#define DATA_FILE_2 "../test/data/vis/testvis2.xml"
#define DATA_FILE_COUNT "../test/data/vis/testvis_count.xml"

TestVis::TestVis()
{
}

bool TestVis::testFast()
{
    return testVisData();
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
    if( visMapDialog._summary.totalSize != 87 ) {
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

    if( visMapDialog._summary.totalSize != 30 ) {
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
    if( visMapDialog._summary.maxSize != 14 ) {
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
    NodesRelationsDialog dialog(false, nodesList);
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
