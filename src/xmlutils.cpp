/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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

/*!
 * \brief XmlUtils::getNsPrefix
 * \param name
 * \param prefix
 * \return  true if the name is a name space declaration; then set the parameter to
 * the prefix of the given qualified name
 * \note the check is not completly exact: the attribute name must not contain a colon other than declaration
 * see 3 Declaring Namespaces of Namespaces in XML 1.1 W3C
 */
bool XmlUtils::getNsPrefix(const QString &name, QString &prefix)
{
    prefix = "" ;
    if(name == "xmlns") {
        return true;
    }
    QStringList ns = name.split(':');
    if(ns.length() == 2) {
        if(ns.at(0) == "xmlns") {
            prefix = ns.at(1);
            return true;
        }
    }
    return false;
}

/*!
 * \brief XmlUtils::decodeQualifiedName decodes the name into prefix and localname
 * \param name
 * \param prefix
 * \param localName
 */
void XmlUtils::decodeQualifiedName(const QString &name, QString &prefix, QString &localName)
{
    int indexOfColon = name.indexOf(":");
    if(indexOfColon >= 0) {
        prefix = name.left(indexOfColon);
        localName = name.mid(indexOfColon + 1);
    } else {
        prefix = "";
        localName = name;
    }
}

QString XmlUtils::makeQualifiedName(const QString &prefix, const QString &localName)
{
    if(prefix.isEmpty()) {
        return localName;
    }
    return QString("%1:%2").arg(prefix).arg(localName);
}

QString XmlUtils::makeNSDeclaration(const QString &nsPrefix)
{
    QString xmlNs = "xmlns" ;
    if(nsPrefix.isEmpty()) {
        return xmlNs ;
    }
    return QString("%1:%2").arg(xmlNs).arg(nsPrefix);
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

/*!
 * \brief XmlUtils::isS tests for space as stated in XML specifications
 * \param ch
 * \return
 */
bool XmlUtils::isS(const QChar &qch)
{
    ushort ch = qch.unicode();
    if((0x20 == ch)
            || (0x09 == ch)
            || (0x0D == ch)
            || (0x0A == ch)) {
        return true;
    }
    return false;
}


/**
 * @brief XSDDefaultAnnotationEditor::innerContent
 * @param inputString
 * @param startTag
 * @return
 */
QString XmlUtils::innerContent(const QString &inputString)
{
    int indexOfEnd = inputString.indexOf(">");
    if(indexOfEnd > 0) {
        if(inputString.at(indexOfEnd - 1) == '/') {
            return "";
        }
        // else
        int lastOpenPar = inputString.lastIndexOf("<");
        if(lastOpenPar >= 0) {
            return inputString.mid(indexOfEnd + 1, lastOpenPar - indexOfEnd - 1);
        }

    }
    // impossible to be here
    return inputString ;
}

QString XmlUtils::makeNewPrefixForChangePrefixKey(const QString &ns, const QString &prefix)
{
    return QString("%1:%2").arg(prefix).arg(ns);
}

QString XmlUtils::makeNewPrefixForChangePrefix(QHash<QString, QString> &prefixes, QSet<QString> &allPrefixes, const QString &ns, const QString &prefix)
{
    QString key = makeNewPrefixForChangePrefixKey(ns, prefix);
    if(prefixes.contains(key)) {
        return prefixes[key];
    }
    int index = 0;
    while(true) {
        QString newPrefix = prefix + QString::number(index);
        if(!allPrefixes.contains(newPrefix)) {
            allPrefixes.insert(newPrefix);
            prefixes.insert(key, newPrefix);
            return newPrefix;
        }
        index++ ;
    }
}

bool XmlUtils::isNCNAME(const QString &name)
{
    if(Utils::checkXMLName(name)) {
        if(name.indexOf(":") >= 0) {
            return false;
        }
        return true;
    }
    return false;
}

bool XmlUtils::isNMTOKEN(const QString &name)
{
    if(name.isEmpty()) {
        return false;
    }
    //tokens composed of characters, digits, period, colons, hyphens
    const QString val(".:_");
    int len = name.length() ;
    FORINT(i, len) {
        const QChar ch = name.at(i);
        if(val.indexOf(ch) >= 0) {
            continue;
        }
        if(ch.isLetterOrNumber()) {
            continue ;
        }
        return false ;
    }
    return true;
}
