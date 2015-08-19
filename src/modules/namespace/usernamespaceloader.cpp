/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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

#include "usernamespaceloader.h"
#include "data/DataInterface.h"

UserNamespaceLoader::UserNamespaceLoader()
{
}

UserNamespaceLoader::~UserNamespaceLoader()
{
}

QList<UserNamespace*> UserNamespaceLoader::loadUserNamespaces(DataInterface *dataInterface, bool &isOk)
{
    QList<GenericPersistentData*> list;
    isOk = true ;
    OperationStatus* oper = dataInterface->readAllGenericData(QXMLE_PDATA_TypeNamespace, list);
    if((NULL == oper) || !oper->isOk()) {
        isOk = false;
    }
    if(NULL != oper) {
        delete oper ;
    }
    QList<UserNamespace*> userNamespacesList;
    if(isOk) {
        foreach(GenericPersistentData * data, list) {
            UserNamespace *newNS = new UserNamespace();
            if(!newNS->setData(static_cast<GenericPersistentDBData*>(data))) {
                isOk = false;
            }
            userNamespacesList.append(newNS);
        }
    }
    return userNamespacesList ;
}

UserNamespace *UserNamespaceLoader::createUserNamespace(DataInterface *dataInterface)
{
    GenericPersistentDBData *data = static_cast<GenericPersistentDBData*>(dataInterface->newPersistentDatum(QXMLE_PDATA_TypeNamespace));
    if(NULL != data) {
        UserNamespace *ns = new UserNamespace();
        if(NULL != ns) {
            ns->setData(data);
            return ns ;
        } else {
            delete data ;
        }
    }
    return NULL;
}

bool UserNamespaceLoader::deleteUserNamespace(DataInterface *dataInterface, UserNamespace *ns)
{
    bool isOk = true;
    OperationStatus* oper = dataInterface->deleteGenericData(ns->data());
    if((NULL == oper) || !oper->isOk()) {
        isOk = false;
    }
    if(NULL != oper) {
        delete oper;
    }
    return isOk;
}

bool UserNamespaceLoader::saveUserNamespace(DataInterface *dataInterface, UserNamespace *ns)
{
    bool isOk = true;
    OperationStatus* oper = NULL ;
    if(!ns->syncToData()) {
        return false;
    }
    if(ns->data()->id() == 0) {
        oper = dataInterface->insertGenericData(ns->data());
    } else {
        oper = dataInterface->updateGenericData(ns->data());
    }
    if((NULL == oper) || !oper->isOk()) {
        isOk = false;
    }
    if(NULL != oper) {
        delete oper;
    }
    return isOk;
}
