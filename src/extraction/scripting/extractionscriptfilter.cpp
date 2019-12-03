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

#include "extractionscriptfilter.h"
#include <QtScript/QScriptValue>
#include "utils.h"

//------------------------------------------------------------

ScriptError::ScriptError()
{
    reset();
}

void ScriptError::reset()
{
    isError = false ;
    errorLine = -1 ;
    errorMessage = "" ;
    errorStack = "" ;
    errorString = "" ;
}

QString ScriptError::toString()
{
    if(isError) {
        return QObject::tr("%1, at line %2").arg(errorMessage).arg(errorLine);
    }
    return QObject::tr("No error");
}

QString ScriptError::toStringExtended()
{
    if(isError) {
        return QObject::tr("%1, %2\nstack trace:\n %3").arg(toString()).arg(errorString).arg(errorStack);
    }
    return QObject::tr("No error");
}

//------------------------------------------------------------

const QString ExtractionScriptFilter::ContextName("context");
const QString ExtractionScriptFilter::TestContextDouble("contextDouble");
const QString ExtractionScriptFilter::EventName("event");

int ExtractionScriptFilter::instancesCount = 0 ;

ExtractionScriptFilter::ExtractionScriptFilter()
{
    _enabled = false;
    QString _errorString;
    instancesCount++;
}

ExtractionScriptFilter::~ExtractionScriptFilter()
{
    reset();
    instancesCount--;
}

void ExtractionScriptFilter::reset()
{
#ifdef  QXMLEDIT_JS_SCRIPT
    foreach(const QString& name, _registeredObjects) {
        QJSValue nullProxy("NULL");
        _engine.globalObject().setProperty(name, nullProxy);
    }
#endif
    _events.clear();
    _registeredObjects.clear();
    EMPTYPTRLIST(_eventsSet, ExtractionScriptEventHandler)
}

QList<ExtractionScriptEventHandler*> ExtractionScriptFilter::allEvents()
{
    return _eventsSet.toList();
}

int ExtractionScriptFilter::eventsCount()
{
    return _eventsSet.size();
}

void ExtractionScriptFilter::registerContext(ExtractionOperationScriptContext *context)
{
    registerObject(ExtractionScriptFilter::ContextName, context);
}

void ExtractionScriptFilter::registerEvent(QObject *event)
{
    registerObject(ExtractionScriptFilter::EventName, event);
}

#ifdef QXMLEDIT_JS_SCRIPT

void ExtractionScriptFilter::registerObject(const QString &name, QObject *object)
{
    QJSValue proxy = _engine.newQObject(object);
    _engine.globalObject().setProperty(name, proxy);
    _registeredObjects.insert(name);
    Utils::TODO_NEXT_RELEASE("make read only");
}

bool ExtractionScriptFilter::executeScript(ScriptError &errors, const QString &code)
{
    errors.reset();
    QJSValue result = _engine.evaluate(code);
    return handleError(errors, result);
}

bool ExtractionScriptFilter::handleError(ScriptError &errors, const QJSValue &value)
{
    if(value.isError()) {
        errors.isError = true ;
        errors.errorLine = value.property("lineNumber").toInt() ;
        errors.errorMessage = value.property("message").toString() ;
        errors.errorStack = value.property("stack").toString() ;
        errors.errorString = value.toString();
        return false;
    } else {
        errors.reset();
    }
    return true ;
}
#else
bool ExtractionScriptFilter::executeScript(ScriptError &errors, const QString &code)
{
    Q_UNUSED(errors)
    Q_UNUSED(code)
    return false;
}

void ExtractionScriptFilter::registerObject(const QString &name, QObject *object)
{
    Q_UNUSED(name)
    Q_UNUSED(object)
}

#endif

bool ExtractionScriptFilter::evaluateEvent(ScriptError &errors, QObject *event, const EExtractionEventType eventType)
{
    errors.reset();
    if(isEnabled()) {
        ExtractionScriptEventHandler* eventHandler = _events[eventType];
        if(NULL != eventHandler) {
            registerEvent(event);
            return executeScript(errors, eventHandler->handlerCall());
        }
    }
    return true;
}

bool ExtractionScriptFilter::setModel(ExtractionScriptFilterModel *newHandler)
{
    bool added = false ;
    if(NULL != newHandler) {
        if(newHandler->enabled()) {
            reset();
            foreach(ExtractionScriptEventModel* eventModel, newHandler->events()) {
                if(eventModel->enabled()) {
                    ExtractionScriptEventHandler *handler = ExtractionScriptEventHandler::fromModel(eventModel);
                    _events.insert(handler->eventType(), handler);
                    _eventsSet.insert(handler);
                    _enabled = true ;
                }
            }
            added = true ;
        }
        delete newHandler;
    }
    return added ;
}

bool ExtractionScriptFilter::isEnabled()
{
    return _enabled ;
}

bool ExtractionScriptFilter::initScripting(ScriptError &errors)
{
    errors.reset();
    foreach(ExtractionScriptEventHandler *handler, _events.values()) {
        if(NULL == handler) {
            continue ;
        }
#ifdef QXMLEDIT_JS_SCRIPT
        // load the code into the engine
        QJSValue result = _engine.evaluate(handler->eventScript());
        if(!handleError(errors, result)) {
            return false;
        }
#endif
    }
    return true ;
}

bool ExtractionScriptFilter::evaluateTextEvent(ScriptError &errors, ExtractionScriptTextEvent *textEvent)
{
    return evaluateEvent(errors, textEvent, ExtractionEventText);
}

bool ExtractionScriptFilter::evaluateElementEvent(ScriptError &errors, ExtractionScriptElementEvent *elementEvent)
{
    return evaluateEvent(errors, elementEvent, ExtractionEventElement);
}

bool ExtractionScriptFilter::isTextEventEnabled()
{
    ExtractionScriptEventHandler *handler = _events[ExtractionEventText];
    return (handler != NULL);
}

bool ExtractionScriptFilter::isElementEventEnabled()
{
    ExtractionScriptEventHandler *handler = _events[ExtractionEventElement];
    return (handler != NULL);
}
