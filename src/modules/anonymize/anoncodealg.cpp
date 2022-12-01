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


#include "anoncodealg.h"

AnonCodeAlg::AnonCodeAlg(const bool parmAutodelete, AnonProducer *theProducer) : AnonAllAlg(parmAutodelete, theProducer)
{
    _threshold = Threshold ;
}

AnonCodeAlg::~AnonCodeAlg()
{
}

QString AnonCodeAlg::processText(AnonAlgStatContext &context, const QString &path, const QString &input)
{
    bool anonymize = false;
    int length = input.length();
    int letters = 0;
    for(int i = 0 ; i < length ; i ++) {
        QChar ch = input.at(i);
        if(ch.isLetterOrNumber()) {
            letters ++ ;
            if(letters > _threshold) {
                anonymize = true;
                break ;
            }
        }
    }
    if(anonymize) {
        QString result = AnonAllAlg::processText(context, path, input);
        return result ;
    }
    return input ;
}


int AnonCodeAlg::threshold() const
{
    return _threshold;
}

void AnonCodeAlg::setThreshold(int threshold)
{
    _threshold = threshold;
}
