/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2017 by Luca Bellonda and individual contributors  *
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

#include "operationresult.h"

OperationResult::OperationResult()
{
    _isError = false ;
}

OperationResult::~OperationResult()
{
}

bool OperationResult::isError()
{
    return _isError ;
}

void OperationResult::setError(const bool value)
{
    _isError = value ;
}

void OperationResult::setErrorWithText(const QString& value)
{
    setError(true);
    _message = value ;
}

QString OperationResult::message()
{
    return _message ;
}


void OperationResult::setMessage(const QString& value)
{
    _message = value ;
}

bool OperationResult::isOk()
{
    return !_isError;
}

void OperationResult::setOk()
{
    _isError = false;
}

//------------------------------------------------------------------------------------

StringOperationResult::StringOperationResult() : OperationResult()
{
    //;
}

StringOperationResult::~StringOperationResult()
{
    //;
}

QString StringOperationResult::result()
{
    return _result ;
}


void StringOperationResult::setResult(const QString& value)
{
    _result = value ;
}

//------------------------------------------------------------------------------------

MessagesOperationResult::MessagesOperationResult()
{
}

MessagesOperationResult::~MessagesOperationResult()
{
    EMPTYPTRLIST(_messages, SourceMessage);
}

void MessagesOperationResult::addMessage(SourceMessage* newMessage)
{
    _messages.append(newMessage);
}

QList<SourceMessage*> *MessagesOperationResult::messages()
{
    return &_messages ;
}
