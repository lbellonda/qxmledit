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


#ifndef DOCUMENTTYPE_H
#define DOCUMENTTYPE_H

#include <QDomDocument>

/* TODO
class HeaderEntity {
    QString name;
    QString notationName;
    QString publicId;
    QString systemId;

public:
    HeaderEntity();
    ~HeaderEntity();
};

class HeaderNotation {
public:
    HeaderNotation();
    ~HeaderNotation();
};
*/
/**
  \brief XML document type
  */
class DocumentType
{
    QString _docType;
    QString _systemId;
    QString _publicId;
    QString _dtd;

    QString parseDocType();

public:
    DocumentType();
    ~DocumentType();

    QString docType();
    void setDocType(const QString& value);
    QString systemId();
    void setSystemId(const QString& value);
    QString publicId();
    void setPublicId(const QString& value);
    QString dtd() const;
    void setDtd(const QString &dtd);

    bool hasDocType();

    QDomDocument createDocument();
};

#endif // DOCUMENTTYPE_H
