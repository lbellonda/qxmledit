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
#include "testhelpers/xsd/testxsdprintinfo.h"
#include "xsdeditor/xsdwindow.h"
#include "modules/services/systemservices.h"

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
    return testImagesExtInt();
}

bool TestXSDView::testPrintPagination()
{
    _testName = "testPrintPagination";
    TestXSDPrintInfo info;
    if(!info.testAll()) {
        return error(QString("step:%1 cause:%2").arg(info.stepName).arg(info.errorMessage));
    }
    return true;
}

bool TestXSDView::testPrintCheckHTML()
{
    _testName = "testPrintCheckHTML";
    {
        XSDPrintInfo info;
        QString strHeader("<html>");
        if(info.checkHeader(strHeader)) {
            return error(QString("1: expected false"));
        }
    }
    {
        XSDPrintInfo info;
        QString strHeader("<body>");
        if(info.checkHeader(strHeader)) {
            return error(QString("2: expected false"));
        }
    }
    {
        XSDPrintInfo info;
        QString strHeader("</body>");
        if(info.checkHeader(strHeader)) {
            return error(QString("3: expected false"));
        }
    }
    {
        XSDPrintInfo info;
        QString strHeader("</html>");
        if(info.checkHeader(strHeader)) {
            return error(QString("4: expected false"));
        }
    }
    {
        XSDPrintInfo info;
        QString strHeader("<html><body></body></html>");
        if(info.checkHeader(strHeader)) {
            return error(QString("5: expected false"));
        }
    }
    {
        XSDPrintInfo info;
        QString strHeader("<httml><btody></bgody></hthml>");
        if(!info.checkHeader(strHeader)) {
            return error(QString("6: expected true"));
        }
    }
    return true;
}

#define FILE_REPORT_INPUT    "../test/data/xsd/view/report.xsd"
#define FILE_REPORT_HTML    "../test/data/xsd/view/report.html"

QString TestXSDView::cleanAttribute(const QString &html, const QString& searchTerm, bool &isChanged)
{
    const int indexStart = html.indexOf(searchTerm);
    if(indexStart <= 0 ) {
        return html ;
    }
    const int firstCommaIndex = html.indexOf("'", indexStart+searchTerm.length() );
    if(firstCommaIndex <= 0 ) {
        return html ;
    }
    const int lastCommaIndex = html.indexOf("'", firstCommaIndex+1 );
    if(lastCommaIndex <= 0 ) {
        return html;
    }
    isChanged = true ;
    QString result = html.left(indexStart);
    result += html.mid(lastCommaIndex+1);
    return result ;
}

QString TestXSDView::cleanComment(const QString &text, const QString& commentStart, const QString& commentEnd)
{
    const int posRemStart = text.indexOf(commentStart);
    const int posRemEnd = text.indexOf(commentEnd, posRemStart+1);
    if( ( posRemStart < 0 ) || (posRemEnd < 0 ) || (posRemStart >= posRemEnd) ) {
        QString msg = QString("Unable to find rem: %3 %4 %1 %2")
                     .arg(posRemStart).arg(posRemEnd)
                     .arg(commentStart).arg(commentEnd);
        error(msg);
        return "";
    }
    QString result = text.left(posRemStart);
    result += text.right(text.length()-posRemEnd-QString(commentEnd).length());
    return result;
}

bool TestXSDView::cleanHTML(const QString &html, QString &result)
{
    const int firstBody = html.indexOf("<body>");
    const int lastBody = html.indexOf("</body>");
    if( ( firstBody < 0 ) || (lastBody < 0 ) || (firstBody >= lastBody) ) {
        return error(QString("Unable to find body: %1 %2").arg(firstBody).arg(lastBody));
    }
    const QString strBody = html.mid(firstBody, lastBody-firstBody+7);
    const QString ImgIntro = "<img class='diagramImage' src='data:image/png;base64,";
    const int imageIndex = strBody.indexOf(ImgIntro);
    if(imageIndex <= 0 ) {
        return error(QString("Unable to find image index"));
    }
    const int lastImageIndex = strBody.indexOf("'", imageIndex+ImgIntro.length() );
    if(lastImageIndex <= 0 ) {
        return error(QString("Unable to find final image index"));
    }
    QString cdt = strBody.left(imageIndex);
    cdt += strBody.mid(lastImageIndex, strBody.length()-lastImageIndex);
    //
    cdt = cleanComment(cdt, "<!--PS0-->", "<!--PE0-->");
    if(isError()) {
        return false;
    }
    cdt = cleanComment(cdt, "<!--PS1-->", "<!--PE1-->");
    if(isError()) {
        return false;
    }
    // normalize
    result = result.replace("\r\n", "\n");
    bool isChanged = false;
    int index = 0 ;
    do {
        index ++ ;
        isChanged = false ;
        result = cleanAttribute(result, "<a name=", isChanged);
        result = cleanAttribute(result, "<a href=", isChanged);
        //printf("loop:%d %d\n", index, isChanged);
    } while(isChanged && (index < 10000));
    return true;
}

bool TestXSDView::loadAndComparePrint(const QString &id, const QString &inputFilePath, const QString &fileReference)
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
    if(!openXsdViewerOutlineMode(app.mainWindow(), &xsdEditor)) {
        return false;
    }
    Config::saveBool(Config::KEY_XSD_REPORT_USEGRAPHVIZ, false);
    Config::saveBool(Config::KEY_XSD_REPORT_EMBEDIMAGES, true);
    xsdEditor.setChooseProvider(new TestCP());
    // finds the selection
    QString html = xsdEditor.getAsHTML(false);
    QString candidate ;
    if(!cleanHTML(html, candidate)) {
        return false;
    }

    // compare candidate and reference
    QString referenceOrig ;
    if(!readFromFile(fileReference, referenceOrig)) {
        return error(QString("Unable to read reference: %1").arg(fileReference));
    }
    QString reference ;
    if(!cleanHTML(referenceOrig, reference)) {
        return false;
    }
    // normalize cr
    reference = reference.replace("\r\n", "\n");
    if( candidate != reference ) {
        QString msg = "Candidate differs:  ";
        msg += "\n\n";
        msg += QString("Candidate is:\n")+ candidate;
        msg += "\n\n";
        msg += QString("Reference is:\n")+reference;
        msg += "\n\n";
        //writeToFile("/tmp/1", msg);
        return error(msg);
    }
    return true;
}

bool TestXSDView::testUnitHTML()
{
    _subTestName = "testUnitHTML";
    XSDPrint xsdPrint;
    const QString expected = QString(" ")+QString(" ")+QString(" ")+QString(" ") ;
    const QString result = xsdPrint.indentLine(4);
    if(!assertEquals("indent", expected, result)) {
        return false;
    }
    return true ;
}

bool TestXSDView::testUnitPrint()
{
    _subTestName = "testUnitPrint";
    bool isChanged = false ;
    QString found = cleanAttribute("aaa", "<a href=", isChanged);
    if(isChanged) {
        return error(QString("Error found on:%1").arg(found));
    }
    found = cleanAttribute("xxx<a href='cccc'>d</a><a href='ddd'>bb</a>", "<a href=", isChanged);
    if(!isChanged) {
        return error(QString("Error not found on:%1").arg(found));
    }
    if( found != "xxx>d</a><a href='ddd'>bb</a>") {
        return error(QString("Error not equal on:%1").arg(found));
    }
    found = cleanAttribute(found, "<a href=", isChanged);
    if(!isChanged) {
        return error(QString("Error not found 2 on:%1").arg(found));
    }
    if( found != "xxx>d</a>>bb</a>") {
        return error(QString("Error not equal 2 on:%1").arg(found));
    }
    return true;
}

bool TestXSDView::testPrintHTML()
{
    _testName = "testPrintHTML";
    if(!testUnitPrint()) {
        return false;
    }
    if(!testUnitHTML()) {
        return false;
    }
    if(!loadAndComparePrint("html1", FILE_REPORT_INPUT, FILE_REPORT_HTML)) {
        return false;
    }
    if(!testArgsDotViz()) {
        return false;
    }
    if(!testPathDotViz()) {
        return false;
    }
    if(!testImagesExtInt()) {
        return false;
    }

    return true ;
}

bool TestXSDView::testPrint()
{
    _testName = "testPrint";
    if(!testPrintPagination()) {
        return false;
    }
    if(!testPrintCheckHTML()) {
        return false;
    }
    if(!testPrintHTML()) {
        return false;
    }
    return true;
}

bool TestXSDView::testArgsDotViz()
{
    _testName = "testArgsDotViz";
    App app;
    if(!app.init()) {
        return error("init app");
    }
    XSDWindow wnd(app.data());
    XSDPrint print(&wnd, app.data());
    QStringList expectedArgs;
    expectedArgs << "-Tpng" << "f1" << "-o" << "f2";
    QStringList args = print.createDotVizCommandLine("f1", "f2");
    if(!compareStringList("dotviz command line failed", expectedArgs, args)) {
        return false;
    }
    return true;
}

bool TestXSDView::checkSettingsAndExpected(App &app, const QString &msg, const bool useDefault, const QString &path, const QString &expectedPath)
{
    Config::saveBool(Config::KEY_XSD_REPORT_OVERRIDEGRAPHVIZ, useDefault);
    Config::saveString(Config::KEY_XSD_REPORT_PATHGRAPHVIZ, path);
    XSDWindow wnd(app.data());
    XSDPrint print(&wnd, app.data());
    const QString result = print.dotVizPath();
    return assertEquals(msg, expectedPath, result);
}

bool TestXSDView::testPathDotViz()
{
    _testName = "testPathDotViz";
    QString defaultPath("dot");
    if(SystemServices::isWindows()) {
        defaultPath ="dot.exe";
    }
    // this one needed for config
    App app;
    if(!app.init()) {
        return error("init app");
    }
    if(!checkSettingsAndExpected(app, "1", true, "", defaultPath)) {
        return false;
    }
    if(!checkSettingsAndExpected(app, "2", true, "ZZZZ", "ZZZZ")) {
        return false;
    }
    if(!checkSettingsAndExpected(app, "3", false, "ppp", defaultPath)) {
        return false;
    }
    return true;
}

bool TestXSDView::testImagesExtInt()
{
    _testName = "testImagesExtInt" ;
    const QString inputFilePath(FILE_REPORT_INPUT);
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(inputFilePath) ) {
        return error(QString("unable to load input file: '%1' ").arg(inputFilePath));
    }
    if(!testForImage("E0", app, false, false)) {
        return false;
    }
    if(!testForImage("E1", app, false, true)) {
        return false;
    }
    if(!testForImage("I0", app, true, false)) {
        return false;
    }
    if(!testForImage("I1", app, true, true)) {
        return false;
    }
    return true;
}

bool TestXSDView::testForImage(const QString &id, App &app, const bool isEmbedded, const bool isDotViz)
{
    _subTestName = "testImagesExtInt" ;
    Config::saveBool(Config::KEY_XSD_REPORT_USEGRAPHVIZ, isDotViz);
    Config::saveBool(Config::KEY_XSD_REPORT_EMBEDIMAGES, isEmbedded);
    TestXSDWindow xsdEditor(app.data(), app.mainWindow()) ;
    if(!openXsdViewerOutlineMode(app.mainWindow(), &xsdEditor)) {
        return false;
    }
    xsdEditor.setChooseProvider(new TestCP());
    QString html = xsdEditor.getAsHTML(false);
    const QString toSearch("<img class='diagramImage' src='data:image/png;base64,");
    if(isEmbedded) {
        if(html.indexOf(toSearch)<=0) {
            return error(QString("%1 not found embedded image").arg(id));
        }
    } else {
        if(html.indexOf(toSearch)>=0) {
            return error(QString("%1 found embedded image").arg(id));
        }
        const QString toSearch2("<img class='diagramImage' src='");
        if(html.indexOf(toSearch2)<0) {
            return error(QString("%1 not found image").arg(id));
        }
    }
    return true ;
}

/*
void TestXSDView::debugFile(const QString &data)
{
    writeToFile("/tmp/1", data);
}
*/
