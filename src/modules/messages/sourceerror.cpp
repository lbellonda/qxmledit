/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016 by Luca Bellonda and individual contributors       *
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

#include "sourceerror.h"
#include "utils.h"

qint64 SourceMessage::line() const
{
    return _line;
}

void SourceMessage::setLine(const qint64 &line)
{
    _line = line;
}

qint64 SourceMessage::column() const
{
    return _column;
}

void SourceMessage::setColumn(const qint64 &column)
{
    _column = column;
}

QString SourceMessage::description() const
{
    return _description;
}

void SourceMessage::setDescription(const QString &description)
{
    _description = description;
}

QList<int> SourceMessage::path() const
{
    return _path;
}

void SourceMessage::setPath(const QList<int> &path)
{
    _path = path;
}

bool SourceMessage::isFound() const
{
    return _found;
}

void SourceMessage::setFound(bool found)
{
    _found = found;
}

qint64 SourceMessage::foundLine() const
{
    return _foundLine;
}

void SourceMessage::setFoundLine(const qint64 &foundLine)
{
    _foundLine = foundLine;
}

qint64 SourceMessage::foundColumn() const
{
    return _foundColumn;
}

void SourceMessage::setFoundColumn(const qint64 &foundColumn)
{
    _foundColumn = foundColumn;
}

SourceMessage::EType SourceMessage::type() const
{
    return _type;
}

void SourceMessage::setType(const EType &type)
{
    _type = type;
}

QString SourceMessage::source() const
{
    return _source;
}

void SourceMessage::setSource(const QString &source)
{
    _source = source;
}

int SourceMessage::code() const
{
    return _code;
}

void SourceMessage::setCode(int value)
{
    _code = value;
}

SourceMessage::SourceMessage()
{
    _line = 0 ;
    _column = 0 ;
    _found = false ;
    _foundLine = -1 ;
    _foundColumn = -1 ;
    _type = Info;
    _code = 0 ;
}

SourceMessage::~SourceMessage()
{
}

