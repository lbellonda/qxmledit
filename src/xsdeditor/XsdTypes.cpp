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

#include <xsdeditor/xschema.h>
#include "utils.h"

void XSDSchema::addBaseType(const EXType typeIndex)
{
    QString name = baseTypeName(typeIndex);
    if(name.isEmpty()) {
        Utils::error(tr("XSD type not recognized:'%1'").arg(typeIndex));
    } else {
        XSchemaElement *element = new XSchemaElement(this, this);
        element->setName(name);
        element->setIsPredefined(true);
        _baseTypes.insert(name, element);
    }
}


#define EVAL(xType) case xsd_##xType: return QString(STRINGIZE(xType));

QString XSDSchema::baseTypeName(const EXType typeIndex)
{
    switch(typeIndex) {
    default:
        return "";
        EVAL(NONE)
        EVAL(anySimpleType)
        EVAL(anyType)
        EVAL(anyURI)
        EVAL(base64Binary)
        EVAL(boolean)
        EVAL(byte)
        EVAL(date)
        EVAL(dateTime)
        EVAL(decimal)
        EVAL(double)
        EVAL(duration)
        EVAL(float)
        EVAL(gDay)
        EVAL(gMonth)
        EVAL(gMonthDay)
        EVAL(gYear)
        EVAL(gYearMonth)
        EVAL(hexBinary)
        EVAL(ID)
        EVAL(IDREF)
        EVAL(IDREFS)
        EVAL(int)
        EVAL(integer)
        EVAL(language)
        EVAL(long)
        EVAL(Name)
        EVAL(NCName)
        EVAL(negativeInteger)
        EVAL(NMTOKEN)
        EVAL(NMTOKENS)
        EVAL(nonNegativeInteger)
        EVAL(nonPositiveInteger)
        EVAL(normalizedString)
        EVAL(positiveInteger)
        EVAL(QName)
        EVAL(short)
        EVAL(string)
        EVAL(time)
        EVAL(token)
        EVAL(unsignedByte)
        EVAL(unsignedInt)
        EVAL(unsignedLong)
        EVAL(unsignedShort)
    }
}

XSchemaElement *XSDSchema::baseType(const QString &nameWOPrefix)
{
    if(_baseTypes.contains(nameWOPrefix)) {
        return _baseTypes[nameWOPrefix];
    }
    return NULL ;
}
