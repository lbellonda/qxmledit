/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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


#ifndef ATTRIBUTEFILTERDATAACCESS_H
#define ATTRIBUTEFILTERDATAACCESS_H

#include <QObject>


#include "sessions/data_access/model/attrfilterprofile.h"
#include "sessions/data_access/model/attrfilterdetail.h"
#include "sessions/data_access/dataresult.h"
#include <sessiondata>

class AttributeFilterDataAccess : public QObject
{
    Q_OBJECT

    SessionDataInterface *_dataAccess;
public:
    explicit AttributeFilterDataAccess(SessionDataInterface *newDataAccess = NULL, QObject *parent = 0);
    virtual ~AttributeFilterDataAccess();

    void setDataAccess(SessionDataInterface *newDataAccess);

    virtual QList<AttrFilterProfile*> storageReadProfiles(DataResult &result);
    void storageSaveProfile(DataResult &result, AttrFilterProfile *profile, AttrFilterDetail *detail);
    virtual void storageDeleteProfile(DataResult &result, const int idProfile);
    virtual AttrFilterDetail *storageReadProfileDetails(DataResult &result, const int idProfile);

signals:

public slots:

};

#endif // ATTRIBUTEFILTERDATAACCESS_H
