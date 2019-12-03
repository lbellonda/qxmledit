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

#include "extractionscripteventhandler.h"
#include "utils.h"

ExtractionScriptEventHandler::ExtractionScriptEventHandler()
{
    _eventType = ExtractionEventNone ;
    _ready = false ;
}

ExtractionScriptEventHandler::~ExtractionScriptEventHandler()
{
}

bool ExtractionScriptEventHandler::equalsTo(const ExtractionScriptEventHandler* const other)
{
    if(NULL == other) {
        return false;
    }
    return ((other->_eventType == _eventType)
            && (other->_eventName == _eventName)
            && (other->_eventScript == _eventScript));
}

QString ExtractionScriptEventHandler::toString()
{
    return QString("Event type %1 %2").arg(_eventType).arg(_eventName);
}

ExtractionScriptEventHandler *ExtractionScriptEventHandler::fromModel(ExtractionScriptEventModel *eventModel)
{
    ExtractionScriptEventHandler *handler = new ExtractionScriptEventHandler();
    handler->setEventHandler(eventModel->handlerName());
    handler->setEventType(eventModel->eventType());
    handler->setEventScript(eventModel->code());
    return handler ;
}
//------------------

EExtractionEventType ExtractionScriptEventHandler::eventType() const
{
    return _eventType;
}

void ExtractionScriptEventHandler::setEventType(const EExtractionEventType eventType)
{
    _eventType = eventType;
}

QString ExtractionScriptEventHandler::eventName() const
{
    return _eventName;
}

void ExtractionScriptEventHandler::setEventName(const QString &eventName)
{
    _eventName = eventName;
}

QString ExtractionScriptEventHandler::eventScript() const
{
    return _eventScript;
}

void ExtractionScriptEventHandler::setEventScript(const QString &eventScript)
{
    _ready = false ;
    _eventScript = eventScript;
}


QString ExtractionScriptEventHandler::eventHandler() const
{
    return _eventHandler;
}

void ExtractionScriptEventHandler::setEventHandler(const QString &eventHandler)
{
    _ready = false ;
    _eventHandler = eventHandler;
}

QString ExtractionScriptEventHandler::handlerCall()
{
    if(!_ready) {
        _ready = true ;
        _eventHandlerCall = QString("%1(context, event);").arg(_eventHandler);
    }
    return _eventHandlerCall;
}
