/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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


#ifndef TESTCOMPAREXMLUNITHELPER_H
#define TESTCOMPAREXMLUNITHELPER_H


#include "modules/compare/comparemodule.h"
#include "../fakeuidelegate.h"


class DModule : public CompareModule {

public:

    static QString defaultFileName;

    explicit DModule(QWidget *parent, Regola *original, UIDelegate *uiDelegate, QStringList files );
    void testView(DiffNodesChangeList *diffNodes);

    QTreeWidget *testLeftTree(){return leftTree();}
    QTreeWidget *testRightTree(){return rightTree();}

    bool loadAFileForTest( const QString &filePath);
    Regola *loadTestRegola(const QString &fileName);
    void setCompareOptions( const bool isCompareText, const bool isCompareComments );
    bool areEquals();
    void startCompare( Regola *regola1, Regola *regola2 );

    QList<QTreeWidgetItem*> &referenceItemsForTest();
    QList<DiffInfo*> &diffInfos();

    void selectItem(const int itemToSelect);
    bool isFirstDiffButtonEnabled();
    bool isPrevDiffButtonEnabled();
    bool isNextDiffButtonEnabled();
    bool isLastDiffButtonEnabled();

    void selectDiff(const int numSel);
    int selectedDiff();
    void moveToFirst();
    void moveToNext();
    void moveToPrev();
    void moveToLast();

};

class TreeItemInfo {
public:
    int prevDiff;
    int currDiff;
    TreeItemInfo(int newPrevDiff, int newCurrDiff );
    ~TreeItemInfo();
};

class DiffListInfoHolder {

public:
    QList<TreeItemInfo*> compareItems ;
    QList<DiffInfo*> infos ;

    DiffListInfoHolder();
    ~DiffListInfoHolder();
};


class RegolaHolder {
public:
    Regola *ref;
    Regola *source;

    RegolaHolder();
    ~RegolaHolder();

    bool isOk();
};

class TestCompareXmlUnitHelper {
public:
    QList<Element*> resources;
    FakeUIDelegate uiDelegate;
    // this order is important for the constructor
    QStringList files;
    // this order is important for the constructor
    Regola _dummyRule;
    // this order is important for the constructor
    DModule dModule;
    QTreeWidgetItem *leftItemD ;
    QTreeWidgetItem *rightItemD ;

    TestCompareXmlUnitHelper();
    ~TestCompareXmlUnitHelper();

    void startTest(const EDiff::KDiff diffType, Element::ElType type, const QString &texta1, const QString &texta2, const QString &textb1, const QString &textb2, const bool isCData );
    void deallocateResources();
    Element *newElement(Element::ElType type, QList<Element*> &resources, const QString &text, const QString &text2, const bool isCData );
    DiffSingleNodeResult *getNewCompare(const EDiff::KDiff diffType, Element::ElType type, QList<Element*>&resources, const QString &texta1, const QString &texta2, const QString &textb1, const QString &textb2, const bool isCData  );
    DiffNodesChangeList *getNewChangeList(const EDiff::KDiff diffType, Element::ElType type, QList<Element*> &resources, const QString &texta1, const QString &texta2, const QString &textb1, const QString &textb2, const bool isCData );
};


#endif // TESTCOMPAREXMLUNITHELPER_H
