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

#ifndef ANONALGSTATCONTEXT_H
#define ANONALGSTATCONTEXT_H

#include <xmlEdit.h>
#include "libQXmlEdit_global.h"
#include "modules/anonymize/anonbase.h"

class AnonProducer;

class LIBQXMLEDITSHARED_EXPORT AnonAlgStatContext
{
    bool _error;
    QString _errorMessage;
    QString _errorDetail;
    AnonProducer *_producer;

    void init(AnonProducer *producer);
public:
    AnonAlgStatContext(AnonProducer *producer);
    AnonAlgStatContext();
    virtual ~AnonAlgStatContext();

    bool setError(const QString &msg, const QString &detail);
    QString errorDetail() const;
    void setErrorDetail(const QString &errorDetail);
    QString errorMessage() const;
    void setErrorMessage(const QString &errorMessage);
    bool setError(const bool value);
    bool isError() const;
    AnonProducer *producer() const;
    void setProducer(AnonProducer *producer);
};

#endif // ANONALGSTATCONTEXT_H

