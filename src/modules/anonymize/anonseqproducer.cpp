/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2022 by Luca Bellonda and individual contributors  *
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

#include "anonseqproducer.h"
#include "modules/anonymize/algstat/anoncharutils.h"

#ifdef  QXML_DEBUG
#define DD(x)    do{x;}while(0)
#else
#define DD(x)
#endif

#ifdef QXMLEDIT_TEST
bool __qxmledit_test_nextLetterCond = false;
#endif

AnonSeqProducer::AnonSeqProducer()
{
    _letter = 0 ;
    _digit = 0 ;
    _errorCharSet = false;
    if(!_westernCharSet.buildCharSet(0x00C0, 0x00CF, 0x00E0, 0x00EF)) {
        _errorCharSet = true ;
    }
    if(!_eastEuropeCharSet.buildCharSet(0x0100, 0x017F, 0, 0)) {
        _errorCharSet = true ;
    }
}

AnonSeqProducer::~AnonSeqProducer()
{
}

int AnonSeqProducer::nextLetterPos()
{
    const int ASCIIPos = 26;
    const double fractionProbability = ((double)qrand()) / (double)RAND_MAX;
    const int res = (int)(fractionProbability * (ASCIIPos));
    return res;
}

int AnonSeqProducer::nextLetterASCII(const bool uppercase)
{
    int res = nextLetterPos();
    if(uppercase) {
        res += 'A' ;
    } else {
        res += 'a' ;
    }
    return res ;
}

int AnonSeqProducer::nextLetterCyrillic(const bool uppercase)
{
    int res = _letter % AnonCharUtils::CyrillicSize;
    if(uppercase) {
        res += 0x0410 ;
    } else {
        res += 0x0430 ;
    }
    return res ;
}

QChar AnonSeqProducer::nextLetter(const bool uppercase, const ESeqType type)
{
#ifdef QXMLEDIT_TEST
    if(__qxmledit_test_nextLetterCond) {
        switch(type) {
        case AnonProducer::WESTERN:
            _letter++;
            if(uppercase) {
                return 0x00C0;
            } else {
                return 0x00E0;
            }
            break;
        case AnonProducer::EASTEUROPE:
            _letter++;
            if(uppercase) {
                return 0x0160;
            } else {
                return 0x0161;
            }
            break;
        default:
            break;
        }
    }
#endif
    DD(qDebug() << QString("  IN: ucase %1 type:%2").arg(uppercase).arg(type));
    int res ;
    switch(type) {
    default:
    case AnonProducer::ASCII:
        DD(qDebug() << QString("  ASCII"));
        res = nextLetterASCII(uppercase);
        break;
    case AnonProducer::WESTERN:
        DD(qDebug() << QString("  WESTERN"));
        res = _westernCharSet.getChar(!uppercase);
        DD(qDebug() << QString("  res=%1").arg(res));
        break;
    case AnonProducer::EASTEUROPE:
        DD(qDebug() << QString("  EAST E"));
        res = _eastEuropeCharSet.getChar(!uppercase);
        break;
    case AnonProducer::CYRILLIC:
        res = nextLetterCyrillic(uppercase);
    }
    _letter++;
    DD(qDebug() << QString("  OUT: %1 ").arg(res));
    return res ;
}

QChar AnonSeqProducer::nextDigit()
{
    int res = _digit % 10 ;
    _digit++;
    res += '0' ;
    return res;
}

bool AnonSeqProducer::isError()
{
    return _errorCharSet;
}

//---------------

AnonFlatSeqProducer::AnonFlatSeqProducer() : AnonSeqProducer()
{
    //
}

AnonFlatSeqProducer::~AnonFlatSeqProducer()
{
    //
}

int AnonFlatSeqProducer::nextLetterPos()
{
    DD(qDebug() << QString("  NEXT LETTERPOS : %1 ").arg(_letter % 26));
    return _letter % 26 ;
}
