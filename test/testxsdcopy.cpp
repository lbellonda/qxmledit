/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013-2018 by Luca Bellonda and individual contributors  *
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


#include "testxsdcopy.h"
#include "app.h"
#include "testxsdnavigation.h"
#include "comparexml.h"
#include <QTest>

#define TEST_FILE_XSD_BASE  "../test/data/xsd/copytestbase.xsd"
#define FILE_TESTS_DATA "../test/data/xsd/copytestdata.xml"
#define FILE_TESTS_DATA_FACETS "../test/data/xsd/copytestfacetsdata.xml"

TestXSDCopy::TestXSDCopy()
{
    xsdEditor = NULL ;
}

TestXSDCopy::~TestXSDCopy()
{
    deleteEditor();
}

void TestXSDCopy::deleteEditor()
{
    if( NULL != xsdEditor ) {
        //xsdEditor->close();
        //xsdEditor->close();
        xsdEditor->deleteLater();
        //delete xsdEditor ;
        xsdEditor = NULL ;
    }
}


//---------------------------------------------------------

bool TestXSDCopy::testCopy()
{
    _testName = "testCopy";

    if(!testCopyNoattrs()) {
        return false;
    }
    return true;
}


bool TestXSDCopy::testCopyNoattrs()
{
    App app;
    if(!app.initNoWindow() ) {
        return error("init");
    }
    MainWindow mainWindow(false, app.data());
    if( !mainWindow.loadFile(QString(TEST_FILE_XSD_BASE)) ) {
        return error(QString("Unable to load the test file '%1'").arg(TEST_FILE_XSD_BASE));
    }
    Regola *reference = NULL ;
    bool testResult = false;
    if(openXsdViewer(&mainWindow) ) {
        QString errMsgLoadData ;
        reference = Regola::loadFromFile(FILE_TESTS_DATA, &errMsgLoadData);
        if( NULL == reference ) {
            error(QString("Unable to load reference data, error:%1").arg(errMsgLoadData));
        } else {
            testResult = execCopyDefTests(reference);
        }
    }
    deleteEditor();
    if( NULL != reference ) {
        delete reference ;
    }
    return testResult ;
}

//-------------------------------------------------------------------------------------------------

bool TestXSDCopy::openXsdViewer(MainWindow *wnd)
{
    xsdEditor = new TestXSDWindow(wnd->appData(), wnd) ;
    if( NULL == xsdEditor ) {
        return error("Unable to open xsd editor");
    }
    QString xmlAsString = wnd->getRegola()->getAsText();
    //xsdEditor->show();
    xsdEditor->setFileName("test");
    xsdEditor->setTitle("test");
    xsdEditor->loadStringImmediate(xmlAsString);
    if( xsdEditor->isInError() ) {
        error("data not loaded inside the xsd editor");
        return false;
    }
    return true;
}

QString TestXSDCopy::applyResults(XSchemaObject *object)
{
    QApplication::clipboard()->setText("*");
    xsdEditor->testCopyElementActionExecute(object);
    QString result = QApplication::clipboard()->text();
    return result;
}

//--------------------------------------------------------------------------------------------------
bool TestXSDCopy::checkResults(const QString &result, QDomDocument domDocument, const QString &attrName )
{
    QDomDocument resDom;
    if(!resDom.setContent(result)) {
        return error(QString("Unable to transform result into xml for attribute '%2', data are:'%1'").arg(result).arg(attrName));
    }

    CompareXML compare;
    if( !compare.compareDomDocuments(resDom, domDocument)) {
        return error(QString("result dom differs from file %1 cause:%2").arg(result).arg(compare.errorString()));
    }
    return true;
}

Element *TestXSDCopy::findCompareElement(Regola *regola, const QString &value)
{
    return findCompareElement(regola->root(), value);
}

Element *TestXSDCopy::findCompareElement(Element *element, const QString &value)
{
    if( element->getAttributeValue("test") == value) {
        // get the first child;
        foreach( Element *child, *element->getChildItems()) {
            if( child->isElement() && (child->tag() == value ) ) {
                return child;
            }
        }
        return NULL;
    }
    foreach( Element *child, *element->getChildItems()) {
        Element *foundEl = findCompareElement(child, value);
        if( NULL != foundEl) {
            return foundEl;
        }
    }
    return NULL;
}

//--------------------------------------------------------------------------------------------------
bool TestXSDCopy::execCopyDefTests(Regola *reference)
{
    int i = 0 ;
    Element *root = reference->root();
    if( NULL != root ) {
        foreach( Element *el, *root->getChildItems() ) {
            if( el->isElement() ) {
                QString testName = el->getAttributeValue("test");
                if( testName.isEmpty()) {
                    return error(QString("test n. %1 is not found").arg(i));
                }
                if( !singleTest( i, testName, "test:a", testName, true, reference ) ) {
                    return false;
                }
                i++;
            }
        }
    }

    if( singleTest( i, "thisTestMustFail", "", "", true, reference ) ) {
        return error("execCopyDefTests proof failed.");
    }
    if( i == 0 ) {
        return error("No test found.");
    }
    return true;
}

//--------------------------------------------------------------------------------------------------

bool TestXSDCopy::hasTestAttr(XSchemaObject *object, const QString &attrName, const QString &attrValue)
{
    if( object->hasOtherAttribute(attrName) ) {
        if( object->getOtherAttribute(attrName) == attrValue ) {
            return true;
        }
    }
    return False();
}

XSchemaElement *TestXSDCopy::getTestElement( XSchemaObject *parentObject, const QString &attrName, const QString &attrValue)
{
    foreach(XSchemaObject *child, parentObject->getChildren()) {
        if( ( child->getType() == SchemaTypeElement ) && hasTestAttr(child, attrName, attrValue) ) {
            XSchemaElement *el = static_cast<XSchemaElement*>(child);
            return el;
        }
        XSchemaElement *childRes = getTestElement( child, attrName, attrValue );
        if( NULL != childRes ) {
            return childRes ;
        }
    }
    return NULL ;
}

XSchemaObject *TestXSDCopy::getTestObject( XSchemaObject *parentObject, const QString &attrName, const QString &attrValue)
{
    return parentObject->findChildWithOtherAttributeHavingValue( attrName, attrValue);
}


bool TestXSDCopy::singleTest( const int id, const QString &newTestName, const QString &attrName, const QString &testTag, const bool isInnerTest, Regola *reference)
{
    _testName = QString("%1 %2").arg(newTestName).arg(id) ;
    XSchemaElement *element = getTestElement( xsdEditor->schema(), attrName, testTag);
    if( NULL == element ) {
        return error(QString("Unable to find element for attribute:'%1'").arg(attrName));
    }
    QString resultXMLString ;
    if( isInnerTest) {
        resultXMLString = element->xmlRepresentation();
    } else {
        resultXMLString = applyResults(element);
    }
    // make dom document
    Element *celement = findCompareElement(reference, testTag);
    if( NULL == celement ) {
        return error(QString("unable to find the reference element: '%1'").arg(testTag));
    }
    QDomDocument referenceDocument;
    if(!celement->generateDom(referenceDocument, referenceDocument)) {
        return error("Unable to generate DOM for reference element");
    }

    if(!checkResults(resultXMLString, referenceDocument, attrName)) {
        return false;
    }
    return true;
}


//--------------------------------------------------------------------------------------------------------------

/*
TODO

elemento . ref a elem. ref tipo, darice. inner complex. inner restriz,inner estens.
tipo ref. semplice. complex, complex restriz, complez estens
*/


bool TestXSDCopy::testCopyFacets()
{
    _testName = "testCopyFacets";

    if(!testCopyFacetsInt()) {
        return false;
    }
    return true;
}

bool TestXSDCopy::testCopyFacetsInt()
{
    App app;
    if(!app.initNoWindow() ) {
        return error("init");
    }
    MainWindow mainWindow(false, app.data());
    if( !mainWindow.loadFile(QString(TEST_FILE_XSD_BASE)) ) {
        return error(QString("Unable to load the test file '%1'").arg(TEST_FILE_XSD_BASE));
    }
    Regola *reference = NULL ;
    bool testResult = false;
    if(openXsdViewer(&mainWindow) ) {
        QString errMsgLoadData ;
        reference = Regola::loadFromFile(FILE_TESTS_DATA_FACETS, &errMsgLoadData);
        if( NULL == reference ) {
            error(QString("Unable to load reference data, error:%1").arg(errMsgLoadData));
        } else {
            testResult = execCopyDefTestsFacets(reference);
        }
    }
    deleteEditor();
    if( NULL != reference ) {
        delete reference ;
    }
    return testResult ;
}


bool TestXSDCopy::execCopyDefTestsFacets(Regola *reference)
{
    int i = 0 ;
    Element *root = reference->root();
    if( NULL != root ) {
        foreach( Element *el, *root->getChildItems() ) {
            if( el->isElement() ) {
                QString testName = el->getAttributeValue("test");
                if( testName.isEmpty()) {
                    return error(QString("test n. %1 is not found").arg(i));
                }
                if( !singleTestFacets( i, testName, "test:b", testName, reference ) ) {
                    return false;
                }
                i++;
            }
        }
    }

    if( singleTest( i, "thisTestMustFail", "", "", true, reference ) ) {
        return error("execCopyDefTestsFacets proof failed.");
    }
    if( i == 0 ) {
        return error("No test found.");
    }
    return true;
}

bool TestXSDCopy::singleTestFacets( const int id, const QString &newTestName, const QString &attrName, const QString &testTag, Regola *reference)
{
    _testName = QString("%1 %2").arg(newTestName).arg(id) ;
    XSchemaObject *object = getTestObject( xsdEditor->schema(), attrName, testTag);
    if( NULL == object ) {
        return error(QString("Unable to find element for attribute:'%1' value:'%2'").arg(attrName).arg(testTag));
    }
    if( object->getType() != SchemaTypeSimpleTypeRestriction ) {
        return error(QString("Expecting SimpleTypeRestriction, but found '%2' for attribute:'%1'").arg(attrName).arg(object->getType()));
    }

    QString resultString ;
    XSchemaSimpleTypeRestriction *restr = (XSchemaSimpleTypeRestriction*)object ;
    resultString = restr->exportFacets();

    Element *celement = findCompareElement(reference, testTag);
    if( NULL == celement ) {
        return error(QString("unable to find the reference element: '%1'").arg(testTag));
    }
    QString referenceText = celement->getAsSimpleText(false);
    // strip cr and spaces?

    if(referenceText != resultString ) {
        return error(QString("text values differ: for attribute '%1', expected:'%2', found:'%3'").arg(attrName).arg(referenceText).arg(resultString));
    }
    return true;
}
