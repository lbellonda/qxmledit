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

#include "testsplitscriptinghelper.h"

TestSplitScriptingHelper::TestSplitScriptingHelper(bool createFilterModel)
{
    if(createFilterModel) {
        filterModel = newFilterModel();
    } else {
        filterModel = NULL ;
    }
}

TestSplitScriptingHelper::~TestSplitScriptingHelper()
{
    EMPTYPTRLIST(_allEventHandlersForTest, ExtractionScriptEventHandler);
}

ExtractionScriptFilterModel *TestSplitScriptingHelper::newFilterModel()
{
    ExtractionScriptFilterModel* filter = new ExtractionScriptFilterModel();
    filter->setName("test");
    filter->setEnabled(true);
    return filter;
}

void TestSplitScriptingHelper::applyModelList()
{
    filter.setModel(filterModel);
    filterModel = NULL ;
}

ExtractionScriptEventModel *TestSplitScriptingHelper::createBaseEventModelList( const EExtractionEventType eventType, const bool enabled, const QString &eventHandlerName, const QString &eventCode)
{
    ExtractionScriptEventModel *eventModel = baseEventModel(eventType, enabled, eventHandlerName, eventCode);
    filterModel->addEventModel(eventModel);
    if(enabled) {
        ExtractionScriptEventHandler *handler = baseEventHanlder(eventType, eventHandlerName, eventCode);
        expectedHandlers.append(handler);
        _allEventHandlersForTest.append(handler);
    }
    return eventModel ;
}

ExtractionScriptFilterModel *TestSplitScriptingHelper::simpleFilter()
{
    ExtractionScriptFilterModel* filter = new ExtractionScriptFilterModel();
    filter->setName("test");
    filter->setEnabled(true);

    filter->addEventModel(simpleExtractionScriptFilterModel());
    return filter;
}

ExtractionScriptFilterModel *TestSplitScriptingHelper::filterForModel(ExtractionScriptEventModel *model)
{
    ExtractionScriptFilterModel* filter = new ExtractionScriptFilterModel();
    filter->setName("test");
    filter->setEnabled(true);

    filter->addEventModel(model);
    return filter;
}

ExtractionScriptEventModel *TestSplitScriptingHelper::baseEventModel(const EExtractionEventType eventType, const bool enabled, const QString &eventHandlerName, const QString &eventCode)
{
    ExtractionScriptEventModel* event = new ExtractionScriptEventModel();
    event->setEnabled(enabled);
    event->setHandlerName(eventHandlerName);
    event->setEventType(eventType);
    event->setCode(eventCode);
    return event;
}

ExtractionScriptEventHandler *
TestSplitScriptingHelper::baseEventHanlder(const EExtractionEventType eventType, const QString &eventHandlerName, const QString &eventCode)
{
    ExtractionScriptEventHandler* event = new ExtractionScriptEventHandler();
    event->setEventHandler(eventHandlerName);
    event->setEventType(eventType);
    event->setEventScript(eventCode);
    return event;
}

ExtractionScriptEventModel *TestSplitScriptingHelper::simpleExtractionScriptFilterModel()
{
    ExtractionScriptEventModel * filterEvent = baseEventModel(ExtractionEventText, true, "theHandler", "function theHandler() {}");
    return filterEvent;
}
