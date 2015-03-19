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

#ifndef TESTBINARYVIEWER_H
#define TESTBINARYVIEWER_H

#include "testbase.h"
#include <QByteArray>
#include <QBuffer>

#include "modules/binaryviewer/binaryviewermodel.h"
#include "modules/binaryviewer/binaryviewerdialog.h"


class BinaryTestData {
public:
    QString name;
    int page;     // 0: no move, >0 goto page, <0 abs(page), then next or prev
    bool moveNextPage;
    int expectedPage;

    int isFirstPage;
    int isLastPage;
    int numRows;
    int offsetOfDataRow;
    QString displayDataOfRow;
    QString offsetDataOfRow;

    BinaryTestData();
    BinaryTestData(const QString &newName, int newPage, bool moveNextPage,
                   int newExpectedPage, int newIsFirstPage, int newIsLastPage, int newNumRows, int newOffsetOfDataRow,
                   const QString &newDisplayDataOfRow, const QString &newOffsetDataOfRow );
    ~BinaryTestData();
};

class BinaryTestDataPacket {
    // tests:
    // 1- start
    // 2- next page
    // 3- prev page
    // 4- goto page
    // 5- goto non existent page
public:
    // !! not destroyed here
    QByteArray *dataToUse;
    QString name;
    BinaryTestData startTest;
    BinaryTestData nextPage;
    BinaryTestData prevPage;
    BinaryTestData gotoPage;
    BinaryTestData nePage;
    //---
    BinaryTestDataPacket(const QString &newName, QByteArray *newDataToUse);
};

class TestBinaryViewer : public TestBase
{

    bool testModel(const QString &name, BinaryViewerModel *model, BinaryTestData *test, QByteArray *data);
    bool testModel(const QString &name, BinaryTestData *test , QByteArray *data);

    bool searchBase(BinaryViewerModel *model, const QString &searchLemma, const int startPage, const int startRow, const int expectedPage, const int expectedRow, const bool searchUp );
    bool failSearchBase(BinaryViewerModel *model, const QString &searchLemma, const int startPage, const int startRow, const bool searchUp );

    void insertSearchInBufferLatin1(QByteArray *data, const int page, const int row);

    bool testModelPacket( BinaryViewerModel *model, BinaryTestDataPacket *packet );
    bool testNoModelPacket( BinaryTestDataPacket *packet );

    bool testPageSingleOffset( BinaryViewerModel *model, const qint64 position, const int expectedPage, const int expectedRow);

    QByteArray buildTwoPages();
    QByteArray buildSearchBuffer();

    bool testRows(BinaryViewerModel *model);
    bool testSearch();
    bool testPageOffset();

    bool testFunctionalPages();
    bool testFunctionalSearch();
    bool testFunctionalGotoOffset();

public:
    TestBinaryViewer();
    ~TestBinaryViewer();

    bool unitTests();
    bool functionalTests();
};



#endif // TESTBINARYVIEWER_H
