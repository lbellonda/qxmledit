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


#include "testinsertxsdreference.h"
#include "modules/namespace/namespacereferenceentry.h"
#include "app.h"
#include "comparexml.h"
#include "modules/xsd/schemareferencesdialog.h"

#define BASE_PATH "../test/data/xsd/xsdreference/"

#define TEST_INS1_START   BASE_PATH "test1Start.xml"
#define TEST_INS1_END    BASE_PATH "test1End.xml"
#define TEST_INS1_1_START   BASE_PATH "test1.1Start.xml"
#define TEST_INS1_1_END    BASE_PATH "test1.1End.xml"
#define TEST_INS1_2_START   BASE_PATH "test1.2Start.xml"
#define TEST_INS1_2_END    BASE_PATH "test1.2End.xml"
#define TEST_INS2_END    BASE_PATH "test2End.xml"
#define TEST_INS3_END    BASE_PATH "test3End.xml"
#define TEST_INS4_END    BASE_PATH "test4End.xml"
#define TEST_INS4_1_START   BASE_PATH "test4.1Start.xml"
#define TEST_INS4_1_END    BASE_PATH "test4.1End.xml"
#define TEST_INS4_2_START   BASE_PATH "test4.2Start.xml"
#define TEST_INS4_2_END    BASE_PATH "test4.2End.xml"
#define TEST_INS4_3_START   BASE_PATH "test4.3Start.xml"
#define TEST_INS4_3_END    BASE_PATH "test4.3End.xml"

#define TEST_INS7_START   BASE_PATH "test7Start.xml"
#define TEST_INS7_END    BASE_PATH "test7End.xml"
#define TEST_INS8_START   BASE_PATH "test8Start.xml"
#define TEST_INS8_END    BASE_PATH "test8End.xml"
#define TEST_INS9_START   BASE_PATH "test9Start.xml"
#define TEST_INS9_END    BASE_PATH "test9End.xml"

#define FILE_ENTRY_1 BASE_PATH "entry1.xml"
#define FILE_ENTRY_2 BASE_PATH "entry2.xml"

TestInsertXsdReference::TestInsertXsdReference()
{
}

TestInsertXsdReference::~TestInsertXsdReference()
{
}

bool TestInsertXsdReference::testFast()
{
    return test1_1NoData();
}

bool TestInsertXsdReference::testInsertXsdReference()
{
    if(!test1NoData()) {
        return false;
    }
    if(!test1_1NoData()) {
        return false;
    }
    if(!test1_2NoData()) {
        return false;
    }
    //-
    if(!test2()) {
        return false;
    }
    if(!test3()) {
        return false;
    }
    if(!test4()) {
        return false;
    }
    if(!test4_1()) {
        return false;
    }
    if(!test4_2()) {
        return false;
    }
    if(!test4_3()) {
        return false;
    }
    if(!test7()) {
        return false;
    }
    if(!test8()) {
        return false;
    }
    if(!test9()) {
        return false;
    }
    return true;
}


/****************************************************
 * tests operation:
 *  1-no data -> no output
 *  1.1- no data ->existing nons ->removing
 *  1.2- no data ->existing nons + other ->removing all
    2-> only nonamespace | nothing
    3-> other ns | nothing
    4-> nons + other | nothing
    4.1-> nons | nons different
    4.2-> nons + other -> nons different + other different
    4.3-> nons + other -> nons + other some different some not

    (5- xsi exists and is ok -> no other attr) this implicit in the previous ones
    (6- xsi exists with other ns and is ok -> no other attr) this implicit in the previous ones
    7- no xsi exists ->
    8- xsi exists and is not usable ->xsi0
    9- xsi ans xsi0 exist and are not usable ->xsi1
***************************************************/


bool TestInsertXsdReference::test1NoData()
{
    _testName = "test1NoData" ;
    NamespaceReferenceEntry entry;
    if(!testSkeleton(TEST_INS1_START, TEST_INS1_END, &entry) ) {
        return false ;
    }
    return true;
}

bool TestInsertXsdReference::test1_1NoData()
{
    _testName = "test1_1NoData" ;
    NamespaceReferenceEntry entry;
    if(!testSkeleton(TEST_INS1_1_START, TEST_INS1_1_END, &entry) ) {
        return false ;
    }
    return true;
}

bool TestInsertXsdReference::test1_2NoData()
{
    _testName = "test1_2NoData" ;
    NamespaceReferenceEntry entry;
    if(!testSkeleton(TEST_INS1_2_START, TEST_INS1_2_END, &entry) ) {
        return false ;
    }
    return true;
}

bool TestInsertXsdReference::test2()
{
    _testName = "test2" ;
    NamespaceReferenceEntry entry;
    entry.setUseNoNamespaceSchemaLocation(true, "abc");
    if(!testSkeleton(TEST_INS1_START, TEST_INS2_END, &entry) ) {
        return false ;
    }
    return true;
}

bool TestInsertXsdReference::test3()
{
    _testName = "test3" ;
    NamespaceReferenceEntry entry;
    entry.addSchemaReference("aa","bb");
    entry.addSchemaReference("cc","dd");
    if(!testSkeleton(TEST_INS1_START, TEST_INS3_END, &entry) ) {
        return false ;
    }
    return true;
}

bool TestInsertXsdReference::test4()
{
    _testName = "test4" ;
    NamespaceReferenceEntry entry;
    entry.setUseNoNamespaceSchemaLocation(true, "abc");
    entry.addSchemaReference("aa","bb");
    entry.addSchemaReference("cc","dd");
    if(!testSkeleton(TEST_INS1_START, TEST_INS4_END, &entry) ) {
        return false ;
    }
    return true;
}

bool TestInsertXsdReference::test4_1()
{
    _testName = "test4_1" ;
    NamespaceReferenceEntry entry;
    entry.setUseNoNamespaceSchemaLocation(true, "abc");
    if(!testSkeleton(TEST_INS4_1_START, TEST_INS4_1_END, &entry) ) {
        return false ;
    }
    return true;
}

bool TestInsertXsdReference::test4_2()
{
    _testName = "test4_2" ;
    NamespaceReferenceEntry entry;
    entry.setUseNoNamespaceSchemaLocation(true, "abc");
    entry.addSchemaReference("aa","bb");
    entry.addSchemaReference("cc","dd");
    if(!testSkeleton(TEST_INS4_2_START, TEST_INS4_2_END, &entry) ) {
        return false ;
    }
    return true;
}

bool TestInsertXsdReference::test4_3()
{
    _testName = "test4_3" ;
    NamespaceReferenceEntry entry;
    entry.setUseNoNamespaceSchemaLocation(true, "abc");
    entry.addSchemaReference("aa","bb");
    entry.addSchemaReference("cc","dd");
    if(!testSkeleton(TEST_INS4_3_START, TEST_INS4_3_END, &entry) ) {
        return false ;
    }
    return true;
}

bool TestInsertXsdReference::test7()
{
    _testName = "test7" ;
    NamespaceReferenceEntry entry;
    entry.setUseNoNamespaceSchemaLocation(true, "abc");
    entry.addSchemaReference("aa","bb");
    entry.addSchemaReference("cc","dd");
    if(!testSkeleton(TEST_INS7_START, TEST_INS7_END, &entry) ) {
        return false ;
    }
    return true;
}

bool TestInsertXsdReference::test8()
{
    _testName = "test8" ;
    NamespaceReferenceEntry entry;
    entry.setUseNoNamespaceSchemaLocation(true, "abc");
    entry.addSchemaReference("aa","bb");
    entry.addSchemaReference("cc","dd");
    if(!testSkeleton(TEST_INS8_START, TEST_INS8_END, &entry) ) {
        return false ;
    }
    return true;
}

bool TestInsertXsdReference::test9()
{
    _testName = "test9" ;
    NamespaceReferenceEntry entry;
    entry.setUseNoNamespaceSchemaLocation(true, "abc");
    entry.addSchemaReference("aa","bb");
    entry.addSchemaReference("cc","dd");
    if(!testSkeleton(TEST_INS9_START, TEST_INS9_END, &entry) ) {
        return false ;
    }
    return true;
}

bool TestInsertXsdReference::cfr(Regola *regola, const QString &step, const QString &fileResult)
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


bool TestInsertXsdReference::testSkeleton(const QString &fileStart, const QString &fileResult,
                                         NamespaceReferenceEntry *command )
{
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(fileStart) ) {
        return error(QString("unable to load input file: '%1' ").arg(fileStart));
    }
    Regola *regola = app.mainWindow()->getRegola();
    if(!app.mainWindow()->getEditor()->insertXsdReference(command)) {
        return error("Error using the Command");
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

//-------------------------------------------------------------

bool TestInsertXsdReference::testDialogInsert()
{
    _testName = "testDialogInsert";
    if(!testFillDataNegFromRegola()) {
        return false;
    }
    if(!testFillDataFromRegola()) {
        return false;
    }
    //--
    if(!testDialogFromEmpty()) {
        return false;
    }
    if(!testDialogFromData()) {
        return false;
    }
    return true;
}

bool TestInsertXsdReference::testSkeletonEntry(const QString &fileStart, NamespaceReferenceEntry *reference )
{
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(fileStart) ) {
        return error(QString("unable to load input file: '%1' ").arg(fileStart));
    }
    NamespaceReferenceEntry entry;
    app.mainWindow()->getRegola()->XSDReferences(&entry);
    if(!reference->compareTo(&entry)) {
        return error(QString("Error comparing reference entry:\n%1\nreference:\n%2\n")
                     .arg(entry.toString())
                     .arg(reference->toString()));
    }
    return true;
}

bool TestInsertXsdReference::testFillDataNegFromRegola()
{
    _testName = "testFillDataNegFromRegola" ;
    NamespaceReferenceEntry reference ;
    return testSkeletonEntry(FILE_ENTRY_1, &reference );
}

bool TestInsertXsdReference::testFillDataFromRegola()
{
    _testName = "testFillDataFromRegola" ;
    NamespaceReferenceEntry reference ;
    reference.setUseNoNamespaceSchemaLocation(true, "abc");
    reference.addSchemaReference("uno", "due");
    reference.addSchemaReference("tre", "quattro");
    return testSkeletonEntry(FILE_ENTRY_2, &reference );
}

bool TestInsertXsdReference::testSkeletonEntryDialog(NamespaceReferenceEntry *reference)
{
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    SchemaReferencesDialog dlg( NULL, app.data()->namespaceManager(), reference );
    NamespaceReferenceEntry results ;
    dlg.getResults(&results);
    if(!reference->compareTo(&results)) {
        return error(QString("Error comparing reference entry:\n%1\nreference:\n%2\n")
                     .arg(results.toString())
                     .arg(reference->toString()));
    }
    return true;
}

bool TestInsertXsdReference::testDialogFromEmpty()
{
    _testName = "testDialogFromEmpty" ;
    NamespaceReferenceEntry reference ;
    return testSkeletonEntryDialog(&reference );
}

bool TestInsertXsdReference::testDialogFromData()
{
    _testName = "testDialogFromData" ;
    NamespaceReferenceEntry reference ;
    reference.setUseNoNamespaceSchemaLocation(true, "abc");
    reference.addSchemaReference("uno", "due");
    reference.addSchemaReference("tre", "quattro");
    return testSkeletonEntryDialog(&reference );
}

//-------------------------------------------------------------

