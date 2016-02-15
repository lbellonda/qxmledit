/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016 by Luca Bellonda and individual contributors       *
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

#include "testdeletesiblings.h"
#include "app.h"
#include "comparexml.h"

#define TEST_BASE TEST_BASE_DATA "/siblings"
#define FILE_START  TEST_BASE "/delete_siblings_source.xml"
#define FILE_START1  TEST_BASE "/delete_siblings_source1.xml"
#define FILE_DELETE_ALL  TEST_BASE "/delete_siblings_all.xml"
#define FILE_DELETE_BEFORE  TEST_BASE "/delete_siblings_before.xml"
#define FILE_DELETE_AFTER TEST_BASE "/delete_siblings_after.xml"
//--
#define FILE_DELETE_ALL_FIRST  TEST_BASE "/delete_siblings_all_first.xml"
#define FILE_DELETE_BEFORE_FIRST  TEST_BASE "/delete_siblings_before_first.xml"
#define FILE_DELETE_AFTER_FIRST TEST_BASE "/delete_siblings_after_first.xml"
//-
#define FILE_DELETE_ALL_LAST  TEST_BASE "/delete_siblings_all_last.xml"
#define FILE_DELETE_BEFORE_LAST  TEST_BASE "/delete_siblings_before_last.xml"
#define FILE_DELETE_AFTER_LAST TEST_BASE "/delete_siblings_after_last.xml"
//-


TestDeleteSiblings::TestDeleteSiblings()
{
}

TestDeleteSiblings::~TestDeleteSiblings()
{
}

bool TestDeleteSiblings::testUnit()
{
    _testName = "testUnit" ;
    if(!testDeleteBefore()) {
        return false;
    }
    if(!testDeleteAfter()) {
        return false;
    }
    if(!testDeleteAll()) {
        return false;
    }
    return true ;
}


bool TestDeleteSiblings::testDeleteBefore()
{
    _testName = "testDeleteBefore" ;
    if(!testSkeleton(FILE_START, FILE_DELETE_BEFORE, RegolaDeleteSiblings::DeleteAllSiblingsBefore, selPathMiddle())) {
        return false;
    }
    if(!testSkeleton(FILE_START1, FILE_DELETE_BEFORE_FIRST, RegolaDeleteSiblings::DeleteAllSiblingsBefore, selPathFirst())) {
        return false;
    }
    if(!testSkeleton(FILE_START1, FILE_DELETE_BEFORE_LAST, RegolaDeleteSiblings::DeleteAllSiblingsBefore, selPathLast())) {
        return false;
    }
    return true ;
}

bool TestDeleteSiblings::testDeleteAfter()
{
    _testName = "testDeleteAfter" ;
    if(!testSkeleton(FILE_START, FILE_DELETE_AFTER, RegolaDeleteSiblings::DeleteAllSiblingsAfter, selPathMiddle())) {
        return false;
    }
    if(!testSkeleton(FILE_START1, FILE_DELETE_AFTER_FIRST, RegolaDeleteSiblings::DeleteAllSiblingsAfter, selPathFirst())) {
        return false;
    }
    if(!testSkeleton(FILE_START1, FILE_DELETE_AFTER_LAST, RegolaDeleteSiblings::DeleteAllSiblingsAfter, selPathLast())) {
        return false;
    }
    return true;
}

bool TestDeleteSiblings::testDeleteAll()
{
    _testName = "testDeleteAll" ;
    if(!testSkeleton(FILE_START, FILE_DELETE_ALL, RegolaDeleteSiblings::DeleteAllSiblings, selPathMiddle())) {
        return false;
    }
    if(!testSkeleton(FILE_START1, FILE_DELETE_ALL_FIRST, RegolaDeleteSiblings::DeleteAllSiblings, selPathFirst())) {
        return false;
    }
    if(!testSkeleton(FILE_START1, FILE_DELETE_ALL_LAST, RegolaDeleteSiblings::DeleteAllSiblings, selPathLast())) {
        return false;
    }
    return true;
}

QList<int> TestDeleteSiblings::selPathMiddle()
{
    QList<int> selPath;
    selPath << 1 << 4 ;
    return selPath ;
}

QList<int> TestDeleteSiblings::selPathFirst()
{
    QList<int> selPath;
    selPath << 1 << 0 ;
    return selPath ;
}

QList<int> TestDeleteSiblings::selPathLast()
{
    QList<int> selPath;
    selPath << 1 << 9 ;
    return selPath ;
}

bool TestDeleteSiblings::testSkeleton(const QString &fileStart, const QString &fileResult,
                                         const RegolaDeleteSiblings::DeleteOptions option, QList<int> selPath )
{
    CompareXML compare;
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(fileStart) ) {
        return error(QString("unable to load input file: '%1' ").arg(fileStart));
    }
    Regola *regola = app.mainWindow()->getRegola();
    Element *selectedElement = NULL ;
    selectedElement = app.mainWindow()->getRegola()->findElementByArray(selPath);

    if(NULL == selectedElement) {
        return error("no element selected");
    }
    app.mainWindow()->getEditor()->setCurrentItem(selectedElement);

    app.mainWindow()->getEditor()->deleteSiblings(option, selectedElement);

    if(!compare.compareAFileWithRegola( fileResult, regola )) {
        return false;
    }
    regola->undo();
    if(!compare.compareAFileWithRegola( fileStart, regola )) {
        return false;
    }
    regola->redo();
    if(!compare.compareAFileWithRegola( fileResult, regola )) {
        return false;
    }
    regola->undo();
    regola->redo();
    regola->undo();
    regola->redo();
    regola->undo();
    regola->redo();
    if(!compare.compareAFileWithRegola( fileResult, regola )) {
        return false;
    }
    return true;
}

