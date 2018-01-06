/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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

#ifndef TESTXSD_H
#define TESTXSD_H

#include "regola.h"
#include "testbase.h"

class TestXsd : public TestBase
{
    bool testAttributesValue(Element *root,
                             const QString aName1, const QString aValue1,
                             const QString aName2, const QString aValue2);
public:
    TestXsd();

    bool TestNonamespace();
    bool TestNonamespaceFalse();
    bool TestOneSchema();
    bool TestMoreSchema();
    bool testSameNamespace();
};

#endif // TESTXSD_H
