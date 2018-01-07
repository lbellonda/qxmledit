/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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


#ifndef XSDOPERATIONPARAMETERS_H
#define XSDOPERATIONPARAMETERS_H

#include "xmlEdit.h"
#include "libQXmlEdit_global.h"

class Element;

class LIBQXMLEDITSHARED_EXPORT XSDOperationParameters
{
public:

    static const QString TokenType;
    static const QString TokenName;

    enum EObjectType {
        EOI_ELEMENT,
        EOI_TYPE,
        EOI_ATTRIBUTE
    };

    enum EXSDTypeTo {
        EXT_ANONYMOUS_TYPE,
        EXT_REFERENCE_TYPE
    };

    enum EXSDTypeSpec {
        TS_SIMPLE,
        TS_COMPLEX
    };

    enum EXSDTypeSpecSub {
        TSS_NONE,
        TSS_RESTRICTION,
        TSS_EXTENSION,
        TSS_ALL,
        TSS_GROUP,
        TSS_CHOICE,
        TSS_SEQUENCE,
        TSS_LIST,   // simple type
        TSS_UNION   // simple type
    };

    enum EXSDTypeContent {
        TSC_NONE,
        TSC_SIMPLE,
        TSC_COMPLEX,
    };

private:
    QString _xsdNamespacePrefix;
    QString _typeName;
    EXSDTypeTo _typeTo;
    EXSDTypeSpec _typeSpec;
    bool _conserveAnnotations;
    EObjectType _objType;
    bool _usePrefix;
    QString _name;
    EXSDTypeSpecSub _subOper;
    EXSDTypeContent _typeContent;
    QSet<QString> _xsdNamespacePrefixes;
    XSDOperationParameters *_parent;
    QHash<QString, QString> _prefixesToNamespaces;


    void init();

    void setupSimpleType(Element *simpleType);
    bool setupComplexTypeSimpleContent(Element *simpleContent);
    bool setupComplexTypeComplexContent(Element *complexContent);
    bool setupComplexTypeGroup(Element *input);
    bool setupComplexTypeChoice(Element *input);
    bool setupComplexTypeSeq(Element *input);
    bool setupComplexTypeAll(Element *input);
    void setupComplexType(Element *complexType);
    void extractName(Element *inputElement);
    void decodeElement(Element *inputElement);
    void decodeAttribute(Element *inputElement);
    void decodeComplexType(Element *inputElement);
    void decodeSimpleType(Element *inputElement);

public:
    XSDOperationParameters();
    XSDOperationParameters(XSDOperationParameters *theParent);
    ~XSDOperationParameters();
    //---
    QString xsdNamespacePrefix() const;
    void setXsdNamespacePrefix(const QString &xsdNamespacePrefix);
    void setElementDeclarations(Element *element, const bool upToRoot);
    QString getNSForPrefix(const QString &prefix);
    QString typeName() const;
    void setTypeName(const QString &typeName);
    EXSDTypeTo typeTo() const;
    void setTypeTo(const EXSDTypeTo &typeTo);
    EXSDTypeSpec typeSpec() const;
    void setTypeSpec(const EXSDTypeSpec &typeSpec);
    bool isConserveAnnotations() const;
    void setConserveAnnotations(bool conserveAnnotations);
    EObjectType objType() const;
    void setObjType(const EObjectType &objType);
    bool usePrefix() const;
    void setUsePrefix(bool usePrefix);
    QString name() const;
    void setName(const QString &name);
    EXSDTypeSpecSub subOper() const;
    void setSubOper(const EXSDTypeSpecSub &subOper);
    EXSDTypeContent typeContent() const;
    void setTypeContent(const EXSDTypeContent &typeContent);
    //----
    QString decodeToken(const QString &code);
    //---
    void copyTo(XSDOperationParameters *dest);
    void setupFromElement(Element *inputElement);
    //---
    QString makeNameForXSDObject(const QString &localName);

    QSet<QString> xsdNamespacePrefixes() const;
    void setXsdNamespacePrefixes(const QSet<QString> &xsdNamespacePrefixes);
};

#endif // XSDOPERATIONPARAMETERS_H
