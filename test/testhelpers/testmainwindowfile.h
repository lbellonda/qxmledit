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


#ifndef TESTMAINWINDOWFILE_H
#define TESTMAINWINDOWFILE_H

#include "mainwindow.h"

#include "app.h"

class TestMainWindowFile : public MainWindow
{
    Q_OBJECT
    FakeUIDelegate *_fakeUIdelegate;
public:
    bool _execActionSave_called;
    bool _actionSaveAs_internal_called ;
    QString _actionSaveAs_internal_file;
    bool _askFileNameForSaving_called ;
    QString _askFileNameForSaving_file ;
    bool _actionSaveACopyAs_internal_called ;
    QString _actionSaveACopyAs_internal_file ;

    //

    TestMainWindowFile(const bool isAutoDelete, const bool isSlave, ApplicationData *data, QMainWindow *parent = 0);
    ~TestMainWindowFile();
    FakeUIDelegate *fakeUIdelegate() const;
    void setFakeUIDelegate(FakeUIDelegate *fakeUIdelegate);

    void resetFileData();
    virtual void execActionSave();
    virtual void actionSaveACopyAs_internal(const QString &newFilePath);
    virtual void actionSaveAs_internal(const QString &newFilePath);
    virtual QString askFileNameForSaving(const QString &actualName);
    //
    bool verifySaveTriggered();
    bool verifyEmptySaveTriggered();
    bool verifySaveAsTriggered(const QString &expectedPath);
    bool verifyCopyAsTriggered(const QString &expectedPath);
    bool verifyEditSaveTriggered(const QString &expectedPath);
};

#endif // TESTMAINWINDOWFILE_H
