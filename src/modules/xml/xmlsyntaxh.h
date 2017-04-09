/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2017 by Luca Bellonda and individual contributors       *
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

#ifndef XMLSYNTAXH_H
#define XMLSYNTAXH_H

#include "xmlEdit.h"
#include <QSyntaxHighlighter>
#include "paintinfo.h"
#include "modules/services/colormanager.h"

class XMLSyntaxH : public QSyntaxHighlighter
{
    static const int StateBefore = -1 ;
    static const int StateHalfTag = 0 ;
    static const int StateTag = 1 ;
    static const int StateInStringApic = 2 ;
    static const int StateInStringQuote = 3 ;

    QTextCharFormat _attributeNameTextFormat;
    QTextCharFormat _attributeValueTextFormat;
    QTextCharFormat _tagTextFormat;
public:
    XMLSyntaxH(QTextDocument *parent, PaintInfo *paintInfo);
    virtual ~XMLSyntaxH();
protected:
    virtual void highlightBlock(const QString &text);
private:
    int matchUpToString(const QString &text, const int index, const int maxIndex, bool &stringIsOpen, QChar &separator);
    int scanToEndOfToString(const QString &text, const int index, const int maxIndex, bool &stringIsOpen, const QChar &separator);
    int matchUpToEqual(const QString &text, const int index, const int maxIndex);
    int findNextSpace(const QString &text, const int index, const int maxIndex);
    int skipSpaces(const QString &text, const int index, const int maxIndex);
    void setStateStringOpen(const QChar &separator);
    void scanAttributes(const QString &text, const int startIndex, const int maxIndex);
    void highlightBlockBase(const QString &text, const bool lookForTag, const bool lookForHalfTag);
    void highlightEndString(const QString &text, const QChar &separator);
};

#endif // XMLSYNTAXH_H
