/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2020 by Luca Bellonda and individual contributors       *
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

#ifndef TESTLOADSAMPLE_H
#define TESTLOADSAMPLE_H

#include "testbase.h"

class TestLoadSample : public TestBase
{
    bool testLoadError();
    bool testLoadIdentity();
    bool testLoadDouble();
    bool testLoadSumAttributes();
    bool testLoadSumElements();
    bool testLoadSumAttributesNS();
    bool testLoadSumElementsNS();
    bool testLoadPreamble();
    bool testLoadComment();
    bool testLoadProcessingInstruction();
    bool testLoadSumAll();
    bool testLoadSumAllNS();

    //
    bool testAFile(const bool isLoad,
                        const QString &code,
                        const QString &inputFilePath,
                        const bool expectedLoadStatus,
                        const bool expectedModified,
                        const QString &expectedFileName,
                        const QString &compareFileResultPath);
    bool testSame(      const QString &code,
                        const QString &inputFilePath,
                        const bool expectedLoadStatus);
    bool testABatch(const QString &key, const QString &fileSource, const QString &fileReference);
    //

public:
    TestLoadSample();
    ~TestLoadSample();

    bool testUnit();
    bool testFast();
};

#endif // TESTLOADSAMPLE_H
