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


TestIndent::TestIndent()
{
}

TestIndent::~TestIndent()
{
}


bool TestIndent::testSaving()
{
    _testName = "testSaving" ;
    for( int indentation = -1 ; indentation < 16 ; indentation ++ ) {
        if( !saveAndCompare(indentation)) {
            return false;
        }
    }
    return true;
}


bool TestIndent::saveAndCompare(const int indentation)
{
    App app;
    if(!app.init()) {
        return error("app init");
    }
    if( !app.mainWindow()->loadFile(QString(FILE_TEST)) ) {
        return error(QString("opening test file: '%1' for indent:%2").arg(FILE_TEST).arg(indentation));
    }

    QBuffer outData ;
    Regola *regola = app.mainWindow()->getRegola();
    regola->setIndent(indentation);
    if(!regola->write(&outData, true)) {
        return error(QString("writing data indent: '%1'").arg(indentation));
    }
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
