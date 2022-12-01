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


#ifndef ANONSEQPRODUCER_H
#define ANONSEQPRODUCER_H

#include "libQXmlEdit_global.h"
#include <xmlEdit.h>
#include "anonbase.h"
#include "modules/anonymize/algstat/anoncharutils.h"

class LIBQXMLEDITSHARED_EXPORT AnonSeqProducer : public AnonProducer
{
protected:
    int _letter;
    int _digit;
    bool _errorCharSet ;

    AnonCharSet _westernCharSet;
    AnonCharSet _eastEuropeCharSet;

    int nextLetterCyrillic(const bool uppercase);
    int nextLetterASCII(const bool uppercase);
    virtual int nextLetterPos();
public:
    AnonSeqProducer();
    virtual ~AnonSeqProducer();

    virtual QChar nextLetter(const bool uppercase, const ESeqType type);
    virtual QChar nextDigit() ;
    virtual bool isError() ;
#ifdef  QXMLEDIT_TEST
    friend class TestAnonymize;
#endif
};

class LIBQXMLEDITSHARED_EXPORT AnonFlatSeqProducer : public AnonSeqProducer
{
    virtual int nextLetterPos();
public:
    AnonFlatSeqProducer();
    virtual ~AnonFlatSeqProducer();
};

#endif // ANONSEQPRODUCER_H
