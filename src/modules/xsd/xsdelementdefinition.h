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


#ifndef XSDELEMENTDEFINITION_H
#define XSDELEMENTDEFINITION_H

#include <QString>
#include <QList>
#include <QStringList>

class XSDAttributeDefinition;

class XSDElementDefinition
{
    /** @brief this is the name of the element eg: element, type, sequence, ...*/
    QString _name;
    /** @brief this is the namespace for the element, if not empty. The namespace is added to the root elment if missing, using a progressive */
    QString _namespaceId;
    /** @brief the prefix to use for namespace, is the namespace does not exist and the prefix is not yet used, else a numeric key will be created */
    QString _namespacePrefix;
    /** @brief the base type if not empty */
    QString _baseType;
    /** @brief the namespace of the type */
    QString _baseTypeNamespace;
    /** @brief the reference, if not empty, evaluated after the type */
    QString _reference;
    /** @brief the name of the element */
    QString _elementName ;
    /** @brief if the user should input the name or not */
    //bool _userInputForElementName ;
    QStringList _admissibleParents;

    /** @brief the attributes of the element, if any */
    QList<XSDAttributeDefinition*> _attributes;
    /** @brief the children of the element, if any */
    QList<XSDElementDefinition*> _children;
public:
    XSDElementDefinition();
    ~XSDElementDefinition();
};

class XSDAttributeDefinition
{
    /** @brief this is the name of the attribute */
    QString _name;
    /** @brief the default value for the attribute */
    QString _defaultValue;
    /** @brief if the user should modify the value or not */
    bool _userInputValue;
    /** @brief if the input is a multiple choice, the values are listed here */
    QStringList _valueOptions;
    /** @brief the base type if not empty */
    QString _baseType;
    /** @brief the namespace of the type */
    QString _baseTypeNamespace;
    /** @brief the reference, if not empty, evaluated after the type */
    QString _reference;

    /** @brief the enums for attribute restriction of the type string */
    QString _enums;
public:
    XSDAttributeDefinition();
    ~XSDAttributeDefinition();
};
#endif // XSDELEMENTDEFINITION_H
