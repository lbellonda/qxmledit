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
#include "modules/xml/xsdtoxml.h"
#include "app.h"
#include "testhelpers/xml2xsdtest.h"
#include "testhelpers/xsd2xmltest.h"
#include "config/configvalidation.h"

#define DATA_FOLDER  "../test/data/xsd/xmlbeans/"

#define FILE_INPUT_XSD_TO_XML DATA_FOLDER "ixsd2xml.xml"
#define FILE_OUTPUT_XSD_TO_XML DATA_FOLDER "oxsd2xml.xml"
#define FILE_INPUT_XSD_TO_XML_NOSCHEMA DATA_FOLDER "ixsd2xmlnoschema.xml"
#define FILE_NO_XML DATA_FOLDER "noxml"

#define FILE_XML_TO_XSD DATA_FOLDER "xml2xsd.xml"
#define FILE_XML_TO_XSD_OUTPUT DATA_FOLDER "oxml2xsd.xml"
//-----------------------------------

TestXMLBeans::TestXMLBeans()
{
    _factoryChooseElement = false;
    _factoryErrorInExecution = false;
}

TestXMLBeans::~TestXMLBeans()
{
}

bool TestXMLBeans::testFast()
{
    return testXML2XSD();
}

bool TestXMLBeans::testUnit()
{
    _testName = "testUnit";
    if( !testXML2XSD()) {
        return false;
    }
    if( !testXSD2XML()) {
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

bool TestXMLBeans::testXSD2XML()
{
    _subTestName = "testXSD2XML";
    if(!testXSD2XMLParameters()) {
        return false;
    }
    if(!testXSD2XMLRun()) {
        return false;
    }
    if(!testXSD2XMLConfig()) {
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
/*
 * 1 - ok: ok
 * 2 - exec error: false
 * 3 - exec ok, error loading file: false
 */
bool TestXMLBeans::testXML2XSDLoadXSD()
{
    _subTestName = "TestXMLBeans/testXML2XSDLoadXSD";
    if(!testXML2XSDRunInner("1", FILE_XML_TO_XSD, true, false, FILE_XML_TO_XSD_OUTPUT)) {
        return false;
    }
    if(!testXML2XSDRunInner("2", FILE_XML_TO_XSD, false, true, FILE_XML_TO_XSD_OUTPUT)) {
        return false;
    }
    if(!testXML2XSDRunInner("3", FILE_XML_TO_XSD, false, false, "")) {
        return false;
    }
    return true ;
}

bool TestXMLBeans::testXML2XSDRunInner(const QString &code,
                                       const QString &fileInput,
                                       const bool expectedResult, const bool errorInExecution,
                                       const QString &expectedDataFile)
{
    App app;
    if(!app.init()) {
        return error(QString("Code %1, init").arg(code));
    }
    app.mainWindow()->loadFile(fileInput);
    _factoryFileToRead = expectedDataFile ;
    _factoryErrorInExecution = errorInExecution ;
    app.mainWindow()->controller()->setXMLVsXSDFactory(this);
    MainWindow *resultWindow = app.mainWindow()->controller()->generateXSDFromData();
    const bool result = NULL != resultWindow ;
    if(result != expectedResult) {
        return error(QString("Code %1, XML2XSD expected:%2, found %3").arg(code).arg(expectedResult).arg(result));
    }
    if(!result) {
        return true;
    }
    if( !compareXMLBase(resultWindow->getRegola(), QString("Code %1 comparing results").arg(code), expectedDataFile)) {
        return false;
    }
    return true;
}

static QStringList makeXSD2XMLTestArguments0()
{
    QStringList options;
    options << "the source" << "-name" << "abc";
    return options;
}

bool TestXMLBeans::testXSD2XMLParameters()
{
    _subTestName = "TestXMLBeans/testXSD2XMLParameters";
    App app;
    app.initNoWindow();
    XSDToXML xsdToXml(app.data());
    xsdToXml._sourceFilePath = "the source";
    xsdToXml._localNameOfGlobalElement = "abc";
    if(!compareStringList("case 0", makeXSD2XMLTestArguments0(), xsdToXml.makeArguments())) {
        return false;
    }
    return true;
}

/*
1 - xsd, elements, scelta elemento OK ->ok, dati ok
2 - xsd, elements, scelta elemento ko ->false
3 - file non xsd -> false
4 - xsd, elements, execution error -> false
5 - xsd, elements, File generato non xml -> true, no window
*/

bool TestXMLBeans::testXSD2XMLRun()
{
    _subTestName = "TestXMLBeans/testXSD2XMLRun";
    if(!testXSD2XMLRunInner("1", FILE_INPUT_XSD_TO_XML, true, "sample", false, FILE_OUTPUT_XSD_TO_XML)) {
        return false;
    }
    if(!testXSD2XMLRunInner("2", FILE_INPUT_XSD_TO_XML, false, "", false, "")) {
        return false;
    }
    if(!testXSD2XMLRunInner("3", FILE_INPUT_XSD_TO_XML_NOSCHEMA, false, "sample", false, "")) {
        return false;
    }
    if(!testXSD2XMLRunInner("4", FILE_INPUT_XSD_TO_XML, false, "sample", true, "")) {
        return false;
    }
    if(!testXSD2XMLRunInner("5", FILE_INPUT_XSD_TO_XML, false, "sample", true, FILE_NO_XML)) {
        return false;
    }
    return true ;
}


bool TestXMLBeans::testXSD2XMLRunInner(const QString &code,
                                        const QString &fileInput, const bool expectedResult,
                                       const QString &elementChosen, const bool errorInExecution,
                                       const QString &expectedDataFile)
{
    App app;
    if(!app.init()) {
        return error(QString("Code %1, init").arg(code));
    }
    if(!app.mainWindow()->loadFile(fileInput)) {
        return error(QString("Code %1, loading input file :%2").arg(code).arg(fileInput));
    }
    app.mainWindow()->controller()->setXSDTopElementChooser(this);
    _factoryChooseElement = elementChosen ;
    _factoryFileToRead = expectedDataFile ;
    _factoryErrorInExecution = errorInExecution ;
    app.mainWindow()->controller()->setXMLVsXSDFactory(this);
    MainWindow *resultWindow = app.mainWindow()->controller()->generateDataFromXSD();
    const bool result = NULL != resultWindow ;
    if(result != expectedResult) {
        return error(QString("Code %1, generateDataFromXSD expected:%2, found %3").arg(code).arg(expectedResult).arg(result));
    }
    if(!result) {
        return true;
    }
    if( !compareXMLBase(resultWindow->getRegola(), QString("Code %1 comparing results").arg(code), expectedDataFile)) {
        return false ;
    }
    return true ;
}

XSDToXML* TestXMLBeans::newXSD2XML(ApplicationData *appData)
{
    XSD2XMLTest * instance = new XSD2XMLTest(appData, _factoryFileToRead, _factoryErrorInExecution);
    return instance;
}

XMLToXSD* TestXMLBeans::newXML2XSD(ApplicationData *appData)
{
    XML2XSDTest * instance = new XML2XSDTest(appData, _factoryFileToRead, _factoryErrorInExecution);
    return instance;
}

// test configuration
bool TestXMLBeans::testXML2XSDConfig()
{
    _subTestName = "TestXMLBeans/testXML2XSDConfig";
    App app;
    if(!app.init()) {
        return error("init");
    }
    ConfigValidation configValidation;
    Config::saveString(Config::KEY_TOOLS_XMLBEANS_INST2XSD, "xyz");
    configValidation.init(app.data());
    Config::saveString(Config::KEY_TOOLS_XMLBEANS_INST2XSD, "aaa");
    configValidation.save();
    //
    QString savedData = app.data()->inst2XSDPath();
    if(savedData != "xyz") {
        return error(QString("Expected %1 found '%2'").arg("xyz").arg(savedData));
    }
    return true ;
}

bool TestXMLBeans::testXSD2XMLConfig()
{
    _subTestName = "TestXMLBeans/testXSD2XMLConfig";
    App app;
    if(!app.init()) {
        return error("init");
    }
    ConfigValidation configValidation;
    Config::saveString(Config::KEY_TOOLS_XMLBEANS_XSD2INST, "xyz");
    configValidation.init(app.data());
    Config::saveString(Config::KEY_TOOLS_XMLBEANS_XSD2INST, "aaa");
    configValidation.save();
    //
    QString savedData = app.data()->xsd2InstPath();
    if(savedData != "xyz") {
        return error(QString("Expected %1 found '%2'").arg("xyz").arg(savedData));
    }
    return true ;
}

QString TestXMLBeans::selectTopLevelSchemaElement(Regola * /*regola*/)
{
    return _factoryChooseElement ;
}
