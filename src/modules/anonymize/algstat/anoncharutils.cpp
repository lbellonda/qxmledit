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

#include "anoncharutils.h"
#include "utils.h"

//----------------------------------------------
AnonCharUtils::AnonCharUtils()
{
    //
}

AnonCharUtils::~AnonCharUtils()
{
    //
}

//----------------------------------------------

const int AnonCharUtils::WesternUpper [] = {0x00C0, 0x00C9, 0x00C8, 0x00D9, 0x00D2, 0x00CC}; //a e e u o i
const int AnonCharUtils::WesternLower [] = {0x00E0, 0x00E9, 0x00E8, 0x00F9, 0x00F2, 0x00EC};//a e e u o i
const int AnonCharUtils::EastEuropeUpper [] = {0x0106, 0x0110, 0x0160}; // c d s
const int AnonCharUtils::EastEuropeLower [] = {0x0107, 0x0111, 0x0161}; //c d s

const int AnonCharUtils::WesternSize = sizeof(AnonCharUtils::WesternUpper) / sizeof(int);
const int AnonCharUtils::EastEuropeSize = sizeof(AnonCharUtils::EastEuropeUpper) / sizeof(int);
const int AnonCharUtils::CyrillicSize = 0x042F - 0x0410 ;

//----------------------------------------------

__inline static bool checkRange(const QChar theChar, const int rangeMin, const int rangeMax)
{
    if((theChar >= rangeMin) && (theChar <= rangeMax)) {
        return true ;
    }
    return false;
}

bool AnonCharUtils::isCyrillic(const QChar theChar)
{
    return
        checkRange(theChar, 0x0400, 0x04FF) //U+0400–U+04FF, 256 characters
        ||  checkRange(theChar, 0x0500, 0x052F)//Supplement: U+0500–U+052F, 48 characters
        ||  checkRange(theChar, 0x2DE0, 0x2DFF)//Extended-A: U+2DE0–U+2DFF, 32 characters
        ||  checkRange(theChar, 0xA640, 0xA69F)//Extended-B: U+A640–U+A69F, 96 characters
        ||  checkRange(theChar, 0xA640, 0xA69F)//Extended-B: U+A640–U+A69F, 96 characters
        ||  checkRange(theChar, 0x1C80, 0x1C8F)//Extended-C: U+1C80–U+1C8F, 9 characters
        ||  checkRange(theChar, 0x1E030, 0x1E08F)//Extended-D: U+1E030–U+1E08F, 63 characters
        ||  checkRange(theChar, 0x1D2B, 0x1D78)//Phonetic Extensions: U+1D2B, U+1D78, 2 Cyrillic characters
        ||  checkRange(theChar, 0xFE2E, 0xFE2F);//Combining Half Marks: U+FE2E–U+FE2F, 2 Cyrillic characters
}


bool AnonCharUtils::isWestern(const QChar theChar)
{
    return checkRange(theChar, 0x0080, 0x00FF);
}

bool AnonCharUtils::isEastEurope(const QChar theChar)
{
    return checkRange(theChar, 0x0100, 0x017F);
}

bool AnonCharUtils::isASCII(const QChar theChar)
{
    return checkRange(theChar, 0, 0x07F);
}

AnonProducer::ESeqType AnonCharUtils::type(const QChar theChar)
{
    if(isASCII(theChar)) {
        return AnonProducer::ASCII;
    }
    if(isEastEurope(theChar)) {
        return AnonProducer::EASTEUROPE;
    }
    if(isWestern(theChar)) {
        return AnonProducer::WESTERN;
    }
    if(isCyrillic(theChar)) {
        return AnonProducer::CYRILLIC;
    }
    return AnonProducer::UNKNOWN;
}

//--------------------------------------------------------------------------------------

AnonCharSet::AnonCharSet()
{
    _lowerCaseLetters = NULL ;
    _upperCaseLetters = NULL ;
    _lowerCaseCount = 0;
    _upperCaseCount = 0;
    _lowerRange = 0;
    _upperRange = 0;

}

AnonCharSet::~AnonCharSet()
{
    delete [] _lowerCaseLetters ;
    delete [] _upperCaseLetters ;
}

bool AnonCharSet::buildCharSet(const int lowerRange1, const int upperRange1, const int lowerRange2, const int upperRange2)
{
    Utils::TEST_ME("");
    _lowerRange = lowerRange1;
    _upperRange = upperRange1;
    _lowerCaseCount = 0;
    _upperCaseCount = 0;

    findRanges(lowerRange1, upperRange1);
    if(lowerRange2 > 0) {
        findRanges(lowerRange2, upperRange2);
    }
    _lowerCaseLetters = new int [_lowerCaseCount];
    _upperCaseLetters = new int [_upperCaseCount];
    if((NULL == _lowerCaseLetters) && (NULL == _upperCaseLetters)) {
        return false;
    }
    int upperCaseIndex = 0;
    int lowerCaseIndex = 0;
    fillChars(lowerRange1, upperRange1, upperCaseIndex, lowerCaseIndex);
    if(lowerRange2 > 0) {
        fillChars(lowerRange2, upperRange2, upperCaseIndex, lowerCaseIndex);
    }
    return true ;
}


void AnonCharSet::findRanges(const int lowerRange, const int upperRange)
{
    for(int index = lowerRange ; index <= upperRange ; index++) {
        QChar ch(index);
        if(ch.isLetter()) {
            if(ch.isUpper()) {
                _upperCaseCount++;
            } else if(ch.isLower()) {
                _lowerCaseCount++;
            }
        }
    }
}

void AnonCharSet::fillChars(const int lowerRange, const int upperRange, int &upperCaseIndex, int &lowerCaseIndex)
{
    for(int index = lowerRange ; index <= upperRange ; index++) {
        QChar ch(index);
        if(ch.isLetter()) {
            if(ch.isUpper()) {
                _upperCaseLetters[upperCaseIndex++] = ch.unicode();
            } else if(ch.isLower()) {
                _lowerCaseLetters[lowerCaseIndex++] = ch.unicode();
            }
        }
    }
}

int AnonCharSet::getCharRange(const int count, const int letterBase, int letters [])
{
    const int Weight = 2;
    const int ASCIIPos = Weight * 26;
    const double fractionProbability = ((double)qrand()) / RandMax;
    int index = (int)(fractionProbability * (ASCIIPos + count));
    if(index < ASCIIPos) {
        return letterBase + (index / Weight);
    } else {
        return letters[index - ASCIIPos];
    }
}

int AnonCharSet::getChar(const bool lowerRange)
{
    if(lowerRange) {
        return getCharRange(_lowerCaseCount, 'a', _lowerCaseLetters);
    } else {
        return getCharRange(_upperCaseCount, 'A', _upperCaseLetters);
    }
}
