/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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

#include "testsortattributes.h"
#include "app.h"

#define BASE_PATH "../test/data/xml/saving/"

#define UNSORTED  BASE_PATH "unsorted.xml"
#define SORTED  BASE_PATH "sorted.xml"
#define UNSORTED_NOINDENT  BASE_PATH "unsorted_noindent.xml"
#define SORTED_NOINDENT  BASE_PATH "sorted_noindent.xml"

TestSortAttributes::TestSortAttributes()
{
}

TestSortAttributes::~TestSortAttributes()
{
}

bool TestSortAttributes::testFast()
{
    return testSaving() ;
}

/**
 * @brief TestSortAttributes::testSaving
 * @return
 * 1,2 tests: default->default (sorted and unsorted)
 * 3 no sort -> no sort
 * 4 sort -> sort
 * 5 no sort && regola-> sort
 */
bool TestSortAttributes::testSaving()
{
    _testName = "testSaving";
    if( !saveDefaultWithSorted() ) {
        return false;
    }
    if( !saveDefaultWithUnsorted() ) {
        return false;
    }
    if(!noSortAndRegolaSort()) {
        return false;
    }
    if(!noSortAndRegolaUnSort()) {
        return false;
    }
    return true ;
}

bool TestSortAttributes::baseSetup(const QString &fileIn, const QString &fileCompare,
                                   const bool setGlobal, const bool setSortGlobal,
                                   const bool setLocal, const bool setSortLocal)
{
    App app;
    if(!app.init1()) {
        return error("init app 1");
    }
    if(!app.init()) {
        return error("init app 2");
    }
    if( !app.mainWindow()->loadFile(fileIn) ) {
        return error(QString("unable to load input file: '%1' ").arg(fileIn));
    }
    Regola *regola = app.mainWindow()->getRegola();
    QList<int> selection;
    selection << 1 << 0 ;
    Element *selectedElement = regola->findElementByArray(selection);
    if( NULL == selectedElement ) {
        return error("Null element");
    }
    if( setGlobal ) {
        Config::saveBool(Config::KEY_XML_SAVE_SORTATTRIBUTES, setSortGlobal);
    }
    if( setLocal ) {
        regola->setSaveAttributesMethod(setSortLocal?Regola::SaveAttributesSortingAlphabetically:Regola::SaveAttributesNoSort);
    }
    regola->setIndentationForce(0);
    regola->setUseIndentation(true);
    QByteArray resultData = regola->writeMemory();
    QString regolaFromByteArray = QString::fromUtf8(resultData.data());
    QString regolaAsString = regola->getAsText();
    if( regolaFromByteArray != regolaAsString ) {
        return error(QString("String and ba differ.\nByteArray:\n%1\nString:\n%2\n").arg(regolaFromByteArray).arg(regolaAsString));
    }
    // Compare results.
    QString reference ;
    if(!readFromFile(fileCompare, reference) ) {
        return error(QString("unable to load compare file: '%1' ").arg(fileCompare));
    }
    // normalize cr
    reference = reference.replace("\r\n", "\n");
    regolaAsString = regolaAsString.replace("\r\n", "\n");

    if( reference != regolaAsString ) {
        return error(QString("String not expected.\nExpected:\n%1\nString:\n%2\n").arg(reference).arg(regolaAsString));
    }
    return true;
}

bool TestSortAttributes::saveDefaultWithSorted()
{
    _testName = "saveDefaultWithSorted";
    if(!baseSetup(UNSORTED, SORTED_NOINDENT, true, true, false, false )) {
        return false;
    }
    return true ;
}

bool TestSortAttributes::saveDefaultWithUnsorted()
{
    _testName = "saveDefaultWithUnsorted";
    if(!baseSetup(UNSORTED, UNSORTED_NOINDENT, true, false, false, false )) {
        return false;
    }
    return true ;
}

bool TestSortAttributes::noSortAndRegolaSort()
{
    _testName = "noSortAndRegolaSort";
    if(!baseSetup(UNSORTED, SORTED_NOINDENT, true, false, true, true )) {
        return false;
    }
    if(!baseSetup(UNSORTED, SORTED_NOINDENT, true, true, true, true )) {
        return false;
    }
    return true;
}

bool TestSortAttributes::noSortAndRegolaUnSort()
{
    _testName = "noSortAndRegolaUnSort";
    if(!baseSetup(UNSORTED, UNSORTED_NOINDENT, true, false, true, false )) {
        return false;
    }
    if(!baseSetup(UNSORTED, UNSORTED_NOINDENT, true, true, true, false )) {
        return false;
    }
    return true ;
}
