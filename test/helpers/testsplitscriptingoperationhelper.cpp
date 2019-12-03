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

#include "testsplitscriptingoperationhelper.h"
#include "testsplitscriptinghelper.h"
#include "utils.h"

static const QString TextEventHandler("textHandler");
static const QString ElementEventHandler("elementHandler");

TestSplitScriptingOperationHelper::TestSplitScriptingOperationHelper() : op(&results)
{
}

TestSplitScriptingOperationHelper::~TestSplitScriptingOperationHelper()
{
}

void TestSplitScriptingOperationHelper::executeMultipleText(const QString &eventCode1, const QString &eventCode2, const int level, const QString &path, const QString &text)
{
    const QString finalCode1 = QString("function %1(context, event) { %2 } ").arg(TextEventHandler).arg(eventCode1);
    const QString finalCode2 = QString("function %1 (context, event) { %2 } ").arg(TextEventHandler).arg(eventCode2);
    op.scriptManager()->addScriptingFilter(TestSplitScriptingHelper::filterForModel(TestSplitScriptingHelper::baseEventModel(ExtractionEventText, true, TextEventHandler, finalCode1)));
    op.scriptManager()->addScriptingFilter(TestSplitScriptingHelper::filterForModel(TestSplitScriptingHelper::baseEventModel(ExtractionEventText, true, TextEventHandler, finalCode2)));
    op.scriptManager()->initScripting();
    opResult = op.internalManageText(textEvent, level, path, false, false, text);
}

void TestSplitScriptingOperationHelper::executeMultipleElement(const QString &eventCode1,
                                                          const QString &eventCode2,
                                                          const QString &elementName,
                                                          const QString &nameSpace,
                                                          const QString &localName,
                                                          QList<QPair<QString,QString> > &attributes)
{
    const QString finalCode1 = QString("function %1(context, event) { %2 } ").arg(ElementEventHandler).arg(eventCode1);
    const QString finalCode2 = QString("function %1(context, event) { %2 } ").arg(ElementEventHandler).arg(eventCode2);
    op.scriptManager()->addScriptingFilter(TestSplitScriptingHelper::filterForModel(TestSplitScriptingHelper::baseEventModel(ExtractionEventElement, true, ElementEventHandler, finalCode1)));
    op.scriptManager()->addScriptingFilter(TestSplitScriptingHelper::filterForModel(TestSplitScriptingHelper::baseEventModel(ExtractionEventElement, true, ElementEventHandler, finalCode2)));
    op.scriptManager()->initScripting();
    QXmlStreamAttributes xmlAttributes = makeAttributes(attributes);
    opResult = op.internalManageElement(elementEvent, 1, "/root/a", elementName, nameSpace, localName, xmlAttributes );
}

QXmlStreamAttributes TestSplitScriptingOperationHelper::makeAttributes(QList<QPair<QString, QString> > &inputAttributes)
{
    QXmlStreamAttributes attributes;
    QPair<QString, QString> attribute;
    foreach(attribute, inputAttributes ) {
        attributes.append(QXmlStreamAttribute("", attribute.first, attribute.second));
    }
    return attributes ;
}
