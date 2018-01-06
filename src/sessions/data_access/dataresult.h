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


#ifndef DATARESULT_H
#define DATARESULT_H

#include <QObject>
#include <QString>
#include "../QXmlEditSessions_global.h"

#include <data/DataInterface.h>

//TODO: this class will replace SessionOperationStatus
class QXMLEDITSESSIONSSHARED_EXPORT DataResult : public OperationStatus, QObject
{
    bool _ok;
    QString _message;
    QVariant _result;

public:
    DataResult();
    virtual ~DataResult();

    bool isOk();
    void setOk(const bool value);
    QString message();
    void setMessage(const QString& value);
    void setError(const QString& value);
    virtual QVariant result();
    void setResult(QVariant &value);

};

#endif // ATTRIBUTEFILTERDATAACCESS_H
