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

bool TestUtils::testFast()
{
    _testName = "testFast" ;
    if(!testRFC4288()) {
        return false;
    }
    return true ;
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

bool TestUtils::iTestRFC4288(const QString &name, const bool expected)
{
    if(expected != Utils::isRegNameRFC4288(name) ) {
        return error(QString("Test failed for name:'%1' result is:%2").arg(name).arg(!expected));
    }
    return true ;
}

bool TestUtils::testRFC4288()
{
    _testName = "testRFC4288" ;
    if(!iTestRFC4288("abcd", true)) {
        return false ;
    }
    if(!iTestRFC4288("abcd98271378AZAA", true)) {
        return false ;
    }
    if(!iTestRFC4288("09azAZ!#$&.+-^_", true)) {
        return false ;
    }
    if(!iTestRFC4288("09az[]AZ!#$&.+-^_", false)) {
        return false ;
    }
    if(!iTestRFC4288("", false)) {
        return false ;
    }
    QString s128 = QString::fill('a', 128);
    if(!iTestRFC4288(s128, false)) {
        return false ;
    }
    return true ;
}
