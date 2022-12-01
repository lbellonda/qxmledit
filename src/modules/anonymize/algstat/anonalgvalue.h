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

#ifndef ANONALGVALUE_H
#define ANONALGVALUE_H

#include <xmlEdit.h>
#include "anonalgfreqcount.h"
#include "anonalgstatcontext.h"
#include "anonalgchain.h"

class AnonProducer;
/**
 * @brief One data value
 */
class AnonStatAlgValue : public AnonAlgFreqCount
{
    QString _key;
    QRegExp _splitRegExp;

protected:
    AnonStatAlgWordChain* chainForWords(const int size);
    virtual AnonAlgFreqTarget* newChild();
    void dump(const QString &message);
    QString dumpAsString();

public:
    AnonStatAlgValue();
    virtual~AnonStatAlgValue();

    bool scan(AnonAlgStatContext &context, const QString &input);
    QString value(AnonAlgStatContext &context, AnonProducer *producer);
};

#endif // ANONALGVALUE_H
