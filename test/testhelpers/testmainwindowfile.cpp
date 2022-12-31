/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2022 by Luca Bellonda and individual contributors  *
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


#include "testmainwindowfile.h"


TestMainWindowFile::TestMainWindowFile(const bool isAutoDelete, const bool isSlave, ApplicationData *data, QMainWindow *parent) :
    MainWindow(isAutoDelete, isSlave, data, parent)
{
    _fakeUIdelegate = NULL ;
    resetFileData();
}

TestMainWindowFile::~TestMainWindowFile()
{
}

FakeUIDelegate *TestMainWindowFile::fakeUIdelegate() const
{
    return _fakeUIdelegate;
}

void TestMainWindowFile::setFakeUIDelegate(FakeUIDelegate *fakeUIdelegate)
{
    _fakeUIdelegate = fakeUIdelegate;
}

void TestMainWindowFile::resetFileData()
{
    _execActionSave_called = false ;
    _actionSaveAs_internal_called = false ;
    _actionSaveAs_internal_file = "";
    _askFileNameForSaving_called = false ;
    _askFileNameForSaving_file = "";
    _actionSaveACopyAs_internal_called = false ;
    _actionSaveACopyAs_internal_file = "" ;
}

void TestMainWindowFile::execActionSave()
{
    _execActionSave_called = true ;
}

void TestMainWindowFile::actionSaveAs_internal(const QString &newFilePath)
{
    _actionSaveAs_internal_called = true ;
    _actionSaveAs_internal_file = newFilePath ;
}

QString TestMainWindowFile::askFileNameForSaving(const QString &actualName)
{
    _askFileNameForSaving_called = true ;
    _askFileNameForSaving_file = fileNameForSaving(actualName);
    return _askFileNameForSaving_file;
}

void TestMainWindowFile::actionSaveACopyAs_internal(const QString &newFilePath)
{
    _actionSaveACopyAs_internal_called = true ;
    _actionSaveACopyAs_internal_file = newFilePath ;
}

bool TestMainWindowFile::verifySaveTriggered()
{
    return _execActionSave_called
        && !_actionSaveAs_internal_called
    && (_actionSaveAs_internal_file =="")
    && !_askFileNameForSaving_called
    && (_askFileNameForSaving_file == "" )
    && !_actionSaveACopyAs_internal_called
    && (_actionSaveACopyAs_internal_file == "" );
}

bool TestMainWindowFile::verifyEmptySaveTriggered()
{
    return !_execActionSave_called
        && !_actionSaveAs_internal_called
    && (_actionSaveAs_internal_file == "")
    && !_askFileNameForSaving_called
    && (_askFileNameForSaving_file == "" )
    && !_actionSaveACopyAs_internal_called
    && (_actionSaveACopyAs_internal_file == "" );
}

bool TestMainWindowFile::verifySaveAsTriggered(const QString &expectedPath)
{
    return !_execActionSave_called
        && _actionSaveAs_internal_called
    && (_actionSaveAs_internal_file == expectedPath)
    && _askFileNameForSaving_called
    && (_askFileNameForSaving_file == expectedPath )
    && !_actionSaveACopyAs_internal_called
    && (_actionSaveACopyAs_internal_file == "" );
}

bool TestMainWindowFile::verifyCopyAsTriggered(const QString &expectedPath)
{
    return !_execActionSave_called
        && !_actionSaveAs_internal_called
    && (_actionSaveAs_internal_file == "")
    && _askFileNameForSaving_called
    && (_askFileNameForSaving_file == expectedPath )
    && _actionSaveACopyAs_internal_called
    && (_actionSaveACopyAs_internal_file == expectedPath );
}

bool TestMainWindowFile::verifyEditSaveTriggered(const QString &expectedPath)
{
    return !_execActionSave_called
        && !_actionSaveAs_internal_called
    && (_actionSaveAs_internal_file == "")
    && _askFileNameForSaving_called
    && (_askFileNameForSaving_file == expectedPath )
    && _actionSaveACopyAs_internal_called
    && (_actionSaveACopyAs_internal_file == expectedPath );
}
