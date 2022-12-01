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

#ifndef ANONALGCHAIN_H
#define ANONALGCHAIN_H

#include "anonalgfreqcount.h"
#include "anonalgword.h"

class AnonProducer;

class AnonStatAlgWordChain : public AnonAlgFreqTarget
{
    QList<AnonStatAlgWord*> _words;
    AnonProducer::ESeqType _type;

protected:
    AnonStatAlgWord* wordForIndex(const int index);
public:
    AnonStatAlgWordChain();
    virtual ~AnonStatAlgWordChain();

    QList<AnonStatAlgWord*> words();
    int wordsCount();
    void scan(AnonAlgStatContext &context, const QStringList &tokens);
    QString value(AnonAlgStatContext &context, AnonProducer *producer);
    bool setTypeIfNotSet(const AnonProducer::ESeqType type);
    AnonProducer::ESeqType type() const;
    QString dumpAsString();
};

#endif // ANONALGCHAIN_H
