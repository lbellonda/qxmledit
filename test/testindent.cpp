/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2016 by Luca Bellonda and individual contributors  *
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

#include "testindent.h"
#include "app.h"
#include "comparexml.h"
#include "modules/xml/xmlindentationdialog.h"
#include "configurationdialog.h"

#include <QTextCodec>
#include <QBuffer>


#define FILE_TEST "../test/data/indent.xml"
#define FILE_TEST_BASE "../test/data/xml/indent"


/***********************************************
 * test cases:

 Use      |        |        |      |
 custom   | App    | Doc    | Case |
 settings | indent | indent |  #   |
 -----------------------------------
          | Y:3    |  Y:2   |   1  |
  N       | Y:1    |   n    |   2  |
          |  n     |   n    |   3  |
          |  n     |  Y:2   |   4  |
 -----------------------------------
          |  n     |  Y:3   |   5  |
  Y       |  n     |   n    |   6  |
          |  y:2   |  Y:3   |   7  |
          |  y:2   |   n    |   8  |
------------------------------------

***********************************************/

TestIndent::TestIndent()
{
    _indentationTest1 = 0 ;
    _useDocIndent = false ;
    _indentationDoc = 0 ;
}

TestIndent::~TestIndent()
{
}

bool TestIndent::testFast()
{
    if( !testIndentAttributesWithDefaultValuesInner(0)) {
        return false;
    }
    return testSettingsFromPreferences();
}

bool TestIndent::testSaving()
{
    _testName = "testSaving" ;
    for( int indentation = -1 ; indentation < 16 ; indentation ++ ) {
        _indentationTest1 = indentation ;
       if( !saveAndCompare(QString("testSaving %1").arg(indentation), indentation, indentation, &TestIndent::simpleWrite)) {
           return false;
       }
    }
    return true;
}

bool TestIndent::testSettings()
{
    _testName = "testSettings" ;
    if( !testWithSettings(1, 3, 3, false, 2 ) ) {
        return false;
    }
    if( !testWithSettings(2, 1, 1, false, -1 ) ) {
        return false;
    }
    if( !testWithSettings(3, -1, -1, false, -1 ) ) {
        return false;
    }
    if( !testWithSettings(4, -1, -1, false, 2 ) ) {
        return false;
    }
    if( !testWithSettings(5, 3, -1, true, 3 ) ) {
        return false;
    }
    if( !testWithSettings(6, -1, -1, true, -1 ) ) {
        return false;
    }
    if( !testWithSettings(7, 3, 2, true, 3 ) ) {
        return false;
    }
    if( !testWithSettings(8, -1, 2, true, -1 ) ) {
        return false;
    }
    return true;
}

bool TestIndent::simpleWrite(App *app, QBuffer *outData)
{
    Regola *regola = app->mainWindow()->getRegola();
    regola->setIndentation(_indentationTest1);
    if(!regola->write(outData, true)) {
        return error(QString("writing data indent: '%1'").arg(_indentationTest1));
    }
    return true;
}


bool TestIndent::writeSettings(App *app, QBuffer *outData)
{
    Regola *regola = app->mainWindow()->getRegola();
    if(_useDocIndent) {
        regola->setUseIndentation(true);
        regola->setIndentationForce(_indentationDoc);
    }
    regola->setIndentation(app->data()->xmlIndent());
    if(!regola->write(outData, true)) {
        return error(QString("Writing data complex."));
    }
    return true;
}

bool TestIndent::testWithSettings(const int caseNo, const int indentation, const int appIndent, const bool useDocIndent, const int docIndent )
{
    _useDocIndent = useDocIndent ;
    _indentationDoc = docIndent ;
    return saveAndCompare(QString("Settings #%1").arg(caseNo), appIndent, indentation, &TestIndent::writeSettings) ;
}

bool TestIndent::saveAndCompare(const QString &caseId, const int appIndentation, const int indentation, bool (TestIndent::*func) (App *app, QBuffer *outData) )
{
    _testName = caseId ;
    App app;
    if(!app.init()) {
        return error("app init");
    }
    app.data()->setXmlIndent(appIndentation);
    if( !app.mainWindow()->loadFile(QString(FILE_TEST)) ) {
        return error(QString("Opening test file: '%1' for indent:%2").arg(FILE_TEST).arg(indentation));
    }

    QBuffer outData ;
    if(!(this->*func)(&app, &outData)) {
        return false;
    }
    /*
    Regola *regola = app.mainWindow()->getRegola();
    regola->setIndent(indentation);
    if(!regola->write(&outData, true)) {
        return error(QString("writing data indent: '%1'").arg(indentation));
    }
    */
    // build compare
    QString compare;
    compare.append("<a>") ;
    if( indentation >= 0 ) {
        compare.append("\n") ;
        for( int i = 0 ; i < indentation ; i++ ) {
            compare.append(" ");
        }
    }
    compare.append("<b>") ;
    if( indentation >= 0 ) {
        compare.append("\n") ;
        for( int i = 0 ; i < 2*indentation ; i++ ) {
            compare.append(" ");
        }
    }
    compare.append("<c/>") ;
    if( indentation >= 0 ) {
        compare.append("\n") ;
        for( int i = 0 ; i < indentation ; i++ ) {
            compare.append(" ");
        }
    }
    compare.append("</b>") ;
    if( indentation >= 0 ) {
        compare.append("\n") ;
    }
    compare.append("</a>") ;
    if( indentation >= 0 ) {
        compare.append("\n") ;
    }
    QBuffer compareBuffer;
    compareBuffer.open(QIODevice::WriteOnly);
    QTextStream stream(&compareBuffer);
    stream.setCodec(QTextCodec::codecForName("UTF-8"));
    stream << compare ;
    stream.flush();
    compareBuffer.close();

    // compare as xml
    CompareXML compareXml ;
    if(!compareXml.compareBufferWithFile(&compareBuffer, FILE_TEST) ) {
        return error(QString("Comparing ref Data for indent %1, details:%2").arg(indentation).arg(compareXml.errorString()) );
    }
    if(!compareXml.compareBufferWithFile(&outData, FILE_TEST) ) {
        return error(QString("Comparing out Data for indent %1, details:%2").arg(indentation).arg(compareXml.errorString()) );
    }
    QString compRef, compRes;
    if( !loadIOAsString(&compareBuffer, "UTF-8", &compRef) ) {
        return error(QString("Loading res1 for indent %1").arg(indentation) );
    }
    if( !loadIOAsString(&outData, "UTF-8", &compRes) ) {
        return error(QString("Loading res2 for indent %1").arg(indentation) );
    }
    if( compRef != compRes ) {
        return error(QString("Comparing for indent %1, refLen=%2, resLen=%3, res='%4', ref='%5'")
                     .arg(indentation).arg(compRef.length()).arg(compRes.length()).arg(compRes).arg(compRef) );
    }

    return true;
}

/**************************************************
 * Verify set value and check default.
 **************************************************
 * verify default values without setting anything
 **************************************************
 * - set default
 * - create object
 * - save
 * - verify
 *
 * 0, 1, 10, 20
 *
 * set default 1
 * set value != from default
 * repeat 0, 1, 10, 20
 *
 * use indent
 *  no indent
 *  indent 0
 *  indent 1
 *  indent 4
****************************************************/

bool TestIndent::testIndentAttributes()
{
    _testName = "testIndentAttributes";
    if(!testIndentAttributesUnit()) {
        return false;
    }
    if(!testIndentAttributesWithDefaultValues()) {
        return false;
    }
    if(!testIndentAttributesWithoutDefaultValues()) {
        return false;
    }
    if(!testIndentAttributesIndent()) {
        return false;
    }
    if(!testSettingsFromDialog()) {
        return false;
    }
    if(!testSettingsFromPreferences()) {
        return false;
    }
    return true;
}

bool TestIndent::testIndentAttributesWithDefaultValues()
{
    if( !testIndentAttributesWithDefaultValuesInner(0)) {
        return false;
    }
    if( !testIndentAttributesWithDefaultValuesInner(10)) {
        return false;
    }
    if( !testIndentAttributesWithDefaultValuesInner(20)) {
        return false;
    }
    return true;
}

bool TestIndent::testIndentAttributesWithoutDefaultValues()
{
    if( !testIndentAttributesWithoutDefaultValuesInner(0)) {
        return false;
    }
    if( !testIndentAttributesWithoutDefaultValuesInner(10)) {
        return false;
    }
    if( !testIndentAttributesWithoutDefaultValuesInner(20)) {
        return false;
    }
    return true;
}

bool TestIndent::testIndentAttributesIndent()
{
    if( !testIndentAttributesIndentInner( -1, 4)) {
        return false;
    }
    if( !testIndentAttributesIndentInner( 0, 1)) {
        return false;
    }
    if( !testIndentAttributesIndentInner( 1, 1)) {
        return false;
    }
    if( !testIndentAttributesIndentInner(10, 10)) {
        return false;
    }
    return true ;
}

//--------------------------------------------------------
#define INDENT_INPUT0  FILE_TEST_BASE "/input0.xml"
#define INDENT_RESULTS_ATTRINDENT  FILE_TEST_BASE "/withDefault"
#define INDENT_RESULTS_INDENT  FILE_TEST_BASE "/indentAttr"
#define INDENT_INPUT0_INDENTED FILE_TEST_BASE "/outIndentedNoA2.xml"


bool TestIndent::testIndentAttributesWithDefaultValuesInner(const int indentValue)
{
    QString caseId = QString("testIndentAttributesWithDefaultValuesInner/%1").arg(indentValue);
    QString resultFile = QString("%1%2.xml").arg(INDENT_RESULTS_ATTRINDENT).arg(indentValue);
    if(!saveAndCompareAttributesIndentation(caseId,
                                 //--
                                 2, 2, false,
                                 //--
                                 true, true, indentValue,
                                 false, 0,
                                 // --
                                 resultFile ) ) {
        return false;
    }
    if(!saveAndCompareAttributesIndentation(caseId,
                                 //--
                                 2, 2, false,
                                 //--
                                 false, true, indentValue,
                                 false, 0,
                                 // --
                                 INDENT_INPUT0_INDENTED ) ) {
        return false;
    }
    return true;
}

bool TestIndent::testIndentAttributesWithoutDefaultValuesInner(const int indentValue)
{
    QString caseId = QString("testIndentAttributesWithoutDefaultValuesInner/%1").arg(indentValue);
    QString resultFile = QString("%1%2.xml").arg(INDENT_RESULTS_ATTRINDENT).arg(indentValue);
    if( !saveAndCompareAttributesIndentation(caseId,
                                 //--
                                 2, 2, false,
                                 //--
                                 true, false, indentValue,
                                 true, indentValue,
                                 // --
                                 resultFile ) ) {
        return false;
    }
    if( !saveAndCompareAttributesIndentation(caseId,
                                 //--
                                 2, 2, false,
                                 //--
                                 false, false, indentValue,
                                 true, indentValue,
                                 // --
                                 INDENT_INPUT0_INDENTED ) ) {
        return false;
    }
    return true ;
}

// use indentation on elements, use indentation on attributes
bool TestIndent::testIndentAttributesIndentInner(const int indentValue, const int attributeIndentation)
{
    QString caseId = QString("testIndentAttributesIndentInner/%1").arg(indentValue);
    QString resultFilei = QString("%1%2i.xml").arg(INDENT_RESULTS_INDENT).arg(indentValue);
    QString resultFileni = QString("%1%2ni.xml").arg(INDENT_RESULTS_INDENT).arg(indentValue);
    if( !saveAndCompareAttributesIndentation(caseId,
                                 //--
                                 indentValue, indentValue, true,
                                 //--
                                 true, false, attributeIndentation,
                                 true, attributeIndentation,
                                 // --
                                 resultFilei ) ) {
        return false;
    }
    if( !saveAndCompareAttributesIndentation(caseId,
                                 //--
                                 indentValue, indentValue, true,
                                 //--
                                 false, false, attributeIndentation,
                                 false, attributeIndentation,
                                 // --
                                 resultFileni ) ) {
        return false;
    }
    return true;
}

/*
static void dumpDataOnFile( const QString &fileName, const QByteArray &data)
{
    QFile f1(fileName);
    f1.open(QFile::WriteOnly);
    f1.write(data);
    f1.close();
}
*/
/*
static void dumpStringOnFile( const QString &fileName, const QString &data)
{
    dumpDataOnFile(fileName, data.toUtf8());
}*/

bool TestIndent::saveAndCompareAttributesIndentation(const QString &caseId,
                                                     //--
                                                     const int appIndentation,
                                                     const int specificIndentation,
                                                     const bool useSpecificIndentation,
                                                     //--
                                                     const bool useIndentation,
                                                     const bool setDefaultValues,
                                                     const int attributeIndentationDefaultValue,
                                                     const bool setSpecificValue,
                                                     const int attributeIndentationSpecificValue,
                                                     // --
                                                     const QString &fileReference )
{
    _testName = caseId ;
    App app;
    if(!app.init()) {
        return error("app init");
    }
    app.data()->setXmlIndent(appIndentation);
    if(setDefaultValues) {
        app.data()->setXmlIndentAttributes(attributeIndentationDefaultValue);
    }
    app.data()->setXmlIndentAttributesType(useIndentation?QXmlEditData::AttributesIndentationMaxCols:QXmlEditData::AttributesIndentationNone);
    if( !app.mainWindow()->loadFile(QString(INDENT_INPUT0)) ) {
        return error(QString("Opening test file: '%1' ").arg(INDENT_INPUT0));
    }
    Regola *regola = app.mainWindow()->getRegola();
    if( useSpecificIndentation) {
        regola->setIndentationForce(specificIndentation);
    }
    if(setSpecificValue) {
        regola->setIndentAttributesSettings(true,
                                            useIndentation?QXmlEditData::AttributesIndentationMaxCols:QXmlEditData::AttributesIndentationNone,
                                            attributeIndentationSpecificValue);
        regola->setUseXmlIndentAttributesSettings(true);
    }
    // sort attributes alpha to have a consistent behaviour
    regola->setSaveAttributesMethod(Regola::SaveAttributesSortingAlphabetically);
    //--
    QByteArray resultData = regola->writeMemory();
    QString regolaFromByteArray = QString::fromUtf8(resultData.data());
    QString regolaAsString = regola->getAsText();
    if( regolaFromByteArray != regolaAsString ) {
        //dumpDataOnFile("1.dat", resultData);
        //dumpStringOnFile("2.dat", resultData);
        return error(QString("String and ba differ.\nByteArray:\n%1\nString:\n%2\n").arg(regolaFromByteArray).arg(regolaAsString));
    }
    // Compare results.
    QString reference ;
    if(!readFromFile(fileReference, reference) ) {
        return error(QString("unable to load reference file: '%1' ").arg(fileReference));
    }
    // normalize cr
    reference = reference.replace("\r\n", "\n");
    regolaAsString = regolaAsString.replace("\r\n", "\n");

///-----
    //dumpStringOnFile("3.dat", regolaAsString);
    //dumpStringOnFile("4.dat", reference);
/// -----

    if( reference != regolaAsString ) {
        return error(QString("String not expected.\nExpected:%1\n%2\nString:%3\n%4\n")
                     .arg(reference.length())
                     .arg(reference)
                     .arg(regolaAsString.length())
                     .arg(regolaAsString));
    }
    return true;
}

bool TestIndent::testIndentAttributesUnit()
{
    _testName = "testIndentAttributesUnit" ;
     // --
    App app;
    if(!app.init()) {
        return error("app init");
    }
    // 1 - Use default: check
    if( !app.mainWindow()->loadFile(QString(INDENT_INPUT0)) ) {
        return error(QString("Opening test file: '%1' ").arg(INDENT_INPUT0));
    }
    if( app.mainWindow()->getRegola()->isUseXmlIndentAttributesSettings() ) {
        return error("After creation not using settings");
    }
    if( app.mainWindow()->getRegola()->xmlIndentAttributesColumns() != QXmlEditData::XmlIndentAttributesColumnsDefault ) {
        return error(QString("After creation expecting %1, found %2").arg(QXmlEditData::XmlIndentAttributesColumnsDefault)
                .arg(app.mainWindow()->getRegola()->xmlIndentAttributesColumns()));
    }
    const int newDefaultValue = QXmlEditData::XmlIndentAttributesColumnsDefault+10;
    app.data()->setXmlIndentAttributes(newDefaultValue);
    if( !app.mainWindow()->loadFile(QString(INDENT_INPUT0)) ) {
        return error(QString("Opening test file: '%1' ").arg(INDENT_INPUT0));
    }
    if( app.mainWindow()->getRegola()->isUseXmlIndentAttributesSettings() ) {
        return error("After creation 1 not using settings");
    }
    if( app.mainWindow()->getRegola()->xmlIndentAttributesColumns() != newDefaultValue ) {
        return error(QString("After creation 1 expecting %1, found %2").arg(newDefaultValue)
                .arg(app.mainWindow()->getRegola()->xmlIndentAttributesColumns()));
    }
    if( !app.mainWindow()->loadFile(QString(INDENT_INPUT0)) ) {
        return error(QString("Opening test file: '%1' ").arg(INDENT_INPUT0));
    }
    //
    const int newValueSpecific = QXmlEditData::XmlIndentAttributesColumnsDefault+21;
    //--
    app.mainWindow()->getRegola()->setUseXmlIndentAttributesSettings(true);
    app.mainWindow()->getRegola()->setIndentAttributesSettings(true, QXmlEditData::AttributesIndentationNone, newValueSpecific );
    if( !app.mainWindow()->getRegola()->isUseXmlIndentAttributesSettings() ) {
        return error("After set 0 not using specific");
    }
    if( app.mainWindow()->getRegola()->xmlIndentAttributesType() != QXmlEditData::AttributesIndentationNone ) {
        return error(QString("After set 0 type expecting %1, found %2").arg(QXmlEditData::AttributesIndentationNone).
                arg(app.mainWindow()->getRegola()->xmlIndentAttributesType()));
    }
    if( app.mainWindow()->getRegola()->xmlIndentAttributesColumns() != newValueSpecific ) {
        return error(QString("After set 0 expecting %1, found %2").arg(newValueSpecific)
                .arg(app.mainWindow()->getRegola()->xmlIndentAttributesColumns()));
    }
    //----
    if( !app.mainWindow()->loadFile(QString(INDENT_INPUT0)) ) {
        return error(QString("Opening test file: '%1' ").arg(INDENT_INPUT0));
    }
    app.mainWindow()->getRegola()->setUseXmlIndentAttributesSettings(true);
    app.mainWindow()->getRegola()->setIndentAttributesSettings(true, QXmlEditData::AttributesIndentationMaxCols, newValueSpecific );
    if( !app.mainWindow()->getRegola()->isUseXmlIndentAttributesSettings() ) {
        return error("After set 1 not using specific");
    }
    if( app.mainWindow()->getRegola()->xmlIndentAttributesType() != QXmlEditData::AttributesIndentationMaxCols ) {
        return error(QString("After set 1 type expecting %1, found %2").arg(QXmlEditData::AttributesIndentationMaxCols).
                arg(app.mainWindow()->getRegola()->xmlIndentAttributesType()));
    }
    if( app.mainWindow()->getRegola()->xmlIndentAttributesColumns() != newValueSpecific ) {
        return error(QString("After set 1 expecting %1, found %2").arg(newValueSpecific)
                .arg(app.mainWindow()->getRegola()->xmlIndentAttributesColumns()));
    }
    //--
    return true;
}

//----------

bool TestIndent::testSettingsFromDialog()
{
    _testName = "testSettingsFromDialog" ;
    // set from dialog
    App app;
    if(!app.init()) {
        return error("app init");
    }
    Regola *regola = app.mainWindow()->getRegola();
    {
        int currentValue = app.data()->xmlIndentAttributes();
        if( regola->xmlIndentAttributesColumns() != currentValue ) {
            return error(QString("Default value is:%1, found :%2").arg(currentValue).arg(regola->xmlIndentAttributesColumns()));
        }
        QXmlEditData::EIndentAttributes currentType = app.data()->xmlIndentAttributesType();
        if( regola->xmlIndentAttributesType() != currentType ) {
            return error(QString("Default type is:%1, found :%2").arg(currentType).arg(regola->xmlIndentAttributesType()));
        }
        currentValue += 5;
        currentType = (QXmlEditData::AttributesIndentationMaxCols==currentType)?QXmlEditData::AttributesIndentationNone:QXmlEditData::AttributesIndentationMaxCols;
        //
        app.data()->setXmlIndentAttributesType(currentType);
        app.data()->setXmlIndentAttributes(currentValue);
        //
        {
            XmlIndentationDialog dialog(app.mainWindow(), app.mainWindow()->getRegola(), app.data());
            QGroupBox *cb = dialog.findChild<QGroupBox*>("indentSettings");
            if( NULL == cb ) {
                return error("Check 1 null");
            }
            cb->setChecked(false);
            dialog.doAccept();
        }
        if( regola->xmlIndentAttributesColumns() != currentValue ) {
            return error(QString("Default 2 value is:%1, found :%2").arg(currentValue).arg(regola->xmlIndentAttributesColumns()));
        }
        if( regola->xmlIndentAttributesType() != currentType ) {
            return error(QString("Default 2 type is:%1, found :%2").arg(currentType).arg(regola->xmlIndentAttributesType()));
        }
        //---
        {
            XmlIndentationDialog dialog(app.mainWindow(), app.mainWindow()->getRegola(), app.data());
            QGroupBox *cb = dialog.findChild<QGroupBox*>("indentSettings");
            if( NULL == cb ) {
                return error("Check 2 null");
            }
            cb->setChecked(true);
            QString radioName = (QXmlEditData::AttributesIndentationMaxCols==currentType)?"attrNoIndendation":"attrNewLineAt";
            QRadioButton *r = dialog.findChild<QRadioButton*>(radioName);
            if( NULL == r ) {
                return error("radio 2 null");
            }
            r->setChecked(true);
            QSpinBox *s = dialog.findChild<QSpinBox*>("attrCharacters");
            if( NULL == s ) {
                return error("spin null");
            }
            s->setValue(currentValue+10);
            dialog.doAccept();
        }
        QXmlEditData::EIndentAttributes expectedType = (QXmlEditData::AttributesIndentationMaxCols==currentType)?QXmlEditData::AttributesIndentationNone:QXmlEditData::AttributesIndentationMaxCols;

        if( regola->xmlIndentAttributesColumns() != (currentValue+10) ) {
            return error(QString("value is:%1, found :%2").arg(currentValue+10).arg(regola->xmlIndentAttributesColumns()));
        }
        if( regola->xmlIndentAttributesType() != expectedType ) {
            return error(QString("type is:%1, found :%2").arg(expectedType).arg(regola->xmlIndentAttributesType()));
        }
    }
    return true ;
}


bool TestIndent::testSettingsFromPreferences()
{
    _testName = "testSettingsFromPreferences" ;
    // set from dialog
    App app;
    if(!app.init()) {
        return error("app init");
    }
    Regola *regola = app.mainWindow()->getRegola();
    {
        int currentValue = app.data()->xmlIndentAttributes();
        if( regola->xmlIndentAttributesColumns() != currentValue ) {
            return error(QString("Default value is:%1, found :%2").arg(currentValue).arg(regola->xmlIndentAttributesColumns()));
        }
        QXmlEditData::EIndentAttributes currentType = app.data()->xmlIndentAttributesType();
        if( regola->xmlIndentAttributesType() != currentType ) {
            return error(QString("Default type is:%1, found :%2").arg(currentType).arg(regola->xmlIndentAttributesType()));
        }

        //
        ConfigurationDialog dialog(app.data(), app.mainWindow());
        ConfigureXMLManagementDialog *ch1 = dialog._configureXmlManagement;

        QString radioName = (QXmlEditData::AttributesIndentationMaxCols==currentType)?"attrNoIndendation":"attrNewLineAt";
        QXmlEditData::EIndentAttributes expectedType = (QXmlEditData::AttributesIndentationMaxCols==currentType)?QXmlEditData::AttributesIndentationNone:QXmlEditData::AttributesIndentationMaxCols;
        QRadioButton *r = ch1->findChild<QRadioButton*>(radioName);
        if( NULL == r ) {
            return error("radio 2 null");
        }
        r->setChecked(true);
        QSpinBox *s = ch1->findChild<QSpinBox*>("attrCharacters");
        if( NULL == s ) {
            return error("spin null");
        }
        int expectedValue = currentValue+10 ;
        s->setValue(expectedValue);
        dialog.accept();

        //
        app.mainWindow()->updateAfterPreferences();
        regola = app.mainWindow()->getRegola();

        if( regola->xmlIndentAttributesColumns() != expectedValue ) {
            return error(QString("Default 2 value is:%1, found :%2").arg(expectedValue).arg(regola->xmlIndentAttributesColumns()));
        }
        if( regola->xmlIndentAttributesType() != expectedType ) {
            return error(QString("Default 2 type is:%1, found :%2").arg(expectedType).arg(regola->xmlIndentAttributesType()));
        }
    }
    return true ;
}
