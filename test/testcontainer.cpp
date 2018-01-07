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


#include "testcontainer.h"
#include "app.h"
#include "helpers/compareregolawitheditor.h"
#include "comparexml.h"

#define _NEGATIVE_QXMLEDIT_TEST 1

#define INPUT_FILE  "../test/data/container/input_1.xml"
#define RESULT_INSERT_CHILDCONTAINER  "../test/data/container/ins_child_container.xml"


#define REMOVE_PARENT_START  "../test/data/container/remove_parent_start.xml"
#define REMOVE_PARENT_END  "../test/data/container/remove_parent_end.xml"

#define INSERT_PARENT_START  "../test/data/container/insert_parent_start.xml"
#define INSERT_PARENT_END  "../test/data/container/insert_parent_end.xml"

#define INS_PARENT_1_START  "../test/data/container/insert_parent_1_start.xml"
#define INS_PARENT_1_END  "../test/data/container/insert_parent_1_end.xml"

#define INS_PARENT_2_START  "../test/data/container/insert_parent_2_start.xml"
#define INS_PARENT_2_END  "../test/data/container/insert_parent_2_end.xml"

#define INS_PARENT_3_START  "../test/data/container/insert_parent_3_start.xml"
#define INS_PARENT_3_END  "../test/data/container/insert_parent_3_end.xml"

#define REM_PARENT_1_START  "../test/data/container/remove_parent_1_start.xml"
#define REM_PARENT_1_END  "../test/data/container/remove_parent_1_end.xml"

#define REM_PARENT_2_START  "../test/data/container/remove_parent_2_start.xml"
#define REM_PARENT_2_END  "../test/data/container/remove_parent_2_end.xml"

#define REM_PARENT_3_START  "../test/data/container/remove_parent_3_start.xml"
#define REM_PARENT_3_END  "../test/data/container/remove_parent_3_end.xml"


TestContainer::TestContainer()
{
}

TestContainer::~TestContainer()
{
}

//------------------


bool TestContainer::testInsertChildContainer()
{
    _testName = "testInsertChildContainer" ;

    App app;
    if(!app.init()) {
        return error("init app");
    }
    if( !app.mainWindow()->loadFile(INPUT_FILE) ){
        return error(QString("Unable to load file:%1").arg(INPUT_FILE));
    }
    Regola *regola = app.mainWindow()->getRegola();
    QList<int> sel;
    sel.append(0);
    sel.append(0);

    Element *selectedElement = regola->findElementByArray(sel);
    if( NULL == selectedElement ) {
        return error("Unable to find element 1");
    }
    app.mainWindow()->getEditor()->setCurrentItem(selectedElement);
    if(!checkRoot(regola)) {
        return false;
    }

    // exec the move
    QList<Attribute*> attributes;
    Attribute a("a", "b");
    attributes << &a;
    regola->doInsertChildContainer(app.mainWindow()->getEditor()->getMainTreeWidget(), "testtag", attributes, selectedElement );
    if(!checkRoot(regola)) {
        return false;
    }

    CompareXML compare;

    if(!compare.compareFileWithRegola(regola, RESULT_INSERT_CHILDCONTAINER)) {
        return error(QString("comparing the results 1: %1").arg(compare.errorString()));
    }
#ifdef _NEGATIVE_QXMLEDIT_TEST
    if(compare.compareFileWithRegola(regola, INPUT_FILE)) {
        return error(QString("comparing the results wrong: %1").arg(compare.errorString()));
    }
#endif
    CompareRegolaWithEditor cre;
    if( !cre.compareTreeWithRegola( regola, app.mainWindow()->getEditor()->getMainTreeWidget() )) {
        return error(QString("compare 1 %1").arg(cre.errorString()));
    }
    // undo
    regola->undo();
    if(!checkRoot(regola)) {
        return false;
    }

    if(!compare.compareFileWithRegola(regola, INPUT_FILE)) {
        return error(QString("comparing the undo results: %1").arg(compare.errorString()));
    }
    CompareRegolaWithEditor cre1;
    if( !cre1.compareTreeWithRegola( regola, app.mainWindow()->getEditor()->getMainTreeWidget() )) {
        return error(QString("compare 2 %1").arg(cre1.errorString()));
    }
    // redo
    regola->redo();
    if(!checkRoot(regola)) {
        return false;
    }

    if(!compare.compareFileWithRegola(regola, RESULT_INSERT_CHILDCONTAINER)) {
        return error(QString("comparing the results after redo: %1").arg(compare.errorString()));
    }
    CompareRegolaWithEditor cre2;
    if( !cre2.compareTreeWithRegola( regola, app.mainWindow()->getEditor()->getMainTreeWidget() )) {
        return error(QString("compare 3 %1").arg(cre2.errorString()));
    }
    App app2;
    if(!app2.init()) {
        return error("init app 2");
    }
    if( !app2.mainWindow()->loadFile(INPUT_FILE) ){
        return error(QString("Unable 2 to load file:%1").arg(INPUT_FILE));
    }
#ifdef _NEGATIVE_QXMLEDIT_TEST
    CompareRegolaWithEditor cre3;
    if( cre3.compareTreeWithRegola( regola, app2.mainWindow()->getEditor()->getMainTreeWidget() )) {
        return error(QString("compare 4 "));
    }
#endif
    if(!checkRoot(regola)) {
        return false;
    }

    return true;
}

//---------------------------------

bool TestContainer::testInsertParent()
{
    _testName = "testInsertParent" ;
    const QString FileStart = INSERT_PARENT_START;
    const QString FileEnd = INSERT_PARENT_END ;

    App app;
    if(!app.init()) {
        return error("init app");
    }
    if( !app.mainWindow()->loadFile(FileStart) ){
        return error(QString("Unable to load file:%1").arg(FileStart));
    }
    Regola *regola = app.mainWindow()->getRegola();
    QList<int> sel;
    sel.append(0);
    sel.append(1);

    Element *selectedElement = regola->findElementByArray(sel);
    if( NULL == selectedElement ) {
        return error("Unable to find element 1");
    }
    app.mainWindow()->getEditor()->setCurrentItem(selectedElement);

    // exec the move
    QList<Attribute*> attributes;
    Attribute a("a", "b");
    attributes << &a;
    regola->doInsertParent(app.mainWindow()->getEditor()->getMainTreeWidget(), "test", attributes, selectedElement );
    CompareXML compare;

    if(!compare.compareFileWithRegola(regola, FileEnd)) {
        return error(QString("comparing the results 1: %1").arg(compare.errorString()));
    }
#ifdef _NEGATIVE_QXMLEDIT_TEST
    if(compare.compareFileWithRegola(regola, FileStart)) {
        return error(QString("comparing the results wrong: %1").arg(compare.errorString()));
    }
#endif
    CompareRegolaWithEditor cre;
    if( !cre.compareTreeWithRegola( regola, app.mainWindow()->getEditor()->getMainTreeWidget() )) {
        return error(QString("compare 1 %1").arg(cre.errorString()));
    }
    // undo
    regola->undo();
    if(!compare.compareFileWithRegola(regola, FileStart)) {
        return error(QString("comparing the undo results: %1").arg(compare.errorString()));
    }
    CompareRegolaWithEditor cre1;
    if( !cre1.compareTreeWithRegola( regola, app.mainWindow()->getEditor()->getMainTreeWidget() )) {
        return error(QString("compare 2 %1").arg(cre1.errorString()));
    }
    // redo
    regola->redo();
    if(!compare.compareFileWithRegola(regola, FileEnd)) {
        return error(QString("comparing the results after redo: %1").arg(compare.errorString()));
    }
    CompareRegolaWithEditor cre2;
    if( !cre2.compareTreeWithRegola( regola, app.mainWindow()->getEditor()->getMainTreeWidget() )) {
        return error(QString("compare 3 %1").arg(cre2.errorString()));
    }
    App app2;
    if(!app2.init()) {
        return error("init app 2");
    }
    if( !app2.mainWindow()->loadFile(FileStart) ){
        return error(QString("Unable 2 to load file:%1").arg(FileStart));
    }
#ifdef _NEGATIVE_QXMLEDIT_TEST
    CompareRegolaWithEditor cre3;
    if( cre3.compareTreeWithRegola( regola, app2.mainWindow()->getEditor()->getMainTreeWidget() )) {
        return error(QString("compare 4 "));
    }
#endif
    QList<int> ins1;
    ins1 << 0 ;
    if(! testGeneric(true, "insert_parent_1", INS_PARENT_1_START, INS_PARENT_1_END, ins1, true) ) {
        return false ;
    }
    QList<int> ins2;
    ins2 << 2 ;
    if(! testGeneric(true, "insert_parent_2", INS_PARENT_2_START, INS_PARENT_2_END, ins2, true ) ) {
        return false ;
    }
    QList<int> ins3;
    ins3 << 0;
    ins3 << 1 << 3 ;
    if(! testGeneric(true, "insert_parent_3", INS_PARENT_3_START, INS_PARENT_3_END, ins3, true ) ) {
        return false ;
    }

    return true;
}

bool TestContainer::testRemoveParent()
{
    _testName = "testRemoveParent" ;
    const QString FileStart = REMOVE_PARENT_START;
    const QString FileEnd = REMOVE_PARENT_END ;

    App app;
    if(!app.init()) {
        return error("init app");
    }
    if( !app.mainWindow()->loadFile(FileStart) ){
        return error(QString("Unable to load file:%1").arg(FileStart));
    }
    Regola *regola = app.mainWindow()->getRegola();
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    sel.append(0);

    Element *selectedElement = regola->findElementByArray(sel);
    if( NULL == selectedElement ) {
        return error("Unable to find element 1");
    }
    app.mainWindow()->getEditor()->setCurrentItem(selectedElement);

    // exec the move
    regola->removeParent(app.mainWindow()->getEditor()->getMainTreeWidget(), selectedElement );
    CompareXML compare;

    if(!compare.compareFileWithRegola(regola, FileEnd)) {
        return error(QString("comparing the results 1: %1").arg(compare.errorString()));
    }
#ifdef _NEGATIVE_QXMLEDIT_TEST
    if(compare.compareFileWithRegola(regola, FileStart)) {
        return error(QString("comparing the results wrong: %1").arg(compare.errorString()));
    }
#endif
    CompareRegolaWithEditor cre;
    if( !cre.compareTreeWithRegola( regola, app.mainWindow()->getEditor()->getMainTreeWidget() )) {
        return error(QString("compare 1 %1").arg(cre.errorString()));
    }
    // undo
    regola->undo();
    if(!compare.compareFileWithRegola(regola, FileStart)) {
        return error(QString("comparing the undo results: %1").arg(compare.errorString()));
    }
    CompareRegolaWithEditor cre1;
    if( !cre1.compareTreeWithRegola( regola, app.mainWindow()->getEditor()->getMainTreeWidget() )) {
        return error(QString("compare 2 %1").arg(cre1.errorString()));
    }
    // redo
    regola->redo();
    if(!compare.compareFileWithRegola(regola, FileEnd)) {
        return error(QString("comparing the results after redo: %1").arg(compare.errorString()));
    }
    CompareRegolaWithEditor cre2;
    if( !cre2.compareTreeWithRegola( regola, app.mainWindow()->getEditor()->getMainTreeWidget() )) {
        return error(QString("compare 3 %1").arg(cre2.errorString()));
    }
    App app2;
    if(!app2.init()) {
        return error("init app 2");
    }
    if( !app2.mainWindow()->loadFile(FileStart) ){
        return error(QString("Unable 2 to load file:%1").arg(FileStart));
    }
#ifdef _NEGATIVE_QXMLEDIT_TEST
    CompareRegolaWithEditor cre3;
    if( cre3.compareTreeWithRegola( regola, app2.mainWindow()->getEditor()->getMainTreeWidget() )) {
        return error(QString("compare 4 "));
    }
#endif
    if(!checkRoot(regola)) {
        return false;
    }
    QList<int> rem1;
    rem1 << 0 << 0 ;
    if(! testGeneric(false, "remove_parent_1", REM_PARENT_1_START, REM_PARENT_1_END, rem1, true) ) {
        return false ;
    }
    QList<int> rem2;
    rem2 << 2 << 0 ;
    if(! testGeneric(false, "remove_parent_2", REM_PARENT_2_START, REM_PARENT_2_END, rem2, true) ) {
        return false ;
    }
    QList<int> rem3;
    rem3 << 0 << 1 << 3 << 0 ;
    if(! testGeneric(false, "remove_parent_3", REM_PARENT_3_START, REM_PARENT_3_END, rem3, true) ) {
        return false ;
    }
    return true;
}
/*
bool TestContainer::testGenericWithRegola(const bool isInsert, const QString &testName, const QList<Element*> &start, const QList<Element*> end, QList<int> selectPath, const bool expected)
{
    _testName = testName ;
    Regola regolaStart, regolaEnd;

    buildRegola(&regolaEnd, end);

    QString actionName = isInsert? "actionInsertParent" : "actionRemoveParent" ;
    QString msgFile = QString(" state:'start'");

    App app;
    if(!app.init()) {
        return error(QString("init app %1").arg(msgFile));
    }
    Regola *regola = app.mainWindow()->getRegola();
    buildRegola(regola, start);

    if( !testEnableRegola( window, fileName, selectPath, expected, actionName) ) {
        return false;
    }
    if(!checkRoot(regola)) {
        QString msg = errorString();
        return error( QString("testEnable checkroot 1 %1 %2").arg(msg).arg(msgFile));
    }

    if(!expected) {
        return true;
    }

    // exec the move
    if(isInsert) {
        regola->doInsertParent(app.mainWindow()->getEditor()->getMainTreeWidget(), "testParent", selectedElement );
    } else {
        regola->removeParent(app.mainWindow()->getEditor()->getMainTreeWidget(), selectedElement );
    }
    CompareXML compare;
    if(!checkRoot(regola)) {
        QString msg = errorString();
        return error( QString("testEnable checkroot 2 %1 %2").arg(msg).arg(msgFile));
    }

    if(!compare.compareFileWithRegola(regola, fileEnd)) {
        return error(QString("comparing the results 1: %1 %2").arg(compare.errorString()).arg(msgFile));
    }
#ifdef _NEGATIVE_QXMLEDIT_TEST
    if(compare.compareFileWithRegola(regola, fileStart)) {
        return error(QString("comparing the results wrong: %1 %2").arg(compare.errorString()).arg(msgFile));
    }
#endif
    CompareRegolaWithEditor cre;
    if( !cre.compareTreeWithRegola( regola, app.mainWindow()->getEditor()->getMainTreeWidget() )) {
        return error(QString("compare tree 1 %1 %2").arg(cre.errorString()).arg(msgFile));
    }
    // undo
    regola->undo();
    if(!checkRoot(regola)) {
        QString msg = errorString();
        return error( QString("testEnable checkroot 3 %1 %2").arg(msg).arg(msgFile));
    }

    if(!compare.compareFileWithRegola(regola, fileStart)) {
        return error(QString("comparing the undo results: %1 %2").arg(compare.errorString()).arg(msgFile));
    }
    CompareRegolaWithEditor cre1;
    if( !cre1.compareTreeWithRegola( regola, app.mainWindow()->getEditor()->getMainTreeWidget() )) {
        return error(QString("compare 2 %1 %2").arg(cre1.errorString()).arg(msgFile));
    }
    // redo
    regola->redo();
    if(!checkRoot(regola)) {
        QString msg = errorString();
        return error( QString("testEnable checkroot 4 %1 %2").arg(msg).arg(msgFile));
    }

    if(!compare.compareFileWithRegola(regola, fileEnd)) {
        return error(QString("comparing the results after redo: %1 %2").arg(compare.errorString()).arg(msgFile));
    }
    CompareRegolaWithEditor cre2;
    if( !cre2.compareTreeWithRegola( regola, app.mainWindow()->getEditor()->getMainTreeWidget() )) {
        return error(QString("compare 3 %1 %2").arg(cre2.errorString()).arg(msgFile));
    }
    App app2;
    if(!app2.init()) {
        return error(QString("init app 2 %1").arg(msgFile));
    }
    if( !app2.mainWindow()->loadFile(fileStart) ){
        return error(QString("Unable 2 to load file:%1").arg(fileStart));
    }
#ifdef _NEGATIVE_QXMLEDIT_TEST
    CompareRegolaWithEditor cre3;
    if( cre3.compareTreeWithRegola( regola, app2.mainWindow()->getEditor()->getMainTreeWidget() )) {
        return error(QString("compare 4 %1").arg(msgFile));
    }
#endif
    if(!checkRoot(regola)) {
        QString msg = errorString();
        return error( QString("testEnable checkroot 5 %1 %2").arg(msg).arg(msgFile));
    }
    return true;
}
*/

bool TestContainer::testGeneric(const bool isInsert, const QString &testName, const QString &fileStart, const QString &fileEnd, QList<int> selectPath, const bool expected)
{
    _testName = testName ;
    QString actionName = isInsert? "actionInsertParent" : "actionRemoveParent" ;
    QString msgFile = QString(" file:'%1'").arg(fileStart);

    App app;
    if(!app.init()) {
        return error(QString("init app %1").arg(msgFile));
    }
    if( !app.mainWindow()->loadFile(fileStart) ){
        return error(QString("Unable to load file:%1").arg(fileStart));
    }
    Regola *regola = app.mainWindow()->getRegola();
    Element *selectedElement = NULL ;
    if( !testEnableRegola( app.mainWindow(), fileStart, selectPath, expected, actionName, selectedElement) ) {
        return false;
    }
    if(!checkRoot(regola)) {
        QString msg = errorString();
        return error( QString("testEnable checkroot 1 %1 %2").arg(msg).arg(msgFile));
    }

    if(!expected) {
        return true;
    }

    // exec the move
    if(isInsert) {
        regola->doInsertParent(app.mainWindow()->getEditor()->getMainTreeWidget(), "testParent", QList<Attribute*>(), selectedElement );
    } else {
        regola->removeParent(app.mainWindow()->getEditor()->getMainTreeWidget(), selectedElement );
    }
    CompareXML compare;
    if(!checkRoot(regola)) {
        QString msg = errorString();
        return error( QString("testEnable checkroot 2 %1 %2").arg(msg).arg(msgFile));
    }

    if(!compare.compareFileWithRegola(regola, fileEnd)) {
        return error(QString("comparing the results 1: %1 %2").arg(compare.errorString()).arg(msgFile));
    }
#ifdef _NEGATIVE_QXMLEDIT_TEST
    if(compare.compareFileWithRegola(regola, fileStart)) {
        return error(QString("comparing the results wrong: %1 %2").arg(compare.errorString()).arg(msgFile));
    }
#endif
    CompareRegolaWithEditor cre;
    if( !cre.compareTreeWithRegola( regola, app.mainWindow()->getEditor()->getMainTreeWidget() )) {
        return error(QString("compare tree 1 %1 %2").arg(cre.errorString()).arg(msgFile));
    }
    // undo
    regola->undo();
    if(!checkRoot(regola)) {
        QString msg = errorString();
        return error( QString("testEnable checkroot 3 %1 %2").arg(msg).arg(msgFile));
    }

    if(!compare.compareFileWithRegola(regola, fileStart)) {
        return error(QString("comparing the undo results: %1 %2").arg(compare.errorString()).arg(msgFile));
    }
    CompareRegolaWithEditor cre1;
    if( !cre1.compareTreeWithRegola( regola, app.mainWindow()->getEditor()->getMainTreeWidget() )) {
        return error(QString("compare 2 %1 %2").arg(cre1.errorString()).arg(msgFile));
    }
    // redo
    regola->redo();
    if(!checkRoot(regola)) {
        QString msg = errorString();
        return error( QString("testEnable checkroot 4 %1 %2").arg(msg).arg(msgFile));
    }

    if(!compare.compareFileWithRegola(regola, fileEnd)) {
        return error(QString("comparing the results after redo: %1 %2").arg(compare.errorString()).arg(msgFile));
    }
    CompareRegolaWithEditor cre2;
    if( !cre2.compareTreeWithRegola( regola, app.mainWindow()->getEditor()->getMainTreeWidget() )) {
        return error(QString("compare 3 %1 %2").arg(cre2.errorString()).arg(msgFile));
    }
    App app2;
    if(!app2.init()) {
        return error(QString("init app 2 %1").arg(msgFile));
    }
    if( !app2.mainWindow()->loadFile(fileStart) ){
        return error(QString("Unable 2 to load file:%1").arg(fileStart));
    }
#ifdef _NEGATIVE_QXMLEDIT_TEST
    CompareRegolaWithEditor cre3;
    if( cre3.compareTreeWithRegola( regola, app2.mainWindow()->getEditor()->getMainTreeWidget() )) {
        return error(QString("compare 4 %1").arg(msgFile));
    }
#endif
    if(!checkRoot(regola)) {
        QString msg = errorString();
        return error( QString("testEnable checkroot 5 %1 %2").arg(msg).arg(msgFile));
    }
    return true;
}



bool TestContainer::testFast()
{
    _testName = "testFast" ;
    if( !testEnableInsParent() ) {
        return false;
    }
    QList<int> rem3;
    rem3 << 0 << 1 << 3 << 0 ;
    if(! testGeneric(false, "remove_parent_3", REM_PARENT_3_START, REM_PARENT_3_END, rem3, true) ) {
        return false ;
    }
    if( !testEnableInsParent() ) {
        return false;
    }
    return true;
}


#define INS_PI_ROOTONLY "../test/data/container/ins_par_pionly.xml"
#define INS_PI_ROOTONLY_END "../test/data/container/ins_par_pionly_end.xml"
#define INS_PI_ONLYCOMMENT "../test/data/container/t_only_comment.xml"
#define INS_PI_ONLYCOMMENT_END "../test/data/container/t_only_comment_end.xml"
#define INS_ROOT "../test/data/container/t_root.xml"
#define INS_ROOT_END "../test/data/container/t_root_end.xml"
#define INS_PI_ONLYROOT "../test/data/container/t_only_root.xml"
#define INS_PI_ONLYROOT_END "../test/data/container/t_only_root_end.xml"
#define INS_ROOT_CHILD "../test/data/container/t_root_child.xml"
#define INS_ROOT_CHILD_END "../test/data/container/t_root_child_end.xml"
#define INS_ROOT_CHILD_TEXT "../test/data/container/t_root_child_text.xml"
#define INS_ROOT_CHILD_TEXT_END "../test/data/container/t_root_child_text_end.xml"
#define INS_CHILD "../test/data/container/t_child.xml"
#define INS_CHILD_END "../test/data/container/t_child_end.xml"
#define INS_CHILD_TEXT "../test/data/container/t_child_text.xml"
#define INS_CHILD_TEXT_END "../test/data/container/t_child_text_end.xml"

bool TestContainer::testEnableInsParent()
{
    _testName = "testEnableInsParent";
    QList<int> selectPath;
    // only pi: yes
    selectPath << 0 ;
    //if( !testGeneric(true, "ins_par_pionly", INS_PI_ROOTONLY, INS_PI_ROOTONLY_END, selectPath, true) ) {
    //    return false;
    //}
    // only comment: yes
    //selectPath.clear();
    //selectPath << 0 ;
    //if( !testGeneric(true, "t_only_comment", INS_PI_ONLYCOMMENT, INS_PI_ONLYCOMMENT_END, selectPath, true) ) {
    //    return false;
    //}
    // comment and root*: yes
    selectPath.clear();
    selectPath << 1 ;
    if( !testGeneric(true, "t_root", INS_ROOT, INS_ROOT_END, selectPath, true) ) {
        return false;
    }
    // comment* and root:no
    selectPath.clear();
    selectPath << 0 ;
    if( !testGeneric(true, "t_root", INS_ROOT, INS_ROOT_END, selectPath, false) ) {
        return false;
    }
    // only root: yes
    selectPath.clear();
    selectPath << 0 ;
    if( !testGeneric(true, "t_only_root", INS_PI_ONLYROOT, INS_PI_ONLYROOT_END, selectPath, true) ) {
        return false;
    }
    // root child: yes
    selectPath.clear();
    selectPath << 0 << 0 ;
    if( !testGeneric(true, "t_root_child", INS_ROOT_CHILD, INS_ROOT_CHILD_END, selectPath, true) ) {
        return false;
    }
    // root child text: yes
    selectPath.clear();
    selectPath << 0 << 1 ;
    if( !testGeneric(true, "t_root_child_text", INS_ROOT_CHILD_TEXT, INS_ROOT_CHILD_TEXT_END, selectPath, true) ) {
        return false;
    }
    // child: yes
    selectPath.clear();
    selectPath << 0 << 0 << 0;
    if( !testGeneric(true, "t_child", INS_CHILD, INS_CHILD_END, selectPath, true) ) {
        return false;
    }
    // child text: yes
    selectPath.clear();
    selectPath << 0 << 0 << 1 ;
    if( !testGeneric(true, "t_child_text", INS_CHILD_TEXT, INS_CHILD_TEXT_END, selectPath, true) ) {
        return false;
    }
    return true;
}

#define REM_BASE "../test/data/container/r_base.xml"
#define REM_1 "../test/data/container/r_1.xml"
#define REM_2 "../test/data/container/r_2.xml"
#define REM_2_END "../test/data/container/r_2_end.xml"
#define REM_3 "../test/data/container/r_3.xml"
#define REM_3_END "../test/data/container/r_3_end.xml"
#define REM_4 "../test/data/container/r_4.xml"
#define REM_4_END "../test/data/container/r_4_end.xml"
#define REM_5 "../test/data/container/r_5.xml"


bool TestContainer::testEnableRemoveParent()
{
    _testName = "testEnableRemoveParent";
    QList<int> selectPath;
    selectPath << 0 ;
    if( !testGeneric(false, "r_base", REM_BASE, REM_BASE, selectPath, false) ) {
        return false;
    }

    selectPath.clear();
    selectPath << 1 ;
    if( !testGeneric(false, "r_base", REM_BASE, REM_BASE, selectPath, false) ) {
        return false;
    }
    selectPath.clear();
    selectPath << 2 ;
    if( !testGeneric(false, "r_base", REM_BASE, REM_BASE, selectPath, false) ) {
        return false;
    }
    selectPath.clear();
    selectPath << 3 ;
    if( !testGeneric(false, "r_base", REM_BASE, REM_BASE, selectPath, false) ) {
        return false;
    }
    selectPath.clear();
    selectPath << 0 << 1 ;
    if( !testGeneric(false, "r_1", REM_1, REM_1, selectPath, false) ) {
        return false;
    }
    selectPath.clear();
    selectPath << 0 << 0 ;
    if( !testGeneric(false, "r_2", REM_2, REM_2_END, selectPath, false) ) {
        return false;
    }
    selectPath.clear();
    selectPath << 0 << 1 ;
    if( !testGeneric(false, "r_2", REM_2, REM_2_END, selectPath, false) ) {
        return false;
    }
    selectPath.clear();
    selectPath << 0 << 2 ;
    if( !testGeneric(false, "r_2", REM_2, REM_2_END, selectPath, false) ) {
        return false;
    }
    //------
    selectPath.clear();
    selectPath << 0 << 0 ;
    if( !testGeneric(false, "r_3", REM_3, REM_3_END, selectPath, true) ) {
        return false;
    }
    selectPath.clear();
    selectPath << 0 << 1 ;
    if( !testGeneric(false, "r_3", REM_3, REM_3_END, selectPath, true) ) {
        return false;
    }
    selectPath.clear();
    selectPath << 0 << 2 ;
    if( !testGeneric(false, "r_3", REM_3, REM_3_END, selectPath, true) ) {
        return false;
    }
    selectPath.clear();
    selectPath << 0 << 3 ;
    if( !testGeneric(false, "r_3", REM_3, REM_3_END, selectPath, true) ) {
        return false;
    }
    //---
    selectPath.clear();
    selectPath << 0 << 0 ;
    if( !testGeneric(false, "r_4", REM_4, REM_4_END, selectPath, true) ) {
        return false;
    }
    //---
    selectPath.clear();
    selectPath << 0 << 0 ;
    if( !testGeneric(false, "r_5", REM_5, REM_5, selectPath, false) ) {
        return false;
    }
    selectPath.clear();
    selectPath << 0 << 1 ;
    if( !testGeneric(false, "r_5", REM_5, REM_5, selectPath, false) ) {
        return false;
    }

    return true;
}

bool TestContainer::testEnableRegola( MainWindow *window, const QString &fileName, QList<int> &selectPath, const bool expected, const QString &actionName, Element *&selectedElement)
{
    selectedElement = window->getRegola()->findElementByArray(selectPath);
    if( NULL == selectedElement ) {
        return error(QString("Unable to find element for testEnable path:'%1' file:'%2'").arg(listIntToString(selectPath)).arg(fileName));
    }
    bool isEnabled = false ;
    window->getEditor()->setCurrentItem(selectedElement);
    if( !checkActionEnabled(window, actionName, isEnabled)) {
        return error(QString("Unable to find action '%1'for testEnable path:'%2' file:'%3'").arg(actionName).arg(listIntToString(selectPath)).arg(fileName));
    }
    if( expected != isEnabled ) {
        return error(QString("TestEnable action:'%1' expected:%2, found:%3 path:'%4' file:'%5'").arg(actionName).arg(expected).arg(isEnabled).arg(listIntToString(selectPath)).arg(fileName));
    }
    return true;
}

/*
bool TestContainer::testEnable(const bool isInsert, const QString &fileStart, const QString &fileEnd, QList<int> &selectPath, const bool expected)
{
    QString actionName = isInsert? "actionInsertParent" : "actionRemoveParent" ;
    App app;
    if(!app.init()) {
        return error("init app testEnable");
    }
    if( !app.mainWindow()->loadFile(fileName) ){
        return error(QString("TestEnable Unable to load file:%1").arg(fileStart));
    }
    if( !testEnableRegola( window, fileName, selectPath, expected, actionName) ) {
        return false;
    }
    if(!expected) {
        return true;
    }
    if(isInsert) {
        regola->doInsertParent(app.mainWindow()->getEditor()->getMainTreeWidget(), "testParent", selectedElement );
    } else {
        regola->removeParent(app.mainWindow()->getEditor()->getMainTreeWidget(), selectedElement );
    }
    if(!checkRoot(regola)) {
        QString msg = errorString();
        return error( QString("testEnable checkroot 1 %1").arg(msg));
    }
    CompareXML compare;

    if(!compare.compareFileWithRegola(regola, fileEnd)) {
        return error(QString("comparing the results 1: %1").arg(compare.errorString()));
    }
    CompareRegolaWithEditor cre;
    if( !cre.compareTreeWithRegola( regola, app.mainWindow()->getEditor()->getMainTreeWidget() )) {
        return error(QString("compare tree 1 %1").arg(cre.errorString()));
    }
    // undo
    regola->undo();
    if(!checkRoot(regola)) {
        return false;
    }

    if(!compare.compareFileWithRegola(regola, fileStart)) {
        return error(QString("comparing the undo results: %1").arg(compare.errorString()));
    }
    CompareRegolaWithEditor cre1;
    if( !cre1.compareTreeWithRegola( regola, app.mainWindow()->getEditor()->getMainTreeWidget() )) {
        return error(QString("compare 2 %1").arg(cre1.errorString()));
    }
    // redo
    regola->redo();
    if(!checkRoot(regola)) {
        return false;
    }

    if(!compare.compareFileWithRegola(regola, fileEnd)) {
        return error(QString("comparing the results after redo: %1").arg(compare.errorString()));
    }
    CompareRegolaWithEditor cre2;
    if( !cre2.compareTreeWithRegola( regola, app.mainWindow()->getEditor()->getMainTreeWidget() )) {
        return error(QString("compare 3 %1").arg(cre2.errorString()));
    }
    App app2;
    if(!app2.init()) {
        return error("init app 2");
    }
    if( !app2.mainWindow()->loadFile(fileStart) ){
        return error(QString("Unable 2 to load file:%1").arg(fileStart));
    }
    if(!checkRoot(regola)) {
        return false;
    }
    return false ;
}
*/
