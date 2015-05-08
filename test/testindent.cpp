/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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


#include <QTextCodec>
#include <QBuffer>


#define FILE_TEST "../test/data/indent.xml"


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
    compareBuffer.open(QIODevice::WriteOnly|QIODevice::Text);
    QTextStream stream(&compareBuffer);
    stream.setCodec(QTextCodec::codecForName("UTF-8"));
    stream << compare ;
    stream.flush();
    compareBuffer.close();

    // compare as xml
    CompareXML compareXml ;
    if(!compareXml.compareBufferWithFile(&compareBuffer, FILE_TEST) ) {
        return error(QString("Compaing ref Data for indent %1, details:%2").arg(indentation).arg(compareXml.errorString()) );
    }
    if(!compareXml.compareBufferWithFile(&outData, FILE_TEST) ) {
        return error(QString("Compaing out Data for indent %1, details:%2").arg(indentation).arg(compareXml.errorString()) );
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
