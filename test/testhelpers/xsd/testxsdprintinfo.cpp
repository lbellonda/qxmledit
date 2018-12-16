/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2017-2018 by Luca Bellonda and individual contributors  *
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

#include "testxsdprintinfo.h"

#include "utils.h"

TestXSDPrintInfo::TestXSDPrintInfo()
{
    error = false;
    docSize = 0 ;
}

TestXSDPrintInfo::~TestXSDPrintInfo()
{
}

void TestXSDPrintInfo::newPage()
{
    XSDPrintInfo::newPage() ;
    pages.append(0);
}

bool TestXSDPrintInfo::testAll()
{
    isCalculating = true;
    if(!test1()) {
        return false;
    }
    if(!test2()) {
        return false;
    }
    if(!test3()) {
        return false;
    }
    if(!test4()) {
        return false;
    }
    if(!test5()) {
        return false;
    }
    if(!test6()) {
        return false;
    }
    return true;
}

bool TestXSDPrintInfo::blockIsValid(QTextBlock /*block*/, const int blockIndex)
{
    if(blockIndex < blocks.size()) {
        return true ;
    }
    return false;
}

QRectF TestXSDPrintInfo::blockBoundingBox(QAbstractTextDocumentLayout *, QTextBlock, const int blockIndex)
{
    return blocks.at(blockIndex);
}

void TestXSDPrintInfo::setupDocument(QTextDocument &)
{
}

QSizeF TestXSDPrintInfo::documentSize(QTextDocument &)
{
    return QSize(0, docSize) ;
}

void TestXSDPrintInfo::updatePageHeight(const int heightInPage)
{
    int curPage = pages.size() - 1;
    pages.replace(curPage, heightInPage);
    XSDPrintInfo::updatePageHeight(heightInPage);
}

void TestXSDPrintInfo::resetForTest(const QString step)
{
    stepName = step ;
    isCalculating = true;
    pages.clear();
    expected.clear();
    blocks.clear();
    docSize = 0;
    pageBounds.setRect(0, 0, 10, 10);
    printingBounds.setRect(0, 0, 10, 10);
    pageBottomMargin = 0 ;
    availableHeight = 0 ;
    currentY = printingBounds.top();
}

bool TestXSDPrintInfo::test1()
{
    resetForTest("1");
    expected << 9;
    blocks << QRect(0, 0, 10, 3);
    blocks << QRect(0, 3, 10, 3);
    blocks << QRect(0, 6, 10, 3);
    docSize = 9 ;
    //
    printBox("");
    return checkResults();
}

bool TestXSDPrintInfo::test2()
{
    resetForTest("2");
    expected << 9 << 3;
    blocks << QRect(0, 0, 10, 3);
    blocks << QRect(0, 3, 10, 3);
    blocks << QRect(0, 6, 10, 3);
    blocks << QRect(0, 9, 10, 3);
    docSize = 12 ;
    //
    printBox("");
    return checkResults();
}

bool TestXSDPrintInfo::test3()
{
    resetForTest("3");
    expected << 10 << 2;
    blocks << QRect(0, 0, 0, 12);
    docSize = 12 ;
    //
    printBox("");
    return checkResults();
}

bool TestXSDPrintInfo::test4()
{
    resetForTest("4");
    expected << 10 << 10 << 2;
    blocks << QRect(0, 0, 0, 22);
    docSize = 22 ;
    //
    printBox("");
    return checkResults();
}

bool TestXSDPrintInfo::test5()
{
    resetForTest("5");
    expected << 10 << 9 << 7;
    blocks << QRect(0, 0, 0, 6);
    blocks << QRect(0, 6, 0, 4);
    blocks << QRect(0, 10, 0, 9);
    blocks << QRect(0, 19, 0, 7);
    docSize = 26 ;
    //
    printBox("");
    return checkResults();
}

bool TestXSDPrintInfo::test6()
{
    resetForTest("6");
    expected << 10 << 10 << 10 << 10;
    blocks << QRect(0, 0, 0, 10);
    blocks << QRect(0, 10, 0, 10);
    blocks << QRect(0, 20, 0, 10);
    blocks << QRect(0, 30, 0, 10);
    docSize = 40 ;
    //
    printBox("");
    return checkResults();
}

bool TestXSDPrintInfo::checkResults()
{
    if(error) {
        errorMessage = "Error raised";
        return false ;
    }
    //
    if(pages.size() != expected.size()) {
        errorMessage = QString("Expected %1 page found:%2").arg(expected.size()).arg(pages.size());
        return false;
    }
    int indexMax = pages.size();
    for(int index = 0 ; index < indexMax ; index++) {
        int expectd = expected.at(index);
        int page = pages.at(index);
        if(expectd != page) {
            errorMessage = QString("At:%1 expected:%2 page:%3").arg(index).arg(expectd).arg(page);
            return false;
        }
    }
    return true;
}

