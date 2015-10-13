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


#include "testloadfile.h"
#include "app.h"

#define TEST_DATA TEST_BASE_DATA "/loadfile"

#define FILE_OK TEST_DATA "fileok.xml"
#define FILE_KO TEST_DATA "fileko.xml"

TestLoadFile::TestLoadFile()
{
}

TestLoadFile::~TestLoadFile()
{
}

bool TestLoadFile::unitTest()
{
    _testName = "unitTest" ;
    if(!loadFileOK()) {
        return false;
    }
    if(!loadFileKO()) {
        return false;
    }
    return true;
}

bool TestLoadFile::loadFileOK()
{
    _testName = "loadFileOK";
    App app;
    if(!app.init() ) {
        return error("init");
    }
    app.getUiDelegate()->resetErrorCount();
    if(!app.mainWindow()->loadFile(QString(FILE_OK))) {
        return error("load file");
    }
    if(app.getUiDelegate()->errorCount()>0) {
        return error(QString("Expected no errors, but found:%1").arg(app.getUiDelegate()->errorCount()));
    }
    return true ;
}

bool TestLoadFile::loadFileKO()
{
    _testName = "loadFileKO";
    App app;
    if(!app.init() ) {
        return error("init");
    }
    app.getUiDelegate()->resetErrorCount();
    if(!app.mainWindow()->loadFile(QString(FILE_OK))) {
        return error("load file");
    }
    if(app.getUiDelegate()->errorCount()==0) {
        return error(QString("Expected errors, but found:%1").arg(app.getUiDelegate()->errorCount()));
    }
    return true ;
}

fare test di apparizione messaggio di errore se malformato e non messaggio se benformato:

