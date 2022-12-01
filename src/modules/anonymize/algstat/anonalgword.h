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

#ifndef ANONALGWORD_H
#define ANONALGWORD_H

#include "anonalgfreqcount.h"
#include "anonalgpattern.h"

class AnonProducer;

class AnonStatAlgWord : public AnonAlgFreqCount, public AnonAlgFreqTarget
{
    QHash<QString, AnonStatAlgPattern*> _patterns;
    AnonProducer::ESeqType _type;

protected:
    AnonStatAlgPattern *findPattern(const QString& pattern);
    AnonStatAlgPattern *findCreatePattern(AnonAlgStatContext &context, const QString& pattern);
    virtual AnonAlgFreqTarget* newChild();
    virtual AnonStatAlgPattern *newPattern(const int id, const QString & value);
public:
    AnonStatAlgWord();
    virtual ~AnonStatAlgWord();

    void scan(AnonAlgStatContext &context, const QString &inputValue);
    QString value(AnonAlgStatContext &context, AnonProducer *producer, const AnonProducer::ESeqType type);
    bool setTypeIfNotSet(const AnonProducer::ESeqType type);
    AnonProducer::ESeqType type() const;
    QString dumpAsString();
    //-----
#ifdef  QXMLEDIT_TEST
    friend class TestAnonymize;
#endif
};

#endif // ANONALGWORD_H

