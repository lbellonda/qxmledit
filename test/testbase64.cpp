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

#include "testbase64.h"

#include "app.h"
#include "comparexml.h"
#include "config.h"
#include "mainwindow.h"
#include "utils.h"
#include "edittextnode.h"
#include "fakeuidelegate.h"
#include "modules/utils/base64utils.h"

#define BASE64_INPUTFILE    "../test/data/base64_input.xml"
#define BASE64_BINARYDATA   "../test/data/base64_test.xml"
#define BASE64_RESULT       "../test/data/base64_output.xml"
#define BASE64_FILE_BINARY    "../test/data/base64_binary.jpg"
#define BASE64_FILE_UTILS    "../test/data/base64_utils.dat"

TestBase64::TestBase64()
{
    _showXML = false ;
}

bool TestBase64::test_base64()
{
    int operationType = 0 ;
    App app;
    if(!app.initNoWindow() ) {
        return false;
    }

    MainWindow mainWindow(false, qApp, app.data());

    mainWindow.loadFile(BASE64_INPUTFILE);

    QList<int> selection;
    selection.append(1);
    selection.append(0);
    selection.append(2);
    Element *elementToTransform = mainWindow.getRegola()->findElementByArray(selection);

    if(NULL == elementToTransform) {
        fprintf(stderr, "%s\n", QString("Test:%1 No selected element").arg(operationType).toLatin1().data());
        return false;
    }

    if(elementToTransform ->tag() != "text") {
        fprintf(stderr, "%s\n", QString("Test:%1 Unexpected tag: %2").arg(operationType).arg(elementToTransform->tag()).toLatin1().data());
        return false;
    }

    EditTextNode editDialog(false, "", &mainWindow);
    editDialog.setWindowModality(Qt::WindowModal);
    editDialog.setText(elementToTransform->getAsSimpleText(false));
    if(! editDialog.testLoadBinaryFile(BASE64_BINARYDATA)) {
        fprintf(stderr, "%s\n", QString("Test:%1 edit node failed").arg(operationType).toLatin1().data());
        return false ;
    }
    editDialog.accept();
    elementToTransform->setAsSingleTextNode(editDialog.getText(), false, false) ;

    QByteArray resultData = mainWindow.getRegola()->writeMemory();
    QDomDocument document1;
    QDomDocument document2;
    CompareXML compare;
    if(!compare.loadFileIntoDocument(BASE64_RESULT, document1)) {
        fprintf(stderr, "%s\n", (QString("Test:%1 loadFile %2").arg(operationType).arg(BASE64_RESULT)).toLatin1().data());
        return false;
    }
    if(_showXML) {
        QString res(resultData);
        Utils::message(res);
    }
    QBuffer outputData(&resultData);
    if(!compare.loadFileIntoDocument(&outputData, document2)) {
        fprintf(stderr, "%s\n", (QString("Test:%1 loadFile data").arg(operationType)).toLatin1().data());
        return false;
    }
    bool result = compare.compareDomDocuments(document1, document2);

    return result ;
}

bool TestBase64::test_base64_base()
{
    QString base64Encoded = "PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiPz4KPHJvb3Q+CjxkZGQ+YWJj0YTQuNGBPC9kZGQ+Cjwvcm9vdD4K";
    QString base64Decoded = QString::fromUtf8("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<root>\n<ddd>abc\u0444\u0438\u0441</ddd>\n</root>\n");

    QString result = Utils::toBase64(base64Decoded);
    if( result != base64Encoded ) {
        return false;
    }

    result = Utils::fromBase64Xml(base64Encoded);
    if( result != base64Decoded ) {
        return false;
    }

    result = Utils::fromBase64(base64Encoded);
    if( result != base64Decoded ) {
        return false;
    }
    return true;
}

bool TestBase64::test_base64_file()
{
    EditTextNode etn(false, "");
    etn.setWindowModality(Qt::WindowModal);

    if( !etn.loadFromBinaryFile(BASE64_FILE_BINARY)) {
        return error("open file in");
    }
    QBuffer buffer ;
    buffer.open(QIODevice::ReadWrite);
    if(!etn.saveToBinaryDevice(&buffer)) {
        return error("writing binary");
    }
    buffer.close();
    QFile fileIn(BASE64_FILE_BINARY);
    if( !fileIn.open(QIODevice::ReadOnly)) {
        return error("open file in");
    }
    QByteArray srcData = fileIn.readAll();
    int errorN = fileIn.error() ;
    bool isOk = ( fileIn.error() == QFile::NoError );
    fileIn.close();
    if( !isOk ) {
        return error(QString("reading file: %1").arg(errorN));
    }

    // compare the buffer with the data;
    QByteArray origData = buffer.data();
    // Extra info not needed now.
    //qint64 s = buffer.bytesAvailable();
    //int c = origData.count();
    //int c1 = srcData.count();

    if( origData != srcData ) {
        return error("data not equal");
    }

    return true;
}

bool TestBase64::test_base64_utils()
{
    _testName = "test_base64_utils" ;
    Base64Utils base64;
    bool isError = false;
    bool isAbort = false ;
    QString decoded = base64.loadFromBinaryFile(NULL, BASE64_FILE_UTILS, isError, isAbort);
    if(isAbort) {
        return error("Loading is aborted.");
    }
    if(isError) {
        return error("Loading is error.");
    }
    QString expected = "YWJjZA==" ;
    if(decoded!=expected) {
        return error(QString("Load differs Decoded (%1):'%2'\nExpected (%3):%4").arg(decoded.length()).arg(decoded).arg(expected.length()).arg(expected));
    }

    QBuffer buffer ;
    buffer.open(QIODevice::ReadWrite);
    if(!base64.saveToBinaryDevice( &buffer, decoded )) {
        return error("writing binary");
    }
    buffer.close();
    QFile fileIn(BASE64_FILE_UTILS);
    if( !fileIn.open(QIODevice::ReadOnly)) {
        return error("open file in");
    }
    QByteArray srcData = fileIn.readAll();
    int errorN = fileIn.error() ;
    bool isOk = ( fileIn.error() == QFile::NoError );
    fileIn.close();
    if( !isOk ) {
        return error(QString("reading file: %1").arg(errorN));
    }

    // compare the buffer with the data;
    QByteArray origData = buffer.data();
    // Extra info not needed now.

    if( origData != srcData ) {
        return error(QString("Compare differs Decoded (%1):'%2'\nExpected (%3):%4")
                     .arg(srcData.length()).arg(QString(srcData.toBase64()))
                     .arg(origData.length()).arg(QString(origData.toBase64())));
    }

    return true;
}
