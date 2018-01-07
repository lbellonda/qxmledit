/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2018 by Luca Bellonda and individual contributors  *
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

#include "stdcolormap.h"

//#define VOID_COLOR  qRgb(0x0, 0x0, 0x0)
#define VOID_COLOR  qRgb(0xD0, 0xD0, 0xD0)
#define GRAY_COLOR qRgb(0x40, 0x40, 0x80)


StdColorMap::StdColorMap(const QString &newName, QObject *parent) : ColorMap(newName, parent)
{
    initMap();
}

StdColorMap::~StdColorMap()
{
}

bool StdColorMap::initMap()
{
    if(NULL == _values) {
        return false;
    }

    fillRange(0, Map0To1, &_values[0], qRgb(0x40, 0x80, 0xFF), qRgb(0x0, 0xFF, 0xFF));
    fillRange(Map0To1, Map1To2, &_values[Map0To1], qRgb(0x0, 0xFF, 0xFF), qRgb(0x80, 0xFF, 0x80));
    fillRange(Map1To2, Map2To3, &_values[Map1To2], qRgb(0x80, 0xFF, 0x80), qRgb(0xFF, 0xFF, 0));
    fillRange(Map2To3, MapElements, &_values[Map2To3], qRgb(0xFF, 0xFF, 0), qRgb(0xFF, 0x20, 0x20));

    return true ;
}

uint StdColorMap::backgroundColor()
{
    return VOID_COLOR;
}

uint StdColorMap::emptyColor()
{
    return GRAY_COLOR;
}
