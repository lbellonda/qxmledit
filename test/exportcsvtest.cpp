/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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


#include "exportcsvtest.h"
#include "app.h"
#include <QTest>

#define TEST_FILE_CSVINPUT1 "../test/data/export/csv/input1.xml"
#define TEST_FILE_CSVTEST1 "../test/data/export/csv/test1.csv"
#define TEST_FILE_CSVTEST2 "../test/data/export/csv/test2.csv"
#define TEST_FILE_CSVTEST3 "../test/data/export/csv/test3.csv"


ExportCSVTest::ExportCSVTest()
{
    _isError = false;
}

QString ExportCSVTest::errorString()
{
    return _errorString;
}

bool ExportCSVTest::error(const QString &testName, const QString &msg)
{
    _errorString = QString("%1: %2").arg(testName).arg(msg);
    _isError = true ;
    return false ;
}

bool ExportCSVTest::testUnitTest()
{
    QString strTest = "test";
    QString result = Utils::valueStringCSV(strTest, true);
    if( QString("\"test\"") != result ) {
        return error("testUnitTest", "1");
    }
    QString result2 = Utils::valueStringCSV(strTest, false);
    if( QString(",\"test\"") != result2 ) {
        return error("testUnitTest", "1");
    }
    return true;
}


bool ExportCSVTest::compareFileWithString(const QString &filePath, const QString &exportedData)
{
    bool isOk = false;
    QString data = exportedData;
    QFile inFile(filePath);
    if( inFile.open(QFile::ReadOnly|QFile::Text)) {
        QTextStream inStream(&inFile);
        QTextStream txtStream(&data, QIODevice::ReadOnly );
        bool isDifferent = false;
        QString r1, r2;
        do {
            r1 = inStream.readLine();
            r2 = txtStream.readLine();
            if( r1 != r2 ) {
                isDifferent = true ;
                break;
            }
        } while(!r1.isNull() && !r2.isNull());
        if( r1.isNull() != r2.isNull() ) {
            isDifferent = true ;
        }

        if( inFile.error() == QFile::NoError ) {
            if( !isDifferent ) {
                isOk = true ;
            }
        }
        inFile.close();
    }
    return isOk ;
}

bool ExportCSVTest::testExportSiblingsAttributes()
{
    App app;
    bool isError = false;
    if(!app.init() ) {
        return false;
    }
    app.mainWindow()->loadFile(QString(TEST_FILE_CSVINPUT1));

    Regola *regola = app.mainWindow()->getRegola();
    QList<int> selection;
    selection.append(0);
    selection.append(0);
    selection.append(1);
    Element *selectedElement ;
    selectedElement = regola->findElementByArray(selection);
    if( NULL != selectedElement ) {
        app.mainWindow()->getEditor()->setCurrentItem(selectedElement);
        app.mainWindow()->getUI()->actionExportAttributesSiblingsAsCSVClipboard->activate(QAction::Trigger);
        QString exportedData = QApplication::clipboard()->text();
        if(!compareFileWithString(TEST_FILE_CSVTEST1, exportedData) ) {
            return  error("testExportSiblingsAttributes", "1" );
        }
    } else {
        isError = true ;
    }

    selection.clear();
    selection.append(0);
    selection.append(0);
    selection.append(2);
    selection.append(0);
    selectedElement = regola->findElementByArray(selection);
    if( NULL != selectedElement ) {
        app.mainWindow()->getEditor()->setCurrentItem(selectedElement);
        app.mainWindow()->getUI()->actionExportAttributesSiblingsAsCSVClipboard->activate(QAction::Trigger);
        QString exportedData = QApplication::clipboard()->text();
        if(!compareFileWithString(TEST_FILE_CSVTEST2, exportedData) ) {
            return  error("testExportSiblingsAttributes", "2" );
        }
    } else {
        isError = true ;
    }
    selection.clear();
    selection.append(0);
    selection.append(0);
    selection.append(2);
    selection.append(0);
    selectedElement = regola->findElementByArray(selection);
    if( NULL != selectedElement ) {
        app.mainWindow()->getEditor()->setCurrentItem(selectedElement);
        app.mainWindow()->getUI()->actionExportAttributesSiblingsAsCSVClipboard->activate(QAction::Trigger);
        QString exportedData = QApplication::clipboard()->text();
        if(compareFileWithString(TEST_FILE_CSVTEST3, exportedData) ) {
            return  error("testExportSiblingsAttributes", "3" );
        }
    } else {
        isError = true ;
    }
    if( isError ) {
        return  error("testExportSiblingsAttributes", "-1" );
    }
    return true ;
}

