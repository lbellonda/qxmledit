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

#include "testencodingutils.h"
#include "app.h"

TestEncodingUtils::TestEncodingUtils()
{
}

TestEncodingUtils::~TestEncodingUtils()
{
}

bool TestEncodingUtils::testOne(QXmlEditData *data, const QString &str, const QString &expected )
{
    QChar ch = str.at(0);
    QString found = data->unicodeHelper()->nameForChar(ch.unicode());
    if( found != expected ) {
        return error(QString("char: '%1' expected '%2', found '%3'").arg(ch).arg(expected).arg(found));
    }
    return true;
}

bool TestEncodingUtils::testBase()
{
    _testName = "testName";
    App app;
    if(!app.init()) {
        return error("init");
    }


    if( !testOne(app.data(), "a", "LATIN SMALL LETTER A" )) {
        return false ;
    }

    if( !testOne(app.data(),  QString::fromUtf8("\u00C0"), "LATIN CAPITAL LETTER A WITH GRAVE" )) {
        return false ;
    }

    if( !testOne(app.data(), QString::fromUtf8("\u010C"), "LATIN CAPITAL LETTER C WITH CARON" )) {
        return false ;
    }
    return true ;
}
