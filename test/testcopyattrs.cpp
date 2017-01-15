/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013-2017 by Luca Bellonda and individual contributors  *
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


#include "testcopyattrs.h"
#include "app.h"
#include "modules/copyattr/copyattributesdialog.h"
#include "comparexml.h"
#include "modules/copyattr/copiedattribute.h"

#define FILE_BASE "../test/data/copyattr/base.xml"
#define FILE_STEP1 "../test/data/copyattr/copyall1.xml"
#define FILE_STEP1e "../test/data/copyattr/copyall1e.xml"

TestCopyAttrs::TestCopyAttrs()
{
}

TestCopyAttrs::~TestCopyAttrs()
{
}

//-----------------------------------------------------------
bool TestCopyAttrs::copyAttrs(App *app)
{
    QList<int> selection;
    selection.append(0);
    selection.append(0);
    Element *selectedElement = app->mainWindow()->getRegola()->findElementByArray(selection);
    if(NULL == selectedElement) {
        return error("select item 1");
    }

    CopyAttributesDialog dialog(selectedElement, app->data()->copyAttributesManager(), app->mainWindow());
    dialog.show();
    dialog.on_cmdSelectAll_clicked();
    dialog.accept();
    return true ;
}

bool TestCopyAttrs::testAttributesInClipboard()
{
    QString clipboardData ;
    QClipboard *clipboard = QApplication::clipboard();
    if(NULL != clipboard) {
        clipboardData = clipboard->text();
    }
    const QString expected = "a1=\"aa1\" a2=\"aa2\" a3=\"aa3\"";
    if(expected != clipboardData) {
        return error(QString("Error checking clipboard after attributes copy, found:'%1', expected:'%2'")
                     .arg(clipboardData).arg(expected));
    }
    return true ;
}

bool TestCopyAttrs::pasteAttrs(App *app)
{
    QList<int> selection;
    selection.append(0);
    selection.append(1);
    Element *selectedElement = app->mainWindow()->getRegola()->findElementByArray(selection);
    if(NULL == selectedElement) {
        return error("select item 2");
    }
    app->mainWindow()->getEditor()->setCurrentItem(selectedElement);
    app->mainWindow()->on_actionPasteLastAttributes_triggered();
    return true ;
}

bool TestCopyAttrs::pasteAttrsExcl(App *app)
{
    QList<int> selection;
    selection.append(0);
    selection.append(1);
    Element *selectedElement = app->mainWindow()->getRegola()->findElementByArray(selection);
    if(NULL == selectedElement) {
        return error("select item 2");
    }

    //base:<two a1="aa1" a2="xa2" a4="aa4"/>
    //all1:<two a1="aa1" a2="aa2" a4="aa4"/>
    CopyAttributesSession cas;
    QList<Attribute *> resList;
    Attribute *attr1, *attr2, *attr3, *attr4 ;
    attr1 = new Attribute("a1", "aa1");
    attr2 = new Attribute("a2", "aa2");
    attr3 = new Attribute("a3", "");
    attr4 = new Attribute("a4", "aa4");

    bool isOk = true ;
    if( NULL != attr1 ) {
        resList.append(attr1);
    } else {
        isOk = false ;
    }
    if( NULL != attr2 ) {
        resList.append(attr2);
    } else {
        isOk = false ;
    }
    if( NULL != attr3 ) {
        resList.append(attr3);
    } else {
        isOk = false ;
    }
    if( NULL != attr4 ) {
        resList.append(attr4);
    } else {
        isOk = false ;
    }
    if( !isOk ) {
        return error("null allocated attribute");
    }
    cas.setAttributes(resList);

    app->mainWindow()->getEditor()->setCurrentItem(selectedElement);
    app->mainWindow()->getRegola()->pasteClearAttributesInternals(app->mainWindow()->getEditor()->getMainTreeWidget(), selectedElement, &cas);
    return true ;
}

bool TestCopyAttrs::cfrStep1(App *app, const QString &msg)
{
    return cfr(app, QString("step1 %1").arg(msg), FILE_STEP1);
}

bool TestCopyAttrs::cfrStep1e(App *app, const QString &msg)
{
    return cfr(app, QString("step1e %1").arg(msg), FILE_STEP1e);
}

bool TestCopyAttrs::cfrStep2(App *app, const QString &msg)
{
    return cfr(app, QString("step2 %1").arg(msg), FILE_BASE);
}

bool TestCopyAttrs::cfrStep2e(App *app, const QString &msg)
{
    return cfr(app, QString("step2e %1").arg(msg), FILE_BASE);
}

bool TestCopyAttrs::cfr(App *app, const QString &step, const QString &file)
{
    QByteArray resultData = app->mainWindow()->getRegola()->writeMemory();
    QDomDocument document1;
    QDomDocument document2;
    CompareXML compare;
    if(!compare.loadFileIntoDocument(file, document1)) {
        return error(QString("load file step %1").arg(step));
    }
    QBuffer outputData(&resultData);
    if(!compare.loadFileIntoDocument(&outputData, document2)) {
        return error(QString("load file step %1 a").arg(step));
    }
    bool result = compare.compareDomDocuments(document1, document2);
    if( !result ) {
        return error(QString("comparing files step %1").arg(step));
    }
    return true ;
}

//-----------------------------------------------------------

bool TestCopyAttrs::testCopyAttrs()
{
    _testName = "testCopyAttrs";
    App app;
    if(!app.init() ) {
        return error("init");
    }
    if(!app.mainWindow()->loadFile(QString(FILE_BASE))) {
        return error("load file");
    }
    if( !copyAttrs(&app) ) {
        return false;
    }
    if(!testAttributesInClipboard()) {
        return false;
    }
    if(! pasteAttrs(&app) ) {
        return false;
    }
    if( !cfrStep1(&app, "0") ) {
        return false;
    }
    app.mainWindow()->getRegola()->undo();
    if( !cfrStep2(&app, "1") ) {
        return false;
    }
    app.mainWindow()->getRegola()->undo();
    if( !cfrStep2(&app, "2") ) {
        return false;
    }
    app.mainWindow()->getRegola()->redo();
    if( !cfrStep1(&app, "3") ) {
        return false;
    }
    if( !cfrStep1(&app, "4") ) {
        return false;
    }
    return true ;
}

bool TestCopyAttrs::testCopyAttrsExcl()
{
    _testName = "testCopyAttrsExcl";
    App app;
    if(!app.init() ) {
        return error("init");
    }
    if(!app.mainWindow()->loadFile(QString(FILE_BASE))) {
        return error("load file");
    }
    if(!pasteAttrsExcl(&app) ) {
        return false;
    }
    if( !cfrStep1e(&app, "0") ) {
        return false;
    }
    app.mainWindow()->getRegola()->undo();
    if( !cfrStep2e(&app, "1") ) {
        return false;
    }
    app.mainWindow()->getRegola()->undo();
    if( !cfrStep2e(&app, "2") ) {
        return false;
    }
    app.mainWindow()->getRegola()->redo();
    if( !cfrStep1e(&app, "3") ) {
        return false;
    }
    if( !cfrStep1e(&app, "4") ) {
        return false;
    }
    return true ;
}


