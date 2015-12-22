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


#ifndef ANONYMIZEPARAMETERS_H
#define ANONYMIZEPARAMETERS_H

#include "libQXmlEdit_global.h"
#include <QList>
#include <QDomDocument>
#include "anonexception.h"

class LIBQXMLEDITSHARED_EXPORT AnonymizeParameters
{
public:
    enum Emodes {
        AllText,
        UsingPatterns
    };

    Emodes mode;
    bool useFixedLetter;
    int threshold;
    QList<AnonException*> _exceptions;
public:
    AnonymizeParameters();
    AnonymizeParameters(const Emodes newMode, const bool newUseFixedLetter);
    ~AnonymizeParameters();

    bool readFromDom(const QDomElement &element);
    bool saveToDom(QDomElement &element);

    bool equals(AnonymizeParameters *other);
    void copyValuesFrom(AnonymizeParameters *other);

};

#endif // ANONYMIZEPARAMETERS_H
