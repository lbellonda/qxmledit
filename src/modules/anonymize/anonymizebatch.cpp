/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016-2018 by Luca Bellonda and individual contributors  *
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

#include "anonymizebatch.h"
#include "utils.h"
#include "applicationdata.h"
#include "qxmleditconfig.h"
#include "modules/services/systemservices.h"
#include "modules/anonymize/anonprofile.h"
#include "modules/anonymize/anonprofiledialog.h"
#include "modules/anonymize/anoncontext.h"
#include "anonprofilemanager.h"
#include "modules/anonymize/anonoperationbatch.h"

AnonymizeBatch::AnonymizeBatch(ApplicationData *newData, const QString &newFileInputPath, const QString &newProfileName, const QString &newFileOutputPath, QObject *parent) :
    QObject(parent)
{
    _outProvider = NULL ;
    _error = false ;
    _data = newData ;
    _fileInputPath = newFileInputPath;
    _profileName = newProfileName;
    _fileOutputPath = newFileOutputPath;
}

AnonymizeBatch::~AnonymizeBatch()
{
}

bool AnonymizeBatch::isError()
{
    return _error;
}

QString AnonymizeBatch::errorMessage()
{
    return _errorMessage;
}

bool AnonymizeBatch::error()
{
    _error = true ;
    return false;
}

bool AnonymizeBatch::setError(const QString &message)
{
    _error = true ;
    if(_errorMessage.isEmpty()) {
        _errorMessage = message;
    }
    return false;
}

void AnonymizeBatch::setOutputProvider(AnonOperationBatchOutputFileProvider* newProvider)
{
    if(NULL != newProvider) {
        _outProvider = newProvider ;
    }
}

bool AnonymizeBatch::operation()
{
    AnonProfile *profile = loadProfile();
    if(NULL == profile) {
        return error();
    }
    AnonOperationBatch operation;
    Utils::TODO_THIS_RELEASE("non va");
    operation.setIndent(_data->xmlIndent());
    AnonContext context(NULL, "");
    context.setAlg(profile->params());
    context.setProfile(profile->clone());
    if(NULL != _outProvider) {
        operation.setOutputProvider(_outProvider);
    }
    operation.perform(_fileInputPath, _fileOutputPath, &context);
    const AnonOperationResult *res = operation.result();
    if(NULL != profile) {
        delete profile;
    }
    if(res->isError()) {
        _errorMessage = tr("Error: %1, '%2'").arg(res->code()).arg(res->message());
        return error();
    }
    return true;
}


AnonProfile* AnonymizeBatch::getProfileFromProfileData(GenericPersistentData *input)
{
    AnonProfile * result = new AnonProfile() ;
    bool flag = result->readFromSerializedXmlString(input->payload());
    if(!flag) {
        Utils::error(tr("Unable to parse the profile data."));
        delete result;
        result = NULL ;
    }
    return result;
}


GenericPersistentData* AnonymizeBatch::getProfile(const QString &profileName)
{
    int id = -1 ;

    {
        QList<GenericPersistentData*> list;
        OperationStatus* oper = _data->storageManager()->readAllGenericData(QXMLE_PDATA_TypeAnonProfile, list);
        if((NULL == oper) || !oper->isOk()) {
            setError(tr("Error reading data"));
            return NULL ;
        }
        if(NULL != oper) {
            delete oper;
        }
        foreach(GenericPersistentData * gpi, list) {
            if(gpi->name() == profileName) {
                if(id >= 0) {
                    setError(tr("More than on profile exists with the name '%1'").arg(profileName));
                }
                id = gpi->id() ;
            }
            delete gpi;
        }
        if(id < 0) {
            setError(tr("No profile exists with the name '%1'").arg(profileName));
        }
        if(isError()) {
            return NULL ;
        }
    }
    QList<GenericPersistentData*> resultList;
    OperationStatus* oper2 = _data->storageManager()->readGenericData(QXMLE_PDATA_TypeAnonProfile, id, resultList);
    if((NULL == oper2) || !oper2->isOk()) {
        setError(tr("Error reading data"));
        return NULL ;
    }
    if(NULL != oper2) {
        delete oper2;
    }
    if(resultList.isEmpty()) {
        setError(tr("Unable to read the profile with name '%1'").arg(profileName));
        return NULL ;
    }
    return resultList.first();
}

/*!
 * \brief AnonymizeBatch::loadProfile without show the user interface
 * \return
 */
AnonProfile* AnonymizeBatch::loadProfile()
{
    GenericPersistentData* gpi = getProfile(_profileName);
    AnonProfile* newProfile = NULL ;
    if(NULL != gpi) {
        newProfile = getProfileFromProfileData(gpi);
        delete gpi;
    }
    if(NULL == newProfile) {
        setError(tr("Unable to load profile"));
    }
    return newProfile;
}
