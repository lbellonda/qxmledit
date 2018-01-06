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


#ifndef TESTREPLICA_H
#define TESTREPLICA_H

#include "testbase.h"
#include "modules/replica/replicacommand.h"
#include "mainwndcontroller.h"

class MainWindow ;
class ReplicaCloneInfo ;

class TestReplica : public TestBase, ReplicaInfoProvider
{
    ReplicaCloneInfo *_theCommand;

    bool testApplyFast();
    void enumAlpha();
    bool testUnitReplica();
    bool testFormat();
    bool testFormatId();
    bool testWidth();
    bool testWidth( const int numSiblings, const bool isAlpha, const int expected);
    bool testFormatNumber( const QString &expected, const int index, const bool isPadded, const int totalWidth);
    bool testFormatString( const QString &expected, const int index, const bool isPadded, const int totalWidth);
    bool checkTestFormatId( const QString &expected, const ReplicaCommand::EFormat format, const int startIndex, const int totalCount);
    bool testFormatId( ReplicaCommand *cmd, const QString &expected, const int startIndex, const int totalCount);
    bool baseApply(const QString &id,
                                const QString &fileIn, const QString &fileCompare,
                                       QList<int> selection,
                                        ReplicaCommand *command );
    bool applyCount(const QString &id,
                    const QString &fileIn, const QString &fileCompare,
                    QList<int> selection,
                    ReplicaCommand *command,
                    const int count );
    bool internalApply(const QString &id,
                        const QString &fileIn, const QString &fileCompare,
                        QList<int> selection,
                        ReplicaCommand *command,
                        const int count);
    bool testFormatNumber();
    bool testFormatAlpha();
    bool compareAttributes(Element *element, const QString &attributeName, const QString &expected );
    bool testCheckParams();
    bool checkAndRaiseError( const QString &id, ReplicaCommand *cmd, const ReplicaCommand::EErrorCode expected);
    bool testCommandCompare();
    bool testUnitCommand();
    bool testCommandClone();
    bool testCommandDialog();
    bool checkCombo(QStringList values, const QString &comboName, const QString &expected);
    bool testSkeletonClone(const QString &id, const QString &fileStart, const QString &fileFinal, bool (TestReplica::*apply) (MainWindow *, Element*) );
    bool testSkeletonCloneComplex( const QString &id, const QString &fileStart, const QString &fileFinal, bool (TestReplica::*apply)(MainWindow *, Element *) );
    bool testCloneDo();
    bool testCloneDialog();
    bool doSimpleClone(MainWindow *wnd, Element* element);
    bool doCloneRecursive(MainWindow *wnd, Element*element);
    bool cfr(Regola *regola, const QString &step, const QString &fileResult);
    ReplicaCloneInfo *getCloneInfo(QWidget *parent, Element *);
    bool doCloneDialogRecursive(MainWindow * wnd, Element* element);
    bool doSimpleDialogClone(MainWindow * wnd, Element* element);
public:
    TestReplica();
    ~TestReplica();

    bool testFast();
    bool testUnit();
    bool testAction();
    bool testApply();
    bool testClone();
};

#endif // TESTREPLICA_H
