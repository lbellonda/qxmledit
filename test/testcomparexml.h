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

#ifndef TESTCOMPAREXML_H
#define TESTCOMPAREXML_H

#include "testbase.h"
#include "helpers/testcomparexmlunithelper.h"

class TestCompareXml : public TestBase
{

    bool compareQuick(OperationResult *results, Regola *one, Regola *two, const bool isDenormalizeEOL);
    Regola *loadRegola(QByteArray *dataIn);
    bool testOptionDenormalizeImpl(const bool isDenormalizeEOL, const bool expectedResult);
    bool testOptionDenormalize();

    bool testWithOptions(const QString &name, const bool isCompareText, const bool isCompareComments, const bool expected, const QString &file2);
    bool testWithAllOptions( const QString &name, const bool *expected, const QString &file2);
    bool testWithOptionsConditions();
    bool compareInfoItems(const QString &name, QList<DiffInfo*> &items, QList<DiffInfo*> &compare);
    bool testCompareDifferences(const QString &name, const QString &file1, const QString &file2, QList<TreeItemInfo*> &compareItems, QList<DiffInfo*> &diffInfos );
    bool compareTreeItems(const QString &name, QList<QTreeWidgetItem*> &items, QList<TreeItemInfo*> &compareItems );
    bool testCompareUI();
    bool checkSelectItemDiff( DModule *module, const int itemToSelect,
                                              const bool expectedFirstDiff, const bool expectedPrevDiff,
                                              const bool expectedNextDiff, const bool expectedLastDiff );


    bool testAddedBase(const EDiff::KDiff diffType, Element::ElType type, const QString &texta1, const QString &texta2, const QString &textb1, const QString &textb2, const QString &expectedTextL, const QString &expectedTextR, const bool isCData =false );
    bool testDeletedBase(const EDiff::KDiff diffType, Element::ElType type, const QString &texta1, const QString &texta2, const QString &textb1, const QString &textb2, const QString &expectedTextL, const QString &expectedTextR, const bool isCData=false  );
    bool testModifyBase(const EDiff::KDiff diffType, Element::ElType type, const QString &texta1, const QString &texta2, const QString &textb1, const QString &textb2, const QString &expectedTextL, const QString &expectedTextR, const bool isCData=false  );
    bool testEqualsBase(const EDiff::KDiff diffType, Element::ElType type, const QString &texta1, const QString &texta2, const QString &textb1, const QString &textb2, const QString &expectedTextL, const QString &expectedTextR, const bool isCData =false );
    bool testUnitBase(const EDiff::KDiff diffType, Element::ElType type, const QString &texta1, const QString &texta2, const QString &textb1, const QString &textb2, const QString &expectedTextL, const QString &expectedTextR, const bool isCData, const QColor colorLeft, const QColor colorRight );
    //---------
    bool testEqualsElements();
    bool testEqualsComment();
    bool testEqualsPI();
    bool testEqualsText();
    bool testEqualsTextCData();
    //---------
    bool testAddedElements();
    bool testAddedComment();
    bool testAddedPI();
    bool testAddedText();
    bool testAddedTextCData();
    //---------
    bool testDeletedElements();
    bool testDeletedComment();
    bool testDeletedPI();
    bool testDeletedText();
    bool testDeletedTextCData();
    //---------
    bool testModifiedElements();
    bool testModifiedComment();
    bool testModifiedPI();
    bool testModifiedText();
    bool testModifiedTextCData();
    //---------

    bool testFileLoadPathEmpty();
    bool testFileLoadSameFile();
    bool testFileLoadNoSuchFile();
    bool testFileLoadFileOK();
    //---------
    bool testCompareAttributes();
    bool testCompareElements();
    bool testCompareDifferenceList();
    //---------
    bool testElemWithAttributeAdd();
    bool testElemWithAttributeMod();
    bool testElemWithAttributeEq();
    bool testElemWithAttributeDel();
    bool testElemWithAttributeAddEq();
    bool testElemWithAttributeAddDel();
    bool testElemWithAttributeEqDel();
    bool testElemWithAttributeAddMod();
    bool testElemWithAttributeDelMod();
    bool testElemWithAttributeEqMod();
    bool testElemWithAllAttributes();
    //-----------------------
    bool testElemEq();
    bool testElemAdd();
    bool testElemDel();
    bool testElemMod();
    bool testElemWithAddDel();
    bool testElemWithAddEq();
    bool testElemWithAddMod();
    bool testElemWithEqMod();
    bool testElemWithEqDel();
    bool testElemWithAllElementsAndTypes();


    bool testUnitBase();

    Element *newElement(Element::ElType type, QList<Element*> &resources, const QString text, const QString text2);
    DiffSingleNodeResult *getNewCompare(const EDiff::KDiff diffType, Element::ElType type, QList<Element*> &resources, const QString texta1, const QString texta2, const QString textb1, const QString textb2 );
    void deallocateResources(QList<Element*> &resources);

    bool ckeckItem(QTreeWidgetItem *item, const QString &text, const QColor expextedColor);

    bool helperTestFileLoad( const bool isExpectedError, const QString &filePath, const CompareModule::Errors error, const QString &errorText);

public:
    TestCompareXml();
    ~TestCompareXml();

    bool testUnit();
    bool testLoadFile();
    bool testResults();

};

#endif // TESTCOMPAREXML_H
