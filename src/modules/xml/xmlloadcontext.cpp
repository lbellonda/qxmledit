/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2018 by Luca Bellonda and individual contributors  *
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


#include "xmlloadcontext.h"

qint64 XMLLoadContext::characterOffset() const
{
    return _characterOffset;
}

XMLLoadContext::XMLLoadContext()
{
    _isError = false;
    _firstElementSeen = false;
    _isAfterDTD = false;
    _line = -1 ;
    _column = -1 ;
    _characterOffset = -1 ;
}

XMLLoadContext::~XMLLoadContext()
{
}

//----

bool XMLLoadContext::isError() const
{
    return _isError;
}

bool XMLLoadContext::isOk() const
{
    return !isError();
}

void XMLLoadContext::setIsError(bool isError)
{
    _isError = isError;
}

QString XMLLoadContext::errorMessage() const
{
    return _errorMessage;
}

void XMLLoadContext::setErrorMessage(const QString &errorMessage)
{
    _errorMessage = errorMessage;
}

bool XMLLoadContext::setErrorWithMessage(const QString &theErrorMessage)
{
    setIsError(true);
    setErrorMessage(theErrorMessage);
    return false;
}

void XMLLoadContext::copyErrorData(QXmlStreamReader *xmlReader)
{
    setErrorPosition(xmlReader->lineNumber(), xmlReader->columnNumber(), xmlReader->characterOffset());
}

bool XMLLoadContext::setError(const QString &errorMessage, QXmlStreamReader *xmlReader)
{
    setIsError(true);
    QString errorMsg = QObject::tr("Error parsing XML: '%1' at row:%2 column:%3 offset:%4")
                       .arg(errorMessage)
                       .arg(xmlReader->lineNumber())
                       .arg(xmlReader->columnNumber())
                       .arg(xmlReader->characterOffset());
    setErrorMessage(errorMsg);
    copyErrorData(xmlReader);
    return false;
}

bool XMLLoadContext::setErrorFromReader(QXmlStreamReader *xmlReader)
{
    if(xmlReader->hasError()) {
        QString errorMsg = QObject::tr("Error parsing XML: '%1' code:'%2' at row:%3 column:%4 offset:%5")
                           .arg(xmlReader->errorString())
                           .arg(xmlReader->error())
                           .arg(xmlReader->lineNumber())
                           .arg(xmlReader->columnNumber())
                           .arg(xmlReader->characterOffset());
        setIsError(true);
        setErrorMessage(errorMsg);
        copyErrorData(xmlReader);
    } else {
        setError(QObject::tr("Unexpected error"), xmlReader);
    }
    return false;
}

void XMLLoadContext::setErrorPosition(const qint64 line, const qint64 column, const qint64 offset)
{
    _line = line;
    _column = column;
    _characterOffset = offset;
}

QString XMLLoadContext::encoding() const
{
    return _encoding;
}

void XMLLoadContext::setEncoding(const QString &value)
{
    _encoding = value;
}

bool XMLLoadContext::firstElementSeen() const
{
    return _firstElementSeen;
}

void XMLLoadContext::setFirstElementSeen(bool value)
{
    _firstElementSeen = value;
}


bool XMLLoadContext::isAfterDTD() const
{
    return _isAfterDTD;
}

void XMLLoadContext::setIsAfterDTD(bool value)
{
    _isAfterDTD = value;
}

QList<Element *> &XMLLoadContext::firstComments()
{
    return _firstComments;
}

void XMLLoadContext::addFirstComment(Element *comment)
{
    _firstComments.append(comment);
}
