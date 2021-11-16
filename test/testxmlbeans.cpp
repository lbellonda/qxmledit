/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2021 by Luca Bellonda and individual contributors       *
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

#include "testxmlbeans.h"
#include "modules/xml/xmltoxsd.h"
#include "app.h"
#include "testhelpers/xml2xsdtest.h"

#define FILE_INPUT  "../test/data/xsd/i2x.xml"

TestXMLBeans::TestXMLBeans()
{
}

TestXMLBeans::~TestXMLBeans()
{

}


bool TestXMLBeans::testFast()
{
    return testXML2XSDLoadXSD();
}

bool TestXMLBeans::testUnit()
{
    _testName = "testUnit";
    if( !testXML2XSD()) {
        return false;
    }
    return true ;
}


bool TestXMLBeans::testXML2XSD()
{
    _subTestName = "testXML2XSD";
    if(!testXML2XSDParameters()) {
        return false;
    }
    if(!testXML2XSDLoadXSD()) {
        return false;
    }
    if(!testXML2XSDConfig()) {
        return false;
    }
    return true ;
}

static QStringList makeTestArguments0()
{
    QStringList options;
    options << "-enumerations" << "never" << "-simple-content-types" << "string" << "-design" << "rd" << "-outDir" << "dirPath" << "-outPrefix" << "qxmleditschema" << "a/b/c";
    return options;
}

static QStringList makeTestArguments1()
{
    QStringList options;
    options << "-enumerations" << "1" << "-simple-content-types" << "smart" << "-design" << "ss" << "-outDir" << "dirPath" << "-outPrefix" << "qxmleditschema" << "a/b/c";
    return options;
}

static QStringList makeTestArguments2()
{
    QStringList options;
    options << "-enumerations" << "10" << "-simple-content-types" << "smart" << "-design" << "vb" << "-outDir" << "dirPath" << "-outPrefix" << "qxmleditschema" << "a/b/c";
    return options;
}

// test command line parameters
bool TestXMLBeans::testXML2XSDParameters()
{
    _subTestName = "testXML2XSD/testXML2XSDParameters";
    App app;
    app.initNoWindow();
    XMLToXSD xmlToXsd(app.data());
    xmlToXsd._dirPath = "dirPath";
    xmlToXsd._sourceFilePath = "a/b/c";
    if(!compareStringList("case 0", makeTestArguments0(), xmlToXsd.makeArguments(XMLToXSD::GENXSD_RUSSIAN_DOLL, 0, false))) {
        return false;
    }
    if(!compareStringList("case 1", makeTestArguments1(), xmlToXsd.makeArguments(XMLToXSD::GENXSD_SALAMI_SLICE, 1, true))) {
        return false;
    }
    if(!compareStringList("case 2", makeTestArguments2(), xmlToXsd.makeArguments(XMLToXSD::GENXSD_VENETIAN_BLIND, 10, true))) {
        return false;
    }
    return true;
}

// load xsd after creation
bool TestXMLBeans::testXML2XSDLoadXSD()
{
    _subTestName = "testXML2XSD/testXML2XSDLoadXSD";
    App app;
    if(!app.init()) {
        return error("init");
    }
    XML2XSDTest xml2xsdTest(app.data(), FILE_INPUT);
    app.mainWindow()->loadFile(FILE_INPUT);
    OperationResult result;
    if(!xml2xsdTest.generateXSD(&result, app.mainWindow()->getRegola(), XMLToXSD::GENXSD_RUSSIAN_DOLL, 2, false) ) {
        return error("exec");
    }
    if(!xml2xsdTest.existsInputFile()) {
        return error("inputfile empty");
    }
    if(xml2xsdTest.schemaData().isEmpty()) {
        return error("schema empty");
    }
    return true;
}

// test configuration
bool TestXMLBeans::testXML2XSDConfig()
{
    _subTestName = "testXML2XSD/testXML2XSDConfig";
    return error("nyi");
}
