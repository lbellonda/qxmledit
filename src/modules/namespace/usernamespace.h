/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2016 by Luca Bellonda and individual contributors  *
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


#ifndef USERNAMESPACE_H
#define USERNAMESPACE_H

#include "xmlEdit.h"
#include "sessions/data_access/model/genericpersistentdbdata.h"

class UserNamespace
{
    //------------
    QString _uri;
    QString _schemaLocation;
    QStringList _prefixes;
    QString _preferredPrefix;
    GenericPersistentDBData *_data;

    void reset();
public:
    UserNamespace();
    ~UserNamespace();

    bool saveToDom(QDomDocument &document);
    bool readFromDom(const QDomElement &element);
    bool readFromSerializedXmlString(const QString &string);
    QString toXMLSerializedString();

    //-----
    QString uri() const;
    void setUri(const QString &uri);
    QString description() const;
    void setDescription(const QString &description);
    QStringList prefixes() const;
    void setPrefixes(const QStringList &prefixes);
    QString preferredPrefix() const;
    void setPreferredPrefix(const QString &preferredPrefix);
    QString name() const;
    void setName(const QString &name);
    QString schemaLocation() const;
    void setSchemaLocation(const QString &schemaLocation);
    //----

    GenericPersistentDBData *data() const;
    bool setData(GenericPersistentDBData *data);
    bool syncToData();
};

#endif // USERNAMESPACE_H
