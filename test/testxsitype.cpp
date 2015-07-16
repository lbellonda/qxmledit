/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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


#include "testxsitype.h"
#include "app.h"
#include "comparexml.h"

/* schema of the tests:
 *
 * Insert
 *
 * If the namespace xsi is not present, the namespace is inserted and the attribute created
 * if the attribute is present
 * - remove any "nil" attribute
 * - insert nil attribute
 *
 *NSP: Name space parent
 *NS: ns element
 *ATTR: attribute
 *
 *namespace: DEF:default X: namespace NE: not exists
 *value: E: exists, NE: not exists, O: other value
 * #  NSP  NS  ATTR RIS
 * 1  NE   NE  NE   NS, NS:A
 * 2  X    NE  NE   Nns, x:A
 * 3  DEF  NE  NE   Nns, DEF:A
 * 4  NE   X   E    X, E
 * 5  X    X   E    X, E non legale
 * 6  DEF  X   E    X, E
 * 7  NE   DEF O    DEF, E
 * 8  X    DEF O
 * 9  DEF  DEF O
 *
 * double attribute definition, change both
 *10  A    B    A1:0, A2:O SU QUESTO NON SONO CONVINTO


Removal:

General usage: Identify the namespace looking up to the root.
If found, remove the attribute.
If the attibute is removed, if the namespace is declared at the element level and none of the children is using it in elements
and attributes, remove it.

NSP: ns parent
NST: ns this
NSC: ns children
  #   NSP NST NSC ATT RES
  1 .  N   N   N   N   same
  2 .  Y   Y   N   N   same
  3 .  N   Y   N   Y   NNS, NA
  4 .  N   Y   Y   Y   NS, NA

  #   NSP NST NSC ATT1 ATT2 RES
  5 .  N   A,B  N   A   B   NNS, NA, NB

  #   NSP NST ATT1 ATT2 RES
  6    A   B   A    N    NS, NA
  7    SP  B   B    N    NNS, NA
  8    A   SP  A    N    NS, NA
  9    SP  N   SP   N    NA
---------------------------------
 10    A   B   A    B    NS, NA
 11    SP  B   B    SP   NNS, NA
 12    A   SP  A    SP   NNS, NA
*/

TestXSIType::TestXSIType()
{
}

TestXSIType::~TestXSIType()
{
}

bool TestXSIType::testFast()
{
    _testName = "testFast" ;
    if(!testRemove1()) {
        return false;
    }
    return true;
}


bool TestXSIType::testUnit()
{
    _testName = "testUnit" ;
    if(!testRemove()) {
        return false;
    }
    if(!testInsert()) {
        return false;
    }
    return true;
}

bool TestXSIType::testInsert()
{
    _testName = "testInsert" ;

    if(!testInsert1()) {
        return false;
    }
    if(!testInsert2()) {
        return false;
    }
    if(!testInsert3()) {
        return false;
    }
    if(!testInsert4()) {
        return false;
    }
    /*if(!testInsert5()) {non legal
        return false;
    }*/
    if(!testInsert6()) {
        return false;
    }
    if(!testInsert7()) {
        return false;
    }
    if(!testInsert8()) {
        return false;
    }
    if(!testInsert9()) {
        return false;
    }
    if(!testInsert10()) {
        return false;
    }
    return true;
}

bool TestXSIType::testRemove()
{
    _testName = "testRemove" ;
    if(!testRemove1()) {
        return false;
    }
    if(!testRemove2()) {
        return false;
    }
    if(!testRemove3()) {
        return false;
    }
    if(!testRemove4()) {
        return false;
    }
    if(!testRemove5()) {
        return false;
    }
    if(!testRemove6()) {
        return false;
    }
    if(!testRemove7()) {
        return false;
    }
    if(!testRemove8()) {
        return false;
    }
    if(!testRemove9()) {
        return false;
    }
    if(!testRemove10()) {
        return false;
    }
    if(!testRemove11()) {
        return false;
    }
    if(!testRemove12()) {
        return false;
    }
    return true;
}

bool TestXSIType::doRemove(App * appData)
{
    appData->mainWindow()->getEditor()->removeXSITypeAttribute();
    return true ;
}

bool TestXSIType::doInsert(App * appData)
{
    appData->mainWindow()->getEditor()->insertXSITypeAttribute("abc");
    return true ;
}

#define START_FILE_REMOVE1      "../test/data/xsd/xsitype/remove1.xml"
#define START_FILE_REMOVE2      "../test/data/xsd/xsitype/remove2.xml"
#define START_FILE_REMOVE3      "../test/data/xsd/xsitype/remove3.xml"
#define END_FILE_REMOVE3      "../test/data/xsd/xsitype/end_remove3.xml"
#define START_FILE_REMOVE4      "../test/data/xsd/xsitype/remove4.xml"
#define END_FILE_REMOVE4      "../test/data/xsd/xsitype/end_remove4.xml"
#define START_FILE_REMOVE5      "../test/data/xsd/xsitype/remove5.xml"
#define END_FILE_REMOVE5      "../test/data/xsd/xsitype/end_remove5.xml"
#define START_FILE_REMOVE6      "../test/data/xsd/xsitype/remove6.xml"
#define END_FILE_REMOVE6      "../test/data/xsd/xsitype/end_remove6.xml"
#define START_FILE_REMOVE7      "../test/data/xsd/xsitype/remove7.xml"
#define END_FILE_REMOVE7      "../test/data/xsd/xsitype/end_remove7.xml"
#define START_FILE_REMOVE8      "../test/data/xsd/xsitype/remove8.xml"
#define END_FILE_REMOVE8      "../test/data/xsd/xsitype/end_remove8.xml"
#define START_FILE_REMOVE9      "../test/data/xsd/xsitype/remove9.xml"
#define END_FILE_REMOVE9      "../test/data/xsd/xsitype/end_remove9.xml"
#define START_FILE_REMOVE10      "../test/data/xsd/xsitype/remove10.xml"
#define END_FILE_REMOVE10      "../test/data/xsd/xsitype/end_remove10.xml"
#define START_FILE_REMOVE11      "../test/data/xsd/xsitype/remove11.xml"
#define END_FILE_REMOVE11      "../test/data/xsd/xsitype/end_remove11.xml"
#define START_FILE_REMOVE12      "../test/data/xsd/xsitype/remove12.xml"
#define END_FILE_REMOVE12      "../test/data/xsd/xsitype/end_remove12.xml"


bool TestXSIType::testRemove1()
{
    _testName = "testRemove1" ;
    if(!testSkeleton(START_FILE_REMOVE1, START_FILE_REMOVE1, &TestXSIType::doRemove ) ) {
        return false ;
    }
    return true;
}

bool TestXSIType::testRemove2()
{
    _testName = "testRemove2" ;
    if(!testSkeleton(START_FILE_REMOVE2, START_FILE_REMOVE2, &TestXSIType::doRemove ) ) {
        return false ;
    }
    return true;
}

bool TestXSIType::testRemove3()
{
    _testName = "testRemove3" ;
    if(!testSkeleton(START_FILE_REMOVE3, END_FILE_REMOVE3, &TestXSIType::doRemove ) ) {
        return false ;
    }
    return true;
}

bool TestXSIType::testRemove4()
{
    _testName = "testRemove4" ;
    if(!testSkeleton(START_FILE_REMOVE4, END_FILE_REMOVE4, &TestXSIType::doRemove ) ) {
        return false ;
    }
    return true;
}

bool TestXSIType::testRemove5()
{
    _testName = "testRemove5" ;
    if(!testSkeleton(START_FILE_REMOVE5, END_FILE_REMOVE5, &TestXSIType::doRemove ) ) {
        return false ;
    }
    return true;
}

bool TestXSIType::testRemove6()
{
    _testName = "testRemove6" ;
    if(!testSkeleton(START_FILE_REMOVE6, END_FILE_REMOVE6, &TestXSIType::doRemove ) ) {
        return false ;
    }
    return true;
}

bool TestXSIType::testRemove7()
{
    _testName = "testRemove7" ;
    if(!testSkeleton(START_FILE_REMOVE7, END_FILE_REMOVE7, &TestXSIType::doRemove ) ) {
        return false ;
    }
    return true;
}

bool TestXSIType::testRemove8()
{
    _testName = "testRemove8" ;
    if(!testSkeleton(START_FILE_REMOVE8, END_FILE_REMOVE8, &TestXSIType::doRemove ) ) {
        return false ;
    }
    return true;
}

bool TestXSIType::testRemove9()
{
    _testName = "testRemove9" ;
    if(!testSkeleton(START_FILE_REMOVE9, END_FILE_REMOVE9, &TestXSIType::doRemove ) ) {
        return false ;
    }
    return true;
}

bool TestXSIType::testRemove10()
{
    _testName = "testRemove10" ;
    if(!testSkeleton(START_FILE_REMOVE10, END_FILE_REMOVE10, &TestXSIType::doRemove ) ) {
        return false ;
    }
    return true;
}

bool TestXSIType::testRemove11()
{
    _testName = "testRemove11" ;
    if(!testSkeleton(START_FILE_REMOVE11, END_FILE_REMOVE11, &TestXSIType::doRemove ) ) {
        return false ;
    }
    return true;
}

bool TestXSIType::testRemove12()
{
    _testName = "testRemove12" ;
    if(!testSkeleton(START_FILE_REMOVE12, END_FILE_REMOVE12, &TestXSIType::doRemove ) ) {
        return false ;
    }
    return true;
}


bool TestXSIType::cfr(Regola *regola, const QString &step, const QString &fileResult)
{
    QByteArray resultData = regola->writeMemory();
    QDomDocument document1;
    QDomDocument document2;
    CompareXML compare;
    if(!compare.loadFileIntoDocument(fileResult, document1)) {
        return error(QString("step: %1, load file result %2").arg(step).arg(fileResult));
    }
    QBuffer outputData(&resultData);
    if(!compare.loadFileIntoDocument(&outputData, document2)) {
        return error(QString("step %1 load modified data").arg(step));
    }
    bool result = compare.compareDomDocuments(document1, document2);
    if( !result ) {
        compare.dumpErrorCause();
        return error(QString("Step: %1 comparing file with doc: %2").arg(step).arg(compare.errorString()));
    }
    return true ;
}


bool TestXSIType::testSkeleton(const QString &fileStart, const QString &fileResult,
                                         bool (TestXSIType::*functPtr)(App *appData) )
{
    QList<int> sel;
    sel << 1 << 0 << 0;
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(fileStart) ) {
        return error(QString("unable to load input file: '%1' ").arg(fileStart));
    }
    Regola *regola = app.mainWindow()->getRegola();
    Element *selectedElement = app.mainWindow()->getRegola()->findElementByArray(sel);
    if(NULL == selectedElement) {
        return error("no element selected");
    }
    app.mainWindow()->getEditor()->setCurrentItem(selectedElement);
    if(!(this->*functPtr)(&app)) {
        return false;
    }
    if(!cfr(regola, "operation", fileResult)){
        return false;
    }
    regola->undo();
    if(!cfr(regola, "undo", fileStart)){
        return false;
    }
    regola->redo();
    if(!cfr(regola, "redo", fileResult)){
        return false;
    }
    return true;
}


#define START_FILE_INSERT_1      "../test/data/xsd/xsitype/insert1.xml"
#define END_FILE_INSERT_1      "../test/data/xsd/xsitype/end_insert1.xml"
#define START_FILE_INSERT_2      "../test/data/xsd/xsitype/insert2.xml"
#define END_FILE_INSERT_2      "../test/data/xsd/xsitype/end_insert2.xml"
#define START_FILE_INSERT_3      "../test/data/xsd/xsitype/insert3.xml"
#define END_FILE_INSERT_3      "../test/data/xsd/xsitype/end_insert3.xml"
#define START_FILE_INSERT_4      "../test/data/xsd/xsitype/insert4.xml"
#define END_FILE_INSERT_4      "../test/data/xsd/xsitype/end_insert4.xml"
#define START_FILE_INSERT_5      "../test/data/xsd/xsitype/insert5.xml"
#define END_FILE_INSERT_5      "../test/data/xsd/xsitype/end_insert5.xml"
#define START_FILE_INSERT_6      "../test/data/xsd/xsitype/insert6.xml"
#define END_FILE_INSERT_6      "../test/data/xsd/xsitype/end_insert6.xml"
#define START_FILE_INSERT_7      "../test/data/xsd/xsitype/insert7.xml"
#define END_FILE_INSERT_7      "../test/data/xsd/xsitype/end_insert7.xml"
#define START_FILE_INSERT_8      "../test/data/xsd/xsitype/insert8.xml"
#define END_FILE_INSERT_8      "../test/data/xsd/xsitype/end_insert8.xml"
#define START_FILE_INSERT_9      "../test/data/xsd/xsitype/insert9.xml"
#define END_FILE_INSERT_9      "../test/data/xsd/xsitype/end_insert9.xml"
#define START_FILE_INSERT_10      "../test/data/xsd/xsitype/insert10.xml"
#define END_FILE_INSERT_10      "../test/data/xsd/xsitype/end_insert10.xml"

bool TestXSIType::testInsert1()
{
    _testName = "testInsert1" ;
    if(!testSkeleton(START_FILE_INSERT_1, END_FILE_INSERT_1, &TestXSIType::doInsert ) ) {
        return false ;
    }
    return true;
}

bool TestXSIType::testInsert2()
{
    _testName = "testInsert2" ;
    if(!testSkeleton(START_FILE_INSERT_2, END_FILE_INSERT_2, &TestXSIType::doInsert ) ) {
        return false ;
    }
    return true;
}

bool TestXSIType::testInsert3()
{
    _testName = "testInsert3" ;
    if(!testSkeleton(START_FILE_INSERT_3, END_FILE_INSERT_3, &TestXSIType::doInsert ) ) {
        return false ;
    }
    return true;
}

bool TestXSIType::testInsert4()
{
    _testName = "testInsert4" ;
    if(!testSkeleton(START_FILE_INSERT_4, END_FILE_INSERT_4, &TestXSIType::doInsert ) ) {
        return false ;
    }
    return true;
}
/*
bool TestXSIType::testInsert5()
{
    _testName = "testInsert5" ;
    if(!testSkeleton(START_FILE_INSERT_5, END_FILE_INSERT_5, &TestXSIType::doInsert ) ) {
        return false ;
    }
    return true;
}
*/
bool TestXSIType::testInsert6()
{
    _testName = "testInsert6" ;
    if(!testSkeleton(START_FILE_INSERT_6, END_FILE_INSERT_6, &TestXSIType::doInsert ) ) {
        return false ;
    }
    return true;
}

bool TestXSIType::testInsert7()
{
    _testName = "testInsert7" ;
    if(!testSkeleton(START_FILE_INSERT_7, END_FILE_INSERT_7, &TestXSIType::doInsert ) ) {
        return false ;
    }
    return true;
}

bool TestXSIType::testInsert8()
{
    _testName = "testInsert8" ;
    if(!testSkeleton(START_FILE_INSERT_8, END_FILE_INSERT_8, &TestXSIType::doInsert ) ) {
        return false ;
    }
    return true;
}

bool TestXSIType::testInsert9()
{
    _testName = "testInsert9" ;
    if(!testSkeleton(START_FILE_INSERT_9, END_FILE_INSERT_9, &TestXSIType::doInsert ) ) {
        return false ;
    }
    return true;
}

bool TestXSIType::testInsert10()
{
    _testName = "testInsert10" ;
    if(!testSkeleton(START_FILE_INSERT_10, END_FILE_INSERT_10, &TestXSIType::doInsert ) ) {
        return false ;
    }
    return true;
}

