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
 * Insert a new nillable with ns. If the ns is found, it is used, else
 * it is inserted using a naming nsi nsi0, nsi1, etc...
 * base file: source
 * final file: source with nillable
 *
 *
 * Test table
 * NSI: namespace instance
 * ATTR: nillable attribute
 * CONT: content of the element
 * S: SAME
 * D: DIFFERS
 * E, NE: Exists or not
 * N: unchanged
 * Y: inserted
 * ANS: User answered
 *
 * some test uses more than one namespace to test ns handling
 # NSI ATTR CONT ANS RES (NS+ATTR)
 1 NE   NE   E   -   N
 2 E    NE   E   -   N
 3 E    D    E   -   N
 4 NE   NE   NE  -   Y
 5 NE   S    NE  -   Y
 6 NE   D    NE  -   Y
 7 S    NE   NE  -   Y
 8 S    S    NE  -   Y
 9 S    D    NE  -   Y
10 S*   D    NE  -   Y same ns, but different prefix, current prefix is used
11 D    NE   NE  -   Y using alternate NS
12 D    D    NE  -   Y using alternate NS

NS tests:
1- declare same ns in parent -> duplicated in child
2- declare more than one ns in the element -> all preserved

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
---------------------------------
 10    A   B   A    B    NS, NA
 11    SP  B   B    SP   NNS, NA
 12    A   SP  A    SP   NNS, NA
*/

/* VECCHI TEST

EPar: exists in parent
OTH: other attributes use the namespace
OCH: other children use the namespace
OCHA: other children attrib use the namespace
 # NSI ATTR OTH OCH OCHA RES
 1 NE   NE   -   -   -   No change
 2 E    NE   -   -   -   No change
 .1EPar NE   -   -   -   No change
 3 E    E    E   -   -   AR
 4 E    E    -   E   -   AR
 5 E    E    -   -   E   AR
 6 E    E    E   -   E   AR
 7 E    E    E   E   -   AR
 8 E    E    E   E   E   AR
 9 E    E    -   -   -   AR NR
10 EPar E    -   -   -   AR

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
    if(!testRemove3()) {
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
    return error("nyi");
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
    /*if(!testRemove6()) {
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
    }*/
    return true;
}

bool TestNillable::doRemove(App * appData)
{
    appData->mainWindow()->getEditor()->removeNilAttribute();
    return true ;
}

bool TestNillable::doInsert(App * /*appData*/)
{
    return error("nyi");
}

#define START_FILE_REMOVE1      "../test/data/xsd/nillable/remove1.xml"
#define START_FILE_REMOVE2      "../test/data/xsd/nillable/remove2.xml"
#define START_FILE_REMOVE3      "../test/data/xsd/nillable/remove3.xml"
#define END_FILE_REMOVE3      "../test/data/xsd/nillable/end_remove3.xml"
#define START_FILE_REMOVE4      "../test/data/xsd/nillable/remove4.xml"
#define END_FILE_REMOVE4      "../test/data/xsd/nillable/end_remove4.xml"
#define START_FILE_REMOVE5      "../test/data/xsd/nillable/remove5.xml"
#define END_FILE_REMOVE5      "../test/data/xsd/nillable/end_remove5.xml"


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
