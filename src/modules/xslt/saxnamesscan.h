/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013-2018 by Luca Bellonda and individual contributors  *
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


#ifndef SAXNAMESSCAN_H
#define SAXNAMESSCAN_H

#include <QXmlDefaultHandler>
#include "modules/xslt/xslthelper.h"

class SaxNamesScan : public QXmlDefaultHandler
{
    XsltHelper *_helper;
public:
    explicit SaxNamesScan(XsltHelper *helper);
    virtual ~SaxNamesScan();

    bool startElement(const QString &namespaceURI, const QString &localName,
                      const QString &qName, const QXmlAttributes &attributes);
    bool fatalError(const QXmlParseException &exception);
    bool error(const QXmlParseException &exception);
    QString errorString() const;

signals:

public slots:

};

#endif // SAXNAMESSCAN_H
