/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2019 by Luca Bellonda and individual contributors       *
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

#ifndef XMLTEST_H
#define XMLTEST_H

#include <xmlEdit.h>
#include "modules/xml/xmlloadcontext.h"

class UIDelegate;

class XMLTest : public QObject
{
    Q_OBJECT
public:
    explicit XMLTest(QObject *parent = 0);
    virtual ~XMLTest();

    bool readTestXML(XMLLoadContext *context, QXmlStreamReader *xmlReader);
    bool readTestFromStream(XMLLoadContext *context, QXmlStreamReader *xmlReader);
    bool loadTestFileInnerStream(XMLLoadContext *context, QIODevice *ioDevice);

public:
    bool testLoadFile(XMLLoadContext *context, const QString &filePath);
    bool testXMLFile(UIDelegate *uiDelegate, const QString &filePath);

signals:

public slots:
};

#endif // XMLTEST_H
