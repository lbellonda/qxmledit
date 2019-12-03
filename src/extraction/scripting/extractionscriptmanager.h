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

#ifndef EXTRACTIONSCRIPTMANAGER_H
#define EXTRACTIONSCRIPTMANAGER_H

#include "xmlEdit.h"
#include "extraction/scripting/extractionscriptexecutor.h"
#include "extraction/scripting/events/extractionoperationscriptcontext.h"
#include "extraction/scripting/events/extractionscripttext.h"
#include "extraction/scripting/extractionscriptfilter.h"
#include "extraction/scripting/model/extractionscriptfiltermodel.h"

class ExtractionScriptManager
{
    QPointer<ExtractionOperationScriptContext> _context;
    ExtractionScriptContainer _container;
    QList<ExtractionScriptFilter*> _filters;
    ScriptError _error;
    bool _isReady ;
    bool _isEnabled;
    bool _isInitedOk;
    int _eventsCount;
    int _textEventsCount;
    int _elementEventsCount;

    void initTextEvent(ExtractionScriptTextEvent *textEvent);

public:
    ExtractionScriptManager();
    ~ExtractionScriptManager();

    enum EEventResult {
        EventResult_IgnoreEvent,
        EventResult_WriteOriginalData,
        EventResult_WriteModifiedData,
        EventResult_Error,
    };

    bool isScriptingEnabled();
    // call this before using
    bool initScripting();

    QString errorMessage();
    QString errorMessageExtended();

    int eventsCount();
    int textEventsCount();
    int elementEventsCount();

    bool isError();

    void addScriptingFilter(ExtractionScriptFilterModel *newFilter);

    //------- events ---
    EEventResult textEvent(const int level, const QString &path, ExtractionScriptTextEvent *textEvent);
    EEventResult elementEvent(const int level, const QString &path, ExtractionScriptElementEvent *elementEvent);
};

#endif // EXTRACTIONSCRIPTMANAGER_H
