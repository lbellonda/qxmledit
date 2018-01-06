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


#ifndef SEARCHMANAGER_H
#define SEARCHMANAGER_H

#include <QObject>
#include "findtextparams.h"

class SearchManager : public QObject
{
    Q_OBJECT

    static const int LIM_SEARCH_SIZE = 10;

    bool _started;
    QStringList _searchTerms;
    QStringList _searchScopes;
    FindTextParams _params;

    void init();
    void registerSearchItems(const QString &value, QStringList &list);
    void saveSearchItems();
    void loadSearchItems();

public:
    explicit SearchManager(QObject *parent = 0);
    ~SearchManager();

    QStringList searchItems();
    QStringList searchScopes();

    FindTextParams* params();
    void registerSearchTerms(int key, const QString &value, const QString &scope);

    void saveSearchSettings(const FindTextParams *search);
    void saveSearchSettings(const FindTextParams &search);

signals:
    void itemsChanged(const int key);
public slots:

};

#endif // SEARCHMANAGER_H
