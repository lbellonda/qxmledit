/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2018 by Luca Bellonda and individual contributors  *
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

#include "documenttype.h"
#include "xmlutils.h"
#include "utils.h"

DocumentType::DocumentType()
{
}

DocumentType::~DocumentType()
{
}


QString DocumentType::docType()
{
    return _docType ;
}

void DocumentType::setDocType(const QString& value)
{
    _docType = value ;
}

QString DocumentType::systemId()
{
    return _systemId ;
}

void DocumentType::setSystemId(const QString& value)
{
    _systemId = value ;
}

QString DocumentType::publicId()
{
    return _publicId ;
}

void DocumentType::setPublicId(const QString& value)
{
    _publicId = value ;
}

bool DocumentType::hasDocType()
{
    if(_docType.isEmpty()) {
        return false;
    }
    return true ;
}

QString DocumentType::dtd() const
{
    return _dtd;
}

void DocumentType::setDtd(const QString &dtd)
{
    // W3c specifications state that dtd does not have blanks before or after
    _dtd = dtd.trimmed();
    // Note: change name and public id as well
    _docType = parseDocType();
}

QString DocumentType::parseDocType()
{
    QString src = _dtd.trimmed();
    //doctypedecl ::= '<!DOCTYPE' S Name (S ExternalID)? S? ('[' int
    const QString DOCTYPE = "<!DOCTYPE";
    const int lenSrc = src.length() ;
    int indexOfStart = src.indexOf(DOCTYPE);
    if((indexOfStart >= 0) && (indexOfStart < 10)) {
        indexOfStart += DOCTYPE.length();
        QChar ch = src.at(indexOfStart);
        while(XmlUtils::isS(ch)) {
            indexOfStart++;
            if(indexOfStart >= lenSrc) {
                return "";
            }
            ch = src.at(indexOfStart);
        }
        // here the name
        int startIndex = indexOfStart;
        // look for end
        while(!XmlUtils::isS(ch) && (ch.unicode() != '[') && (ch.unicode() != '>')) {
            indexOfStart++;
            if(indexOfStart >= lenSrc) {
                return "";
            }
            ch = src.at(indexOfStart);
        }
        int endIndexPlusOne = indexOfStart;
        QString newDtd = src.mid(startIndex, endIndexPlusOne - startIndex);
        return newDtd;
    }
    return "";
}

QDomDocument DocumentType::createDocument()
{
    if(hasDocType()) {
        QDomImplementation implementation;
        QDomDocumentType documentType = implementation.createDocumentType(docType(), publicId(), systemId());
        QDomDocument  document(documentType);
        return document;
    } else {
        QDomDocument    document;
        return document;
    }
}
