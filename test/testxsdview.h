/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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
};

#endif // TESTXSDVIEW_H
