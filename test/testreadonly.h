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


#ifndef TESTREADONLY_H
#define TESTREADONLY_H

#include "testbase.h"
#include "app.h"

class TestReadOnly : public TestBase
{

    bool checkReadOnlyInitial(MainWindow *wnd);
    bool compareOriginalData(MainWindow *wnd);
    bool checkReadOnlyStatus(MainWindow *wnd, const bool expectedReadOnly);
    bool checkReadOnlyModification(MainWindow *wnd);
    bool checkSetReadOnly(MainWindow *wnd);
    bool checkReadOnlyPasteElement(MainWindow *wnd);
    bool checkReadOnlyPasteText(MainWindow *wnd);
    bool checkUnsetReadOnly(MainWindow *wnd);
    bool checkTransformComment(MainWindow *wnd);
    bool checkOpenFile(MainWindow *wnd);
    bool checkNewFromTemplate(MainWindow *wnd);
    bool checkNewFromClipboard(MainWindow *wnd);
    bool checkReadOnlyEdit(MainWindow *wnd, const bool isReadOnly);
    bool setReadOnly(MainWindow *wnd, const bool how);
    bool checkButtonsEnabled( QWidget *editor, const bool expected);
    bool checkEditorButtons(MainWindow *wnd);

public:
    TestReadOnly();
    ~TestReadOnly();

    bool testUnit();
};

#endif // TESTREADONLY_H
