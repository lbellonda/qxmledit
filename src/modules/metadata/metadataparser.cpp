/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013-2018 by Luca Bellonda and individual contributors  *
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


#include "metadataparser.h"
#include "utils.h"

//-------------------

MetadataParsedResult::MetadataParsedResult()
{
}

MetadataParsedResult::~MetadataParsedResult()
{
    clean();
}

QString MetadataParsedResult::dumpAttributes()
{
    QString result = "\n Dumping attributes:\n" ;
    int index = 0;
    foreach(PseudoAttribute * attr, _attributes) {
        result += QString(" at:%1 '%2' -> '%3'\n").arg(index).arg(attr->name()).arg(attr->value());
        index++ ;
    }
    result += "---\n" ;
    return result;
}

void MetadataParsedResult::addAttribute(const QString &name, const QString &value, const int rowPos)
{
    PseudoAttribute *attr = new PseudoAttribute();
    attr->setName(name);
    attr->setExists(true);
    attr->setUsed(true);
    attr->setValue(value);
    attr->setPosition(_attributes.size());
    attr->setRow(rowPos);
    _attributes.append(attr);
}

void MetadataParsedResult::forgetAttributes()
{
    _attributes.clear();
}

void MetadataParsedResult::clean()
{
    foreach(PseudoAttribute * a, _attributes) {
        delete a;
    }
    _attributes.clear();
}

PseudoAttribute *MetadataParsedResult::find(const QString &name)
{
    foreach(PseudoAttribute * a, _attributes) {
        if(a->name() == name) {
            return a;
        }
    }
    return NULL ;
}

QList<PseudoAttribute*> &MetadataParsedResult::attributes()
{
    return _attributes ;
}

//-------------------

MetadataParser::MetadataParser()
{
}

MetadataParser::~MetadataParser()
{
}


bool MetadataParser::parseMetadata(const QString &inputData, MetadataParsedResult *attributes, const int rowPos)
{
    MetadataParserInfo info;
    info.setInput(inputData);
    info.setResult(attributes);
    info.setRow(rowPos);
    info.parse();
    if(info.isError()) {
        attributes->clean();
        return false;
    }
    return true;
}

//-------------------

MetadataParserInfo::MetadataParserInfo()
{
    initParse();
}

MetadataParserInfo::~MetadataParserInfo()
{
}

bool MetadataParserInfo::isError()
{
    return _isError ;
}

void MetadataParserInfo::setInput(const QString &inputData)
{
    _inputString = inputData ;
    _index = 0;
    _lenInput = _inputString.length();
}

void MetadataParserInfo::setResult(MetadataParsedResult *newAttributes)
{
    _attributes = newAttributes ;
}

int MetadataParserInfo::row() const
{
    return _row;
}

void MetadataParserInfo::setRow(const int row)
{
    _row = row;
}


void MetadataParserInfo::parse()
{
    initParse();
    while(!isEof() && !_isError) {
        parseAttribute();
    }
}

void MetadataParserInfo::initParse()
{
    _isError = 0;
    _index = -1;
    _lenInput = _inputString.length();
    _ch = '\0';
}

bool MetadataParserInfo::isEof()
{
    if(_index < _lenInput) {
        return false;
    }
    return true;
}

void MetadataParserInfo::raiseError()
{
    _isError = true ;
}

void MetadataParserInfo::nextChar()
{
    if(isEof()) {
        return ;
    }
    _index ++ ;
    if(isEof()) {
        return ;
    }
    _ch = _inputString.at(_index);
}

void MetadataParserInfo::parseAttribute()
{
    if(_isError) {
        return;
    }
    nextChar();
    if(isEof()) {
        return ;
    }
    while(isBlank()) {
        nextChar();
    }
    if(isEof()) {
        return ;
    }
    QString name ;
    if(canStartName()) {
        int len = 1;
        int nameStartIndex = _index ;
        nextChar();
        while(canFollowName()) {
            nextChar();
            len++;
        }
        name = _inputString.mid(nameStartIndex, len);
        if(0 == name.length()) {  // how can be it possible here?
            raiseError();
        }
        while(isBlank()) {
            nextChar();
        }
        if(isEqual()) {
            nextChar();
        } else {
            raiseError();
        }
        while(isBlank()) {
            nextChar();
        }
        QString value ;
        if(isQuote()) {
            value = eatDelimitedString('\'');
        } else if(isDoubleQuote()) {
            value = eatDelimitedString('"');
        } else {
            raiseError();
        }
        if(!_isError) {
            _attributes->addAttribute(name, value, row());
        }
    } else {
        raiseError();
    }
}


bool MetadataParserInfo::isBlank()
{
    if(isEof() || _isError) {
        return false;
    }
    switch(_ch.unicode()) {
    case 0x20:
    case 0x9:
    case 0xD:
    case 0xA:
        return true;
    }
    return false;
}

bool MetadataParserInfo::canStartName()
{
    if(isEof() || _isError) {
        return false;
    }
    return Utils::canStartXmlName(_ch);
}

bool MetadataParserInfo::canFollowName()
{
    if(isEof() || _isError) {
        return false;
    }
    return Utils::canFollowXmlStartName(_ch);
}

bool MetadataParserInfo::isQuote()
{
    if(isEof() || _isError) {
        return false;
    }
    if(_ch == '\'') {
        return true;
    }
    return false;
}


bool MetadataParserInfo::isDoubleQuote()
{
    if(isEof() || _isError) {
        return false;
    }
    if(_ch == '\"') {
        return true;
    }
    return false;
}

bool MetadataParserInfo::isEqual()
{
    if(isEof() || _isError) {
        return false;
    }
    if(_ch == '=') {
        return true;
    }
    return false;
}

QString MetadataParserInfo::eatDelimitedString(const char delimiter)
{
    if(isEof() || _isError) {
        return "";
    }
    // start of the string
    int startIndex = _index + 1;
    int len = 0 ;
    for(;;) {
        nextChar();
        if(isEof()) {
            raiseError() ;
        }
        if(isEof() || _isError) {
            return "";
        }
        len++;
        if(_ch == delimiter) {
            // end of string, make the result;
            if(len <= 1) {
                return "";
            }
            QString result = _inputString.mid(startIndex, len - 1);
            return unescape(result);
        }
    }
    raiseError();
    return "";
}

QString MetadataParserInfo::unescape(const QString &str)
{
    QString result = str;
    result.replace("&lt;", "<");
    result.replace("&gt;", ">");
    result.replace("&quot;", "\"");
    result.replace("&apos;", "'");
    result.replace("&amp;", "&");//this last
    return result;
}

QString MetadataParserInfo::escape(const QString &str)
{
    QString result = str;
    result.replace("&", "&amp;"); // this first
    result.replace("<", "&lt;");
    result.replace(">", "&gt;");
    result.replace("\"", "&quot;");
    result.replace("'", "&apos;");
    return result;
}
