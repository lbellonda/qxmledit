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

#include "extractionscripteventmodel.h"
#include "xmlutils.h"
#include "utils.h"

static int instancesCount = 0;

//
#define ATTR_TYPE           "type"
#define ATTR_ENABLED        "enabled"
#define ATTR_HANDLER        "handler"
#define ATTR_DESCRIPTION    "description"
#define ELM_CODE            "code"
//
#define EVENT_TEXT_STRING       "text"
#define EVENT_ELEMENT_STRING    "element"
//

ExtractionScriptEventModel::ExtractionScriptEventModel()
{
    _enabled = false;
    _eventType = ExtractionEventNone ;
    instancesCount++;
}

ExtractionScriptEventModel::~ExtractionScriptEventModel()
{
    instancesCount--;
    Q_ASSERT(instancesCount > -1);
}

bool ExtractionScriptEventModel::enabled() const
{
    return _enabled;
}

void ExtractionScriptEventModel::setEnabled(bool enabled)
{
    _enabled = enabled;
}

QString ExtractionScriptEventModel::description() const
{
    return _description;
}

void ExtractionScriptEventModel::setDescription(const QString &description)
{
    _description = description;
}

QString ExtractionScriptEventModel::code() const
{
    return _code;
}

void ExtractionScriptEventModel::setCode(const QString &code)
{
    _code = code;
}

QString ExtractionScriptEventModel::handlerName() const
{
    return _handlerName;
}

void ExtractionScriptEventModel::setHandlerName(const QString &handlerName)
{
    _handlerName = handlerName;
}


EExtractionEventType ExtractionScriptEventModel::eventType() const
{
    return _eventType;
}

void ExtractionScriptEventModel::setEventType(const EExtractionEventType &eventType)
{
    _eventType = eventType;
}

EExtractionEventType ExtractionScriptEventModel::decodeTypeFromString(const QString &eventTypeNameString)
{
    if(eventTypeNameString == EVENT_TEXT_STRING) {
        return ExtractionEventText;
    } else if(eventTypeNameString == EVENT_ELEMENT_STRING) {
        return ExtractionEventElement ;
    }
    return ExtractionEventNone;
}

bool ExtractionScriptEventModel::scanEventFromDom(const QDomElement &element)
{
    const QString eventTypeNameString = element.attribute(ATTR_TYPE);
    const QString eventEnabledString = element.attribute(ATTR_ENABLED);
    const QString eventHandlerString = element.attribute(ATTR_HANDLER);
    const QString eventDescriptionString = element.attribute(ATTR_DESCRIPTION);

    _eventType = decodeTypeFromString(eventTypeNameString);
    if(ExtractionEventNone == _eventType) {
        return false ;
    }
    _enabled = XmlUtils::readFromBool(eventEnabledString, false);
    _handlerName = eventHandlerString;
    _description = eventDescriptionString;

    int nodi = element.childNodes().size();

    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = element.childNodes().item(i) ;
        if(childNode.isElement()) {
            QDomElement childElement = childNode.toElement();
            if(ELM_CODE == childElement.tagName()) {
                _code = childElement.text();
            }
        }
    }
    return true;
}
