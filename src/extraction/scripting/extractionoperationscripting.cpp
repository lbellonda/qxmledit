/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2019-2020 by Luca Bellonda and individual contributors  *
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

#include "xmlEdit.h"
#include "extraction/extractionoperation.h"
#include "utils.h"
#include <QXmlStreamReader>
#include <QTextCodec>
#include "qxmleditconfig.h"
#include "extraction/extractionscriptingprovider.h"

// ---startRegion(scripting)
bool ExtractionOperation::isScriptingEnabled()
{
    return _scriptManager.isScriptingEnabled();
}

bool ExtractionOperation::initScripting()
{
    return _scriptManager.initScripting();
}

void ExtractionOperation::addScriptingFilter(ExtractionScriptFilterModel *newFilter)
{
    _scriptManager.addScriptingFilter(newFilter);
}

ExtractionScriptManager *ExtractionOperation::scriptManager()
{
    return &_scriptManager;
}

ExtractionScriptManager::EEventResult
ExtractionOperation::internalManageText(
    ExtractionScriptTextEvent &textEvent, const int level, const QString &path,
    const bool isWhitespace, const bool isCDATA, const QString &text)
{
    prepareEventText(textEvent, isWhitespace, isCDATA, text);
    const ExtractionScriptManager::EEventResult result = _scriptManager.textEvent(level, path, &textEvent);
    return result;
}

bool ExtractionOperation::manageText(ExtractInfo &info, const int level, const QString &path, QXmlStreamReader &xmlReader, bool &dontWrite)
{
    ExtractionScriptTextEvent textEvent;
    const ExtractionScriptManager::EEventResult result = internalManageText(textEvent, level, path, xmlReader.isWhitespace(), xmlReader.isCDATA(), xmlReader.text().toString());
    if(_scriptManager.isError() || (result == ExtractionScriptManager::EventResult_Error)) {
        setError(EXML_Scripting, _scriptManager.errorMessage());
        return false;
    }
    switch(result) {
    case ExtractionScriptManager::EventResult_IgnoreEvent:
        dontWrite = true;
        break;
    case ExtractionScriptManager::EventResult_WriteOriginalData:
        dontWrite = false;
        break;
    case ExtractionScriptManager::EventResult_WriteModifiedData:
        dontWrite = true;
        if(!writeText(info, textEvent.isCDATA(), textEvent.text())) {
            return false ;
        }
        break;
    default:
        // this should not happen
        return false;
    case ExtractionScriptManager::EventResult_Error:
        return false ;
    }
    return true ;
}

void ExtractionOperation::prepareEventText(ExtractionScriptTextEvent &textEvent, const bool isWhitespace, const bool isCDATA, const QString &text)
{
    textEvent.setCDATA(isCDATA);
    textEvent.setText(text);
    textEvent.setWhiteSpaceFlag(isWhitespace);
    textEvent.resetModifed();
}

//----------------------------------------------------------------------------
ExtractionScriptManager::EEventResult ExtractionOperation::internalManageElement(ExtractionScriptElementEvent &elementEvent, const int level, const QString &path, const QString &name, const QString &nameSpace, const QString &localName, QXmlStreamAttributes attributes)
{
    prepareEventElement(elementEvent, name, nameSpace, localName, attributes);
    const ExtractionScriptManager::EEventResult result = _scriptManager.elementEvent(level, path, &elementEvent);
    return result;
}

bool ExtractionOperation::manageElement(ExtractInfo &info, const int level, const QString &path, QXmlStreamReader &xmlReader, bool &dontWrite)
{
    ExtractionScriptElementEvent elementEvent;

    const ExtractionScriptManager::EEventResult result = internalManageElement(elementEvent, level, path, xmlReader.qualifiedName().toString(), xmlReader.namespaceUri().toString(), xmlReader.name().toString(), xmlReader.attributes());
    if(_scriptManager.isError() || (result == ExtractionScriptManager::EventResult_Error)) {
        setError(EXML_Scripting, _scriptManager.errorMessage());
        return false;
    }
    switch(result) {
    case ExtractionScriptManager::EventResult_IgnoreEvent:
        dontWrite = true;
        break;
    case ExtractionScriptManager::EventResult_WriteOriginalData:
        dontWrite = false;
        break;
    case ExtractionScriptManager::EventResult_WriteModifiedData:
        dontWrite = true;
        {
            if(!writeElement(info, elementEvent.nameSpace(), elementEvent.localName(), elementEvent.elementName(), elementEvent.attributes())) {
                return false ;
            }
        }
        break;
    default:
        // this should not happen
        return false;
    case ExtractionScriptManager::EventResult_Error:
        return false ;
    }
    return true ;
}

void ExtractionOperation::prepareEventElement(ExtractionScriptElementEvent &elementEvent, const QString &name, const QString &nameSpace, const QString &localName, QXmlStreamAttributes attributes)
{
    elementEvent.setElementName(name);
    elementEvent.setNameSpace(nameSpace);
    elementEvent.setLocalName(localName);
    foreach(QXmlStreamAttribute attribute, attributes) {
        elementEvent.setAttribute(&attribute);
    }
    elementEvent.resetModifed();
}

void ExtractionOperation::prepareScripting()
{
    QStringList filtersIdList = filterListAsIdList();
    if(!filtersIdList.isEmpty()) {
        ExtractionScriptingProvider provider;
        foreach(const QString &id, filtersIdList) {
            ExtractionScriptFilterModel* filter = provider.getPredefinedScriptFromScriptId(id);
            if(NULL != filter) {
                _scriptManager.addScriptingFilter(filter);
            }
        }
    }
}
// ----endRegion(scripting)









