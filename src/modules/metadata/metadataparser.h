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


#ifndef METADATAPARSER_H
#define METADATAPARSER_H

#include <QChar>
#include <QList>
#include <QString>
#include "modules/metadata/basecomplexvariable.h"

class MetadataParsedResult
{
private:
    QList<PseudoAttribute*> _attributes;
public:
    MetadataParsedResult();
    ~MetadataParsedResult();

    void addAttribute(const QString &name, const QString &value, const int rowPos);
    PseudoAttribute *find(const QString &name);
    QList<PseudoAttribute*> &attributes();
    QString dumpAttributes();

    void clean();
    void forgetAttributes();
};

class MetadataParserInfo
{
private:
    bool _isError;
    int _lenInput ;
    int _index ;
    QChar _ch;
    QString _inputString;
    MetadataParsedResult *_attributes;
    int _row;

    void initParse();
    bool isEof();
    void raiseError();

    void parseAttribute();
    void nextChar();
    bool canFollowName();
    bool canStartName();
    bool isBlank();
    bool isEqual();
    bool isQuote();
    bool isDoubleQuote();

    QString eatDelimitedString(const char delimiter);
public:
    MetadataParserInfo();
    ~MetadataParserInfo();

    void setInput(const QString &inputData);
    void setResult(MetadataParsedResult *newAttributes);
    void parse();
    bool isError();

    static QString unescape(const QString &str);
    static QString escape(const QString &str);
    int row() const;
    void setRow(const int row);
};

class MetadataParser
{
private:

public:
    MetadataParser();
    ~MetadataParser();

    bool parseMetadata(const QString &inputData, MetadataParsedResult *attributes, const int rowPos);
};

#endif // METADATAPARSER_H
