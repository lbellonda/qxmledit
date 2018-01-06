/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2018 by Luca Bellonda and individual contributors  *
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


#include "fakeattributefilterdataaccess.h"

FakeAttributeFilterDataAccess::FakeAttributeFilterDataAccess()
{
    resultToShow = true ;
}

FakeAttributeFilterDataAccess::~FakeAttributeFilterDataAccess()
{
}

QList<AttrFilterProfile*> FakeAttributeFilterDataAccess::storageReadProfiles(DataResult &result)
{
    result.setOk(resultToShow);
    QList<AttrFilterProfile*> resultList;
    AttrFilterProfile *first  = new AttrFilterProfile(1, "one", "descr. one", true, QDateTime::currentDateTime(), QDateTime::currentDateTime());
    AttrFilterProfile *second = new AttrFilterProfile(1, "two", "descr. two", false, QDateTime::currentDateTime(), QDateTime::currentDateTime());
    resultList.append(first);
    resultList.append(second);
    return resultList;
}


AttrFilterDetail *FakeAttributeFilterDataAccess::storageReadProfileDetails(DataResult &result, const int /*idProfile*/)
{
    result.setOk(resultToShow);
    AttrFilterDetail *detail = new AttrFilterDetail();
    detail->addName("name1");
    detail->addName("name2");
    return detail;
}

void FakeAttributeFilterDataAccess::storageDeleteProfile(DataResult &result, const int /*idProfile*/)
{
    result.setOk(resultToShow);
}
