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

#include "testexecxslt.h"
#include "modules/xslt/xsltexecutor.h"
#include "modules/messages/sourceerror.h"
#include "comparexml.h"
#include "app.h"
#include "StartParams.h"
#include "modules/xslt/xslparametermanager.h"
#include "modules/xslt/xsltexecdialog.h"
#include <QList>

#define TEST_BASE   TEST_BASE_DATA  "xslt/exec/"

#define FILE_INPUT_0    TEST_BASE  "input.xml"
#define FILE_XSL_0  TEST_BASE  "xslt.xsl"
#define FILE_OUTPUT_0   TEST_BASE  "expected.xml"

#define FILE_INPUT_2    TEST_BASE  "input.xml"
#define FILE_XSL_2  TEST_BASE  "xslt_encoding.xsl"
#define FILE_OUTPUT_2   TEST_BASE  "expected_encoding.xml"

extern int doXSL(ApplicationData *appData, StartParams *startParams);

TestExecXSLT::TestExecXSLT()
{
}

TestExecXSLT::~TestExecXSLT()
{
}

bool TestExecXSLT::testFast()
{
    return checkSaxonEngine();
}

bool TestExecXSLT::testUnit()
{
    _testName = "testUnit" ;
    if(!checkExecParameters()) {
        return false;
    }
    if(!checkComamndLine()) {
        return false;
    }

    if(!checkChooseEngine()) {
        return false;
    }

    if(!checkSaxonEngine()) {
        return false;
    }

    if(!checkSaxonEngineCfg()) {
        return false;
    }

    if(!checkExecRun()) {
        return false;
    }
    if(!checkHandleParameters()) {
        return false;
    }
    return true;
}

//---------------------------------------------------
/* check errors into messages
no source
no sheet
no output
*/

bool TestExecXSLT::checkExecParameters()
{
    _testName = "checkExecParameters" ;
    App app;
    if(!app.initNoWindow()) {
        return error("Init no window");
    }
    {
        app.data()->setUseSaxonXSL(false);
        MessagesOperationResult result;
        XSLTExecutor executor(app.data());
        if(executor.exec(result)) {
            return error("Expected error param 1");
        }
        QList<int> errorsToCheck;
        QList<int> errorsToAvoid;
        errorsToCheck << XSLTExecutor::ErrorSourceMissing << XSLTExecutor::ErrorXSLMissing << XSLTExecutor::ErrorOutputMissing ;
        errorsToAvoid << XSLTExecutor::ErrorXSLTInvalid ;
        if(!checkExecErrors("i1", result, errorsToCheck, errorsToAvoid)) {
            return false ;
        }
        app.data()->setUseSaxonXSL(true);
        MessagesOperationResult resulta;
        if(executor.exec(resulta)) {
            return error("Expected error param 1a");
        }
        if(!checkExecErrors("i1a", resulta, errorsToCheck, errorsToAvoid)) {
            return false ;
        }
    }
    {
        app.data()->setUseSaxonXSL(false);
        MessagesOperationResult result;
        XSLTExecutor executor(app.data());
        executor.setInputFile(FILE_INPUT_0);
        if(executor.exec(result)) {
            return error("Expected error param 2");
        }
        QList<int> errorsToCheck;
        errorsToCheck << XSLTExecutor::ErrorXSLMissing << XSLTExecutor::ErrorOutputMissing ;
        QList<int> errorsToAvoid;
        errorsToAvoid << XSLTExecutor::ErrorSourceMissing << XSLTExecutor::ErrorXSLTInvalid ;
        if(!checkExecErrors("i2", result, errorsToCheck, errorsToAvoid)) {
            return false ;
        }
        app.data()->setUseSaxonXSL(true);
        MessagesOperationResult resulta;
        if(executor.exec(resulta)) {
            return error("Expected error param 2a");
        }
        if(!checkExecErrors("i2a", resulta, errorsToCheck, errorsToAvoid)) {
            return false ;
        }
    }
    {
        app.data()->setUseSaxonXSL(false);
        MessagesOperationResult result;
        XSLTExecutor executor(app.data());
        executor.setInputFile(FILE_INPUT_0);
        executor.setXSLFile(FILE_XSL_0);
        if(executor.exec(result)) {
            return error("Expected error param 3");
        }
        QList<int> errorsToCheck;
        errorsToCheck << XSLTExecutor::ErrorOutputMissing ;
        QList<int> errorsToAvoid;
        errorsToAvoid << XSLTExecutor::ErrorSourceMissing << XSLTExecutor::ErrorXSLMissing << XSLTExecutor::ErrorXSLTInvalid ;
        if(!checkExecErrors("i3", result, errorsToCheck, errorsToAvoid)) {
            return false ;
        }
        app.data()->setUseSaxonXSL(true);
        MessagesOperationResult resulta;
        if(executor.exec(resulta)) {
            return error("Expected error param 3a");
        }
        if(!checkExecErrors("i3a", resulta, errorsToCheck, errorsToAvoid)) {
            return false ;
        }
    }
    {
        app.data()->setUseSaxonXSL(false);
        MessagesOperationResult result;
        XSLTExecutor executor(app.data());
        QString output;
        executor.setInputFile(FILE_INPUT_0);
        executor.setXSLFile(FILE_XSL_0);
        executor.setOutput(&output);
        executor.exec(result);
        QList<int> errorsToCheck;
        QList<int> errorsToAvoid;
        errorsToAvoid << XSLTExecutor::ErrorSourceMissing << XSLTExecutor::ErrorXSLMissing << XSLTExecutor::ErrorOutputMissing << XSLTExecutor::ErrorXSLTInvalid ;
        if(!checkExecErrors("i4", result, errorsToCheck, errorsToAvoid)) {
            return false ;
        }
        app.data()->setUseSaxonXSL(true);
        MessagesOperationResult resulta;
        executor.exec(resulta);
        if(!checkExecErrors("i4a", resulta, errorsToCheck, errorsToAvoid)) {
            return false ;
        }
    }
    {
        app.data()->setUseSaxonXSL(false);
        MessagesOperationResult result;
        XSLTExecutor executor(app.data());
        QString output;
        executor.setInputFile(FILE_INPUT_0);
        executor.setXSLFile(FILE_INPUT_0);
        executor.setOutput(&output);
        if(executor.exec(result)) {
            return error("Expected error param 5");
        }
        QList<int> errorsToCheck;
        errorsToCheck << XSLTExecutor::ErrorXSLTInvalid ;
        QList<int> errorsToAvoid;
        errorsToAvoid << XSLTExecutor::ErrorSourceMissing << XSLTExecutor::ErrorXSLMissing << XSLTExecutor::ErrorOutputMissing ;
        if(!checkExecErrors("i5", result, errorsToCheck, errorsToAvoid)) {
            return false ;
        }
        // unable to check saxon without starting saxon
    }
    {
        app.data()->setUseSaxonXSL(false);
        MessagesOperationResult result;
        XSLTExecutor executor(app.data());
        executor.setInputFile(":::***..****::D:D:D:D:D:D");
        executor.setXSLFile(":::***..****::D:D:D:D:D:D");
        executor.setOutput(":::***..****::D:D:D:D:D:D");
        if(executor.exec(result)) {
            return error("Expected error param 6");
        }
        QList<int> errorsToCheck;
        errorsToCheck << XSLTExecutor::ErrorOpeningDeviceInput << XSLTExecutor::ErrorOpeningDeviceXSL << XSLTExecutor::ErrorOpeningDeviceOutput ;
        QList<int> errorsToAvoid;
        if(!checkExecErrors("i6", result, errorsToCheck, errorsToAvoid)) {
            return false ;
        }
        // unable to check saxon without starting saxon
    }
    return true;
}

bool TestExecXSLT::checkExecErrors(const QString &id, MessagesOperationResult &result, QList<int> errorsToCheck, QList<int> errorsToAvoid)
{
    _testName = "checkExecErrors" ;
    QSet<int> errors ;
    foreach( SourceMessage *message, *result.messages() ) {
        if( message->type() == SourceMessage::Error ) {
            errors.insert((int)message->code());
        }
    }

    foreach( const int err, errorsToCheck ) {
        if( !errors.contains(err)) {
            return error(QString("For %2 Error expected, but not found:%1").arg(err).arg(id));
        }
    }
    foreach( const int err, errorsToAvoid ) {
        if( errors.contains(err)) {
            return error(QString("For %2 Error to avoid, but found:%1").arg(err).arg(id));
        }
    }
    return true ;
}

/* cases for the test
 *
id_|input_source|sheet_source|output
---+------------+------------+------
.1.|.file_______|_file_______|string
.2.|.editor_____|_file_______|string
.3.|.file_______|_editor_____|string
.4.|.file_______|_file_______|file
.5.|.editor_____|_editor_____|string
*/
bool TestExecXSLT::checkExecRunSources()
{
    _testName = "checkExecRunSources" ;
    if(!checkExecRunSources1()) {
        return false;
    }
    if(!checkExecRunSources2()) {
        return false;
    }
    if(!checkExecRunSources3()) {
        return false;
    }
    if(!checkExecRunSources4()) {
        return false;
    }
    if(!checkExecRunSources5()) {
        return false;
    }
    if(!checkExecRunSourcesSaxon()) {
        return false;
    }
    return true;
}

bool TestExecXSLT::checkExecRun()
{
    _testName = "checkExecRun" ;
    if(!checkExecRunSources()) {
        return false;
    }
    return true;
}

bool TestExecXSLT::checkExecRunSources1()
{
    _testName = "checkExecRunSources1" ;
    MessagesOperationResult result;
    App app;
    if(!app.initNoWindow()) {
        return error("Init no window");
    }
    XSLTExecutor executor(app.data());
    QString output;
    executor.setInputFile(FILE_INPUT_0);
    executor.setXSLFile(FILE_XSL_0);
    executor.setOutput(&output);
    if(!executor.exec(result) ) {
        return error("Failed exec");
    }
    if(result.isError()) {
        return error("Failed exec result");
    }
    CompareXML xmlCompare ;
    QByteArray ba = output.toUtf8();
    QBuffer buffer;
    buffer.setData(ba);
    if(!xmlCompare.compareBufferWithFile(&buffer, FILE_OUTPUT_0)) {
        return error(QString("Failed comparing %1").arg(xmlCompare.errorString()));
    }
    return true ;
}

bool TestExecXSLT::checkExecRunSources2()
{
    _testName = "checkExecRunSources2" ;
    MessagesOperationResult result;
    QString output;
    App app;
    if(!app.init() ) {
        return error("init app");
    }
    XSLTExecutor executor(app.data());
    if(!app.mainWindow()->loadFile(FILE_INPUT_0) ) {
        return error(QString("Loading file %1").arg(FILE_INPUT_0));
    }
    QString input = app.mainWindow()->getRegola()->getAsText();
    executor.setInputLiteral(input);
    executor.setXSLFile(FILE_XSL_0);
    executor.setOutput(&output);
    if(!executor.exec(result) ) {
        return error("Failed exec");
    }
    if(result.isError()) {
        return error("Failed exec result");
    }
    CompareXML xmlCompare ;
    QByteArray ba = output.toUtf8();
    QBuffer buffer;
    buffer.setData(ba);
    if(!xmlCompare.compareBufferWithFile(&buffer, FILE_OUTPUT_0)) {
        return error(QString("Failed comparing %1").arg(xmlCompare.errorString()));
    }
    return true;
}

bool TestExecXSLT::checkExecRunSources3()
{
    _testName = "checkExecRunSources3" ;
    MessagesOperationResult result;
    QString output;
    App app;
    if(!app.init() ) {
        return error("init app");
    }
    if(!app.mainWindow()->loadFile(FILE_XSL_0) ) {
        return error(QString("Loading file %1").arg(FILE_XSL_0));
    }
    XSLTExecutor executor(app.data());
    QString xsl = app.mainWindow()->getRegola()->getAsText();
    executor.setInputFile(FILE_INPUT_0);
    executor.setXSLLiteral(xsl);
    executor.setOutput(&output);
    if(!executor.exec(result) ) {
        return error("Failed exec");
    }
    if(result.isError()) {
        return error("Failed exec result");
    }
    CompareXML xmlCompare ;
    QByteArray ba = output.toUtf8();
    QBuffer buffer;
    buffer.setData(ba);
    if(!xmlCompare.compareBufferWithFile(&buffer, FILE_OUTPUT_0)) {
        return error(QString("Failed comparing %1").arg(xmlCompare.errorString()));
    }
    return true;
}

bool TestExecXSLT::checkExecRunSources4()
{
    _testName = "checkExecRunSources4" ;
    App app;
    if(!app.initNoWindow()) {
        return error("Init no window");
    }
    MessagesOperationResult result;
    XSLTExecutor executor(app.data());
    executor.setXSLFile(FILE_XSL_0);
    executor.setInputFile(FILE_INPUT_0);
    QTemporaryFile tempFile;
    tempFile.setFileTemplate("qxmledit-testxxxxxxxxxxx");
    tempFile.open();
    executor.setOutput(tempFile.fileName());
    if(!executor.exec(result) ) {
        return error("Failed exec");
    }
    if(result.isError()) {
        return error("Failed exec result");
    }
    CompareXML xmlCompare ;
    if(!xmlCompare.compareFiles(tempFile.fileName(), FILE_OUTPUT_0)) {
        return error(QString("Failed comparing %1").arg(xmlCompare.errorString()));
    }
    return true;
}

bool TestExecXSLT::checkExecRunSources5()
{
    _testName = "checkExecRunSources5" ;
    MessagesOperationResult result;
    QString output;
    App app;
    if(!app.init() ) {
        return error("init app");
    }
    if(!app.mainWindow()->loadFile(FILE_XSL_0) ) {
        return error(QString("Loading file %1").arg(FILE_XSL_0));
    }
    XSLTExecutor executor(app.data());
    QString xsl = app.mainWindow()->getRegola()->getAsText();

    if(!app.mainWindow()->loadFile(FILE_INPUT_0) ) {
        return error(QString("Loading file %1").arg(FILE_INPUT_0));
    }
    QString input = app.mainWindow()->getRegola()->getAsText();
    executor.setXSLLiteral(xsl);
    executor.setInputLiteral(input);
    executor.setOutput(&output);
    if(!executor.exec(result) ) {
        return error("Failed exec");
    }
    if(result.isError()) {
        return error("Failed exec result");
    }
    CompareXML xmlCompare ;
    QByteArray ba = output.toUtf8();
    QBuffer buffer;
    buffer.setData(ba);
    if(!xmlCompare.compareBufferWithFile(&buffer, FILE_OUTPUT_0)) {
        return error(QString("Failed comparing %1").arg(xmlCompare.errorString()));
    }
    return true ;
}

bool TestExecXSLT::checkHandleParameters()
{
    _testName = "checkHandleParameters" ;
    if(!checkExtractParametersFromXSL()) {
        return false ;
    }
    if(!checkExtractParametersFromDialog()) {
        return false ;
    }
    if(!checkMergeParametersFromXSL()) {
        return false ;
    }
    if(!checkMergeParametersFromEditor()) {
        return false ;
    }
    if(!checkResetParametersFromXSL()) {
        return false ;
    }
    return true ;
}

bool TestExecXSLT::checkExtractParametersFromXSL()
{
    _testName = "checkExtractParametersFromXSL" ;
    XSLParameterManager manager;
    if(!manager.extractParametersFromFile(FILE_XSL_0) ) {
        return error("Error extracting parameters.");
    }
    QSet<QString> expected;
    expected << "p1" << "p2" ;
    QSet<QString> found = manager.parameters();
    if(!compareStringSets("Comparing parameters", expected, found) ) {
        return false ;
    }
    return true ;
}

bool TestExecXSLT::checkExtractParametersFromDialog()
{
    _testName = "checkExtractParametersFromDialog" ;
    App app;
    if(!app.init()) {
        return error("init app");
    }
    XSLTExecDialog dialog(NULL, app.data());
    dialog.setXSLFile(FILE_XSL_0);
    dialog.mergeParameters();
    QHash<QString,QString> expected;
    expected["p1"] = "" ;
    expected["p2"] = "" ;
    QHash<QString,QString> found = dialog.getParameters();
    if(!testHash(expected, found) ) {
        return false ;
    }
    return true ;
}

bool TestExecXSLT::checkMergeParametersFromXSL()
{
    _testName = "checkMergeParametersFromXSL" ;
    App app;
    if(!app.init()) {
        return error("init app");
    }
    XSLTExecDialog dialog(NULL, app.data());
    // loading parameters in dialog
    dialog.addParamRow("test1", "value1");
    dialog.addParamRow("test2", "value2");
    dialog.setXSLFile(FILE_XSL_0);
    dialog.mergeParameters();
    QHash<QString,QString> expected;
    expected["p1"] = "" ;
    expected["p2"] = "" ;
    expected["test1"] = "value1" ;
    expected["test2"] = "value2" ;
    QHash<QString,QString> found = dialog.getParameters();
    if(!testHash(expected, found) ) {
        return false ;
    }
    return true ;
}

bool TestExecXSLT::checkMergeParametersFromEditor()
{
    App app;
    if(!app.init()) {
        return error("init app");
    }
    if(!app.mainWindow()->loadFile(FILE_XSL_0) ) {
        return error(QString("Loading file %1").arg(FILE_XSL_0));
    }
    XSLTExecDialog dialog(NULL, app.data());
    dialog.setXSLEditor(app.mainWindow()->getEditor());
    dialog.mergeParameters();
    QHash<QString,QString> expected;
    expected["p1"] = "" ;
    expected["p2"] = "" ;
    QHash<QString,QString> found = dialog.getParameters();
    if(!testHash(expected, found) ) {
        return false ;
    }
    return true ;
}

bool TestExecXSLT::checkResetParametersFromXSL()
{
    _testName = "checkResetParametersFromXSL" ;
    App app;
    if(!app.init()) {
        return error("init app");
    }
    XSLTExecDialog dialog(NULL, app.data());
    // loading parameters in dialog
    dialog.addParamRow("test1", "value1");
    dialog.addParamRow("test2", "value2");
    dialog.setXSLFile(FILE_XSL_0);
    dialog.mergeParameters();
    dialog.resetParameters();
    QHash<QString,QString> expected;
    expected["p1"] = "" ;
    expected["p2"] = "" ;
    QHash<QString,QString> found = dialog.getParameters();
    if(!testHash(expected, found) ) {
        return false ;
    }
    return true ;
}

bool TestExecXSLT::checkComamndLine()
{
    _testName = "checkComamndLine" ;
    //----
    {
        StartParams sp;
        QStringList cmdLine ;
        cmdLine << "pgm" ;
        sp.decodeCommandLine(cmdLine);
        if(StartParams::XSLExec == sp.type) {
            return error(QString("xsl mode without parameters, error is: %1").arg(sp.errorMessage));
        }
    }
    {
        StartParams sp;
        QStringList cmdLine ;
        cmdLine << "pgm" << "-xslt";
        bool success = sp.decodeCommandLine(cmdLine);
        if(StartParams::XSLExec != sp.type) {
            return error(QString("xsl mode not activated, error is: %1").arg(sp.errorMessage));
        }
        if(success) {
            return error(QString("xsl empty param succeeded, error is: %1").arg(sp.errorMessage));
        }
    }
    {
        StartParams sp;
        QStringList cmdLine ;
        cmdLine << "pgm" << "-xslt" << "-saxon" << "-xsl=x.xsl" << "-output=o.xml" << "-pp1=a" << "-pp2=bb" << "i.xml";
        bool success = sp.decodeCommandLine(cmdLine);
        if(StartParams::XSLExec != sp.type) {
            return error(QString("xsl mode not activated, error is: %1").arg(sp.errorMessage));
        }
        if(!success) {
            return error(QString("xsl param failed, error is: %1").arg(sp.errorMessage));
        }
        if(sp.fileName != "i.xml") {
            return error(QString("xsl 1 failed parm input, expected: %1, found: %2").arg("i.xml").arg(sp.fileName));
        }
        if(sp.xsl != "x.xsl") {
            return error(QString("xsl 1 failed parm xsl, expected: %1, found: %2").arg("z.xsl").arg(sp.xsl));
        }
        if(sp.outputFile != "o.xml") {
            return error(QString("xsl 1 failed parm output, expected: %1, found: %2").arg("o.xml").arg(sp.outputFile));
        }
        if(!sp.forceSaxon) {
            return error(QString("xsl 1 failed parm saxon, expected: %1, found: %2").arg(true).arg(sp.forceSaxon));
        }
        QHash<QString, QString> parameters;
        Utils::copyPairsToHash(parameters, sp.params );
        QHash<QString, QString> expected;
        expected["p1"] = "a";
        expected["p2"] = "bb";
        if(expected != parameters ) {
            return error(QString("xsl 1 failed parm parameters, keys expected: %1, found: %2").arg(dumpHash(expected)).arg(dumpHash(parameters)));
        }
    }
    {
        StartParams sp;
        QStringList cmdLine ;
        cmdLine << "pgm" << "-xslt" << "-xsl=x.xsl" << "-output=o.xml" << "-pp1=a" << "-pp2=bb" << "i.xml";
        bool success = sp.decodeCommandLine(cmdLine);
        if(StartParams::XSLExec != sp.type) {
            return error(QString("xsl mode not activated, error is: %1").arg(sp.errorMessage));
        }
        if(!success) {
            return error(QString("xsl param failed, error is: %1").arg(sp.errorMessage));
        }
        if(sp.fileName != "i.xml") {
            return error(QString("xsl 1 failed parm input, expected: %1, found: %2").arg("i.xml").arg(sp.fileName));
        }
        if(sp.xsl != "x.xsl") {
            return error(QString("xsl 1 failed parm xsl, expected: %1, found: %2").arg("x.xsl").arg(sp.xsl));
        }
        if(sp.outputFile != "o.xml") {
            return error(QString("xsl 1 failed parm output, expected: %1, found: %2").arg("o.xml").arg(sp.outputFile));
        }
        if(sp.forceSaxon) {
            return error(QString("xsl 1 failed parm saxon, expected: %1, found: %2").arg(true).arg(sp.forceSaxon));
        }
        QHash<QString, QString> parameters;
        Utils::copyPairsToHash(parameters, sp.params );
        QHash<QString, QString> expected;
        expected["p1"] = "a";
        expected["p2"] = "bb";
        if(expected != parameters ) {
            return error(QString("xsl 1 failed parm parameters, keys expected: %1, found: %2").arg(dumpHash(expected)).arg(dumpHash(parameters)));
        }
    }
    //----
    return true;
}

bool TestExecXSLT::checkSaxonEngine()
{
    _testName = "checkSaxonEngine" ;

    App app;
    if(!app.initNoWindow()) {
        return error("init app");
    }
    QString saxonPath = "aaa" ;
    app.data()->setSaxonXSLPath(saxonPath);
    StartParams sp;
    QStringList cmdLine ;
    cmdLine << "pgm" << "-xslt" << "-saxon" << "-xsl=x.xsl" << "-output=o.xml" << "-pp1=a" << "-pp2=bb" << "i.xml";
    bool success = sp.decodeCommandLine(cmdLine);
    if(!success) {
        return error(QString("xsl param failed, error is: %1").arg(sp.errorMessage));
    }
    const int retCode = doXSL(app.data(), &sp);
    if(retCode != 0) {
        return error(QString("RetCode not zero, is: %1").arg(retCode));
    }
    QStringList expected;
    expected << "-jar" ;
    expected << saxonPath;
    expected << QString("-s:%1").arg("i.xml");
    expected << QString("-xsl:%1").arg("x.xsl");
    expected << QString("-o:%1").arg("o.xml");
    expected << "p1=a" << "p2=bb" ;
    if(!compareStringList("Comparing arguments", expected, XSLTExecutor::testSaxonArguments)) {
        return false;
    }
    return true;
}

bool TestExecXSLT::checkChooseEngine()
{
    _testName = "checkChooseEngine" ;
    XSLTExecutor::stop = false;
    if(!checkChooseEngine1() ) {
        XSLTExecutor::stop = false;
        return false ;
    }
    XSLTExecutor::stop = false;
    return  true ;
}

bool TestExecXSLT::checkChooseEngine1()
{
    App app;
    if(!app.initNoWindow()) {
        return error("init app");
    }
    const QString saxonPath = "aaa" ;
    app.data()->setSaxonXSLPath(saxonPath);

    // --- 1 no saxon -> no saxon
    {
        XSLTExecutor::resetTest();
        XSLTExecutor::stop = true;
        app.data()->setUseSaxonXSL(false);
        StartParams sp;
        QStringList cmdLine ;
        cmdLine << "pgm" << "-xslt" << "-xsl=x.xsl" << "-output=o.xml" << "-pp1=a" << "-pp2=bb" << "i.xml";
        bool success = sp.decodeCommandLine(cmdLine);
        if(!success) {
            return error(QString("1 xsl param failed, error is: %1").arg(sp.errorMessage));
        }
        const int retCode = doXSL(app.data(), &sp);
        if(retCode != 0) {
            return error(QString("1 RetCode not zero, is: %1").arg(retCode));
        }
        if(XSLTExecutor::saxonWasChosen) {
            return error("1 saxon chosen");
        }
    }
    // --- 2 saxon -> saxon
    {
        XSLTExecutor::resetTest();
        XSLTExecutor::stop = true;
        app.data()->setUseSaxonXSL(true);
        StartParams sp;
        QStringList cmdLine ;
        cmdLine << "pgm" << "-xslt" << "-xsl=x.xsl" << "-output=o.xml" << "-pp1=a" << "-pp2=bb" << "i.xml";
        bool success = sp.decodeCommandLine(cmdLine);
        if(!success) {
            return error(QString("2 xsl param failed, error is: %1").arg(sp.errorMessage));
        }
        const int retCode = doXSL(app.data(), &sp);
        if(retCode != 0) {
            return error(QString("2 RetCode not zero, is: %1").arg(retCode));
        }
        if(!XSLTExecutor::saxonWasChosen) {
            return error("2 saxon chosen");
        }
    }
    // --- 3 no saxon, force -> saxon
    {
        XSLTExecutor::resetTest();
        XSLTExecutor::stop = true;
        app.data()->setUseSaxonXSL(false);
        StartParams sp;
        QStringList cmdLine ;
        cmdLine << "pgm" << "-xslt" << "-saxon" << "-xsl=x.xsl" << "-output=o.xml" << "-pp1=a" << "-pp2=bb" << "i.xml";
        bool success = sp.decodeCommandLine(cmdLine);
        if(!success) {
            return error(QString("3 xsl param failed, error is: %1").arg(sp.errorMessage));
        }
        const int retCode = doXSL(app.data(), &sp);
        if(retCode != 0) {
            return error(QString("3 RetCode not zero, is: %1").arg(retCode));
        }
        if(!XSLTExecutor::saxonWasChosen) {
            return error("3 saxon chosen");
        }
    }
    {
        XSLTExecutor::resetTest();
        XSLTExecutor::stop = false;
        StartParams sp;
        QStringList cmdLine ;
        cmdLine << "pgm" << "-xslt" << "-saxon" << "-xsl=x.xsl" << "-output=o.xml" << "-pp1=a" << "-pp2=bb" << "i.xml";
        bool success = sp.decodeCommandLine(cmdLine);
        if(!success) {
            return error(QString("4 xsl param failed, error is: %1").arg(sp.errorMessage));
        }
        const int retCode = doXSL(app.data(), &sp);
        if(retCode != 0) {
            return error(QString("4 RetCode not zero, is: %1").arg(retCode));
        }
        QStringList expected;
        expected << "-jar" ;
        expected << saxonPath;
        expected << QString("-s:%1").arg("i.xml");
        expected << QString("-xsl:%1").arg("x.xsl");
        expected << QString("-o:%1").arg("o.xml");
        expected << "p1=a" << "p2=bb" ;
        if(!compareStringList("4 Comparing arguments", expected, XSLTExecutor::testSaxonArguments)) {
            return false;
        }
    }
    //----------
    {
        XSLTExecutor::resetTest();

        app.data()->setUseSaxonXSL(true);
        app.data()->setSaxonXSLPath("");
        XSLTExecutor executor(app.data());
        QString output;
        MessagesOperationResult result;
        executor.setInputFile(FILE_INPUT_0);
        executor.setXSLFile(FILE_XSL_0);
        QTemporaryFile tempFile;
        tempFile.setFileTemplate("qxmledit-testxxxxxxxxxxx");
        tempFile.open();
        executor.setOutput(tempFile.fileName());
        if(executor.exec(result) ) {
            return error("5 exec succeeded");
        }
        if(!result.isError()) {
            return error("5 Expected error from execution");
        }
        bool found = false;
        foreach( SourceMessage *message, *result.messages() ) {
            if( message->type() == SourceMessage::Error ) {
                if(message->code() == XSLTExecutor::ErrorEngineNotConfigured ) {
                    found = true ;
                    break;
                }
            }
        }
        if(!found) {
            return error("5 Expected error for not configured engine.");
        }
    }
    Utils::TODO_THIS_RELEASE("testare errori con saxon di file di input");
    return true;
}

bool TestExecXSLT::checkSaxonEngineCfg()
{
    _testName = "checkSaxonEngineCfg" ;
    App app;
    if(!app.initNoWindow()) {
        return error("init app");
    }
    const QString saxonPath = "aaa" ;
    app.data()->setSaxonXSLPath(saxonPath);
    XSLTExecutor::resetTest();

    app.data()->setUseSaxonXSL(true);
    {
        XSLTExecutor executor(app.data());
        MessagesOperationResult result;
        executor.setInputLiteral("<root/>");
        executor.setXSLFile(FILE_XSL_0);
        executor.setOutput("  :C::S:****::/tmp/xx");
        if(executor.exec(result) ) {
            return error("exec succeeded");
        }
        QList<int> errorsToCheck;
        errorsToCheck << XSLTExecutor::ErrorConfigurationMissing ;
        QList<int> errorsToAvoid;
        if(!checkExecErrors("input cfg missing", result, errorsToCheck, errorsToAvoid)) {
            return false ;
        }
    }
    {
        XSLTExecutor executor(app.data());
        MessagesOperationResult result;
        executor.setInputFile(FILE_INPUT_0);
        executor.setXSLLiteral("<root/>");
        executor.setOutput("  :C::S:****::/tmp/xx");
        if(executor.exec(result) ) {
            return error("exec succeeded");
        }
        QList<int> errorsToCheck;
        errorsToCheck << XSLTExecutor::ErrorConfigurationMissing ;
        QList<int> errorsToAvoid;
        if(!checkExecErrors("xsl cfg missing", result, errorsToCheck, errorsToAvoid)) {
            return false ;
        }
    }
    {
        XSLTExecutor executor(app.data());
        MessagesOperationResult result;
        executor.setInputFile(FILE_INPUT_0);
        executor.setXSLLiteral("<root/>");
        QString output;
        executor.setOutput(&output);
        if(executor.exec(result) ) {
            return error("exec succeeded");
        }
        QList<int> errorsToCheck;
        errorsToCheck << XSLTExecutor::ErrorConfigurationMissing ;
        QList<int> errorsToAvoid;
        if(!checkExecErrors("output cfg missing", result, errorsToCheck, errorsToAvoid)) {
            return false ;
        }
    }
    return true ;
}

bool TestExecXSLT::checkExecRunSourcesSaxon()
{
    _testName = "checkExecRunSourcesSaxon" ;
    MessagesOperationResult result;
    QString output;
    App app;
    if(!app.init() ) {
        return error("init app");
    }
    QString saxonPath = "aaa" ;
    app.data()->setSaxonXSLPath(saxonPath);
    app.data()->setUseSaxonXSL(true);
    XSLTExecutor executor(app.data());
    if(!app.mainWindow()->loadFile(FILE_XSL_2, true, MainWindow::OpenUsingNewWindow) ) {
        return error(QString("Loading file %1").arg(FILE_XSL_0));
    }
    QString xsl = app.mainWindow()->getRegola()->getAsText();

    if(!app.mainWindow()->loadFile(FILE_INPUT_2, true, MainWindow::OpenUsingNewWindow) ) {
        return error(QString("Loading file %1").arg(FILE_INPUT_0));
    }
    QString input = app.mainWindow()->getRegola()->getAsText();

    executor.setXSLLiteral(xsl);
    executor.setInputLiteral(input);
    executor.setOutput(&output);

    if(!executor.exec(result) ) {
        return error("Failed exec");
    }
    if(result.isError()) {
        return error("Failed exec result");
    }
    CompareXML xmlCompare ;
    if(!xmlCompare.compareStringWithFile(output, FILE_OUTPUT_2)) {
        return error(QString("Failed comparing %1").arg(xmlCompare.errorString()));
    }
    return true ;
}
