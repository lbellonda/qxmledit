/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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


#include "basecomplexvariable.h"
#include "utils.h"
#include "metadataparser.h"


PseudoAttribute::PseudoAttribute()
{
    _exists = false ;
    _used = false ;
    _row = 0 ;
    _position = 0;
    _isSingleOrDoubleQuote = false ;
}

PseudoAttribute::~PseudoAttribute()
{
}

void PseudoAttribute::copyTo(PseudoAttribute *target)
{
    target->_exists = _exists;
    target->_used = _used ;
    target->_name = _name ;
    target->_row = _row;
    target->_position = _position ;
    target->_isSingleOrDoubleQuote = _isSingleOrDoubleQuote;
    target->_value = _value;
}

void PseudoAttribute::reset(const QString &newValue)
{
    _value = newValue;
    _used = !newValue.isEmpty() ;
    _row = 0;
    _position = 0 ;
}

void PseudoAttribute::reset()
{
    reset("");
}

void PseudoAttribute::read(PseudoAttribute*src)
{
    if(NULL == src) {
        return ;
    }
    src->copyTo(this);
    setExists(true);
}

/**
 * @brief PseudoAttribute::toString
 * write the pattern name="$data"
 * @return
 */
QString PseudoAttribute::toString()
{
    if(!_used) {
        return "";
    }
    QString result = " ";
    result += _name ;
    result += "=\"" ;
    result += MetadataParserInfo::escape(_value) ;
    result += "\"" ;
    return result;
}

/**
 * @brief PseudoAttribute::toStringComplete
 *  writes the pattern type="$name" value="$data"
 * @return
 */
QString PseudoAttribute::toStringComplete()
{
    if(!_used) {
        return "";
    }
    QString result = " type=\"";
    result += _name ;
    result += "\" value=\"" ;
    result += MetadataParserInfo::escape(_value) ;
    result += "\"" ;
    return result;
}

/**
 * @brief PseudoAttribute::toStringStd
 * write the pattern value="$data"
 * @return
 */
QString PseudoAttribute::toStringStd()
{
    if(!_used) {
        return "";
    }
    QString result = " ";
    result += "value=\"" ;
    result += MetadataParserInfo::escape(_value) ;
    result += "\"" ;
    return result;
}

bool PseudoAttribute::exists() const
{
    return _exists;
}

void PseudoAttribute::setExists(bool exists)
{
    _exists = exists;
}

bool PseudoAttribute::used() const
{
    return _used;
}

void PseudoAttribute::setUsed(bool used)
{
    _used = used;
}

QString PseudoAttribute::name() const
{
    return _name;
}

void PseudoAttribute::setName(const QString &name)
{
    _name = name;
}

QString PseudoAttribute::value() const
{
    return _value;
}

void PseudoAttribute::setValue(const QString &value)
{
    _value = value;
    _used = true ;
}

int PseudoAttribute::position() const
{
    return _position;
}

void PseudoAttribute::setPosition(int position)
{
    _position = position;
}


int PseudoAttribute::row() const
{
    return _row;
}

void PseudoAttribute::setRow(int row)
{
    _row = row;
}
