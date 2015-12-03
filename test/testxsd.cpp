/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
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
#include "app.h"
#include "testxsd.h"
#include "mainwindow.h"
#include "utils.h"
#include "qxmleditconfig.h"
#include "xsdeditor/xsdwindow.h"
#include <QTest>

#define XSI_NSPREFIX  "xsi"
#define XSI_SCHEMANAME  "http://www.w3.org/2001/XMLSchema-instance"
#define TEST_SCHEMANAME "http://test.test/test"

#define MORE_TEST_NSNAME                "http://www.test.test/test"
#define MORE_TESTHTML_NSPREFIX          "html"
#define MORE_TESTHTML_NSNAME            "http://www.test.test/1999/xhtml"
#define MORE_TEST_SCHEMALOCATION        "test.xsd"
#define MORE_TESTHTML_SCHEMALOCATION    "http://www.test.test/1999/xhtml.xsd"


#define TEST_FILE_XSD_NONS "../test/data/xsd_nons.xml"
#define TEST_FILE_XSD_NONS_FALSE "../test/data/xsd_nonsfalse.xml"
#define TEST_FILE_XSD_ONESCHEMA "../test/data/xsd_oneschema.xml"
#define TEST_FILE_XSD_MORESCHEMA "../test/data/xsd_moreschema.xml"
#define TEST_FILE_XSD_ONESCHEMAMNS "../test/data/xsd_oneschemamns.xml"


#define XMLNS_PREFIX    "xmlns"
#define ATTRIBUTE_SCHEMA_LOCATION    "xsi:schemaLocation"
#define SCHEMA_LOCATION    "schemaLocation"
#define ATTRIBUTE_SCHEMA_NONS_LOCATION    "xsi:noNamespaceSchemaLocation"
#define SCHEMA_NONS_LOCATION    "noNamespaceSchemaLocation"
#define SCHEMA_URI "http://www.w3.org/2001/XMLSchema-instance"
#define XSI_NAMESPACE   "xsi"
#define XSI_NAMESPACE_COMPLETE   "xmlns:xsi"

#define XSD_SAME_NAMESPACE  "../test/data/xsd/samenamespace.xsd"

TestXsd::TestXsd()
{
}

// load some schema, then check inner data
bool TestXsd::TestNonamespace()
{
    App app;
    if(!app.initNoWindow() ) {
        return false;
    }
    MainWindow mainWindow(false, qApp, app.data());

    mainWindow.loadFile(QString(TEST_FILE_XSD_NONS));

    Regola *regola = mainWindow.getRegola();

    if(regola->_noNameSpaceXsd != QString("test.xsd")) {
        return false;
    }
    if(regola->_documentXsd != QString("test.xsd")) {
        return false;
    }
    if(regola->_namespacesByPrefixAndName.size() != 1) {
        return false;
    }
    if(regola->_namespacesByPrefixAndName[QString(XSI_NSPREFIX)] != QString(XSI_SCHEMANAME)) {
        return false;
    }
    if(regola->_namespacesByNameAndPrefix.size() != 1) {
        return false;
    }
    if(regola->_namespacesByNameAndPrefix[QString(XSI_SCHEMANAME)] != QString(XSI_NSPREFIX)) {
        return false;
    }
    if(!regola->_schemaLocationsByNamespace.isEmpty()) {
        return false;
    }
    return true ;
}

bool TestXsd::TestNonamespaceFalse()
{
    App app;
    if(!app.initNoWindow() ) {
        return false;
    }
   MainWindow mainWindow(false, qApp, app.data());

    mainWindow.loadFile(QString(TEST_FILE_XSD_NONS_FALSE));

    Regola *regola = mainWindow.getRegola();

    if(!regola->_noNameSpaceXsd.isEmpty()) {
        return false;
    }
    if(!regola->_documentXsd.isEmpty()) {
        return false;
    }
    if(!regola->_namespacesByPrefixAndName.isEmpty()) {
        return false;
    }
    if(!regola->_namespacesByNameAndPrefix.isEmpty()) {
        return false;
    }
    if(!regola->_schemaLocationsByNamespace.isEmpty()) {
        return false;
    }
    return true ;
}

bool TestXsd::TestOneSchema()
{
    App app;
    if(!app.initNoWindow() ) {
        return false;
    }
   MainWindow mainWindow(false, qApp, app.data());
    mainWindow.loadFile(QString(TEST_FILE_XSD_ONESCHEMA));

    Regola *regola = mainWindow.getRegola();

    if(!regola->_noNameSpaceXsd.isEmpty()) {
        return false;
    }
    if(regola->_documentXsd != QString("test.xsd")) {
        return false;
    }
    if(regola->_namespacesByPrefixAndName.size() != 2) {
        return false;
    }
    if(regola->_namespacesByPrefixAndName[QString(XSI_NSPREFIX)] != QString(XSI_SCHEMANAME)) {
        return false;
    }
    if(regola->_namespacesByPrefixAndName[QString("")] != QString(TEST_SCHEMANAME)) {
        return false;
    }
    if(regola->_namespacesByNameAndPrefix.size() != 2) {
        return false;
    }
    if(regola->_namespacesByNameAndPrefix[QString(XSI_SCHEMANAME)] != QString(XSI_NSPREFIX)) {
        return false;
    }
    if(regola->_namespacesByNameAndPrefix[QString(TEST_SCHEMANAME)] != QString("")) {
        return false;
    }
    if(regola->_schemaLocationsByNamespace.size() != 1) {
        return false;
    }
    if(regola->_schemaLocationsByNamespace[QString(TEST_SCHEMANAME)] != QString("test.xsd")) {
        return false;
    }
    return true ;
}

bool TestXsd::TestMoreSchema()
{
    App app;
    if(!app.initNoWindow() ) {
        return false;
    }
   MainWindow mainWindow(false, qApp, app.data());
    mainWindow.loadFile(QString(TEST_FILE_XSD_MORESCHEMA));

    Regola *regola = mainWindow.getRegola();

    if(!regola->_noNameSpaceXsd.isEmpty()) {
        return false;
    }
    if(regola->_documentXsd != QString(MORE_TEST_SCHEMALOCATION)) {
        return false;
    }
    if(regola->_namespacesByPrefixAndName.size() != 3) {
        return false;
    }
    if(regola->_namespacesByPrefixAndName[QString(XSI_NSPREFIX)] != QString(XSI_SCHEMANAME)) {
        return false;
    }
    if(regola->_namespacesByPrefixAndName[QString("")] != QString(MORE_TEST_NSNAME)) {
        return false;
    }
    if(regola->_namespacesByPrefixAndName[QString(MORE_TESTHTML_NSPREFIX)] != QString(MORE_TESTHTML_NSNAME)) {
        return false;
    }
    if(regola->_namespacesByNameAndPrefix.size() != 3) {
        return false;
    }
    if(regola->_namespacesByNameAndPrefix[QString(XSI_SCHEMANAME)] != QString(XSI_NSPREFIX)) {
        return false;
    }
    if(regola->_namespacesByNameAndPrefix[QString(TEST_SCHEMANAME)] != QString("")) {
        return false;
    }
    if(regola->_namespacesByNameAndPrefix[QString(MORE_TESTHTML_NSNAME)] != QString(MORE_TESTHTML_NSPREFIX)) {
        return false;
    }
    if(regola->_schemaLocationsByNamespace.size() != 2) {
        return false;
    }
    if(regola->_schemaLocationsByNamespace[QString(MORE_TEST_NSNAME)] != QString(MORE_TEST_SCHEMALOCATION)) {
        return false;
    }
    if(regola->_schemaLocationsByNamespace[QString(MORE_TESTHTML_NSNAME)] != QString(MORE_TESTHTML_SCHEMALOCATION)) {
        return false;
    }
    return true ;
}


bool TestXsd::testAttributesValue(Element *root,
                                  const QString aName1, const QString aValue1,
                                  const QString aName2, const QString aValue2)
{
    bool result = true ;
    Attribute * attrNSSchema = root->getAttribute(aName1);
    Attribute * attrNSReference = root->getAttribute(aName2);
    if((NULL == attrNSSchema) || (NULL == attrNSReference)) {
        result = false;
    } else {
        if((attrNSSchema->value != aValue1)
                || (attrNSReference->value != aValue2)) {
            result = false ;
        }
    }
    return result ;
}


bool TestXsd::testSameNamespace()
{
    _testName = "testSameNamespace";
    App app;
    if(!app.initNoWindow() ) {
        return error("init");
    }
    MainWindow mainWindow(false, qApp, app.data());

    mainWindow.loadFile(QString(XSD_SAME_NAMESPACE));

    XSDWindow * xsdEditor = new XSDWindow(app.data(), app.mainWindow()) ;
    QString xmlAsString = mainWindow.getRegola()->getAsText();
    xsdEditor->show();
    xsdEditor->setFileName("test");
    xsdEditor->setTitle("test");
    xsdEditor->loadString(xmlAsString);
    QTest::qWait(200);
    bool result = xsdEditor->isInError();

    delete xsdEditor;
    if( result ) {
        return error("error loading xsw with same namespace");
    }

    return true ;
}


/*
bool TestXsd::testAttributes2()
{
    Regola::EInsSchemaRefInfo error;
    bool result = true ;
    Regola *regola = new Regola();
    Element *root = new Element("root", "", regola );
    regola->addTopElement(root);

    error = regola->insertNoNamespaceSchemaReferenceAttributes();
    if( error != Regola::INSERT_SCHEMA_ATTR_NOERROR ) {
        result = false;
    }

    if( !testAttributesValue( root,
                              XSI_NAMESPACE_COMPLETE, ATTRIBUTE_SCHEMA_LOCATION,
                              SCHEMA_URI, "***" ) ) {
             result = false ;
    }
    delete regola;
    if(!result ) {
        return false;
    }
    //-----------------------------------------------------------------------------
    regola = new Regola();
    root = new Element("root", "", regola );
    regola->addTopElement(root);

    error = regola->insertSchemaReferenceAttributes();
    if( error != Regola::INSERT_SCHEMA_ATTR_NOERROR ) {
        result = false;
    }

    if( !testAttributesValue( root,
                              XSI_NAMESPACE_COMPLETE, ATTRIBUTE_SCHEMA_LOCATION,
                              SCHEMA_URI, "***" ) ) {
             result = false ;
    }
    delete regola;
    if(!result ) {
        return false;
    }
    //-----------------------------------------------------------------------------
    regola = new Regola();
    root = new Element("root", "", regola );
    regola->addTopElement(root);

    root->addAttribute(XSI_NAMESPACE_COMPLETE, "x");

    error = regola->insertSchemaReferenceAttributes();
    if( error != Regola::INSERT_SCHEMA_ATTR_INFO_SCHEMAPRESENT ) {
        result = false;
    }

    if( !testAttributesValue( root,
                              XSI_NAMESPACE_COMPLETE, ATTRIBUTE_SCHEMA_LOCATION,
                              "x", "***" ) ) {
             result = false ;
    }
    delete regola;
    if(!result ) {
        return false;
    }

    regola = new Regola();
    root = new Element("root", "", regola );
    regola->addTopElement(root);

    root->addAttribute(ATTRIBUTE_SCHEMA_LOCATION, "x");

    error = regola->insertSchemaReferenceAttributes();
    if( error != Regola::INSERT_SCHEMA_ATTR_INFO_SCHEMAPRESENT ) {
        result = false;
    }

    if( !testAttributesValue( root,
                              XSI_NAMESPACE_COMPLETE, ATTRIBUTE_SCHEMA_LOCATION,
                              SCHEMA_URI, "x" ) ) {
             result = false ;
    }
    delete regola;
    if(!result ) {
        return false;
    }
    //-----------------------------------------------------------------------------
    regola = new Regola();
    root = new Element("root", "", regola );
    regola->addTopElement(root);

    root->addAttribute(ATTRIBUTE_SCHEMA_LOCATION, "x1");
    root->addAttribute(XSI_NAMESPACE_COMPLETE, "x2");

    error = regola->insertSchemaReferenceAttributes();
    if( error != Regola::INSERT_SCHEMA_ATTR_INFO_SCHEMAPRESENT ) {
        result = false;
    }

    if( !testAttributesValue( root,
                              XSI_NAMESPACE_COMPLETE, ATTRIBUTE_SCHEMA_LOCATION,
                              "x1", "x2" ) ) {
             result = false ;
    }
    delete regola;
    if(!result ) {
        return false;
    }
    return true ;
}

*/

/*


bool TestXsd::testAttributes()
{
    bool result = true ;
    Regola *regola = new Regola();
    Element *root = new Element("root", "", regola );
    regola->addTopElement(root);

    Regola::EInsSchemaRefInfo error = regola->insertNoNamespaceSchemaReferenceAttributes();
    if( error != Regola::INSERT_SCHEMA_ATTR_NOERROR ) {
        result = false;
    }
    Attribute * attrNSSchema = root()->getAttribute(XSI_NAMESPACE_COMPLETE);
    Attribute * attrNSReference = root()->getAttribute(ATTRIBUTE_SCHEMA_NONS_LOCATION);
    if( ( NULL == attrNSSchema ) || ( NULL == attrNSReference ) ) {
        result = false;
    } else {
        if(     ( attrNSSchema->value != SCHEMA_URI)
            ||  ( attrNSReference->value != "***" ) ) {
            result = false ;
        }
    }
    delete regola;
    if(!result ) {
        return false;
    }

    regola = new Regola();
    root = new Element("root", "", regola );
    regola->addTopElement(root);

    Regola::EInsSchemaRefInfo error = regola->insertNoNamespaceSchemaReferenceAttributes();
    if( error != Regola::INSERT_SCHEMA_ATTR_NOERROR ) {
        result = false;
    }
    Attribute * attrNSSchema = root()->getAttribute(XSI_NAMESPACE_COMPLETE);
    Attribute * attrNSReference = root()->getAttribute(ATTRIBUTE_SCHEMA_NONS_LOCATION);
    if( ( NULL == attrNSSchema ) || ( NULL == attrNSReference ) ) {
        result = false;
    } else {
        if(     ( attrNSSchema->value != SCHEMA_URI)
            ||  ( attrNSReference->value != "***" ) ) {
            result = false ;
        }
    }
    delete regola;
    if(!result ) {
        return false;
    }

}
*/
