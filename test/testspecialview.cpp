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

#include "testspecialview.h"

#define BASE_DATA  TEST_BASE_DATA "view/"

#define FILE_1  BASE_DATA "one.xml"
#define FILE_2  BASE_DATA "two.xml"

TestSpecialView::TestSpecialView()
{

}

TestSpecialView::~TestSpecialView()
{

}

bool TestSpecialView::testFast()
{
    return testOpenClose();
}

bool TestSpecialView::loadFile(App &app, const QString &inputFileName)
{
    if(!app.init() ) {
        return error("init app");
    }
    if(!app.mainWindow()->loadFile(inputFileName) ) {
        return error(QString("Error loading file:%1").arg(inputFileName));
    }
    return true;
}

bool TestSpecialView::selectPath(App &app, QList<int> &sel)
{
    QTreeWidget *tree = app.mainWindow()->getMainTreeWidget();
    Element *foundElm = app.mainWindow()->getRegola()->findElementByArray(sel);
    if( NULL != foundElm ) {
        tree->setCurrentItem(foundElm->getUI());
        return true;
    } else {
        return error("Selection not found");
    }
}

bool TestSpecialView::testUnit()
{
    _testName = "testUnit";
    App app;
    if(!loadFile(app, FILE_1)) {
        return false;
    }
    Regola *regola = app.mainWindow()->getRegola();
    regola->expand(app.mainWindow()->getEditor()->getMainTreeWidget());
    QList<int> pos;
    pos << 0 << 1 << 1 << 0 ;
    Element *selection = app.mainWindow()->getRegola()->findElementByArray(pos);
    if( NULL != selection ) {
        app.mainWindow()->getEditor()->getMainTreeWidget()->setCurrentItem(selection->getUI());
    } else {
        return error("NULL selection");
    }
    app.mainWindow()->getEditor()->openSiblingsSameLevel();
    // verify brute force approach ;
    if(!checkOpenRecursive(app.mainWindow()->getEditor()->getMainTreeWidget())) {
        return false;
    }
    return true;
}

bool TestSpecialView::checkOpenRecursive(QTreeWidget *tree)
{
    int count = tree->topLevelItemCount();
    FORINT(index, count) {
        QTreeWidgetItem *item = tree->topLevelItem(index);
        Element *element = Element::fromItemData(item);
        if(element->isElement()) {
            if((element->getAttributeValue("o") == "close") && item->isExpanded() ) {
                return error(QString("Element %1 expected close, but expanded").arg(element->pathString()));
            }
            if((element->getAttributeValue("o") == "open") && !item->isExpanded() ) {
                return error(QString("Element %1 expected open, but not expanded").arg(element->pathString()));
            }
            if(!checkOpenRecursive(item)) {
                return false;
            }
        }
    }
    return true ;
}

bool TestSpecialView::checkOpenRecursive(QTreeWidgetItem *parent)
{
    int count = parent->childCount();
    FORINT(index, count) {
        QTreeWidgetItem *item = parent->child(index);
        Element *element = Element::fromItemData(item);
        if(element->isElement()) {
            if((element->getAttributeValue("o") == "close") && item->isExpanded() ) {
                return error(QString("Element %1 expected close, but expanded").arg(element->pathString()));
            }
            if((element->getAttributeValue("o") == "open") && !item->isExpanded() ) {
                return error(QString("Element %1 expected open, but not expanded").arg(element->pathString()));
            }
            if(!checkOpenRecursive(item)) {
                return false;
            }
        }
    }
    return true ;
}

bool TestSpecialView::checkSameOpeningState(QTreeWidgetItem *parent, const bool expectedExpanding)
{
    int count = parent->childCount();
    FORINT(index, count) {
        QTreeWidgetItem *item = parent->child(index);
        Element *element = Element::fromItemData(item);
        if(element->isElement()) {
            if(expectedExpanding != item->isExpanded() ) {
                return error(QString("Element %1 expected expansion %2, but not verified").arg(element->pathString()).arg(expectedExpanding));
            }
            if(!checkSameOpeningState(item, expectedExpanding)) {
                return false;
            }
        }
    }
    return true ;
}

bool TestSpecialView::testOpenClose()
{
    _testName = "testOpenClose" ;
    if(!testCloseAll()) {
        return false;
    }
    if(!testCloseAllBranch()) {
        return false;
    }
    return true ;
}

bool TestSpecialView::loadFileAndSelectItem(App &app, const QString &fileName, QList<int> selectionPath)
{
    if(!loadFile(app, fileName)) {
        return false;
    }
    Regola *regola = app.mainWindow()->getRegola();
    regola->expand(app.mainWindow()->getEditor()->getMainTreeWidget());
    if(NULL == selectAnItem(app, selectionPath)) {
        return error("NULL selection");
    }
    return true;
}

bool TestSpecialView::testCloseAll()
{
    _testName = "testCloseAll";
    App app;
    QList<int> selectionPath;
    selectionPath << 0 ;
    if(!loadFileAndSelectItem(app, FILE_2, selectionPath) ) {
        return false;
    }
    app.mainWindow()->getEditor()->closeItemAndChildren();
    Element *root = app.mainWindow()->getRegola()->findElementByArray(selectionPath);
    if( NULL == root ) {
        return error("Unable to find element 0");
    }
    // verify brute force approach ;
    if(!checkSameOpeningState(root->getUI(), false)) {
        return false;
    }
    return true;
}

bool TestSpecialView::testCloseAllBranch()
{
    _testName = "testCloseAllBranch";
    App app;
    QList<int> selectionPath0;
    selectionPath0 << 0 << 0 ;
    if(!loadFileAndSelectItem(app, FILE_2, selectionPath0) ) {
        return false;
    }
    app.mainWindow()->getEditor()->closeItemAndChildren();
    // verify brute force approach ;
    Element *expectedClose = app.mainWindow()->getRegola()->findElementByArray(selectionPath0);
    if(NULL == expectedClose) {
        return error("Unable to find first element");
    }
    if(!checkSameOpeningState(expectedClose->getUI(), false)) {
        return false;
    }
    QList<int> selectionPath1 ;
    selectionPath1 << 0 << 1;
    Element *expectedOpen = app.mainWindow()->getRegola()->findElementByArray(selectionPath1);
    if(NULL == expectedOpen) {
        return error("Unable to find second element");
    }
    if(!checkSameOpeningState(expectedOpen->getUI(), true)) {
        return false;
    }
    return true;
}
