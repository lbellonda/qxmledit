/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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

#include "testexport.h"
#include "app.h"

#define FILE_SRC        "../test/data/exp_src.xml"
#define FILE_CMP_JAVA   "../test/data/exp_res.txt"
#define FILE_CMP_C      "../test/data/exp_res_c.txt"

TestExport::TestExport()
{
}

TestExport::~TestExport()
{
}

//----
bool TestExport::testExportJava()
{
    _testName = "testExportJava" ;

    App app;
    if(!app.init(true) ) {
        return error("init app");
    }
    if(!app.mainWindow()->loadFile(FILE_SRC) ) {
        return error(QString("Loading file '%1'").arg(FILE_SRC));
    }
    QBuffer memoryDevice;
    if( !app.mainWindow()->getRegola()->writeAsJavaString(&memoryDevice) ) {
        return error(QString("Writing device"));
    }
    QString deviceString ;
    QString fileData;
    QFile file(FILE_CMP_JAVA);
    if( !loadIOAsString(&file, "UTF-8", &fileData ) ) {
        return error(QString("reading from result file"));
    }
    if( !loadIOAsString(&memoryDevice, "UTF-8", &deviceString ) ) {
        return error(QString("reading from result string"));
    }
    fileData = fileData.replace("\r\n", "\n");
    deviceString = deviceString.replace("\r\n", "\n");
    if( isError() ) {
        return false;
    }
    if( deviceString != fileData ) {
        return error(QString("Comparing data found '%1'\n expecting: '%2'").arg(deviceString).arg(fileData));
    }
    return true ;
}

bool TestExport::testExportC()
{
    _testName = "testExportC" ;

    App app;
    if(!app.init(true) ) {
        return error("init app");
    }
    if(!app.mainWindow()->loadFile(FILE_SRC) ) {
        return error(QString("Loading file '%1'").arg(FILE_SRC));
    }
    QBuffer memoryDevice;
    if( !app.mainWindow()->getRegola()->writeAsCString(&memoryDevice) ) {
        return error(QString("Writing device"));
    }
    QString deviceString ;
    QString fileData;
    QFile file(FILE_CMP_C);
    if( !loadIOAsString(&file, "UTF-8", &fileData ) ) {
        return error(QString("reading from result file"));
    }
    if( !loadIOAsString(&memoryDevice, "UTF-8", &deviceString ) ) {
        return error(QString("reading from result string"));
    }
    if( isError() ) {
        return false;
    }

    fileData = fileData.replace("\r\n", "\n");
    deviceString = deviceString.replace("\r\n", "\n");
    if( deviceString != fileData ) {
        return error(QString("Comparing data found '%1'\n expecting: '%2'").arg(deviceString).arg(fileData));
    }
    return true ;
}
