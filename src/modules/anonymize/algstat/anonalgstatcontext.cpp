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

#include "anonalgstatcontext.h"
#include "utils.h"

AnonAlgStatContext::AnonAlgStatContext(AnonProducer *producer)
{
    init(producer);
}

AnonAlgStatContext::AnonAlgStatContext()
{
    init(NULL);
}

void AnonAlgStatContext::init(AnonProducer *producer)
{
    //
    _error = false ;
    _producer = producer;
}

AnonAlgStatContext::~AnonAlgStatContext()
{
    //
}

AnonProducer *AnonAlgStatContext::producer() const
{
    return _producer;
}

void AnonAlgStatContext::setProducer(AnonProducer *producer)
{
    _producer = producer;
}

bool AnonAlgStatContext::setError(const QString &msg, const QString &detail)
{
    _error = true;
    _errorMessage = msg;
    _errorDetail = detail;
    return false ;
}

QString AnonAlgStatContext::errorDetail() const
{
    return _errorDetail;
}

void AnonAlgStatContext::setErrorDetail(const QString &errorDetail)
{
    _errorDetail = errorDetail;
}


QString AnonAlgStatContext::errorMessage() const
{
    return _errorMessage;
}

void AnonAlgStatContext::setErrorMessage(const QString &errorMessage)
{
    _errorMessage = errorMessage;
}

bool AnonAlgStatContext::isError() const
{
    return _error;
}

bool AnonAlgStatContext::setError(const bool value)
{
    _error = value ;
    return _error ;
}

