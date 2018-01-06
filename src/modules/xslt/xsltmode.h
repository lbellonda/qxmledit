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


#ifndef XSLTMODE_H
#define XSLTMODE_H

#include <QString>
#include <QList>

// this file defines the data strucutre to work with xslt mode


class XslLevel
{
public:
    bool isXsl;
    QString tag;
    QString simpleTag;

    XslLevel();
};

class XslContext
{
public:
    QList<XslLevel*> levels;
    bool inTemplate;

    XslContext();
    ~XslContext();
};


class XslAutocompletionValues
{
private:
    XslAutocompletionValues() {}
    ~XslAutocompletionValues() {}
public:
    enum ESource {
        SourceTemplate, // inside a template
        SourceSameElement,  // look in the same element
        SourceParent, // look using the parent axis
        SourceAll // look inside all the sheet
    };
};


class XsltCompletionType
{
    XsltCompletionType();
    ~XsltCompletionType();
public:
    enum ECompletion {
        All, // all the symbols
        TemplateNames,  // (call-template)
        ParameterNames // parameters (call-template)
    };
};

class XsltInsertable
{
private:
    XsltInsertable() {}
    ~XsltInsertable() {}
public:
    enum EInsertMode {
        InsAnyWhere, // insert everywhere class a
        InsChildInCall, // eg param class c
        InsInCallParam, // parameter of call
        InsTopLevel, //eg: template class t
        InsWhen, //eg: otherwise class w
        InsRoot //only the root item
    };
};


class XsltPosition
{
private:
    XsltPosition() {}
    ~XsltPosition() {}
public:
    enum EPosition {
        PositionFirst,
        PositionLast,
        PositionAll
    };
};

#include "modules/xslt/xsltelement.h"


#endif // XSLTMODE_H
