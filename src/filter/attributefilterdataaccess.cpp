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

#include "attributefilterdataaccess.h"
#include "utils.h"

//----------------------------------------------------------------


AttributeFilterDataAccess::AttributeFilterDataAccess(SessionDataInterface *newDataAccess, QObject *parent) :
    QObject(parent)
{
    _dataAccess = newDataAccess ;
}

AttributeFilterDataAccess::~AttributeFilterDataAccess()
{
}

void AttributeFilterDataAccess::setDataAccess(SessionDataInterface *newDataAccess)
{
    _dataAccess = newDataAccess ;
}

QList<AttrFilterProfile*> AttributeFilterDataAccess::storageReadProfiles(DataResult &result)
{
    return _dataAccess->attributeNamesFilterReadProfiles(result);
}

AttrFilterDetail *AttributeFilterDataAccess::storageReadProfileDetails(DataResult &result, const int idProfile)
{
    return _dataAccess->attributeNamesFilterReadDetail(result, idProfile);
}

void AttributeFilterDataAccess::storageDeleteProfile(DataResult &result, const int idProfile)
{
    _dataAccess->attributeNamesFilterDeleteProfile(result, idProfile);
}

void AttributeFilterDataAccess::storageSaveProfile(DataResult &result, AttrFilterProfile *profile, AttrFilterDetail *detail)
{
    _dataAccess->attributeNamesFilterSaveProfile(result, profile, detail);
}

