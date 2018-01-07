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


#ifndef XMLPROLOG_H
#define XMLPROLOG_H

#include <QString>
#include <QList>

#include "libQXmlEdit_global.h"

class PseudoAttribute;

class LIBQXMLEDITSHARED_EXPORT XmlProlog
{
    QString _version;
    QString _encoding;
    QString _standalone;
    QList<PseudoAttribute *> _attributes;
    PseudoAttribute * _attributeEncoding ;

public:
    XmlProlog();
    ~XmlProlog();

    void reset();

    QString version() const;
    void setVersion(const QString &version);
    QString encoding() const;
    void setEncoding(const QString &encoding);
    QString standalone() const;
    void setStandalone(const QString &standalone);
    QList<PseudoAttribute*> attributes() const;
    void addAttribute(PseudoAttribute *input);
    QString generateData();
    //----
    static const QString PrologVersionAttr;
    static const QString PrologEncodingAttr;
    static const QString PrologStandaloneAttr;
};

#endif // XMLPROLOG_H
