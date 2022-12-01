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

#ifndef ANONCHARUTILS_H
#define ANONCHARUTILS_H

#include <xmlEdit.h>
#include "modules/anonymize/anonbase.h"
#include "utils.h"

class AnonCharUtils
{
private:
    AnonCharUtils();
    ~AnonCharUtils();

public:
    static const int WesternSize;
    static const int EastEuropeSize;
    static const int CyrillicSize;
    //--
    static const int WesternUpper [] ;
    static const int WesternLower [];
    static const int EastEuropeUpper [];
    static const int EastEuropeLower [];
    //---
    static bool isCyrillic(const QChar theChar);
    static bool isWestern(const QChar theChar);
    static bool isEastEurope(const QChar theChar);
    static bool isASCII(const QChar theChar);
    static AnonProducer::ESeqType type(const QChar theChar);
};

class AnonCharSet
{
    int *_lowerCaseLetters;
    int *_upperCaseLetters;
    int _lowerRange;
    int _upperRange;
    int _lowerCaseCount;
    int _upperCaseCount;

    const double RandMax = (double)RAND_MAX;

    int getCharRange(const int count, const int letterBase, int letters []);
    void findRanges(const int lowerRange, const int upperRange);
    void fillChars(const int lowerRange, const int upperRange, int &upperCaseIndex, int &lowerCaseIndex);
public:
    AnonCharSet();
    ~AnonCharSet();

    int getChar(const bool lowerRange);
    bool buildCharSet(const int lowerRange1, const int upperRange1, const int lowerRange2, const int upperRange2);

};


#endif // ANONCHARUTILS_H
