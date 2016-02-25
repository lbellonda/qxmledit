/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016 by Luca Bellonda and individual contributors       *
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


#ifndef ANONYMIZEBATCH_H
#define ANONYMIZEBATCH_H

#include "xmlEdit.h"

class ApplicationData;
class AnonProfile;
class AnonOperationBatchOutputFileProvider;

class AnonymizeBatch : public QObject
{
    Q_OBJECT
    QString _fileInputPath;
    QString _profileName;
    QString _fileOutputPath;
    QString _errorMessage ;
    bool _error ;
    ApplicationData *_data;
    //---
    bool error();
    bool setError(const QString &message);
    AnonProfile* loadProfile();
    GenericPersistentData* getProfile(const QString &profileName);
    AnonProfile* getProfileFromProfileData(GenericPersistentData *input);
    AnonOperationBatchOutputFileProvider *_outProvider;
public:
    explicit AnonymizeBatch(ApplicationData *newData, const QString &newFileInputPath, const QString &newProfileName, const QString &newFileOutputPath, QObject *parent = 0);
    ~AnonymizeBatch();
    bool isError();

    bool operation();
    QString errorMessage();
    void setOutputProvider(AnonOperationBatchOutputFileProvider* newProvider);

signals:

public slots:

};

#endif // ANONYMIZEBATCH_H
