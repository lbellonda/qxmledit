/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2022 by Luca Bellonda and individual contributors  *
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

#include "xmlanonutils.h"
#include "modules/anonymize/anoncontext.h"

XmlAnonUtils::XmlAnonUtils()
{
}

XmlAnonUtils::~XmlAnonUtils()
{
}

//-------

QString XmlAnonUtils::anonymizeTextOfElement(AnonContext *context, const QString &inputText, void * key)
{
    QString result = inputText ;
    AnonContextText thisContext(context);
    thisContext.pushContextNamespaceText();
    AnonException *exception = thisContext.getException();
    if(thisContext.canAnonymize(exception)) {
        if(context->isCollectingData()) {
            context->setOrigData(key, inputText);
        }
        QString newData = thisContext.anonymize(exception, inputText) ;
        result = newData ;
    }
    return result ;
}

void XmlAnonUtils::scanTextOfElement(AnonContext *context, const QString &inputText)
{
    AnonContextText thisContext(context);
    thisContext.pushContextNamespaceText();
    AnonException *exception = thisContext.getException();
    if(thisContext.canAnonymize(exception)) {
        thisContext.scanAnonymize(exception, inputText);
    }
}

bool XmlAnonUtils::setTypeIfNotSet(AnonProducer::ESeqType &type, const AnonProducer::ESeqType newType)
{
    if(newType != AnonProducer::ASCII) {
        if(newType > type) {
            type = newType;
            return true;
        }
    }
    return false;
}

