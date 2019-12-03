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

#ifndef TESTSPLITSCRIPTINGHELPER_H
#define TESTSPLITSCRIPTINGHELPER_H

class ExtractionScriptFilterModel ;
class ExtractionScriptEventModel ;
class ExtractionScriptEventHandler;
class ExtractionScriptFilter;

#include "xmlEdit.h"
#include "extraction/scripting/extractionscriptfilter.h"

class TestSplitScriptingHelper
{
    public:
    QList<ExtractionScriptEventHandler*> _allEventHandlersForTest;
    //-----
    ExtractionScriptFilter filter;
    ExtractionScriptFilterModel *filterModel;
    QList<ExtractionScriptEventHandler*> expectedHandlers;
    ExtractionScriptEventModel *createBaseEventModelList( const EExtractionEventType eventType, const bool enabled, const QString &eventHandlerName, const QString &eventCode);
    void applyModelList();

    TestSplitScriptingHelper(bool createFilterModel);
    ~TestSplitScriptingHelper();

    static ExtractionScriptFilterModel *sampleFilter();
    static ExtractionScriptEventModel *baseEventModel(const EExtractionEventType eventType, const bool enabled, const QString &eventHandlerName, const QString &eventCode);
    static ExtractionScriptFilterModel *filterForModel(ExtractionScriptEventModel *model);
    static ExtractionScriptEventHandler *baseEventHanlder(const EExtractionEventType eventType, const QString &eventHandlerName, const QString &eventCode);
    static ExtractionScriptFilterModel *simpleFilter();
    static ExtractionScriptEventModel *simpleExtractionScriptFilterModel();
    static ExtractionScriptFilterModel *newFilterModel();

};

#endif // TESTSPLITSCRIPTINGHELPER_H
