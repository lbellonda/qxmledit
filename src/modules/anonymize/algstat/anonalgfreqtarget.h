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

#ifndef ANONALGFREQTARGET_H
#define ANONALGFREQTARGET_H

#include <xmlEdit.h>
#include "utils.h"

class AnonAlgFreqTarget
{
    QString _key;
    int _id;
    int _count;
    double _minInterval0To1Value;
    double _maxInterval0To1Value;
public:
    AnonAlgFreqTarget();
    AnonAlgFreqTarget(const int newCount);
    virtual ~AnonAlgFreqTarget();

    void newHit();
    int count();
    void setRandomValueInterval0To1(const double newMinValue, const double newMaxValue);
    void setLastInInterval0To1();
    bool isInInterval0To1(const double value);
    double minInterval0To1Value() const;
    double maxInterval0To1Value() const;
    int id() const;
    void setId(int id);
    QString key() const;
    void setKey(const QString &key);

#ifdef QXMLEDIT_TEST
    friend class TestAnonymize;
#endif

};

#endif // ANONALGFREQTARGET_H

