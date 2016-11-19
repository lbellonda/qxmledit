/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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


#ifndef TESTLOADFILE_H
#define TESTLOADFILE_H

#include    "testbase.h"
#include    "regola.h"

class App;

class TestLoadFile : public TestBase, XMLLoadErrorHandler
{
    bool _xmlLoadHandlerAnswer;

    bool testErrorLoadAnyway();
    bool testError0Pos();
    bool testErrorLoadUnit();
    bool loadFileOK();
    bool loadFileKO();
    bool testErrorNOK();
    bool testVerifyLoad1();
    bool testVerifyLoad2();
    bool testLoadWithModifications();
    bool loadDrop();
    bool loadReload();
    bool loadSession();
    bool loadRecent();
    bool loadPreferredDir();
    bool newSpec();
    bool actionReload(App *app, const QString & filePath);
    bool actionDrop(App *app, const QString & filePath);
    bool actionLoadSession(App *app, const QString & filePath);
    bool actionLoadRecent(App *app, const QString & filePath);
    bool actionLoadPreferredDir(App *app, const QString & filePath);
    bool actionNewSpecialized(App *app, const QString & /*filePath*/);

    bool loadFileForMod(const bool openOtherWindow, const QString &filePath, const bool isModification,
                        const int expectingAskingFirst, const int expectingAskingLater,
                        const int expectedWindows,
                        bool (TestLoadFile::*method)(App *, const QString &));
    bool loadFileMod(
                                      const bool openOtherWindow,
                                      const int expectingAskingFirst, const int expectingAskingLater,
                                      const int expectedWindows,
                                      bool (TestLoadFile::*method)(App *, const QString &) );
    bool loadFileNoMod(
                                      const bool openOtherWindow,
                                      const int expectingAskingFirst, const int expectingAskingLater,
                                      const int expectedWindows,
                                      bool (TestLoadFile::*method)(App *, const QString &) );
    bool testAnErrorPos(              const QString &dataIn,
                                      const qint64 line, const qint64 column, const qint64 offset,
                                      const QString &expectedBefore,
                                      const QString &expectedIn,
                                      const QString &expectedAfter);

public:
    TestLoadFile();
    ~TestLoadFile();

    bool testUnit();
    bool testFast();
    bool testErrorsManagement();

    //-- interface(XMLLoadErrorHandler)
    bool showErrorAndAskUserIfContinue(QWidget *parent, XMLLoadContext *context, QXmlStreamReader *xmlReader) ;

};

#endif // TESTLOADFILE_H
