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

#ifndef ANONALGPATTERN_H
#define ANONALGPATTERN_H

#include "anonalgstatcontext.h"
#include "anonalgfreqcount.h"
#include "modules/anonymize/anonbase.h"

class ScanPatternInfo
{
public:
    AnonProducer::ESeqType type;
    ScanPatternInfo()
    {
        type = AnonProducer::ASCII;
    }
};

class AnonStatAlgPattern : public AnonAlgFreqTarget
{
    static const int ALPHA_UPPER = 'A';
    static const int ALPHA_LOWER = 'a';
    static const int DIGIT = '0';

    int _id;
    QString _pattern;

public:
    AnonStatAlgPattern(const int id, const QString & value);
    virtual ~AnonStatAlgPattern();

    QString pattern();
    int id();
    QString value(AnonProducer *producer, const AnonProducer::ESeqType type);

    static QString scanPattern(const QString &token, ScanPatternInfo &result);
};

#endif // ANONALGPATTERN_H

