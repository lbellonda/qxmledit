/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2018 by Luca Bellonda and individual contributors       *
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

#include "teststartupandbehavior.h"
#include "modules/services/startactionsengine.h"
#include "app.h"
#include "StartParams.h"

//--------
struct TestStartInvocationData {
public:
    //--
    bool askUserTypePanel;
    bool openGuidedPanel;
    bool askVisDetailsTypePanel ;
    bool showWelcome ;
    bool openFile;
    ApplicationData::EUserType panelUserType;
    TestStartInvocationData();
    ~TestStartInvocationData();
};

TestStartInvocationData::TestStartInvocationData()
{
    askUserTypePanel = false ;
    openGuidedPanel = false ;
    askVisDetailsTypePanel = false ;
    showWelcome = false ;
    openFile = false ;
    panelUserType = ApplicationData::UserTypeExpert;
}

TestStartInvocationData::~TestStartInvocationData()
{
    //
}

//--------
class FakeStartActionsExecutor : public StartActionsExecutor {

public:
    TestStartInvocationData _data;
   ApplicationData *_appData ;
    FakeStartActionsExecutor(ApplicationData *appData);
    virtual ~FakeStartActionsExecutor();

    virtual void startActionShowUserTypePanel();
    virtual bool startActionShowGuidedOperationsPanel();
    virtual void startActionSetupFirstAccessForPreferences();
    virtual void startActionTriggersWelcomeDialog();
    virtual void startActionLoadFile(const QString &fileName);
};

FakeStartActionsExecutor::FakeStartActionsExecutor(ApplicationData *appData)
{
    _appData = appData ;
}

FakeStartActionsExecutor::~FakeStartActionsExecutor()
{
}

void FakeStartActionsExecutor::startActionShowUserTypePanel()
{
    _data.askUserTypePanel = true;
    _appData->setUserType(_data.panelUserType);
}

bool FakeStartActionsExecutor::startActionShowGuidedOperationsPanel()
{
    _data.openGuidedPanel = true ;
    return true ;
}

void FakeStartActionsExecutor::startActionSetupFirstAccessForPreferences()
{
    _data.askVisDetailsTypePanel = true ;
}

void FakeStartActionsExecutor::startActionTriggersWelcomeDialog()
{
    _data.showWelcome = true ;
}

void FakeStartActionsExecutor::startActionLoadFile(const QString & /*fileName*/)
{
    _data.openFile = true ;
}

//-------

TestStartupAndBehavior::TestStartupAndBehavior()
{
    //
}


TestStartupAndBehavior::~TestStartupAndBehavior()
{
    //
}

bool TestStartupAndBehavior::testFast()
{
    return testUnit();
}

/**
Test       User       Argument   Access     Ask User TyPanel(s)   Run
     0     Expert     ***        First      Yes        View DetailWelcome
     1     Expert     Open File  First      Yes        View DetailOpen File
     2     Expert     ***        Following  No         ***        Welcome
     3     Expert     Open File  Following  No         ***        Open File
     4     Basic      ***        First      Yes        Operations ***
     5     Basic      Open File  First      Yes        Operations Open File
     6     Basic      ***        Following  No         Operations ***
     7     Basic      Open File  Following  No         Operations Open File
*/
bool TestStartupAndBehavior::testUnit()
{
    _testName = "testUnit";
    if(!checkStartupAction("0", true, false, ApplicationData::UserTypeExpert,
                true, false, true, true, false, ApplicationData::UserTypeExpert,
                ApplicationData::UserTypeExpert )) {
        return false;
    }
    if(!checkStartupAction("1", true, true, ApplicationData::UserTypeExpert,
                true, false, true, false, true, ApplicationData::UserTypeExpert,
                ApplicationData::UserTypeExpert )) {
        return false;
    }
    if(!checkStartupAction("2", false, false, ApplicationData::UserTypeExpert,
                false, false, false, false, true, ApplicationData::UserTypeExpert,
                ApplicationData::UserTypeExpert )) {
        return false;
    }
    if(!checkStartupAction("3", false, true, ApplicationData::UserTypeExpert,
                false, false, false, true, false, ApplicationData::UserTypeExpert,
                ApplicationData::UserTypeExpert )) {
        return false;
    }
    if(!checkStartupAction("4", true, false, ApplicationData::UserTypeExpert,
                true, true, false, false, false, ApplicationData::UserTypeGuided,
                ApplicationData::UserTypeGuided )) {
        return false;
    }
    if(!checkStartupAction("5", true, true, ApplicationData::UserTypeExpert,
                true, true, false, false, true, ApplicationData::UserTypeGuided,
                ApplicationData::UserTypeGuided )) {
        return false;
    }
    if(!checkStartupAction("6", false, false, ApplicationData::UserTypeExpert,
                false, true, false, false, false, ApplicationData::UserTypeGuided,
                ApplicationData::UserTypeGuided )) {
        return false;
    }
    if(!checkStartupAction("7", false, true, ApplicationData::UserTypeExpert,
                false, true, false, false, true, ApplicationData::UserTypeGuided,
                ApplicationData::UserTypeGuided )) {
        return false;
    }
    return true ;
}

bool TestStartupAndBehavior::checkStartupAction(
        const QString &id,
        // set values
        const bool setFirstAccess, const bool setOpenFile, const ApplicationData::EUserType setUserType,
        // -- expected
        const bool expectedAskUserTypePanel, const bool expectedOpenGuidedPanel,
        const bool expectedAskVisDetailsTypePanel, const bool expectedShowWelcome,
        const bool expectedOpenFile, const ApplicationData::EUserType panelUserType,
        // final values
        const ApplicationData::EUserType checkFinalUserType )
{
    _subTestName = id ;
    App app;
    if(!app.initNoWindow() ) {
        return error("Init");
    }
    if(!setFirstAccess){
        app.data()->fireUserFirstAccess();
    } else {
        app.data()->setUserType(setUserType);
    }

    //----
    StartParams startupParams;
    if(setOpenFile) {
        startupParams.type = StartParams::OpenFile;
        startupParams.fileName = "test" ;
    } else {
        startupParams.type = StartParams::Nothing;
    }
    //----
    // check set values
    if(!app.data()->isUserFirstAccess() != setFirstAccess) {
        return error(QString("id:%1 wrong first access, expected %2").arg(id).arg(setFirstAccess));
    }
    if(!setFirstAccess) {
        if(ApplicationData::UserTypeExpert == setUserType) {
            if(app.data()->isUserGuidedOperation()) {
                return error(QString("id:%1 set user expert but not verified").arg(id));
            }
        } else if(ApplicationData::UserTypeGuided == setUserType) {
            if(!app.data()->isUserGuidedOperation()) {
                return error(QString("id:%1 set user guided but not verified").arg(id));
            }
        } else {
            return error(QString("id:%1 invalid user type %2").arg(id).arg(setUserType));
        }
    }
    //--
    TestStartInvocationData expected;
    expected.askUserTypePanel = expectedAskUserTypePanel;
    expected.openGuidedPanel = expectedOpenGuidedPanel;
    expected.askVisDetailsTypePanel = expectedAskVisDetailsTypePanel;
    expected.showWelcome = expectedShowWelcome ;
    expected.openFile = expectedOpenFile;
    expected.panelUserType = panelUserType ;
    //----
    FakeStartActionsExecutor executor(app.data());
    StartActionsEngine engine( app.data(), &executor);
    engine.execute(startupParams);
    if(!checkExpectedStartupResults(&executor._data, &expected)) {
        return false;
    }
    if(app.data()->isUserFirstAccess()) {
        return error(QString("id:%1 still first access").arg(id));
    }

    if(ApplicationData::UserTypeExpert == checkFinalUserType) {
        if(app.data()->isUserGuidedOperation()) {
            return error(QString("id:%1 final expected user expert but not verified").arg(id));
        }
    } else if(ApplicationData::UserTypeGuided == checkFinalUserType) {
        if(!app.data()->isUserGuidedOperation()) {
            return error(QString("id:%1 final set user guided but not verified").arg(id));
        }
    } else {
        return error(QString("id:%1 final invalid user type %2").arg(id).arg(setUserType));
    }

    return true;
}

bool TestStartupAndBehavior::checkExpectedStartupResults(TestStartInvocationData *obj, TestStartInvocationData *expected)
{
    if(obj->askUserTypePanel != expected->askUserTypePanel ) {
        return error( QString("askUserTypePanel expected:%1, found:%2").arg(expected->askUserTypePanel).arg(obj->askUserTypePanel));
    }
    if(obj->openGuidedPanel != expected->openGuidedPanel ) {
        return error( QString("openGuidedPanel expected:%1, found:%2").arg(expected->openGuidedPanel).arg(obj->openGuidedPanel));
    }
    if(obj->askVisDetailsTypePanel != expected->askVisDetailsTypePanel ) {
        return error( QString("askVisDetailsTypePanel expected:%1, found:%2").arg(expected->askVisDetailsTypePanel).arg(obj->askVisDetailsTypePanel));
    }
    if(obj->showWelcome != expected->showWelcome ) {
        return error( QString("showWelcome expected:%1, found:%2").arg(expected->showWelcome).arg(obj->showWelcome));
    }
    if(obj->openFile != expected->openFile ) {
        return error( QString("openFile expected:%1, found:%2").arg(expected->openFile).arg(obj->openFile));
    }
    return true ;
}
