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


#ifndef FAKENETWORKREPLY_H
#define FAKENETWORKREPLY_H

#include <QNetworkReply>
#include <QBuffer>

class FakeNetworkReply : public QNetworkReply
{
    Q_OBJECT

    QString _file;
    QByteArray _fakeData ;
public:
    explicit FakeNetworkReply(const QString &file, QObject *parent = 0);
    virtual ~FakeNetworkReply();

    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);
    QByteArray readAll();
    qint64 bytesAvailable() const;
    quint64 readBufferSize() const;

signals:

public slots:
    void abort();

private slots:
    void onTimerStart();

};

#endif // FAKENETWORKREPLY_H
