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

#ifndef XMLSAVECONTEXT_H
#define XMLSAVECONTEXT_H

#include "xmlEdit.h"

class XMLSaveContext
{
    bool _isDoIndent;
    int _indentation;
    bool _isSortAttributesAlpha;
    bool _isDTDWritten;
    int _attributesMaxColumns;
    bool _isAttributesColumns;
    int _level;
    qint64 _currentAttrPos ;
    qint64 _baseAttrPos ;
    int _attrIndex;
    QHash<QString, QString> _namespacesToInsert;
    QByteArray _spaceBytes;
    QByteArray _crBytes;
    int _bytesPerChar;
    bool _canUseTextMode;
public:
    XMLSaveContext();
    ~XMLSaveContext();

    void setCodec(QTextCodec *theCodec);
    bool isMultiByte();
    bool isDoIndent() const;
    void setDoIndent(bool doIndent);
    int indentation() const;
    void setIndentation(int indentation);
    bool isSortAttributesAlpha() const;
    void setIsSortAttributesAlpha(bool isSortAttributesAlpha);
    bool isDTDWritten() const;
    void setIsDTDWritten(bool isDTDWritten);
    int attributesMaxColumns() const;
    void setAttributesMaxColumns(int attributesMaxColumns);
    bool isAttributesColumns() const;
    void setIsAttributesColumns(bool isAttributesColumns);

    void incLevel();
    void decLevel();
    int level();
    int indentBase(const QString &tag);
    void startElement(QXmlStreamWriter &writer);
    void afterAttributePos(QXmlStreamWriter &writer);
    void incAttributePos(QXmlStreamWriter &writer, const int indentBase);
    //---
    void clearNamespaceDeclarations();
    QHash<QString, QString> namespaceDeclarations();
    QHash<QString, QString> &namespaceDeclarationsReference();
    bool hasNamespaceDeclarations();
    bool canUseTextMode();
};

#endif // XMLSAVECONTEXT_H
