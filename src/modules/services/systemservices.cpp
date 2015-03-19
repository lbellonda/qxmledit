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


#include "systemservices.h"
#include "xmlEdit.h"

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QDesktopServices>
#endif

#include <QDesktopServices>


SystemServices::SystemServices()
{
}

SystemServices::~SystemServices()
{
}

//------ static methods ----

QString SystemServices::tempLocation()
{
    QString result ;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    result = QStandardPaths::writableLocation(QStandardPaths::TempLocation) ;
#else
    result = QDesktopServices::storageLocation(QDesktopServices::TempLocation);
#endif
    return result;
}

QString SystemServices::storageDataLocation()
{
    QString result ;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    result = QStandardPaths::writableLocation(QStandardPaths::DataLocation) ;
#else
    result = QDesktopServices::storageLocation(QDesktopServices::DataLocation) ;
#endif
    return result;
}


QString SystemServices::cacheProgramDirectory()
{
    QString cacheProgramDirectory ;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    cacheProgramDirectory = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
#else
    cacheProgramDirectory = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
#endif
    return cacheProgramDirectory;
}


QString SystemServices::userDocumentsDirectory()
{
    QString userDocuments ;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QStringList folderList = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation) ;
    if(folderList.size() > 0) {
        userDocuments = folderList.first();
    }
#else
    userDocuments = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
#endif
    return userDocuments;
}

QString SystemServices::userNameFromEnv()
{
#if defined(Q_OS_WIN)
    QByteArray qName = qgetenv("USERNAME");
#else
    QByteArray qName = qgetenv("USER");
#endif
    return QString(qName.constData());
}


bool SystemServices::isWindows()
{
#if defined(Q_WS_WIN) || defined(Q_OS_WIN)
    return true;
#else
    return false;
#endif
}
