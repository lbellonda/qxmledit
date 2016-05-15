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


#ifndef XMLUTILS_H
#define XMLUTILS_H

#include <QString>
class AnonContext;

class XmlUtils
{
private:
    XmlUtils();
    ~XmlUtils();
public:
    static bool IsXsdValid(const QString &nsUri, const QString &localName);
    static QString namespacePrefix(const QString &name);
    static QString stripNs(const QString &name);
    static bool hasPrefix(const QString &tag, const QString &nsPrefix);

    static int readFromInt(const QString &inputVal, const int defaultValue);
    static bool readFromBool(const QString &inputVal, const bool defaultValue);
    static QString boolToBoolValue(const bool value);
    static QString intToStringValue(const int value);
    //
    static bool isDeclaringNS(const QString &attributeName);
    static bool isDataAttribute(const QString &attributeName);
    static bool getNsPrefix(const QString &name, QString &prefix);
    static void decodeQualifiedName(const QString &name, QString &prefix, QString &localName);
    static QString makeQualifiedName(const QString &prefix, const QString &localName);
    static QString makeNSDeclaration(const QString &ns);

    static bool isNamespaceDeclarationForPrefix(const QString &name, const QString &prefix);

    //--
    static bool isS(const QChar &qch);
    //--
    static QString innerContent(const QString &inputString);

};

#endif // XMLUTILS_H
