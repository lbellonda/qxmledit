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

#include "testextractionexecutorhelper.h"

TestExtractionExecutorHelper::TestExtractionExecutorHelper()
{
    textEvent = NULL ;
    elementEvent = NULL ;

    reference = new ExtractionOperationScriptContext();
    compare = new TestWritableExtractionOperationScriptContext();
}

TestExtractionExecutorHelper::~TestExtractionExecutorHelper()
{
    delete compare;
    delete reference;
}

bool TestExtractionExecutorHelper::execute(const QString &code)
{
    filter.registerContext(reference);
    filter.registerObject(ExtractionScriptFilter::TestContextDouble, compare);
    return filter.executeScript(_error, code);
}

bool TestExtractionExecutorHelper::executeText(const QString &code, bool isCDATA, const QString &inputText)
{
    textEvent = new ExtractionScriptTextEvent();
    textEvent->setText(inputText);
    textEvent->setCDATA(isCDATA);
    textEvent->resetModifed();
    filter.registerEvent(textEvent);
    return filter.executeScript(_error, code);
}

bool TestExtractionExecutorHelper::executeElement(const QString &code,
                                                  const QString &elementName,
                                                  const QString &nameSpace,
                                                  const QString &localName,
                                                  QList<QPair<QString,QString> > attributes)
{
    elementEvent = new ExtractionScriptElementEvent();
    elementEvent->setElementName(elementName);
    elementEvent->setNameSpace(nameSpace);
    elementEvent->setLocalName(localName);
    QPair<QString,QString> attribute;
    foreach( attribute, attributes ) {
        elementEvent->setAttributeValueByName(attribute.first, attribute.second);
    }
    elementEvent->resetModifed();
    filter.registerEvent(elementEvent);
    return filter.executeScript(_error, code);
}

