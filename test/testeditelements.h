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


#ifndef TESTEDITELEMENTS_H
#define TESTEDITELEMENTS_H

#include "app.h"

class TestEditElements
{
    App app;
    Element *selectedElement ;
    Regola *regola ;

    static bool error();
    bool start(const bool isNew, QList<int> &selection);
    bool compare(const QString &referenceFileName);
    bool compareDocuments(const QString &filename, Regola *regola);
    Element *newProcessingInstruction();
    Element *createElement();

    bool insertCommentEmpty();
    bool insertCommentAtRootNoSel();
    bool insertCommentUnderRoot();
    bool appendCommentToRoot();
    bool insertCommentAsChild();
    bool appendCommentAsSibling();
    bool insertProcessingInstructionEmpty();
    bool insertProcessingInstructionAtRootNoSel();
    bool insertProcessingInstructionUnderRoot();
    bool appendProcessingInstructionToRoot();
    bool insertProcessingInstructionAsChild();
    bool appendProcessingInstructionAsSibling();

    bool insertElementEmpty();
    bool insertElementAtRootNoSel();
    bool insertElementUnderRoot();
    bool insertElementAsChild();
    bool appendElementAsSibling();
    bool insertElementAsChildWithAttributes();
    bool appendElementAsSiblingWithAttributes();
    bool insertElementAsChildWithAttributesAndText();
    bool appendElementAsSiblingWithAttributesAndText();

public:
    TestEditElements();

    static bool test();
};

#endif // TESTEDITELEMENTS_H
