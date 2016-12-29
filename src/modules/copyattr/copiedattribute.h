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


#ifndef COPIEDATTRIBUTE_H
#define COPIEDATTRIBUTE_H

#include <QList>
#include "libQXmlEdit_global.h"

#include "element.h"

class LIBQXMLEDITSHARED_EXPORT CopyAttributesSession
{
    QList<Attribute*> _attributes;
    // TODO: think about a list of attributes to remove
    QString _name;
public:
    CopyAttributesSession();
    ~CopyAttributesSession();

    QList<Attribute*> &attributes();
    void setAttributes(QList<Attribute*> &attributes);
    QString name();
    void setName(const QString &newName);
    void clear();
    void removeEmptyAttributes();
    QString attributeValue(const QString &name);
};

class LIBQXMLEDITSHARED_EXPORT CopyAttributesManager
{
    static const int StackDepth = 10;
    QList<CopyAttributesSession*> _copies;

public:
    CopyAttributesManager();
    ~CopyAttributesManager();

    bool isEmpty();
    void addSession(CopyAttributesSession* newSession);
    CopyAttributesSession *first();
    QList<CopyAttributesSession*> &sessions();

    void reset();
    void resetExcept(CopyAttributesSession *session);

};


#endif // COPIEDATTRIBUTE_H
