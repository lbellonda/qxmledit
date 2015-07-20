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


#ifndef NAMESPACEMANAGER_H
#define NAMESPACEMANAGER_H

#include <QString>
#include <QHash>

class NamespaceDef;

class NamespaceManager
{
public:
    NamespaceManager();
    ~NamespaceManager();

    // constants for namespaces
    enum EWellKnownNs {
        XSI_NAMESPACE,
        XSD_NAMESPACE,
    };

    QString namespaceUri(const EWellKnownNs eWellKnownNs);
    //--
private:
    QHash<EWellKnownNs, NamespaceDef*> _namespaces;

    void reset();
    void insertItem(const EWellKnownNs wellKnownNs, const QString &theNamespace, const QString &theDescription);

};


class NamespaceDef
{
public:
    NamespaceDef(const NamespaceManager::EWellKnownNs codeForWellKnown, const QString &theNamespace, const QString &theDescription);
    ~NamespaceDef();
    QString _namespace;
    QString _description;
    NamespaceManager::EWellKnownNs _codeForWellKnown;
};

#endif // NAMESPACEMANAGER_H
