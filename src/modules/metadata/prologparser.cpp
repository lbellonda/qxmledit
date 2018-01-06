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

#include "prologparser.h"
#include "modules/metadata/metadataparser.h"
#include "element.h"

const QString PrologVersionAttr("version");
const QString PrologEncodingAttr("encoding");
const QString PrologStandaloneAttr("standalone");


PrologParser::PrologParser()
{
    reset();
}


PrologParser::~PrologParser()
{
}

//------

void PrologParser::reset()
{
    _prolog.reset();
}


bool PrologParser::parse(const QString & strProlog)
{
    reset();
    return parse(strProlog, &_prolog);
}

bool PrologParser::parse(const QString & strProlog, XmlProlog *prolog)
{
    prolog->reset();
    MetadataParser parser;
    MetadataParsedResult attributes;
    if(parser.parseMetadata(strProlog, &attributes, 0)) {
        // scan for attributes
        foreach(PseudoAttribute * attr, attributes.attributes()) {
            prolog->addAttribute(attr);
        }
        attributes.forgetAttributes();
        return true;
    }

    return false;
}

bool PrologParser::parse(Element * element)
{
    if(isProlog(element)) {
        return parse(element->getPIData());
    }
    return false;
}

bool PrologParser::parse(Element * element, XmlProlog *prolog)
{
    if(isProlog(element)) {
        return parse(element->getPIData(), prolog);
    }
    return false;
}

bool PrologParser::isProlog(Element * element)
{
    if(element->getType() == Element::ET_PROCESSING_INSTRUCTION) {
        // in the prolog the compare is case sensitive http://www.w3.org/TR/xml11 -> 2.8 Prolog and Document Type Declaration
        if(element->getPITarget() == "xml") {
            // the prolog should be well formed
            QString data = element->getPIData().toLower().replace(" ", "");
            if(data.contains("version=") || data.contains("encoding=")) {
                return true;
            }
        }
    }
    return false;
}

