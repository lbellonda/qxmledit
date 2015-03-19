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

#include "attributefiltermanagement.h"
#include "attributeprofilemgmtdialog.h"
#include "utils.h"
#include "fakeattributefilterdataaccess.h"

FakeAttributeFilterDataAccess fakeAccess;

AttributeFilterManagement::AttributeFilterManagement(QObject *parent) :
    QObject(parent)
{
}

AttributeFilterManagement::~AttributeFilterManagement()
{
}

AttributeFilterDataAccess* AttributeFilterManagement::dataAccess()
{
    return &_dataAccess ;
}


void AttributeFilterManagement::setDataAccess(SessionDataInterface *value)
{
    _dataAccess.setDataAccess(value) ;
}

AttributeFilter *AttributeFilterManagement::showProfiles(QWidget *parent, UIDelegate *uiDelegate)
{
    AttributeProfileMgmtDialog dialog(uiDelegate, this, parent);
    dialog.show();
    int result = dialog.exec() ;
    AttributeFilter *filter = handleResult(dialog, result);
    return filter;
}



AttributeFilter *AttributeFilterManagement::handleResult(AttributeProfileMgmtDialog &dialog, const int result)
{
    if(result == QDialog::Accepted) {
        AttributeFilter *newFilter = dialog.buildFilter();
        if(NULL != newFilter) {
            return newFilter ;
        } else {
            Utils::errorOutOfMem(dialog.parentWidget());
        }
    }
    return NULL;
}


QList<AttrFilterProfile*> AttributeFilterManagement::readProfiles(DataResult &result)
{
    return _dataAccess.storageReadProfiles(result);
}


AttrFilterDetail *AttributeFilterManagement::readDetail(DataResult &result, const int idProfile)
{
    return _dataAccess.storageReadProfileDetails(result, idProfile);
}


void AttributeFilterManagement::deleteProfile(DataResult &result, const int idProfile)
{
    _dataAccess.storageDeleteProfile(result, idProfile);
}

void AttributeFilterManagement::saveProfile(DataResult &result, AttrFilterProfile *profile, AttrFilterDetail *detail)
{
    _dataAccess.storageSaveProfile(result, profile, detail);
}


bool AttributeFilterManagement::insertFilterAsProfile(const QString &name, AttributeFilter *attrFilter)
{
    DataResult result;
    AttrFilterProfile profile;
    AttrFilterDetail detail;

    profile.setName(name);
    profile.setWhiteList(attrFilter->isIsWhiteList());
    profile.setDescription(name);
    profile.setId(0);
    foreach(QString name, attrFilter->attributeNames()) {
        detail.addName(name);
    }
    _dataAccess.storageSaveProfile(result, &profile, &detail);
    return result.isOk();
}
