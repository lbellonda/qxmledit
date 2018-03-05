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

//--------
struct TestStartInvocationData {
public:
    _userType;
    //--
    bool firstAccessCalled;
    TestStartInvocationData();
    ~TestStartInvocationData();
};

TestStartInvocationData::TestStartInvocationData()
{
    firstAccessCalled = false ;
}

TestStartInvocationData::~TestStartInvocationData()
{
    //
}

//--------
class FakeStartActionsExecutor : public StartActionsExecutor {

public:
    TestStartInvocationData _data;
    FakeStartActionsExecutor();
    virtual ~FakeStartActionsExecutor();

    virtual void startActionShowUserTypePanel();
    virtual bool startActionShowGuidedOperationsPanel();
    virtual void startActionSetupFirstAccessForPreferences();
    virtual void startActionTriggersWelcomeDialog();
    virtual void startActionLoadFile(const QString &fileName);
};

FakeStartActionsExecutor::FakeStartActionsExecutor()
{
}

FakeStartActionsExecutor::~FakeStartActionsExecutor()
{
}

void FakeStartActionsExecutor::startActionShowUserTypePanel()
{
    _data.firstAccessCalled = true ;
}
bool FakeStartActionsExecutor::startActionShowGuidedOperationsPanel()
{
    a
}
void FakeStartActionsExecutor::startActionSetupFirstAccessForPreferences()
{
    s
}
void FakeStartActionsExecutor::startActionTriggersWelcomeDialog()
{
    s
}
void FakeStartActionsExecutor::startActionLoadFile(const QString &fileName)
{
    s
}

//-------

TestStartupAndBehavior::TestStartupAndBehavior()
{

}


TestStartupAndBehavior::~TestStartupAndBehavior()
{

}

bool TestStartupAndBehavior::testFast()
{
    return testUnit();
}

bool TestStartupAndBehavior::testUnit()
{
    _testName = "testUnit";
    User	Argument	Access	Ask User Type	Panel(s)	Run

    Expert	***	First	Yes	View Details	Welcome
    Expert	Open File	First	Yes	View Details	Open File
    Expert	***	Following	No	***	Welcome
    Expert	Open File	Following	No	***	Open File
    Basic	***	First	Yes	Operations	***
    Basic	Open File	First	Yes	Operations	Open File
    Basic	***	Following	No	Operations	***
    Basic	Open File	Following	No	Operations	Open File


    return error("nyi");
}

bool TestStartupAndBehavior::checkStartupAction(const bool setFirstAccess,
                                                const bool expectedFirstAccess )
{
    App app;
    if(!app.initWitooutWIndow() ) {
        return error("Init");
    }
    TestStartInvocationData expected;
    expected.firstAccessCalled = expectedFirstAccess ;
    //---
    FakeStartActionsExecutor executor;
    //----
    StartupParams startupParams;
    //----
    StartActionsEngine engine(app.data());
    engine.execute(startupParams);
    return checkStartup(executor, expected);
}

bool TestStartupAndBehavior::checkStartup(TestStartInvocationData *obj, TestStartInvocationData *expected)
{
    if(obj->firstAccessCalled != expected->firstAccessCalled ) {
        return error( QString("firstAccessCalled expected:%1, found:%2").arg(expected->firstAccessCalled).arg(obj->firstAccessCalled));
    }
        Expert	***	First	Yes	View Details	Welcome
        Expert	Open File	First	Yes	View Details	Open File
        Expert	***	Following	No	***	Welcome
        Expert	Open File	Following	No	***	Open File
        Basic	***	First	Yes	Operations	***
        Basic	Open File	First	Yes	Operations	Open File
        Basic	***	Following	No	Operations	***
        Basic	Open File	Following	No	Operations	Open File

}
