/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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


#ifndef COMPARERESULTTEXTFORMAT_H
#define COMPARERESULTTEXTFORMAT_H

#include <QString>
#include "operationresult.h"
#include "diffresult.h"

class CompareResultTextFormat
{
    QString resultText;

    QString stateE;
    QString stateA;
    QString stateM;
    QString stateD;

    QString stateToClassCode(const EDiff::KDiff state);
    void closeTag(Element *element, const EDiff::KDiff state, const bool isEmpty);

    void dumpAttribute(Attribute *attribute, const EDiff::KDiff state);
    void dumpAttributeList(QList<Attribute*> &attributes, const EDiff::KDiff state);
    void dumpAttributes(DiffSingleNodeResult *node);
    QString convertTextInHTML(const QString &strIn);
    void dumpText(const QString text, const EDiff::KDiff state, const bool isCData, const int indent);
    void endElement(Element *element, const int indent, const EDiff::KDiff state);
    Element *dumpElement(DiffSingleNodeResult *node, const int indent, const EDiff::KDiff state);
    void scanRecursive(DiffSingleNodeResult * node, const int indent);

public:
    CompareResultTextFormat();
    virtual ~CompareResultTextFormat();

    QString formatText(OperationResult &result, DiffNodesChangeList *diffList);
    QString text();
};

#endif // COMPARERESULTTEXTFORMAT_H
