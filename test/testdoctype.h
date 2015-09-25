/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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


#ifndef TESTDOCTYPE_H
#define TESTDOCTYPE_H

#include "testbase.h"
#include "app.h"

class TestDocType : public TestBase
{
    bool checkUnit(const QString &theDTD, const QString &expectedDtd, const QString &expectedDocType);
    bool go(const QString &fileNameIn, const QString &fileNameOut);
    bool goEdit(const QString &fileNameIn, const QString &fileNameOut,
                        const bool setReadOnly, const bool isEdit,
                        const QString &newValue, const bool isCancel=false );
    bool compareDocuments(const QString &filename, Regola *regola);
    bool testType();
    bool testPublic();
    bool testSystem();
    bool testCommentLoading();
    //
    bool testEditEmptyFilled();
    bool testEditFilledEmpty();
    bool testEditChange();
    bool testEditReadOnly();
    bool testEditSameNoEdit();
    bool testEditCancel();
    bool testDTD();
public:
    TestDocType();
    ~TestDocType();

    bool test();
    bool testUnit();
    bool testEdit();
    bool testFast();
};

#endif // TESTDOCTYPE_H
