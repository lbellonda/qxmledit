/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2023 by Luca Bellonda and individual contributors       *
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

#include "testvalidateasxsd.h"
#include "app.h"

#define BASE_DATA "../test/data/xsd/"
#define FILE_OK     BASE_DATA "viewasxsd_ok.xsd"
#define FILE_KO     BASE_DATA "viewasxsd_ko.xsd"
#define FILE_NO     BASE_DATA "viewasxsd_no.xml"
#define FILE_ISO    BASE_DATA "viewasxsd_iso.xml"

TestValidateAsXSD::TestValidateAsXSD()
{
    //
}

TestValidateAsXSD::~TestValidateAsXSD()
{
    //
}

bool TestValidateAsXSD::testFast()
{
    return testValidateNO();
}

bool TestValidateAsXSD::testUnit() {
    _testName = "testUnit";

    TEST_OR_DIE(testValidateOK());

    TEST_OR_DIE(testValidateKO());

    TEST_OR_DIE(testValidateNO());

    TEST_OR_DIE(testValidateEncodingISO());

    return true;
}

bool TestValidateAsXSD::testValidateOK()
{
    _testName = "testValidateOK";
    TEST_OR_DIE(loadAndValidate(FILE_OK, true, true));
    return true ;
}

bool TestValidateAsXSD::testValidateKO()
{
    _testName = "testValidateKO";
    TEST_OR_DIE(loadAndValidate(FILE_KO, true, false));
    return true ;
}

bool TestValidateAsXSD::testValidateNO()
{
    _testName = "testValidateNO";
    TEST_OR_DIE(loadAndValidate(FILE_NO, false, false));
    return true ;
}

bool TestValidateAsXSD::testValidateEncodingISO()
{
    _testName = "testValidateEncodingISO";
    TEST_OR_DIE(loadAndValidate(FILE_ISO, true, true));
    return true ;
}

bool TestValidateAsXSD::loadAndValidate(const QString &fileToLoad, const bool expectedEnabled, const bool expectedResult)
{
    App app;
    if(!app.init()) {
        return error("Unable to init");
    }
    if(!app.mainWindow()->loadFile(fileToLoad)) {
        return error(QString("Unable to load %1").arg(fileToLoad));
    }
    {
        bool result = false ;
        const bool found = checkActionEnabled(app.mainWindow(), "actionValidateXSD", result);
        if(!found) {
            return error(QString("Action enabling not found for %1, expecting %2")
                         .arg(fileToLoad).arg(expectedEnabled));
        }
        if(expectedEnabled != result) {
            return error(QString("Action enabling failure for %1, expecting %2, but was %3")
                         .arg(fileToLoad).arg(expectedEnabled).arg(result));
        }
    }
    {
        const bool result = app.mainWindow()->actionValidateXSD();
        if(expectedResult != result) {
            return error(QString("Validation error for %1, expecting %2, found %3")
                         .arg(fileToLoad).arg(expectedResult).arg(expectedResult));
        }
    }
    return true ;
}
