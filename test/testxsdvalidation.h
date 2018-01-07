/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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


#ifndef TESTXSDVALIDATION_H
#define TESTXSDVALIDATION_H


#include "testbase.h"

#include <xsdeditor/xsdloadcontext.h>

class TestXSDValidation : public TestBase
{
protected:
    XSDLoadContext *setupLoadContext();
    bool testLoad(XSDLoadContext *loadContext, const QString &fileIn);
    bool returnCondition(XSDLoadContext *loadContext, const bool result );
    bool testExec(const QString &thisTestName, const EXSDLoadError errorExpected, const QString &fileIn);
    QString firstMessage(XSDLoadContext *loadContext);


    bool testAttributeOK();
    bool testAttributeOKRule2();
    bool testAttributeRule1Internal();
    bool testAttributeRule1Top();
    bool testAttributeRule2Internal();
    bool testAttributeRule2Top();
    bool testAttributeRule31Internal();
    bool testAttributeRule32Internal();
    bool testAttributeRule6();
    //----
    bool testElementOK();
    bool testElement1();
    bool testElementInternal();
    bool testElementInternalRule2_2();
    bool testElementInternalRule3();
    //---
    bool testAnonType();
    bool testTypeOK();
    bool testTopType();
    //----
    bool loadAndTestValidation(const QString &inputFileName, const QString &fileXSDUrl, const bool expectedResult );
    bool testValidateSchemaOK();
    bool testValidateElement();
    bool testValidateAttribute();

public:
    TestXSDValidation();
    ~TestXSDValidation();

    bool testAttributes();
    bool testElements();
    bool testTypes();
    bool testLast();
    bool testValidateSchema();
};

#endif // TESTXSDVALIDATION_H
