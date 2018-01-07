/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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


#include "testprolog.h"
#include "app.h"
#include "modules/metadata/prologparser.h"
#include "comparexml.h"


#define AUTOINS_PROLOG  "../test/data/prolog/auto_ins.xml"

TestProlog::TestProlog()
{
}

TestProlog::~TestProlog()
{
}

//-------
/**

  test manual insert prolog:

  -insert
  a. a prolog is already in place
  b. no prolog, other p.i exists
  c. no prolog, data
  d. empty


  test autoinsert prolog and encoding:

situation |  enabled | encoding   | result
    yes   |    yes   |   no       | prolog, no encoding
    yes   |    yes   |   yes      | prolog, selected encoding
    any   |    no    |   any      | no prolog

situations:
 load from file: no
 new : yes
 new from clipboard: no

 */

#define INSPROLOG_EMPTY_RESULT      "../test/data/prolog/ins_empty_res.xml"
#define INSPROLOG_EXISTING          "../test/data/prolog/ins_exist.xml"
#define INSPROLOG_EXISTING_RESULT   "../test/data/prolog/ins_exist.xml"
#define INSPROLOG_DATA              "../test/data/prolog/ins_data.xml"
#define INSPROLOG_DATA_RESULT       "../test/data/prolog/ins_data_res.xml"
#define INSPROLOG_OTHERPI           "../test/data/prolog/ins_pi.xml"
#define INSPROLOG_OTHERPI_RESULT    "../test/data/prolog/ins_pi_res.xml"



bool TestProlog::testRecognizeProlog(Element *element, const bool expected )
{
    bool recognized = false ;
    PrologParser parser ;
    if( parser.isProlog(element)) {
        recognized = true ;
    }
    if( expected != recognized ) {
        return error( QString("Failed to detect prolog, expecting:%1 result is:%2 for:%3").arg(expected).arg(recognized).arg(element->getPIData()));
    }
    return true;
}

bool TestProlog::testUnitTests()
{
    _testName = "testUnitTests";
    Element element( "version", "", NULL, NULL);
    Element pi(NULL, Element::ET_PROCESSING_INSTRUCTION, NULL);
    if( !testRecognizeProlog(&element, false)) {
        return false;
    }
    pi.setPITarget("xml");
    pi.setPIData("");
    if( !testRecognizeProlog(&pi, false)) {
        return false;
    }
    pi.setPIData("version='1.0'");
    if( !testRecognizeProlog(&pi, true)) {
        return false;
    }
    pi.setPITarget("XML");
    pi.setPIData("version='1.0'");
    if( !testRecognizeProlog(&pi, false)) {
        return false;
    }
    pi.setPITarget("cml");
    pi.setPIData("version='1.0'");
    if( !testRecognizeProlog(&pi, false)) {
        return false;
    }
    return true;
}

bool TestProlog::testInsertProlog()
{
    if( !testDetectProlog("", INSPROLOG_EMPTY_RESULT, "new") ) {
        return false;
    }
    if( !testDetectProlog(INSPROLOG_EXISTING, INSPROLOG_EXISTING_RESULT, "existing") ) {
        return false;
    }
    if( !testDetectProlog(INSPROLOG_DATA, INSPROLOG_DATA_RESULT, "only data") ) {
        return false;
    }
    if( !testDetectProlog(INSPROLOG_OTHERPI, INSPROLOG_OTHERPI_RESULT, "otherPI") ) {
        return false;
    }
    return true;
}


bool TestProlog::testFunctionalTests()
{
    //------
    if(!testInsertProlog()) {
        return false;
    }
    //--------
    if( !testAutoInsertProlog()) {
        return false;
    }
    return true;
}

//---------

bool TestProlog::testDetectProlog(const QString &argFileName, const QString &argFileResult, const QString &label)
{
    _testName = QString("TestProlog/testDetectProlog:%1").arg(label) ;
    App app;
    if(!app.init() ) {
        return error("init app");
    }
    if( !argFileName.isEmpty()) {
        if( !app.mainWindow()->loadFile(argFileName)) {
           return error(QString("Opening file '%1'").arg(argFileName));
        }
    } else {
        Regola *regola = app.mainWindow()->getRegola();
        Element *el = new Element( "root", "", regola, NULL);
        if( NULL == el ) {
            return error("Unable to create empty element");
        }
        regola->setRootElement(el);
    }
    if( !fireAction(app.mainWindow(), "actionInsertProlog") ) {
        return error(QString("Unable to fire insert prolog action "));
    }
    {
        CompareXML compare;
        if(!compare.compareFileWithRegola(app.mainWindow()->getRegola(), argFileResult )) {
            return error( QString("Verify %2 cause:%1").arg(compare.errorString()).arg(label));
        }
    }
    app.mainWindow()->getRegola()->undo();
    if( !argFileName.isEmpty() )
    {
        CompareXML compare;
        if(!compare.compareAFileWithRegola(argFileName, app.mainWindow()->getRegola())) {
            return error( QString("Verify undo %2, cause:%1").arg(compare.errorString()).arg(label));
        }
    }
    app.mainWindow()->getRegola()->redo();
    {
        CompareXML compare;
        if(!compare.compareAFileWithRegola(argFileResult,app.mainWindow()->getRegola())) {
            return error( QString("Verify redo %2 cause:%1").arg(compare.errorString()).arg(label));
        }
    }
    return true;
}

class TestAutoInsParams {
public:
    enum EOpenType {
        O_NEW,
        O_OPEN,
        O_NEWFROMCLIPBOARD
    };

    EOpenType openType;
    bool enableAutoIns;
    QString encoding;
    QString result;

    TestAutoInsParams( const EOpenType pOpenType, const bool pEnableAutoIns, const QString & pEncoding, const QString &pResult) {
        openType = pOpenType ;
        enableAutoIns = pEnableAutoIns ;
        encoding = pEncoding ;
        result = pResult ;
    }

};

QString TestProlog::describeParams(TestAutoInsParams *classa)
{
    QString result = QString("TestAutoInsParams is: openType:%1, enableAutoIns:%2, encoding:%3, result:%4")
        .arg(classa->openType)//1
        .arg(classa->enableAutoIns)//2
        .arg(classa->encoding)//3
        .arg(classa->result);//4
    return result;
}

bool TestProlog::testAutoInsertProlog()
{
    _testName = "testAutoInsertProlog";
    {
        TestAutoInsParams t(TestAutoInsParams::O_NEW, false, "", "");
        if(!testAutoIns(&t)) {
            return false;
        }
    }
    {
        TestAutoInsParams t(TestAutoInsParams::O_NEW, false, "true, ", "");
        if(!testAutoIns(&t)) {
            return false;
        }
    }
    {
        TestAutoInsParams t(TestAutoInsParams::O_NEW, true, "", " version=\"1.0\" encoding=\"UTF-8\"");
        if(!testAutoIns(&t)) {
            return false;
        }
    }
    {
        TestAutoInsParams t(TestAutoInsParams::O_NEW, true, "X", " version=\"1.0\" encoding=\"X\"");
        if(!testAutoIns(&t)) {
            return false;
        }
    }
    {
        TestAutoInsParams t(TestAutoInsParams::O_NEW, true, "ISO-8859-15", " version=\"1.0\" encoding=\"ISO-8859-15\"");
        if(!testAutoIns(&t)) {
            return false;
        }
    }
    {
        TestAutoInsParams t(TestAutoInsParams::O_NEWFROMCLIPBOARD, false, "UTF-8", "");
        if(!testAutoIns(&t)) {
            return false;
        }
    }
    {
        TestAutoInsParams t(TestAutoInsParams::O_NEWFROMCLIPBOARD, true, "UTF-8", "");
        if(!testAutoIns(&t)) {
            return false;
        }
    }
    {
        TestAutoInsParams t(TestAutoInsParams::O_NEWFROMCLIPBOARD, false, "", "");
        if(!testAutoIns(&t)) {
            return false;
        }
    }
    {
        TestAutoInsParams t(TestAutoInsParams::O_NEWFROMCLIPBOARD, true, "", "");
        if(!testAutoIns(&t)) {
            return false;
        }
    }
    {
        TestAutoInsParams t(TestAutoInsParams::O_OPEN, false, "UTF-8", "");
        if(!testAutoIns(&t)) {
            return false;
        }
    }
    {
        TestAutoInsParams t(TestAutoInsParams::O_OPEN, true, "UTF-8", "");
        if(!testAutoIns(&t)) {
            return false;
        }
    }
    {
        TestAutoInsParams t(TestAutoInsParams::O_OPEN, false, "", "");
        if(!testAutoIns(&t)) {
            return false;
        }
    }
    {
        TestAutoInsParams t(TestAutoInsParams::O_OPEN, true, "", "");
        if(!testAutoIns(&t)) {
            return false;
        }
    }
    return true;
}

bool TestProlog::testAutoIns(TestAutoInsParams *params)
{
    App app;
    if(!app.init(true)) {
        return error(QString("init app, params:%1").arg(describeParams(params)));
    }
    if(app.mainWindow()->getRegola()->getItems().size() > 0 ) {
        return error(QString("at start expecting no elements, found:%1, params:%2").arg(app.mainWindow()->getRegola()->getItems().size()).arg(describeParams(params)));
    }
    app.data()->setAutoInsertProlog(params->enableAutoIns);
    app.data()->setAutoInsertPrologEncoding(params->encoding);

    switch(params->openType) {
    case TestAutoInsParams::O_NEW:
        if(!fireAction( app.mainWindow(), "actionNew" ) ) {
            return error( QString("error firing new, state:%1").arg(describeParams(params)) );
        }
        break;
    case TestAutoInsParams::O_NEWFROMCLIPBOARD:
        {
            QApplication::clipboard()->setText("<root/>");
            if(!fireAction( app.mainWindow(), "actionNewFromClipboard" ) ) {
                return error( QString("error firing new from clipboard, state:%1").arg(describeParams(params)) );
            }
        }
        break;

    case TestAutoInsParams::O_OPEN:
        if(!app.mainWindow()->loadFile(AUTOINS_PROLOG) ) {
            return error( QString("error reading file open, state:%1").arg(describeParams(params)) );
        }
        break;
    default:
        return error( QString("new method not recognized, state:%1").arg(describeParams(params)) );
    }

    // the xml can be not valid here, so check the elements
    bool expected = !params->result.isEmpty();
    bool existsProlog = false;
    QString prologData;
    Regola *regola = app.mainWindow()->getRegola();
    if( regola->getChildItems()->size()>0 ) {
        Element *first = regola->getChildItems()->at(0);
        PrologParser parser ;
        if( parser.isProlog(first)) {
            existsProlog = true ;
            prologData = first->getPIData() ;
        }
    }
    if( expected != existsProlog ) {
        return error( QString("After new differs, found:%1, data:'%2' params:%3").arg(existsProlog).arg(prologData).arg(describeParams(params)) );
    }
    if( expected ) {
        if( prologData != params->result ) {
            return error( QString("After new data differs, found:'%1'', expecting:'%2' params:%3").arg(prologData).arg(params->result).arg(describeParams(params)) );
        }
    }
    return true;
}
