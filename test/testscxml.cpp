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

#define BASE_PATH "../test/data/xml/specialized/scxml"
#define FILE_STATES BASE_PATH "/states.xml"

static QStringList allTokens()
{
    QStringList tokens ;

    //tokens << "assign";
    //tokens << "cancel";
    //tokens << "content";
    //tokens << "data";
    //tokens << "datamodel";
    //tokens << "else";
    //tokens << "elseif";
    //tokens << "raise";
    //tokens << "final";
    //tokens << "finalize";
    //tokens << "history";
    //tokens << "if";
    //tokens << "initial";
    //tokens << "invoke";
    //tokens << "foreach";
    //tokens << "log";
    //tokens << "onentry";
    //tokens << "onexit";
    //tokens << "parallel";
    //tokens << "param";
    //tokens << "script";
    tokens << "scxml";
    //tokens << "send";
    //tokens << "state";
    //tokens << "transition";
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
    return testEditscxml();
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
    bool testEditraise();
    bool testEditfinal();
    bool testEditfinalize();
    bool testEdithistory();
    bool testEditif();
    bool testEditinitial();
    bool testEditinvoke();
    bool testEditforeach();
    bool testEditlog();
    bool testEditonentry();
    bool testEditonexit();
    bool testEditparallel();
    bool testEditparam();
    bool testEditscript();*/
    if(!testEditscxml() ) {
        return false;
    }
    /*bool testEditsend();
    bool testEditstate();
    bool testEdittransition();
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
    return error("nyi");
}

bool TestSCXML::testTemplatesCompile()
{
    _testName = "testTemplatesCompile" ;
    return error("nyi");
}

bool TestSCXML::testTemplatesLoadStates()
{
    _testName = "testTemplatesLoadStates" ;
    /*App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(FILE_STATES) ) {
        return error(QString("unable to load input file: '%1' ").arg(FILE_STATES));
    }
    Regola *regola = app.mainWindow()->getRegola();
*/
    return error("nyi");
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


bool TestSCXML::testEditassign()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditcancel()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditcontent()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditdata()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditdatamodel()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditelse()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditelseif()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditraise()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditfinal()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditfinalize()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEdithistory()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditif()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditinitial()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditinvoke()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditforeach()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditlog()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditonentry()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditonexit()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditparallel()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditparam()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditscript()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditsend()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditstate()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEdittransition()
{_testName ="" ;return error("nyi");}bool TestSCXML::testEditvar()
{_testName ="" ;return error("nyi");}

