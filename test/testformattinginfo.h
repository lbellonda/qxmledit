/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2017 by Luca Bellonda and individual contributors       *
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

#ifndef TESTFORMATTINGINFO_H
#define TESTFORMATTINGINFO_H

#include "testbase.h"
#include "regola.h"

class App;

class TestFormattingInfo : public TestBase
{
    bool testSetup();
    bool testRead();
    bool testSetReset();
    bool testWrite();

    bool testBaseLoad(const QString &fileName, const bool expected,
                          bool useIndent, const int indent,
                          const Regola::ESaveAttributes saveAttrMethod,
                          const QXmlEditData::EIndentAttributes indentAttributesSetting, const int indentAttributesColumns);

    bool loadNoData();
    bool loadOnlyFormatting();
    bool loadOnlyFormattingAndMetadata();
    bool loadOnlyMetadata();
    bool loadAllConfigurations();

public:
    TestFormattingInfo();
    ~TestFormattingInfo();

    bool testFast();
    bool testUnit();
};

#endif // TESTFORMATTINGINFO_H
