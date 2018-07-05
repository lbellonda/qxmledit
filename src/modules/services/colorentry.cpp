/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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


#include "colorentry.h"
#include "qxmleditconfig.h"
#include "utils.h"

ColorEntry::ColorEntry(const QString &newKey, const QColor newDefaultValue) : _color(0, 0, 0)
{
    _key = newKey ;
    _isSet = false;
    _defaultValue  = newDefaultValue;
}

ColorEntry::~ColorEntry()
{
}

//---------------------------------------------------------------------

void ColorEntry::reset()
{
    _isSet = false;
    _color = _defaultValue ;
}

void ColorEntry::readCfg()
{
    QString colorString = Config::getString(_key, "");
    if(colorString.isEmpty()) {
        reset();
    } else {
        _isSet = true ;
        _color = parseColorFromString(colorString);
    }
}

void ColorEntry::saveCfg()
{
    if(isSet()) {
        QString colorData = Utils::fmtNumber(true, (_color.red() << 16) + (_color.green() << 8) + _color.blue(), 6, false);
        Config::saveString(key(), colorData);
    } else {
        Config::saveString(key(), "");
    }
}

QColor ColorEntry::parseColorFromString(const QString &newColor)
{
    bool ok ;
    QColor color ;
    uint val = newColor.toUInt(&ok, 16) ;
    if(ok) {
        if(newColor.length() <= 6) {
            color = QColor((val >> 16) & 0x00FF, (val >> 8) & 0x00FF, val & 0x00FF);
        } else {
            color = QColor((val >> 16) & 0x00FF, (val >> 8) & 0x00FF, val & 0x00FF, (val >> 24) & 0x00FF);
        }
    }
    return color;
}


void ColorEntry::setColorFromString(const QString &newColor)
{
    QColor clr = parseColorFromString(newColor);
    if(clr.isValid()) {
        setColor(clr);
    }
}

//---------------------------------------------------------------------


QString ColorEntry::key() const
{
    return _key;
}

void ColorEntry::setKey(const QString &key)
{
    _key = key;
}

QColor ColorEntry::color() const
{
    return _color;
}

void ColorEntry::setColor(const QColor &color)
{
    _isSet = true ;
    _color = color;
}

bool ColorEntry::isSet() const
{
    return _isSet;
}

void ColorEntry::setIsSet(bool isSet)
{
    _isSet = isSet;
}

QColor ColorEntry::defaultValue() const
{
    return _defaultValue;
}

void ColorEntry::setDefaultValue(const QColor &defaultValue)
{
    _defaultValue = defaultValue;
}
