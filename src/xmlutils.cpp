/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
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

#include "xmlutils.h"
#include "xsdeditor/xschema.h"
#include "utils.h"

XmlUtils::XmlUtils()
{
}

XmlUtils::~XmlUtils()
{
}


/**
  checks if the root element namespace and element correspond to thos if XSD
  */

bool XmlUtils::IsXsdValid(const QString &nsUri, const QString &localName)
{
    return XSDSchema::IsXsdValid(nsUri, localName);
}


QString XmlUtils::namespacePrefix(const QString &name)
{
    int indexOf = name.indexOf(":");
    if(indexOf >= 0) {
        return name.mid(indexOf + 1);
    }
    return "";
}

/**
 * @brief XmlUtils::stripNs
 *name without namespace, if any
 * @param name
 * @return
 */
QString XmlUtils::stripNs(const QString &name)
{
    int indexOf = name.indexOf(":");
    if(indexOf >= 0) {
        return name.mid(indexOf + 1);
    }
    return name;
}


bool XmlUtils::hasPrefix(const QString &tag, const QString &nsPrefix)
{
    bool result ;
    if(nsPrefix.isEmpty()) {
        result = !tag.contains(":");
    } else {
        result = tag.startsWith(nsPrefix + ":");
    }
    return result;
}


QString XmlUtils::boolToBoolValue(const bool input)
{
    return input ? "true" : "false";
}

bool XmlUtils::readFromBool(const QString &input, const bool defaultValue)
{
    if(input.isEmpty()) {
        return defaultValue ;
    }
    if("true" == input) {
        return true ;
    }
    return false;
}

QString XmlUtils::intToStringValue(const int value)
{
    return QString("%1").arg(value);
}

int XmlUtils::readFromInt(const QString &inputVal, const int defaultValue)
{
    bool isOk = false;
    int ret = inputVal.toInt(&isOk);
    if(!isOk) {
        return defaultValue;
    }
    return ret;
}

bool XmlUtils::isDeclaringNS(const QString &attributeName)
{
    if(attributeName.startsWith("xmlns:") || (attributeName == "xmlns")) {
        return true;
    }
    return false;
}

bool XmlUtils::isDataAttribute(const QString &attributeName)
{
    if(isDeclaringNS(attributeName) || (attributeName == "xml") || attributeName.startsWith("xml:")) {
        return false;
    }
    return true;
}

bool XmlUtils::getNsPrefix(const QString &name, QString &prefix)
{
    Utils::TODO_THIS_RELEASE("questa routine non e esatta, usa indexof");
    QStringList ns = name.split(':');
    if(ns.length() < 2) {
        prefix = "" ;
        if(name == "xmlns") {
            return true;
        }
    } else {
        prefix = ns.at(1);
        if(ns.at(0) == "xmlns") {
            return true;
        }
    }
    return false;
}

void XmlUtils::decodeQualifiedName(const QString &name, QString &prefix, QString &localName)
{
    Utils::TODO_THIS_RELEASE("non va bene dividere in due, usa indexof");
    QStringList ns = name.split(':');
    if(ns.length() < 2) {
        prefix = "" ;
        localName = name;
    } else {
        prefix = ns.at(0);
        localName = ns.at(1);
    }
}

QString XmlUtils::makeQualifiedName(const QString &prefix, const QString &localName)
{
    if(prefix.isEmpty()) {
        return localName;
    }
    return QString("%1:%2").arg(prefix).arg(localName);
}

QString XmlUtils::makeNSDeclaration(const QString &ns)
{
    QString xmlNs = "xmlns" ;
    if(ns.isEmpty()) {
        return xmlNs ;
    }
    return QString("%1:%2").arg(xmlNs).arg(ns);
}

bool XmlUtils::isNamespaceDeclarationForPrefix(const QString &name, const QString &prefix)
{
    QString declaration = "xmlns" ;
    if(!prefix.isEmpty()) {
        declaration = QString("xmlns:%1").arg(prefix);
    }
    if(name == declaration) {
        return true ;
    }
    return false;
}

