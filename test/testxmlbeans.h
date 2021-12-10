/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2021 by Luca Bellonda and individual contributors       *
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

#ifndef TESTXMLBEANS_H
#define TESTXMLBEANS_H

#include "testbase.h"
#include "mainwndcontroller.h"

class TestXMLBeans : public TestBase, XSDTopElementChooser, XMLVsXSDFactory
{
    bool testXML2XSDParameters();
    bool testXML2XSDLoadXSD();
    bool testXML2XSDConfig();
    bool testXML2XSDConfigCheck();
    bool testXML2XSD();
    bool testXML2XSDRunInner(const QString &code,
                             const QString &fileInput, const bool expectedResult,
                             const bool errorInExecution, const QString &expectedDataFile);
    //
    bool testXSD2XML();
    bool testXSD2XMLParameters();
    bool testXSD2XMLRun();
    bool testXSD2XMLConfig();
    bool testXSD2XMLConfigCheck();
    bool testXSD2XMLRunInner(const QString &code,
                             const QString &fileInput, const bool expectedResult,
                             const QString &elementChosen, const bool errorInExecution,
                             const QString &expectedDataFile);


    QString _factoryChooseElement ;
    QString _factoryFileToRead ;
    bool _factoryErrorInExecution ;
public:
    TestXMLBeans();
    virtual ~TestXMLBeans();

    bool testFast();
    bool testUnit();

    virtual QString selectTopLevelSchemaElement(Regola *regola);
    virtual XMLToXSD* newXML2XSD(ApplicationData *appData);
    virtual XSDToXML* newXSD2XML(ApplicationData *appData);

};

#endif // TESTXMLBEANS_H
