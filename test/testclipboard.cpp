/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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

#include "testclipboard.h"
#include "app.h"
#include "comparexml.h"

#define FILE_TEST   "../test/data/clipb1.xml"

#define FILE_BASE_PASTEASS   "../test/data/clipboard/pastebase.xml"
#define FILE_TEST1_PASTEASS   "../test/data/clipboard/pastetest1.xml"
#define FILE_BASE_COPYALL   "../test/data/clipboard/copyall.xml"

TestClipboard::TestClipboard()
{
}

bool TestClipboard::verifyClipboard(const QString &value)
{
    bool compareOk = false;
    QClipboard *clipboard = QApplication::clipboard();
    if(NULL != clipboard) {
        QString cs = clipboard->text();
        if( cs == value ) {
            compareOk = true ;
        }
    }
    return compareOk;
}


bool TestClipboard::setClipboard(const QString &value)
{
    QClipboard *clipboard = QApplication::clipboard();
    if(NULL != clipboard) {
        clipboard->setText(value);
        return true;
    }
    return false;
}

bool TestClipboard::testCopyElementPathToClipboard()
{
    _testName = "testCopyElementPathToClipboard" ;
    App app;
    if(!app.initNoWindow() ) {
        return error("init window");
    }
    MainWindow mainWindow(false, app.data());

    if( !mainWindow.loadFile(QString(FILE_TEST)) ) {
        return error(QString("opening test file: '%1'").arg(FILE_TEST));
    }

    if(!setClipboard("~")){
        return error("set clipboard 1");
    }

    QList<int> selection1;
    selection1.append(1); // root
    selection1.append(0); //a1
    selection1.append(1); //b2
    selection1.append(1); //c2
    Element *selectedElement = mainWindow.getRegola()->findElementByArray(selection1);
    if(NULL == selectedElement) {
        return error("select 1");
    }
    mainWindow.getEditor()->setCurrentItem(selectedElement);
    if( !fireAction(&mainWindow, "actionCopyElementPathClipboard" ) ) {
        return error("action not found");
    }

    if(!verifyClipboard("/root/a1/b2/c2")){
        return error("verify clipboard inner element");
    }
    //---------
    if(!setClipboard("~")){
        return error("set clipboard 2");
    }

    // this is a comment
    QList<int> selection2;
    selection2.append(1); // root
    selection2.append(0); //a1
    selection2.append(0); //comment
    selectedElement = mainWindow.getRegola()->findElementByArray(selection2);
    if(NULL == selectedElement) {
        return error("select 2");
    }
    mainWindow.getEditor()->setCurrentItem(selectedElement);
    if( !fireAction(&mainWindow, "actionCopyElementPathClipboard" ) ) {
        return error("action not found");
    }

    if(!verifyClipboard("~")){
        return error("verify comment");
    }
    if(!setClipboard("~")){
        return error("set clipboard 3");
    }

    // root
    QList<int> selection3;
    selection3.append(1);
    selectedElement = mainWindow.getRegola()->findElementByArray(selection3);
    if(NULL == selectedElement) {
        return error("select 3");
    }
    mainWindow.getEditor()->setCurrentItem(selectedElement);
    if( !fireAction(&mainWindow, "actionCopyElementPathClipboard" ) ) {
        return error("action not found");
    }

    if(!verifyClipboard("/root")){
        return error("verify clipboard root");
    }
    return true;
}

Element *TestClipboard::selectElement(MainWindow *mainWindow, QList<int> &selection)
{
    Element *selectedElement = mainWindow->getRegola()->findElementByArray(selection);
    if(NULL == selectedElement) {
        return NULL;
    }
    mainWindow->getEditor()->setCurrentItem(selectedElement);
    return selectedElement ;
}

bool TestClipboard::testPasteAsSibling()
{
    _testName = "testPasteAsSibling" ;
    App app;
    if(!app.initNoWindow() ) {
        return error("init window");
    }
    MainWindow mainWindow(false, app.data());

    if( !mainWindow.loadFile(QString(FILE_BASE_PASTEASS)) ) {
        return error(QString("opening test file: '%1'").arg(FILE_BASE_PASTEASS));
    }

    if(!setClipboard("~")){
        return error("set clipboard 1");
    }

    QList<int> selection1;
    selection1.append(1); // root
    selection1.append(0); //a1
    selection1.append(1); //b2
    if(NULL == selectElement(&mainWindow, selection1)) {
        return error("select 1");
    }
    if( !fireAction(&mainWindow, "actionCopy" ) ) {
        return error("action copy not found");
    }

    QList<int> selection2;
    selection2.append(1); // root
    selection2.append(0); //a1
    selection2.append(1); //b2
    selection2.append(0); //c1

    if(NULL == selectElement(&mainWindow, selection2)) {
        return error("select 2");
    }

    if( !fireAction(&mainWindow, "actionPasteAsSibling" ) ) {
        return error("action paste not found");
    }
    // compare with file
    CompareXML compare;
    if(!compare.compareFileWithRegola(mainWindow.getEditor()->getRegola(), FILE_TEST1_PASTEASS)) {
        return error(QString("comparing the results 1: %1").arg(compare.errorString()));
    }

    mainWindow.getEditor()->getRegola()->undo();
    if(!compare.compareFileWithRegola(mainWindow.getEditor()->getRegola(), FILE_BASE_PASTEASS)) {
        return error(QString("comparing the results after undo with base: %1").arg(compare.errorString()));
    }

    mainWindow.getEditor()->getRegola()->redo();
    if(!compare.compareFileWithRegola(mainWindow.getEditor()->getRegola(), FILE_TEST1_PASTEASS)) {
        return error(QString("comparing the results after redo: %1").arg(compare.errorString()));
    }

    if( !mainWindow.loadFile(QString(FILE_BASE_PASTEASS)) ) {
        return error(QString("opening test file for second time: '%1'").arg(FILE_BASE_PASTEASS));
    }

    QList<int> selection3;
    selection3.append(1); // root
    selection3.append(0); //a1
    selection3.append(1); //b2
    selection3.append(0); //c1

    if(NULL == selectElement(&mainWindow, selection3)) {
        return error("select 3");
    }

    if( !fireAction(&mainWindow, "actionCopy" ) ) {
        return error("action copy not found 2");
    }

    QList<int> selection4;
    selection4.append(0); // root

    if(NULL == selectElement(&mainWindow, selection4)) {
        return error("select 4");
    }

    if( !fireAction(&mainWindow, "actionPasteAsSibling" ) ) {
        return error("action paste 2 not found");
    }

    // compare with file
    if(!compare.compareFileWithRegola(mainWindow.getEditor()->getRegola(), FILE_BASE_PASTEASS)) {
        return error(QString("comparing pasting item as root : %1").arg(compare.errorString()));
    }

    return true;
}

bool TestClipboard::testCopyAllToClipboard()
{
    _testName = "testCopyAllToClipboard";
    App app;
    if(!app.init(true) ) {
        return error("init window");
    }
    if( !app.mainWindow()->loadFile(FILE_BASE_COPYALL) ) {
        return error(QString("opening test file: '%1'").arg(FILE_BASE_COPYALL));
    }

    if(!setClipboard("~")){
        return error("set clipboard 1");
    }

    if( !fireAction(app.mainWindow(), "actionCopyAllToClipboard" ) ) {
        return error("action copy all not found");
    }
    QString result = QApplication::clipboard()->text();
    QByteArray ba = result.toUtf8();
    QBuffer buffer;
    buffer.setData(ba);
    CompareXML compare;
    if( !compare.compareBufferWithFile(&buffer, FILE_BASE_COPYALL)) {
        return error(QString("comparing item : %1").arg(compare.errorString()));
    }
    return true;
}
