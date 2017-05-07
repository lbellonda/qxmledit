/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2016 by Luca Bellonda and individual contributors  *
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


#ifndef TESTINDENT_H
#define TESTINDENT_H

#include <QBuffer>
#include "testbase.h"

class App;

class TestIndent : public TestBase
{
    int _indentationTest1 ;
    bool _useDocIndent;
    int _indentationDoc;

    bool saveAndCompare(const QString &caseId, const int appIndentation, const int indentation, bool (TestIndent::*func) (App *app, QBuffer *outData) );
    bool testWithSettings(const int caseNo, const int indentation, const int appIndent, const bool useDocIndent, const int docIndent );

    bool writeSettings(App *app, QBuffer *outData);
    bool simpleWrite(App *app, QBuffer *outData);

    //---
    bool testSettingsFromDialog();
    bool testSettingsFromPreferences();
    //---
    bool testIndentAttributesUnit();
    bool testIndentAttributesIndent();
    bool testIndentAttributesWithoutDefaultValues();
    bool testIndentAttributesWithDefaultValues();
    //--
    bool testIndentAttributesWithDefaultValuesInner(const int indentValue);
    bool testIndentAttributesWithoutDefaultValuesInner(const int indentValue);
    bool testIndentAttributesIndentInner(const int indentValue, const int attributeIndentation);
    bool saveAndCompareAttributesIndentation(const QString &caseId,
                                             //--
                                             const int appIndentation,
                                             const int specificIndentation,
                                             const bool useSpecificIndentation, const bool useIndentation,
                                             //--
                                             const bool setDefaultValues,
                                             const int attributeIndentationDefaultValue,
                                             const bool setSpecificValue,
                                             const int attributeIndentationSpecificValue,
                                             // --
                                             const QString &fileReference );
    bool testOnePreset(const bool isEmpty, const QString &fileReference, const XMLIndentationSettings::ESettings preset);
    bool checkFileWithRegola(const QString &id, Regola* regola, const QString &fileReference);
    bool testPreset();
public:
    TestIndent();
    ~TestIndent();

    bool testSaving();
    bool testSettings();
    bool testFast();
    bool testIndentAttributes();
};

#endif // TESTINDENT_H
