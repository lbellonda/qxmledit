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


#include "anonprofilemanager.h"
#include "modules/anonymize/anonprofiledialog.h"

AnonProfileManager::AnonProfileManager()
{
}

AnonProfileManager::~AnonProfileManager()
{
}

GenericPersistentData *AnonProfileManager::chooseProfile(QXmlEditData *data, QWidget *parent)
{
    AnonProfileDialog handler(data, parent);
    handler.setModal(true);
    if(handler.exec() == QDialog::Accepted) {
        return handler.result();
    }
    return NULL;
}

GenericPersistentData *AnonProfileManager::insertProfile(QWidget *parent, QXmlEditData *data, AnonProfile *inputProfile)
{
    AnonProfileDialog handler(data, parent);
    return handler.insertProfile(inputProfile);
}

bool AnonProfileManager::saveProfile(QWidget *parent, QXmlEditData *data, GenericPersistentData *persData)
{
    AnonProfileDialog handler(data, parent);
    return handler.persistSnippet(persData);
}

//------------------------------------------------------------------

AnonProfileManagerFactory::AnonProfileManagerFactory()
{
}

AnonProfileManagerFactory::~AnonProfileManagerFactory()
{
}

AnonProfileManager* AnonProfileManagerFactory::newProfileManager()
{
    return new AnonProfileManager();
}
