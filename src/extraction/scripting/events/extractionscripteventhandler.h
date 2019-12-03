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

#ifndef EXTRACTIONSCRIPTEVENTHANDLER_H
#define EXTRACTIONSCRIPTEVENTHANDLER_H

#include "xmlEdit.h"
#include "extraction/scripting/model/extractionscripteventmodel.h"

/**
 * @brief this class holds the event definition
 */
class ExtractionScriptEventHandler
{
    EExtractionEventType _eventType;
    QString _eventName;
    QString _eventScript;
    QString _eventHandler;
    QString _eventHandlerCall;
    bool _ready;
public:
    ExtractionScriptEventHandler();
    ~ExtractionScriptEventHandler();
    bool equalsTo(const ExtractionScriptEventHandler* const other);
    QString toString();

    EExtractionEventType eventType() const;
    void setEventType(const EExtractionEventType eventType);
    QString eventName() const;
    void setEventName(const QString &eventName);
    QString eventScript() const;
    void setEventScript(const QString &eventScript);
    QString eventHandler() const;
    void setEventHandler(const QString &eventHandler);
    //
    QString handlerCall();
    static ExtractionScriptEventHandler *fromModel(ExtractionScriptEventModel *eventModel);
};

#endif // EXTRACTIONSCRIPTEVENTHANDLER_H
