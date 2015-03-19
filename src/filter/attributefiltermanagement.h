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


#ifndef ATTRIBUTEFILTERMANAGEMENT_H
#define ATTRIBUTEFILTERMANAGEMENT_H

#include <QObject>
#include "attributefilterdataaccess.h"
#include "attributefilter.h"
#include "sessiondata"
#include "UIDelegate.h"

class AttributeProfileMgmtDialog;

class AttributeFilterManagement : public QObject
{
    Q_OBJECT

    AttributeFilterDataAccess _dataAccess;

public:
    explicit AttributeFilterManagement(QObject *parent = 0);
    virtual ~AttributeFilterManagement();
    //---------
    AttributeFilter *showProfiles(QWidget *parent, UIDelegate *uiDelegate);
    AttributeFilter *handleResult(AttributeProfileMgmtDialog &dialog, const int result);
    //---------
    QList<AttrFilterProfile*> readProfiles(DataResult &result);
    AttrFilterDetail *readDetail(DataResult &result, const int idProfile);
    void deleteProfile(DataResult &result, const int idProfile);
    void saveProfile(DataResult &result, AttrFilterProfile *profile, AttrFilterDetail *detail);

    bool insertFilterAsProfile(const QString &name, AttributeFilter *attrFilter) ;

    //---------
    AttributeFilterDataAccess* dataAccess();
    void setDataAccess(SessionDataInterface* value);

signals:

public slots:

};

#endif // ATTRIBUTEFILTERMANAGEMENT_H
