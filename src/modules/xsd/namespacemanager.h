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
#include "libQXmlEdit_global.h"

class NamespaceDef;
class DataInterface;

class LIBQXMLEDITSHARED_EXPORT NamespaceManager
{
    DataInterface *_dataInterface;
    bool _inited;
public:
    NamespaceManager();
    ~NamespaceManager();

    static const QString XSLFONamespace;
    static const QString XSL1Namespace;
    static const QString XQueryLocalFuncNamespace;
    static const QString MavenPom4Namespace;
    static const QString XHTML11Namespace;

    static const QString NoNamespaceSchemaLocationAttributeName;
    static const QString SchemaLocationAttributeName;

    // constants for namespaces
    enum EWellKnownNs {
        GENERIC_NAMESPACE,
        XSI_NAMESPACE,
        XSD_NAMESPACE,
        XSLFO_NAMESPACE,
        XSL1_NAMESPACE,
        XQUERY_LOCALFUNC_NAMESPACE,
    };

    QString namespaceUri(const EWellKnownNs eWellKnownNs);
    NamespaceDef* namespacesForUri(const QString &uri);
    QList<NamespaceDef*> allNamespaces();
    //--
    DataInterface *dataInterface() const;
    void setDataInterface(DataInterface *dataInterface);

private:
    QHash<EWellKnownNs, NamespaceDef*> _namespaces;
    QHash<QString, NamespaceDef*> _uriNamespaces;

    void reset();
    void init();
    void insertItem(const EWellKnownNs wellKnownNs, const QString &theNamespace, const QString &theSchemaLocation, const QString &theDescription, const QString &defaultPrefix);

};


class NamespaceDef
{
public:
    NamespaceDef(const NamespaceManager::EWellKnownNs codeForWellKnown, const QString &theNamespace, const QString &theSchemaLocation, const QString &theDescription, const QString &defaultPrefix);
    ~NamespaceDef();
    QString _namespace;
    QString _description;
    QString _defaultPrefix;
    QString _schemaLocation;
    NamespaceManager::EWellKnownNs _codeForWellKnown;
    QString uri() const;
    void setUri(const QString &value);
    QString description() const;
    void setDescription(const QString &description);
    QString defaultPrefix() const;
    void setDefaultPrefix(const QString &defaultPrefix);
    QString schemaLocation() const;
    void setSchemaLocation(const QString &location);
};

#endif // NAMESPACEMANAGER_H
