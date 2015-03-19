/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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


#include "testxsdview.h"
#include "helpers/comparexsdwithxml.h"

TestXSDView::TestXSDView()
{
}

TestXSDView::~TestXSDView()
{
}

bool TestXSDView::openXsdViewer(MainWindow *window, TestXSDWindow *xsdEditor)
{
    QString xmlAsString = window->getRegola()->getAsText();
    xsdEditor->show();
    xsdEditor->setFileName("test");
    xsdEditor->setTitle("test");
    xsdEditor->loadStringImmediate(xmlAsString);
    if( xsdEditor->isInError() ) {
        return error("data not loaded in xsd viwever");
    }
    return true;
}

bool TestXSDView::doTest(const QString &inputFilePath, const QString &resultFilePath, const QList<int> &selectionPath)
{
    /*crea xsd;
    load file;
    seleziona oggetto per nome;
    zoomma;
    prendi albero items;
    chiama confronto usando la classe helper;*/
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(inputFilePath) ) {
        return error(QString("unable to load input file: '%1' ").arg(inputFilePath));
    }
    TestXSDWindow xsdEditor(app.data(), app.mainWindow()) ;
    if(!openXsdViewer(app.mainWindow(), &xsdEditor)) {
        return false;
    }
    // finds the selection
    XSDItem *item = xsdEditor.selectItemByIndexAndZoom(selectionPath);
    if( NULL == item ) {
        return error(QString("Unable to select item at path:'%1'").arg(listIntToString(selectionPath)));
    }

    // compare using compare functions
    CompareXSDWithXML compareXSDWithXML;

    if(!compareXSDWithXML.compareTreeWithFile(resultFilePath, item)) {
        QString treeDumped = compareXSDWithXML.dumpItemTree(item, 0);
        return error(QString("Compare failed:").append(compareXSDWithXML.errorString()).append(">>").append(treeDumped));
    }
    return true;
}

//------------------------------------------------------------
#define FILE_BASE_INPUT "../test/data/xsd/types/base.xsd"
#define FILE_BASE_RESULT "../test/data/xsd/types/verify/base.xml"
//-
#define FILE_EXTREF_INPUT "../test/data/xsd/types/ref_element.xsd"
#define FILE_EXTREF_RESULT "../test/data/xsd/types/verify/ref_element.xml"

#define FILE_EXT_INPUT "../test/data/xsd/types/ext_complex.xsd"
#define FILE_EXT_RESULT "../test/data/xsd/types/verify/ext_complex.xml"

#define FILE_RESTR_INPUT    "../test/data/xsd/types/type_restriction.xsd"
#define FILE_RESTR_RESULT   "../test/data/xsd/types/verify/type_restriction.xml"

#define FILE_ELEMENT_INPUT "../test/data/xsd/types/element.xsd"
#define FILE_ELEMENT_RESULT "../test/data/xsd/types/verify/element.xml"


#define FILE_TYPESWITHELEMENTSREF_INPUT "../test/data/xsd/types/element_type.xsd"
#define FILE_TYPESWITHELEMENTSREF_RESULT    "../test/data/xsd/types/verify/element_type.xml"
//------------------------------------------------------------
#define FILE_GROUPS_INPUT "../test/data/xsd/types/groups.xsd"
#define FILE_REFAGROUP_RESULT "../test/data/xsd/types/verify/agroup.xml"
#define FILE_REFGROUP_RESULT "../test/data/xsd/types/verify/refgroup.xml"
#define FILE_AGROUP_RESULT "../test/data/xsd/types/verify/agroup.xml"
#define FILE_REFGROUP_RESULT "../test/data/xsd/types/verify/refgroup.xml"
#define FILE_GROUPS_RESULT  "../test/data/xsd/types/verify/groups.xml"
#define FILE_REFATTRIBUTE_RESULT "../test/data/xsd/types/verify/ref_attribute.xml"
#define FILE_ATTRIBUTE_RESULT "../test/data/xsd/types/verify/attribute.xml"

bool TestXSDView::testBaseElement()
{
    _testName = "testBaseElement";
    QList<int> selectionPath ;
    selectionPath << 0 ;
    selectionPath << 2;
    if( !doTest(FILE_BASE_INPUT, FILE_BASE_RESULT, selectionPath) ) {
        return false;
    }
    return true;
}

bool TestXSDView::testElementInf()
{
    _testName = "testElementInf";
    QList<int> selectionPath ;
    selectionPath << 0 ;
    selectionPath << 2;
    if( !doTest(FILE_ELEMENT_INPUT, FILE_ELEMENT_RESULT, selectionPath) ) {
        return false;
    }
    return true;
}

bool TestXSDView::testRefElement()
{
    _testName = "testRefElement";
    QList<int> selectionPath ;
    selectionPath << 0 ;
    selectionPath << 2;
    if( !doTest(FILE_EXTREF_INPUT, FILE_EXTREF_RESULT, selectionPath) ) {
        return false;
    }
    return true;
}

bool TestXSDView::testTypeWithElementsRef()
{
    _testName = "testTypeWithElementsRef";
    QList<int> selectionPath ;
    selectionPath << 0 ;
    selectionPath << 2;
    if( !doTest(FILE_TYPESWITHELEMENTSREF_INPUT, FILE_TYPESWITHELEMENTSREF_RESULT, selectionPath) ) {
        return false;
    }
    return true;
}


bool TestXSDView::testReferenceAttribute()
{
    _testName = "testReferenceAttribute";
    QList<int> selectionPath ;
    selectionPath << 0 ;
    selectionPath << 2;
    if( !doTest(FILE_GROUPS_INPUT, FILE_REFATTRIBUTE_RESULT, selectionPath) ) {
        return false;
    }
    return true;
}

bool TestXSDView::testReferenceAttributeGroup()
{
    _testName = "testReferenceAttributeGroup";
    QList<int> selectionPath ;
    selectionPath << 0 ;
    selectionPath << 2;
    if( !doTest(FILE_GROUPS_INPUT, FILE_REFAGROUP_RESULT, selectionPath) ) {
        return false;
    }
    return true;
}

bool TestXSDView::testAttribute()
{
    _testName = "testAttribute";
    QList<int> selectionPath ;
    selectionPath << 0 ;
    selectionPath << 2;
    if( !doTest(FILE_GROUPS_INPUT, FILE_ATTRIBUTE_RESULT, selectionPath) ) {
        return false;
    }
    return true;
}

bool TestXSDView::testAttributeGroup()
{
    _testName = "testAttributeGroup";
    QList<int> selectionPath ;
    selectionPath << 0 ;
    selectionPath << 2;
    if( !doTest(FILE_GROUPS_INPUT, FILE_AGROUP_RESULT, selectionPath) ) {
        return false;
    }
    return true;
}

bool TestXSDView::testReferenceGroups()
{
    _testName = "testReferenceGroups";
    QList<int> selectionPath ;
    selectionPath << 0 ;
    selectionPath << 2;
    if( !doTest(FILE_GROUPS_INPUT, FILE_REFGROUP_RESULT, selectionPath) ) {
        return false;
    }
    return true;
}

bool TestXSDView::testGroups()
{
    _testName = "testGroups";
    QList<int> selectionPath ;
    selectionPath << 0 ;
    selectionPath << 2;
    if( !doTest(FILE_GROUPS_INPUT, FILE_GROUPS_RESULT, selectionPath) ) {
        return false;
    }
    return true;
}

bool TestXSDView::testComplexExtension()
{
    _testName = "testComplexExtension";
    QList<int> selectionPath ;
    selectionPath << 0 ;
    selectionPath << 1;
    if( !doTest(FILE_EXT_INPUT, FILE_EXT_RESULT, selectionPath) ) {
        return false;
    }
    return true;
}

bool TestXSDView::testComplexRestriction()
{
    _testName = "testComplexRestriction";
    QList<int> selectionPath ;
    selectionPath << 0 ;
    selectionPath << 4;
    if( !doTest(FILE_RESTR_INPUT, FILE_RESTR_RESULT, selectionPath) ) {
        return false;
    }
    return true;
}

//---------------------------------------


bool TestXSDView::testBase()
{
    if(! testBaseElement() ) {
        return false;
    }
    return true;
}


bool TestXSDView::testElement()
{
    if(!testElementInf()) {
        return false ;
    }
    if(!testRefElement()) {
        return false ;
    }
    return true;
}

bool TestXSDView::testType()
{
    /*if(! testSimpleType() ) {
        return false;
    }*/
    if(!testTypeWithElementsRef() ) {
        return false;
    }
    return true;
}

bool TestXSDView::testReference()
{
    /*if(! testReferenceElement() ) {
        return false;
    }*/
    if(! testReferenceAttribute() ) {
        return false;
    }
    /*if(! testReferenceAttributeGroup() ) {
        return false;
    }
    if(! testReferenceGroups() ) {
        return false;
    }*/
    return true;
}

bool TestXSDView::testExtension()
{
    if(! testComplexExtension() ) {
        return false;
    }
    return true;
}

bool TestXSDView::testRestriction()
{
    if(! testComplexRestriction() ) {
        return false;
    }
    return true;
}

/*
bool TestXSDView::testAttributeAndGroups()
{
    if(! testAttribute() ) {
        return false;
    }
    if(! testAttributeGroup() ) {
        return false;
    }
    if(!testGroups() ) {
        return false;
    }
    return true;
}*/
