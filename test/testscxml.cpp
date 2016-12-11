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

#include "testscxml.h"
#include "app.h"
#include "regola.h"
#include "modules/xsd/namespacemanager.h"
#include "modules/specialized/scxml/scxmltokenmanager.h"
#include "modules/specialized/scxml/scxmlroot.h"
#include "modules/specialized/scxml/scxmleditormanager.h"
#include "modules/specialized/scxml/dialogs/scxmlstatedialog.h"
#include "modules/specialized/scxml/dialogs/scxmlparalleldialog.h"
#include "modules/specialized/scxml/dialogs/scxmltransitiondialog.h"
#include "modules/specialized/scxml/dialogs/scxmlfinaldialog.h"
#include "modules/specialized/scxml/dialogs/scxmlhistoryldialog.h"
#include "modules/specialized/scxml/dialogs/scxmlraisedialog.h"
#include "modules/specialized/scxml/dialogs/scxmlifdialog.h"
#include "modules/specialized/scxml/dialogs/scxmlelseifdialog.h"
#include "modules/specialized/scxml/dialogs/scxmlforeachdialog.h"
#include "modules/messages/sourceerror.h"
#include "sourcemessagemanager.h"
#if QT_VERSION >= QT_VERSION_CHECK(5,7,0)
#include <QScxmlStateMachine>
#include <QScxmlError>
#endif

#define BASE_PATH "../test/data/xml/special/scxml"
#define FILE_STATES BASE_PATH "/states.xml"
#define FILE_CHECKLOAD BASE_PATH "/checkload.txt"

static QStringList allTokens()
{
    QStringList tokens ;

    //tokens << "assign";
    //tokens << "cancel";
    //tokens << "content";
    //tokens << "data";
    //tokens << "datamodel";
    tokens << "else";
    tokens << "elseif";
    tokens << "raise";
    tokens << "final";
    //tokens << "finalize";
    tokens << "history";
    tokens << "if";
    tokens << "initial";
    //tokens << "invoke";
    tokens << "foreach";
    tokens << "log";
    tokens << "onentry";
    tokens << "onexit";
    tokens << "parallel";
    //tokens << "param";
    //tokens << "script";
    tokens << "scxml";
    //tokens << "send";
    tokens << "state";
    tokens << "transition";
    //tokens <<  "var";
    return tokens ;
}

TestSCXML::TestSCXML()
{
}

TestSCXML::~TestSCXML()
{
}

bool TestSCXML::testFast()
{
    _testName = "testFast" ;
    return testTemplates();
}

bool TestSCXML::testLoadTokens()
{
    _testName = "testLoadTokens" ;
    QStringList tokens = allTokens();

    SCXMLTokenManager tokenManager;
    tokenManager.init();
    foreach( const QString &token, tokens ) {
        if( NULL == tokenManager.tokenForName(token) ) {
            return error(QString("Token %1 missing").arg(token));
        }
    }
    return true ;
}

bool TestSCXML::testEditTokens()
{
    _testName = "testEditTokens" ;
    /*bool testEditassign();
    bool testEditcancel();
    bool testEditcontent();
    bool testEditdata();
    bool testEditdatamodel();
    bool testEditelse();
    bool testEditelseif();
    */
    if(!testEditif() ) {
        return false;
    }
    if(!testEditraise() ) {
        return false;
    }
    if(!testEditfinal() ) {
        return false;
    }
    /*bool testEditfinalize();*/
    if(!testEdithistory() ) {
        return false;
    }
    if(!testEditinitial() ) {
        return false;
    }
    /*bool testEditinvoke();*/
    if(!testEditforeach() ) {
        return false;
    }
    if(!testEditlog() ) {
        return false;
    }
    if(!testEditonentry() ) {
        return false;
    }
    if(!testEditonexit() ) {
        return false;
    }
    if(!testEditparallel() ) {
        return false;
    }
    /*bool testEditparam();
    bool testEditscript();*/
    if(!testEditscxml() ) {
        return false;
    }
    /*bool testEditsend();*/
    if(!testEditstate() ) {
        return false;
    }
    if(!testEdittransition() ) {
        return false;
    }
    /*
    bool testEditvar();*/
    return true;
}

bool TestSCXML::testTemplates()
{
    _testName = "testTemplates" ;
    if(!testTemplatesCompile()) {
        return false;
    }
    if(!testTemplatesLoadStates()) {
        return false;
    }
    if(!testLoadMessages()) {
        return false;
    }
    return true;
}

bool TestSCXML::testTemplatesCompile()
{
    _testName = "testTemplatesCompile" ;
#if QT_VERSION >= QT_VERSION_CHECK(5,7,0)
    bool isError = false;
    QString msg;
    QScxmlStateMachine *testMachine = QScxmlStateMachine::fromFile(":/templates/templateSCXML");
    if( NULL != testMachine ) {
        if (!testMachine->parseErrors().isEmpty()) {
            isError = true;
            foreach (const QScxmlError &error, testMachine->parseErrors()) {
                msg += error.toString();
            }
        }
        delete testMachine ;
    } else {
        msg = "null SCXML machine";
        isError = true;
    }

    if(isError) {
        return error(msg);
    }
#endif
    return true ;
}

bool TestSCXML::testTemplatesLoadStates()
{
    _testName = "testTemplatesLoadStates" ;
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(FILE_STATES) ) {
        return error(QString("unable to load input file: '%1' ").arg(FILE_STATES));
    }
    Regola *regola = app.mainWindow()->getRegola();
    SCXMLEditorManager *mgr = regola->namespaceManager()->scxmlEditorManager();
    if( NULL == mgr ) {
        return error("null manager");
    }
    SCXMLInfo info;
    mgr->findInfoStates(regola, &info);
    QStringList expected ;
    expected << "initial" << "multi" << "one" << "two" << "multi2" << "TwoOne" << "TwoTwo";
    if( !compareStringList("allStates", expected, info.allStates())) {
        return false;
    }
    return true;
}

bool TestSCXML::setEAE(QWidget *widget, const QString &name, const QString &attr, const QString value, Element *e1)
{
    if(!setTextWidget(widget, name, value) ) {
        return error(QString("Edit %1 not found").arg(name));
    }
    e1->setAttribute(attr, value);
    return true;
}

bool TestSCXML::setEAC(QWidget *widget, const QString &name, const QString &attr, const QString value, Element *e1)
{
    if(!setComboWidget(widget, name, value) ) {
        return error(QString("Edit %1 not found").arg(name));
    }
    e1->setAttribute(attr, value);
    return true;
}

bool TestSCXML::setEAB(QWidget *widget, const QString &name, const bool checked, const QString &attr, const QString value, Element *e1)
{
    if(!setCheckBoxWidget(widget, name, checked) ) {
        return error(QString("CheckBox %1 not found").arg(name));
    }
    e1->setAttribute(attr, value);
    return true;
}

// --- edit tokens
bool TestSCXML::testEditscxml()
{
    _testName ="testEditscxml" ;
    Element elementEdit(NULL);
    Element elementCompare(NULL);
    Regola regola;
    SCXMLInfo info;
    elementCompare.setTag(SCXMLToken::Tag_scxml) ;
    SCXMLRoot dialog(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
    if(!setEAB(&dialog, "binding", true, SCXMLscxmlToken::A_binding, "early", &elementCompare)){return false;}
    if(!setEAC(&dialog, "datamodel", SCXMLscxmlToken::A_datamodel, "aData", &elementCompare)){return false;}
    if(!setEAE(&dialog, "name", SCXMLscxmlToken::A_name, "aName", &elementCompare)){return false;}
    if(!setEAE(&dialog, "version", SCXMLscxmlToken::A_version, "1.0", &elementCompare)){return false;}
    elementCompare.setAttribute(SCXMLscxmlToken::A_xmlns, NamespaceManager::SCXMLNamespace);
    if(!setEAC(&dialog, "initial", SCXMLscxmlToken::A_initial, "aInitial", &elementCompare)){return false;}
    dialog.accept();
    if(!compare(&elementCompare, &elementEdit)) {
        return false;
    }
    return true;
}

bool TestSCXML::testEditstate()
{
    _testName ="testEditstate" ;
    Element elementEdit(NULL);
    Element elementCompare(NULL);
    Regola regola;
    SCXMLInfo info;
    elementCompare.setTag(SCXMLToken::Tag_state) ;
    SCXMLStateDialog dialog(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
    if(!setEAC(&dialog, "initial", SCXMLstateToken::A_initial, "aInit", &elementCompare)){return false;}
    if(!setEAE(&dialog, "id", SCXMLstateToken::A_id, "aId", &elementCompare)){return false;}
    dialog.accept();
    if(!compare(&elementCompare, &elementEdit)) {
        return false;
    }
    return true;
}

bool TestSCXML::testEditparallel()
{
    _testName ="testEditparallel" ;
    Element elementEdit(NULL);
    Element elementCompare(NULL);
    Regola regola;
    SCXMLInfo info;
    elementCompare.setTag(SCXMLToken::Tag_parallel) ;
    SCXMLParallelDialog dialog(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
    if(!setEAE(&dialog, "id", SCXMLparallelToken::A_id, "aId", &elementCompare)){return false;}
    dialog.accept();
    if(!compare(&elementCompare, &elementEdit)) {
        return false;
    }
    return true;
}

bool TestSCXML::testEdittransition()
{
    _testName ="testEdittransition" ;
    Element elementEdit(NULL);
    Element elementCompare(NULL);
    Regola regola;
    SCXMLInfo info;
    elementCompare.setTag(SCXMLToken::Tag_transition) ;
    SCXMLTransitionDialog dialog(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
    if(!setEAC(&dialog, "target", SCXMLtransitionToken::A_target, "aTarget", &elementCompare)){return false;}
    if(!setEAC(&dialog, "type", SCXMLtransitionToken::A_type, "Internal", &elementCompare)){return false;}
    if(!setEAE(&dialog, "cond", SCXMLtransitionToken::A_cond, "cond", &elementCompare)){return false;}
    if(!setEAE(&dialog, "event", SCXMLtransitionToken::A_event, "aEvent", &elementCompare)){return false;}
    dialog.accept();
    if(!compare(&elementCompare, &elementEdit)) {
        return false;
    }
    return true;
}

bool TestSCXML::testEditinitial()
{
    _testName ="testEditinitial" ;
    Element elementEdit(NULL);
    Element elementCompare(NULL);
    Regola regola;
    SCXMLInfo info;
    elementCompare.setTag(SCXMLToken::Tag_initial) ;
    SCXMLinitialToken token;
    token.editToken(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
    if(!compare(&elementCompare, &elementEdit)) {
        return false;
    }
    return true;
}

bool TestSCXML::testEditfinal()
{
    _testName ="testEditfinal" ;
    Element elementEdit(NULL);
    Element elementCompare(NULL);
    Regola regola;
    SCXMLInfo info;
    elementCompare.setTag(SCXMLToken::Tag_final) ;
    SCXMLFinalDialog dialog(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
    if(!setEAE(&dialog, "id", SCXMLfinalToken::A_id, "id", &elementCompare)){return false;}
    dialog.accept();
    if(!compare(&elementCompare, &elementEdit)) {
        return false;
    }
    return true;
}

bool TestSCXML::testEditonentry()
{
    _testName ="testEditonentry" ;
    Element elementEdit(NULL);
    Element elementCompare(NULL);
    Regola regola;
    SCXMLInfo info;
    elementCompare.setTag(SCXMLToken::Tag_onentry) ;
    SCXMLonentryToken token;
    token.editToken(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
    if(!compare(&elementCompare, &elementEdit)) {
        return false;
    }
    return true;
}

bool TestSCXML::testEditonexit()
{
    _testName ="testEditonexit" ;
    Element elementEdit(NULL);
    Element elementCompare(NULL);
    Regola regola;
    SCXMLInfo info;
    elementCompare.setTag(SCXMLToken::Tag_onexit) ;
    SCXMLonexitToken token;
    token.editToken(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
    if(!compare(&elementCompare, &elementEdit)) {
        return false;
    }
    return true;
}


bool TestSCXML::testEdithistory()
{
    _testName ="testEdithistory" ;
    Element elementEdit(NULL);
    Element elementCompare(NULL);
    Regola regola;
    SCXMLInfo info;
    elementCompare.setTag(SCXMLToken::Tag_history) ;
    SCXMLHistoryDialog dialog(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
    if(!setEAE(&dialog, "id", SCXMLhistoryToken::A_id, "id", &elementCompare)){return false;}
    if(!setEAC(&dialog, "type", SCXMLhistoryToken::A_type, "type", &elementCompare)){return false;}
    dialog.accept();
    if(!compare(&elementCompare, &elementEdit)) {
        return false;
    }
    return true;
}

bool TestSCXML::testEditraise()
{
    _testName ="testEditraise" ;
    Element elementEdit(NULL);
    Element elementCompare(NULL);
    Regola regola;
    SCXMLInfo info;
    elementCompare.setTag(SCXMLToken::Tag_raise) ;
    SCXMLRaiseDialog dialog(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
    if(!setEAE(&dialog, "event", SCXMLraiseToken::A_event, "event", &elementCompare)){return false;}
    dialog.accept();
    if(!compare(&elementCompare, &elementEdit)) {
        return false;
    }
    return true;
}

bool TestSCXML::testEditif()
{
    _testName ="testEditif" ;
    Element elementEdit(NULL);
    Element elementCompare(NULL);
    Regola regola;
    SCXMLInfo info;
    elementCompare.setTag(SCXMLToken::Tag_if) ;
    SCXMLIfDialog dialog(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
    if(!setEAE(&dialog, "cond", SCXMLifToken::A_cond, "cond", &elementCompare)){return false;}
    dialog.accept();
    if(!compare(&elementCompare, &elementEdit)) {
        return false;
    }
    return true;
}


bool TestSCXML::testEditelse()
{
    _testName ="testEditelse" ;
    Element elementEdit(NULL);
    Element elementCompare(NULL);
    Regola regola;
    SCXMLInfo info;
    elementCompare.setTag(SCXMLToken::Tag_else) ;
    SCXMLelseToken token;
    token.editToken(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
    if(!compare(&elementCompare, &elementEdit)) {
        return false;
    }
    return true;
}

bool TestSCXML::testEditelseif()
{
    _testName ="testEditelseif" ;
    Element elementEdit(NULL);
    Element elementCompare(NULL);
    Regola regola;
    SCXMLInfo info;
    elementCompare.setTag(SCXMLToken::Tag_elseif) ;
    SCXMLElseIfDialog dialog(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
    if(!setEAE(&dialog, "cond", SCXMLelseifToken::A_cond, "cond", &elementCompare)){return false;}
    dialog.accept();
    if(!compare(&elementCompare, &elementEdit)) {
        return false;
    }
    return true;
}

bool TestSCXML::testEditforeach()
{
    _testName ="testEditforeach" ;
    Element elementEdit(NULL);
    Element elementCompare(NULL);
    Regola regola;
    SCXMLInfo info;
    elementCompare.setTag(SCXMLToken::Tag_foreach) ;
    SCXMLForeachDialog dialog(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
    if(!setEAE(&dialog, "array", SCXMLforeachToken::A_array, "aArray", &elementCompare)){return false;}
    if(!setEAE(&dialog, "index", SCXMLforeachToken::A_index, "aindex", &elementCompare)){return false;}
    if(!setEAE(&dialog, "item", SCXMLforeachToken::A_item, "aitem", &elementCompare)){return false;}
    dialog.accept();
    if(!compare(&elementCompare, &elementEdit)) {
        return false;
    }
    return true;
}

bool TestSCXML::testEditlog()
{
    _testName ="testEditlog" ;
    Element elementEdit(NULL);
    Element elementCompare(NULL);
    Regola regola;
    SCXMLInfo info;
    elementCompare.setTag(SCXMLToken::Tag_log) ;
    SCXMLTransitionDialog dialog(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
    if(!setEAE(&dialog, "label", SCXMLlogToken::A_label, "alabel", &elementCompare)){return false;}
    if(!setEAE(&dialog, "expr", SCXMLlogToken::A_expr, "aexpr", &elementCompare)){return false;}
    dialog.accept();
    if(!compare(&elementCompare, &elementEdit)) {
        return false;
    }
    return true;
}



bool TestSCXML::testEditassign()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditcancel()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditcontent()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditdata()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditdatamodel()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditfinalize()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditinvoke()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditparam()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditscript()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditsend()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditvar()
{_testName ="" ;return error("nyi");}

//----
static SourceMessage *newMsg(const int line, const int column, const QString &text)
{
    SourceMessage *sourceError = new SourceMessage();
    sourceError->setColumn(column);
    sourceError->setLine(line);
    sourceError->setDescription(text);
    sourceError->setType(SourceMessage::Error);
    return sourceError;
}

bool TestSCXML::testLoadMessages()
{
    _testName = "testLoadMessages" ;
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(FILE_CHECKLOAD) ) {
        return error(QString("unable to load input file: '%1' ").arg(FILE_CHECKLOAD));
    }
    Regola *regola = app.mainWindow()->getRegola();
    QList<SourceMessage*> errors;
    errors.append(newMsg(2, 1, "root"));
    errors.append(newMsg(3, 6, "a"));
    errors.append(newMsg(5, 1, "d"));

    QBuffer *dataStream = app.mainWindow()->controller()->getDataForSourceDecode();
    if( NULL == dataStream ) {
        return error("Unable to write data");
    }
    dataStream->open(QIODevice::ReadOnly);
    SourceMessageManager manager;
    manager.bindToSource(dataStream, errors, regola);
    {
        SourceMessage *m1 = errors.at(0);
        QList<int> pos ;
        QList<int> path = m1->path() ;
        pos << 1 ;
        if(!compareListInts("msg0", pos, path)) {
            EMPTYPTRLIST(errors, SourceMessage);
            return false;
        }
    }
    {
        SourceMessage *m1 = errors.at(1);
        QList<int> pos ;
        QList<int> path = m1->path() ;
        pos << 1 << 0 ;
        if(!compareListInts("msg1", pos, path)) {
            EMPTYPTRLIST(errors, SourceMessage);
            return false;
        }
    }
    {
        SourceMessage *m1 = errors.at(2);
        QList<int> pos ;
        QList<int> path = m1->path() ;
        pos << 1 << 0 << 1 ;
        if(!compareListInts("msg2", pos, path)) {
            EMPTYPTRLIST(errors, SourceMessage);
            return false;
        }
    }
    EMPTYPTRLIST(errors, SourceMessage);
    return true ;
}
