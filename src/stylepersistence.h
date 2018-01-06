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
#ifndef STYLEPERSISTENCE_H
#define STYLEPERSISTENCE_H

#include "style.h"
#include <QDomElement>

class StylePersistence : public QObject
{

    Q_OBJECT
    QHash<QString, TokenEntry*> mapTokens;

public:
    StylePersistence();
    ~StylePersistence();

    bool scanDirectory(const QString &dirName, QVector<VStyle*> *styles);
    bool readStyleFile(VStyle* tyles, const QString & filePath);

private:
    bool collectAnId(VStyle *style, QDomElement *node);
    bool collectIds(VStyle *style, QDomNodeList &nodes);
    bool collectKeywords(VStyle *style, QDomNodeList &nodes);
    bool collectAKeyword(VStyle *style, QDomElement *keyWrd);
    StyleEntry *collectStyle(QDomElement *element);
    bool collectAStyle(VStyle *style, QDomElement *keyWrd);
    bool collectStyles(VStyle *style, QDomNodeList &nodes);
    bool scanStyleData(VStyle *style, QDomNode &rootNode);
    bool scanData(QVector<VStyle*> *styles, QDomNode &rootNode);
    bool readStyleFile(QVector<VStyle*> *styles, const QString & filePath);
    void completeStyle(VStyle *style);
    bool scanDataSingleStyle(VStyle* style, QDomNode &rootNode);
    bool collectDefault(VStyle *style, QDomNodeList nodes);
    StyleRule* collectRule(QDomElement *element);
    StyleRuleSet* collectRuleSet(QDomElement *element);
};

#endif // STYLEPERSISTENCE_H
