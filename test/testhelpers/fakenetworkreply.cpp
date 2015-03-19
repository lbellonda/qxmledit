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


#include "fakenetworkreply.h"
#include <QTimer>
#include <QFile>

FakeNetworkReply::FakeNetworkReply(const QString &file, QObject *parent) :
    QNetworkReply(parent)
{
    _file = file;
    QTimer::singleShot(50, this, SLOT(onTimerStart()));
}

FakeNetworkReply::~FakeNetworkReply()
{
}


void FakeNetworkReply::onTimerStart()
{
    bool ok = false;
    // read the data.
    QFile file (_file);
    if( file.open(QIODevice::ReadOnly)) {
        if( open(QIODevice::WriteOnly)) {
            _fakeData = file.readAll();
            ok = true ;
            close();
        }
        file.close();
    }
    if( ok ) {
        emit finished();
    } else {
        emit error(TimeoutError);
    }
}

quint64 FakeNetworkReply::readBufferSize() const
{
    return _fakeData.size() ;
}

qint64 FakeNetworkReply::bytesAvailable() const
{
    return _fakeData.size() + QIODevice::bytesAvailable();
}

qint64 FakeNetworkReply::writeData(const char * /*data*/, qint64 /*maxlen*/)
{
    return 0;
}

qint64 FakeNetworkReply::readData(char *data, qint64 len)
{
    qint64 readLen = -1 ;
    if( _fakeData.size() != 0 ) {
        qint64 limit = (len < _fakeData.size())? len : _fakeData.size();
        memcpy( data, _fakeData.data(), limit );
        readLen = limit;
        // emulates the real object
        _fakeData.remove(0, readLen);
    }

    return readLen;
}

QByteArray FakeNetworkReply::readAll()
{
    QByteArray result(_fakeData.data(),  _fakeData.size());
    return result;
}

void FakeNetworkReply::abort()
{
    emit finished();
}
