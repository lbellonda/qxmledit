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


#ifndef TESTSQLACCESS_H
#define TESTSQLACCESS_H

#include "sqlliteaccess.h"

class TestSQLAccess
{

    QString _msg;
    bool deleteByDateIntervalInternal(const QString &sqlData, const SessionDataInterface::EDateInterval intervalType, const bool isFiles, const bool isErrorExpected=false );

public:
    TestSQLAccess();
    ~TestSQLAccess();

    bool error();
    bool error(const QString &msg);
    QString msg();

    bool insertSession();
    bool testFile();
    bool testTransCommands();
    bool insertFileAccess();
    bool testFileNotFound();
    bool insertFileAccessLow();
    bool readFileAccess();
    bool deleteSession();
    bool updateSession();
    bool touchSession();
    //--------------------
    bool deleteByDate1Month();
    bool deleteByDate6Months();
    bool deleteByGivenDate();
    bool deleteByDateWrongType();
    bool deleteByDate1MonthFiles();
    bool deleteByDate6MonthsFiles();
    bool deleteByGivenDateFiles();

};

#endif // TESTSQLACCESS_H
