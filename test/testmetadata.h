/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013-2018 by Luca Bellonda and individual contributors  *
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


#ifndef TESTMETADATA_H
#define TESTMETADATA_H

#include "testbase.h"
#include <QPair>

class ClassATest;
class App;
class MetadataInfo;

class TestMetadata : public TestBase
{
    QString dumpPair(QList<QPair<QString,QString> > expectedAttributes);
    bool testParseExpectingError(const QString &argTestName, const QString &toParse);
    bool testParseEmpty();
    bool testParseExpectingSuccess(const QString &argTestName, const QString &toParse, QList<QPair<QString,QString> > expectedAttributes);

    QString describeParams(ClassATest *classa);
    bool testCycleOnMeta(ClassATest *classa);

    bool checkMeta(MetadataInfo *info, const QString &name, const bool read, const bool write, const QString &value);
    bool testWriteManualValuesStart(App *app, const QString &argFileName, MetadataInfo *info);
    bool testWriteManualValuesExec(App *app, const QString &argOrigFileName, const QString &argTestFileName, const QString &label);

    bool testParseManualFilled();
    bool testParseManualEmpty();
    bool testEditValuesManual();
    bool testWriteManualValues();


public:
    TestMetadata();
    ~TestMetadata();

    bool testSuiteParsing();
    bool testManageMeta();
    bool testModifyMeta();
};

#endif // TESTMETADATA_H
