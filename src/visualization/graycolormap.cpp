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

#include "graycolormap.h"

#define VOID_COLOR  qRgb(0x00, 0xFF, 0xFF)
#define GRAY_COLOR qRgb(0xD0, 0x00, 0xD0)

GrayColorMap::GrayColorMap(const QString &newName, QObject *parent) : ColorMap(newName, parent)
{
    initMap();
}

GrayColorMap::~GrayColorMap()
{
}

bool GrayColorMap::initMap()
{
    if(NULL == _values) {
        return false;
    }

    uint *value = &_values[0];
    for(int i = 0 ; i < MapElements ; i++) {
        int grayLevel = (255 * i) / MapElements ;
        *value++ = qRgb(grayLevel, grayLevel, grayLevel);
    }
    return true ;
}

uint GrayColorMap::backgroundColor()
{
    return VOID_COLOR;
}

uint GrayColorMap::emptyColor()
{
    return GRAY_COLOR;
}
