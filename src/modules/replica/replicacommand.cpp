/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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


#include "replicacommand.h"
#include "utils.h"

ReplicaCommand::ReplicaCommand()
{
    _recursive = false;
    _overwrite = true ;
    _startNumber = 1;
    _atEnd = false ;
    _format = NumberUnpadded ;
    _replace = false ;
}

ReplicaCommand::~ReplicaCommand()
{
}

#define COPY(other, field) other->field = field

ReplicaCommand *ReplicaCommand::clone()
{
    ReplicaCommand *other = new ReplicaCommand();
    COPY(other, _recursive);
    COPY(other, _attributeName);
    COPY(other, _overwrite);
    COPY(other, _startNumber);
    COPY(other, _atEnd);
    COPY(other, _separator);
    COPY(other, _format);
    COPY(other, _replace);
    return other;
}

#define COMPARE(other, field) do { if( other->field != field ) { return false ; } } while(false);

bool ReplicaCommand::compare(ReplicaCommand* other)
{
    COMPARE(other, _recursive);
    COMPARE(other, _attributeName);
    COMPARE(other, _overwrite);
    COMPARE(other, _startNumber);
    COMPARE(other, _atEnd);
    COMPARE(other, _separator);
    COMPARE(other, _format);
    COMPARE(other, _replace);
    return true;
}



bool ReplicaCommand::recursive() const
{
    return _recursive;
}

void ReplicaCommand::setRecursive(bool recursive)
{
    _recursive = recursive;
}

QString ReplicaCommand::attributeName() const
{
    return _attributeName;
}

void ReplicaCommand::setAttributeName(const QString &attributeName)
{
    _attributeName = attributeName;
}

bool ReplicaCommand::overwrite() const
{
    return _overwrite;
}

void ReplicaCommand::setOverwrite(bool overwrite)
{
    _overwrite = overwrite;
}

int ReplicaCommand::startNumber() const
{
    return _startNumber;
}

void ReplicaCommand::setStartNumber(int startNumber)
{
    _startNumber = startNumber;
}

bool ReplicaCommand::atEnd() const
{
    return _atEnd;
}

void ReplicaCommand::setAtEnd(bool atEnd)
{
    _atEnd = atEnd;
}

QString ReplicaCommand::separator() const
{
    return _separator;
}

void ReplicaCommand::setSeparator(const QString &value)
{
    _separator = value;
}

ReplicaCommand::EFormat ReplicaCommand::format() const
{
    return _format;
}

void ReplicaCommand::setFormat(const EFormat &format)
{
    _format = format;
}

ReplicaCommand::EErrorCode ReplicaCommand::check()
{
    if(!Utils::checkXMLName(_attributeName)) {
        return AttributeNameError ;
    }
    if(_startNumber < 0) {
        return StartNumberError ;
    }
    if(!isLegalFormat()) {
        return IllegalFormatError ;
    }
    return NoError ;
}

QString ReplicaCommand::errorMessage(const EErrorCode err)
{
    switch(err) {
    default:
        return QObject::tr("Unknown error");
    case NoError:
        return "";
    case AttributeNameError:
        return QObject::tr("Invalid attribute name:%1").arg(_attributeName);
    case StartNumberError:
        return QObject::tr("Invalid start value:%1").arg(_startNumber);
    case IllegalFormatError:
        return QObject::tr("Invalid format value:%1").arg(_format);
    }
}

bool ReplicaCommand::isLegalFormat()
{
    switch(format()) {
    case ReplicaCommand::NumberPadded:
    case ReplicaCommand::NumberUnpadded:
    case ReplicaCommand::AlphaPadded:
    case ReplicaCommand::AlphaUnpadded:
        return true ;
    default:
        return false;
    }
}

bool ReplicaCommand::replace() const
{
    return _replace;
}

void ReplicaCommand::setReplace(bool replace)
{
    _replace = replace;
}
