/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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


#ifndef NODESSAX_H
#define NODESSAX_H

#include <QXmlDefaultHandler>
#include <QStack>
#include "modules/graph/tagnodes.h"

class AttributesSummaryData;

class NodesSax : public QXmlDefaultHandler
{
public:
    QStack<QString> elements;
    QHash<QString, TagNode*> *tagNodes;
    AttributesSummaryData *attributesSummaryData;
private:
    void addTagNode(const QString &name);
    QString pathAsString() const;
    void handleAttributes(const QXmlAttributes & attributes);
public:
    NodesSax(QHash<QString, TagNode*> *newTagNodes, AttributesSummaryData *attributesSummaryData);
    ~NodesSax();

    bool startElement(const QString &namespaceURI, const QString &localName,
                      const QString &qName, const QXmlAttributes &attributes);
    bool endElement(const QString &namespaceURI, const QString &localName,
                    const QString &qName);
    bool fatalError(const QXmlParseException &exception);
    bool error(const QXmlParseException &exception);
    QString errorString() const;
};

#endif // NODESSAX_H
