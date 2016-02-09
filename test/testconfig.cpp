/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016 by Luca Bellonda and individual contributors       *
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

#include "testconfig.h"
#include "app.h"

TestConfig::TestConfig()
{
}

TestConfig::~TestConfig()
{
}

bool TestConfig::testUnit()
{
    _testName = "testUnit" ;
    if(!testSaveAndReadWithoutLimit()) {
        return false;
    }
    if(!testSaveAndReadWithLimit()) {
        return false;
    }
    if(!testSaveWithLimitAndReadWithoutLimit()) {
        return false;
    }
    if(!testSaveWithoutLimitAndReadWithLimit()) {
        return false;
    }
    if(!testVerifySaveAndRead()) {
        return false;
    }
    return true ;
}

bool TestConfig::testSaveAndReadWithoutLimit()
{
    _testName = "testSaveAndReadWithoutLimit" ;
    App app;
    if(!app.init()) {
        return "init app";
    }
    QStringList referenceList ;
    referenceList << "1" << "2" << "3" << "4" << "5";
    Config::saveStringArray("a", referenceList);
    QStringList newList ;
    if(!Config::loadStringArray("a", newList)) {
        return error("error loading list");
    }
    if(!compareStringList("lists differ", referenceList, newList)) {
        return false;
    }
    return true;
}

bool TestConfig::testSaveAndReadWithLimit()
{
    _testName = "testSaveAndReadWithLimit" ;
    App app;
    if(!app.init()) {
        return "init app";
    }
    QStringList saveList ;
    saveList << "1" << "2" << "3" << "4" << "5";
    Config::saveStringArray("a", saveList, 3);
    QStringList newList ;
    if(!Config::loadStringArray("a", newList, 3)) {
        return error("error loading list");
    }
    QStringList referenceList ;
    referenceList << "1" << "2" << "3" ;
    if(!compareStringList("lists differ", referenceList, newList)) {
        return false;
    }
    return true;
}

bool TestConfig::testSaveWithLimitAndReadWithoutLimit()
{
    _testName = "testSaveWithLimitAndReadWithoutLimit" ;
    App app;
    if(!app.init()) {
        return "init app";
    }
    QStringList saveList ;
    saveList << "1" << "2" << "3" << "4" << "5";
    Config::saveStringArray("a", saveList, 3);
    QStringList newList ;
    if(!Config::loadStringArray("a", newList)) {
        return error("error loading list");
    }
    QStringList referenceList ;
    referenceList << "1" << "2" << "3" ;
    if(!compareStringList("lists differ", referenceList, newList)) {
        return false;
    }
    return true;
}

bool TestConfig::testSaveWithoutLimitAndReadWithLimit()
{
    _testName = "testSaveWithoutLimitAndReadWithLimit" ;
    App app;
    if(!app.init()) {
        return "init app";
    }
    QStringList saveList ;
    saveList << "1" << "2" << "3" << "4" << "5";
    Config::saveStringArray("a", saveList);
    QStringList newList ;
    if(!Config::loadStringArray("a", newList, 3)) {
        return error("error loading list");
    }
    QStringList referenceList ;
    referenceList << "1" << "2" << "3" ;
    if(!compareStringList("lists differ", referenceList, newList)) {
        return false;
    }
    return true;
}

bool TestConfig::testVerifySaveAndRead()
{
    _testName = "testVerifySaveAndRead" ;
    App app;
    if(!app.init()) {
        return "init app";
    }
    QStringList saveList ;
    saveList << "1" << "2" << "3" << "4" << "5";
    Config::saveStringArray("a", saveList);
    QStringList newList ;
    if(!Config::loadStringArray("a", newList, 3)) {
        return error("error loading list");
    }
    QStringList referenceList ;
    referenceList << "1" << "2" << "3" << "4" ;
    if(compareStringList("lists differ", referenceList, newList)) {
        return error("test does not recognize a difference");
    }
    return true;
}
