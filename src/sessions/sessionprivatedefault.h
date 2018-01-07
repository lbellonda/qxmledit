/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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


#ifndef SESSION_PRIVATEDEFAULT_H
#define SESSION_PRIVATEDEFAULT_H

#include "sessionprivate.h"

class Session::PrivateDefault : public Session::Private
{
    Q_OBJECT

    int _instances;
    AccessModel *newAccessModel(FileModel *file, const QDateTime &accessDate);
    FileModel *newFileModel(const QString &path);

public:
    PrivateDefault(Session *p, QObject *parent = 0);
    virtual ~PrivateDefault();
    bool enrollFile(SessionDataInterface *access, const QString &filePath);
    bool read(SessionDataInterface *access, const int idSession);

    bool touch(SessionDataInterface *dataAccess);
    void setDefaultData(QStringList files);
    bool isDefaultSession() const;

};


#endif // SESSION_PRIVATEDEFAULT_H
