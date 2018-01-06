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

#include "xmlprolog.h"
#include "modules/metadata/basecomplexvariable.h"

const QString XmlProlog::PrologVersionAttr("version");
const QString XmlProlog::PrologEncodingAttr("encoding");
const QString XmlProlog::PrologStandaloneAttr("standalone");


XmlProlog::XmlProlog()
{
    _attributeEncoding = NULL ;
}

XmlProlog::~XmlProlog()
{
    reset();
}

void XmlProlog::reset()
{
    _version = "";
    _encoding = "";
    _standalone = "";
    _attributeEncoding = NULL ;
    foreach(PseudoAttribute * a, _attributes) {
        if(NULL != a) {
            delete a;
        }
    }
    _attributes.clear();
}


//------

QString XmlProlog::version() const
{
    return _version;
}

void XmlProlog::setVersion(const QString &version)
{
    _version = version;
}

QString XmlProlog::encoding() const
{
    return _encoding;
}

void XmlProlog::setEncoding(const QString &encoding)
{
    _encoding = encoding;
    if(NULL == _attributeEncoding) {
        _attributeEncoding = new PseudoAttribute();
        _attributeEncoding->setName("encoding");
        _attributeEncoding->setValue(encoding);
        _attributes.append(_attributeEncoding);
    }
    _attributeEncoding->setValue(encoding);
}

QString XmlProlog::standalone() const
{
    return _standalone;
}

void XmlProlog::setStandalone(const QString &standalone)
{
    _standalone = standalone;
}


QList<PseudoAttribute*> XmlProlog::attributes() const
{
    return _attributes;
}

void XmlProlog::addAttribute(PseudoAttribute* input)
{
    QString name = input->name();
    if(name == PrologVersionAttr) {
        _version = input->value() ;
    } else if(name == PrologEncodingAttr) {
        _encoding = input->value() ;
        _attributeEncoding = input;
    } else if(name == PrologStandaloneAttr) {
        _standalone = input->value() ;
    }
    _attributes.append(input);
}

QString XmlProlog::generateData()
{
    QString result;
    // sort the values
    QList<PseudoAttribute *> finalList;
    PseudoAttribute *pVersion = NULL ;
    PseudoAttribute *pEncoding = NULL ;
    PseudoAttribute *pStandalone = NULL ;

    foreach(PseudoAttribute * attribute, _attributes) {
        if(attribute->name() == PrologVersionAttr) {
            pVersion = attribute ;
        } else if(attribute->name() == PrologEncodingAttr) {
            pEncoding = attribute ;
        } else if(attribute->name() == PrologStandaloneAttr) {
            pStandalone = attribute ;
        } else {
            finalList.append(attribute);
        }
    }
    if(NULL != pVersion) {
        result += pVersion->toString();
    }
    if(NULL != pEncoding) {
        result += pEncoding->toString();
    }
    if(NULL != pStandalone) {
        result += pStandalone->toString();
    }
    foreach(PseudoAttribute * attribute, finalList) {
        result += attribute->toString();
    }
    return result;
}
