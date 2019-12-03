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

#ifndef TESTSPLITSCRIPTINGOPERATIONHELPER_H
#define TESTSPLITSCRIPTINGOPERATIONHELPER_H

#include "xmlEdit.h"
#include "extraction/extractionoperation.h"

class TestSplitScriptingOperationHelper
{
public:
    ExtractResults results;
    ExtractionOperation op;
    ExtractionScriptManager::EEventResult opResult;
    ExtractionScriptTextEvent textEvent;
    ExtractionScriptElementEvent elementEvent;

    TestSplitScriptingOperationHelper();
    ~TestSplitScriptingOperationHelper();

    static QXmlStreamAttributes makeAttributes(QList<QPair<QString, QString> > &attributes);
    void executeMultipleText(const QString &eventCode1, const QString &eventCode2, const int level, const QString &path, const QString &text);
    void executeMultipleElement(const QString &code1,
                                const QString &code2,
                          const QString &elementName,
                          const QString &nameSpace,
                          const QString &localName,
                          QList<QPair<QString, QString> > &attributes);

    void setFilterModel(ExtractionScriptFilterModel *newHandler);
};

#endif // TESTSPLITSCRIPTINGOPERATIONHELPER_H
