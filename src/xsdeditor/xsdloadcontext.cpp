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

#include "xsdloadcontext.h"
#include "utils.h"

//---------------------------------------------------

XSDOpError::XSDOpError()
{
}

XSDOpError::~XSDOpError()
{
}

EXSDLoadError XSDOpError::errorCode() const
{
    return _errorCode;
}

void XSDOpError::setErrorCode(const EXSDLoadError &errorCode)
{
    _errorCode = errorCode;
}

QString XSDOpError::msg() const
{
    return _msg;
}

void XSDOpError::setMsg(const QString &msg)
{
    _msg = msg;
}


//---------------------------------------------------

XSDLoadContext::XSDLoadContext()
{
    reset();
}

XSDLoadContext::~XSDLoadContext()
{
    reset();
}


void XSDLoadContext::cloneSettingsFrom(XSDLoadContext *theTemplate)
{
    if(NULL != theTemplate) {
        reset();
        setErrorPolicy(theTemplate->errorPolicy());
        setLoadKeys(theTemplate->loadKeys());
    }
}

void XSDLoadContext::reset()
{
    foreach(XSDOpError * err, _messages) {
        delete err;
    }
    _messages.clear();
    _lastError = XSD_LOAD_NOERROR;
    _errorPolicy = XSD_LOADPOLICY_ERRORSTOP ;
    _loadKeys = false ;
}

bool XSDLoadContext::isPolicyThrowError()
{
    return _errorPolicy == XSD_LOADPOLICY_ERRORSTOP;
}


bool XSDLoadContext::hasErrors()
{
    return (XSD_LOAD_NOERROR != _lastError) || !_messages.isEmpty() ;
}

int XSDLoadContext::errorsCount()
{
    return _messages.size();
}

XSDOpError* XSDLoadContext::errorAt(const int index)
{
    if((index >= 0) && (index < _messages.size())) {
        return _messages.at(index);
    }
    return NULL ;
}

EXSDLoadErrorPolicy XSDLoadContext::errorPolicy() const
{
    return _errorPolicy;
}

void XSDLoadContext::setErrorPolicy(const EXSDLoadErrorPolicy &errorPolicy)
{
    _errorPolicy = errorPolicy;
}


void XSDLoadContext::addError(const EXSDLoadError errorCode, const QString &msg)
{
    if(XSD_LOAD_NOERROR != errorCode) {
        _lastError = errorCode;
    }
    XSDOpError *newError = new XSDOpError();
    newError->setErrorCode(errorCode);
    newError->setMsg(msg);
    _messages.append(newError);
}

bool XSDLoadContext::containsCode(const EXSDLoadError errorCode)
{
    if(errorCode == _lastError) {
        return true;
    }
    foreach(XSDOpError * err, _messages) {
        if(err->errorCode() == errorCode) {
            return true ;
        }
    }
    return false;
}


QHash<QString, XSchemaObject*> *XSDLoadContext::inputKeyMap()
{
    return &_keyMap ;
}

void XSDLoadContext::setKey(const QString &key, XSchemaObject *ptr)
{
    _keyMap.insert(key, ptr);
}

XSchemaObject *XSDLoadContext::findObjectForKey(const QString &key)
{
    if(_keyMap.contains(key)) {
        return _keyMap[key];
    }
    return NULL ;
}


bool XSDLoadContext::loadKeys() const
{
    return _loadKeys;
}

void XSDLoadContext::setLoadKeys(bool loadKeys)
{
    _loadKeys = loadKeys;
}

//---------------------------------------------------
