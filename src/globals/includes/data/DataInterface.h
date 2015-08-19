/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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


#ifndef DATAINTERFACE_H
#define DATAINTERFACE_H

#include "data/GenericPersistentData.h"

#define QXMLE_PDATA_TypeSearchlet "TySrchl"
#define QXMLE_PDATA_TypeAnonProfile "TyApro"
#define QXMLE_PDATA_TypeNamespace "TyNsXml"

class OperationStatus
{

public:
    virtual ~OperationStatus() {}
    virtual bool isOk() = 0;
    virtual QString message() = 0;
    virtual QVariant result() = 0;
};

class DataInterface
{
public:
    virtual ~DataInterface() {}
    virtual OperationStatus* insertGenericData(GenericPersistentData *model) = 0;
    virtual OperationStatus* updateGenericData(GenericPersistentData *model) = 0;
    virtual OperationStatus* deleteGenericData(GenericPersistentData *sessionModel) = 0;
    virtual OperationStatus* readAllGenericData(const QString &type, QList<GenericPersistentData*> &resultList) = 0;
    virtual OperationStatus* readGenericData(const QString &type, const int id, QList<GenericPersistentData*> &resultList) = 0;

    virtual GenericPersistentData *newPersistentDatum(const QString &type) = 0;

};


#endif // DATAINTERFACE_H
