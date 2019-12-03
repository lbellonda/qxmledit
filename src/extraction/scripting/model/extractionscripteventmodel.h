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

#ifndef EXTRACTIONSCRIPTEVENTMODEL_H
#define EXTRACTIONSCRIPTEVENTMODEL_H

#include "xmlEdit.h"

enum EExtractionEventType {
    ExtractionEventNone,
    ExtractionEventText,
    ExtractionEventElement
};

class ExtractionScriptEventModel
{
    bool _enabled;
    QString _handlerName;
    QString _code;
    QString _description;
    EExtractionEventType _eventType;

    EExtractionEventType decodeTypeFromString(const QString &eventTypeNameString);
public:
    ExtractionScriptEventModel();
    ~ExtractionScriptEventModel();

    bool enabled() const;
    void setEnabled(bool enabled);
    QString description() const;
    void setDescription(const QString &description);
    QString code() const;
    void setCode(const QString &code);
    QString handlerName() const;
    void setHandlerName(const QString &handlerName);
    EExtractionEventType eventType() const;
    void setEventType(const EExtractionEventType &eventType);
    //
    bool scanEventFromDom(const QDomElement &element);
};

#endif // EXTRACTIONSCRIPTEVENTMODEL_H
