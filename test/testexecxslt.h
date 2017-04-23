/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2017 by Luca Bellonda and individual contributors       *
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

#ifndef TESTEXECXSLT_H
#define TESTEXECXSLT_H

#include "testbase.h"

class MessagesOperationResult;

class TestExecXSLT : public TestBase
{
    bool checkExecParameters();
    bool checkExecRun();
    bool checkExecRunSources();
    bool checkExecRunSources1();
    bool checkExecRunSources2();
    bool checkExecRunSources3();
    bool checkExecRunSources4();
    bool checkExecRunSources5();
    bool checkExecRunSources6();
    bool checkExecErrors(const QString &id, MessagesOperationResult &result, QList<int> errorsToCheck, QList<int> errorsToAvoid);
    bool checkHandleParameters();
    bool checkExtractParametersFromXSL();
    bool checkMergeParametersFromXSL();
    bool checkResetParametersFromXSL();
    bool checkExtractParametersFromDialog();
    bool checkMergeParametersFromEditor();
    bool checkComamndLine();
    bool checkSaxonEngine();
    bool checkSaxonEngineCfg();
    bool checkChooseEngine1();
    bool checkChooseEngine();
    bool checkExecRunSourcesSaxon();
public:
    TestExecXSLT();
    ~TestExecXSLT();

    bool testFast();
    bool testUnit();
};

#endif // TESTEXECXSLT_H
