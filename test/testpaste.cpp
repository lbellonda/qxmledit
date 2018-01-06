/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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
#include "app.h"
#include "testpaste.h"
#include "comparexml.h"
#include "mainwindow.h"
#include "utils.h"
#include "qxmleditconfig.h"

#define TEST_FILE_PASTE_INPUT "../test/data/paste_input.xml"
#define TEST_FILE_PASTE_RESULT "../test/data/paste_result.xml"
#define TEST_FILE_PASTE_RESULTROOT  "../test/data/paste_result_root.xml"
#define TEST_FILE_PASTE_RESULTNOROOT "../test/data/paste_result_noroot.xml"
#define TEST_FILE_PASTE_RESULTELEMENT "../test/data/paste_result_element.xml"

#define TEST_FILE_PASTE_RESULTROOT_SIBLING  "../test/data/paste_result_root_sibling.xml"
#define TEST_FILE_PASTE_RESULTNOROOT_SIBLING "../test/data/paste_result_noroot_sibling.xml"
#define TEST_FILE_PASTE_RESULTELEMENT_SIBLING "../test/data/paste_result_element_sibling.xml"

TestPaste::TestPaste()
{
}

TestPaste::~TestPaste()
{
}

bool TestPaste::init(App *app)
{
    if(!app->init() ) {
        return error("init");
    }
    app->mainWindow()->loadFile(QString(TEST_FILE_PASTE_INPUT));

    QList<int> selection;
    selection.append(1);
    selection.append(0);
    selection.append(1);
    selection.append(1);
    Element *selectedElement = app->mainWindow()->getRegola()->findElementByArray(selection);
    if(NULL == selectedElement) {
        return error("no selected element");
    }
    app->mainWindow()->getEditor()->setCurrentItem(selectedElement);
    return true;
}

bool TestPaste::testAPaste()
{
    App app;
    /*if(!app.initNoWindow() ) {
        return error("init");
    }
    MainWindow mainWindow(false, qApp, app.data());

    mainWindow.loadFile(QString(TEST_FILE_PASTE_INPUT));

    QList<int> selection;
    selection.append(1);
    selection.append(0);
    selection.append(1);
    selection.append(1);
    Element *selectedElement = mainWindow.getRegola()->findElementByArray(selection);
    if(NULL == selectedElement) {
        return error("no selected element");
    }*/

    if(!init(&app)) {
        return false;
    }
    Element *pasteElement = new Element("test", "", NULL);
    if(NULL == pasteElement) {
        return error("no paste element");
    }
    pasteElement->setText("test");
    pasteElement->addAttribute("attr1", "value");
    TextChunk *newTextChunk = new  TextChunk(false, "text node");
    if(NULL == newTextChunk) {
        delete pasteElement ;
        return error("null text chunk");
    }
    pasteElement->addTextNode(newTextChunk);
    Element * selectedElement = app.mainWindow()->getEditor()->getSelectedItem();
    app.mainWindow()->getRegola()->pasteInternals(app.mainWindow()->getMainTreeWidget(), selectedElement, pasteElement, -1);

    delete pasteElement ;
    QByteArray resultData = app.mainWindow()->getRegola()->writeMemory();
    QBuffer outputData(&resultData);
    QApplication::clipboard()->setText(QString(resultData));

    QDomDocument document1;
    QDomDocument document2;
    CompareXML compare;
    if(!compare.loadFileIntoDocument(TEST_FILE_PASTE_RESULT, document1)) {
        return error("compare no 1");
    }
    if(!compare.loadFileIntoDocument(&outputData, document2)) {
        return error("compare no 2");
    }
    bool result = compare.compareDomDocuments(document1, document2);
    if(!result ) {
        return error("compare no 3");
    }

    return result ;
}

//-------

bool TestPaste::testPasteMultiple()
{
    if( !testPastePasteFromClipboard("testPasteMultiple/noxml", "bau bau", TEST_FILE_PASTE_INPUT, "actionPaste") ) {
        return false ;
    }
    if( ! testPastePasteFromClipboard("testPasteMultiple/xmlroot", "<?xml version='1.0' ?><root a='1'><b/></root>", TEST_FILE_PASTE_RESULTROOT, "actionPaste") ) {
        return false;
    }
    if( ! testPastePasteFromClipboard("testPasteMultiple/xmlnoroot", "<root a='1'><b/></root><bb/><aa/>", TEST_FILE_PASTE_RESULTNOROOT, "actionPaste") ) {
        return false;
    }
    if( ! testPastePasteFromClipboardForElement("testPasteMultiple/xmlelement", TEST_FILE_PASTE_RESULTELEMENT, "actionPaste") ) {
        return false;
    }
    //----------------------------------------------------
    if( !testPastePasteFromClipboard("testPasteMultiple/noxmlSibling", "bau bau", TEST_FILE_PASTE_INPUT, "actionPasteAsSibling") ) {
        return false ;
    }
    if( ! testPastePasteFromClipboard("testPasteMultiple/xmlrootSibling", "<?xml version='1.0' ?><root a='1'><b/></root>", TEST_FILE_PASTE_RESULTROOT_SIBLING, "actionPasteAsSibling") ) {
        return false;
    }
    if( ! testPastePasteFromClipboard("testPasteMultiple/xmlnorootSibling", "<root a='1'><b/></root><bb/><aa/>", TEST_FILE_PASTE_RESULTNOROOT_SIBLING, "actionPasteAsSibling") ) {
        return false;
    }
    if( ! testPastePasteFromClipboardForElement("testPasteMultiple/xmlelementSibling", TEST_FILE_PASTE_RESULTELEMENT_SIBLING, "actionPasteAsSibling") ) {
        return false;
    }
    //-------------------------------------------
    return true;
}

bool TestPaste::testPastePasteFromClipboard(const QString &newTestName, const QString &textToPaste, const QString &fileResults, const QString &actionName)
{
    _testName = newTestName;
    App app;
    if(!init(&app)) {
        return false;
    }

    QApplication::clipboard()->setText(textToPaste);

    if(!fireAction(app.mainWindow(), actionName)) {
        return error("Unable to fire paste.");
    }

    QByteArray resultData = app.mainWindow()->getRegola()->writeMemory();
    QBuffer outputData(&resultData);

    QDomDocument document1;
    QDomDocument document2;
    CompareXML compare;
    if(!compare.loadFileIntoDocument(fileResults, document1)) {
        return error(QString("unable to load file 1: '%1'").arg(fileResults));
    }
    if(!compare.loadFileIntoDocument(&outputData, document2)) {
        return error(QString("unable to load result string :%1").arg(QString::fromUtf8(resultData)));
    }
    bool result = compare.compareDomDocuments(document1, document2);
    if(!result ) {
        compare.dumpErrorCause();
        QString source = QString::fromUtf8(resultData);
        return error(QString("The documents differ error:'%2' source:%1").arg(source).arg(compare.errorString()));
    }

    return result ;
}


bool TestPaste::testPastePasteFromClipboardForElement(const QString &newTestName, const QString &fileResults, const QString &actionName)
{
    _testName = newTestName;
    App app;
    if(!init(&app)) {
        return error("init app");
    }

    //app.mainWindow()->getEditor()->setCurrentItem(selectedElement);

    if(!fireAction(app.mainWindow(), "actionCopy")) {
        return error("Unable to fire paste.");
    }

    QList<int> selection;
    selection.append(1);
    selection.append(0);
    selection.append(0);
    Element *selectedElement = app.mainWindow()->getRegola()->findElementByArray(selection);
    if(NULL == selectedElement) {
        return error("no selected element for paste");
    }
    app.mainWindow()->getEditor()->setCurrentItem(selectedElement);
    if(!fireAction(app.mainWindow(), actionName)) {
        return error("Unable to fire paste.");
    }

    QByteArray resultData = app.mainWindow()->getRegola()->writeMemory();
    QBuffer outputData(&resultData);

    QDomDocument document1;
    QDomDocument document2;
    CompareXML compare;
    if(!compare.loadFileIntoDocument(fileResults, document1)) {
        return error(QString("unable to load file 1: '%1'").arg(fileResults));
    }
    if(!compare.loadFileIntoDocument(&outputData, document2)) {
        return error(QString("unable to load result string :%1").arg(QString::fromUtf8(resultData)));
    }
    bool result = compare.compareDomDocuments(document1, document2);
    if(!result ) {
        compare.dumpErrorCause();
        return error(QString("The documents differ error:'%2' source:%1").arg(QString::fromUtf8(resultData)).arg(compare.errorString()));
    }

    return result ;
}
