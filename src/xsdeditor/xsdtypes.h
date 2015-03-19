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

#ifndef XSDTYPES_H
#define XSDTYPES_H

enum EXType {
    MIN_XSD_TYPE = 0,
    xsd_NONE = 0,
    xsd_anySimpleType,
    xsd_anyType,
    xsd_anyURI,
    xsd_base64Binary,
    xsd_boolean,
    xsd_byte,
    xsd_date,
    xsd_dateTime,
    xsd_decimal,
    xsd_double,
    xsd_duration,
    xsd_float,
    xsd_gDay,
    xsd_gMonth,
    xsd_gMonthDay,
    xsd_gYear,
    xsd_gYearMonth,
    xsd_hexBinary,
    xsd_ID,
    xsd_IDREF,
    xsd_IDREFS,
    xsd_int,
    xsd_integer,
    xsd_language,
    xsd_long,
    xsd_Name,
    xsd_NCName,
    xsd_negativeInteger,
    xsd_NMTOKEN,
    xsd_NMTOKENS,
    xsd_nonNegativeInteger,
    xsd_nonPositiveInteger,
    xsd_normalizedString,
    xsd_positiveInteger,
    xsd_QName,
    xsd_short,
    xsd_string,
    xsd_time,
    xsd_token,
    xsd_unsignedByte,
    xsd_unsignedInt,
    xsd_unsignedLong,
    xsd_unsignedShort,
    MAX_XSD_TYPE
};

#endif // XSDTYPES_H
