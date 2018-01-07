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

#ifndef TESTXSDPRINTINFO_H
#define TESTXSDPRINTINFO_H

#include "xsdeditor/xsdwindow.h"

class TestXSDPrintInfo : public XSDPrintInfo
{
public:
    QList<int> pages;
    QList<int> expected;
    QList<QRectF> blocks;
    int docSize;
    QString errorMessage;
    QString stepName;

    bool checkResults();
    TestXSDPrintInfo();
    ~TestXSDPrintInfo();
    bool testAll();
    bool test1();
    bool test2();
    bool test3();
    bool test4();
    bool test5();
    bool test6();
    virtual void newPage();
    virtual void updatePageHeight(const int heightInPage);
    bool blockIsValid(QTextBlock block, const int blockIndex);
    QRectF blockBoundingBox(QAbstractTextDocumentLayout *layout, QTextBlock block, const int blockIndex);
    void resetForTest(const QString step);
    virtual void setupDocument(QTextDocument &document);
    virtual QSizeF documentSize(QTextDocument &document);
};

#endif // TESTXSDPRINTINFO_H
