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


#ifndef SESSIONSUMMARY_H
#define SESSIONSUMMARY_H

#include <QObject>

#include "QXmlEditSessions_global.h"

#include "sessionsummarydata.h"

class QXMLEDITSESSIONSSHARED_EXPORT SessionSummary : public QObject
{
    Q_OBJECT

    SessionCategoryAccess _mostRecent;
    SessionCategoryAccess _mostUsed;
    SessionCategoryAccess _mostRecentFolders;

    QList<SessionCategoryAccess*> _categories;
public:
    explicit SessionSummary(QObject *parent = 0);
    ~SessionSummary();

    void addRecentFile(SessionFileAccess *fileAccess);
    void addMostUsedFile(SessionFileAccess *fileAccess);
    void addMostRecentFolder(SessionDirectoryAccess *folderAccess);

    SessionCategoryAccess* mostRecent();
    SessionCategoryAccess* mostUsed();
    SessionCategoryAccess* mostRecentFolders();

    QList<SessionCategoryAccess*> *categories();

signals:

public slots:

};

#endif // SESSIONSUMMARY_H
