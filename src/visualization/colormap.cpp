/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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
#include "colormap.h"
#include <QColor>

#define VOID_COLOR  qRgb(0xFF, 0xFF, 0xFF)
#define GRAY_COLOR qRgb(0xD0, 0xD0, 0xD0)

const int ColorMap::MapElements = 1000;
const int ColorMap::Map0To1 = 256;
const int ColorMap::Map1To2 = 512;
const int ColorMap::Map2To3 = 768;

ColorMap::ColorMap(const QString &newName, QObject *parent) :
    QObject(parent)
{
    _name = newName;
    _values = new uint[MapElements];
    initMap();
}

ColorMap::~ColorMap()
{
    if(NULL != _values) {
        delete [] _values;
        _values = NULL;
    }
}

uint *ColorMap::values()
{
    return _values;
}

void ColorMap::fillRange(const int start, const int end, uint *p, uint colorStart, uint colorEnd)
{
    int size = end - start + 1;
    int r2 = qRed(colorStart);
    int g2 = qGreen(colorStart);
    int b2 = qBlue(colorStart);

    int r1 = qRed(colorEnd);
    int g1 = qGreen(colorEnd);
    int b1 = qBlue(colorEnd);
    int count = end - start ;
    for(int i = 0 ; i < count ; i++) {
        uint r = ((r1 * i) + (r2 * (count - i))) / size;
        uint g = ((g1 * i) + (g2 * (count - i))) / size;
        uint b = ((b1 * i) + (b2 * (count - i))) / size;
        *p++ = qRgb(r, g, b);
    }
}


bool ColorMap::initMap()
{
    if(NULL == _values) {
        return false;
    }
    fillRange(0, Map0To1, &_values[0], qRgb(0xFF, 0xFF, 0), qRgb(0x0, 0xFF, 0xFF));
    fillRange(Map0To1, Map1To2, &_values[Map0To1], qRgb(0x0, 0xFF, 0xFF), qRgb(0x40, 0x80, 0xA0));
    fillRange(Map1To2, Map2To3, &_values[Map1To2], qRgb(0x40, 0x80, 0xA0), qRgb(0xFF, 0, 0));
    fillRange(Map2To3, MapElements, &_values[Map2To3], qRgb(0xFF, 0, 0), qRgb(0, 0, 0));
    return true ;
}

uint ColorMap::backgroundColor()
{
    return VOID_COLOR;
}

uint ColorMap::emptyColor()
{
    return GRAY_COLOR;
}

QString ColorMap::name()
{
    return _name;
}
