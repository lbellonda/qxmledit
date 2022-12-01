/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2022 by Luca Bellonda and individual contributors       *
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

#include "anonalgfreqtarget.h"

AnonAlgFreqTarget::AnonAlgFreqTarget()
{
    _count = 0 ;
    _minInterval0To1Value = 0 ;
    _maxInterval0To1Value = 0;
    _id = 0;
}

AnonAlgFreqTarget::AnonAlgFreqTarget(const int newCount)
{
    _count = newCount;
}

AnonAlgFreqTarget::~AnonAlgFreqTarget()
{
    //
}

double AnonAlgFreqTarget::minInterval0To1Value() const
{
    return _minInterval0To1Value;
}

double AnonAlgFreqTarget::maxInterval0To1Value() const
{
    return _maxInterval0To1Value;
}


void AnonAlgFreqTarget::newHit()
{
    _count++;
}

int AnonAlgFreqTarget::count()
{
    return _count;
}

void AnonAlgFreqTarget::setRandomValueInterval0To1(const double newMinValue, const double newMaxValue)
{
    _minInterval0To1Value = newMinValue ;
    _maxInterval0To1Value = newMaxValue;
}

void AnonAlgFreqTarget::setLastInInterval0To1()
{
    _maxInterval0To1Value = 1;
}

bool AnonAlgFreqTarget::isInInterval0To1(const double value)
{
    if((value >= _minInterval0To1Value) && (value <= _maxInterval0To1Value)) {
        return true;
    }
    return false;
}

int AnonAlgFreqTarget::id() const
{
    return _id;
}

void AnonAlgFreqTarget::setId(int id)
{
    _id = id;
}

QString AnonAlgFreqTarget::key() const
{
    return _key;
}

void AnonAlgFreqTarget::setKey(const QString &key)
{
    _key = key;
}
