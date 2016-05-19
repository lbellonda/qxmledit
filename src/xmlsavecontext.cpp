/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2016 by Luca Bellonda and individual contributors  *
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

#include "xmlsavecontext.h"

XMLSaveContext::XMLSaveContext()
{
    _isDoIndent = false ;
    _indentation = 0;
    _isSortAttributesAlpha = false ;
    _isDTDWritten = false ;
    _attributesMaxColumns = 0 ;
    _isAttributesColumns = false ;
    _level = 0;
    _currentAttrPos = 0;
    _baseAttrPos = 0 ;
    _attrIndex = 0 ;
    _bytesPerChar = 1 ;
}

XMLSaveContext::~XMLSaveContext()
{
}

void XMLSaveContext::incLevel()
{
    _level ++ ;
}

void XMLSaveContext::decLevel()
{
    _level -- ;
}

int XMLSaveContext::level()
{
    return _level ;
}

int XMLSaveContext::indentBase(const QString &tag)
{
    int result = level() * indentation() + tag.length() + 2;
    return result ;
}

void XMLSaveContext::setCodec(QTextCodec * theCodec)
{
    QTextEncoder *encoder = theCodec->makeEncoder();
    /*QByteArray discard = */encoder->fromUnicode(" ");
    _spaceBytes = encoder->fromUnicode(" ");
    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite | QIODevice::Text);
    buffer.write("\n");
    buffer.close();
    QByteArray data = buffer.data();
    QString terminator(data);
    _crBytes = encoder->fromUnicode(terminator);
    _bytesPerChar = _spaceBytes.length();
    delete encoder;
}

bool XMLSaveContext::isDoIndent() const
{
    return _isDoIndent;
}

void XMLSaveContext::setDoIndent(bool doIndent)
{
    _isDoIndent = doIndent;
}

int XMLSaveContext::indentation() const
{
    return _indentation;
}

void XMLSaveContext::setIndentation(int indentation)
{
    _indentation = indentation;
}

bool XMLSaveContext::isSortAttributesAlpha() const
{
    return _isSortAttributesAlpha;
}

void XMLSaveContext::setIsSortAttributesAlpha(bool isSortAttributesAlpha)
{
    _isSortAttributesAlpha = isSortAttributesAlpha;
}

bool XMLSaveContext::isDTDWritten() const
{
    return _isDTDWritten;
}

void XMLSaveContext::setIsDTDWritten(bool isDTDWritten)
{
    _isDTDWritten = isDTDWritten;
}

int XMLSaveContext::attributesMaxColumns() const
{
    return _attributesMaxColumns;
}

void XMLSaveContext::setAttributesMaxColumns(int attributesMaxColumns)
{
    _attributesMaxColumns = attributesMaxColumns;
}

bool XMLSaveContext::isAttributesColumns() const
{
    return _isAttributesColumns;
}

void XMLSaveContext::setIsAttributesColumns(bool isAttributesColumns)
{
    _isAttributesColumns = isAttributesColumns;
}


void XMLSaveContext::startElement(QXmlStreamWriter &writer)
{
    _currentAttrPos = 0;
    _baseAttrPos = writer.device()->pos();
    _attrIndex = 0;
}

void XMLSaveContext::incAttributePos(QXmlStreamWriter &writer, const int indentBase)
{
    if((_indentation > 0) && isAttributesColumns() && (_attrIndex > 0)) {
        if(_currentAttrPos >= attributesMaxColumns()) {
            writer.device()->write(_crBytes);
            // WARNING: PEEKING INTO SOURCE: x-1, the last space is added by writer code.
            for(int i = 0 ; i < (indentBase - 1) ; i ++) {
                writer.device()->write(_spaceBytes);
            }
            _currentAttrPos = 0 ;
            _baseAttrPos = writer.device()->pos();
        }
    }
    _attrIndex++;
}

void XMLSaveContext::afterAttributePos(QXmlStreamWriter &writer)
{
    if((_indentation > 0) && isAttributesColumns()) {
        qint64 nowPos = writer.device()->pos();
        // using space len as average char width
        int diff1 = (nowPos - _baseAttrPos);
        int diffDiv = diff1 / _bytesPerChar ;
        if((0 == diffDiv) && (diff1 > 0)) {
            diffDiv = 1 ;
        }
        _currentAttrPos = diffDiv ;
    }
}

void XMLSaveContext::clearNamespaceDeclarations()
{
    _namespacesToInsert.clear();
}

QHash<QString, QString> XMLSaveContext::namespaceDeclarations()
{
    return _namespacesToInsert ;
}

QHash<QString, QString> &XMLSaveContext::namespaceDeclarationsReference()
{
    return _namespacesToInsert ;
}

bool XMLSaveContext::hasNamespaceDeclarations()
{
    return _namespacesToInsert.size() > 0;
}
