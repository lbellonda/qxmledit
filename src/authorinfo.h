/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
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

#ifndef AUTHORINFO_H
#define AUTHORINFO_H

#include <QString>
#include <QList>

enum EAuthorRole {
    AR_DEVELOPER,
    AR_TRANSLATOR,
    AR_CONTRIBUTOR
};

class AuthorInfo
{
public:
    QString name;
    QString contactInfo;
    QString specialities;
    EAuthorRole role;

    AuthorInfo(const QString &newName, const QString &newContactInfo, const EAuthorRole newRole, const QString &newSpecialities) {
        name = newName ;
        contactInfo = newContactInfo;
        role = newRole;
        specialities = newSpecialities;
    }

    static const QString appName;
    static const QString version;
    static const QString copyright;
    static const QString license;
    static const QString other;

};


QList<AuthorInfo*> authorsInfo();
void deleteAuthorsInfo(QList<AuthorInfo*> &authors);

#endif // AUTHORINFO_H
