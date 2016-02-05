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


#include "searchmanager.h"
#include "qxmleditconfig.h"

SearchManager::SearchManager(QObject *parent) :
    QObject(parent)
{
    _started = false;
    _params.loadState();
}

SearchManager::~SearchManager()
{
}

void SearchManager::init()
{
    if(_started) {
        return ;
    }
    _started = true ;
    Config::loadStringArray(Config::KEY_SEARCH_ITEMS, _searchTerms);
    Config::loadStringArray(Config::KEY_SEARCH_SCOPES, _searchScopes);
}


void SearchManager::loadSearchItems()
{
    Config::loadStringArray(Config::KEY_SEARCH_ITEMS, _searchTerms);
    Config::loadStringArray(Config::KEY_SEARCH_SCOPES, _searchScopes);
}

void SearchManager::saveSearchItems()
{
    Config::saveStringArray(Config::KEY_SEARCH_ITEMS, _searchTerms);
    Config::saveStringArray(Config::KEY_SEARCH_SCOPES, _searchScopes);
}

void SearchManager::registerSearchItems(const QString &value, QStringList &list)
{
    if(!value.isEmpty()) {
        list.removeAll(value);
        while(list.size() > LIM_SEARCH_SIZE) {
            list.removeAt(list.size() - 1);
        }
        list.insert(0, value);
    }
}

void SearchManager::registerSearchTerms(int key, const QString &value, const QString &scope)
{
    registerSearchItems(value, _searchTerms);
    registerSearchItems(scope, _searchScopes);
    saveSearchItems();
    emit itemsChanged(key);
}

QStringList SearchManager::searchItems()
{
    init();
    return _searchTerms;
}

QStringList SearchManager::searchScopes()
{
    init();
    return _searchScopes;
}

FindTextParams* SearchManager::params()
{
    init();
    return &_params;
}

void SearchManager::saveSearchSettings(const FindTextParams &search)
{
    search.saveState();
}

void SearchManager::saveSearchSettings(const FindTextParams *search)
{
    search->saveState();
}
