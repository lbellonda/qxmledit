/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
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

#ifndef TESTMIXEDCONTENT_H
#define TESTMIXEDCONTENT_H

#include "../src/editelement.h"

class MCTestTransformer
{
public:
    virtual bool preDialog(Element *element, const int operationType) = 0 ;
    virtual bool exec(EditElement *subject, const int operationType) = 0 ;
    virtual bool postDialog(Element *element, const int operationType) = 0 ;
    bool deleteElement(Element *element, const int index);
    void selectAndDelete(EditElement *subject, const int index);
    void selectAndModify(EditElement *subject, const int index, const QString newData);
    void selectAndAdd(EditElement *subject, const int index, const QString newData);
    void selectAndMoveUp(EditElement *subject, const int index);
    void selectAndMoveDown(EditElement *subject, const int index);
    bool updateElement(Element *element, const int index, const QString &newTag);
    Element *elementAt(Element *parent, const int index);
};

class M1Case1Test : public MCTestTransformer
{
public:
    virtual bool exec(EditElement *subject, const int operationType);
    virtual bool preDialog(Element *element, const int operationType);
    virtual bool postDialog(Element *element, const int operationType) ;
};

class M3Case1Test : public MCTestTransformer
{
public:
    virtual bool exec(EditElement *subject, const int operationType);
    virtual bool preDialog(Element *element, const int operationType);
    virtual bool postDialog(Element *element, const int operationType) ;
};

class M6Case1Test : public MCTestTransformer
{
public:
    virtual bool exec(EditElement *subject, const int operationType);
    virtual bool preDialog(Element *element, const int operationType);
    virtual bool postDialog(Element *element, const int operationType) ;
};

class TestMixedContent
{
    bool _showXML ;
    bool execTest(const QString &inputFileName, const QString &outputFileName, MCTestTransformer &transformer, const int operationType);

    bool internalTest(const QString &testName, const QString &inputFileName, const QString &outputFileName, MCTestTransformer &transformer, const int operationType);

public:
    TestMixedContent();

    bool test_m1();
    bool test_m3();
    bool test_m6();

};

#endif // TESTMIXEDCONTENT_H
