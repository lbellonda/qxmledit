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


#ifndef TESTXSDVIEW_H
#define TESTXSDVIEW_H

#include "testbase.h"
#include "app.h"
#include "testxsdnavigation.h"

class QGraphicsItem ;

class TestXSDView : public TestBase
{
    bool doTest(const QString &inputFilePath, const QString &resultFilePath, const QList<int> &selectionPath);
    bool openXsdViewer(MainWindow *window, TestXSDWindow *xsdEditor);
    bool openXsdViewerOutlineMode(MainWindow *window, TestXSDWindow *xsdEditor);

    bool testBaseElement();
    bool testRefElement();

    bool testSimpleType();
    bool testElementInf();
    bool testTypeWithElementsRef();
    bool testReferenceElement();
    bool testReferenceAttribute();
    bool testReferenceAttributeGroup();
    bool testAttribute();
    bool testAttributeGroup();
    bool testGroups();
    bool testReferenceGroups();
    bool testComplexExtension();
    bool testComplexRestriction();
    bool loadAndCompareOutline(const QString &id, const QString &inputFilePath, const QString &fileReference);
    bool compareOutlineStructure(XSDItem *item, Element *xml, const QString &path);
    bool testPrintPagination();
    bool testPrintCheckHTML();
    bool testPrintHTML();
    bool loadAndComparePrint(const QString &id, const QString &inputFilePath, const QString &fileReference);
    bool testUnitPrint();
    bool testUnitHTML();
    bool cleanHTML(const QString &html, QString &result);
    QString cleanComment(const QString &text, const QString& commentStart, const QString& commentEnd);
    QString cleanAttribute(const QString &html, const QString& searchTerm, bool &isChanged);
    bool testArgsDotViz();
    bool testPathDotViz();
    bool checkSettingsAndExpected(App &app, const QString &msg, const bool useDefault, const QString &path, const QString &expectedPath);
    bool testForImage(const QString &id, App &app, const bool isEmbedded, const bool isDotViz);
    bool testImagesExtInt();
    bool testItemsIntersectLine();
    bool testItemsSingleIntersectLine( const QString &key, const XSDItem::EIntersectType expected, const QRectF &candidate, const QRectF &source );
    bool testItemsSplit();
    bool testFacetsInReport();
    bool testFacetsInReport2();
    bool testViewItems();
    void getItemInfo(QList<ItemInfoDimensions*> &infos);
    bool testItemBaseline();
    bool testItemDisposition();
    bool checkItem(QList<ItemInfoDimensions*> infos, const int index, const qreal posY);
    //void debugFile(const QString &data);

    QList <QGraphicsItem*> _gi;
public:
    TestXSDView();
    ~TestXSDView();

    bool testBase();
    bool testElement();
    bool testType();
    bool testReference();
    bool testExtension();
    bool testRestriction();
    bool testFast();
    bool testOutline();
    bool testPrint();
    bool testUnit();
};

#endif // TESTXSDVIEW_H
