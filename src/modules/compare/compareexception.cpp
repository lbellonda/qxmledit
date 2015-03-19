/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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


#include "compareexception.h"
#include "utils.h"

CompareException::CompareException(const QString &theCause) : _cause(theCause)
{
    Utils::error(QString("TODO: compare exception thrown, cause:%1 ").arg(theCause));
}

CompareException::CompareException()
{
}

const char* CompareException::what() const throw()
{
    return _cause.toLatin1().data();
}

QString CompareException::cause() const throw()
{
    return _cause;
}

CompareException::~CompareException() throw()
{
}

void CompareError(const QString &message)
{
    throw new CompareException(message);
}
