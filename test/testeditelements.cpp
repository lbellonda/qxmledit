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


#include "testeditelements.h"
#include "comparexml.h"

#define EDIT_ELEMENTS_BASE  "../test/data/editelements/base.xml"
#define RES_INSERTCOMMENTEMPTY "../test/data/editelements/insCommEmpty.xml"
#define RES_INSERTPITEMPTY "../test/data/editelements/insPIEmpty.xml"

#define RES_APPENDCOMMENTATROOT "../test/data/editelements/appCommAtRoot.xml"
#define RES_APPENDPIATROOT  "../test/data/editelements/appPIAtRoot.xml"

#define RES_APPENDCOMMENTUNDERROOT "../test/data/editelements/appCommUnderRoot.xml"
#define RES_APPENDPIUNDERROOT "../test/data/editelements/appPIUnderRoot.xml"
#define RES_APPEND_ELEM_UNDERROOT "../test/data/editelements/appElUnderRoot.xml"

#define RES_INSCOMM_CHILD   "../test/data/editelements/insCommChild.xml"
#define RES_INSPI_CHILD "../test/data/editelements/insPIChild.xml"
#define RES_INSELEM_CHILD   "../test/data/editelements/insElemChild.xml"
#define RES_INSELEM_CHILD_ATTRTEXT  "../test/data/editelements/insElemChildAttrText.xml"

#define RES_APPCOMM_SIBLING   "../test/data/editelements/appCommSibling.xml"
#define RES_APPPI_SIBLING    "../test/data/editelements/appPISibling.xml"
#define RES_APPELEM_SIBLING "../test/data/editelements/appElemSibling.xml"
#define RES_APPELEM_SIBLING_ATTR "../test/data/editelements/appElemSiblingAttr.xml"
#define RES_APPELEM_SIBLING_ATTRTEXT    "../test/data/editelements/appElemSiblingAttrText.xml"

#define EDIT_ELEMENTS_EL_BASE   "../test/data/editelements/baseEl.xml"
#define RES_ELEMENTENTEMPTY "../test/data/editelements/insElementEmpty.xml"

#define RES_INSELEM_CHILD_ATTR  "../test/data/editelements/appElemChildAttr.xml"


TestEditElements::TestEditElements()
{
    selectedElement = NULL;
    regola = NULL ;
}

bool TestEditElements::error()
{
    return false;
}

bool TestEditElements::test()
{
    {
        TestEditElements test;
        if( !test.insertCommentEmpty() ) {
            return error();
        }
    }
    {
        TestEditElements test;
        if( !test.insertCommentAtRootNoSel() ) {
            return error();
        }
    }
    {
        TestEditElements test;
        if( !test.appendCommentToRoot() ) {
            return error();
        }
    }
    {
        TestEditElements test;
        if( !test.insertCommentUnderRoot() ) {
            return error();
        }
    }
    {
        TestEditElements test;
        if( !test.appendCommentAsSibling() ) {
            return error();
        }
    }
    //------------------
    {
        TestEditElements test;
        if( !test.insertProcessingInstructionEmpty() ) {
            return error();
        }
    }
    {
        TestEditElements test;
        if( !test.insertProcessingInstructionAtRootNoSel() ) {
            return error();
        }
    }
    {
        TestEditElements test;
        if( !test.appendProcessingInstructionToRoot() ) {
            return error();
        }
    }
    {
        TestEditElements test;
        if( !test.insertProcessingInstructionUnderRoot() ) {
            return error();
        }
    }
    {
        TestEditElements test;
        if( !test.appendProcessingInstructionAsSibling() ) {
            return error();
        }
    }

    //------------------

    {
        TestEditElements test;
        if( !test.insertElementEmpty() ) {
            return error();
        }
    }
    /*{
        TestEditElements test;
        if( !test.insertElementAtRootNoSel() ) {
            return error();
        }
    }*/
    {
        TestEditElements test;
        if( !test.insertElementUnderRoot() ) {
            return error();
        }
    }
    {
        TestEditElements test;
        if( !test.insertElementAsChild() ) {
            return error();
        }
    }
   {
        TestEditElements test;
        if( !test.appendElementAsSibling() ) {
            return error();
        }
    }
    {
        TestEditElements test;
        if( !test.insertElementAsChildWithAttributes() ) {
            return error();
        }
    }
    {
        TestEditElements test;
        if( !test.appendElementAsSiblingWithAttributes() ) {
            return error();
        }
    }
    {
        TestEditElements test;
        if( !test.insertElementAsChildWithAttributesAndText() ) {
            return error();
        }
    }
    {
        TestEditElements test;
        if( !test.appendElementAsSiblingWithAttributesAndText() ) {
            return error();
        }
    }

    return true;
}

bool TestEditElements::compareDocuments(const QString &filename, Regola *regola)
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

bool TestEditElements::start(const bool isNew, QList<int> &selection)
{
    if(!app.init() ) {
        return error();
    }
    if( !isNew ) {
        app.mainWindow()->loadFile(EDIT_ELEMENTS_BASE);
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

bool TestEditElements::compare(const QString &referenceFileName)
{
    if( !compareDocuments(referenceFileName, app.mainWindow()->getRegola()) ) {
        return error();
    }
    return true;
}


bool TestEditElements::insertCommentEmpty()
{
    QList<int> sel;
    if( !start(true, sel) ) {
        return error();
    }
    Element *newComment = new Element( regola, Element::ET_COMMENT, NULL) ;
    if(NULL == newComment) {
        return error();
    }
    newComment->setText("new comment");
    regola->addComment(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newComment);

    Element *newElement = new Element( "a", "", regola, NULL ) ;
    if(NULL == newElement) {
        return error();
    }
    regola->addBrother(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    if( !compareDocuments(RES_INSERTCOMMENTEMPTY, regola) ) {
        return error();
    }
    return true ;
}

//--------------------------------------------- FINO QUI -----------------------------------------------

// this does no insert since there is no selection
bool TestEditElements::insertCommentAtRootNoSel()
{
    QList<int> sel;
    if( !start(false, sel) ) {
        return error();
    }
    Element *newComment = new Element( regola, Element::ET_COMMENT, NULL) ;
    if(NULL == newComment) {
        return error();
    }
    newComment->setText("new comment");
    regola->addComment(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newComment);

    Element *newElement = new Element( "a", "", regola, NULL ) ;
    if(NULL == newElement) {
        return error();
    }
    regola->addBrother(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    if( !compareDocuments(EDIT_ELEMENTS_BASE, regola) ) {
        return error();
    }
    return true ;
}

bool TestEditElements::insertCommentUnderRoot()
{
    QList<int> sel;
    sel.append(0);
    if( !start(false, sel) ) {
        return error();
    }
    Element *newComment = new Element( regola, Element::ET_COMMENT, NULL) ;
    if(NULL == newComment) {
        return error();
    }
    newComment->setText("new comment");
    regola->addComment(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newComment);

    if( !compareDocuments(RES_APPENDCOMMENTUNDERROOT, regola) ) {
        return error();
    }
    return true ;
}

bool TestEditElements::appendCommentToRoot()
{
    QList<int> sel;
    sel.append(0);
    if( !start(false, sel) ) {
        return error();
    }
    Element *newComment = new Element( regola, Element::ET_COMMENT, NULL) ;
    if(NULL == newComment) {
        return error();
    }
    newComment->setText("new comment");
    regola->appendComment(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newComment);

    if( !compareDocuments(RES_APPENDCOMMENTATROOT, regola) ) {
        return error();
    }
    return true ;
}


bool TestEditElements::insertCommentAsChild()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    if( !start(false, sel) ) {
        return error();
    }
    Element *newComment = new Element( regola, Element::ET_COMMENT, NULL) ;
    if(NULL == newComment) {
        return error();
    }
    newComment->setText("new comment");
    regola->addComment(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newComment);

    if( !compareDocuments(RES_INSCOMM_CHILD, regola) ) {
        return error();
    }
    return true ;
}

bool TestEditElements::appendCommentAsSibling()
{
    QList<int> sel;
    sel.append(0);
    sel.append(3);
    sel.append(0);
    if( !start(false, sel) ) {
        return error();
    }
    Element *newComment = new Element( regola, Element::ET_COMMENT, NULL) ;
    if(NULL == newComment) {
        return error();
    }
    newComment->setText("new comment");
    regola->appendComment(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newComment);

    if( !compareDocuments(RES_APPCOMM_SIBLING, regola) ) {
        return error();
    }
    return true ;
}

//--------------------------------

Element *TestEditElements::newProcessingInstruction()
{
    Element *newProcessingInstr = new Element( regola, Element::ET_PROCESSING_INSTRUCTION, NULL) ;
    if(NULL == newProcessingInstr) {
        error();
        return NULL;
    }
    newProcessingInstr->setPIData("PIData");
    newProcessingInstr->setPITarget("PITarget");
    return newProcessingInstr;
}

Element *TestEditElements::createElement()
{
    Element *element = new Element("tag", "", regola, NULL);
    if(NULL == element) {
        error();
        return NULL;
    }
    return element;
}


bool TestEditElements::insertProcessingInstructionEmpty()
{
    QList<int> sel;
    if( !start(true, sel) ) {
        return error();
    }
    Element *newPI = newProcessingInstruction();
    if(NULL == newPI) {
        return error();
    }
    regola->addProcessingInstruction(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newPI);

    Element *newElement = new Element( "a", "", regola, NULL ) ;
    if(NULL == newElement) {
        return error();
    }
    regola->addBrother(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    if( !compareDocuments(RES_INSERTPITEMPTY, regola) ) {
        return error();
    }
    return true ;
}

// this does not insert since there is no selection
bool TestEditElements::insertProcessingInstructionAtRootNoSel()
{
    QList<int> sel;
    if( !start(false, sel) ) {
        return error();
    }
    Element *newPI = newProcessingInstruction();
    if(NULL == newPI) {
        return error();
    }
    regola->addProcessingInstruction(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newPI);

    Element *newElement = new Element( "a", "", regola, NULL ) ;
    if(NULL == newElement) {
        return error();
    }
    regola->addBrother(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    if( !compareDocuments(EDIT_ELEMENTS_BASE, regola) ) {
        return error();
    }
    return true ;
}

bool TestEditElements::insertProcessingInstructionUnderRoot()
{
    QList<int> sel;
    sel.append(0);
    if( !start(false, sel) ) {
        return error();
    }
    Element *newPI = newProcessingInstruction();
    if(NULL == newPI) {
        return error();
    }
    regola->addProcessingInstruction(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newPI);

    if( !compareDocuments(RES_APPENDPIUNDERROOT, regola) ) {
        return error();
    }
    return true ;
}

bool TestEditElements::appendProcessingInstructionToRoot()
{
    QList<int> sel;
    sel.append(0);
    if( !start(false, sel) ) {
        return error();
    }
    Element *newPI = newProcessingInstruction();
    if(NULL == newPI) {
        return error();
    }
    regola->appendProcessingInstruction(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newPI);

    if( !compareDocuments(RES_APPENDPIATROOT, regola) ) {
        return error();
    }
    return true ;
}

bool TestEditElements::insertProcessingInstructionAsChild()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    if( !start(false, sel) ) {
        return error();
    }
    Element *newPI = newProcessingInstruction();
    if(NULL == newPI) {
        return error();
    }
    regola->addProcessingInstruction(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newPI);

    if( !compareDocuments(RES_INSPI_CHILD, regola) ) {
        return error();
    }
    return true ;
}

bool TestEditElements::appendProcessingInstructionAsSibling()
{
    QList<int> sel;
    sel.append(0);
    sel.append(3);
    sel.append(0);
    if( !start(false, sel) ) {
        return error();
    }
    Element *newPI = newProcessingInstruction();
    if(NULL == newPI) {
        return error();
    }
    regola->appendProcessingInstruction(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newPI);

    if( !compareDocuments(RES_APPPI_SIBLING, regola) ) {
        return error();
    }
    return true ;
}

//--------------------------------------------------

bool TestEditElements::insertElementEmpty()
{
    QList<int> sel;
    if( !start(true, sel) ) {
        return error();
    }
    Element *newElement = createElement() ;
    if(NULL == newElement) {
        return error();
    }
    regola->addChild(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    newElement = new Element( "a", "", regola, NULL ) ;
    if(NULL == newElement) {
        return error();
    }
    regola->addBrother(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    if( !compareDocuments(RES_ELEMENTENTEMPTY, regola) ) {
        return error();
    }
    return true ;
}
/*
bool TestEditElements::insertElementAtRootNoSel()
{
    QList<int> sel;
    if( !start(false, sel) ) {
        return error();
    }
    Element *newElement = createElement() ;
    if(NULL == newElement) {
        return error();
    }
    regola->addChild(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    newElement = new Element( "a", "", regola, NULL ) ;
    if(NULL == newElement) {
        return error();
    }
    regola->addChild(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    if( !compareDocuments(EDIT_ELEMENTS_EL_BASE, regola) ) {
        return error();
    }
    return true ;
}
*/
bool TestEditElements::insertElementUnderRoot()
{
    QList<int> sel;
    sel.append(0);
    if( !start(false, sel) ) {
        return error();
    }
    Element *newElement = createElement();
    if(NULL == newElement) {
        return error();
    }
    regola->addChild(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    if( !compareDocuments(RES_APPEND_ELEM_UNDERROOT, regola) ) {
        return error();
    }
    return true ;
}

bool TestEditElements::insertElementAsChild()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    if( !start(false, sel) ) {
        return error();
    }
    Element *newElement = createElement() ;
    if(NULL == newElement) {
        return error();
    }
    regola->addComment(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    if( !compareDocuments(RES_INSELEM_CHILD, regola) ) {
        return error();
    }
    return true ;
}

bool TestEditElements::appendElementAsSibling()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    if( !start(false, sel) ) {
        return error();
    }
    Element *newElement = createElement() ;
    if(NULL == newElement) {
        return error();
    }
    regola->addBrother(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    if( !compareDocuments(RES_APPELEM_SIBLING, regola) ) {
        return error();
    }
    return true ;
}

bool TestEditElements::insertElementAsChildWithAttributes()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    if( !start(false, sel) ) {
        return error();
    }
    Element *newElement = createElement() ;
    if(NULL == newElement) {
        return error();
    }
    newElement->addAttribute("name", "value");
    regola->addChild(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    if( !compareDocuments(RES_INSELEM_CHILD_ATTR, regola) ) {
        return error();
    }
    return true ;
}

bool TestEditElements::appendElementAsSiblingWithAttributes()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    if( !start(false, sel) ) {
        return error();
    }
    Element *newElement = createElement() ;
    if(NULL == newElement) {
        return error();
    }
    newElement->addAttribute("name", "value");
    regola->addBrother(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    if( !compareDocuments(RES_APPELEM_SIBLING_ATTR, regola) ) {
        return error();
    }
    return true ;
}
bool TestEditElements::insertElementAsChildWithAttributesAndText()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    if( !start(false, sel) ) {
        return error();
    }
    Element *newElement = createElement() ;
    if(NULL == newElement) {
        return error();
    }
    newElement->addAttribute("name", "value");
    newElement->addTextNode(new TextChunk(false,"some text"));
    regola->addChild(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    if( !compareDocuments(RES_INSELEM_CHILD_ATTRTEXT, regola) ) {
        return error();
    }
    return true ;
}

bool TestEditElements::appendElementAsSiblingWithAttributesAndText()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    if( !start(false, sel) ) {
        return error();
    }
    Element *newElement = createElement() ;
    if(NULL == newElement) {
        return error();
    }
    newElement->addAttribute("name", "value");
    newElement->addTextNode(new TextChunk(true, "some text"));
    regola->addBrother(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    if( !compareDocuments(RES_APPELEM_SIBLING_ATTRTEXT, regola) ) {
        return error();
    }
    return true ;
}

