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

#include "extractionscripttext.h"
#include "utils.h"

ExtractionScriptTextEvent::ExtractionScriptTextEvent(QObject *parent) : QObject(parent)
{
    _isModified = false ;
    _isCDATA = false;
    _isIgnored = false ;
    _isWhitespace = false ;
    _isError = false;
}

ExtractionScriptTextEvent::~ExtractionScriptTextEvent()
{
}

QString ExtractionScriptTextEvent::text() const
{
    return _text;
}

void ExtractionScriptTextEvent::setText(const QString &value)
{
    SET_VAR_if(_text, value);
}

bool ExtractionScriptTextEvent::isCDATA() const
{
    return _isCDATA;
}

void ExtractionScriptTextEvent::setCDATA(bool value)
{
    SET_VAR_if(_isCDATA, value);
}

bool ExtractionScriptTextEvent::isModified() const
{
    return _isModified;
}

bool ExtractionScriptTextEvent::resetModifed()
{
    bool result = _isModified ;
    _isModified = false;
    return result ;
}

bool ExtractionScriptTextEvent::compareTo(ExtractionScriptTextEvent &other)
{
    if(_isModified != other._isModified) {
        return false;
    }
    if(_isIgnored != other._isIgnored) {
        return false;
    }
    if(_isCDATA != other._isCDATA) {
        return false;
    }
    if(_text != other._text) {
        return false;
    }
    if(_isError != other._isError) {
        return false;
    }
    if(_errorMessage != other._errorMessage) {
        return false;
    }
    return true ;
}

bool ExtractionScriptTextEvent::isError() const
{
    return _isError;
}

void ExtractionScriptTextEvent::setError(bool value)
{
    SET_VAR_if(_isError, value);
}

QString ExtractionScriptTextEvent::errorMessage() const
{
    return _errorMessage;
}

void ExtractionScriptTextEvent::setErrorMessage(const QString &value)
{
    SET_VAR_if(_errorMessage, value)
}

bool ExtractionScriptTextEvent::isIgnored() const
{
    return _isIgnored;
}

void ExtractionScriptTextEvent::setIgnored(bool value)
{
    SET_VAR_if(_isIgnored, value)
}

void ExtractionScriptTextEvent::setWhiteSpaceFlag(bool how)
{
    _isWhitespace = how ;
}

bool ExtractionScriptTextEvent::isWhitespace() const
{
    return _isWhitespace;
}
