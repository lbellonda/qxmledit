/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2019 by Luca Bellonda and individual contributors       *
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

#ifndef TESTEXTRACTIONEXECUTORHELPER_H
#define TESTEXTRACTIONEXECUTORHELPER_H

#include "extraction/scripting/events/extractionoperationscriptcontext.h"
#include "extraction/scripting/extractionscriptfilter.h"
#include "helpers/testwritableextractionoperationscriptcontext.h"
#include "extraction/scripting/events/extractionscripttext.h"
#include "extraction/scripting/events/extractionscriptelement.h"

class TestExtractionExecutorHelper
{
public:
    ScriptError _error;
    ExtractionOperationScriptContext *reference ;
    TestWritableExtractionOperationScriptContext *compare ;
    ExtractionScriptFilter filter;
    ExtractionScriptTextEvent *textEvent;
    ExtractionScriptElementEvent *elementEvent;

    TestExtractionExecutorHelper();
    ~TestExtractionExecutorHelper();

    bool execute(const QString &code);
    bool executeText(const QString &code, bool isCDATA, const QString &inputText);
    bool executeElement(const QString &code,
                          const QString &elementName,
                          const QString &nameSpace,
                          const QString &localName,
                          QList<QPair<QString,QString> > attributes);
};

#endif // TESTEXTRACTIONEXECUTORHELPER_H
