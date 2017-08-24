/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2016 by Luca Bellonda and individual contributors  *
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


#ifndef VISDATASAX_H
#define VISDATASAX_H

#include <QXmlDefaultHandler>
#include <QSet>
#include "xmlEdit.h"
#include "elementbase.h"
#include "modules/graph/tagnodes.h"
#include "attributessummarydata.h"

class VisDataSax : public QXmlDefaultHandler
{
public:
    ElementBase *root;
    ElementBase *currentElement;
    QSet<QString> *names;
    QHash<QString, TagNode*> *tagNodes;
    int _elementsCount;
    AttributesSummaryData *attributesSummaryData;
private :
    bool _hasError;
    QString _errorMessage;
    volatile bool _userAborted;
    QString _currentElementPath;
private:
    void addTagNode(const QString &name);
public:
    VisDataSax(QSet<QString> *newNames, QHash<QString, TagNode*> *newTagNodes, AttributesSummaryData *newAttributesSummaryData);
    ~VisDataSax();

    bool startDocument();
    bool startElement(const QString &namespaceURI, const QString &localName,
                      const QString &qName, const QXmlAttributes &attributes);
    bool endElement(const QString &namespaceURI, const QString &localName,
                    const QString &qName);
    bool characters(const QString &str);
    bool fatalError(const QXmlParseException &exception);
    bool error(const QXmlParseException &exception);
    QString errorString() const;
    bool hasError() const;
    void setHasError(bool hasError);
    QString errorMessage() const;
    void setErrorMessage(const QString &errorMessage);
    bool userAborted() const;
    void setUserAborted(bool userAborted);
};


#endif // VISDATASAX_H
