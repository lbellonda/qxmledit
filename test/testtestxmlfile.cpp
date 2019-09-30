/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2019 by Luca Bellonda and individual contributors       *
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

#include "testtestxmlfile.h"
#include "fakeuidelegate.h"
#include "xmltest.h"

#define DATA_DIR "../test/data/xml/test/"

#define FILE_OK DATA_DIR "fileok.xml"
#define FILE_KO DATA_DIR "fileko.xml"
#define FILE_NOTCLOSED "filenotclosed.xml"
#define FILE_NOTAFILE DATA_DIR "?* :"

TestTestXMLFile::TestTestXMLFile()
{

}

bool TestTestXMLFile::testUnit()
{
    if(!testXMLOK()) {
        return false;
    }
    if(!testXMLKO()) {
        return false;
    }
    if(!testXMLNotClosed()) {
        return false;
    }
    if(!testXMLNotAFile()) {
        return false;
    }
    return true;
}

bool TestTestXMLFile::testAFile(const bool expectedResult, const QString &filePath)
{
    FakeUIDelegate delegate;
    XMLTest testXML;
    const bool result = testXML.testXMLFile(&delegate, filePath);
    if(result != expectedResult) {
        return error(QString("Test result, expected: %1, found: %2").arg(expectedResult).arg(result));
    }
    const bool delegateHasErrors = delegate.errorCount()>0;
    if(expectedResult != !delegateHasErrors ) {
        return error(QString("Delegate error count mismatch, expected: %1, found: %2").arg(expectedResult).arg(delegateHasErrors));
    }
    return true ;
}


bool TestTestXMLFile::testXMLOK()
{
    _testName = "testXMLOK";
    return testAFile(true, FILE_OK);
}

bool TestTestXMLFile::testXMLKO()
{
    return testAFile(false, FILE_KO);
}

bool TestTestXMLFile::testXMLNotClosed()
{
    return testAFile(false, FILE_NOTCLOSED);
}

bool TestTestXMLFile::testXMLNotAFile()
{
    return testAFile(false, FILE_NOTAFILE);
}
