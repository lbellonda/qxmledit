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

#include "testexport.h"
#include "app.h"
#include "comparexml.h"

#define FILE_SRC        "../test/data/exp_src.xml"
#define FILE_CMP_JAVA   "../test/data/exp_res.txt"
#define FILE_CMP_C      "../test/data/exp_res_c.txt"

#define BASE_PATH "../test/data/xml/saving"
#define FILE_EXPORT_SRC_NOD BASE_PATH "/export_src_nd.xml"
#define FILE_EXPORT_SRC_D BASE_PATH "/export_src_d.xml"
#define FILE_EXPORT_RIS_NONS_NOD BASE_PATH "/export_r_n_n.xml"
#define FILE_EXPORT_RIS_NS_NOD BASE_PATH "/export_r_s_n.xml"
#define FILE_EXPORT_RIS_NONS_D BASE_PATH "/export_r_n_d.xml"
#define FILE_EXPORT_RIS_NS_D BASE_PATH "/export_r_s_d.xml"

TestExport::TestExport()
{
}

TestExport::~TestExport()
{
}

bool TestExport::testFast()
{
    return testExportSelected();
}

//----
bool TestExport::testExportJava()
{
    _testName = "testExportJava" ;

    App app;
    if(!app.init(true) ) {
        return error("init app");
    }
    if(!app.mainWindow()->loadFile(FILE_SRC) ) {
        return error(QString("Loading file '%1'").arg(FILE_SRC));
    }
    QBuffer memoryDevice;
    if( !app.mainWindow()->getRegola()->writeAsJavaString(&memoryDevice) ) {
        return error(QString("Writing device"));
    }
    QString deviceString ;
    QString fileData;
    QFile file(FILE_CMP_JAVA);
    if( !loadIOAsString(&file, "UTF-8", &fileData ) ) {
        return error(QString("reading from result file"));
    }
    if( !loadIOAsString(&memoryDevice, "UTF-8", &deviceString ) ) {
        return error(QString("reading from result string"));
    }
    fileData = fileData.replace("\r\n", "\n");
    deviceString = deviceString.replace("\r\n", "\n");
    if( isError() ) {
        return false;
    }
    if( deviceString != fileData ) {
        return error(QString("Comparing data found '%1'\n expecting: '%2'").arg(deviceString).arg(fileData));
    }
    return true ;
}

bool TestExport::testExportC()
{
    _testName = "testExportC" ;

    App app;
    if(!app.init(true) ) {
        return error("init app");
    }
    if(!app.mainWindow()->loadFile(FILE_SRC) ) {
        return error(QString("Loading file '%1'").arg(FILE_SRC));
    }
    QBuffer memoryDevice;
    if( !app.mainWindow()->getRegola()->writeAsCString(&memoryDevice) ) {
        return error(QString("Writing device"));
    }
    QString deviceString ;
    QString fileData;
    QFile file(FILE_CMP_C);
    if( !loadIOAsString(&file, "UTF-8", &fileData ) ) {
        return error(QString("reading from result file"));
    }
    if( !loadIOAsString(&memoryDevice, "UTF-8", &deviceString ) ) {
        return error(QString("reading from result string"));
    }
    if( isError() ) {
        return false;
    }

    fileData = fileData.replace("\r\n", "\n");
    deviceString = deviceString.replace("\r\n", "\n");
    if( deviceString != fileData ) {
        return error(QString("Comparing data found '%1'\n expecting: '%2'").arg(deviceString).arg(fileData));
    }
    return true ;
}

bool TestExport::testExportSelected()
{
    _testName = "testExportSelected" ;

    if(!testExportSelectedInner("0", FILE_EXPORT_SRC_D, FILE_EXPORT_RIS_NONS_NOD, Regola::ExportOptionNone, 1)) {
        return false;
    }
    if(!testExportSelectedInner("1", FILE_EXPORT_SRC_D, FILE_EXPORT_RIS_NS_NOD, Regola::ExportOptionUseNamespace, 1)) {
        return false;
    }
    if(!testExportSelectedInner("2", FILE_EXPORT_SRC_D, FILE_EXPORT_RIS_NONS_D, Regola::ExportOptionUseDeclaration, 1)) {
        return false;
    }
    if(!testExportSelectedInner("3", FILE_EXPORT_SRC_D, FILE_EXPORT_RIS_NS_D, Regola::ExportOptionUseDeclaration|Regola::ExportOptionUseNamespace, 1)) {
        return false;
    }
    //---
    if(!testExportSelectedInner("0", FILE_EXPORT_SRC_NOD, FILE_EXPORT_RIS_NONS_NOD, Regola::ExportOptionNone, 0)) {
        return false;
    }
    if(!testExportSelectedInner("1", FILE_EXPORT_SRC_NOD, FILE_EXPORT_RIS_NS_NOD, Regola::ExportOptionUseNamespace, 0)) {
        return false;
    }
    if(!testExportSelectedInner("2", FILE_EXPORT_SRC_NOD, FILE_EXPORT_RIS_NONS_NOD, Regola::ExportOptionUseDeclaration, 0)) {
        return false;
    }
    if(!testExportSelectedInner("3", FILE_EXPORT_SRC_NOD, FILE_EXPORT_RIS_NS_NOD, Regola::ExportOptionUseDeclaration|Regola::ExportOptionUseNamespace, 0)) {
        return false;
    }
    return true ;
}

bool TestExport::testExportSelectedInner(const QString &id, const QString &fileIn, const QString &fileResult, const Regola::EExportOptions options, const int first)
{
    _testName = QString("testExportSelected/%1").arg(id) ;

    App app;
    if(!app.init(true) ) {
        return error("init app");
    }
    if(!app.mainWindow()->loadFile(fileIn) ) {
        return error(QString("Loading file '%1'").arg(fileIn));
    }
    QList<int> selection ;
    selection << first << 1;
    Element *selectedElement = app.mainWindow()->getRegola()->findElementByArray(selection);
    if( NULL == selectedElement ) {
        return error("Null selection");
    }
    app.mainWindow()->getEditor()->setCurrentItem(selectedElement);

    QBuffer memoryDevice;
    if( !app.mainWindow()->getRegola()->exportElement( &memoryDevice, options, selectedElement) ) {
        return error(QString("Writing device"));
    }
    CompareXML compare;
    if( !compare.compareFiles(fileResult, &memoryDevice) ) {
        return error(QString("Error comparing file:%1 %2").arg(fileResult).arg(compare.errorString()));
    }
    return true ;
}
