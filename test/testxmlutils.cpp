/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2018 by Luca Bellonda and individual contributors  *
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

#include "testxmlutils.h"
#include "xmlutils.h"

TestXmlUtils::TestXmlUtils()
{
}

TestXmlUtils::~TestXmlUtils()
{
}

bool TestXmlUtils::testUnits()
{
    _testName = "testUnits";
    if(!testParsing()) {
        return false;
    }

    if(!testNsPrefix()) {
        return false;
    }

    if(!testDecodeQualifiedName()) {
        return false;
    }
    return true;
}


bool TestXmlUtils::innerTestNsPrefix(const QString &name, const QString &expectedPrefix, const bool expectedResult)
{
    QString prefix;
    bool result = XmlUtils::getNsPrefix(name, prefix);
    if( result != expectedResult ) {
        return error(QString("getNsPrefix for:'%1', expected:%2, result:%3").arg(name).arg(expectedResult).arg(result));
    }
    if(expectedResult) {
        if( prefix != expectedPrefix ) {
            return error(QString("getNsPrefix for:'%1', expectedPrefix:'%2'', result:'%3'").arg(name).arg(expectedPrefix).arg(prefix));
        }
    }
    return true;
}

bool TestXmlUtils::testNsPrefix()
{
    _testName = "testUnits/testNsPrefix";
    if(!innerTestNsPrefix("abc", "", false) ) {
        return false;
    }
    if(!innerTestNsPrefix("xmlns", "", true) ) {
        return false;
    }
    if(!innerTestNsPrefix("xmlns:a", "a", true) ) {
        return false;
    }
    if(!innerTestNsPrefix("xmlns:a:a", "a:a", false) ) {
        return false;
    }
    return true;
}

bool TestXmlUtils::innerTestDecodeLocalName(const QString &name, const QString &expectedPrefix, const QString &expectedLocalName)
{
    QString prefix;
    QString localName;
    XmlUtils::decodeQualifiedName(name, prefix, localName);
    if( prefix != expectedPrefix ) {
        return error(QString("decodeQualifiedName for:'%1', expectedPrefix:'%2'', result:'%3'").arg(name).arg(expectedPrefix).arg(prefix));
    }
    if( localName != expectedLocalName ) {
        return error(QString("decodeQualifiedName for:'%1', expectedLocalName:'%2'', result:'%3'").arg(name).arg(expectedLocalName).arg(localName));
    }
    return true;
}

bool TestXmlUtils::testDecodeQualifiedName()
{
    _testName = "testUnits/testDecodeQualifiedName";
    if(!innerTestDecodeLocalName("abc", "", "abc") ) {
        return false;
    }
    if(!innerTestDecodeLocalName(":abc", "", "abc") ) {
        return false;
    }
    if(!innerTestDecodeLocalName("a:abc", "a", "abc") ) {
        return false;
    }
    if(!innerTestDecodeLocalName("a:a:abc", "a", "a:abc") ) {
        return false;
    }
    return true;
}

bool TestXmlUtils::checkIs(const QChar &ch, const bool expectedResult)
{
    const bool result = XmlUtils::isS(ch);
    if(result != expectedResult) {
        return error(QString("Decode '%1' expected %2, found %3")
                     .arg(QString::number(ch.unicode(), 16))
                     .arg(expectedResult)
                     .arg(result));
    }
    return true ;
}

bool TestXmlUtils::testParsing()
{
    _testName = "testUnits/testParsing";
    if(!checkIs(0x20, true) ) {
        return false ;
    }
    if(!checkIs(0x09, true) ) {
        return false ;
    }
    if(!checkIs(0x0D, true) ) {
        return false ;
    }
    if(!checkIs(0x0A, true) ) {
        return false ;
    }
    if(!checkIs(0x21, false) ) {
        return false ;
    }
    if(!checkIs('a', false) ) {
        return false ;
    }
    if(!checkIs('[', false) ) {
        return false ;
    }
    return true;
}

