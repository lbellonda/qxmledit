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


#include "testopeninnewwindow.h"
#include "app.h"

#define FILE_BASE   "../test/data/xml/loading/"
#define FILE_OK FILE_BASE "ok.xml"
#define FILE_KO FILE_BASE "ko.xml"
#define FILE_OK1 FILE_BASE "ok1.xml"

TestOpenInNewWindow::TestOpenInNewWindow()
{
}

TestOpenInNewWindow::~TestOpenInNewWindow()
{
}

bool TestOpenInNewWindow::testFast()
{
    return testUnit();
}

/* tests are:

# Empty | Modf | Abandon? | LoadOK | with options: | #count New | #count Same
1   Y       o       o        Y                          1             1
1.1 Y       o       o        N                          1             1
2   N       Y       Y        Y                          2             1
3   N       Y       Y        N                          1             1
4   N       Y       N        Y                          2             1
5   N       Y       N        N                          1             1
6   N       N       Y        Y                          2             1
7   N       N       Y        N                          1             1
8   N       N       N        Y                          2             1
9   N       N       N        N                          1             1
*/

bool TestOpenInNewWindow::testSingleResult( App *app, const int expectedOpen, const QString &msg )
{
    qApp->processEvents();
    if( app->data()->windowsCount() != expectedOpen ) {
        return error(QString("Expected open %1 windows, but found %2 in phase: %3.").arg(expectedOpen).arg(app->data()->windowsCount()).arg(msg));
    }
    return true ;
}

bool TestOpenInNewWindow::testSingleUsingSettings(const bool isEmpty, const bool isMod, const bool isAbandon,
                                                  const bool isLoadOk, const int expectedOpen, const bool isOpenSame,
                                                  const QString &msg)
{
    App app;
    if(!app.init()) {
        return false;
    }
    QString fileIn = FILE_OK;
    if( !isLoadOk ) {
        fileIn = FILE_KO ;
    }
    Config::saveBool(Config::KEY_GENERAL_OPEN_NEWWINDOW, !isOpenSame);
    if( !isEmpty ) {
        if(!app.mainWindow()->loadFile(FILE_OK1)) {
            return error(QString("Error loading file '%1'").arg(FILE_OK1));
        }
        if(!testSingleResult(&app, 1, "start")) {
            return false;
        }
    }
    if( isMod ) {
        app.mainWindow()->getRegola()->setModified(true);
    }
    if( isAbandon ) {
        app.mainWindow()->setUIDelegate(app.getUiDelegateYes());
    } else {
        app.mainWindow()->setUIDelegate(app.getUiDelegate());
    }
    if(!app.mainWindow()->loadFile(fileIn) ) {
        if(isLoadOk) {
            return error(QString("Error loading 2nd operation file '%1'").arg(fileIn));
        }
    }
    return testSingleResult( &app, expectedOpen, msg );
}

bool TestOpenInNewWindow::testUnitCase( const bool isEmpty, const bool isMod, const bool isAbandon, const bool isLoadOk, const int expectedNew, const int expectedSame)
{
    QString msg = QString("isEmpty %1, isMod %2, isAbandon %3, isLoadOk %4, expectedNew %5, expectedSame %6")
            .arg(isEmpty).arg(isMod).arg(isAbandon).arg(isLoadOk).arg(expectedNew).arg(expectedSame);

    _testName = msg;
    QString msg1 = QString("NEW:")+msg;
    if(!testSingleUsingSettings(isEmpty, isMod, isAbandon, isLoadOk, expectedNew, false, msg1) ) {
        return false;
    }
    msg1 = QString("NEW:")+msg;
    if(!testSingleUsingSettings(isEmpty, isMod, isAbandon, isLoadOk, expectedSame, true, msg1) ) {
        return false;
    }
    return true ;
}

bool TestOpenInNewWindow::testUnit()
{
    _testName = "TestUnit";
    //1   Y       o       o        N                          1             1
    if(!testUnitCase( true, false, false, false, 1, 1)) {
        return false;
    }
    //1.1   Y       o       o        Y                          1             1
    if(!testUnitCase( true, false, false, true, 1, 1)) {
        return false;
    }
    //2   N       Y       Y        Y                          2             1
    if(!testUnitCase( false, true, true, true, 2, 1)) {
        return false;
    }
    if(!testUnitCase( false, true, true, false, 1, 1)) {
        return false;
    }
    if(!testUnitCase( false, true, false, true, 2, 1)) {
        return false;
    }
    if(!testUnitCase( false, true, false, false, 1, 1)) {
        return false;
    }
    //--
    if(!testUnitCase( false, false, true, true, 2, 1)) {
        return false;
    }
    if(!testUnitCase( false, false, true, false, 1, 1)) {
        return false;
    }
    if(!testUnitCase( false, false, false, true, 2, 1)) {
        return false;
    }
    if(!testUnitCase( false, false, false, false, 1, 1)) {
        return false;
    }
    return true ;
}
