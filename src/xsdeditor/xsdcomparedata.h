/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2018 by Luca Bellonda and individual contributors  *
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


#ifndef XSDCOMPAREDATA_H
#define XSDCOMPAREDATA_H

#include <QList>
#include <QSet>

bool isCompareAnnotationsPersistentOption();
void setCompareAnnotationsPersistentOption(bool value);


class XSchemaObject;

class XSDCompareData
{
public:
    QList<XSchemaObject*> finalCollection;
    QList<XSchemaObject*> targetCollection;
    QSet<int> targetRemoved ;
    int referenceCount ;
    int targetCount ;
    int indexTarget  ;

    XSDCompareData(QList<XSchemaObject*> &referenceList, QList<XSchemaObject*> &targetList);
    ~XSDCompareData();

    int nextIndexTarget(const int indexTarget);
};

class XSDSchema ;

class XSDCompareResult
{
    bool _error;
    bool _different;
    // reference,
    // target
    // options
    // difference list


public:
    XSDSchema *_schema;

    explicit XSDCompareResult();
    virtual ~XSDCompareResult();

    bool isError();
    void setError(const bool value);
    void setDifferent(const bool value);
    bool areDifferent();
};


class XSDCompareOptions
{
    bool _compareComment;

public:
    explicit XSDCompareOptions();
    virtual ~XSDCompareOptions();

    bool isCompareComment();
    void setCompareComment(const bool value);
};


#endif // XSDCOMPAREDATA_H
