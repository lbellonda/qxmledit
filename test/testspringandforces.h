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


#ifndef TESTSPRINGANDFORCES_H
#define TESTSPRINGANDFORCES_H

#include "testbase.h"
#include <QTableWidget>

class TagMarker;

class TestSpringAndForces : public TestBase
{
    bool testUnitForces1();
    bool testUnitForces2();
    bool checkTestUnitForces2(TagMarker *tag, const double expected, const int number, int x0, int y0);
    bool likeAngle( const double angle, const double expected );
    bool testLoadSampleData();

    bool rowIs(QTableWidget *table, const int row, const QString &name);
    bool checkRow(QTableWidget *table, const int row, const QString &countCol, const QString &percentCol, const QString &incomingLinksCol,
                                       const QString &outcomingLinksCol, const QString &itemCol, const QString &obkectCol, const QString &rootCol );
    bool checkItem(QTableWidget *table, const int row, const int column, const QString &text);
public:
    TestSpringAndForces();
    ~TestSpringAndForces();

    bool testUnitForces();
};

#endif // TESTSPRINGANDFORCES_H
