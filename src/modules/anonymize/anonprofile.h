/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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


#ifndef ANONPROFILE_H
#define ANONPROFILE_H

#include "libQXmlEdit_global.h"
#include "anonexception.h"
#include <QHash>
#include <QDomDocument>

class AnonymizeParameters;

class LIBQXMLEDITSHARED_EXPORT AnonProfile
{
    QList<AnonException *> _exceptions;
    bool _useNamespace;
    QHash<QString, AnonException *> _exceptionsByPath;
    AnonymizeParameters *_params;

    bool scanExceptionsFromDom(const QDomElement &element, QList<AnonException*> &eList);
public:
    AnonProfile();
    virtual ~AnonProfile();

    void addException(AnonException *newExc);
    void reset();
    AnonException *getExceptionByPath(const QString &thePath);
    AnonException *getExceptionByPathWithNamespace(const QString &thePath);
    AnonProfile *clone();
    bool saveToDom(QDomDocument &document);
    bool readFromDom(const QDomElement &element);
    bool readFromSerializedXmlString(const QString &string);
    QString toXMLSerializedString();
    void removeException(AnonException *exc);
    QList<AnonException *> exceptions();
    bool useNamespace() const;
    void setUseNamespace(bool useNamespace);
    AnonymizeParameters *params() const;
    void setParams(AnonymizeParameters *params);
};

#endif // ANONPROFILE_H
