/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2017 by Luca Bellonda and individual contributors       *
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

#include "testformattinginfo.h"
#include "app.h"

#define BASE_TEST  TEST_BASE_DATA   "formatting/"

#define FILE_READ_NODATA BASE_TEST  "read_nodata.xml"
#define FILE_READ_ONLYFORMATTING  BASE_TEST  "read_onlyformatting.xml"
#define FILE_READ_FORMATTINGANDMETADATA BASE_TEST "read_formattingandmetadata.xml"
#define FILE_READ_ONLYMETADATA BASE_TEST "read_onlymetadata.xml"

#define FILE_READ_CFG0 BASE_TEST "read0.xml"
#define FILE_READ_CFG1 BASE_TEST "read1.xml"
#define FILE_READ_CFG2 BASE_TEST "read2.xml"
#define FILE_READ_CFG3 BASE_TEST "read3.xml"
#define FILE_READ_CFG4 BASE_TEST "read4.xml"

TestFormattingInfo::TestFormattingInfo()
{

}

TestFormattingInfo::~TestFormattingInfo()
{

}

bool TestFormattingInfo::testFast()
{
    return  loadOnlyFormatting();
}

bool TestFormattingInfo::testUnit()
{
    _testName = "testUnit";
    if(!testSetup()) {
        return false;
    }
    if(!testRead()) {
        return false;
    }
    if(!testSetReset()) {
        return false;
    }
    if(!testWrite()) {
        return false;
    }
    return true ;
}

//---
bool TestFormattingInfo::testSetup()
{
    _testName = "testSetup";

    {
        App app;
        if(!app.init() ) {
            return error("init window");
        }
        app.data()->setFormattingInfoEnabled(true);
        Regola *regola = app.mainWindow()->getRegola();
        if(regola->hasFormattingInfo()) {
            return error("Config: yes, found formatting info");
        }
    }
    {
        App app;
        if(!app.init() ) {
            return error("init window");
        }
        app.data()->setFormattingInfoEnabled(false);
        Regola *regola = app.mainWindow()->getRegola();
        if(regola->hasFormattingInfo()) {
            return error("Config: no, found formatting info");
        }
    }

    return true;
}


bool TestFormattingInfo::testRead()
{
    _testName = "testRead";
    // load without data
    if(!loadNoData()) {
        return false;
    }
    if(!loadOnlyFormatting()) {
        return false;
    }
    if(!loadOnlyFormattingAndMetadata()) {
        return false;
    }
    if(!loadOnlyMetadata()) {
        return false;
    }
    if(!loadAllConfigurations()) {
        return false;
    }
    return true;
}

bool TestFormattingInfo::testSetReset()
{
    _testName = "testSetReset";
    return error("nyi");
}

bool TestFormattingInfo::testWrite()
{
    _testName = "testWrite";
    return error("nyi");
}

//-----------------------------------------------------------------------
bool TestFormattingInfo::testBaseLoad(const QString &fileName, const bool expected,
                                      bool useIndent, const int indent,
                                      const Regola::ESaveAttributes saveAttrMethod,
                                      const QXmlEditData::EIndentAttributes indentAttributesSetting, const int indentAttributesColumns)
{
    QList<bool> bools;
    bools << false << true ;
    foreach( const bool value, bools ) {
        App app;
        if(!app.init() ) {
            return error("init window");
        }
        app.data()->setFormattingInfoEnabled(value);
        MainWindow *window = app.mainWindow()->loadFileAndReturnWindow(fileName);
        if( NULL == window ) {
            return error(QString("opening test file: '%1'").arg(fileName));
        }
        Regola *regola = window->getRegola();
        if( value ) {
            if(regola->hasFormattingInfo() != expected) {
                return error(QString("Config: %1, file '%2' found formatting info:%3").arg(value).arg(fileName).arg(regola->hasFormattingInfo()));
            }
            if(expected) {
                // check values
                if(useIndent!=regola->useIndentation()) {
                    return error(QString("Config: %1, file '%2' found use indentation: %3").arg(value).arg(fileName).arg(regola->useIndentation()));
                }
                if(indent!=regola->indentation()) {
                    return error(QString("Config: %1, file '%2' found indentation: %3").arg(value).arg(fileName).arg(regola->indentation()));
                }
                if(indentAttributesSetting!=regola->xmlIndentAttributesType()) {
                    return error(QString("Config: %1, file '%2' found attribute indentation: %3").arg(value).arg(fileName).arg(regola->xmlIndentAttributesType()));
                }
                if(indentAttributesColumns!=regola->xmlIndentAttributesColumns()) {
                    return error(QString("Config: %1, file '%2' found attribute indentation column: %3").arg(value).arg(fileName).arg(regola->xmlIndentAttributesColumns()));
                }
                if(saveAttrMethod!=regola->saveAttributesMethod()) {
                    return error(QString("Config: %1, file '%2' found attribute saving method: %3").arg(value).arg(fileName).arg(regola->saveAttributesMethod()));
                }
            }
        } else {
            if(regola->hasFormattingInfo() ) {
                return error(QString("Config: %1, file '%2' found formatting info:%3").arg(value).arg(fileName).arg(regola->hasFormattingInfo()));
            }
        }
    }
    return true;
}

/*
?qxmledit type="formatting", indent="on", indentValue="1", sortAlphaAttr="on", attrLineLen="40" ?
true, 1, Regola::SaveAttributesSortingAlphabetically, QXmlEditData::AttributesIndentationMaxCols, 40 );
 */
bool TestFormattingInfo::loadNoData()
{
    _subTestName = "loadNoData";
    return testBaseLoad(FILE_READ_NODATA, false, true, 1, Regola::SaveAttributesSortingAlphabetically, QXmlEditData::AttributesIndentationMaxCols, 40 );
}

bool TestFormattingInfo::loadOnlyFormatting()
{
    _subTestName = "loadOnlyFormatting";
    return testBaseLoad(FILE_READ_ONLYFORMATTING, true, true, 1, Regola::SaveAttributesSortingAlphabetically, QXmlEditData::AttributesIndentationMaxCols, 40 );
}

bool TestFormattingInfo::loadOnlyFormattingAndMetadata()
{
    _subTestName = "loadOnlyFormattingAndMetadata";
    return testBaseLoad(FILE_READ_FORMATTINGANDMETADATA, true, true, 1, Regola::SaveAttributesSortingAlphabetically, QXmlEditData::AttributesIndentationMaxCols, 40 );
}

bool TestFormattingInfo::loadOnlyMetadata()
{
    _subTestName = "loadOnlyMetadata";
    return testBaseLoad(FILE_READ_ONLYMETADATA, false, true, 1, Regola::SaveAttributesSortingAlphabetically, QXmlEditData::AttributesIndentationMaxCols, 40 );
}

bool TestFormattingInfo::loadAllConfigurations()
{
    _subTestName = "loadAllConfigurations";

    if(!testBaseLoad(FILE_READ_CFG0, true, false, 0, Regola::SaveAttributesNoSort, QXmlEditData::AttributesIndentationNone, 0 )) {
        return false;
    }
    if(!testBaseLoad(FILE_READ_CFG1, true, true, 0, Regola::SaveAttributesNoSort, QXmlEditData::AttributesIndentationNone, 0 )) {
        return false;
    }
    if(!testBaseLoad(FILE_READ_CFG2, true, true, 10, Regola::SaveAttributesNoSort, QXmlEditData::AttributesIndentationNone, 0 )) {
        return false;
    }
    if(!testBaseLoad(FILE_READ_CFG3, true, true, 10, Regola::SaveAttributesSortingAlphabetically, QXmlEditData::AttributesIndentationNone, 0 )) {
        return false;
    }
    if(!testBaseLoad(FILE_READ_CFG4, true, true, 10, Regola::SaveAttributesSortingAlphabetically, QXmlEditData::AttributesIndentationMaxCols, 22 )) {
        return false;
    }
    return true;
}
