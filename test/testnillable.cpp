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


#include "testnillable.h"
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

TestNillable::TestNillable()
{
}

TestNillable::~TestNillable()
{
}

bool TestNillable::testFast()
{
    _testName = "testFast" ;
    if(!testInsert4()) {
        return false;
    }
    return true;
}


bool TestNillable::testUnit()
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

bool TestNillable::testInsert()
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

bool TestNillable::testRemove()
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

bool TestNillable::doRemove(App * appData)
{
    appData->mainWindow()->getEditor()->removeNilAttribute();
    return true ;
}

bool TestNillable::doInsert(App * appData)
{
    appData->mainWindow()->getEditor()->insertNilAttribute();
    return true ;
}

#define START_FILE_REMOVE1      "../test/data/xsd/nillable/remove1.xml"
#define START_FILE_REMOVE2      "../test/data/xsd/nillable/remove2.xml"
#define START_FILE_REMOVE3      "../test/data/xsd/nillable/remove3.xml"
#define END_FILE_REMOVE3      "../test/data/xsd/nillable/end_remove3.xml"
#define START_FILE_REMOVE4      "../test/data/xsd/nillable/remove4.xml"
#define END_FILE_REMOVE4      "../test/data/xsd/nillable/end_remove4.xml"
#define START_FILE_REMOVE5      "../test/data/xsd/nillable/remove5.xml"
#define END_FILE_REMOVE5      "../test/data/xsd/nillable/end_remove5.xml"
#define START_FILE_REMOVE6      "../test/data/xsd/nillable/remove6.xml"
#define END_FILE_REMOVE6      "../test/data/xsd/nillable/end_remove6.xml"
#define START_FILE_REMOVE7      "../test/data/xsd/nillable/remove7.xml"
#define END_FILE_REMOVE7      "../test/data/xsd/nillable/end_remove7.xml"
#define START_FILE_REMOVE8      "../test/data/xsd/nillable/remove8.xml"
#define END_FILE_REMOVE8      "../test/data/xsd/nillable/end_remove8.xml"
#define START_FILE_REMOVE9      "../test/data/xsd/nillable/remove9.xml"
#define END_FILE_REMOVE9      "../test/data/xsd/nillable/end_remove9.xml"
#define START_FILE_REMOVE10      "../test/data/xsd/nillable/remove10.xml"
#define END_FILE_REMOVE10      "../test/data/xsd/nillable/end_remove10.xml"
#define START_FILE_REMOVE11      "../test/data/xsd/nillable/remove11.xml"
#define END_FILE_REMOVE11      "../test/data/xsd/nillable/end_remove11.xml"
#define START_FILE_REMOVE12      "../test/data/xsd/nillable/remove12.xml"
#define END_FILE_REMOVE12      "../test/data/xsd/nillable/end_remove12.xml"


bool TestNillable::testRemove1()
{
    _testName = "testRemove1" ;
    if(!testSkeleton(START_FILE_REMOVE1, START_FILE_REMOVE1, &TestNillable::doRemove ) ) {
        return false ;
    }
    return true;
}

bool TestNillable::testRemove2()
{
    _testName = "testRemove2" ;
    if(!testSkeleton(START_FILE_REMOVE2, START_FILE_REMOVE2, &TestNillable::doRemove ) ) {
        return false ;
    }
    return true;
}

bool TestNillable::testRemove3()
{
    _testName = "testRemove3" ;
    if(!testSkeleton(START_FILE_REMOVE3, END_FILE_REMOVE3, &TestNillable::doRemove ) ) {
        return false ;
    }
    return true;
}

bool TestNillable::testRemove4()
{
    _testName = "testRemove4" ;
    if(!testSkeleton(START_FILE_REMOVE4, END_FILE_REMOVE4, &TestNillable::doRemove ) ) {
        return false ;
    }
    return true;
}

bool TestNillable::testRemove5()
{
    _testName = "testRemove5" ;
    if(!testSkeleton(START_FILE_REMOVE5, END_FILE_REMOVE5, &TestNillable::doRemove ) ) {
        return false ;
    }
    return true;
}

bool TestNillable::testRemove6()
{
    _testName = "testRemove6" ;
    if(!testSkeleton(START_FILE_REMOVE6, END_FILE_REMOVE6, &TestNillable::doRemove ) ) {
        return false ;
    }
    return true;
}

bool TestNillable::testRemove7()
{
    _testName = "testRemove7" ;
    if(!testSkeleton(START_FILE_REMOVE7, END_FILE_REMOVE7, &TestNillable::doRemove ) ) {
        return false ;
    }
    return true;
}

bool TestNillable::testRemove8()
{
    _testName = "testRemove8" ;
    if(!testSkeleton(START_FILE_REMOVE8, END_FILE_REMOVE8, &TestNillable::doRemove ) ) {
        return false ;
    }
    return true;
}

bool TestNillable::testRemove9()
{
    _testName = "testRemove9" ;
    if(!testSkeleton(START_FILE_REMOVE9, END_FILE_REMOVE9, &TestNillable::doRemove ) ) {
        return false ;
    }
    return true;
}

bool TestNillable::testRemove10()
{
    _testName = "testRemove10" ;
    if(!testSkeleton(START_FILE_REMOVE10, END_FILE_REMOVE10, &TestNillable::doRemove ) ) {
        return false ;
    }
    return true;
}

bool TestNillable::testRemove11()
{
    _testName = "testRemove11" ;
    if(!testSkeleton(START_FILE_REMOVE11, END_FILE_REMOVE11, &TestNillable::doRemove ) ) {
        return false ;
    }
    return true;
}

bool TestNillable::testRemove12()
{
    _testName = "testRemove12" ;
    if(!testSkeleton(START_FILE_REMOVE12, END_FILE_REMOVE12, &TestNillable::doRemove ) ) {
        return false ;
    }
    return true;
}


bool TestNillable::cfr(Regola *regola, const QString &step, const QString &fileResult)
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


bool TestNillable::testSkeleton(const QString &fileStart, const QString &fileResult,
                                         bool (TestNillable::*functPtr)(App *appData) )
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


#define START_FILE_INSERT_1      "../test/data/xsd/nillable/insert1.xml"
#define END_FILE_INSERT_1      "../test/data/xsd/nillable/end_insert1.xml"
#define START_FILE_INSERT_2      "../test/data/xsd/nillable/insert2.xml"
#define END_FILE_INSERT_2      "../test/data/xsd/nillable/end_insert2.xml"
#define START_FILE_INSERT_3      "../test/data/xsd/nillable/insert3.xml"
#define END_FILE_INSERT_3      "../test/data/xsd/nillable/end_insert3.xml"
#define START_FILE_INSERT_4      "../test/data/xsd/nillable/insert4.xml"
#define END_FILE_INSERT_4      "../test/data/xsd/nillable/end_insert4.xml"
#define START_FILE_INSERT_5      "../test/data/xsd/nillable/insert5.xml"
#define END_FILE_INSERT_5      "../test/data/xsd/nillable/end_insert5.xml"
#define START_FILE_INSERT_6      "../test/data/xsd/nillable/insert6.xml"
#define END_FILE_INSERT_6      "../test/data/xsd/nillable/end_insert6.xml"
#define START_FILE_INSERT_7      "../test/data/xsd/nillable/insert7.xml"
#define END_FILE_INSERT_7      "../test/data/xsd/nillable/end_insert7.xml"
#define START_FILE_INSERT_8      "../test/data/xsd/nillable/insert8.xml"
#define END_FILE_INSERT_8      "../test/data/xsd/nillable/end_insert8.xml"
#define START_FILE_INSERT_9      "../test/data/xsd/nillable/insert9.xml"
#define END_FILE_INSERT_9      "../test/data/xsd/nillable/end_insert9.xml"
#define START_FILE_INSERT_10      "../test/data/xsd/nillable/insert10.xml"
#define END_FILE_INSERT_10      "../test/data/xsd/nillable/end_insert10.xml"

bool TestNillable::testInsert1()
{
    _testName = "testInsert1" ;
    if(!testSkeleton(START_FILE_INSERT_1, END_FILE_INSERT_1, &TestNillable::doInsert ) ) {
        return false ;
    }
    return true;
}

bool TestNillable::testInsert2()
{
    _testName = "testInsert2" ;
    if(!testSkeleton(START_FILE_INSERT_2, END_FILE_INSERT_2, &TestNillable::doInsert ) ) {
        return false ;
    }
    return true;
}

bool TestNillable::testInsert3()
{
    _testName = "testInsert3" ;
    if(!testSkeleton(START_FILE_INSERT_3, END_FILE_INSERT_3, &TestNillable::doInsert ) ) {
        return false ;
    }
    return true;
}

bool TestNillable::testInsert4()
{
    _testName = "testInsert4" ;
    if(!testSkeleton(START_FILE_INSERT_4, END_FILE_INSERT_4, &TestNillable::doInsert ) ) {
        return false ;
    }
    return true;
}
/*
bool TestNillable::testInsert5()
{
    _testName = "testInsert5" ;
    if(!testSkeleton(START_FILE_INSERT_5, END_FILE_INSERT_5, &TestNillable::doInsert ) ) {
        return false ;
    }
    return true;
}
*/
bool TestNillable::testInsert6()
{
    _testName = "testInsert6" ;
    if(!testSkeleton(START_FILE_INSERT_6, END_FILE_INSERT_6, &TestNillable::doInsert ) ) {
        return false ;
    }
    return true;
}

bool TestNillable::testInsert7()
{
    _testName = "testInsert7" ;
    if(!testSkeleton(START_FILE_INSERT_7, END_FILE_INSERT_7, &TestNillable::doInsert ) ) {
        return false ;
    }
    return true;
}

bool TestNillable::testInsert8()
{
    _testName = "testInsert8" ;
    if(!testSkeleton(START_FILE_INSERT_8, END_FILE_INSERT_8, &TestNillable::doInsert ) ) {
        return false ;
    }
    return true;
}

bool TestNillable::testInsert9()
{
    _testName = "testInsert9" ;
    if(!testSkeleton(START_FILE_INSERT_9, END_FILE_INSERT_9, &TestNillable::doInsert ) ) {
        return false ;
    }
    return true;
}

bool TestNillable::testInsert10()
{
    _testName = "testInsert10" ;
    if(!testSkeleton(START_FILE_INSERT_10, END_FILE_INSERT_10, &TestNillable::doInsert ) ) {
        return false ;
    }
    return true;
}

