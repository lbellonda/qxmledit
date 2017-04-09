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
#include <QPlainTextEdit>
#include "comparexml.h"
#include "modules/namespace/namespacecommands.h"
#include "testhelpers/editelementtest.h"
#include "editelementwithtexteditor.h"


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

#define TEXTUAL_EDIT_START      "../test/data/editelements/textualStart.xml"
#define TEXTUAL_EDIT_START_1    "../test/data/editelements/textual1.xml"

TestEditElements::TestEditElements()
{
    selectedElement = NULL;
    regola = NULL ;
}

bool TestEditElements::error1()
{
    return false;
}

bool TestEditElements::test()
{
    {
        TestEditElements test;
        if( !test.insertCommentEmpty() ) {
            return error1();
        }
    }
    {
        TestEditElements test;
        if( !test.insertCommentAtRootNoSel() ) {
            return error1();
        }
    }
    {
        TestEditElements test;
        if( !test.appendCommentToRoot() ) {
            return error1();
        }
    }
    {
        TestEditElements test;
        if( !test.insertCommentUnderRoot() ) {
            return error1();
        }
    }
    {
        TestEditElements test;
        if( !test.appendCommentAsSibling() ) {
            return error1();
        }
    }
    //------------------
    {
        TestEditElements test;
        if( !test.insertProcessingInstructionEmpty() ) {
            return error1();
        }
    }
    {
        TestEditElements test;
        if( !test.insertProcessingInstructionAtRootNoSel() ) {
            return error1();
        }
    }
    {
        TestEditElements test;
        if( !test.appendProcessingInstructionToRoot() ) {
            return error1();
        }
    }
    {
        TestEditElements test;
        if( !test.insertProcessingInstructionUnderRoot() ) {
            return error1();
        }
    }
    {
        TestEditElements test;
        if( !test.appendProcessingInstructionAsSibling() ) {
            return error1();
        }
    }

    //------------------

    {
        TestEditElements test;
        if( !test.insertElementEmpty() ) {
            return error1();
        }
    }
    /*{
        TestEditElements test;
        if( !test.insertElementAtRootNoSel() ) {
            return error(test);
        }
    }*/
    {
        TestEditElements test;
        if( !test.insertElementUnderRoot() ) {
            return error1();
        }
    }
    {
        TestEditElements test;
        if( !test.insertElementAsChild() ) {
            return error1();
        }
    }
   {
        TestEditElements test;
        if( !test.appendElementAsSibling() ) {
            return error1();
        }
    }
    {
        TestEditElements test;
        if( !test.insertElementAsChildWithAttributes() ) {
            return error1();
        }
    }
    {
        TestEditElements test;
        if( !test.appendElementAsSiblingWithAttributes() ) {
            return error1();
        }
    }
    {
        TestEditElements test;
        if( !test.insertElementAsChildWithAttributesAndText() ) {
            return error1();
        }
    }
    {
        TestEditElements test;
        if( !test.appendElementAsSiblingWithAttributesAndText() ) {
            return error1();
        }
    }
    {
        TestEditElements test;
        if( !test.testNamespaces() ) {
            return error1();
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
        return error1();
    }
    QBuffer outputData(&resultData);
    if(!compare.loadFileIntoDocument(&outputData, document2)) {
        return error1();
    }
    bool result = compare.compareDomDocuments(document1, document2);
    if( !result ) {
        return error1();
    }
    return result ;
}

bool TestEditElements::start(const bool isNew, QList<int> &selection)
{
    if(!app.init() ) {
        return error1();
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
        return error1();
    }
    return true;
}


bool TestEditElements::insertCommentEmpty()
{
    QList<int> sel;
    if( !start(true, sel) ) {
        return error1();
    }
    Element *newComment = new Element( regola, Element::ET_COMMENT, NULL) ;
    if(NULL == newComment) {
        return error1();
    }
    newComment->setText("new comment");
    regola->addComment(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newComment);

    Element *newElement = new Element( "a", "", regola, NULL ) ;
    if(NULL == newElement) {
        return error1();
    }
    regola->addBrother(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    if( !compareDocuments(RES_INSERTCOMMENTEMPTY, regola) ) {
        return error1();
    }
    return true ;
}

//--------------------------------------------- FINO QUI -----------------------------------------------

// this does no insert since there is no selection
bool TestEditElements::insertCommentAtRootNoSel()
{
    QList<int> sel;
    if( !start(false, sel) ) {
        return error1();
    }
    Element *newComment = new Element( regola, Element::ET_COMMENT, NULL) ;
    if(NULL == newComment) {
        return error1();
    }
    newComment->setText("new comment");
    regola->addComment(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newComment);

    Element *newElement = new Element( "a", "", regola, NULL ) ;
    if(NULL == newElement) {
        return error1();
    }
    regola->addBrother(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    if( !compareDocuments(EDIT_ELEMENTS_BASE, regola) ) {
        return error1();
    }
    return true ;
}

bool TestEditElements::insertCommentUnderRoot()
{
    QList<int> sel;
    sel.append(0);
    if( !start(false, sel) ) {
        return error1();
    }
    Element *newComment = new Element( regola, Element::ET_COMMENT, NULL) ;
    if(NULL == newComment) {
        return error1();
    }
    newComment->setText("new comment");
    regola->addComment(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newComment);

    if( !compareDocuments(RES_APPENDCOMMENTUNDERROOT, regola) ) {
        return error1();
    }
    return true ;
}

bool TestEditElements::appendCommentToRoot()
{
    QList<int> sel;
    sel.append(0);
    if( !start(false, sel) ) {
        return error1();
    }
    Element *newComment = new Element( regola, Element::ET_COMMENT, NULL) ;
    if(NULL == newComment) {
        return error1();
    }
    newComment->setText("new comment");
    regola->appendComment(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newComment);

    if( !compareDocuments(RES_APPENDCOMMENTATROOT, regola) ) {
        return error1();
    }
    return true ;
}


bool TestEditElements::insertCommentAsChild()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    if( !start(false, sel) ) {
        return error1();
    }
    Element *newComment = new Element( regola, Element::ET_COMMENT, NULL) ;
    if(NULL == newComment) {
        return error1();
    }
    newComment->setText("new comment");
    regola->addComment(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newComment);

    if( !compareDocuments(RES_INSCOMM_CHILD, regola) ) {
        return error1();
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
        return error1();
    }
    Element *newComment = new Element( regola, Element::ET_COMMENT, NULL) ;
    if(NULL == newComment) {
        return error1();
    }
    newComment->setText("new comment");
    regola->appendComment(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newComment);

    if( !compareDocuments(RES_APPCOMM_SIBLING, regola) ) {
        return error1();
    }
    return true ;
}

//--------------------------------

Element *TestEditElements::newProcessingInstruction()
{
    Element *newProcessingInstr = new Element( regola, Element::ET_PROCESSING_INSTRUCTION, NULL) ;
    if(NULL == newProcessingInstr) {
        error1();
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
        error1();
        return NULL;
    }
    return element;
}


bool TestEditElements::insertProcessingInstructionEmpty()
{
    QList<int> sel;
    if( !start(true, sel) ) {
        return error1();
    }
    Element *newPI = newProcessingInstruction();
    if(NULL == newPI) {
        return error1();
    }
    regola->addProcessingInstruction(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newPI);

    Element *newElement = new Element( "a", "", regola, NULL ) ;
    if(NULL == newElement) {
        return error1();
    }
    regola->addBrother(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    if( !compareDocuments(RES_INSERTPITEMPTY, regola) ) {
        return error1();
    }
    return true ;
}

// this does not insert since there is no selection
bool TestEditElements::insertProcessingInstructionAtRootNoSel()
{
    QList<int> sel;
    if( !start(false, sel) ) {
        return error1();
    }
    Element *newPI = newProcessingInstruction();
    if(NULL == newPI) {
        return error1();
    }
    regola->addProcessingInstruction(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newPI);

    Element *newElement = new Element( "a", "", regola, NULL ) ;
    if(NULL == newElement) {
        return error1();
    }
    regola->addBrother(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    if( !compareDocuments(EDIT_ELEMENTS_BASE, regola) ) {
        return error1();
    }
    return true ;
}

bool TestEditElements::insertProcessingInstructionUnderRoot()
{
    QList<int> sel;
    sel.append(0);
    if( !start(false, sel) ) {
        return error1();
    }
    Element *newPI = newProcessingInstruction();
    if(NULL == newPI) {
        return error1();
    }
    regola->addProcessingInstruction(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newPI);

    if( !compareDocuments(RES_APPENDPIUNDERROOT, regola) ) {
        return error1();
    }
    return true ;
}

bool TestEditElements::appendProcessingInstructionToRoot()
{
    QList<int> sel;
    sel.append(0);
    if( !start(false, sel) ) {
        return error1();
    }
    Element *newPI = newProcessingInstruction();
    if(NULL == newPI) {
        return error1();
    }
    regola->appendProcessingInstruction(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newPI);

    if( !compareDocuments(RES_APPENDPIATROOT, regola) ) {
        return error1();
    }
    return true ;
}

bool TestEditElements::insertProcessingInstructionAsChild()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    if( !start(false, sel) ) {
        return error1();
    }
    Element *newPI = newProcessingInstruction();
    if(NULL == newPI) {
        return error1();
    }
    regola->addProcessingInstruction(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newPI);

    if( !compareDocuments(RES_INSPI_CHILD, regola) ) {
        return error1();
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
        return error1();
    }
    Element *newPI = newProcessingInstruction();
    if(NULL == newPI) {
        return error1();
    }
    regola->appendProcessingInstruction(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newPI);

    if( !compareDocuments(RES_APPPI_SIBLING, regola) ) {
        return error1();
    }
    return true ;
}

//--------------------------------------------------

bool TestEditElements::insertElementEmpty()
{
    QList<int> sel;
    if( !start(true, sel) ) {
        return error1();
    }
    Element *newElement = createElement() ;
    if(NULL == newElement) {
        return error1();
    }
    regola->addChild(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    newElement = new Element( "a", "", regola, NULL ) ;
    if(NULL == newElement) {
        return error1();
    }
    regola->addBrother(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    if( !compareDocuments(RES_ELEMENTENTEMPTY, regola) ) {
        return error1();
    }
    return true ;
}
/*
bool TestEditElements::insertElementAtRootNoSel()
{
    QList<int> sel;
    if( !start(false, sel) ) {
        return error1();
    }
    Element *newElement = createElement() ;
    if(NULL == newElement) {
        return error1();
    }
    regola->addChild(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    newElement = new Element( "a", "", regola, NULL ) ;
    if(NULL == newElement) {
        return error1();
    }
    regola->addChild(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    if( !compareDocuments(EDIT_ELEMENTS_EL_BASE, regola) ) {
        return error1();
    }
    return true ;
}
*/
bool TestEditElements::insertElementUnderRoot()
{
    QList<int> sel;
    sel.append(0);
    if( !start(false, sel) ) {
        return error1();
    }
    Element *newElement = createElement();
    if(NULL == newElement) {
        return error1();
    }
    regola->addChild(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    if( !compareDocuments(RES_APPEND_ELEM_UNDERROOT, regola) ) {
        return error1();
    }
    return true ;
}

bool TestEditElements::insertElementAsChild()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    if( !start(false, sel) ) {
        return error1();
    }
    Element *newElement = createElement() ;
    if(NULL == newElement) {
        return error1();
    }
    regola->addComment(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    if( !compareDocuments(RES_INSELEM_CHILD, regola) ) {
        return error1();
    }
    return true ;
}

bool TestEditElements::appendElementAsSibling()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    if( !start(false, sel) ) {
        return error1();
    }
    Element *newElement = createElement() ;
    if(NULL == newElement) {
        return error1();
    }
    regola->addBrother(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    if( !compareDocuments(RES_APPELEM_SIBLING, regola) ) {
        return error1();
    }
    return true ;
}

bool TestEditElements::insertElementAsChildWithAttributes()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    if( !start(false, sel) ) {
        return error1();
    }
    Element *newElement = createElement() ;
    if(NULL == newElement) {
        return error1();
    }
    newElement->addAttribute("name", "value");
    regola->addChild(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    if( !compareDocuments(RES_INSELEM_CHILD_ATTR, regola) ) {
        return error1();
    }
    return true ;
}

bool TestEditElements::appendElementAsSiblingWithAttributes()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    if( !start(false, sel) ) {
        return error1();
    }
    Element *newElement = createElement() ;
    if(NULL == newElement) {
        return error1();
    }
    newElement->addAttribute("name", "value");
    regola->addBrother(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    if( !compareDocuments(RES_APPELEM_SIBLING_ATTR, regola) ) {
        return error1();
    }
    return true ;
}
bool TestEditElements::insertElementAsChildWithAttributesAndText()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    if( !start(false, sel) ) {
        return error1();
    }
    Element *newElement = createElement() ;
    if(NULL == newElement) {
        return error1();
    }
    newElement->addAttribute("name", "value");
    newElement->addTextNode(new TextChunk(false,"some text"));
    regola->addChild(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    if( !compareDocuments(RES_INSELEM_CHILD_ATTRTEXT, regola) ) {
        return error1();
    }
    return true ;
}

bool TestEditElements::appendElementAsSiblingWithAttributesAndText()
{
    QList<int> sel;
    sel.append(0);
    sel.append(1);
    if( !start(false, sel) ) {
        return error1();
    }
    Element *newElement = createElement() ;
    if(NULL == newElement) {
        return error1();
    }
    newElement->addAttribute("name", "value");
    newElement->addTextNode(new TextChunk(true, "some text"));
    regola->addBrother(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), newElement);

    if( !compareDocuments(RES_APPELEM_SIBLING_ATTRTEXT, regola) ) {
        return error1();
    }
    return true ;
}

NamespaceCommands* TestEditElements::makeOperation(const bool useNs, const QString &prefix, const QString &uri)
{
    NamespaceCommands *cmd = new NamespaceCommands();
    cmd->setUseNsElement(useNs);
    cmd->setDeclareNs(NamespaceCommands::DoNotDeclare);
    cmd->tagSpec()->prefix = prefix;
    cmd->tagSpec()->uri = uri ;

    return cmd;
}

NamespaceCommands* TestEditElements::makeOperationOther(const bool useNs, const QString &prefix, const QString &uri, const QString &otherNs)
{
    NamespaceCommands *cmd = makeOperation(useNs, prefix, uri);
    cmd->setDeclareNs(NamespaceCommands::DoNotDeclare);
    QHash<QString, QString> decoded = unpackAttrib(otherNs);
    //
    foreach( QString key, decoded.keys()) {
        cmd->addNamespace(key, decoded[key]);
    }
    return cmd;
}

NamespaceCommands* TestEditElements::makeOperationDeclare(const bool useNs, const QString &prefix, const QString &uri)
{
    NamespaceCommands *cmd = makeOperation(useNs, prefix, uri);
    cmd->setDeclareNs(NamespaceCommands::DeclareInElement);
    return cmd;
}


/*
 * x -> x
 * a:x -> x
 * x -> a:x
 * a:x -> b:x
 *
 * 1,2,3,4 without declaring ns
 * 5,6,7,8 declaring ns
 *
 * 9: ins ns altri
 * 10: remove ns altri
 *
 * 11: tag decl + ins ns altri + remove ns altri
 *
 * 12: x -> x no mod DA PENSARE
 * 13: a:x -> b:x
 */
bool TestEditElements::testNamespaces()
{
    _testName = "testNamespaces" ;

    if(!tagXtoX0()) {
        return false;
    }
    if(!tagAXtoX1()) {
        return false;
    }
    if(!tagXtoAX2()) {
        return false;
    }
    if(!tagAXtoBX3()) {
        return false;
    }
    if(!tagXtoX4()) {
        return false;
    }
    if(!tagAXtoX5()) {
        return false;
    }
    if(!tagXtoAX6()) {
        return false;
    }
    if(!tagAXtoBX7()) {
        return false;
    }
    //9
    if(!tagInsNsAltri()) {
        return false;
    }
    //10
    if(!tagRemoveNsAltri()) {
        return false;
    }
    // 11
    if(!tagNsInsModDel()) {
        return false;
    }
    //12
    if(!tagNsNoMod()) {
        return false;
    }

    //13
    if(!tagNsChangeNs()) {
        return false;
    }

    return true;
}

bool TestEditElements::tagXtoX0()
{
    _testName = "tagXtoX0" ;
    Element *sourceElement = makeElement("x", "");
    Element *resultElement = makeElement("x", "");
    NamespaceCommands *cmd = makeOperation(false, "", "");
    return doCheckElementNamespaceOp(sourceElement, resultElement, cmd );
}

bool TestEditElements::tagAXtoX1()
{
    _testName = "tagAXtoX1" ;
    Element *sourceElement = makeElement("a:x", "");
    Element *resultElement = makeElement("x", "");
    NamespaceCommands *cmd = makeOperation(false, "", "");
    return doCheckElementNamespaceOp(sourceElement, resultElement, cmd );
}

bool TestEditElements::tagXtoAX2()
{
    _testName = "tagXtoAX2" ;
    Element *sourceElement = makeElement("x", "");
    Element *resultElement = makeElement("a:x", "");
    NamespaceCommands *cmd = makeOperation(false, "a", "");
    return doCheckElementNamespaceOp(sourceElement, resultElement, cmd );
}

bool TestEditElements::tagAXtoBX3()
{
    _testName = "tagAXtoBX3" ;
    Element *sourceElement = makeElement("a:x", "");
    Element *resultElement = makeElement("b:x", "");
    NamespaceCommands *cmd = makeOperation(false, "b", "");
    return doCheckElementNamespaceOp(sourceElement, resultElement, cmd );
}

bool TestEditElements::tagXtoX4()
{
    _testName = "tagXtoX4" ;
    Element *sourceElement = makeElement("x", "");
    Element *resultElement = makeElement("x", "xmlns=uri_test");
    NamespaceCommands *cmd = makeOperationDeclare(true, "", "uri_test");
    return doCheckElementNamespaceOp(sourceElement, resultElement, cmd );
}

bool TestEditElements::tagAXtoX5()
{
    _testName = "tagAXtoX5" ;
    Element *sourceElement = makeElement("a:x", "");
    Element *resultElement = makeElement("x", "xmlns=uri_test");
    NamespaceCommands *cmd = makeOperationDeclare(true, "", "uri_test");
    return doCheckElementNamespaceOp(sourceElement, resultElement, cmd );
}

bool TestEditElements::tagXtoAX6()
{
    _testName = "tagXtoAX6" ;
    Element *sourceElement = makeElement("x", "");
    Element *resultElement = makeElement("a:x", "xmlns:a=uri_test");
    NamespaceCommands *cmd = makeOperationDeclare(true, "a", "uri_test");
    return doCheckElementNamespaceOp(sourceElement, resultElement, cmd );
}

bool TestEditElements::tagAXtoBX7()
{
    _testName = "tagAXtoBX7" ;
    Element *sourceElement = makeElement("a:x", "");
    Element *resultElement = makeElement("b:x", "xmlns:b=uri_test");
    NamespaceCommands *cmd = makeOperationDeclare(true, "b", "uri_test");
    return doCheckElementNamespaceOp(sourceElement, resultElement, cmd );
}


bool TestEditElements::tagInsNsAltri()
{
    _testName = "tagInsNsAltri" ;
    Element *sourceElement = makeElement("x", "");
    Element *resultElement = makeElement("x", "xmlns:c=uri_test,xmlns:d=xxx,xmlns=other");
    NamespaceCommands *cmd = makeOperationOther(false, "", "", "c=uri_test,d=xxx,=other" );
    return doCheckElementNamespaceOp(sourceElement, resultElement, cmd );
}

bool TestEditElements::tagRemoveNsAltri()
{
    _testName = "tagRemoveNsAltri" ;
    Element *sourceElement = makeElement("a:x", "a=1,xmlns:c=uri_test,xmlns:d=xxx,xmlns=other");
    Element *resultElement = makeElement("a:x", "a=1");
    NamespaceCommands *cmd = makeOperationOther(false, "a", "", "" );
    return doCheckElementNamespaceOp(sourceElement, resultElement, cmd );
}

bool TestEditElements::tagNsInsModDel()
{
    _testName = "tagNsInsModDel" ;
    Element *sourceElement = makeElement("a:x", "a=1,xmlns:c=uri_test,xmlns:d=xxx,xmlns=other");
    Element *resultElement = makeElement("x", "a=1,xmlns:c=uri_test,xmlns=yyy,xmlns:d=kkk");
    NamespaceCommands *cmd = makeOperationOther(true, "", "", "c=uri_test,=yyy,d=kkk" );
    return doCheckElementNamespaceOp(sourceElement, resultElement, cmd );
}

bool TestEditElements::tagNsChangeNs()
{
    _testName = "tagNsChangeNsl" ;
    Element *sourceElement = makeElement("a:x", "a=1,xmlns:c=uri_test,xmlns:b=xxx,xmlns=other");
    Element *resultElement = makeElement("d:x", "a=1,xmlns:c=uri_test,xmlns:d=xxx,xmlns=other");
    NamespaceCommands *cmd = makeOperationOther(true, "d", "xxx", "c=uri_test,d=xxx,=other" );
    return doCheckElementNamespaceOp(sourceElement, resultElement, cmd );
}

bool TestEditElements::tagNsNoMod()
{
    _testName = "tagNsNoMod" ;
    Element *sourceElement = makeElement("a:x", "a=1,xmlns:c=uri_test,xmlns:b=xxx,xmlns=other");
    Element *resultElement = makeElement("a:x", "a=1,xmlns:c=uri_test,xmlns:d=xxx,xmlns=other");
    NamespaceCommands *cmd = makeOperationOther(false, "a", "xxx", "c=uri_test,d=xxx,=other" );
    return doCheckElementNamespaceOp(sourceElement, resultElement, cmd );
}

bool TestEditElements::doCheckElementNamespaceOp(Element *sourceElement, Element *resultElement, NamespaceCommands *cmd )
{
    bool result = false ;
    if( (NULL != sourceElement) && (NULL != resultElement) && (NULL != cmd))  {
        EditElementTest dlg;
        dlg.setTarget(sourceElement);
        if(!dlg.testApplyNamespaceOper(cmd)) {
            error("error updating target");
        } else {
            QString msg;
            result = sourceElement->compareToElement(resultElement, msg);
            if(!result) {
                error(msg);
            }
        }
    } else {
        error("no memory");
    }

    if( NULL != sourceElement ) {
        delete sourceElement ;
    }
    if( NULL != resultElement ) {
        delete resultElement ;
    }
    if( NULL != cmd ) {
        delete cmd ;
    }
    return result;
}

//-------------

#define NS_FILE_EMPTY       "../test/data/namespace/t_empty.xml"
#define NS_FILE_COMPLEX     "../test/data/namespace/t_complex.xml"
#define NS_FILE_SHADOWED    "../test/data/namespace/t_shadowed.xml"

#define NS_FILE_INS0    "../test/data/namespace/ins0.xml"
#define NS_FILE_INS1    "../test/data/namespace/ins1.xml"
#define NS_FILE_EDIT0   "../test/data/namespace/edit0.xml"
#define NS_FILE_EDIT1   "../test/data/namespace/edit1.xml"

bool TestEditElements::testLoadElm(const QString &fileStart, const QString& expected, QList<int> &sel, const bool isInsert )
{
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(fileStart) ) {
        return error(QString("unable to load input file: '%1' ").arg(fileStart));
    }
    Element *selectedElement = app.mainWindow()->getRegola()->findElementByArray(sel);
    if(NULL == selectedElement) {
        return error("no selected element");
    }
    EditElement edit(app.mainWindow());
    if(isInsert) {
        Element element(NULL);
        edit.setTarget(&element, selectedElement);
    } else {
        edit.setTarget(selectedElement, NULL);
    }
    QLineEdit *editTag = edit.findChild<QLineEdit*>("editTag");
    if(NULL == editTag) {
        return error("tag edit field not found");
    }
    if(editTag->text() != expected) {
        return error(QString("Tag field, expected:'%1', but found:'%2'").arg(expected).arg(editTag->text()));
    }
    return true;
}

// testa visibilita ns in elemento
bool TestEditElements::testInsertElement()
{
    _testName = "testInsertElement/None";
    QList<int> sel;
    sel << 1 << 0 << 0 ;
    // test for prefix inserting

    _testName = "testInsertElement/Ins0";
    //- no prefix
    if(!testLoadElm(NS_FILE_INS0, "", sel, true )) {
        return false;
    }
    _testName = "testInsertElement/Ins1";
    //- prefix
    if(!testLoadElm(NS_FILE_INS1, "a:", sel, true )) {
        return false;
    }
    _testName = "testInsertElement/Edit0";
    //- no prefix
    if(!testLoadElm(NS_FILE_EDIT0, "this", sel, false )) {
        return false;
    }
    _testName = "testInsertElement/Edit1";
    //- prefix
    if(!testLoadElm(NS_FILE_EDIT1, "a:this", sel, false )) {
        return false;
    }
    return true;
}

bool TestEditElements::testFast()
{
    if(!testEditText()) {
        return false;
    }
    QList<int> sel;
    sel << 1 << 0 << 0 ;
    if(!testLoadElm(NS_FILE_INS1, "a:", sel, true )) {
        return false;
    }
    return testNamespaces();
}

class TestEditElements_EditHook : public TextEditorInterface
{
    public:
    QString _textToEditIs ;
    TestEditElements *parent;
    Regola *regola;
    TestEditElements_EditHook();
    ~TestEditElements_EditHook();
    bool editTextualForInterface(QWidget *const parentWindow, Element *element);
    friend class TestEditElements;
};

TestEditElements_EditHook::TestEditElements_EditHook(){parent = NULL;}
TestEditElements_EditHook::~TestEditElements_EditHook(){}

bool TestEditElements_EditHook::editTextualForInterface(QWidget *const parentWindow, Element *element)
{
    EditElementWithTextEditor editor(parentWindow, element, regola);
    if(!_textToEditIs.isEmpty()) {
        QPlainTextEdit *editTag = editor.findChild<QPlainTextEdit*>("editor");
        if(NULL == editTag) {
            return parent->error("tag edit field not found");
        }
        editTag->setPlainText(_textToEditIs);
    }
    const bool result = editor.makeItAccectped();
    return result ;
}

bool TestEditElements::testLoadElmText(const QString &fileStart, const QString& fileToCompare,
                                       QList<int> &sel,
                                       const QString& newValue,
                                       const bool expectedResult,
                                       const QString& expectedTag, const QString& expectedAttributes )
{
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(fileStart) ) {
        return error(QString("unable to load input file: '%1' ").arg(fileStart));
    }
    Element *selectedElement = app.mainWindow()->getRegola()->findElementByArray(sel);
    if(NULL == selectedElement) {
        return error("no selected element");
    }

    Regola *regola = app.mainWindow()->getRegola();
    TestEditElements_EditHook hook;
    hook.parent = this ;
    Element *expectedElement = makeElement(expectedTag, expectedAttributes);
    hook._textToEditIs = newValue ;
    hook.regola = regola;
    const bool result = regola->editElementWithTextEditor(app.mainWindow(), app.mainWindow()->getMainTreeWidget(), selectedElement->getUI(), &hook);
    if(isError()) {
        return false;
    }
    //---------
    QString msg;
    if(!expectedElement->compareToElement(selectedElement, msg)) {
        delete expectedElement;
        return error(QString("Comparing elements: %1").arg(msg));
    }
    delete expectedElement;
    //---------
    if(result != expectedResult) {
        return error(QString("Expected result:%1, but found: %2").arg(expectedResult).arg(result));
    }
    //---------
    if(!compareXMLBase(regola, "operation", fileToCompare)){
        return false;
    }
    regola->undo();
    if(!compareXMLBase(regola, "undo", fileStart)){
        return false;
    }
    regola->redo();
    if(!compareXMLBase(regola, "redo", fileToCompare)){
        return false;
    }
    return true;
}

bool TestEditElements::testEditText()
{
    _testName = "testEditText" ;
    QList<int> sel;
    sel << 1 << 1;

    // 1- all ok
    _testName = "testEditText/1";
    if(!testLoadElmText( TEXTUAL_EDIT_START, TEXTUAL_EDIT_START_1, sel,
                                           "abc c=\"aaa\"\nd='v vv' efed:dd=\"ddddd&amp;\"",
                                           true,
                                           "abc", "c=aaa,d=v vv,efed:dd=ddddd&" ) ) {
        return false;
    }
    // 2- not editable
    _testName = "testEditText/2";
    if(!testLoadElmText( TEXTUAL_EDIT_START, TEXTUAL_EDIT_START, sel,
                                           "abcaac=\"aaa\"\nd='v vv' efed:dd=\"ddddd&amp;\"",
                                           false,
                                           "bbb", "ddd=123,cpcd=xxx") ) {
        return false;
    }
    // 3- no change
    _testName = "testEditText/3";
    if(!testLoadElmText( TEXTUAL_EDIT_START, TEXTUAL_EDIT_START, sel,
                                           "",
                                           true,
                                           "bbb", "ddd=123,cpcd=xxx" ) ) {
        return false;
    }

    return true;
}
