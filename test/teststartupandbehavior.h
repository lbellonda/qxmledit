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

#ifndef TESTSTARTUPANDBEHAVIOR_H
#define TESTSTARTUPANDBEHAVIOR_H

#include "testbase.h"
#include "applicationdata.h"

struct TestStartInvocationData;

class TestStartupAndBehavior : public TestBase
{
    bool checkStartupAction(
            const QString &id,
            // set values
            const bool setFirstAccess, const bool setOpenFile, const ApplicationData::EUserType setUserType,
            // -- expected
            const bool expectedAskUserTypePanel, const bool expectedOpenGuidedPanel,
            const bool expectedAskVisDetailsTypePanel, const bool expectedShowWelcome,
            const bool expectedOpenFile, const ApplicationData::EUserType panelUserType,
            // final values
            const ApplicationData::EUserType checkFinalUserType );
    bool checkExpectedStartupResults(TestStartInvocationData *obj, TestStartInvocationData *expected);

public:
    TestStartupAndBehavior();
    ~TestStartupAndBehavior();

    bool testUnit();
    bool testFast();
};

#endif // TESTSTARTUPANDBEHAVIOR_H
