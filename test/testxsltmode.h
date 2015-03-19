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


#ifndef TESTXSLTMODE_H
#define TESTXSLTMODE_H

#include "testbase.h"
#include "testhelpers/XSLTAutoModeDialogprivatetest.h"
#include "testhelpers/XsltElementDialogprivatetest.h"

class MainWindow ;
class XsltElementDialogParam;
class XmlEditWidget;

class TestXSLTMode : public TestBase
{

    bool innerDTest(const QString &nameElem, QList<int> &selection, QSet<QString> &nameItems, QSet<QString> &selectItems , const bool namesEnabled, const bool selectEnabled);
    bool innerDTest( const QString &nameElem, const QString &namesFileAuto, QList<int> &selection, QSet<QString> &nameItems, QSet<QString> &selectItems, const bool namesEnabled, const bool selectEnabled );
    bool checkMenuItems(const QString &filePath, const QString &tag, QList<int> &selection, QSet<QString> &itemsToFind );
    bool checkMenuItemsBase(const QString &tag, QList<int> &selection, QSet<QString> &itemsToFind );
    bool checkWithDialog(XsltElementDialogParam *param, const bool shouldNameEnabled, const bool shouldSelectEnabled, QSet<QString> &nameItems, QSet<QString> &selectItems);
    bool setDialogValues(const bool isWaitDialog, XsltElementDialogParam *param, const QString &nameItem, const QString &selectItem, const bool isInsertOrEdit, const bool isInsertOrAppendElement);
    bool innerDEditOp( const QString &selectedElem, const QString &nameElem, const QString &fileOrig, const QString &fileAfterOperation, QList<int> &selection, const QString &nameItem, const QString &selectItem, const bool isInsertOrEdit, const bool isInsertOrAppendElement);

    bool checkNamespace(MainWindow *mainWindow);
    //---
    bool checkMenuChoose();
    bool checkMenuCall();
    //---

    bool checkMCallTemplate(MainWindow *mainWindow);
    bool checkMWhere(MainWindow *mainWindow);
    bool checkMOtherwise(MainWindow *mainWindow);
    bool checkMApplyTemplate(MainWindow *mainWindow);
    bool checkMParam(MainWindow *mainWindow);
    bool checkMWithParam(MainWindow *mainWindow);
    //---

    bool checkDCallTemplate();
    bool checkDChoose();
    bool checkDWhen();
    bool checkDOtherwise();
    bool checkDApplyTemplate();
    bool checkDIf();
    bool checkDIfTargetDocument();
    bool checkDParam(MainWindow *mainWindow);
    bool checkDWithParam(MainWindow *mainWindow);
    //---

    bool checkInsNewTemplate();
    bool checkInsChoose(MainWindow *mainWindow);
    bool checkInsWhen(MainWindow *mainWindow);

    //-----
    bool checkEditTemplate();

    //-----
    bool checkNavigationSort(MainWindow *mainWindow);
    bool checkNavigationEnable(MainWindow *mainWindow);
    bool checkNavigationGoto(MainWindow *mainWindow);

    //------------------------------
    bool testPanelShowOnOpenFile(const QString &fileName, const XSLTAutoModeDialog::PrivateTest::Tests testToDo, const bool expected);
    bool testPanelShowOnOpenFileXml();
    bool testPanelShowOnOpenFileXsl();

    //---------------------------------------------
    bool testPanelShowOnOpenFileUsingFlags(const XSLTAutoModeDialog::PrivateTest::Tests testToDo, const bool isShow, const bool isAuto, const bool expected );
    bool testPanelShowFlagsNoHideNoAuto();
    bool testPanelShowFlagsNoHideAuto() ;
    bool testPanelShowFlagsHideNoAuto() ;
    bool testPanelShowFlagsHideAuto() ;

    //------------------------
    bool testPanelShowWithAnswers();
    bool testPanelAnswers(const QString &fileName, const XSLTAutoModeDialog::PrivateTest::Tests testToDo, const bool isShow, const XSLTAutoModeDialog::ERetCode answer,
                                        const bool expectedFirstShot, const bool expectedOpenSecondShot,
                                        const bool expectedXSLAfterFirstShot, const bool expectedXSLAfterSecondShot );
    bool intTestInsParameters(const bool isParameters, const QString &fileReference);

public:
    TestXSLTMode();

    bool testBaseMode();
    bool testAdvancedMode();
    bool testNavigation();
    bool testPanel();
    bool testInsParameters();

};

#endif // TESTXSLTMODE_H
