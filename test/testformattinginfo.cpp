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
#include "applicationdata.h"
#include "modules/metadata/metadatainfo.h"
#include "comparexml.h"
#include "modules/xml/xmlindentationdialog.h"

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

#define FILE_WRITE_SOURCE_0 BASE_TEST "write_source_0.xml"
#define WRITE_REAL_0 BASE_TEST "write_real_0.xml"
#define WRITE_REAL_1 BASE_TEST "write_real_1.xml"
#define WRITE_REAL_2 BASE_TEST "write_real_2.xml"
#define WRITE_REAL_3 BASE_TEST "write_real_3.xml"
#define WRITE_REAL_4 BASE_TEST "write_real_4.xml"
#define END_2_END_IN_NO BASE_TEST "end2end_in_no.xml"
#define END_2_END_OUT_NO BASE_TEST "end2end_out_no.xml"
#define END_2_END_OUT_NO_NO BASE_TEST "end2end_out_no_no.xml"
#define END_2_END_IN_YES BASE_TEST "end2end_in_yes.xml"
#define END_2_END_OUT_YES BASE_TEST "end2end_out_yes.xml"

#define FILE_NEW_0  BASE_TEST "new0.xml"
#define FILE_NEW_1  BASE_TEST "new1.xml"

TestFormattingInfo::TestFormattingInfo()
{
}

TestFormattingInfo::~TestFormattingInfo()
{
}

bool TestFormattingInfo::testFast()
{
    return innerTestNew("1", true, FILE_NEW_1);
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
    _subTestName = "" ;
    return true;
}

bool TestFormattingInfo::testSetReset()
{
    _testName = "testSetReset";
    if(!testNew()) {
        return false;
    }
    if(!testInsertDirective()) {
        return false;
    }
    if(!testRemoveDirective()) {
        return false;
    }
    if(!testPropertiesDirective()) {
        return false;
    }
    _subTestName = "" ;
    return true;
}

bool TestFormattingInfo::testWrite()
{
    _testName = "testWrite";
    if(!testWriteUnit()) {
        return false;
    }
    if(!testWriteReal()) {
        return false;
    }
    if(!testEnd2End()) {
        return false;
    }
    _subTestName = "" ;
    return true;
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
                if(useIndent!=regola->hasIndentation()) {
                    return error(QString("Config: %1, file '%2' found use indentation: %3").arg(value).arg(fileName).arg(regola->overrideGlobalIndentation()));
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
    _testName = "loadAllConfigurations";

    if(!testBaseLoad(FILE_READ_CFG0, true, false, -1, Regola::SaveAttributesNoSort, QXmlEditData::AttributesIndentationNone, 0 )) {
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


bool TestFormattingInfo::testUnitWrite(const QString &id, const QString &expected,
                                       const bool useIndent, const int indent,
                                       const Regola::ESaveAttributes saveAttributesMethod,
                                       const QXmlEditData::EIndentAttributes indentAttributesSetting, const int indentAttributesColumns)

{
    QString calculated;
    MetadataInfo info;
    XMLIndentationSettings settings;
    //--
    settings.setup( useIndent, indent, saveAttributesMethod, indentAttributesSetting, indentAttributesColumns);
    //--
    calculated = info.toFormatInfo(&settings);
    if(calculated.trimmed() != expected.trimmed()) {
        return error(QString("Write test id:%1 found:'%2' expected:'%3'").arg(id).arg(calculated).arg(expected));
    }
    return true;
}

bool TestFormattingInfo::testWriteUnit()
{
    _testName = "testWriteUnit";

    {
        const QString expected = "type=\"formatting\" indent=\"off\" indentValue=\"0\" sortAlphaAttr=\"off\" attrLineLen=\"off\"";
        if(!testUnitWrite("0", expected, false, 0, Regola::SaveAttributesNoSort, QXmlEditData::AttributesIndentationNone, 0 )) {
            return false;
        }
    }
    {
        const QString expected = "type=\"formatting\" indent=\"on\" indentValue=\"0\" sortAlphaAttr=\"off\" attrLineLen=\"off\"";
        if(!testUnitWrite("1", expected, true, 0, Regola::SaveAttributesNoSort, QXmlEditData::AttributesIndentationNone, 0 )) {
            return false;
        }
    }
    {
        const QString expected = "type=\"formatting\" indent=\"on\" indentValue=\"2\" sortAlphaAttr=\"off\" attrLineLen=\"off\"";
        if(!testUnitWrite("2", expected, true, 2, Regola::SaveAttributesNoSort, QXmlEditData::AttributesIndentationNone, 0 )) {
            return false;
        }
    }
    {
        const QString expected = "type=\"formatting\" indent=\"on\" indentValue=\"2\" sortAlphaAttr=\"on\" attrLineLen=\"off\"";
        if(!testUnitWrite("3", expected, true, 2, Regola::SaveAttributesSortingAlphabetically, QXmlEditData::AttributesIndentationNone, 0 )) {
            return false;
        }
    }
    {
        const QString expected = "type=\"formatting\" indent=\"on\" indentValue=\"2\" sortAlphaAttr=\"on\" attrLineLen=\"44\"";
        if(!testUnitWrite("4", expected, true, 2, Regola::SaveAttributesSortingAlphabetically, QXmlEditData::AttributesIndentationMaxCols, 44 )) {
            return false;
        }
    }

    return true;
}

/*
2 test: uno per set in regola -> in output.
        due per verificare il comportamento con i valori.
*/

void TestFormattingInfo::setupRegola( Regola *regola,
                                      const bool useIndent, const int indent,
                                      const Regola::ESaveAttributes saveAttributesMethod,
                                      const QXmlEditData::EIndentAttributes indentAttributesSetting, const int indentAttributesColumns)
{
    XMLIndentationSettings settings;
    //--
    settings.setup(useIndent, indent, saveAttributesMethod, indentAttributesSetting, indentAttributesColumns );
    settings.useFormatting = true ;
    //--
    regola->applyFormatting(&settings);
}


void TestFormattingInfo::setupIndentSettings(ApplicationData *data, const bool useIndent)
{
    data->setXmlIndent(useIndent?100:-1);
    data->setXmlIndentAttributes(1000);
    data->setXmlIndentAttributesType(QXmlEditData::AttributesIndentationNone);
}

void TestFormattingInfo::setupIndentSettingsDefault(ApplicationData *data)
{
    data->setXmlIndent(-1);
    data->setXmlIndentAttributes(0);
    data->setXmlIndentAttributesType(QXmlEditData::AttributesIndentationNone);
}

bool TestFormattingInfo::execRegolaWrite(const QString &id,
                                         const QString &fileReference,
                                         bool useIndent, const int indent,
                                         const Regola::ESaveAttributes saveAttributesMethod,
                                         const QXmlEditData::EIndentAttributes indentAttributesSetting, const int indentAttributesColumns)
{
    const QString fileStart = FILE_WRITE_SOURCE_0;
    App app;
    if(!app.init() ) {
        return error(QString("case %1 init window").arg(id));
    }
    setupIndentSettings(app.data(), !useIndent);
    app.data()->setFormattingInfoEnabled(true);
    MainWindow *window = app.mainWindow()->loadFileAndReturnWindow(fileStart);
    if( NULL == window ) {
        return error(QString("Case %1 opening test file: '%2'").arg(id).arg(fileStart));
    }
    Regola *regola = window->getRegola();
    setupRegola(regola, useIndent, indent, saveAttributesMethod, indentAttributesSetting, indentAttributesColumns);
    QString result = regola->getAsText();
    // Compare results.
    QString reference ;
    if(!readFromFile(fileReference, reference) ) {
        return error(QString("case %2 unable to load reference file: '%1' ").arg(fileReference).arg(id));
    }
    // normalize cr
    reference = reference.replace("\r\n", "\n");
    result = result.replace("\r\n", "\n");

    ///-----

    if( reference.trimmed() != result.trimmed() ) {
        return error(QString("Case %5 String not expected.\nExpected:'%1'\n%2\nString:%3\n'%4'\n")
                     .arg(reference.length())
                     .arg(reference.replace("\n", "*LF*"))
                     .arg(result.length())
                     .arg(result.replace("\n", "*LF*"))
                     .arg(id));
    }
    return true;
}

bool TestFormattingInfo::testWriteReal()
{
    _testName = "testWriteReal";
    if(!execRegolaWrite("0", WRITE_REAL_0, false, 0, Regola::SaveAttributesNoSort, QXmlEditData::AttributesIndentationNone, 0 )) {
        return false;
    }
    if(!execRegolaWrite("1", WRITE_REAL_1, true, 0, Regola::SaveAttributesNoSort, QXmlEditData::AttributesIndentationNone, 0 )) {
        return false;
    }
    if(!execRegolaWrite("2", WRITE_REAL_2, true, 2, Regola::SaveAttributesNoSort, QXmlEditData::AttributesIndentationNone, 0 )) {
        return false;
    }
    if(!execRegolaWrite("3", WRITE_REAL_3, true, 2, Regola::SaveAttributesSortingAlphabetically, QXmlEditData::AttributesIndentationNone, 0 )) {
        return false;
    }
    if(!execRegolaWrite("4", WRITE_REAL_4, true, 2, Regola::SaveAttributesSortingAlphabetically, QXmlEditData::AttributesIndentationMaxCols, 1 )) {
        return false;
    }
    return true ;
}

bool TestFormattingInfo::testEnd2EndInner(const QString &id,
                                          const bool isEnabled,
                                          const QString &fileInput,
                                          const QString &fileReference)
{
    App app;
    if(!app.init() ) {
        return error(QString("case %1 init window").arg(id));
    }
    setupIndentSettings(app.data(), false);
    app.data()->setFormattingInfoEnabled(isEnabled);
    MainWindow *window = app.mainWindow()->loadFileAndReturnWindow(fileInput);
    if( NULL == window ) {
        return error(QString("Case %1 opening test file: '%2'").arg(id).arg(fileInput));
    }
    QString result = window->getRegola()->getAsText();
    // Compare results.
    QString reference ;
    if(!readFromFile(fileReference, reference) ) {
        return error(QString("case %2 unable to load reference file: '%1' ").arg(fileReference).arg(id));
    }
    // normalize cr
    reference = reference.replace("\r\n", "\n");
    result = result.replace("\r\n", "\n");

    ///-----

    if( reference.trimmed() != result.trimmed() ) {
        return error(QString("Case %5 String not expected.\nExpected:%1\n%2\nString:%3\n%4\n")
                     .arg(reference.length())
                     .arg(reference.replace("\n", "*LF*"))
                     .arg(result.length())
                     .arg(result.replace("\n", "*LF*"))
                     .arg(id));
    }
    return true;
}

bool TestFormattingInfo::testEnd2End()
{
    _testName = "testEnd2End";
    if(!testEnd2EndInner("0", false, END_2_END_IN_NO, END_2_END_OUT_NO_NO)) {
        return false;
    }
    if(!testEnd2EndInner("1", true, END_2_END_IN_NO, END_2_END_OUT_NO_NO)) {
        return false;
    }
    if(!testEnd2EndInner("2", false, END_2_END_IN_YES, END_2_END_OUT_NO)) {
        return false;
    }
    if(!testEnd2EndInner("3", true, END_2_END_IN_YES, END_2_END_OUT_YES)) {
        return false;
    }
    return true ;
}

bool TestFormattingInfo::innerTestNew(const QString &id,
                                      const bool isNewOption,
                                      const QString &fileReference)
{
    App app;
    if(!app.init() ) {
        return error(QString("case %1 init window").arg(id));
    }
    app.data()->setAutoInsertProlog(true);
    app.data()->setFormattingInfoInsertOnNew(isNewOption);
    setupIndentSettingsDefault(app.data());
    MainWindow *window = app.mainWindow()->execNew();
    if( NULL == window ) {
        return error(QString("Case %1 opening creating new.").arg(id));
    }
    Regola *regola = window->getRegola();
    Element * element = new Element("root", "", regola, NULL);
    regola->insertElementComplete(element, NULL, window->getEditor()->getMainTreeWidget());
    CompareXML compare;
    if(!compare.compareFileWithRegola(regola, fileReference)) {
        return error(QString("Case %1 compare failed:\n%2\n")
                     .arg(id)
                     .arg(compare.errorString()));
    }
    return true;
}

bool TestFormattingInfo::testNew()
{
    _testName = "testNew";

    if(!innerTestNew("0", false, FILE_NEW_0)) {
        return false;
    }
    if(!innerTestNew("1", true, FILE_NEW_1)) {
        return false;
    }
    return true ;
}

bool TestFormattingInfo::actOnDirective(const QString &id,
                                        const bool isInsertOrRemove,
                                        const bool expectedAfterUndo,
                                        const QString &fileInput,
                                        const QString &fileReference)
{
    bool expectedRedo = isInsertOrRemove;

    App app;
    if(!app.init() ) {
        return error(QString("case %1 init window").arg(id));
    }
    setupIndentSettings(app.data(), false);
    MainWindow *window = app.mainWindow()->loadFileAndReturnWindow(fileInput);
    if( NULL == window ) {
        return error(QString("Case %1 opening test file: '%2'").arg(id).arg(fileInput));
    }
    CompareXML compare;
    if(isInsertOrRemove) {
        app.mainWindow()->getEditor()->addFormattingInfo();
    } else {
        app.mainWindow()->getEditor()->removeFormattingInfo();
    }
    if(!compare.compareAFileWithRegola(fileReference, app.mainWindow()->getRegola())) {
        return error(QString("Redo 1: id:%1 %2").arg(id).arg(compare.errorString()));
    }
    if(expectedRedo != app.mainWindow()->getRegola()->hasFormattingInfo()) {
        return error(QString("Redo 1: NO INFO id:%1").arg(id));
    }
    app.mainWindow()->getRegola()->undo();
    if(!compare.compareAFileWithRegola(fileInput, app.mainWindow()->getRegola())) {
        return error(QString("Redo 1: id:%1 %2").arg(id).arg(compare.errorString()));
    }
    if(expectedAfterUndo != app.mainWindow()->getRegola()->hasFormattingInfo()) {
        return error(QString("Undo 1: INFO id:%1").arg(id));
    }
    app.mainWindow()->getRegola()->redo();
    if(!compare.compareAFileWithRegola(fileReference, app.mainWindow()->getRegola())) {
        return error(QString("Redo 2: id:%1 %2").arg(id).arg(compare.errorString()));
    }
    if(expectedRedo != app.mainWindow()->getRegola()->hasFormattingInfo()) {
        return error(QString("Redo 2: NO INFO id:%1").arg(id));
    }
    app.mainWindow()->getRegola()->undo();
    if(!compare.compareAFileWithRegola(fileInput, app.mainWindow()->getRegola())) {
        return error(QString("Redo 2: id:%1 %2").arg(id).arg(compare.errorString()));
    }
    if(expectedAfterUndo != app.mainWindow()->getRegola()->hasFormattingInfo()) {
        return error(QString("Undo 2: INFO id:%1").arg(id));
    }
    return true ;
}

bool TestFormattingInfo::testInsertDirective()
{
    _testName = "testInsertDirective";
    if(!actOnDirective("0", true, false, FILE_NEW_0, FILE_NEW_1 ) ) {
        return false;
    }
    if(!actOnDirective("1", true, true, FILE_NEW_1, FILE_NEW_1 ) ) {
        return false;
    }
    return true ;
}

bool TestFormattingInfo::testRemoveDirective()
{
    _testName = "testRemoveDirective";
    if(!actOnDirective("0", false, false, FILE_NEW_0, FILE_NEW_0 ) ) {
        return false;
    }
    if(!actOnDirective("1", false, true, FILE_NEW_1, FILE_NEW_0 ) ) {
        return false;
    }
    return true ;
}

bool TestFormattingInfo::testPropertiesDirective()
{
    _testName = "testPropertiesDirective" ;
    if(!testOption(false, false)) {
        return false;
    }
    if(!testOption(true, true)) {
        return false;
    }
    if(!testOption(true, false)) {
        return false;
    }
    if(!testOption(false, true)) {
        return false;
    }
    return true;
}

bool TestFormattingInfo::testOption(const bool setBefore, const bool expected)
{
    _subTestName = "testOption" ;
    // set from dialog
    App app;
    if(!app.init()) {
        return error("app init");
    }
    Regola *regola = app.mainWindow()->getRegola();
    regola->setFormattingInfo(setBefore);
    XmlIndentationDialog dialog(app.mainWindow(), app.mainWindow()->getRegola(), app.data());
    QCheckBox *cb = dialog.findChild<QCheckBox*>("cbFormattingInfo");
    if( NULL == cb ) {
        return error("Check 1 null");
    }
    cb->setChecked(expected);
    dialog.doAccept();
    if( regola->hasFormattingInfo() != expected ) {
        return error(QString("For %1 option, expecting %2, but found:%3").arg(setBefore).arg(expected).arg(regola->hasFormattingInfo()));
    }
    return true ;
}
