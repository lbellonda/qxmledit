/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2022 by Luca Bellonda and individual contributors  *
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

#include "element.h"
#include "modules/delegates/elementviewinfo.h"
#include <regola.h>
#include "utils.h"
#include "modules/anonymize/anonexception.h"
#include "modules/anonymize/anonattr.h"
#include "modules/anonymize/anoncontext.h"
#include "modules/anonymize/xmlanonutils.h"
#include "xmlutils.h"

void Element::scanAnonymize(AnonContext *context)
{
    switch(type) {
    case ET_ELEMENT:
        scanAnonymizeElement(context);
        break;
    case ET_PROCESSING_INSTRUCTION:
    case ET_COMMENT:
        break ;
    case ET_TEXT:
        XmlAnonUtils::scanTextOfElement(context, text);
        break;
    default:
        break;
    }
}

void Element::anonymize(AnonContext *context)
{
    scanAnonymize(context);
    anonymizeInternal(context);
}

void Element::anonymizeInternal(AnonContext *context)
{
    switch(type) {
    case ET_ELEMENT:
        anonymizeElement(context);
        break;
    case ET_PROCESSING_INSTRUCTION:
    case ET_COMMENT:
        break ;
    case ET_TEXT:
        anonymizeText(context);
        break;
    default:
        break;
    }
}

void Element::anonymizeElement(AnonContext *context)
{
    AnonContext thisContext(context, tag());
    handleNamespace(&thisContext);
    thisContext.setExceptionForElement();
    if(context->isCollectingData()) {
        AnonException *exception = thisContext.getException();
        if(thisContext.canAnonymize(exception)) {
            context->setOrigData(this, _tag);
        }
    }

    foreach(Attribute * attribute, attributes) {
        attribute->anonymize(&thisContext);
    }
    QVectorIterator<TextChunk*> tt(textNodes);
    while(tt.hasNext()) {
        TextChunk *tx = tt.next();
        tx->text = anonymizeTextOfElement(&thisContext, tx->text, tx);
    }
    foreach(Element * child, childItems) {
        if(child->isText()) {
            child->anonymizeText(&thisContext);
        }
    }
    thisContext.restoreContext();

    // apply settings for recursion
    foreach(Element * child, childItems) {
        if(!child->isText()) {
            child->anonymizeInternal(&thisContext);
        }
    }
}

void Element::scanAnonymizeElement(AnonContext *context)
{
    AnonContext thisContext(context, tag());
    handleNamespace(&thisContext);
    thisContext.setExceptionForElement();

    foreach(Attribute * attribute, attributes) {
        attribute->scanAnonymize(&thisContext);
    }
    QVectorIterator<TextChunk*> tt(textNodes);
    while(tt.hasNext()) {
        TextChunk *tx = tt.next();
        XmlAnonUtils::scanTextOfElement(&thisContext, tx->text);
    }
    foreach(Element * child, childItems) {
        if(child->isText()) {
            XmlAnonUtils::scanTextOfElement(&thisContext, child->text);
        }
    }
    thisContext.restoreContext();

    // apply settings for recursion
    foreach(Element * child, childItems) {
        if(!child->isText()) {
            child->scanAnonymize(&thisContext);
        }
    }
    thisContext.restoreContext();
}

void Element::anonymizeText(AnonContext * context)
{
    text = anonymizeTextOfElement(context, text, this);
}

QString Element::anonymizeTextOfElement(AnonContext *context, const QString &inputText, void * key)
{
    return XmlAnonUtils::anonymizeTextOfElement(context, inputText, key);
}

void Element::handleNamespace(AnonContext *context)
{
    foreach(Attribute * attribute, attributes) {
        context->addNamespaceAttribute(attribute->name, attribute->value);
    }
    context->setContextElement(_tag);
}

void Attribute::anonymize(AnonContext *context)
{
    if(isDataAttribute()) {
        AnonContextAttribute thisContext(context, name);
        thisContext.pushContextNamespaceAttribute(name);
        AnonException *exception = thisContext.getException();
        if(thisContext.canAnonymize(exception)) {
            if(context->isCollectingData()) {
                context->setOrigData(this, value);
            }
            QString newData = thisContext.anonymize(exception, value) ;
            value = newData ;
        }
    }
}

void Attribute::scanAnonymize(AnonContext *context)
{
    if(isDataAttribute()) {
        AnonContextAttribute thisContext(context, name);
        thisContext.pushContextNamespaceAttribute(name);
        AnonException *exception = thisContext.getException();
        if(thisContext.canAnonymize(exception)) {
            thisContext.scanAnonymize(exception, value);
        }
    }
}

bool Attribute::isDataAttribute()
{
    return XmlUtils::isDataAttribute(name);
}
