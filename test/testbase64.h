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

#ifndef TESTBASE64_H
#define TESTBASE64_H

#include "testbase.h"
#include "modules/utils/base64utils.h"

class TestBase64 : public TestBase
{
    bool _showXML;

    bool test_base64_utils_binary(const Base64Utils::EBase64 type);
    bool test_base64_utils_text();
    bool testUnitUtilsEncode();
    bool testUnitUtilsDecode();
    bool testUnitUtilsEncode(const Base64Utils::EBase64 type );
    bool testUnitUtilsDecode(const Base64Utils::EBase64 type );
    bool testUnitUtilsSaveDecoded();
    bool testUnitLoadEncoded();
    bool testIO(const Base64Utils::EBase64 type);
public:
    TestBase64();

    bool testUnits();
    bool test_base64();
    bool test_base64_base();
    bool test_base64_file();
    bool test_base64_utils();
};

#endif // TESTBASE64_H
