/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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


#ifndef TESTCONTAINER_H
#define TESTCONTAINER_H

#include "testbase.h"

class MainWindow;

class TestContainer : public TestBase
{
    bool testGeneric(const bool isInsert, const QString &testName, const QString &fileStart, const QString &fileEnd , QList<int> selectPath, const bool expected);
    bool testEnableRegola(MainWindow *window, const QString &fileName, QList<int> &selectPath, const bool expected, const QString &actionName, Element *&selectedElement);
public:
    TestContainer();
    ~TestContainer();

    bool testInsertChildContainer();
    bool testInsertParent();
    bool testRemoveParent();
    bool testFast();
    bool testEnableInsParent();
    bool testEnableRemoveParent();
};

#endif // TESTCONTAINER_H
