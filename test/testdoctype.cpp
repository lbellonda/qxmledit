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

#include "testdoctype.h"
#include "comparexml.h"
#include <QTest>
#include <QPlainTextEdit>
#include "infodialog.h"

#define INPUT_TYPE  "../test/data/dtd/doctype.xml"
#define INPUT_SYSTEM    "../test/data/dtd/doctype_system.xml"
#define INPUT_PUBLIC    "../test/data/dtd/doctype_public.xml"

#define OUTPUT_TYPE  "../test/data/dtd/doctype_output.xml"
#define OUTPUT_SYSTEM    "../test/data/dtd/doctype_system_output.xml"
#define OUTPUT_PUBLIC    "../test/data/dtd/doctype_public_output.xml"

#define LOAD_TEST  "../test/data/dtd/testloader.xml"


#define TEST_BASE  "../test/data/dtd/"
#define LOAD_TEST_ATTR  TEST_BASE "attlist.xml"

TestDocType::TestDocType()
{
}

TestDocType::~TestDocType()
{
}

bool TestDocType::test()
{
    if(!testCommentLoading()) {
        return false;
    }
    if( !testType() ) {
        return false;
    }
    if( !testPublic() ) {
        return false;
    }
    if( !testSystem() ) {
        return false;
    }
    if( !testDTD() ) {
        return false;
    }
    return true;
}

bool TestDocType::testFast()
{
    if( !testDTD() ) {
        return false;
    }
    return true;
}

bool TestDocType::compareDocuments(const QString &filename, Regola *regola)
{
    QByteArray resultData = regola->writeMemory();
    QDomDocument document1;
    QDomDocument document2;
    CompareXML compare;
    if(!compare.loadFileIntoDocument(filename, document1)) {
        return error("load file 1");
    }
    QBuffer outputData(&resultData);
    if(!compare.loadFileIntoDocument(&outputData, document2)) {
        return error("load file 2");
    }
    bool result = compare.compareDomDocuments(document1, document2);
    if( !result ) {
        return error(QString("not result %s").arg(compare.errorString()));
    }
    return result ;
}

bool TestDocType::go(const QString &fileNameIn, const QString &fileNameOut)
{
    App app;
    if(!app.init() ) {
        return error("init app");
    }
    if(!app.mainWindow()->loadFile(fileNameIn)) {
        return error(QString("error loading %1").arg(fileNameIn));
    }
    Regola *regola = app.mainWindow()->getRegola();

    if(!compareDocuments(fileNameOut, regola) ) {
        return error("comparing docs");
    }

    return true;
}

bool TestDocType::testType()
{
    return go(INPUT_TYPE, OUTPUT_TYPE);
}

bool TestDocType::testPublic()
{
    return go(INPUT_PUBLIC, OUTPUT_PUBLIC);
}

bool TestDocType::testSystem()
{
    return go(INPUT_SYSTEM, OUTPUT_SYSTEM);
}

bool TestDocType::testCommentLoading()
{
    _testName = "testCommentLoading";
    App app;
    if(!app.init() ) {
        return error("init app");
    }
    if(!app.mainWindow()->loadFile(LOAD_TEST) ) {
        return error(QString("error loading %1").arg(LOAD_TEST));
    }
    Regola *regola = app.mainWindow()->getRegola();
    int index = 0;
    foreach (Element *el, *regola->getChildItems()) {
        switch( index ) {
         case 0:
            if(el->getType() != Element::ET_PROCESSING_INSTRUCTION) {
                return error("Pos 0: not P.I");
            }
            break;
        case 1:
           if(el->getType() != Element::ET_COMMENT) {
               return error("Pos 1: not comment");
           }
           if( el->getComment().trimmed() != "comment 0" ) {
               return error("Pos 1: wrong comment");
           }
           break;
        case 2:
            if(el->getType() != Element::ET_COMMENT) {
                return error("Pos 2: not comment");
            }
            if( el->getComment().trimmed() != "comment 2" ) {
                return error("Pos 2: wrong comment");
            }
            break;
        }
        index ++ ;
    }
    return true;
}

bool TestDocType::testEdit()
{
    _testName = "testEdit";
    if(!testEditEmptyFilled()) {
        return false;
    }
    if(!testEditFilledEmpty()) {
        return false;
    }
    if(!testEditChange()) {
        return false;
    }
    if(!testEditReadOnly()) {
        return false;
    }
    if(!testEditSameNoEdit()) {
        return false;
    }
    if(!testEditCancel()) {
        return false;
    }
    return true ;
}

bool TestDocType::goEdit(const QString &fileNameIn, const QString &fileNameOut,
                         const bool setReadOnly, const bool isEdit,
                         const QString &newValue, const bool isCancel )
{
    App app;
    if(!app.init() ) {
        return error("init app");
    }
    if(!app.mainWindow()->loadFile(fileNameIn)) {
        return error(QString("error loading %1").arg(fileNameIn));
    }
    Regola *regola = app.mainWindow()->getRegola();
    if(setReadOnly){
        app.mainWindow()->getEditor()->setReadOnly(true);
    }
    InfoDialog dlg(app.mainWindow(), regola, setReadOnly);
    if(isCancel) {
        dlg.accept();
    } else {
        dlg.reject();
    }
    if(isEdit) {
        QPlainTextEdit *e = dlg.findChild<QPlainTextEdit*>("dtdEdit");
        e->setFocus();
        if(e->isEnabled()) {
            e->setPlainText(newValue);
        }
    }

    if(!compareDocuments(fileNameOut, regola) ) {
        return error(QString("comparing docs redo 1: %1").arg(errorString()));
    }
    //--
    regola->undo();
    if(!compareDocuments(fileNameIn, regola) ) {
        return error(QString("comparing docs undo: %1").arg(errorString()));
    }
    //--

    regola->redo();
    if(!compareDocuments(fileNameOut, regola) ) {
        return error(QString("comparing docs redo 2: %1").arg(errorString()));
    }
    //--
    return true;
}

#define EDIT_FILE_IN_EMPTY  TEST_BASE   "in.empty.xml"
#define EDIT_FILE_OUT_EMPTY  TEST_BASE   "out.empty.xml"
#define EDIT_FILE_IN_FILLED TEST_BASE   "in.filled.xml"
#define EDIT_FILE_OUT_FILLED    TEST_BASE   "out.filled.xml"
#define EDIT_FILE_IN_CHANGE TEST_BASE   "in.change.xml"
#define EDIT_FILE_OUT_CHANGE    TEST_BASE   "out.change.xml"

bool TestDocType::testEditEmptyFilled()
{
    _testName = "testEditEmptyFilled" ;
    if(!goEdit(EDIT_FILE_IN_EMPTY, EDIT_FILE_OUT_EMPTY, false, true, "<!DOCTYPE b>") ) {
        return false;
    }
    return true ;
}

bool TestDocType::testEditFilledEmpty()
{
    _testName = "testEditFilledEmpty";
    if(!goEdit(EDIT_FILE_IN_FILLED, EDIT_FILE_OUT_FILLED, false, true, "") ) {
        return false;
    }
    return true ;
}

bool TestDocType::testEditChange()
{
    _testName = "testEditChange";
    if(!goEdit(EDIT_FILE_IN_CHANGE, EDIT_FILE_OUT_CHANGE, false, true, "<!DOCTYPE c>") ) {
        return false;
    }
    return true ;
}

bool TestDocType::testEditReadOnly()
{
    _testName = "testEditReadOnly";
    if(!goEdit(EDIT_FILE_IN_CHANGE, EDIT_FILE_IN_CHANGE, true, true, "<! DOCTYPE b]>") ) {
        return false;
    }
    return true ;
}

bool TestDocType::testEditSameNoEdit()
{
    _testName = "testEditSameNoEdit";
    if(!goEdit(EDIT_FILE_IN_CHANGE, EDIT_FILE_IN_CHANGE, true, true, "<! DOCTYPE b]>") ) {
        return false;
    }
    return true ;
}

bool TestDocType::testEditCancel()
{
    _testName = "testEditCancel";
    if(!goEdit(EDIT_FILE_IN_FILLED, EDIT_FILE_IN_FILLED, false, true, "<! DOCTYPE XXXXX>", true) ) {
        return false;
    }
    return true ;
}

bool TestDocType::checkUnit(const QString &theDTD, const QString &expectedDtd, const QString &expectedDocType)
{
    Regola regola;
    regola.setDtd(theDTD);
    if( regola.dtd() != expectedDtd ) {
        return error(QString("DTS not set:\n**found\n%1\n** expected:\n%2").arg(regola.dtd()).arg(expectedDtd));
    }
    if( regola.docType() != expectedDocType ) {
        return error(QString("DocType not set:\n**found\n%1\n** expected:\n%2").arg(regola.docType()).arg(expectedDocType));
    }
    return true;
}

bool TestDocType::testUnit()
{
    _testName = "testUnit";

    if(!checkUnit("", "", "") ) {
        return false;
    }
    {
        const QString dtd = "<!DOCTYPE abc>";
        if(!checkUnit(dtd, dtd, "abc") ) {
            return false;
        }
    }
    {
        const QString dtd = "<!DOCTYPE abc >";
        if(!checkUnit(dtd, dtd, "abc") ) {
            return false;
        }
    }
    {
        const QString dtd = "<!DOCTYPE abc externalID>";
        if(!checkUnit(dtd, dtd, "abc") ) {
            return false;
        }
    }
    {
        const QString dtd = "<!DOCTYPE abc externalID [int subset]>";
        if(!checkUnit(dtd, dtd, "abc") ) {
            return false;
        }
    }
    {
        const QString dtd = "<!DOCTYPE abc[int subset]>";
        if(!checkUnit(dtd, dtd, "abc") ) {
            return false;
        }
    }
    {
        const QString dtd = "<!DOCTYPE a>";
        if(!checkUnit(dtd, dtd, "a") ) {
            return false;
        }
    }
    {
        const QString dtd = "\n<!DOCTYPE a> \n";
        const QString dtdExp = "<!DOCTYPE a>";
        if(!checkUnit(dtd, dtdExp, "a") ) {
            return false;
        }
    }
    {
        const QString dtd = "<!DOCTYPE xy [\n<!ELEMENT xay (#PCDATA)>\n]>";
        if(!checkUnit(dtd, dtd, "xy") ) {
            return false;
        }
    }
    return true ;
}

bool TestDocType::testDTD()
{
    _testName = "testDTD" ;
    const QString fileNameIn = LOAD_TEST_ATTR;
    App app;
    if(!app.init() ) {
        return error("init app");
    }
    if(!app.mainWindow()->loadFile(fileNameIn)) {
        return error(QString("error loading %1").arg(fileNameIn));
    }
    Regola *regola = app.mainWindow()->getRegola();
    const QString expected = QString("<!DOCTYPE root [\n"
"  <!ELEMENT root EMPTY>\n"
"  <!ATTLIST root a CDATA #REQUIRED>\n"
"  <!ATTLIST root b CDATA #REQUIRED>\n"
"]>").replace("\r\n", "\n");
    QString found = regola->dtd().replace("\r\n", "\n") ;

    if( found != expected ) {
        return error(QString("dtd found:\n'%1'\nexpecting:\n'%2'\n").arg(found).arg(expected));
    }
    return true;
}
