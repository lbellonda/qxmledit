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


#ifndef TESTUNDOREDO_H
#define TESTUNDOREDO_H

#include "app.h"

class TestUndoRedo
{
    App app;
    Element *selectedElement ;
    Regola *regola ;

    bool start(const bool isNew, QList<int> &selection);
    bool start(const bool isNew, QList<int> &selection, const QString &fileName);

    bool testInsert1();
    bool testInsert2();
    bool testInsert3();
    bool testAppend1();
    bool testAppend2();
    bool testAppend3();
    bool testAppendProcessingInstruction();
    bool testAddProcessingInstruction();
    bool testAppendComment();
    bool testAddComment();
    bool testTransformInComment();
    bool testEditTextNode();

    bool testEditTextNode1();
    bool testEditTextNode2();
    bool testEditTextNode3();
    bool testEditTextNode4();

    bool testDelete();
    bool testCut();
    bool testDeleteRootElement();
    bool testDeleteRootComment();

    bool testEdit();
    bool testPaste();
    bool testMoveUp1();
    bool testMoveUp2();
    bool testMoveUp2W();
    bool testMoveUpBumper();
    //-
    bool testMoveDown1();
    bool testMoveDown2();
    bool testMoveDown2W();
    bool testMoveDownBumper();
    //
    bool testMixed();

    bool error();
    bool error(const QString &theCause);
    bool checkAfterMove(const bool isUndo, const bool isRedo, const QString &docName);
    bool compareDocuments(const QString &filename, Regola *regola);
    bool select(QList<int> &selection);

    Element *newElement();
    Element *newElementWithTextAndAttributes();
    Element *newElementWithTextAndAttributesComplex();
public:
    QString cause;

    TestUndoRedo();

    bool test();
};

#endif // TESTUNDOREDO_H
