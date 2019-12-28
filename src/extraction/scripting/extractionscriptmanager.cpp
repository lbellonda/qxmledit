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

#include "extractionscriptmanager.h"
#include "utils.h"

ExtractionScriptManager::ExtractionScriptManager()
{
    _isReady = false;
    _isEnabled = false ;
    _isInitedOk = false ;
    _eventsCount = 0 ;
    _textEventsCount = 0 ;
    _elementEventsCount = 0;
    _context = new ExtractionOperationScriptContext();
}

ExtractionScriptManager::~ExtractionScriptManager()
{
    EMPTYPTRLIST(_filters, ExtractionScriptFilter)
    delete _context;
}

bool ExtractionScriptManager::isScriptingEnabled()
{
    if(!_isReady) {
        initScripting();
    }
    return _isEnabled;
}

int ExtractionScriptManager::eventsCount()
{
    return _eventsCount ;
}

int ExtractionScriptManager::textEventsCount()
{
    return _textEventsCount;
}

int ExtractionScriptManager::elementEventsCount()
{
    return _elementEventsCount;
}

QString ExtractionScriptManager::errorMessage()
{
    return _error.toString();
}

QString ExtractionScriptManager::errorMessageExtended()
{
    return _error.toStringExtended();
}

bool ExtractionScriptManager::initScripting()
{
    _isEnabled = false ;
    if(!_isReady) {
        _container.reset();
        _eventsCount = 0;
        _textEventsCount = 0;
        _elementEventsCount = 0 ;
        _isInitedOk = _container.init();
        foreach(ExtractionScriptFilter* filter, _filters) {
            _isEnabled = true ;
            if(filter->isEnabled()) {
                filter->registerContext(_context);
                if(filter->isTextEventEnabled()) {
                    _container.addTextEventHandler(filter);
                }
                if(filter->isElementEventEnabled()) {
                    _container.addElementEventHandler(filter);
                }
                if(!filter->initScripting(_error)) {
                    _isInitedOk = false ;
                    break;
                }
            }
        }
        _isReady = true ;
    }
    return _isInitedOk ;
}

bool ExtractionScriptManager::isError()
{
    return _error.isError;
}

void ExtractionScriptManager::addScriptingFilter(ExtractionScriptFilterModel *newFilter)
{
    ExtractionScriptFilter* filter = new ExtractionScriptFilter();
    filter->setModel(newFilter);
    _filters.append(filter);
    _isReady = false ;
}

//------- begin_region(events) ---

void ExtractionScriptManager::initTextEvent(ExtractionScriptTextEvent *textEvent)
{
    textEvent->resetModifed();
}


// First ignore -> ignore
// First error -> error
ExtractionScriptManager::EEventResult ExtractionScriptManager::textEvent(const int level, const QString &path, ExtractionScriptTextEvent *textEvent)
{
    if(!initScripting()) {
        _error.isError = true ;
        _error.errorMessage = Utils::tr("Not inizialized");
        return EventResult_Error;
    }
    initTextEvent(textEvent);
    bool isModified = false ;
    _error.reset();
    _context->setPath(path);
    _context->setLevel(level);
    foreach(ExtractionScriptFilter *filter, _container.allTextEvents()) {
        filter->evaluateTextEvent(_error, textEvent);
        _eventsCount++ ;
        _textEventsCount ++ ;
        if(_error.isError) {
            return EventResult_Error;
        }

        if(textEvent->isModified()) {
            isModified = true ;
            if(textEvent->isIgnored()) {
                return EventResult_IgnoreEvent;
            }
        }

        if(textEvent->isError()) {
            _error.isError = true ;
            _error.errorMessage = textEvent->errorMessage();
            _error.errorString = textEvent->errorMessage();
            return EventResult_Error;
        }
    }
    // write back the result to the element
    return isModified ? EventResult_WriteModifiedData : EventResult_WriteOriginalData ;
}

ExtractionScriptManager::EEventResult ExtractionScriptManager::elementEvent(const int level, const QString &path, ExtractionScriptElementEvent *elementEvent)
{
    if(!initScripting()) {
        _error.isError = true ;
        _error.errorMessage = Utils::tr("Not inizialized");
        return EventResult_Error;
    }

    bool isModified = false ;
    _error.reset();
    _context->setPath(path);
    _context->setLevel(level);
    foreach(ExtractionScriptFilter *filter, _container.allElementEvents()) {
        filter->evaluateElementEvent(_error, elementEvent);
        _eventsCount++ ;
        _elementEventsCount ++ ;
        if(_error.isError) {
            return EventResult_Error;
        }

        if(elementEvent->isModified()) {
            isModified = true ;
        }

        if(elementEvent->isError()) {
            _error.isError = true ;
            _error.errorMessage = elementEvent->errorMessage();
            _error.errorString = elementEvent->errorMessage();
            return EventResult_Error;
        }
    }
    // write back the result to the element
    return isModified ? EventResult_WriteModifiedData : EventResult_WriteOriginalData ;
}

//------- end_region(events) ---

