/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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
