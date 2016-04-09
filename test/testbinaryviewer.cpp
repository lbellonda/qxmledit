/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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

#include "testbinaryviewer.h"
#include "testhelpers/testbvd.h"
#include <QBuffer>
#include <QTableView>

#define SEARCH_LEMMA    "abcdx"


BinaryTestData::BinaryTestData( const QString &newName, int newPage, bool newMoveNextPage,
    int newExpectedPage, int newIsFirstPage, int newIsLastPage, int newNumRows, int newOffsetOfDataRow,
    const QString &newDisplayDataOfRow, const QString &newOffsetDataOfRow )
{
    name = newName;
    page = newPage ;
    moveNextPage = newMoveNextPage ;
    expectedPage = newExpectedPage;

    isFirstPage = newIsFirstPage;
    isLastPage = newIsLastPage;
    numRows = newNumRows;
    offsetOfDataRow = newOffsetOfDataRow;
    displayDataOfRow = newDisplayDataOfRow;
    offsetDataOfRow = newOffsetDataOfRow;
}

BinaryTestData::BinaryTestData()
{
    //
}

BinaryTestData::~BinaryTestData()
{
    //
}

//----------------------
BinaryTestDataPacket::BinaryTestDataPacket(const QString &newName, QByteArray *newDataToUse)
{
    name = newName ;
    dataToUse = newDataToUse ;
}

//----------------------
TestBinaryViewer::TestBinaryViewer()
{

}

TestBinaryViewer::~TestBinaryViewer()
{

}

bool TestBinaryViewer::testModel( const QString &name, BinaryTestData *test, QByteArray* data )
{
    BinaryViewerModel model;
    return testModel( name, &model, test, data );
}

bool TestBinaryViewer::testModel( const QString &name, BinaryViewerModel *model, BinaryTestData *test, QByteArray* data )
{
    QBuffer  *ioBuffer = new QBuffer(data);
    _testName = QString("testModel/%1/%2").arg(name).arg(test->name);
    BinaryViewerModel::ErrCodes code = model->setFile(ioBuffer);
    if( BinaryViewerModel::EC_NOERROR != code ) {
        return error(QString("Setting the buffer data, expecting:%1, found:%2").arg(BinaryViewerModel::EC_NOERROR).arg(code));
    }
    if(!model->setCodecByName("ISO-8859-1")) {
        return error("Unable to set the codec for the string");
    }

    if( 0 != model->currentPage() ) {
        return error(QString("The first page is not 0 as expected, but:%1").arg(model->currentPage()));
    }
    // 0: no move, >0 goto page, <0 abs(page), then next or prev
    if( test->page > 0 ) {
        model->goToPageAbs(test->page);
    } else if( test->page < 0 ) {
        model->goToPageAbs(-test->page);
        model->goToPage(test->moveNextPage);
    }

    if( test->expectedPage != model->currentPage() ) {
        return error(QString("The first page is not %1 as expected, but:%2").arg(test->expectedPage).arg(model->currentPage()));
    }
    if( model->isFirstPage() != test->isFirstPage ) {
        return error(QString("First page, expected %1, but found:%2").arg(test->isFirstPage).arg(model->isFirstPage()));
    }
    if( model->isLastPage() != test->isLastPage ) {
        return error(QString("Last page, expected %1, but found:%2").arg(test->isLastPage).arg(model->isLastPage()));
    }
    if( model->rowCount() != test->numRows ) {
        return error(QString("Rows number is wrong, expected %1, but found:%2").arg(test->numRows).arg(model->rowCount()));
    }

    QVariant addressResult = model->data(model->index(test->offsetOfDataRow, BinaryViewerModel::OffsetColumn, QModelIndex() ), Qt::DisplayRole);
    if( addressResult.toString() != test->offsetDataOfRow ) {
        return error(QString("Wrong offset data expecting '%1', but found:'%2'").arg(test->offsetDataOfRow).arg(addressResult.toString()));
    }
    QVariant dataResult = model->data(model->index(test->offsetOfDataRow, BinaryViewerModel::DataColumn, QModelIndex() ), Qt::DisplayRole);
    if( dataResult.toString() != test->displayDataOfRow ) {
        return error(QString("Wrong display data expecting '%1', but found:'%2'").arg(test->displayDataOfRow).arg(dataResult.toString()));
    }
    return true;
}


// tests:
// 1- start
// 2- next page
// 3- prev page
// 4- goto page
// 5- goto non existent page

bool TestBinaryViewer::testModelPacket( BinaryViewerModel *model, BinaryTestDataPacket *packet )
{
    if(!testModel(QString("modelPacket/%1/%2").arg(packet->name).arg("startTest"), model, &packet->startTest, packet->dataToUse)) {
        return false;
    }
    if(!testModel(QString("modelPacket/%1/%2").arg(packet->name).arg("nextPage"), model, &packet->nextPage, packet->dataToUse)) {
        return false;
    }
    if(!testModel(QString("modelPacket/%1/%2").arg(packet->name).arg("prevPage"), model, &packet->prevPage, packet->dataToUse)) {
        return false;
    }
    if(!testModel(QString("modelPacket/%1/%2").arg(packet->name).arg("gotoPage"), model, &packet->gotoPage, packet->dataToUse)) {
        return false;
    }
    if(!testModel(QString("modelPacket/%1/%2").arg(packet->name).arg("nePage"), model, &packet->nePage, packet->dataToUse)) {
        return error(QString("modelPacket/%1/%2 Positive result using wrong data").arg(packet->name).arg("nePage"));
    }
    return true;
}

bool TestBinaryViewer::testNoModelPacket( BinaryTestDataPacket *packet )
{
    {
        BinaryViewerModel model1;
        if(!testModel(QString("modelPacket/%1/%2").arg(packet->name).arg("startTest"), &model1, &packet->startTest, packet->dataToUse)) {
            return false;
        }
    }
    {
        BinaryViewerModel model2;
        if(!testModel(QString("modelPacket/%1/%2").arg(packet->name).arg("nextPage"), &model2, &packet->nextPage, packet->dataToUse)) {
            return false;
        }
    }
    {
        BinaryViewerModel model3;
        if(!testModel(QString("modelPacket/%1/%2").arg(packet->name).arg("prevPage"), &model3, &packet->prevPage, packet->dataToUse)) {
            return false;
        }
    }
    {
        BinaryViewerModel model4;
        if(!testModel(QString("modelPacket/%1/%2").arg(packet->name).arg("gotoPage"), &model4, &packet->gotoPage, packet->dataToUse)) {
            return false;
        }
    }
    {
        BinaryViewerModel model5;
        if(!testModel(QString("modelPacket/%1/%2").arg(packet->name).arg("nePage"), &model5, &packet->nePage, packet->dataToUse)) {
            return error(QString("modelPacket/%1/%2 Positive result using wrong data").arg(packet->name).arg("nePage"));
        }
    }
    return true;
}

QByteArray TestBinaryViewer::buildTwoPages()
{
    int len =  2*BinaryViewerModel::SizeOfPage-BinaryBlock::DataPerRow;
    QByteArray twoPages(len, 0);
    char *charArray = twoPages.data();
    int counter = 0;
    int loopMax = (len/4)*4;
    for( int i = 0 ; i < loopMax ; i+=4 ) {
        charArray[3] = counter & 0x00FF;
        charArray[2] = (counter>>8)& 0x00FF;
        charArray[1] = (counter>>16)& 0x00FF;
        charArray[0] = (counter>>24)& 0x00FF;
        charArray+=4;
        counter ++ ;
    }
    return twoPages ;
}

/*
 *sizeof row: 16
 *sizeof block: 16384
 *sizeof page: 262.144
 *rows per page:
 */

bool TestBinaryViewer::testRows(BinaryViewerModel *model)
{
    QByteArray twoPagesAndHalf = buildTwoPages();

    BinaryTestDataPacket pck2pages("2pages", &twoPagesAndHalf);
    int rowsSecondPage = BinaryViewerModel::RowsPerPage-1;

    pck2pages.startTest= BinaryTestData("start",    0, false, 0,  true, false,   BinaryViewerModel::RowsPerPage, 0, "00 00 00 00 | 00 00 00 01 | 00 00 00 02 | 00 00 00 03 ", "0" );
    pck2pages.nextPage = BinaryTestData("nextPage",-1, true,  1, false,  true,                   rowsSecondPage, 0, "00 01 00 00 | 00 01 00 01 | 00 01 00 02 | 00 01 00 03 ", "262144" );
    pck2pages.prevPage = BinaryTestData("prevPage",-1, false, 0,  true, false,   BinaryViewerModel::RowsPerPage, 0, "00 00 00 00 | 00 00 00 01 | 00 00 00 02 | 00 00 00 03 ", "0" );
    pck2pages.gotoPage = BinaryTestData("gotoPage", 1, false, 1, false,  true,                   rowsSecondPage, 0, "00 01 00 00 | 00 01 00 01 | 00 01 00 02 | 00 01 00 03 ", "262144" );
    pck2pages.nePage   = BinaryTestData("nePage",   2, false, 0,  true, false,   BinaryViewerModel::RowsPerPage, 0, "00 00 00 00 | 00 00 00 01 | 00 00 00 02 | 00 00 00 03 ", "0" );

    if(!testNoModelPacket( &pck2pages ) ) {
        return false;
    }
    if(!testModelPacket( model, &pck2pages ) ) {
        return false;
    }
    return true;
}


//--------------------------------- search -----------------------------------------------------------------

bool TestBinaryViewer::searchBase(BinaryViewerModel *model, const QString &searchLemma, const int startPage, const int startRow, const int expectedPage, const int expectedRow, const bool searchUp )
{
    bool isOk = false;
    bool isDiff = false;
    BinaryViewerOperationResult result;
    if(model->findOccurrences(result, searchLemma, startPage, startRow, searchUp) ) {
        if(result.errorCode == BinaryViewerOperationResult::BNO_ERROR) {
            isOk = true ;
            if( (expectedRow != result.row) || (expectedPage != result.page) ) {
                isDiff = true;
            }
        }
    }
    if(!isOk||isDiff) {
        return error(QString("Searching got %1, code %2, diff is %3, direction: %4 starting: page %5, row %6, expecting: page %7, row %8, found: page %9 row %10")
                     .arg(isOk).arg(isDiff).arg(result.errorCode).arg(searchUp)
                     .arg(startPage).arg(startRow)
                     .arg(expectedPage).arg(expectedRow)
                     .arg(result.page).arg(result.row));
    }
    return true;
}

bool TestBinaryViewer::failSearchBase(BinaryViewerModel *model, const QString &searchLemma, const int startPage, const int startRow, const bool searchUp )
{
    bool isOk = false;
    BinaryViewerOperationResult result;
    if(model->findOccurrences(result, searchLemma, startPage, startRow, searchUp) ) {
        if(result.errorCode == BinaryViewerOperationResult::BNO_ERROR) {
            isOk = true ;
        }
    }
    if(isOk) {
        return error(QString("Searching was expected to fail, but did not. Got %1, code %2, direction: %3 starting: page %4, row %5, found: page %6 row %7")
                     .arg(isOk).arg(result.errorCode).arg(searchUp)
                     .arg(startPage).arg(startRow)
                     .arg(result.page).arg(result.row));
    }
    return true;
}

bool TestBinaryViewer::testSearch()
{
    _testName = QString("testModel/search");
    BinaryViewerModel model;
    QByteArray data = buildSearchBuffer();
    QBuffer *ioBuffer = new QBuffer(&data);
    BinaryViewerModel::ErrCodes code = model.setFile(ioBuffer);
    if( BinaryViewerModel::EC_NOERROR != code ) {
        return error(QString("Setting the buffer data, expecting:%1, found:%2").arg(BinaryViewerModel::EC_NOERROR).arg(code));
    }
    QString searchLemma(SEARCH_LEMMA);

    if(!model.setCodecByName("ISO-8859-1")) {
        return error("Unable to set the codec for the string");
    }
    if(!searchBase(&model, searchLemma, 0, 0, 0, 1, true)) {
        return false;
    }
    if(!searchBase(&model, searchLemma, 0, 1, 0, 2, true)) {
        return false;
    }
    if(!searchBase(&model, searchLemma, 0, 2, 1, 0, true)) {
        return false;
    }
    if(!failSearchBase(&model, searchLemma, 1, 0, true)) {
        return false;
    }
    if(!searchBase(&model, searchLemma, 1, 0, 0, 2, false)) {
        return false;
    }
    if(!searchBase(&model, searchLemma, 0, 2, 0, 1, false)) {
        return false;
    }
    if(!failSearchBase(&model, searchLemma, 0, 1, false)) {
        return false;
    }
    return true;
}

void TestBinaryViewer::insertSearchInBufferLatin1(QByteArray *data, const int page, const int row)
{
    char *charArray = data->data();
    QString lemma(SEARCH_LEMMA);
    for( int i = 0 ; i < lemma.length() ; i ++ ) {
        charArray[page*BinaryViewerModel::SizeOfPage+row*BinaryBlock::SizeOfRow+i] = (lemma.at(i)).toLatin1() & 0x00FF;
    }
}


QByteArray TestBinaryViewer::buildSearchBuffer()
{
    int len =  BinaryViewerModel::SizeOfPage+3*BinaryBlock::SizeOfRow;
    QByteArray twoPages(len, '0');
    insertSearchInBufferLatin1(&twoPages, 0, 1);
    insertSearchInBufferLatin1(&twoPages, 0, 2);
    insertSearchInBufferLatin1(&twoPages, 1, 0);
    return twoPages ;
}

//-------------------------------- page offset ---------------------------------------------------

bool TestBinaryViewer::testPageSingleOffset( BinaryViewerModel *model, const qint64 position, const int expectedPage, const int expectedRow)
{
    BinaryViewerOperationResult result;
    if( model->findPageOfAddress(result, position) ) {
        if(!result.ok) {
            return error(QString("find page, operation failed for address:%1").arg(position));
        } else if( (expectedRow!=result.row) || (expectedPage!=result.page) ) {
            return error(QString("find page, result for address:%1, expected page:%2, row:%3, found page:%4, row:%5")
                         .arg(position)
                         .arg(expectedPage).arg(expectedRow)
                         .arg(result.page).arg(result.row));
        }

    } else {
        return error(QString("find page, operation returned false for address:%1").arg(position));
    }
    return true;
}

bool TestBinaryViewer::testPageOffset()
{
    _testName = "testPageOffset";
    BinaryViewerModel model;
    QByteArray twoPagesAndHalf = buildTwoPages();
    QBuffer *ioBuffer = new QBuffer(&twoPagesAndHalf);
    model.setFile(ioBuffer);
    if( !testPageSingleOffset( &model, 0, 0, 0)){
        return false;
    }
    if( !testPageSingleOffset( &model, 1, 0, 0)){
        return false;
    }
    if( !testPageSingleOffset( &model, BinaryBlock::SizeOfRow, 0, 1)){
        return false;
    }
    if( !testPageSingleOffset( &model, BinaryViewerModel::SizeOfPage, 1, 0)){
        return false;
    }
    if( !testPageSingleOffset( &model, BinaryViewerModel::SizeOfPage+2*BinaryBlock::SizeOfRow, 1, 2)){
        return false;
    }
    return true;
}

//-------------------------------- base ---------------------------------------------------

bool TestBinaryViewer::unitTests()
{
    _testName = "TestBinaryViewer::unitTests";
    BinaryViewerModel model;

    if(!testRows(&model)) {
        return false;
    }
    if(!testSearch()) {
        return false;
    }
    if(!testPageOffset()) {
        return false;
    }
    return true;
}


//-------------------------------- functional ---------------------------------------------------

bool TestBinaryViewer::testFunctionalPages()
{
    _testName = "testFunctionalPages";
    QByteArray arrayData = buildTwoPages();
    QBuffer *ioBuffer = new QBuffer(&arrayData);
    TestBVD dialog;
    QAbstractButton *btnNextPage = dialog.findChild<QPushButton*>("cmdPageNext");
    QAbstractButton *btnPrevPage = dialog.findChild<QPushButton*>("cmdPagePrev");
    dialog.setTestData(ioBuffer);

    QString data = dialog.firstRow();
    if( "0" != data ) {
        return error(QString( "Data offset found:'%1', expected :0").arg(data));
    }

    if( btnPrevPage->isEnabled()) {
        return error("PagePrev is enabled on start");
    }
    if( !btnNextPage->isEnabled()) {
        return error("PageNext is disabled on start");
    }
    btnNextPage->click();
    data = dialog.firstRow();
    if( "262144" != data ) {
        return error(QString( "Data offset found:'%1', expected :262.144").arg(data));
    }
    if( !btnPrevPage->isEnabled()) {
        return error("PagePrev is disabled on page 1");
    }
    if( btnNextPage->isEnabled()) {
        return error("PageNext is enabled on page 1");
    }
    btnPrevPage->click();
    data = dialog.firstRow();
    if( "0" != data ) {
        return error(QString( "Data offset found:'%1', expected :0").arg(data));
    }
    if( btnPrevPage->isEnabled()) {
        return error("PagePrev is enabled on page 0");
    }
    if( !btnNextPage->isEnabled()) {
        return error("PageNext is disabled on page 0");
    }
    return true;
}

bool TestBinaryViewer::testFunctionalGotoOffset()
{
    _testName = "testFunctionalGotoOffset";
    QByteArray arrayData = buildTwoPages();
    QBuffer *ioBuffer = new QBuffer(&arrayData);
    TestBVD dialog;
    QAbstractButton *btnGoto = dialog.findChild<QPushButton*>("cmdGoToAddress");
    QLineEdit *text = dialog.findChild<QLineEdit*>("gotoAddress");
    dialog.setTestData(ioBuffer);

    text->setText("32");
    btnGoto->click();

    QString data = dialog.selectedRow();

    if( "32" != data ) {
        return error(QString( "Data offset found:'%1', expected :32").arg(data));
    }
    return true;
}

bool TestBinaryViewer::testFunctionalSearch()
{
    _testName = "testFunctionalSearch";
    QByteArray arrayData = buildSearchBuffer();
    QBuffer *ioBuffer = new QBuffer(&arrayData);
    TestBVD dialog;
    QAbstractButton *btnGo = dialog.findChild<QPushButton*>("cmdSearchNext");
    QLineEdit *searchBox = dialog.findChild<QLineEdit*>("searchBox");
    dialog.setTestData(ioBuffer);

    searchBox->setText(SEARCH_LEMMA);
    btnGo->click();

    QString data = dialog.selectedRow();

    if( "16" != data ) {
        return error(QString( "Data offset found:'%1', expected :16").arg(data));
    }
    return true;
}


bool TestBinaryViewer::functionalTests()
{
    _testName = "TestBinaryViewer::functionalTests";

    if(!testFunctionalPages()) {
        return false;
    }

    if(!testFunctionalSearch()) {
        return false;
    }

    if(!testFunctionalGotoOffset()) {
        return false;
    }
    return true;

}

//----------------------------------------------------------------------------------------------------------------------------

