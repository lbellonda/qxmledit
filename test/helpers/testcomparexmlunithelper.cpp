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


#include "testcomparexmlunithelper.h"
#include "ui_comparemodule.h"

//--------------------------

TreeItemInfo::TreeItemInfo(int newPrevDiff, int newCurrDiff )
{
    currDiff = newCurrDiff;
    prevDiff = newPrevDiff;
}

TreeItemInfo::~TreeItemInfo()
{
}

//--------------------------

DiffListInfoHolder::DiffListInfoHolder()
{
}

DiffListInfoHolder:: ~DiffListInfoHolder()
{
    foreach( TreeItemInfo* item, compareItems ) {
        delete item;
    }
    foreach( DiffInfo* item, infos ) {
        delete item;
    }
}

//--------------------------

RegolaHolder::RegolaHolder()
{
    ref = NULL ;
    source = NULL ;
}

RegolaHolder::~RegolaHolder()
{
    if( NULL != ref ) {
        delete ref ;
    }
    if( NULL != source ) {
        delete source ;
    }
}

bool RegolaHolder::isOk()
{
    if( ( NULL != ref) && (NULL != source) ) {
        return true ;
    }
    return false;
}


//--------------------------

QString DModule::defaultFileName("/a/b/c");

DModule::DModule(QWidget *parent, Regola *original, UIDelegate *uiDelegate, QStringList files ) : CompareModule(parent, true, original, uiDelegate, files)
{
}

void DModule::testView(DiffNodesChangeList *diffNodes)
{
    clearResultData();
    showAnalyticView(diffNodes);
}

bool DModule::loadAFileForTest( const QString &filePath)
{
    return loadFile( _compareFile, filePath);
}

Regola *DModule::loadTestRegola(const QString &fileName)
{
    return loadRegola(fileName);
}

void DModule::setCompareOptions( const bool isCompareText, const bool isCompareComments )
{
    _options.setCompareComments(isCompareComments);
    _options.setCompareText(isCompareText);
}

void DModule::startCompare( Regola *regola1, Regola *regola2 )
{
    CompareModule::startCompare(regola1, regola2);
}

bool DModule::areEquals()
{
    return _diffList->isReferenceEqualToCompare();
}

QList<QTreeWidgetItem*> &DModule::referenceItemsForTest()
{
    return referenceItems();
}

QList<DiffInfo*> &DModule::diffInfos()
{
    return _infoList;
}

void DModule::selectItem(const int itemToSelect)
{
    QTreeWidgetItem *item = _itemsLeft.at(itemToSelect);
    ui->treeLeft->setCurrentItem(item);
}

bool DModule::isFirstDiffButtonEnabled()
{
    return ui->gotoFirst->isEnabled();
}

bool DModule::isPrevDiffButtonEnabled()
{
    return ui->gotoPrev->isEnabled();
}

bool DModule::isNextDiffButtonEnabled()
{
    return ui->gotoNext->isEnabled();
}

bool DModule::isLastDiffButtonEnabled()
{
    return ui->gotoLast->isEnabled();
}

void DModule::selectDiff(const int numSel)
{
    return ui->differenceTable->selectRow(numSel);
}

int DModule::selectedDiff()
{
    return ui->differenceTable->currentIndex().row();
}

void DModule::moveToFirst()
{
    ui->gotoFirst->click();
}

void DModule::moveToNext()
{
    ui->gotoNext->click();
}

void DModule::moveToPrev()
{
    ui->gotoPrev->click();
}

void DModule::moveToLast()
{
    ui->gotoLast->click();
}


//--------------------------


TestCompareXmlUnitHelper::TestCompareXmlUnitHelper() : dModule( NULL, &_dummyRule, &uiDelegate, files)
{
    _dummyRule.setFileName(DModule::defaultFileName);

    leftItemD = NULL ;
    rightItemD = NULL ;

}

TestCompareXmlUnitHelper::~TestCompareXmlUnitHelper()
{
    deallocateResources();
}

void TestCompareXmlUnitHelper::deallocateResources()
{
    foreach( Element* element, resources ) {
        if( NULL != element ) {
            delete element;
        }
    }
    resources.clear();
}


void TestCompareXmlUnitHelper::startTest(const EDiff::KDiff diffType, Element::ElType type, const QString &texta1, const QString &texta2, const QString &textb1, const QString &textb2, const bool isCData )
{
    //-- test for equals elements
    DiffNodesChangeList *diffList = getNewChangeList(diffType, type, resources, texta1, texta2, textb1, textb2, isCData );
    dModule.testView(diffList);
    leftItemD = dModule.testLeftTree()->topLevelItem(0);
    rightItemD = dModule.testRightTree()->topLevelItem(0);
    delete diffList ;
}


Element *TestCompareXmlUnitHelper::newElement(Element::ElType type, QList<Element*> &resources, const QString &text, const QString &text2, const bool isCData )
{
    Element *element = new Element(&_dummyRule, type, NULL);
    resources.append(element);
    switch( type ) {
    case Element::ET_ELEMENT:
        element->_tag = text;
        break;
    case Element::ET_PROCESSING_INSTRUCTION:
        element->setPIData(text2);
        element->setPITarget(text);
        break;
    case Element::ET_COMMENT:
        element->setComment(text);
    case Element::ET_TEXT:
        element->setTextOfTextNode(text, isCData );
        break;
    default:
        return NULL ;
    }
    return element;
}

DiffSingleNodeResult *TestCompareXmlUnitHelper::getNewCompare(const EDiff::KDiff diffType, Element::ElType type, QList<Element*>&resources, const QString &texta1, const QString &texta2, const QString &textb1, const QString &textb2, const bool isCData )
{
    Element *referenceElement = newElement( type, resources, texta1, texta2, isCData );
    Element *compareElement = newElement( type, resources, textb1, textb2, isCData );

    DiffSingleNodeResult *result = NULL ;
    switch( diffType ) {
    case EDiff::ED_EQUAL:
        result = new EqualsDiffNodeResult(new SourceElementDiffOperation(referenceElement), new SourceElementDiffOperation(compareElement));
        break;
    case EDiff::ED_ADDED:
        result = new AddedDiffNodeResult(new SourceElementDiffOperation(referenceElement));
        break;
    case EDiff::ED_MODIFIED:
        result = new ModifiedDiffNodeResult(new SourceElementDiffOperation(referenceElement), new SourceElementDiffOperation(compareElement));
        break;
    case EDiff::ED_DELETED:
        result = new DeletedDiffNodeResult(new SourceElementDiffOperation(compareElement));
        break;
    default:
        return NULL ;
    }
    return result;
}

DiffNodesChangeList *TestCompareXmlUnitHelper::getNewChangeList(const EDiff::KDiff diffType, Element::ElType type, QList<Element*> &resources, const QString &texta1, const QString &texta2, const QString &textb1, const QString &textb2, const bool isCData )
{
    DiffNodesChangeList *diffList = new DiffNodesChangeList();
    DiffSingleNodeResult *node = getNewCompare(diffType, type, resources, texta1, texta2, textb1, textb2, isCData );
    diffList->addToRootLevel(node);

    return diffList;
}

//--------------------------
