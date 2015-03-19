/***************************************************************************
 *   This file is part of qaccessorsgen                                    *
 *   Copyright (C) 2011 by Luca Bellonda                                   *
 *   lbellonda _at_ gmail.com                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *    Boston, MA  02110-1301, USA.                                         *
 ***************************************************************************/


#include "sessionsummary.h"

SessionSummary::SessionSummary(QObject *parent) :
    QObject(parent)
{
    _categories.append(&_mostRecent);
    _mostRecent.setName(tr("\nRecent"));
    _mostRecent.setIndex(0);
    _categories.append(&_mostUsed);
    _mostUsed.setName(tr("\nMost used"));
    _mostUsed.setIndex(1);
    _categories.append(&_mostRecentFolders);
    _mostRecentFolders.setIndex(2);
    _mostRecentFolders.setName(tr("\nMost recent folders"));

}

SessionSummary::~SessionSummary()
{
}

void SessionSummary::addRecentFile(SessionFileAccess *fileAccess)
{
    _mostRecent.addChild(fileAccess);
}

void SessionSummary::addMostUsedFile(SessionFileAccess *fileAccess)
{
    _mostUsed.addChild(fileAccess);
}

void SessionSummary::addMostRecentFolder(SessionDirectoryAccess *folderAccess)
{
    _mostRecentFolders.addChild(folderAccess);
}

SessionCategoryAccess* SessionSummary::mostRecent()
{
    return &_mostRecent;
}

SessionCategoryAccess* SessionSummary::mostUsed()
{
    return &_mostUsed;
}

SessionCategoryAccess* SessionSummary::mostRecentFolders()
{
    return &_mostRecentFolders;
}

QList<SessionCategoryAccess*> *SessionSummary::categories()
{
    return &_categories;
}


