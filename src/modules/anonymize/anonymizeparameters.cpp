/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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


#include "anonymizeparameters.h"
#include "xmlutils.h"

#define     ATTR_MODE   "mode"
#define     ATTR_USEFIXEDLETTER "useFixedLetter"


AnonymizeParameters::AnonymizeParameters()
{
    mode = UsingPatterns;
    useFixedLetter = false ;
}

AnonymizeParameters::AnonymizeParameters(const Emodes newMode, const bool newUseFixedLetter)
{
    mode = newMode;
    useFixedLetter = newUseFixedLetter ;
}

AnonymizeParameters::~AnonymizeParameters()
{
}


bool AnonymizeParameters::readFromDom(const QDomElement &element)
{
    bool result = true;

    mode = (Emodes)XmlUtils::readFromInt(element.attribute(ATTR_MODE), mode);
    useFixedLetter = XmlUtils::readFromBool(element.attribute(ATTR_USEFIXEDLETTER), useFixedLetter);
    return result;
}

bool AnonymizeParameters::saveToDom(QDomElement &element)
{
    bool result = true;
    element.setAttribute(ATTR_MODE, XmlUtils::intToStringValue(mode));
    element.setAttribute(ATTR_USEFIXEDLETTER, XmlUtils::boolToBoolValue(useFixedLetter));
    return result;
}


bool AnonymizeParameters::equals(AnonymizeParameters *other)
{
    bool isEqual = true ;
    isEqual = (mode == other->mode) && (useFixedLetter == other->useFixedLetter);
    return isEqual  ;
}

void AnonymizeParameters::copyValuesFrom(AnonymizeParameters *other)
{
    mode = other->mode ;
    useFixedLetter = other->useFixedLetter ;
}
