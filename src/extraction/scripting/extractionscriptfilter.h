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

#ifndef EXTRACTIONSCRIPTFILTER_H
#define EXTRACTIONSCRIPTFILTER_H

#include "xmlEdit.h"
#include "extraction/scripting/model/extractionscriptfiltermodel.h"
#include "extraction/scripting/events/extractionscripteventhandler.h"
#include "extraction/scripting/events/extractionoperationscriptcontext.h"
#include "extraction/scripting/events/extractionscripttext.h"
#include "extraction/scripting/events/extractionscriptelement.h"

#ifdef  QXMLEDIT_JS_SCRIPT
#include <QJSEngine>
#include <QJSValue>
#endif

class ScriptError
{
public:
    bool isError;
    int errorLine ;
    QString errorMessage;
    QString errorStack;
    QString errorString;
    ScriptError();
    void reset();
    void copyTo(ScriptError *target);
    QString toString();
    QString toStringExtended();
};

class ExtractionScriptFilter
{
#ifdef  QXMLEDIT_JS_SCRIPT
    QJSEngine _engine;
#endif
    bool _enabled;
    QHash<int, ExtractionScriptEventHandler*> _events;
    QSet<ExtractionScriptEventHandler*> _eventsSet;
    QSet<QString> _registeredObjects;
    //---- errors
    //----end errors
    static int instancesCount;

    void reset();
    bool executeScript(ScriptError &errors, const QString &code);
    void registerObject(const QString &name, QObject *object);
#ifdef QXMLEDIT_JS_SCRIPT
    bool handleError(ScriptError &errors, const QJSValue &value);
#endif
    bool evaluateEvent(ScriptError &errors, QObject *event, const EExtractionEventType eventType);
public:
    ExtractionScriptFilter();
    ~ExtractionScriptFilter();

    bool isEnabled();
    int eventsCount();
    QList<ExtractionScriptEventHandler*> allEvents();
    bool setModel(ExtractionScriptFilterModel *newModel);
    void registerContext(ExtractionOperationScriptContext *context);
    void registerEvent(QObject *event);
    bool initScripting(ScriptError &errors);
    //----
    bool isTextEventEnabled();
    bool evaluateTextEvent(ScriptError &errors, ExtractionScriptTextEvent *textEvent);
    //----
    bool evaluateElementEvent(ScriptError &errors, ExtractionScriptElementEvent *elementEvent);
    bool isElementEventEnabled();
    //
    static const QString ContextName;
    static const QString TestContextDouble;
    static const QString EventName;
#ifdef QXMLEDIT_TEST
    friend class TestSplit;
    friend class TestExtractionExecutorHelper ;
#endif
};

#endif // EXTRACTIONSCRIPTFILTER_H
