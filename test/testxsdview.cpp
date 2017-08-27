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

bool TestXSDView::openXsdViewerOutlineMode(MainWindow *window, TestXSDWindow *xsdEditor)
{
    QString xmlAsString = window->getRegola()->getAsText();
    xsdEditor->show();
    xsdEditor->setFileName("test");
    xsdEditor->setTitle("test");
    xsdEditor->setOutlineMode(true);
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

bool TestXSDView::compareOutlineStructure(XSDItem *item, Element *xml, const QString &path)
{
    XSchemaObject *object = item->item();
    if( NULL == object ) {
        return error(QString("Null items at: path:%1 expected:'%2'").arg(path).arg(xml->tag()));
    }
    const QString objectType = object->typeString();
    if( objectType != xml->tag() ) {
        return error(QString("items differ: path:%1 left:'%2', expected:'%3'").arg(path).arg(objectType).arg(xml->tag()));
    }
    if( object->name() != xml->getAttributeValue("name") ) {
        return error(QString("items name differ: path:%1 left:'%2', expected:'%3'").arg(object->name()).arg(objectType).arg(xml->getAttributeValue("name")));
    }
    RChildren *itemChildren = item->rChildren();
    const int xmlChildrenCount = xml->getChildItems()->size();
    if( xml->getChildItems()->size() != itemChildren->childrenSize() ) {
        return error( QString("element size differ, expected:%1 found:%2  path: %3").arg(xml->getChildItems()->size()).arg(itemChildren->childrenSize()).arg(path));
    }
    int itemsXMLCount = xml->getChildItemsCount();
    if( itemsXMLCount != xmlChildrenCount ) {
        return error(QString("children items differs: path:%1 left:%2, expected:%3").arg(path).arg(itemsXMLCount).arg(xmlChildrenCount));
    }
    for( int i = 0 ; i < xmlChildrenCount ; i ++ ) {
        QString itemPath = QString("%1/%2").arg(path).arg(i);
        if(!compareOutlineStructure(itemChildren->childAt(i)->item(), xml->getChildAt(i), itemPath)) {
            return false;
        }
    }
    return true;
}

class TestCP: public XSDRootChooseProvider {
    public:
    QString chooseRoot(QWidget *, QList<XSchemaElement*> )
    {
        return "theRoot";
    }
};

bool TestXSDView::loadAndCompareOutline(const QString &id, const QString &inputFilePath, const QString &fileReference)
{
    _subTestName = id ;
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(inputFilePath) ) {
        return error(QString("unable to load input file: '%1' ").arg(inputFilePath));
    }
    TestXSDWindow xsdEditor(app.data(), app.mainWindow()) ;
    // TODO: set outline mode
    if(!openXsdViewerOutlineMode(app.mainWindow(), &xsdEditor)) {
        return false;
    }
    xsdEditor.setChooseProvider(new TestCP());
    // finds the selection
    XSDItem *item = xsdEditor.root();
    if( NULL == item ) {
        return error(QString("Unable to select root item "));
    }

    MainWindow *windowReference = app.mainWindow()->loadFileAndReturnWindow(fileReference, true, MainWindow::OpenUsingNewWindow);
    if( NULL == windowReference ) {
        return error(QString("opening test file: '%1'").arg(fileReference));
    }
    Regola *regola = windowReference->getRegola();

    if(!compareOutlineStructure(item, regola->root(), "0")) {
        QString regolaOutline = regola->getAsText();
        QString msg = this->errorString();
        msg += "\n\n";
        msg += QString("Elab is:\n")+ item->dumpAsString(0);
        msg += "\n\n";
        msg += QString("Reference regola is:\n")+regolaOutline;
        msg += "\n\n";
        this->error(msg);
        //writeToFile("/tmp/1", msg);
        return false;
    }
    return true;
}

#define FILE_OUTLINE_BASE_PATH "../test/data/xsd/outline/"

struct TestXSDViewOutline {
    const char *id;
    const char *fileIn;
    const char *fileOut;
};

/* test to do:
 * elem con gerarchia ns/nons
 * elem ref con gerarchia ns/no inner/top
 * elm type sempl inner/top
 * elm type sempl restriz inner/top
 * elm type sempl estens inner/top
 * elm complex
 *   seq-elm
 *   choice
 *   all
 *   group
 *   any
 *  estensione
 * restrizione
 *
 * tutto, con ricorsione
 *
 */

static TestXSDViewOutline testDataOutlines []  = {

    //--------------------------------
    { "element", "element.xsd", "element.ref"},
    // simple type
    {"elementSimpleType", "elementst.xsd", "elementst.ref"},
    {"elementSimpleTypeNoPrefix", "elementstnop.xsd", "elementstnop.ref"},
    // complex element:
    // sequence elements
    {"elementSequence1", "sequence1.xsd", "sequence1.ref"},
    {"elementSequence", "sequence.xsd", "sequence.ref"},
    {"elementSequenceRef", "sequenceref.xsd", "sequenceref.ref"},
    {"elementSequenceRefType", "sequencereftype.xsd", "sequencereftype.ref"},
    {"elementInner", "elementinner.xsd", "elementinner.ref"},
    {"elementCExt", "elres.xsd", "elres.ref"},
    {"elementCRestr", "elrcrestr.xsd", "elcrestr.ref"},
    {"recursiveElements", "recursiveelements.xsd", "recursiveelements.ref"},
    {"recursiveElements1", "recursiveelements1.xsd", "recursiveelements1.ref"},
    {"recursiveTypes", "recursivetypes.xsd", "recursivetypes.ref"},
    // choice elements
    // group elements
    {"simpleGroup", "simplegroup.xsd", "simplegroup.ref"},
    {"simpleGroupRef", "group/simplegroupref.xsd", "group/simplegroupref.ref"},
    {"simpleGroupRefns", "group/simplegrouprefns.xsd", "group/simplegrouprefns.ref"},
    {"groupAll", "group/groupall.xsd", "group/groupall.ref"},
    {"groupchoice", "group/groupchoice.xsd", "group/groupchoice.ref"},
    {"groupchoiceRef", "group/groupchoiceref.xsd", "group/groupchoiceref.ref"},
    // all elements
    {"allElements", "allelements.xsd", "allelements.ref"},
    // restriction
    // extension

    // type ref complex
    {NULL, NULL, NULL}
};

bool TestXSDView::testOutline()
{
    _testName = "testOutline" ;

    TestXSDViewOutline *ptr = &testDataOutlines[0] ;
    while(ptr->id != NULL) {
        const QString fileIn = QString(FILE_OUTLINE_BASE_PATH)+QString(ptr->fileIn);
        const QString fileOut = QString(FILE_OUTLINE_BASE_PATH)+QString(ptr->fileOut);
        if(!loadAndCompareOutline(ptr->id, fileIn, fileOut)) {
            return false;
        }
        ptr++;
    }
    return true ;
}


bool TestXSDView::testFast()
{
    _testName = "testFast";
    //return testBaseElement();
    return testComplexExtension();
    //return testOutline();
    //return testExtension();
}
