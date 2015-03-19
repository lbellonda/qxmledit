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


#include "testreadonly.h"
#include "app.h"
#include "comparexml.h"
#include "editelement.h"
#include "edittextnode.h"

#define FILE_ORIG   "../test/data/readonly.xml"

static bool editHook(QWidget *const parentWindow, QTreeWidgetItem *item, UIDelegate *uiDelegate, Element* element);
static bool editTextHook(QWidget *const parentWindow, QTreeWidgetItem *item, UIDelegate *uiDelegate, Element* element);

static bool isEdited = false;
bool editHook(QWidget *const parentWindow, QTreeWidgetItem * /*item*/, UIDelegate * /*uiDelegate*/, Element* element)
{
    isEdited = true;
    if(NULL == element) {
        return false;
    }
    element->_tag = "testTag";
    EditElement editElementDialog(parentWindow);
    editElementDialog.setTarget(element) ;
    editElementDialog.sendSelect(0);
    editElementDialog.sendAddCommand("newdata");
    editElementDialog.sendSelect(1);
    editElementDialog.sendMoveUpCommand();
    editElementDialog.accept();

    return true ;
}

bool editTextHook(QWidget *const /*parentWindow*/, QTreeWidgetItem * /*item*/, UIDelegate * /*uiDelegate*/, Element* element)
{
    isEdited = true;
    if(NULL == element) {
        return false;
    }
    element->clearTextNodes();
    TextChunk  *newText = new TextChunk(false, "test");
    element->addTextNode(newText);

    return true ;
}

//-------

TestReadOnly::TestReadOnly()
{
}

TestReadOnly::~TestReadOnly()
{
}


//---

bool TestReadOnly::testUnit()
{
    _testName = "TestReadOnly" ;

    App app;
    if(!app.init(true) ) {
        return error("init app");
    }

    if(!checkReadOnlyInitial(app.mainWindow())) {
        return false;
    }
    if( !app.mainWindow()->loadFile(FILE_ORIG) ) {
        return error("loading main file");
    }

    if(!checkSetReadOnly(app.mainWindow())) {
        return false;
    }
    if(!checkReadOnlyModification(app.mainWindow())) {
        return false;
    }
    if(!checkReadOnlyPasteText(app.mainWindow())) {
        return false;
    }
    if(!checkReadOnlyPasteElement(app.mainWindow())) {
        return false;
    }
    if(!checkReadOnlyEdit(app.mainWindow(), true)) {
        return false;
    }
    if(!checkTransformComment(app.mainWindow())) {
        return false;
    }
    if(!checkUnsetReadOnly(app.mainWindow())) {
        return false;
    }
    if(!checkReadOnlyEdit(app.mainWindow(), false)) {
        return false;
    }
    if(!checkSetReadOnly(app.mainWindow())) {
        return false;
    }
    if(!checkOpenFile(app.mainWindow())) {
        return false;
    }
    if(!checkUnsetReadOnly(app.mainWindow())) {
        return false;
    }
    if(!checkNewFromClipboard(app.mainWindow())) {
        return false;
    }
    if(!checkNewFromTemplate(app.mainWindow())) {
        return false;
    }
    if(!checkEditorButtons(app.mainWindow())) {
        return false;
    }
    return true;
}

//---

bool TestReadOnly::checkReadOnlyStatus(MainWindow *wnd, const bool expectedReadOnly)
{
    QAction *readOnlyAction = Utils::findAction(wnd, "actionReadOnly");
    if( NULL == readOnlyAction) {
        return error("Unable to find action 'actionReadOnly'");
    }
    if( readOnlyAction->isChecked() != expectedReadOnly ) {
        return error(QString("Action check (%1) differs from expected (%2)").arg(readOnlyAction->isChecked()).arg(expectedReadOnly));
    }
    QString textExpected = wnd->readOnlyStateText(expectedReadOnly);
    QLabel *label = wnd->readOnlyStatusLabel();
    if( textExpected != label->text()) {
        return error(QString("Expecting status set to '%1', but found '%2' ").arg(label->text()).arg(textExpected));
    }
    if( wnd->isReadOnly() != expectedReadOnly ) {
        return error(QString("Window read only differs, expected '%1', but found '%2' ").arg(expectedReadOnly).arg(wnd->isReadOnly()));
    }
    if( wnd->getEditor()->isReadOnly() != expectedReadOnly ) {
        return error(QString("Editor read only differs, expected '%1', but found '%2' ").arg(expectedReadOnly).arg(wnd->getEditor()->isReadOnly()));
    }
    return true;
}

bool TestReadOnly::compareOriginalData(MainWindow *wnd)
{
    CompareXML compare;
    if( !compare.compareFileWithRegola( wnd->getRegola(), FILE_ORIG) ) {
        return error(QString("The data differs from original file: %1").arg(compare.errorString()));
    }
    return true;
}

bool TestReadOnly::checkReadOnlyInitial(MainWindow *wnd)
{
    _testName = "checkReadOnlyInitial";
    if(! checkReadOnlyStatus(wnd, false)) {
        return false;
    }
    return true;
}

bool TestReadOnly::checkSetReadOnly(MainWindow *wnd)
{
    _testName = "checkSetReadOnly";
    if(! checkReadOnlyStatus(wnd, false)) {
        return false;
    }
    if( !toggleAction(wnd, "actionReadOnly")) {
        return error(QString("Unable to fire action readonly"));
    }
    if(! checkReadOnlyStatus(wnd, true)) {
        return false;
    }
    return true;
}


bool TestReadOnly::checkEditorButtons(MainWindow *wnd)
{
    _testName = "checkReadOnlyButtons";

    if( !wnd->loadFile(FILE_ORIG) ) {
        return error("loading main file");
    }
    QList<int> selPath ;
    selPath << 0;
    selPath << 0;
    selPath << 1;

    Element *selectedElement = wnd->getRegola()->findElementByArray(selPath);
    if( NULL == selectedElement ) {
        return error("no selection");
    }
    wnd->getEditor()->setCurrentItem(selectedElement);

    if(! checkButtonsEnabled(wnd->getEditor(), true)) {
        return false;
    }
    if( !toggleAction(wnd, "actionReadOnly")) {
        return error(QString("Unable to fire action readonly"));
    }
    if(! checkButtonsEnabled(wnd->getEditor(), false)) {
        return false;
    }
    wnd->getEditor()->setCurrentItem(selectedElement);
    if(! checkButtonsEnabled(wnd->getEditor(), false)) {
        return false;
    }
    return true;
}

bool TestReadOnly::checkButtonsEnabled( QWidget *editor, const bool expected)
{
    if( !checkOneWidgetEnabled( editor, "addChild", expected) ) {
        return false;
    }
    if( !checkOneWidgetEnabled( editor, "addBrother", expected) ) {
        return false;
    }
    if( !checkOneWidgetEnabled( editor, "deleteItem", expected) ) {
        return false;
    }
    if( !checkOneWidgetEnabled( editor, "moveUp", expected) ) {
        return false;
    }
    if( !checkOneWidgetEnabled( editor, "moveDown", expected) ) {
        return false;
    }
    return true;
}


bool TestReadOnly::checkReadOnlyModification(MainWindow *wnd)
{
    _testName = "checkReadOnlyModification";
    if(! checkReadOnlyStatus(wnd, true)) {
        return false;
    }
    QList<int> sel;
    sel << 0;
    Element *selectedElement = wnd->getRegola()->findElementByArray(sel);
    if(NULL == selectedElement) {
        return error("no selected element");
    }
    wnd->getEditor()->setCurrentItem(selectedElement);
    if( !fireAction(wnd, "actionCut")) {
        return error("Unable to fire cut");
    }
    if( wnd->getRegola()->isModified()) {
        return error("Regola is modified unexptectedctly");
    }
    if(!compareOriginalData(wnd)) {
        return false;
    }
    if(! checkReadOnlyStatus(wnd, true)) {
        return false;
    }
    return true;
}

bool TestReadOnly::checkReadOnlyPasteText(MainWindow *wnd)
{
    _testName = "checkReadOnlyPasteText";
    if(! checkReadOnlyStatus(wnd, true)) {
        return false;
    }
    QList<int> sel;
    sel << 0;
    Element *selectedElement = wnd->getRegola()->findElementByArray(sel);
    if(NULL == selectedElement) {
        return error("no selected element");
    }
    wnd->getEditor()->setCurrentItem(selectedElement);
    QApplication::clipboard()->setText("<root h=\"h\"/>");
    if( !fireAction(wnd, "actionPaste")) {
        return error("Unable to fire paste");
    }
    if( wnd->getRegola()->isModified()) {
        return error("Regola is modified unexptectedctly");
    }
    if(!compareOriginalData(wnd)) {
        return false;
    }
    if(! checkReadOnlyStatus(wnd, true)) {
        return false;
    }
    return true;
}

bool TestReadOnly::checkReadOnlyPasteElement(MainWindow *wnd)
{
    _testName = "checkReadOnlyPasteElement";
    if(! checkReadOnlyStatus(wnd, true)) {
        return false;
    }
    QList<int> sel;
    sel << 0;
    Element *selectedElement = wnd->getRegola()->findElementByArray(sel);
    if(NULL == selectedElement) {
        return error("no selected element");
    }
    wnd->getEditor()->setCurrentItem(selectedElement);
    QApplication::clipboard()->setText("<root h=\"h\"/>");
    if( !fireAction(wnd, "actionPaste")) {
        return error("Unable to fire paste");
    }
    if( wnd->getRegola()->isModified()) {
        return error("Regola is modified unexptectedctly");
    }
    if(!compareOriginalData(wnd)) {
        return false;
    }
    if(! checkReadOnlyStatus(wnd, true)) {
        return false;
    }
    return true;
}

bool TestReadOnly::checkTransformComment(MainWindow *wnd)
{
    _testName = "checkReadOnlyPasteElement";
    if(! checkReadOnlyStatus(wnd, true)) {
        return false;
    }
    QList<int> sel;
    sel << 0;
    Element *selectedElement = wnd->getRegola()->findElementByArray(sel);
    if(NULL == selectedElement) {
        return error("no selected element");
    }
    wnd->getEditor()->setCurrentItem(selectedElement);
    QApplication::clipboard()->setText("<root h=\"h\"/>");
    if( !fireAction(wnd, "actionTransformInComment")) {
        return error("Unable to fire transform in comment");
    }
    if( wnd->getRegola()->isModified()) {
        return error("Regola is modified unexptectedctly");
    }
    if(!compareOriginalData(wnd)) {
        return false;
    }
    if(! checkReadOnlyStatus(wnd, true)) {
        return false;
    }
    return true;
}

bool TestReadOnly::checkUnsetReadOnly(MainWindow *wnd)
{
    _testName = "checkUnsetReadOnly";
    if( !setReadOnly(wnd, true) ) {
        return error(QString("Unable to fire action readonly"));
    }
    if(! checkReadOnlyStatus(wnd, true)) {
        return false;
    }
    if( !toggleAction(wnd, "actionReadOnly")) {
        return error(QString("Unable to fire action readonly"));
    }
    if(! checkReadOnlyStatus(wnd, false)) {
        return false;
    }
    return true;
}

bool TestReadOnly::setReadOnly(MainWindow *wnd, const bool how)
{
    if( wnd->isReadOnly() != how ) {
        if(! wnd->isReadOnly()) {
            if( !toggleAction(wnd, "actionReadOnly")) {
                return error(QString("Unable to fire action readonly"));
            }
        }
    }
    return true;
}

bool TestReadOnly::checkOpenFile(MainWindow *wnd)
{
    _testName = "checkOpenFile";
    if( !setReadOnly(wnd, true) ) {
        return error(QString("Unable to fire action readonly"));
    }
    if(! checkReadOnlyStatus(wnd, true)) {
        return false;
    }
    if( !wnd->loadFile(FILE_ORIG) ) {
        return error("loading second file");
    }
    if(! checkReadOnlyStatus(wnd, false)) {
        return false;
    }
    return true;
}

bool TestReadOnly::checkReadOnlyEdit(MainWindow *wnd, const bool isReadOnly)
{
    _testName = "checkReadOnlyEdit";
    if( !setReadOnly(wnd, isReadOnly) ) {
        return error(QString("Unable to fire action readonly"));
    }

    if(! checkReadOnlyStatus(wnd, isReadOnly)) {
        return false;
    }
    Regola *regola = wnd->getRegola();
    isEdited = false;
    regola->setEditTextHook(&editTextHook);
    regola->setEditHook(&editHook);
    QList<int> sel;
    sel << 0;
    Element *selectedElement = wnd->getRegola()->findElementByArray(sel);
    if(NULL == selectedElement) {
        return error("no selected element");
    }
    wnd->getEditor()->setCurrentItem(selectedElement);
    if( !fireAction(wnd, "actionEdit")) {
        return error("error firing edit");
    }

    regola->setEditTextHook(NULL);
    regola->setEditHook(NULL);

    if( isEdited == isReadOnly)  {
        return error(QString("Data flag differs, expected: %1, found :%2").arg(isReadOnly).arg(!isEdited));
    }
    if(isReadOnly != compareOriginalData(wnd)) {
        return error(QString("Comparing data with original, expected: %1, found :%2").arg(isReadOnly).arg(!isReadOnly));
    }
    return true;
}

bool TestReadOnly::checkNewFromClipboard(MainWindow *wnd)
{
    _testName = "checkReadOnlyEdit";
    if( !setReadOnly(wnd, true) ) {
        return error(QString("Unable to fire action readonly"));
    }
    if(! checkReadOnlyStatus(wnd, true)) {
        return false;
    }
    QApplication::clipboard()->setText("<root h=\"h\"/>");
    if( !fireAction(wnd, "actionNewFromClipboard")) {
        return error(QString("Unable to fire action actionNewFromClipboard"));
    }

    if(! checkReadOnlyStatus(wnd, false)) {
        return false;
    }
    return true;
}

bool TestReadOnly::checkNewFromTemplate(MainWindow *wnd)
{
    _testName = "checkNewFromTemplate";
    if( !setReadOnly(wnd, true) ) {
        return error(QString("Unable to fire action readonly"));
    }
    if(! checkReadOnlyStatus(wnd, true)) {
        return false;
    }
    if( !fireAction(wnd, "actionNewXSDDocument")) {
        return error(QString("Unable to fire action actionNewXSDDocument"));
    }

    if(! checkReadOnlyStatus(wnd, false)) {
        return false;
    }
    return true;
}
