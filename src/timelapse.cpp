/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2018 by Luca Bellonda and individual contributors       *
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

#include "timelapse.h"
#include <stdio.h>

TimeLapse::TimeLapse(const QString &message)
{
    _message = message ;
}

TimeLapse::~TimeLapse()
{
}

void TimeLapse::reset()
{
    _dtStart = QDateTime::currentDateTime();
}

void TimeLapse::end()
{
    _dtEnd = QDateTime::currentDateTime();
}

long TimeLapse::elapsed()
{
    return _dtStart.msecsTo(_dtEnd);
}

QString TimeLapse::toString()
{
    QString res = QString("%1 %2 sec").arg(_message).arg((elapsed() / 1000));
    return res ;
}

void TimeLapse::printElapsed()
{
    QString msg = toString();
    printf("%s\n", msg.toLatin1().data());
    fflush(stdout);
}

void TimeLapse::stopAndPrintElapsed()
{
    end();
    printElapsed();
}
