/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2020 by Luca Bellonda and individual contributors       *
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

#include "testloadsample.h"
#include "app.h"

#define BASE_PATH "../test/data/xml/loadsample/"
#define FILE_ERROR  BASE_PATH "error.xml"
#define FILE_UNITY  BASE_PATH "unity.xml"
#define FILE_DOUBLE  BASE_PATH "double.xml"
#define FILE_DOUBLE_E  BASE_PATH "double_e.xml"
#define FILE_3  BASE_PATH "3.xml"
#define FILE_3_E  BASE_PATH "3_e.xml"
#define FILE_4  BASE_PATH "4.xml"
#define FILE_4_E  BASE_PATH "4_e.xml"
#define FILE_5  BASE_PATH "5.xml"
#define FILE_5_E  BASE_PATH "5_e.xml"
#define FILE_ADDATTR  BASE_PATH "addattr.xml"
#define FILE_ADDATTR_E  BASE_PATH "addattr_e.xml"
#define FILE_ADDELEMENT  BASE_PATH "addelement.xml"
#define FILE_ADDELEMENT_E  BASE_PATH "addelement_e.xml"
#define FILE_ADDATTRNS  BASE_PATH "addattr_ns.xml"
#define FILE_ADDATTRNS_E  BASE_PATH "addattr_ns_e.xml"
#define FILE_ADDELEMENTNS  BASE_PATH "addelement_ns.xml"
#define FILE_ADDELEMENTNS_E  BASE_PATH "addelement_ns_e.xml"
#define FILE_PREAMBLE  BASE_PATH "preamble.xml"
#define FILE_COMMENT  BASE_PATH "comment.xml"
#define FILE_COMMENT_E  BASE_PATH "comment_e.xml"
#define FILE_PROCESSINGINSTR  BASE_PATH "pi.xml"
#define FILE_PROCESSINGINSTR_E  BASE_PATH "pi_e.xml"
#define FILE_LOADSUMALL  BASE_PATH "loadsumall.xml"
#define FILE_LOADSUMALL_E  BASE_PATH "loadsumall_e.xml"
#define FILE_LOADSUMALLNS  BASE_PATH "loadsumall_ns.xml"
#define FILE_LOADSUMALLNS_E  BASE_PATH "loadsumall_ns_e.xml"

TestLoadSample::TestLoadSample()
{
    //
}

TestLoadSample::~TestLoadSample()
{
    //
}

bool TestLoadSample::testFast()
{
    return testLoadDouble();
}

bool TestLoadSample::testUnit()
{
    _testName = "testUnit";
    if(!testLoadError()) {
        return false;
    }
    if(!testLoadIdentity()) {
        return false;
    }
    if(!testLoadPreamble()) {
        return false;
    }
    if(!testLoadComment()) {
        return false;
    }
    if(!testLoadProcessingInstruction()) {
        return false;
    }
    if(!testLoadDouble()) {
        return false;
    }
    if(!testLoadSumAttributes()) {
        return false;
    }
    if(!testLoadSumElements()) {
        return false;
    }
    if(!testLoadSumAttributesNS()) {
        return false;
    }
    if(!testLoadSumElementsNS()) {
        return false;
    }
    if(!testLoadSumAll()) {
        return false;
    }
    if(!testLoadSumAllNS()) {
        return false;
    }
    return true;
}


bool TestLoadSample::testAFile(
                    const bool isLoad,
                    const QString &code,
                    const QString &inputFilePath,
                    const bool expectedLoadStatus,
                    const bool expectedModified,
                    const QString &expectedFileName,
                    const QString &compareFileResultPath)
{
    App app;
    if(!app.init()) {
        return error(QString("Code %1, init app").arg(code));
    }
    if(isLoad) {
        if( app.mainWindow()->loadFile(inputFilePath) != expectedLoadStatus) {
            return error(QString("Code %1, expected %2 for file:%3").arg(code).arg(expectedLoadStatus).arg(inputFilePath));
        }
    } else {
        if( app.mainWindow()->loadSample(inputFilePath) != expectedLoadStatus) {
            return error(QString("Code %1, expected %2 for file:%3").arg(code).arg(expectedLoadStatus).arg(inputFilePath));
        }
    }
    if(expectedLoadStatus) {
        Regola *regola = app.mainWindow()->getEditor()->getRegola();
        if(regola->isModified() != expectedModified ) {
            return error(QString("Code %1, modified expected:%2, found:%3").arg(code).arg(expectedModified).arg(regola->isModified()));
        }
        if(regola->fileName() != expectedFileName ) {
            return error(QString("Code %1, file path expected:'%2' found:'%3'").arg(code).arg(expectedFileName).arg(regola->fileName()));
        }
        if(!compare(regola, code, compareFileResultPath)) {
            return false;
        }
    }
    return true ;
}

bool TestLoadSample::testSame(
                    const QString &code,
                    const QString &inputFilePath,
                    const bool expectedLoadStatus) {
    return testAFile( true, code, inputFilePath, expectedLoadStatus, false, inputFilePath, inputFilePath );
}


bool TestLoadSample::testLoadError()
{
    _subTestName = "testLoadError";
    if(!testSame(_subTestName, FILE_ERROR, false)) {
        return false;
    }
    if(!testAFile(false, _subTestName, FILE_ERROR, false, true, FILE_UNITY, FILE_UNITY)) {
        return false;
    }
    return true;
}

bool TestLoadSample::testLoadIdentity()
{
    _subTestName = "testLoadIdentity";
    if(!testABatch("1", FILE_UNITY, FILE_UNITY)) {
        return false;
    }
    return true;
}

bool TestLoadSample::testABatch(const QString &key, const QString &fileSource, const QString &fileReference)
{
    if(!testSame(_subTestName+"Ident "+key, fileSource, true)) {
        return false;
    }
    if(!testAFile(false, _subTestName+"Sample"+ key, fileSource, true, true, "", fileReference)) {
        return false;
    }
    return true ;
}

bool TestLoadSample::testLoadDouble()
{
    _subTestName ="testLoadDouble";
    if(!testABatch("1", FILE_DOUBLE, FILE_DOUBLE_E)) {
        return false;
    }
    if(!testABatch("3", FILE_3, FILE_3_E)) {
        return false;
    }
    if(!testABatch("4", FILE_4, FILE_4_E)) {
        return false;
    }
    if(!testABatch("5", FILE_5, FILE_5_E)) {
        return false;
    }
    return true;
}

bool TestLoadSample::testLoadSumAttributes()
{
    _subTestName ="testLoadSumAttributes";
    if(!testABatch("1", FILE_ADDATTR, FILE_ADDATTR_E)) {
        return false;
    }
    return true;
}

bool TestLoadSample::testLoadSumElements()
{
    _subTestName ="testLoadSumElements";
    if(!testABatch("1", FILE_ADDELEMENT, FILE_ADDELEMENT_E)) {
        return false;
    }
    return true;
}

bool TestLoadSample::testLoadSumAttributesNS()
{
    _subTestName ="testLoadSumAttributesNS";
    if(!testABatch("1", FILE_ADDATTRNS, FILE_ADDATTRNS_E)) {
        return false;
    }
    return true;
}

bool TestLoadSample::testLoadSumElementsNS()
{
    _subTestName ="testLoadSumElementsNS";
    if(!testABatch("1", FILE_ADDELEMENTNS, FILE_ADDELEMENTNS_E)) {
        return false;
    }
    return true;
}

bool TestLoadSample::testLoadPreamble()
{
    _subTestName ="testLoadPreamble";
    if(!testABatch("1", FILE_PREAMBLE, FILE_PREAMBLE)) {
        return false;
    }
    return true;
}

bool TestLoadSample::testLoadComment()
{
    _subTestName ="testLoadComment";
    if(!testABatch("1", FILE_COMMENT, FILE_COMMENT_E)) {
        return false;
    }
    return true;
}

bool TestLoadSample::testLoadProcessingInstruction()
{
    _subTestName ="testLoadProcessingInstruction";
    if(!testABatch("1", FILE_PROCESSINGINSTR, FILE_PROCESSINGINSTR_E)) {
        return false;
    }
    return true;
}

bool TestLoadSample::testLoadSumAll()
{
    _subTestName ="testLoadSumAll";
    if(!testABatch("1", FILE_LOADSUMALL, FILE_LOADSUMALL_E)) {
        return false;
    }
    return true;
}

bool TestLoadSample::testLoadSumAllNS()
{
    _subTestName ="testLoadSumAllNS";
    if(!testABatch("1", FILE_LOADSUMALLNS, FILE_LOADSUMALLNS_E)) {
        return false;
    }
    return true;
}
