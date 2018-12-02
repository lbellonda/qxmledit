/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016-2018 by Luca Bellonda and individual contributors  *
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
#include "modules/specialized/scxml/dialogs/scxmllogdialog.h"
#include "modules/specialized/scxml/dialogs/scxmldatadialog.h"
#include "modules/specialized/scxml/dialogs/scxmlassigndialog.h"
#include "modules/specialized/scxml/dialogs/scxmlparamdialog.h"
#include "modules/specialized/scxml/dialogs/scxmlscriptdialog.h"
#include "modules/specialized/scxml/dialogs/scxmlinvokedialog.h"
#include "modules/specialized/scxml/dialogs/scxmlsenddialog.h"
#include "modules/specialized/scxml/dialogs/scxmlcontentdialog.h"
#include "modules/messages/sourceerror.h"
#include "sourcemessagemanager.h"
#include "modules/messages/sourcerelatedmessages.h"
#include <QListWidget>
#ifdef QXMLEDIT_QT_SCXML_ENABLED
#include <QScxmlStateMachine>
#include <QScxmlError>
#endif

#define BASE_PATH "../test/data/xml/special/scxml"
#define FILE_STATES BASE_PATH "/states.xml"
#define FILE_CHECKLOAD BASE_PATH "/checkload.txt"
#define FILE_BASE_XML   BASE_PATH "/base.xml"
#define FILE_BASE   BASE_PATH "/base.scxml"
#define FILE_BASE_NOEXT BASE_PATH  "/base.aaa"

static QStringList allTokens()
{
    QStringList tokens ;

    tokens << "assign";
    tokens << "donedata";
    tokens << "cancel";
    tokens << "content";
    tokens << "data";
    tokens << "datamodel";
    tokens << "else";
    tokens << "elseif";
    tokens << "raise";
    tokens << "final";
    tokens << "finalize";
    tokens << "history";
    tokens << "if";
    tokens << "initial";
    tokens << "invoke";
    tokens << "foreach";
    tokens << "log";
    tokens << "onentry";
    tokens << "onexit";
    tokens << "parallel";
    tokens << "param";
    tokens << "script";
    tokens << "scxml";
    tokens << "send";
    tokens << "state";
    tokens << "transition";
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
    return testPanelShowFlagsNoHideAuto();
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

    if(!testEditassign() ) {
        return false;
    }
    if(!testEditcancel() ) {
        return false;
    }
    if(!testEditcontent() ) {
        return false;
    }
    if(!testEditdata() ) {
        return false;
    }
    if(!testEditdatamodel() ) {
        return false;
    }
    if(!testEditelse() ) {
        return false;
    }
    if(!testEditelseif() ) {
        return false;
    }
    if(!testEditif() ) {
        return false;
    }
    if(!testEditraise() ) {
        return false;
    }
    if(!testEditfinal() ) {
        return false;
    }
    if(!testEditfinalize() ) {
        return false;
    }
    if(!testEdithistory() ) {
        return false;
    }
    if(!testEditinitial() ) {
        return false;
    }
    if(!testEditinvoke() ) {
        return false;
    }
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
    if(!testEditparam() ) {
        return false;
    }
    if(!testEditscript() ) {
        return false;
    }
    if(!testEditscxml() ) {
        return false;
    }
    if(!testEditsend() ) {
        return false;
    }
    if(!testEditstate() ) {
        return false;
    }
    if(!testEdittransition() ) {
        return false;
    }
    if(!testEditdonedata() ) {
        return false;
    }
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
#ifdef QXMLEDIT_QT_SCXML_ENABLED
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
    SCXMLInfo::findInfoStates(regola, &info);
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
    SCXMLLogDialog dialog(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
    if(!setEAE(&dialog, "label", SCXMLlogToken::A_label, "alabel", &elementCompare)){return false;}
    if(!setEAE(&dialog, "expr", SCXMLlogToken::A_expr, "aexpr", &elementCompare)){return false;}
    dialog.accept();
    if(!compare(&elementCompare, &elementEdit)) {
        return false;
    }
    return true;
}

bool TestSCXML::testEditdatamodel()
{
    _testName ="testEditdatamodel" ;
    Element elementEdit(NULL);
    Element elementCompare(NULL);
    Regola regola;
    SCXMLInfo info;
    elementCompare.setTag(SCXMLToken::Tag_datamodel) ;
    SCXMLdatamodelToken token;
    token.editToken(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
    if(!compare(&elementCompare, &elementEdit)) {
        return false;
    }
    return true;
}

bool TestSCXML::testEditdata()
{
    _testName ="testEditdata" ;
    Element elementEdit(NULL);
    Element elementCompare(NULL);
    Regola regola;
    SCXMLInfo info;
    elementCompare.setTag(SCXMLToken::Tag_data) ;
    SCXMLDataDialog dialog(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
    if(!setEAE(&dialog, "id", SCXMLdataToken::A_id, "aID", &elementCompare)){return false;}
    if(!setEAE(&dialog, "expr", SCXMLdataToken::A_expr, "aExpr", &elementCompare)){return false;}
    if(!setEAE(&dialog, "src", SCXMLdataToken::A_src, "aSrc", &elementCompare)){return false;}
    dialog.accept();
    if(!compare(&elementCompare, &elementEdit)) {
        return false;
    }
    return true;
}

bool TestSCXML::testEditassign()
{
    _testName ="testEditassign" ;
    Element elementEdit(NULL);
    Element elementCompare(NULL);
    Regola regola;
    SCXMLInfo info;
    elementCompare.setTag(SCXMLToken::Tag_assign) ;
    SCXMLAssignDialog dialog(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
    if(!setEAE(&dialog, "location", SCXMLassignToken::A_location, "aLocation", &elementCompare)){return false;}
    if(!setEAE(&dialog, "expr", SCXMLassignToken::A_expr, "aExpr", &elementCompare)){return false;}
    dialog.accept();
    if(!compare(&elementCompare, &elementEdit)) {
        return false;
    }
    return true;
}

bool TestSCXML::testEditdonedata()
{
    _testName ="testEditdonedata" ;
    Element elementEdit(NULL);
    Element elementCompare(NULL);
    Regola regola;
    SCXMLInfo info;
    elementCompare.setTag(SCXMLToken::Tag_donedata) ;
    SCXMLdonedataToken token;
    token.editToken(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
    if(!compare(&elementCompare, &elementEdit)) {
        return false;
    }
    return true;
}

bool TestSCXML::testEditcontent()
{
    _testName ="testEditcontent" ;
    Element elementEdit(NULL);
    Element elementCompare(NULL);
    Regola regola;
    SCXMLInfo info;
    elementCompare.setTag(SCXMLToken::Tag_content) ;
    SCXMLContentDialog dialog(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
    if(!setEAE(&dialog, "expr", SCXMLcontentToken::A_expr, "aExpr", &elementCompare)){return false;}
    dialog.accept();
    if(!compare(&elementCompare, &elementEdit)) {
        return false;
    }
    return true;
}

bool TestSCXML::testEditparam()
{
    _testName ="testEditparam" ;
    Element elementEdit(NULL);
    Element elementCompare(NULL);
    Regola regola;
    SCXMLInfo info;
    elementCompare.setTag(SCXMLToken::Tag_param) ;
    SCXMLParamDialog dialog(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
    if(!setEAE(&dialog, "expr", SCXMLparamToken::A_expr, "aExpr", &elementCompare)){return false;}
    if(!setEAE(&dialog, "name", SCXMLparamToken::A_name, "aName", &elementCompare)){return false;}
    if(!setEAE(&dialog, "location", SCXMLparamToken::A_location, "aLocation", &elementCompare)){return false;}
    dialog.accept();
    if(!compare(&elementCompare, &elementEdit)) {
        return false;
    }
    return true;
}

bool TestSCXML::testEditscript()
{
    _testName ="testEditscript" ;
    Element elementEdit(NULL);
    Element elementCompare(NULL);
    Regola regola;
    SCXMLInfo info;
    elementCompare.setTag(SCXMLToken::Tag_script) ;
    SCXMLScriptDialog dialog(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
    if(!setEAE(&dialog, "src", SCXMLscriptToken::A_src, "aSrc", &elementCompare)){return false;}
    dialog.accept();
    if(!compare(&elementCompare, &elementEdit)) {
        return false;
    }
    return true;
}

bool TestSCXML::testEditcancel()
{
    _testName ="testEditcancel" ;
    Element elementEdit(NULL);
    Element elementCompare(NULL);
    Regola regola;
    SCXMLInfo info;
    elementCompare.setTag(SCXMLToken::Tag_cancel) ;
    SCXMLcancelToken token;
    token.editToken(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
    if(!compare(&elementCompare, &elementEdit)) {
        return false;
    }
    return true;
}

bool TestSCXML::testEditfinalize()
{
    _testName ="testEditfinalize" ;
    Element elementEdit(NULL);
    Element elementCompare(NULL);
    Regola regola;
    SCXMLInfo info;
    elementCompare.setTag(SCXMLToken::Tag_finalize) ;
    SCXMLfinalizeToken token;
    token.editToken(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
    if(!compare(&elementCompare, &elementEdit)) {
        return false;
    }
    return true;
}

bool TestSCXML::testEditinvoke()
{
    _testName ="testEditinvoke" ;
    {
        Element elementEdit(NULL);
        Element elementCompare(NULL);
        Regola regola;
        SCXMLInfo info;
        elementCompare.setTag(SCXMLToken::Tag_invoke) ;
        SCXMLInvokeDialog dialog(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
        //if(!setEAC(&dialog, "type", SCXMLinvokeToken::A_type, "atype", &elementCompare)){return false;}
        if(!setEAE(&dialog, "typeexpr", SCXMLinvokeToken::A_typeexpr, "atypeexpr", &elementCompare)){return false;}
        //if(!setEAE(&dialog, "src", SCXMLinvokeToken::A_src, "asrc", &elementCompare)){return false;}
        if(!setEAE(&dialog, "srcexpr", SCXMLinvokeToken::A_srcexpr, "asrcexpr", &elementCompare)){return false;}
        //if(!setEAE(&dialog, "id", SCXMLinvokeToken::A_id, "aid", &elementCompare)){return false;}
        if(!setEAE(&dialog, "idlocation", SCXMLinvokeToken::A_idlocation, "aidlocation", &elementCompare)){return false;}
        if(!setEAE(&dialog, "namelist", SCXMLinvokeToken::A_namelist, "anamelist", &elementCompare)){return false;}
        if(!setEAB(&dialog, "autoforward", true, SCXMLinvokeToken::A_autoforward, "autoforward", &elementCompare)){return false;}
        dialog.accept();
        if(!compare(&elementCompare, &elementEdit)) {
            return false;
        }
    }
    //---
    {
        Element elementEdit(NULL);
        Element elementCompare(NULL);
        Regola regola;
        SCXMLInfo info;
        elementCompare.setTag(SCXMLToken::Tag_invoke) ;
        SCXMLInvokeDialog dialog(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
        if(!setEAC(&dialog, "type", SCXMLinvokeToken::A_type, "atype", &elementCompare)){return false;}
        //if(!setEAE(&dialog, "typeexpr", SCXMLinvokeToken::A_typeexpr, "atypeexpr", &elementCompare)){return false;}
        if(!setEAE(&dialog, "src", SCXMLinvokeToken::A_src, "asrc", &elementCompare)){return false;}
        //if(!setEAE(&dialog, "srcexpr", SCXMLinvokeToken::A_srcexpr, "asrcexpr", &elementCompare)){return false;}
        if(!setEAE(&dialog, "id", SCXMLinvokeToken::A_id, "aid", &elementCompare)){return false;}
        //if(!setEAE(&dialog, "idlocation", SCXMLinvokeToken::A_idlocation, "aidlocation", &elementCompare)){return false;}
        if(!setEAE(&dialog, "namelist", SCXMLinvokeToken::A_namelist, "anamelist", &elementCompare)){return false;}
        if(!setEAB(&dialog, "autoforward", true, SCXMLinvokeToken::A_autoforward, "autoforward", &elementCompare)){return false;}
        dialog.accept();
        if(!compare(&elementCompare, &elementEdit)) {
            return false;
        }
    }
    return true;
}

bool TestSCXML::testEditsend()
{
    _testName ="testEditsend" ;
    {
        Element elementEdit(NULL);
        Element elementCompare(NULL);
        Regola regola;
        SCXMLInfo info;
        elementCompare.setTag(SCXMLToken::Tag_send) ;
        SCXMLSendDialog dialog(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
        if(!setEAE(&dialog, "event", SCXMLsendToken::A_event, "aevent", &elementCompare)){return false;}
        //if(!setEAE(&dialog, "eventexpr", SCXMLsendToken::A_eventexpr, "aeventexpr", &elementCompare)){return false;}
        if(!setEAE(&dialog, "target", SCXMLsendToken::A_target, "atarget", &elementCompare)){return false;}
        //if(!setEAE(&dialog, "targetexpr", SCXMLsendToken::A_targetexpr, "atargetexpr", &elementCompare)){return false;}
        if(!setEAE(&dialog, "type", SCXMLsendToken::A_type, "atype", &elementCompare)){return false;}
        //if(!setEAE(&dialog, "typeexpr", SCXMLsendToken::A_typeexpr, "atypeexpr", &elementCompare)){return false;}
        if(!setEAE(&dialog, "id", SCXMLsendToken::A_id, "aid", &elementCompare)){return false;}
        //if(!setEAE(&dialog, "idlocation", SCXMLsendToken::A_idlocation, "aidlocation", &elementCompare)){return false;}
        if(!setEAE(&dialog, "delay", SCXMLsendToken::A_delay, "adelay", &elementCompare)){return false;}
        //if(!setEAE(&dialog, "delayexpr", SCXMLsendToken::A_delayexpr, "adelayexpr", &elementCompare)){return false;}
        if(!setEAE(&dialog, "namelist", SCXMLsendToken::A_namelist, "anamelist", &elementCompare)){return false;}
        dialog.accept();
        if(!compare(&elementCompare, &elementEdit)) {
            return false;
        }
    }
    //---
    {
        Element elementEdit(NULL);
        Element elementCompare(NULL);
        Regola regola;
        SCXMLInfo info;
        elementCompare.setTag(SCXMLToken::Tag_send) ;
        SCXMLSendDialog dialog(NULL, &info, &regola, true, true, &elementEdit, &elementEdit, NULL);
        //if(!setEAE(&dialog, "event", SCXMLsendToken::A_event, "aevent", &elementCompare)){return false;}
        if(!setEAE(&dialog, "eventexpr", SCXMLsendToken::A_eventexpr, "aeventexpr", &elementCompare)){return false;}
        //if(!setEAE(&dialog, "target", SCXMLsendToken::A_target, "atarget", &elementCompare)){return false;}
        if(!setEAE(&dialog, "targetexpr", SCXMLsendToken::A_targetexpr, "atargetexpr", &elementCompare)){return false;}
        //if(!setEAE(&dialog, "type", SCXMLsendToken::A_type, "atype", &elementCompare)){return false;}
        if(!setEAE(&dialog, "typeexpr", SCXMLsendToken::A_typeexpr, "atypeexpr", &elementCompare)){return false;}
        //if(!setEAE(&dialog, "id", SCXMLsendToken::A_id, "aid", &elementCompare)){return false;}
        if(!setEAE(&dialog, "idlocation", SCXMLsendToken::A_idlocation, "aidlocation", &elementCompare)){return false;}
        //if(!setEAE(&dialog, "delay", SCXMLsendToken::A_delay, "adelay", &elementCompare)){return false;}
        if(!setEAE(&dialog, "delayexpr", SCXMLsendToken::A_delayexpr, "adelayexpr", &elementCompare)){return false;}
        if(!setEAE(&dialog, "namelist", SCXMLsendToken::A_namelist, "anamelist", &elementCompare)){return false;}
        dialog.accept();
        if(!compare(&elementCompare, &elementEdit)) {
            return false;
        }
    }
    return true;
}

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
    bool freeMessages = true;
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
    SourceRelatedMessages *panel = app.mainWindow()->sourceRelatedMessages();
    if( NULL == panel ) {
        error("Unable to find messages panel");
    } else {
        panel->setMessages(errors);
        freeMessages = false ;
        QListWidget *list = panel->findChild<QListWidget*>("messages");
        if( NULL == list ) {
            error("Unable to find messages list");
        } else {
            list->setCurrentRow(2, QItemSelectionModel::Select);
            QPushButton *button = panel->findChild<QPushButton*>("go");
            if( NULL == button ) {
                error("Unable to find button");
            } else {
                button->click();
                Element *selection = app.mainWindow()->getEditor()->getSelectedItem();
                if(NULL == selection) {
                    error("NULL selection after jump.");
                } else {
                    SourceMessage *m1 = errors.at(2);
                    QList<int> pos = selection->indexPath();
                    QList<int> path = m1->path() ;
                    compareListInts("final compare", pos, path);
                }
            }
        }
    }
    if(freeMessages) {
        EMPTYPTRLIST(errors, SourceMessage);
    }
    return !isError() ;
}

//---------------------------------------------------------

bool TestSCXML::testPanel()
{
    _testName = "testPanel" ;
    //testa flag di pannello con xml e xslt: mostra, no mostra
    if( !testPanelShowOnOpenFileXml()) {
        return false;
    }
    if( !testPanelShowOnOpenFileSCXML()) {
        return false;
    }
    //testa combinazioni flag mostra con flag auto, 4 combinazioni apertura pannello
    if( !testPanelShowFlagsNoHideNoAuto() ) {
        return false;
    }
    if( !testPanelShowFlagsNoHideAuto() ) {
        return false;
    }
    if( !testPanelShowFlagsHideNoAuto() ) {
        return false;
    }
    if( !testPanelShowFlagsHideAuto() ) {
        return false;
    }
    if(!testPanelShowWithAnswers() ) {
        return false;
    }
    //----------------------------------------------------------------
    return true ;
}

bool TestSCXML::testPanelShowOnOpenFile(const QString &fileName, const SCXMLAutoModeDialog::PrivateTest::Tests testToDo, const bool expected)
{
    App app;
    if(!app.initNoWindow() ) {
        return error("init window");
    }
    MainWindow mainWindow(false, false, app.data());
    SCXMLAutoModeDialog::PrivateTest::setTestToExecute(testToDo);
    SCXMLAutoModeDialog::PrivateTest::setPanelOpen(false);
    if( !mainWindow.loadFile(QString(fileName)) ) {
        return error(QString("opening test file: '%1'").arg(fileName));
    }
    if( expected != SCXMLAutoModeDialog::PrivateTest::panelOpen() ) {
        return error(QString("test failed panel shown:%1 with file '%2', expected:%3, found:%4")
                     .arg(SCXMLAutoModeDialog::PrivateTest::testMessage()).arg(fileName).arg(expected).arg(SCXMLAutoModeDialog::PrivateTest::panelOpen()));
    }

    //----------------------------------------------------------------
    return true ;
}


bool TestSCXML::testPanelShowOnOpenFileXml()
{
    _testName = "testPanel/testPanelShowOnOpenFileXml" ;
    return testPanelShowOnOpenFile(FILE_BASE_XML, SCXMLAutoModeDialog::PrivateTest::TEST_SHOWPANEL_XML_FILE, false);
}

bool TestSCXML::testPanelShowOnOpenFileSCXML()
{
    _testName = "testPanel/testPanelShowOnOpenFileSCXML" ;
    return testPanelShowOnOpenFile(FILE_BASE, SCXMLAutoModeDialog::PrivateTest::TEST_SHOWPANEL_SCXML_FILE, true);
}

bool TestSCXML::testPanelShowOnOpenFileUsingFlags(const SCXMLAutoModeDialog::PrivateTest::Tests testToDo, const bool isShow, const bool isAuto, const bool expected )
{
    App app;
    if(!app.initNoWindow() ) {
        return error("init window");
    }
    MainWindow mainWindow(false, false, app.data());
    app.data()->setAutoSCXMLMode(isAuto);
    app.data()->setShowSCXMLPanel(isShow);
    SCXMLAutoModeDialog::PrivateTest::setPanelOpen(false);
    SCXMLAutoModeDialog::PrivateTest::setTestToExecute(testToDo);
    if( !mainWindow.loadFile(QString(FILE_BASE)) ) {
        return error(QString("opening test file: '%1'").arg(FILE_BASE));
    }
    if( expected != SCXMLAutoModeDialog::PrivateTest::panelOpen() ) {
        return error(QString("test failed panel shown:%1 with file '%2' flag was show:%3, auto:%4, expected:%5, found:%6")
                     .arg(SCXMLAutoModeDialog::PrivateTest::testMessage()).arg(FILE_BASE)
                     .arg(isShow).arg(isAuto).arg(expected).arg(SCXMLAutoModeDialog::PrivateTest::panelOpen()));
    }

    //----------------------------------------------------------------
    return true ;
}

bool TestSCXML::testPanelShowFlagsNoHideNoAuto()
{
    _testName = "testPanel/testPanelShowFlagsNoHideNoAuto" ;
    return testPanelShowOnOpenFileUsingFlags(SCXMLAutoModeDialog::PrivateTest::TEST_SHOW_NOHIDE_NOAUTO, true, false, true);
}

bool TestSCXML::testPanelShowFlagsNoHideAuto()
{
    _testName = "testPanel/testPanelShowFlagsNoHideAuto" ;
    return testPanelShowOnOpenFileUsingFlags(SCXMLAutoModeDialog::PrivateTest::TEST_SHOW_NOHIDE_AUTO, true, true, false);
}

bool TestSCXML::testPanelShowFlagsHideNoAuto()
{
    _testName = "testPanel/testPanelShowFlagsHideNoAuto" ;
    return testPanelShowOnOpenFileUsingFlags(SCXMLAutoModeDialog::PrivateTest::TEST_SHOW_HIDE_NOAUTO, false, false, false);
}

bool TestSCXML::testPanelShowFlagsHideAuto()
{
    _testName = "testPanel/testPanelShowFlagsHideAuto" ;
    return testPanelShowOnOpenFileUsingFlags(SCXMLAutoModeDialog::PrivateTest::TEST_SHOW_HIDE_AUTO, false, false, false);
}

bool TestSCXML::testPanelAnswers( const QString &fileName,
                                    const SCXMLAutoModeDialog::PrivateTest::Tests testToDo, const bool isShow, const SCXMLAutoModeDialog::ERetCode answer,
                                    const bool expectedFirstShot, const bool expectedOpenSecondShot,
                                    const bool expectedSCXMLAfterFirstShot, const bool expectedSCXMLAfterSecondShot )
{
    App app;
    if(!app.initNoWindow() ) {
        return error("init window");
    }
    MainWindow mainWindow(false, false, app.data());
    SCXMLAutoModeDialog::PrivateTest::setTestToExecute(testToDo);
    SCXMLAutoModeDialog::PrivateTest::setAnswerShowSCXMLPanel(isShow);
    SCXMLAutoModeDialog::PrivateTest::setAnswer(answer);
    if( !mainWindow.loadFile(QString(fileName)) ) {
        return error(QString("1. opening test file: '%1'").arg(fileName));
    }
    if( expectedFirstShot != SCXMLAutoModeDialog::PrivateTest::panelOpen() ) {
        return error(QString("test failed panel with file:'%1'' flag was show:%2, answer:%3, expected open:%4, found:%5")
                     .arg(fileName)
                     .arg(isShow).arg(answer).arg(expectedFirstShot).arg(SCXMLAutoModeDialog::PrivateTest::panelOpen()));
    }

    if( !SCXMLAutoModeDialog::PrivateTest::testPassed()) {
        return error(QString("test failed with message panel with message '%6' file:'%1' flag was show:%2, answer:%3, expected open:%4, found:%5")
                     .arg(fileName)
                     .arg(isShow).arg(answer).arg(expectedFirstShot).arg(SCXMLAutoModeDialog::PrivateTest::panelOpen())
                     .arg(SCXMLAutoModeDialog::PrivateTest::testMessage()));
    }

    if( expectedSCXMLAfterFirstShot != (mainWindow.getEditor()->editMode() == XmlEditWidgetEditMode::SCXML )) {
        return error(QString(" xml edit mode after first shot was not expected file:'%1' flag was show:%2, answer:%3, edit mode exp shot:%4, edit mode found:%5, test:%6")
                     .arg(fileName)
                     .arg(isShow).arg(answer).arg(expectedFirstShot).arg(mainWindow.getEditor()->editMode())
                     .arg(testToDo));

    }

    SCXMLAutoModeDialog::PrivateTest::setPanelOpen(false);
    // second shot
    if( !mainWindow.loadFile(QString(fileName)) ) {
        return error(QString("2. opening test file: '%1'").arg(fileName));
    }
    if( expectedOpenSecondShot != SCXMLAutoModeDialog::PrivateTest::panelOpen() ) {
        return error(QString("Second shot test failed shown: with file '%1' flag was show:%2, answer:%3, expected first shot:%4, found:%5")
                     .arg(fileName)
                     .arg(isShow).arg(answer).arg(expectedFirstShot).arg(SCXMLAutoModeDialog::PrivateTest::panelOpen()));
    }
    if( expectedSCXMLAfterSecondShot != (mainWindow.getEditor()->editMode() == XmlEditWidgetEditMode::SCXML )) {
        return error(QString(" xml edit mode after second shot was not expected file:'%1' flag was show:%2, answer:%3, edit mode exp shot:%4, edit mode found:%5")
                     .arg(fileName)
                     .arg(isShow).arg(answer).arg(expectedSCXMLAfterSecondShot).arg(mainWindow.getEditor()->editMode()));

    }

    // loading a regular xml
    SCXMLAutoModeDialog::PrivateTest::setPanelOpen(false);
    if( !mainWindow.loadFile(QString(FILE_BASE_XML)) ) {
        return error(QString("3. opening test file: '%1'").arg(FILE_BASE_XML));
    }
    if( SCXMLAutoModeDialog::PrivateTest::panelOpen() ) {
        return error(QString("Loading regular XML with file '%1' flag was show:%2, answer:%3, expected first shot:%4, found:%5")
                     .arg(FILE_BASE_XML)
                     .arg(isShow).arg(answer).arg(expectedFirstShot).arg(SCXMLAutoModeDialog::PrivateTest::panelOpen()));
    }
    if( mainWindow.getEditor()->editMode() != XmlEditWidgetEditMode::XML) {
        return error(QString(" regular xml did not trigger xml edit mode file:'%1' flag was show:%2, answer:%3, edit mode found:%4")
                     .arg(FILE_BASE_XML)
                     .arg(isShow).arg(answer).arg(mainWindow.getEditor()->editMode()));

    }
    //----------------------------------------------------------------
    return true ;
}

struct TestInfoData {
    QString testName;
    SCXMLAutoModeDialog::PrivateTest::Tests testToDo;
    bool isShow;
    SCXMLAutoModeDialog::ERetCode answer;
    bool expectedFirstShot;
    bool expectedOpenSecondShot;
    bool expectedSCXMLAfterFirstShot;
    bool expectedSCXMLAfterSecondShot;
};

bool TestSCXML::testPanelShowWithAnswers()
{
    TestInfoData testData[] = {
        { "Ys", SCXMLAutoModeDialog::PrivateTest::TEST_BEHAVIOUR, true, SCXMLAutoModeDialog::ENTER_SCXMLMODE, true, true, true, true },
        { "Yn", SCXMLAutoModeDialog::PrivateTest::TEST_BEHAVIOUR, false, SCXMLAutoModeDialog::ENTER_SCXMLMODE, true, false, true, false },
        { "Ns", SCXMLAutoModeDialog::PrivateTest::TEST_BEHAVIOUR, true, SCXMLAutoModeDialog::DONOTENTER_SCXMLMODE, true, true, false, false },
        { "Nn", SCXMLAutoModeDialog::PrivateTest::TEST_BEHAVIOUR, false, SCXMLAutoModeDialog::DONOTENTER_SCXMLMODE, true, false, false, false },
        { "As", SCXMLAutoModeDialog::PrivateTest::TEST_BEHAVIOUR, true, SCXMLAutoModeDialog::ALWAYS_SCXMLMODE, true, false, true, true },
        { "An", SCXMLAutoModeDialog::PrivateTest::TEST_BEHAVIOUR, false, SCXMLAutoModeDialog::ALWAYS_SCXMLMODE, true, false, true, true },
        // guard
        { "", SCXMLAutoModeDialog::PrivateTest::TEST_BEHAVIOUR, false, SCXMLAutoModeDialog::ENTER_SCXMLMODE, false, false, true, true },
    };
    /*int size = sizeof(testData)/sizeof(testData[0]); // hoping for the best
    for( int i = 0 ; i < size ; i ++ ) {
        TestInfoData *test = testData[i];
    }*/
    TestInfoData *test = &testData[0];
    while( !test->testName.isEmpty() ) {
        _testName = QString("TestSCXMLTMode/infoTest/%1").arg(test->testName);
        if( !testPanelAnswers(FILE_BASE, test->testToDo, test->isShow, test->answer, test->expectedFirstShot, test->expectedOpenSecondShot, test->expectedSCXMLAfterFirstShot, test->expectedSCXMLAfterSecondShot) ) {
            return false;
        }
        test++;
        //printf("test...\n");
    }
    test = &testData[0];
    _testName = QString("TestSCXMLTMode/infoTest/%1").arg(test->testName);
    if( !testPanelAnswers(FILE_BASE_NOEXT, test->testToDo, test->isShow, test->answer, test->expectedFirstShot, test->expectedOpenSecondShot, test->expectedSCXMLAfterFirstShot, test->expectedSCXMLAfterSecondShot) ) {
        return false;
    }
    return true;
}



/*
testa flag always al caricamento con XML e SCXML

testa risposta 3 *sinosempre con flag mostra e no mostra
caricamento successivo

start | Y|N |A | <- answer
first |sn|sn|sn| <- show/not show at the first start
---------------
scx?  |yy|nn|yy|
second|on|on|nn|
scx?  |yn|nn|yy|
xml   |nn|nn|nn|

with xml on the second run nothing will open

*/
