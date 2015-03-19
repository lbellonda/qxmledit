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


#include "testundoredo.h"
#include "comparexml.h"
#include "editelement.h"

#define     RES_UNDO_BASE   "../test/data/undo/base.xml"
#define     RES_UNDO_MOVEUP1   "../test/data/undo/moveup.xml"
#define     RES_UNDO_MOVEUP2   "../test/data/undo/moveup2.xml"
#define     RES_UNDO_MOVEUP_BUMP1 "../test/data/undo/moveupbump1.xml"

#define     RES_UNDO_MOVEDOWN1   "../test/data/undo/movedown.xml"
#define     RES_UNDO_MOVEDOWN2   "../test/data/undo/movedown2.xml"
#define     RES_UNDO_MOVEDOWN_BUMP1 "../test/data/undo/movedownbump1.xml"

#define     RES_UNDO_BASECOMPLEX   "../test/data/undo/base_complex.xml"
#define     RES_UNDO_DELETE         "../test/data/undo/delete.xml"
#define     RES_UNDO_DELETE_ROOTELEMENT     "../test/data/undo/delete_rootelement.xml"
#define     RES_UNDO_DELETE_ROOTCOMMENT     "../test/data/undo/delete_rootcomment.xml"

#define     RES_UNDO_INS1   "../test/data/undo/ins1.xml"
#define     RES_UNDO_INS2   "../test/data/undo/ins2.xml"
#define     RES_UNDO_INS3   "../test/data/undo/ins3.xml"

#define     RES_UNDO_APPEND1   "../test/data/undo/app1.xml"
#define     RES_UNDO_APPEND2   "../test/data/undo/app2.xml"
#define     RES_UNDO_APPEND3   "../test/data/undo/app3.xml"

#define     RES_UNDO_INSCOMM1   "../test/data/undo/inscomm1.xml"
#define     RES_UNDO_INSCOMM2   "../test/data/undo/inscomm2.xml"

#define     RES_UNDO_INSPI1   "../test/data/undo/inspi1.xml"
#define     RES_UNDO_INSPI2   "../test/data/undo/inspi2.xml"
#define     RES_UNDO_TRCOMM     "../test/data/undo/trcomm.xml"
#define     RES_UNDO_EDIT   "../test/data/undo/edit.xml"
#define     RES_UNDO_EDIT1   "../test/data/undo/edit1.xml"
#define     RES_UNDO_EDIT2   "../test/data/undo/edit1.xml"
#define     RES_UNDO_EDIT3   "../test/data/undo/edit1.xml"
#define     RES_UNDO_EDIT4   "../test/data/undo/edit1.xml"

#define     RES_UNDO_AFTERPASTE "../test/data/undo/paste.xml"
#define     RES_UNDO_MIXED "../test/data/undo/mixed.xml"

//------------------------------------------------------------

static bool editHook(QWidget *const parentWindow, QTreeWidgetItem *item, UIDelegate *uiDelegate, Element* element);
static bool editTextHook(QWidget *const parentWindow, QTreeWidgetItem *item, UIDelegate *uiDelegate, Element* element);

//------------------------------------------------------------

TestUndoRedo::TestUndoRedo()
{
    selectedElement = NULL;
    regola = NULL ;
}

bool TestUndoRedo::error(const QString &theCause)
{
    cause = theCause ;
    return false ;
}

bool TestUndoRedo::error()
{
    return false ;
}

bool TestUndoRedo::test()
{
    {
        TestUndoRedo tur;
        if( !tur.testMoveUp1() ) {
            return error("testMoveUp1");
        }
    }
    {
        TestUndoRedo tur;
        if( !tur.testMoveUp2() ) {
            return error("testMoveUp2");
        }
    }
    {
        TestUndoRedo tur;
        if( !tur.testMoveUp2W() ) {
            return error("testMoveUp2W");
        }
    }
    {
        TestUndoRedo tur;
        if( !tur.testMoveUpBumper() ) {
            return error("testMoveDownBumper");
        }
    }
    //----------------------------------
    {
        TestUndoRedo tur;
        if( !tur.testMoveDown1() ) {
            return error("testMoveDown1");
        }
    }
    {
        TestUndoRedo tur;
        if( !tur.testMoveDown2() ) {
            return error("testMoveDown2");
        }
    }
    {
        TestUndoRedo tur;
        if( !tur.testMoveDown2W() ) {
            return error("testMoveDown2W");
        }
    }
    {
        TestUndoRedo tur;
        if( !tur.testMoveDownBumper() ) {
            return error("testMoveDownBumper");
        }
    }
    //-------------------------------------------
    {
        TestUndoRedo tur;
        if( !tur.testDelete() ) {
            return error("testDelete");
        }
    }
    {
        TestUndoRedo tur;
        if( !tur.testDeleteRootComment() ) {
            return error("testDeleteRootComment");
        }
    }
    {
        TestUndoRedo tur;
        if( !tur.testDeleteRootElement() ) {
            return error("testDeleteRootElement");
        }
    }
    {
        TestUndoRedo tur;
        if( !tur.testCut() ) {
            return error("testCut");
        }
    }
    //--------------------------------------
    {
        TestUndoRedo tur;
        if( !tur.testInsert1() ) {
            return error("testInsert1");
        }
    }
    {
        TestUndoRedo tur;
        if( !tur.testInsert2() ) {
            return error("testInsert2");
        }
    }
    {
        TestUndoRedo tur;
        if( !tur.testInsert3() ) {
            return error("testInsert3");
        }
    }
    {
        TestUndoRedo tur;
        if( !tur.testAppend1() ) {
            return error("testAppend");
        }
    }
    {
        TestUndoRedo tur;
        if( !tur.testAppend2() ) {
            return error("testAppend2");
        }
    }
    {
        TestUndoRedo tur;
        if( !tur.testAppend3() ) {
            return error("testAppend3");
        }
    }
    {
        TestUndoRedo tur;
        if( !tur.testAddComment() ) {
            return error("testAddComment");
        }
    }
    {
        TestUndoRedo tur;
        if( !tur.testAppendComment() ) {
            return error("testAppendComment");
        }
    }
    {
        TestUndoRedo tur;
        if( !tur.testAddProcessingInstruction() ) {
            return error("testAddProcessingInstruction");
        }
    }
    {
        TestUndoRedo tur;
        if( !tur.testAppendProcessingInstruction() ) {
            return error("testAppendProcessingInstruction");
        }
    }

    {
        TestUndoRedo tur;
        if( !tur.testTransformInComment() ) {
            return error("testTransformInComment");
        }
    }

    {
        TestUndoRedo tur;
        if( !tur.testEdit() ) {
            return error("testEdit");
        }
    }

    {
        TestUndoRedo tur;
        if( !tur.testPaste() ) {
            return error("testPaste");
        }
    }

    {
        TestUndoRedo tur;
        if( !tur.testEditTextNode1() ) {
            return error("testEditTextNode1");
        }
    }

    {
        TestUndoRedo tur;
        if( !tur.testEditTextNode2() ) {
            return error("testEditTextNode2");
        }
    }

    {
        TestUndoRedo tur;
        if( !tur.testEditTextNode3() ) {
            return error("testEditTextNode3");
        }
    }

    {
        TestUndoRedo tur;
        if( !tur.testEditTextNode4() ) {
            return error("testEditTextNode4");
        }
    }

    {
        TestUndoRedo tur;
        if( !tur.testMixed() ) {
            return error("testmixed");
        }
    }
    //----------------------------------
    return true;
}


bool TestUndoRedo::start(const bool isNew, QList<int> &selection, const QString &fileName)
{
    if(!app.init() ) {
        return error();
    }
    if( !isNew ) {
        app.mainWindow()->loadFile(fileName);
    }
    regola = app.mainWindow()->getRegola();

    if( !selection.isEmpty()) {
        selectedElement = regola->findElementByArray(selection);
        if( NULL != selectedElement ) {
            app.mainWindow()->getEditor()->setCurrentItem(selectedElement);
        }
    }
    return true;
}

bool TestUndoRedo::start(const bool isNew, QList<int> &selection)
{
    return start(isNew, selection, RES_UNDO_BASE);
}


bool TestUndoRedo::select(QList<int> &selection)
{
    bool isOk = false;
    if( !selection.isEmpty()) {
        selectedElement = regola->findElementByArray(selection);
        if( NULL != selectedElement ) {
            app.mainWindow()->getEditor()->setCurrentItem(selectedElement);
            isOk = true ;
        }
    }
    return isOk ;
}


    //TODO testInsert()
/*
load   0  0
ins    1  0
ins    2  0
-cmp
undo   1  1
-cmp
undo   0  2
-cmp
redo   1  1
-cmp
undo   0  2
-cmp
undo  0   2
*/


bool TestUndoRedo::compareDocuments(const QString &filename, Regola *regola)
{
    QByteArray resultData = regola->writeMemory();
    QDomDocument document1;
    QDomDocument document2;
    CompareXML compare;
    if(!compare.loadFileIntoDocument(filename, document1)) {
        return error();
    }
    QBuffer outputData(&resultData);
    if(!compare.loadFileIntoDocument(&outputData, document2)) {
        return error();
    }
    bool result = compare.compareDomDocuments(document1, document2);
    if( !result ) {
        return error();
    }
    return result ;
}

bool TestUndoRedo::checkAfterMove(const bool isUndo, const bool isRedo, const QString &docName)
{
    if( regola->canUndo() != isUndo ) {
        return error();
    }

    if( app.mainWindow()->getUI()->actionUndo->isEnabled() != isUndo ) {
        return error();
    }

    if( regola->canRedo() != isRedo ) {
        return error();
    }

    if( app.mainWindow()->getUI()->actionRedo->isEnabled() != isRedo ) {
        return error();
    }

    if( !compareDocuments(docName, regola) ) {
        return error();
    }
    if( NULL != selectedElement ) {
        if( !regola->checkIfUIItemsCorrespondToElements(selectedElement)) {
            return error();
        }
    }
    return true;
}

bool TestUndoRedo::testMoveUp1()
{
    QList<int> sel;
    sel.append(0);
    sel.append(2);
    if( !start(false, sel) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASE) ) {
        return error();
    }

    if( !regola->moveUp(selectedElement) ) {
        return error();
    }

    if( !checkAfterMove(true, false, RES_UNDO_MOVEUP1) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASE) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, false, RES_UNDO_MOVEUP1) ) {
        return error();
    }

    return true ;
}

bool TestUndoRedo::testMoveUp2()
{
    QList<int> sel;
    sel.append(0);
    sel.append(2);
    if( !start(false, sel) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASE) ) {
        return error();
    }

    if( !regola->moveUp(selectedElement) ) {
        return error();
    }

    if( !checkAfterMove(true, false, RES_UNDO_MOVEUP1) ) {
        return error();
    }

    if( !regola->moveUp(selectedElement) ) {
        return error();
    }

    if( !checkAfterMove(true, false, RES_UNDO_MOVEUP2) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(true, true, RES_UNDO_MOVEUP1) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASE) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, true, RES_UNDO_MOVEUP1) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, false, RES_UNDO_MOVEUP2) ) {
        return error();
    }

    return true ;
}

bool TestUndoRedo::testMoveUp2W()
{
    QList<int> sel;
    sel.append(0);
    sel.append(2);
    if( !start(false, sel) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASE) ) {
        return error();
    }

    app.mainWindow()->getEditor()->onActionMoveUp();

    if( !checkAfterMove(true, false, RES_UNDO_MOVEUP1) ) {
        return error();
    }

    app.mainWindow()->getEditor()->onActionMoveUp();

    if( !checkAfterMove(true, false, RES_UNDO_MOVEUP2) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(true, true, RES_UNDO_MOVEUP1) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASE) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, true, RES_UNDO_MOVEUP1) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, false, RES_UNDO_MOVEUP2) ) {
        return error();
    }

    return true ;
}

// 1-up
// 2-bump
// 3-undo bump
// 3- undo
// 4-redo
bool TestUndoRedo::testMoveUpBumper()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    if( !start(false, sel) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASE) ) {
        return error();
    }
    //1
    if( !regola->moveUp(selectedElement) ) {
        return error();
    }

    if( !checkAfterMove(true, false, RES_UNDO_MOVEUP_BUMP1) ) {
        return error();
    }
    //2
    if( regola->moveUp(selectedElement) ) {
        return error();
    }

    if( !checkAfterMove(true, false, RES_UNDO_MOVEUP_BUMP1) ) {
        return error();
    }
    //3 - fake undo, no real move, but undo stack count
    regola->undo();

    if( !checkAfterMove(true, true, RES_UNDO_MOVEUP_BUMP1) ) {
        return error();
    }
    //3 - undo
    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASE) ) {
        return error();
    }
    //4
    regola->redo();

    if( !checkAfterMove(true, true, RES_UNDO_MOVEUP_BUMP1) ) {
        return error();
    }
    return true ;
}

//-------------------------------

bool TestUndoRedo::testMoveDown1()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    if( !start(false, sel) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASE) ) {
        return error();
    }

    if( !regola->moveDown(selectedElement) ) {
        return error();
    }

    if( !checkAfterMove(true, false, RES_UNDO_MOVEDOWN1) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASE) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, false, RES_UNDO_MOVEDOWN1) ) {
        return error();
    }

    return true ;
}

bool TestUndoRedo::testMoveDown2()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    if( !start(false, sel) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASE) ) {
        return error();
    }

    if( !regola->moveDown(selectedElement) ) {
        return error();
    }

    if( !checkAfterMove(true, false, RES_UNDO_MOVEDOWN1) ) {
        return error();
    }

    if( !regola->moveDown(selectedElement) ) {
        return error();
    }

    if( !checkAfterMove(true, false, RES_UNDO_MOVEDOWN2) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(true, true, RES_UNDO_MOVEDOWN1) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASE) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, true, RES_UNDO_MOVEDOWN1) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, false, RES_UNDO_MOVEDOWN2) ) {
        return error();
    }

    return true ;
}

bool TestUndoRedo::testMoveDown2W()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    if( !start(false, sel) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASE) ) {
        return error();
    }

    app.mainWindow()->getEditor()->onActionMoveDown();

    if( !checkAfterMove(true, false, RES_UNDO_MOVEDOWN1) ) {
        return error();
    }

    app.mainWindow()->getEditor()->onActionMoveDown();

    if( !checkAfterMove(true, false, RES_UNDO_MOVEDOWN2) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(true, true, RES_UNDO_MOVEDOWN1) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASE) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, true, RES_UNDO_MOVEDOWN1) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, false, RES_UNDO_MOVEDOWN2) ) {
        return error();
    }

    return true ;
}

// 1-up
// 2-bump
// 3-undo bump
// 3- undo
// 4-redo
bool TestUndoRedo::testMoveDownBumper()
{
    QList<int> sel;
    sel.append(0);
    sel.append(5);
    if( !start(false, sel) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASE) ) {
        return error();
    }
    //1
    if( !regola->moveDown(selectedElement) ) {
        return error();
    }

    if( !checkAfterMove(true, false, RES_UNDO_MOVEDOWN_BUMP1) ) {
        return error();
    }
    //2
    if( regola->moveDown(selectedElement) ) {
        return error();
    }

    if( !checkAfterMove(true, false, RES_UNDO_MOVEDOWN_BUMP1) ) {
        return error();
    }
    //3 - fake undo, no real move, but undo stack count
    regola->undo();

    if( !checkAfterMove(true, true, RES_UNDO_MOVEDOWN_BUMP1) ) {
        return error();
    }
    //3 - undo
    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASE) ) {
        return error();
    }
    //4
    regola->redo();

    if( !checkAfterMove(true, true, RES_UNDO_MOVEDOWN_BUMP1) ) {
        return error();
    }
    return true ;
}

//-------------------------------------------------------

bool TestUndoRedo::testDelete()
{
    QList<int> sel;
    sel.append(0);
    sel.append(4);
    sel.append(0);
    if( !start(false, sel, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    if( !regola->deleteElement(selectedElement) ) {
        return error();
    }
    selectedElement = NULL ;
    if( !checkAfterMove(true, false, RES_UNDO_DELETE) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, false, RES_UNDO_DELETE) ) {
        return error();
    }

    return true ;
}

//-------------------------------------------------------

bool TestUndoRedo::testCut()
{
    QList<int> sel;
    sel.append(0);
    sel.append(4);
    sel.append(0);
    if( !start(false, sel, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    app.mainWindow()->getEditor()->onActionCut();

    selectedElement = NULL ;
    if( !checkAfterMove(true, false, RES_UNDO_DELETE) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, false, RES_UNDO_DELETE) ) {
        return error();
    }

    return true ;
}

//-------------------------------------------------------

bool TestUndoRedo::testDeleteRootElement()
{
    QList<int> sel;
    sel.append(0);
    if( !start(false, sel, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    if( !regola->deleteElement(selectedElement) ) {
        return error();
    }
    selectedElement = NULL ;
    if( regola->getChildItems()->at(0)->getType() != Element::ET_COMMENT ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->redo();

    if( regola->getChildItems()->at(0)->getType() != Element::ET_COMMENT ) {
        return error();
    }

    return true ;
}

//-------------------------------------------------------

bool TestUndoRedo::testDeleteRootComment()
{
    QList<int> sel;
    sel.append(1);
    if( !start(false, sel, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    if( !regola->deleteElement(selectedElement) ) {
        return error();
    }
    selectedElement = NULL ;
    if( !checkAfterMove(true, false, RES_UNDO_DELETE_ROOTCOMMENT) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, false, RES_UNDO_DELETE_ROOTCOMMENT) ) {
        return error();
    }

    return true ;
}

//-------------------------------------------------------

Element *TestUndoRedo::newElement()
{
    Element *newElement = new Element( "new", "", regola, NULL ) ;
    return newElement ;
}

Element *TestUndoRedo::newElementWithTextAndAttributes()
{
    Element *newElement = new Element( "new1", "", regola, NULL ) ;
    newElement->addAttribute("newattr1", "a1");
    newElement->addAttribute("newattr2", "a2");
    newElement->addTextNode(new TextChunk(false, "simple text"));
    return newElement ;
}

Element *TestUndoRedo::newElementWithTextAndAttributesComplex()
{
    Element *newElement = new Element( "new1", "", regola, NULL ) ;
    newElement->addAttribute("newattr1", "a1");
    newElement->addAttribute("newattr2", "a2");
    Element *newElementText1 = new Element( regola, Element::ET_TEXT, newElement ) ;
    newElementText1->text = "text1";
    Element *newElementText2 = new Element( regola, Element::ET_TEXT, newElement ) ;
    newElementText2->text = "text2";
    newElementText2->_isCData = true ;
    Element *newElementChild = new Element( "new2", "", regola, newElement ) ;
    newElement->getItems().append(newElementText1);
    newElement->getItems().append(newElementChild);
    newElement->getItems().append(newElementText2);
    return newElement ;
}

bool TestUndoRedo::testInsert1()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    if( !start(false, sel, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }
    Element *new1 = newElement();
    regola->addBrother(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), new1);

    if( !checkAfterMove(true, false, RES_UNDO_INS1) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, false, RES_UNDO_INS1) ) {
        return error();
    }

    return true ;
}

bool TestUndoRedo::testInsert2()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    sel.append(0);
    sel.append(0);
    if( !start(false, sel, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }
    Element *new1 = newElementWithTextAndAttributes();
    regola->addBrother(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), new1);

    if( !checkAfterMove(true, false, RES_UNDO_INS2) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, false, RES_UNDO_INS2) ) {
        return error();
    }

    return true ;
}

//-------------------------------------------------------

bool TestUndoRedo::testInsert3()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    sel.append(0);
    sel.append(0);
    if( !start(false, sel, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }
    Element *new1 = newElementWithTextAndAttributesComplex();
    regola->addBrother(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), new1);

    if( !checkAfterMove(true, false, RES_UNDO_INS3) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, false, RES_UNDO_INS3) ) {
        return error();
    }

    return true ;
}

//-------------------------------------------------------

bool TestUndoRedo::testAppend1()
{
    QList<int> sel;
    sel.append(0);
    if( !start(false, sel, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }
    Element *new1 = newElement();
    regola->addChild(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), new1);

    if( !checkAfterMove(true, false, RES_UNDO_APPEND1) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, false, RES_UNDO_APPEND1) ) {
        return error();
    }

    return true ;
}

bool TestUndoRedo::testAppend2()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    sel.append(0);
    if( !start(false, sel, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }
    Element *new1 = newElementWithTextAndAttributes();
    regola->addChild(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), new1);

    if( !checkAfterMove(true, false, RES_UNDO_APPEND2) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, false, RES_UNDO_APPEND2) ) {
        return error();
    }

    return true ;
}

//-------------------------------------------------------

bool TestUndoRedo::testAppend3()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    sel.append(0);
    if( !start(false, sel, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }
    Element *new1 = newElementWithTextAndAttributesComplex();
    regola->addChild(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), new1);

    if( !checkAfterMove(true, false, RES_UNDO_APPEND3) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, false, RES_UNDO_APPEND3) ) {
        return error();
    }

    return true ;
}

//-------------------------------------------------------

bool TestUndoRedo::testAppendProcessingInstruction()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    if( !start(false, sel, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    Element *newPI = regola->newElement(Element::ET_PROCESSING_INSTRUCTION);
    newPI->setPITarget("textTarget");
    newPI->setPIData("textData");

    regola->appendProcessingInstruction(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newPI);

    if( !checkAfterMove(true, false, RES_UNDO_INSPI2) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, false, RES_UNDO_INSPI2) ) {
        return error();
    }
    return true ;
}

//-------------------------------------------------------

bool TestUndoRedo::testAddProcessingInstruction()
{
    QList<int> sel;
    sel.append(0);
    if( !start(false, sel, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    Element *newPI = regola->newElement(Element::ET_PROCESSING_INSTRUCTION);
    newPI->setPITarget("textTarget");
    newPI->setPIData("textData");

    regola->addProcessingInstruction(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newPI);

    if( !checkAfterMove(true, false, RES_UNDO_INSPI1) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, false, RES_UNDO_INSPI1) ) {
        return error();
    }
    return true ;
}

//-------------------------------------------------------

bool TestUndoRedo::testAppendComment()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    if( !start(false, sel, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    Element *newComment = regola->newElement(Element::ET_COMMENT);
    newComment->setComment("the comment");

    regola->appendComment(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newComment);

    if( !checkAfterMove(true, false, RES_UNDO_INSCOMM1) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, false, RES_UNDO_INSCOMM1) ) {
        return error();
    }
    return true ;
}

//-------------------------------------------------------

bool TestUndoRedo::testAddComment()
{
    QList<int> sel;
    sel.append(0);
    if( !start(false, sel, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    Element *newComment = regola->newElement(Element::ET_COMMENT);
    newComment->setComment("the comment");

    regola->addComment(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newComment);

    if( !checkAfterMove(true, false, RES_UNDO_INSCOMM2) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, false, RES_UNDO_INSCOMM2) ) {
        return error();
    }
    return true ;
}

//-------------------------------------------------------

bool TestUndoRedo::testTransformInComment()
{
    QList<int> sel;
    sel.append(0);
    sel.append(6);
    if( !start(false, sel, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->transformInComment(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), selectedElement);
    selectedElement = NULL ;
    if( !checkAfterMove(false, false, RES_UNDO_TRCOMM) ) {
        return error();
    }
    selectedElement = regola->findElementByArray(sel);
    if( ! regola->generateFromComment(app.mainWindow()->getMainTreeWidget(), new FakeUIDelegate(), selectedElement) ) {
        return error();
    }
    selectedElement = NULL;
    if( !checkAfterMove(false, false, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    return true ;
}

//-------------------------------------------------------

bool TestUndoRedo::testEdit()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    if( !start(false, sel, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->setEditHook(&editHook);
    if( ! regola->editElement(app.mainWindow(), selectedElement->getUI(), new FakeUIDelegate()) ) {
        return error();
    }

    if( !checkAfterMove(true, false, RES_UNDO_EDIT) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, false, RES_UNDO_EDIT) ) {
        return error();
    }
    return true ;
}

//-------------------------------------------------------

bool TestUndoRedo::testPaste()
{
    QList<int> sel;
    sel.append(0);
    sel.append(0);
    if( !start(false, sel, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    Element * pElement = selectedElement->copyToClipboard();

    if( NULL == pElement ) {
        return error();
    }
    QList<int> sel2;
    sel2.append(0);
    sel2.append(2);
    Element *elementToPasteTo = regola->findElementByArray(sel2);

    app.mainWindow()->getEditor()->setCurrentItem(elementToPasteTo);

    regola->paste( app.mainWindow()->getMainTreeWidget(), pElement );

    if( !checkAfterMove(true, false, RES_UNDO_AFTERPASTE) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, false, RES_UNDO_AFTERPASTE) ) {
        return error();
    }
    return true ;
}

//-------------------------------------------------------

bool TestUndoRedo::testEditTextNode1()
{
    QList<int> sel;
    sel.append(0);
    sel.append(3);
    sel.append(1);
    sel.append(1);
    if( !start(false, sel, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->setEditTextHook(&editTextHook);
    if( ! regola->editAndSubstituteTextInNodeElement(app.mainWindow(), selectedElement, new FakeUIDelegate()) ) {
        return error();
    }

    if( !checkAfterMove(true, false, RES_UNDO_EDIT1) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, false, RES_UNDO_EDIT1) ) {
        return error();
    }
    return true ;
}

//-------------------------------------------------------

bool TestUndoRedo::testEditTextNode2()
{
    QList<int> sel;
    sel.append(0);
    sel.append(3);
    sel.append(1);
    sel.append(1);
    if( !start(false, sel, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->setEditTextHook(&editTextHook);
    if( ! regola->editTextNodeElementBase64(app.mainWindow(), selectedElement->getUI(), new FakeUIDelegate()) ) {
        return error();
    }

    if( !checkAfterMove(true, false, RES_UNDO_EDIT2) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, false, RES_UNDO_EDIT2) ) {
        return error();
    }
    return true ;
}

//-------------------------------------------------------

bool TestUndoRedo::testEditTextNode3()
{
    QList<int> sel;
    sel.append(0);
    sel.append(3);
    sel.append(1);
    sel.append(1);
    if( !start(false, sel, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->setEditTextHook(&editTextHook);
    if( ! regola->editInnerXMLBase64Element(selectedElement->getUI(), new FakeUIDelegate()) ) {
        return error();
    }

    if( !checkAfterMove(true, false, RES_UNDO_EDIT3) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, false, RES_UNDO_EDIT3) ) {
        return error();
    }
    return true ;
}

//-------------------------------------------------------

bool TestUndoRedo::testEditTextNode4()
{
    QList<int> sel;
    sel.append(0);
    sel.append(3);
    sel.append(1);
    sel.append(1);
    if( !start(false, sel, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->setEditTextHook(&editTextHook);
    if( ! regola->editInnerXMLElement(selectedElement->getUI(), new FakeUIDelegate()) ) {
        return error();
    }

    if( !checkAfterMove(true, false, RES_UNDO_EDIT4) ) {
        return error();
    }

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->redo();

    if( !checkAfterMove(true, false, RES_UNDO_EDIT4) ) {
        return error();
    }
    return true ;
}

//-------------------------------------------------------

bool TestUndoRedo::testMixed()
{
    QList<int> sel;
    sel.append(0);
    sel.append(3);
    sel.append(1);
    if( !start(false, sel, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    if( !checkAfterMove(false, false, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    if( !regola->moveDown(selectedElement) ) {
        return error();
    }

    QList<int> sel2;
    sel2.append(0);
    sel2.append(3);
    sel2.append(1);
    Element *elementToEdit = regola->findElementByArray(sel2);
    app.mainWindow()->getEditor()->setCurrentItem(elementToEdit);

    regola->setEditHook(&editHook);
    if( ! regola->editElement(app.mainWindow(), elementToEdit->getUI(), new FakeUIDelegate()) ) {
        return error();
    }

    QList<int> sel3;
    sel3.append(0);
    sel3.append(3);
    sel3.append(3);

    Element *elementToCopy = regola->findElementByArray(sel3);
    Element * copiedElement = elementToCopy->copyToClipboard();

    QList<int> sel4;
    sel4.append(0);
    sel4.append(3);
    sel4.append(4);
    Element *elementToPasteTo = regola->findElementByArray(sel4);
    app.mainWindow()->getEditor()->setCurrentItem(elementToPasteTo);

    regola->paste( app.mainWindow()->getMainTreeWidget(), copiedElement );

    QList<int> sel5;
    sel5.append(0);
    sel5.append(4);
    Element *elementDelete = regola->findElementByArray(sel5);
    if( !regola->deleteElement(elementDelete) ) {
        return error();
    }

    if( !checkAfterMove(true, false, RES_UNDO_MIXED) ) {
        return error();
    }

    regola->undo();

    regola->undo();

    regola->undo();

    regola->undo();

    if( !checkAfterMove(false, true, RES_UNDO_BASECOMPLEX) ) {
        return error();
    }

    regola->redo();

    regola->redo();

    regola->redo();

    regola->redo();

    if( !checkAfterMove(true, false, RES_UNDO_MIXED) ) {
        return error();
    }

    return true ;

}


bool editHook(QWidget *const parentWindow, QTreeWidgetItem * /*item*/, UIDelegate * /*uiDelegate*/, Element* element)
{
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
    if(NULL == element) {
        return false;
    }

    element->clearTextNodes();
    TextChunk  *newText = new TextChunk(false, "test");
    element->addTextNode(newText);

    return true ;
}
