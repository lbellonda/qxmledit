/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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


#ifndef TESTCLIPBOARD_H
#define TESTCLIPBOARD_H

#include "testbase.h"

class Element;
class MainWindow;

class TestClipboard : public TestBase
{

    bool setClipboard(const QString &value);
    bool verifyClipboard(const QString &value);
    Element *selectElement(MainWindow *mainWindow, QList<int> &selection);

public:
    TestClipboard();

    bool testCopyElementPathToClipboard();
    bool testPasteAsSibling();
    bool testCopyAllToClipboard();
};

#endif // TESTCLIPBOARD_H
