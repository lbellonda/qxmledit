/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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

#ifndef SCANSAX_H
#define SCANSAX_H

#include <QXmlDefaultHandler>
#include <QMap>

class XmlScanInfo
{
public:
    QString token;
    QStringList tokens;
    bool isGroup ;
    int * occurrences;
    int * currentCount;
    bool *inAxisArray;
    int size ;
    bool isError;
    volatile bool isAbort;
    QString errorMessage;
    QMap<int, int> *groups;

    XmlScanInfo();
    ~XmlScanInfo();

    bool init(const QString &value, const bool isGroup);
    void reset();
    bool check();
};


class ScanSax : public QXmlDefaultHandler
{
    int deep;
    XmlScanInfo &info;

    void setFinalCountForItem(const int level);
public:
    ScanSax(XmlScanInfo &newValue);
    ~ScanSax();

    bool startElement(const QString &namespaceURI, const QString &localName,
                      const QString &qName, const QXmlAttributes &attributes);
    bool endElement(const QString &namespaceURI, const QString &localName,
                    const QString &qName);
    bool fatalError(const QXmlParseException &exception);
    bool error(const QXmlParseException &exception);
    QString errorString() const;
};

#endif // SCANSAX_H
