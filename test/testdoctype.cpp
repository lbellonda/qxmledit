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

#include "testdoctype.h"
#include "comparexml.h"

#define INPUT_TYPE  "../test/data/dtd/doctype.xml"
#define INPUT_SYSTEM    "../test/data/dtd/doctype_system.xml"
#define INPUT_PUBLIC    "../test/data/dtd/doctype_public.xml"

#define OUTPUT_TYPE  "../test/data/dtd/doctype_output.xml"
#define OUTPUT_SYSTEM    "../test/data/dtd/doctype_system_output.xml"
#define OUTPUT_PUBLIC    "../test/data/dtd/doctype_public_output.xml"


TestDocType::TestDocType()
{
}

bool TestDocType::error()
{
    return false ;
}

bool TestDocType::test()
{
    if( !testType() ) {
        return error();
    }
    if( !testPublic() ) {
        return error();
    }
    if( !testSystem() ) {
        return error();
    }
    return true;
}

bool TestDocType::compareDocuments(const QString &filename, Regola *regola)
{
    QByteArray resultData = regola->writeMemory();
    QDomDocument document1;
    QDomDocument document2;
    CompareXML compare;
    if(!compare.loadFileIntoDocument(filename, document1)) {
        return error();
    }
    QBuffer outputData(&resultData);
    if(!compare.loadFileIntoDocument(&outputData, document2)) {
        return error();
    }
    bool result = compare.compareDomDocuments(document1, document2);
    if( !result ) {
        return error();
    }
    return result ;
}

bool TestDocType::go(const QString &fileNameIn, const QString &fileNameOut)
{
    if(!app.init() ) {
        return error();
    }
    app.mainWindow()->loadFile(fileNameIn);
    Regola *regola = app.mainWindow()->getRegola();

    if(!compareDocuments(fileNameOut, regola) ) {
        return error();
    }

    return true;
}


bool TestDocType::testType()
{
    return go(INPUT_TYPE, OUTPUT_TYPE);
}

bool TestDocType::testPublic()
{
    return go(INPUT_PUBLIC, OUTPUT_PUBLIC);
}

bool TestDocType::testSystem()
{
    return go(INPUT_SYSTEM, OUTPUT_SYSTEM);
}
