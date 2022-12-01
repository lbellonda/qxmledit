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

#include "anonalgpattern.h"
#include "anoncharutils.h"

#ifdef  QXML_DEBUG
#define DD(x)    do{x;}while(0)
#else
#define DD(x)
#endif


AnonStatAlgPattern::AnonStatAlgPattern(const int newId, const QString &value)
{
    _id = newId ;
    _pattern = value;
}

AnonStatAlgPattern::~AnonStatAlgPattern()
{
    //
}

QString AnonStatAlgPattern::pattern()
{
    return _pattern ;
}

int AnonStatAlgPattern::id()
{
    return _id ;
}

QString AnonStatAlgPattern::scanPattern(const QString &token, ScanPatternInfo &info)
{
    DD(qDebug() << QString("  scan pattern=%1").arg(token));
    info.type = AnonProducer::ASCII;
    QString pattern;
    const int size = token.length();
    for(int index = 0 ; index < size ; index ++) {
        QChar theChar = token.at(index);
        DD(qDebug() << QString("  input=%1").arg(theChar));
        if(theChar.isLetter()) {
            if(theChar.isLower()) {
                pattern.append(ALPHA_LOWER);
            } else {
                pattern.append(ALPHA_UPPER);
            }
            if(AnonProducer::ASCII == info.type) {
                DD(qDebug() << QString("  ascii"));
                if(theChar > 127) {
                    DD(qDebug() << QString("  over"));
                    if(AnonCharUtils::isCyrillic(theChar)) {
                        info.type = AnonProducer::CYRILLIC;
                    } else if(AnonCharUtils::isWestern(theChar)) {
                        DD(qDebug() << QString("  west"));
                        info.type = AnonProducer::WESTERN;
                    } else if(AnonCharUtils::isEastEurope(theChar)) {
                        info.type = AnonProducer::EASTEUROPE;
                    }
                    // other: stick to ASCII
                }
            }
        } else if(theChar.isDigit()) {
            pattern.append(DIGIT);
        } else {
            pattern.append(theChar);
        }
    }
    return pattern;
}

QString AnonStatAlgPattern::value(AnonProducer *generator, const AnonProducer::ESeqType type)
{
    const int size = _pattern.length();
    QString result;
    for(int index = 0 ; index < size ; index ++) {
        QChar theChar = _pattern.at(index);
        switch(theChar.unicode()) {
        case ALPHA_LOWER:
            result.append(generator->nextLetter(false, type));
            break;
        case ALPHA_UPPER:
            result.append(generator->nextLetter(true, type));
            break;
        case DIGIT:
            result.append(generator->nextDigit());
            break;
        default:
            result.append(theChar);
            break;
        }
    }
    return result;
}
