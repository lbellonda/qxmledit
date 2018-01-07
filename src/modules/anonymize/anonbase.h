/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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


#ifndef ANONBASEALG_H
#define ANONBASEALG_H

#include "libQXmlEdit_global.h"
#include <QString>
#include "anonymizeparameters.h"

class LIBQXMLEDITSHARED_EXPORT AnonProducer
{
public:
    AnonProducer();
    virtual ~AnonProducer();

    virtual QChar nextLetter(const bool uppercase) = 0 ;
    virtual QChar nextDigit() = 0 ;
    virtual QChar nextLetterOrDigit(const bool uppercase) = 0 ;
};

class LIBQXMLEDITSHARED_EXPORT AnonAlg
{
protected:
    AnonProducer *_producer;
    QList<AnonException*> _exceptions;
    bool _autodelete ;
public:
    AnonAlg(const bool parmAutodelete, AnonProducer *theProducer = NULL);
    virtual ~AnonAlg();

    void setProducer(AnonProducer *theProducer)
    {
        _producer = theProducer ;
    }
    AnonProducer * producer()
    {
        return _producer ;
    }

    void autodelete();

    virtual QString processText(const QString &input) = 0 ;
};

#endif // ANONBASEALG_H
