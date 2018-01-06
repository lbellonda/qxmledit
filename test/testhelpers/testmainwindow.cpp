/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2018 by Luca Bellonda and individual contributors  *
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


#include "testmainwindow.h"


TestMainWindow::TestMainWindow(const bool isSlave, ApplicationData *data, QMainWindow *parent) :
    MainWindow(isSlave, data, parent)
{
    _fakeUIdelegate = NULL ;
}

TestMainWindow::~TestMainWindow()
{
}

FakeUIDelegate *TestMainWindow::fakeUIdelegate() const
{
    return _fakeUIdelegate;
}

void TestMainWindow::setFakeUIDelegate(FakeUIDelegate *fakeUIdelegate)
{
    _fakeUIdelegate = fakeUIdelegate;
}

bool TestMainWindow::loadFile(const QString &filePath, const bool activateModes, const EWindowOpen useWindow, const bool isRegularFile)
{
    if( NULL != _fakeUIdelegate ) {
        _fakeUIdelegate->justBeforeLoad();
    }
    return MainWindow::loadFile(filePath, activateModes, useWindow, isRegularFile);
}
