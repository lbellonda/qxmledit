/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2019 by Luca Bellonda and individual contributors       *
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

#ifndef EXTRACTIONSCRIPTELEMENT_H
#define EXTRACTIONSCRIPTELEMENT_H

#include "xmlEdit.h"
#include <QXmlStreamAttribute>

class ExtractionScriptAttribute
{
    static int instances;
public:
    QString nameSpace;
    QString name;
    QString value;
    ExtractionScriptAttribute();
    ~ExtractionScriptAttribute();
    bool compareTo(ExtractionScriptAttribute *other);
};

class ExtractionScriptElementEvent : public QObject
{
    Q_OBJECT
protected:
    bool _isModified;
    bool _useNamespaces ;
    bool _isError;
    QString _errorMessage;
    QString _elementName;
    QString _localName;
    QString _nameSpace;

    QList<ExtractionScriptAttribute*> _attributesSorted;
    QHash<QString, ExtractionScriptAttribute*> _attributesByNamespaceAndName;

    bool iCheckInternalState();
    void checkInternalState();
    static QString makeAttributeKey(const QString &attributeNameSpace, const QString &attributeName);
    static QString makeAttributeKey(ExtractionScriptAttribute *attribute);
    ExtractionScriptAttribute *findAttribute(const QString &attributeNameSpace, const QString &attributeName);
    bool addAttribute(ExtractionScriptAttribute *newAttribute);
    void triggerError(const QString &message);
    void iSetAttributeNameByIndex(const int index, const QString &namespaceId, const QString &attributeName);
    void iSetAttributeValueByName(const QString &nameSpace, const QString &attributeName, const QString &attributeValue);
    QString iAttributeValueByNameNS(const QString &namespaceId, const QString &attributeName);
    int iRemoveAttributeByName(const QString &nameSpace, const QString &attributeName);
public:
    explicit ExtractionScriptElementEvent(QObject *parent = NULL);
    virtual ~ExtractionScriptElementEvent();

    void dump();

    QList<ExtractionScriptAttribute*> attributes();

    Q_PROPERTY(bool isError READ isError)
    Q_PROPERTY(QString errorMessage READ errorMessage)
    Q_PROPERTY(bool useNamespaces READ useNamespaces)
    Q_PROPERTY(QString elementName READ elementName)
    Q_PROPERTY(QString localName READ localName)
    Q_PROPERTY(QString nameSpace READ nameSpace WRITE setNameSpace)
    Q_PROPERTY(int attributesCount READ attributesCount)

    QString elementName() const;
    void setElementName(const QString &value);
    QString localName() const;
    void setLocalName(const QString &value);
    QString nameSpace() const;
    void setNameSpace(const QString &value);
    int attributesCount();
    bool useNamespaces() const;
    void setUseNamespaces(const bool value);
    bool isError() const;
    void setError(const bool value);
    QString errorMessage() const;
    void setErrorMessage(const QString &errorMessage);

    void resetError() ;

    void setAttribute(QXmlStreamAttribute* attribute);

    bool compareTo(ExtractionScriptElementEvent &other);

    bool isModified() const;
    bool resetModifed();

    Q_INVOKABLE int removeAttributeByName(const QString &name);
    Q_INVOKABLE int removeAttributeByNameNS(const QString &namespaceId, const QString &name);
    Q_INVOKABLE int removeAttributeByIndex(const int index);
    Q_INVOKABLE void sortAttributesByName();
    Q_INVOKABLE void sortAttributesByNamespaceAndName();
    //---
    Q_INVOKABLE QString attributeValueByName(const QString &attributeName);
    Q_INVOKABLE QString attributeValueByNameNS(const QString &namespaceId, const QString &name);
    Q_INVOKABLE QString attributeValueByIndex(const int index);
    //---
    Q_INVOKABLE QString attributeNameByIndex(const int index);
    Q_INVOKABLE QString attributeNameSpaceByIndex(const int index);
    //--
    Q_INVOKABLE void setAttributeValueByName(const QString &attributeName, const QString &attributeValue);
    Q_INVOKABLE void setAttributeValueByNameNS(const QString &namespaceId, const QString &attributeName, const QString &attributeValue);
    Q_INVOKABLE void setAttributeValueByIndex(const int index, const QString &attributeValue);
    //--
    Q_INVOKABLE void setAttributeNameByIndex(const int index, const QString &attributeName);
    Q_INVOKABLE void setAttributeNameByIndexNS(const int index, const QString &namespaceId, const QString &attributeName);
    //

signals:

public slots:
};

#endif // EXTRACTIONSCRIPTELEMENT_H
