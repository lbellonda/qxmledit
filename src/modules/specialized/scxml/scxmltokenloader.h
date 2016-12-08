/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016 by Luca Bellonda and individual contributors       *
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

#ifndef SCXMLTOKENLOADER_H
#define SCXMLTOKENLOADER_H

#include "xmlEdit.h"
#include <QXmlDefaultHandler>
#include "modules/specialized/scxml/scxmltoken.h"

class SCXMLTokenLoader : public QXmlDefaultHandler
{
    SCXMLToken    *_currentToken ;
    SCXMLGroup    *_currentGroup ;
    bool _foundChildren;
    QHash<QString, SCXMLToken*> *_tokens;
    QHash<QString, SCXMLGroup*> _groups;
    QString _parseError;

    bool oomError();
    int decodeCardinality(const QString &value);
    bool setErrorMessage(const QString &value);
    SCXMLToken *createSCXMLToken(const QString &tag);
public:
    SCXMLTokenLoader();
    ~SCXMLTokenLoader();

    bool load(const QString &fileName, QHash<QString, SCXMLToken*> *tokens);
    //-----------------------
    bool startElement(const QString &namespaceURI, const QString &localName,
                      const QString &qName, const QXmlAttributes &attributes);
    bool fatalError(const QXmlParseException &exception);
    bool error(const QXmlParseException &exception);
    QString errorString() const;
    //-----------------------
};

#endif // SCXMLTOKENLOADER_H
