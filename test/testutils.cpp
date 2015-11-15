/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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

#include "testutils.h"
#include "utils.h"

TestUtils::TestUtils()
{
}

TestUtils::~TestUtils()
{
}

bool TestUtils::testUnit()
{
    _testName = "testUnit" ;
    if(!testNormalizeFileName()) {
        return false;
    }
    return true ;
}

bool TestUtils::testNormalizeFileName(const QString &src, const QString &expected)
{
    QString output = Utils::normalizeFilePath(src) ;
    if( output != expected ) {
        return error(QString("testNormalizeFileName expected:'%1' found:'%2'").arg(output).arg(expected));
    }
    return true ;
}

bool TestUtils::testNormalizeFileName()
{
    _testName = "testNormalizeFileName" ;
    if( !testNormalizeFileName("", "") ) {
        return false;
    }
    return true ;
}
