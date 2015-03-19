/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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


#include "anonallalg.h"

AnonAllAlg::AnonAllAlg(const bool parmAutodelete, AnonProducer *theProducer) : AnonAlg(parmAutodelete, theProducer)
{
}

AnonAllAlg::~AnonAllAlg()
{
}

QString AnonAllAlg::processText(const QString &input)
{
    int length = input.length();
    QString result;
    for(int i = 0 ; i < length ; i ++) {
        QChar ch = input.at(i);
        if(ch.isLetter()) {
            QChar chRes = _producer->nextLetter(ch.isUpper());
            result.append(chRes);
        } else if(ch.isDigit()) {
            QChar chRes = _producer->nextDigit();
            result.append(chRes);
        } else {
            result.append(ch);
        }
    }
    return result ;
}
