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


#ifndef NAMESPACEMANAGER_H
#define NAMESPACEMANAGER_H

#include <QString>
#include <QHash>
#include "libQXmlEdit_global.h"

class NamespaceDef;
class DataInterface;
class QWidget;
class Element;
class QTreeWidget;
class Regola ;
class NamespaceHandlerForEdit;
class XIncludeEditorManager;

class SingleHandlerForInsert
{
public:
    QString name ;
    QString description ;
    QString id;
};

class NamespaceHandlerForEdit;

class HandlerForInsert
{
public:
    QString nameSpace;
    QString name;
    QString outputSelectedCode;
    QList<SingleHandlerForInsert*> elements;
    NamespaceHandlerForEdit *handler;
    HandlerForInsert();
    ~HandlerForInsert();
};

class LIBQXMLEDITSHARED_EXPORT NamespaceHandlerForEdit
{
public:
    NamespaceHandlerForEdit();
    virtual ~NamespaceHandlerForEdit();
    virtual bool handleEdit(QWidget *parent, QTreeWidget *tree, Regola *regola, Element *element) = 0 ;
    virtual HandlerForInsert *handlerForInsert(Regola *regola, Element *element, const bool isChild) = 0 ;
    virtual bool handleInsert(QTreeWidget *tree, Regola *regola, Element *element, const bool isChild, const QString &itemCode) = 0 ;
};

class LIBQXMLEDITSHARED_EXPORT NamespaceEditorInsertChoiceProvider
{
public:
    NamespaceEditorInsertChoiceProvider();
    virtual ~NamespaceEditorInsertChoiceProvider();

    virtual HandlerForInsert *handleInsertElementForSpecialized(QWidget *parent, QList<HandlerForInsert*> *handlers) = 0;
};

class LIBQXMLEDITSHARED_EXPORT NamespaceManager : public NamespaceEditorInsertChoiceProvider
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
    static const QString XIncludeNamespace;
    static const QString SCXMLNamespace;

    static const QString NoNamespaceSchemaLocationAttributeName;
    static const QString SchemaLocationAttributeName;
    //
    static const QString XIncludePrefix;
    static const QString SCXLMPrefix;

    // constants for namespaces
    enum EWellKnownNs {
        GENERIC_NAMESPACE,
        XSI_NAMESPACE,
        XSD_NAMESPACE,
        XSLFO_NAMESPACE,
        XSL1_NAMESPACE,
        XQUERY_LOCALFUNC_NAMESPACE,
        MAVEN_NAMESPACE,
        XINCLUDE_NAMESPACE,
        SCXML_NAMESPACE
    };

    QString namespaceUri(const EWellKnownNs eWellKnownNs);
    NamespaceDef* namespacesForUri(const QString &uri);
    QList<NamespaceDef*> allNamespaces();
    //--
    DataInterface *dataInterface() const;
    void setDataInterface(DataInterface *dataInterface);

    bool editElement(QWidget *parent, QTreeWidget *tree, Regola *regola, Element *element);
    bool insertElement(QWidget *parent, QTreeWidget *tree, Regola *regola, Element *element, const bool isChildOrSibling);
    void init();

    HandlerForInsert *handleInsertElementForSpecialized(QWidget *parent, QList<HandlerForInsert*> *handlers);
    void setProviderForInsert(NamespaceEditorInsertChoiceProvider *newProvider);
    XIncludeEditorManager *xIncludeEditorManager();

private:
    QHash<EWellKnownNs, NamespaceDef*> _namespaces;
    QHash<QString, NamespaceDef*> _uriNamespaces;
    QHash<QString, NamespaceHandlerForEdit *> _editHandlers;
    NamespaceEditorInsertChoiceProvider *_insertEditorProvider;

    void reset();
    void insertItem(const EWellKnownNs wellKnownNs, const QString &theNamespace, const QString &theSchemaLocation,
                    const QString &theDescription, const QString &defaultPrefix,
                    NamespaceHandlerForEdit *editHandler = NULL);

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
