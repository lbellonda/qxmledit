/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016-2018 by Luca Bellonda and individual contributors  *
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

#include "testeditenum.h"
#include "modules/xsd/xsdenumdialog.h"
#include "modules/xsd/choosefacetdialog.h"
#include "modules/xsd/xsdenumdialog.h"
#include "app.h"

#define BASE_PATH "../test/data/xsd/editenum/"

TestEditEnum::TestEditEnum()
{
}

TestEditEnum::~TestEditEnum()
{
}


bool TestEditEnum::testUnit()
{
    _testName = "testUnit";

    if(!testEditEnum()) {
        return false;
    }
    if(!testGetSameEditData()) {
        return false;
    }
    if(!testAdd()) {
        return false;
    }
    if(!testModify()) {
        return false;
    }
    if(!testDelete()) {
        return false;
    }
    if(!testMoveUp()) {
        return false;
    }
    if(!testMoveDown()) {
        return false;
    }
    return true;
}

//----

bool TestEditEnum::compareFacetsLists(QList<XSDFacet*> reference, QList<XSDFacet*> compare)
{
    if( reference.size() != compare.size() ) {
        return error(QString("Size differs: expected %1, found %2").arg(reference.size()).arg(compare.size()));
    }
    int iMax = reference.size();
    FORINT(i,iMax) {
        XSDFacet *f1 = reference.at(i);
        XSDFacet *f2 = compare.at(i);
        if(!f1->compareTo(f2)) {
            return error(QString("Differs at: %1, value 1:%2, value 2:%3").arg(i).arg(f1->toString()).arg(f2->toString()));
        }
    }
    return true ;
}

void TestEditEnum::emptyList(QList<XSDFacet*> reference)
{
    EMPTYPTRLIST(reference, XSDFacet);
}

bool TestEditEnum::testGetSameEditData()
{
    _testName = "testUnit/testGetSameEditData";
    {
        QList<XSDFacet*> reference ;
        XSDEnumDialog dialog( NULL, reference );
        dialog.accept();
        QList<XSDFacet*> compare = dialog.result() ;
        if(!compareFacetsLists(reference, compare)) {
            emptyList(compare);
            return false;
        }
        emptyList(compare);
    }
    {
        QList<XSDFacet*> reference ;
        XSDFacet f1(XSDFacet::Pattern, "aaa");
        reference << &f1 ;
        XSDEnumDialog dialog( NULL, reference );
        dialog.accept();
        QList<XSDFacet*> compare = dialog.result() ;
        if(!compareFacetsLists(reference, compare)) {
            emptyList(compare);
            return false;
        }
        emptyList(compare);
    }
    {
        QList<XSDFacet*> reference ;
        XSDFacet f1(XSDFacet::Pattern, "aaa");
        XSDFacet f2(XSDFacet::Enumeration, "123");
        f2.setAnnotation(new XSchemaAnnotation(NULL, NULL));
        XDocumentation *doc = new XDocumentation(NULL, NULL);
        doc->setContentString("<zzz/>");
        QDomDocument qdoc;
        QDomElement e = qdoc.createElement("zzz") ;
        doc->setContent(e);
        f2.annotation()->addXInfo(doc);
        reference << &f1 ;
        reference << &f2 ;
        XSDEnumDialog dialog( NULL, reference );
        dialog.accept();
        QList<XSDFacet*> compare = dialog.result() ;
        if(!compareFacetsLists(reference, compare)) {
            emptyList(compare);
            return false;
        }
        emptyList(compare);
    }
    return true;
}

bool TestEditEnum::testAdd()
{
    _testName = "testUnit/testAdd";
    {
        QList<XSDFacet*> reference ;
        XSDFacet f1(XSDFacet::Pattern, "aaa");
        reference << &f1 ;
        XSDEnumDialog dialog( NULL, reference );
        XSDFacet *fnew = new XSDFacet(XSDFacet::Enumeration, "bbb");
        XSDFacet f2(XSDFacet::Enumeration, "bbb");
        reference << &f2 ;
        dialog.addFacet(fnew);
        dialog.accept();
        QList<XSDFacet*> compare = dialog.result() ;
        if(!compareFacetsLists(reference, compare)) {
            emptyList(compare);
            return false;
        }
        emptyList(compare);
    }
    return true ;
}

bool TestEditEnum::testModify()
{
    _testName = "testUnit/testModify";
    QList<XSDFacet*> input ;
    XSDFacet f1(XSDFacet::Pattern, "aaa");
    input << &f1 ;
    XSDEnumDialog dialog( NULL, input );
    XSDFacet *modFacet = dialog.facetAt(0);
    modFacet->setType(XSDFacet::FractionDigits);
    modFacet->setValue("1");
    dialog.updateFacet(modFacet);
    //---
    XSDFacet f2(XSDFacet::FractionDigits, "1");
    QList<XSDFacet*> reference ;
    reference << &f2 ;
    dialog.accept();
    QList<XSDFacet*> compare = dialog.result() ;
    if(!compareFacetsLists(reference, compare)) {
        emptyList(compare);
        return false;
    }
    emptyList(compare);
    return true ;
}

bool TestEditEnum::testDelete()
{
    _testName = "testUnit/testDelete";
    {
        QList<XSDFacet*> source ;
        QList<XSDFacet*> reference ;
        XSDFacet f1(XSDFacet::Pattern, "aaa");
        XSDFacet f2(XSDFacet::Enumeration, "bbb");
        source << &f1 ;
        source << &f2 ;
        reference << &f1 ;
        XSDEnumDialog dialog( NULL, source );
        dialog.deleteFacetAt(1);
        dialog.accept();
        QList<XSDFacet*> compare = dialog.result() ;
        if(!compareFacetsLists(reference, compare)) {
            emptyList(compare);
            return false;
        }
        emptyList(compare);
    }
    return true ;
}

bool TestEditEnum::testMoveUp()
{
    _testName = "testUnit/testMoveUp";
    {
        QList<XSDFacet*> source ;
        QList<XSDFacet*> reference ;
        XSDFacet f1(XSDFacet::Pattern, "aaa");
        XSDFacet f2(XSDFacet::Enumeration, "bbb");
        source << &f1 ;
        source << &f2 ;
        reference << &f2 ;
        reference << &f1 ;
        XSDEnumDialog dialog( NULL, source );
        dialog.moveUp(1);
        dialog.accept();
        QList<XSDFacet*> compare = dialog.result() ;
        if(!compareFacetsLists(reference, compare)) {
            emptyList(compare);
            return false;
        }
        emptyList(compare);
    }
    {
        QList<XSDFacet*> source ;
        QList<XSDFacet*> reference ;
        XSDFacet f1(XSDFacet::Pattern, "aaa");
        XSDFacet f2(XSDFacet::Enumeration, "bbb");
        source << &f1 ;
        source << &f2 ;
        reference << &f1 ;
        reference << &f2 ;
        XSDEnumDialog dialog( NULL, source );
        dialog.moveUp(0);
        dialog.accept();
        QList<XSDFacet*> compare = dialog.result() ;
        if(!compareFacetsLists(reference, compare)) {
            emptyList(compare);
            return false;
        }
        emptyList(compare);
    }
    return true ;
}

bool TestEditEnum::testMoveDown()
{
    _testName = "testUnit/testMoveDown";
    {
        QList<XSDFacet*> source ;
        QList<XSDFacet*> reference ;
        XSDFacet f1(XSDFacet::Pattern, "aaa");
        XSDFacet f2(XSDFacet::Enumeration, "bbb");
        source << &f1 ;
        source << &f2 ;
        reference << &f2 ;
        reference << &f1 ;
        XSDEnumDialog dialog( NULL, source );
        dialog.moveDown(0);
        dialog.accept();
        QList<XSDFacet*> compare = dialog.result() ;
        if(!compareFacetsLists(reference, compare)) {
            emptyList(compare);
            return false;
        }
        emptyList(compare);
    }
    {
        QList<XSDFacet*> source ;
        QList<XSDFacet*> reference ;
        XSDFacet f1(XSDFacet::Pattern, "aaa");
        XSDFacet f2(XSDFacet::Enumeration, "bbb");
        source << &f1 ;
        source << &f2 ;
        reference << &f1 ;
        reference << &f2 ;
        XSDEnumDialog dialog( NULL, source );
        dialog.moveDown(1);
        dialog.accept();
        QList<XSDFacet*> compare = dialog.result() ;
        if(!compareFacetsLists(reference, compare)) {
            emptyList(compare);
            return false;
        }
        emptyList(compare);
    }
    return true ;
}


bool TestEditEnum::testEditEnum()
{
    _testName = "test";
    XSDFacet f1(XSDFacet::Pattern, "aaa");
    ChooseFacetDialog dlg( NULL, &f1);
    QRadioButton *b = dlg.findChild<QRadioButton*>("Length");
    if( NULL == b ) {
        return error("Null radio");
    }
    QLineEdit *value = dlg.findChild<QLineEdit*>("value");
    if( NULL == value ) {
        return error("Null value");
    }
    b->setChecked(true);
    b->click();
    value->setText("111");
    dlg.doIt();
    if( f1.value() != "111") {
        return error(QString("Value differs, expected '111' found:%1").arg(f1.value()));
    }
    if( f1.type() != XSDFacet::Length ) {
        return error(QString("Type differs, expected 'Length' found:%1").arg(f1.typeString()));
    }
    return true;
}

bool TestEditEnum::testEdit()
{
    _testName = "testEdit" ;
    // insert one elment
    if(!testIdentity()) {
        return false;
    }
    if(!testInsertSimple()) {
        return false;
    }
    // insert one elment and annotation
    if(!testInsertComplex()) {
        return false;
    }
    // insert more than one elment and annotation
    if(!testInsertComplexMultiple()) {
        return false;
    }
    //-----
    if(!testUpdateSimple()) {
        return false;
    }
    // update one elment and annotation
    if(!testUpdateComplex()) {
        return false;
    }
    // --
    if(!testDeleteSimple()) {
        return false;
    }
    return true;
}

bool TestEditEnum::testSkeleton( const QString &id, const QString &fileStart, const QString &fileFinal, bool (TestEditEnum::*apply)(MainWindow *, Element *))
{
    QString oldName = _testName ;
    _testName = _testName.append(id);
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(fileStart) ) {
        return error(QString("unable to load input file: '%1' ").arg(fileStart));
    }
    QList<int> selPath ;
    selPath << 1 << 2 << 0 ;
    Regola *regola = app.mainWindow()->getRegola();
    Element *selectedElement = NULL ;
    selectedElement = app.mainWindow()->getRegola()->findElementByArray(selPath);
    if(NULL == selectedElement) {
        return error("no element selected");
    }
    app.mainWindow()->getEditor()->setCurrentItem(selectedElement);
    if(!(this->*apply)(app.mainWindow(), selectedElement)) {
        return error("method returned false");
    }
    if(!compare(regola, "do", fileFinal)){
        return false;
    }
    app.mainWindow()->getRegola()->undo();
    if(!compare(regola, "op undo", fileStart)){
        return false;
    }
    app.mainWindow()->getRegola()->redo();
    if(!compare(regola, "op redo", fileFinal)){
        return false;
    }
    _testName = oldName ;
    return true;
}


#define FILE_INSERT_SIMPLE_START  BASE_PATH "insertSimpleStart.xml"
#define FILE_INSERT_SIMPLE_END  BASE_PATH "insertSimpleEnd.xml"
#define FILE_INSERT_COMPLEX_START  BASE_PATH "insertComplexStart.xml"
#define FILE_INSERT_COMPLEX_END  BASE_PATH "insertComplexEnd.xml"
#define FILE_INSERT_COMPLEXMULTIPLE_START  BASE_PATH "insertComplexStart.xml"
#define FILE_INSERT_COMPLEXMULTIPLE_END  BASE_PATH "insertComplexMultipleEnd.xml"
#define FILE_UPDATE_SIMPLE_START BASE_PATH "insertComplexEnd.xml"
#define FILE_UPDATE_SIMPLE_END BASE_PATH "updateSimpleEnd.xml"
#define FILE_UPDATE_COMPLEX_START BASE_PATH "insertComplexMultipleEnd.xml"
#define FILE_UPDATE_COMPLEX_END BASE_PATH "updateComplexEnd.xml"
#define FILE_DELETE_SIMPLE_START BASE_PATH "insertComplexMultipleEnd.xml"
#define FILE_DELETE_SIMPLE_END BASE_PATH "deleteSimpleEnd.xml"

#define FILE_IDENTITY_0_START   BASE_PATH "ident0start.xml"
#define FILE_IDENTITY_0_END   BASE_PATH "ident0end.xml"
#define FILE_IDENTITY_1_START   BASE_PATH "ident1start.xml"
#define FILE_IDENTITY_1_END   BASE_PATH "ident1end.xml"
#define FILE_IDENTITY_2_START   BASE_PATH "ident2start.xml"
#define FILE_IDENTITY_2_END   BASE_PATH "ident2end.xml"
#define FILE_IDENTITY_3_START   BASE_PATH "ident3start.xml"
#define FILE_IDENTITY_3_END   BASE_PATH "ident3end.xml"
#define FILE_IDENTITY_4_START   BASE_PATH "ident4start.xml"
#define FILE_IDENTITY_4_END   BASE_PATH "ident4end.xml"
#define FILE_IDENTITY_4_1_START   BASE_PATH "ident4.1start.xml"
#define FILE_IDENTITY_4_1_END   BASE_PATH "ident4.1end.xml"
#define FILE_IDENTITY_5_START   BASE_PATH "ident5start.xml"
#define FILE_IDENTITY_5_END   BASE_PATH "ident5end.xml"
#define FILE_IDENTITY_6_START   BASE_PATH "ident6start.xml"
#define FILE_IDENTITY_6_END   BASE_PATH "ident6end.xml"

bool TestEditEnum::applyTestInsertSimple(MainWindow * window, Element *selection)
{
    XSDHelper helper;
    XSDOperationParameters params;
    window->getRegola()->XSDSetNamespaceToParams(&params, selection);
    QList<Element*> inputElements = helper.findFacetsElements(selection, &params);
    QList<XSDFacet*> inputFacets = helper.fromElementsToFacets(inputElements, &params);
    XSDEnumDialog editEnumDialog( window, inputFacets);
    XSDFacet *newFacet = new XSDFacet(XSDFacet::Enumeration, "unduetre");
    editEnumDialog.addFacet(newFacet);
    QList<XSDFacet*> finalFacets = editEnumDialog.result();
    window->getEditor()->setFacets(selection, finalFacets);
    EMPTYPTRLIST(finalFacets, XSDFacet);
    EMPTYPTRLIST(inputFacets, XSDFacet);
    return true;
}

bool TestEditEnum::applyTestInsertComplex(MainWindow * window, Element *selection)
{
    XSDHelper helper;
    XSDOperationParameters params;
    window->getRegola()->XSDSetNamespaceToParams(&params, selection);
    QList<Element*> inputElements = helper.findFacetsElements(selection, &params);
    QList<XSDFacet*> inputFacets = helper.fromElementsToFacets(inputElements, &params);
    XSDEnumDialog editEnumDialog( window, inputFacets);
    XSDFacet *newFacet = new XSDFacet(XSDFacet::Enumeration, "unduetre");
    XSchemaAnnotation *annotation = new XSchemaAnnotation(NULL, NULL);
    XDocumentation * doc = new XDocumentation(NULL, NULL);
    doc->setLanguage("en");
    doc->setContentString("<a/>");
    annotation->addXInfo(doc);
    newFacet->setAnnotation(annotation);
    editEnumDialog.addFacet(newFacet);
    QList<XSDFacet*> finalFacets = editEnumDialog.result();
    window->getEditor()->setFacets(selection, finalFacets);
    EMPTYPTRLIST(finalFacets, XSDFacet);
    EMPTYPTRLIST(inputFacets, XSDFacet);
    return true;
}

bool TestEditEnum::applyTestInsertComplexMultiple(MainWindow * window, Element *selection)
{
    XSDHelper helper;
    XSDOperationParameters params;
    window->getRegola()->XSDSetNamespaceToParams(&params, selection);
    QList<Element*> inputElements = helper.findFacetsElements(selection, &params);
    QList<XSDFacet*> inputFacets = helper.fromElementsToFacets(inputElements, &params);
    XSDEnumDialog editEnumDialog( window, inputFacets);
    XSDFacet *newFacet = new XSDFacet(XSDFacet::Enumeration, "unduetre");
    XSchemaAnnotation *annotation = new XSchemaAnnotation(NULL, NULL);
    XDocumentation * doc = new XDocumentation(NULL, NULL);
    doc->setLanguage("en");
    doc->setContentString("<a/>");
    annotation->addXInfo(doc);
    newFacet->setAnnotation(annotation);
    editEnumDialog.addFacet(newFacet);
    newFacet = new XSDFacet(XSDFacet::Enumeration, "456");
    editEnumDialog.addFacet(newFacet);
    //--
    newFacet = new XSDFacet(XSDFacet::Enumeration, "x");
    annotation = new XSchemaAnnotation(NULL, NULL);
    doc = new XDocumentation(NULL, NULL);
    doc->setContentString("<y>t</y>");
    annotation->addXInfo(doc);
    newFacet->setAnnotation(annotation);
    editEnumDialog.addFacet(newFacet);
    //--
    QList<XSDFacet*> finalFacets = editEnumDialog.result();
    window->getEditor()->setFacets(selection, finalFacets);
    EMPTYPTRLIST(finalFacets, XSDFacet);
    EMPTYPTRLIST(inputFacets, XSDFacet);
    return true;
}

bool TestEditEnum::applyIdentity(MainWindow * window, Element *selection)
{
    XSDHelper helper;
    XSDOperationParameters params;
    window->getRegola()->XSDSetNamespaceToParams(&params, selection);
    QList<Element*> inputElements = helper.findFacetsElements(selection, &params);
    QList<XSDFacet*> inputFacets = helper.fromElementsToFacets(inputElements, &params);
    XSDEnumDialog editEnumDialog( window, inputFacets);
    QList<XSDFacet*> finalFacets = editEnumDialog.result();
    window->getEditor()->setFacets(selection, finalFacets);
    EMPTYPTRLIST(finalFacets, XSDFacet);
    EMPTYPTRLIST(inputFacets, XSDFacet);
    return true;
}

bool TestEditEnum::applyTestUpdateSimple(MainWindow * window, Element *selection)
{
    XSDHelper helper;
    XSDOperationParameters params;
    window->getRegola()->XSDSetNamespaceToParams(&params, selection);
    QList<Element*> inputElements = helper.findFacetsElements(selection, &params);
    QList<XSDFacet*> inputFacets = helper.fromElementsToFacets(inputElements, &params);
    XSDEnumDialog editEnumDialog( window, inputFacets);
    XSDFacet *facet = editEnumDialog.facetAt(0);
    facet->setType(XSDFacet::MinInclusive);
    facet->setValue("5");
    editEnumDialog.updateFacet(facet);
    QList<XSDFacet*> finalFacets = editEnumDialog.result();
    window->getEditor()->setFacets(selection, finalFacets);
    EMPTYPTRLIST(finalFacets, XSDFacet);
    EMPTYPTRLIST(inputFacets, XSDFacet);
    return true;
}

bool TestEditEnum::applyTestUpdateComplex(MainWindow * window, Element *selection)
{
    XSDHelper helper;
    XSDOperationParameters params;
    window->getRegola()->XSDSetNamespaceToParams(&params, selection);
    QList<Element*> inputElements = helper.findFacetsElements(selection, &params);
    QList<XSDFacet*> inputFacets = helper.fromElementsToFacets(inputElements, &params);
    XSDEnumDialog editEnumDialog( window, inputFacets);
    XSDFacet *facet = editEnumDialog.facetAt(0);
    facet->setType(XSDFacet::MinInclusive);
    facet->setValue("5");
    editEnumDialog.updateFacet(facet);

    facet = editEnumDialog.facetAt(1);
    XSchemaAnnotation *annotation = new XSchemaAnnotation(NULL, NULL);
    XDocumentation * doc = new XDocumentation(NULL, NULL);
    doc->setLanguage("es");
    doc->setContentString("<z/>");
    annotation->addXInfo(doc);
    facet->setAnnotation(annotation);
    editEnumDialog.updateFacet(facet);

    QList<XSDFacet*> finalFacets = editEnumDialog.result();
    window->getEditor()->setFacets(selection, finalFacets);
    EMPTYPTRLIST(finalFacets, XSDFacet);
    EMPTYPTRLIST(inputFacets, XSDFacet);
    return true;
}


bool TestEditEnum::applyTestDeleteSimple(MainWindow * window, Element *selection)
{
    XSDHelper helper;
    XSDOperationParameters params;
    window->getRegola()->XSDSetNamespaceToParams(&params, selection);
    QList<Element*> inputElements = helper.findFacetsElements(selection, &params);
    QList<XSDFacet*> inputFacets = helper.fromElementsToFacets(inputElements, &params);
    XSDEnumDialog editEnumDialog( window, inputFacets);
    //--
    editEnumDialog.deleteFacetAt(1);
    //--
    XSDFacet *facet = editEnumDialog.facetAt(0);
    facet->setType(XSDFacet::MinInclusive);
    facet->setValue("5");
    editEnumDialog.updateFacet(facet);
    //--
    QList<XSDFacet*> finalFacets = editEnumDialog.result();
    window->getEditor()->setFacets(selection, finalFacets);
    EMPTYPTRLIST(finalFacets, XSDFacet);
    EMPTYPTRLIST(inputFacets, XSDFacet);
    return true;
}

bool TestEditEnum::testInsertSimple()
{
    _testName = "testInsertSimple";
    if(!testSkeleton( "testInsertSimple", FILE_INSERT_SIMPLE_START, FILE_INSERT_SIMPLE_END, &TestEditEnum::applyTestInsertSimple) ) {
        return false;
    }
    return true;
}

bool TestEditEnum::testInsertComplex()
{
    _testName = "testInsertComplex";
    if(!testSkeleton( "testInsertComplex", FILE_INSERT_COMPLEX_START, FILE_INSERT_COMPLEX_END, &TestEditEnum::applyTestInsertComplex) ) {
        return false;
    }
    return true;
}

bool TestEditEnum::testInsertComplexMultiple()
{
    _testName = "testInsertComplexMultiple";
    if(!testSkeleton( "testInsertComplexMultiple", FILE_INSERT_COMPLEXMULTIPLE_START, FILE_INSERT_COMPLEXMULTIPLE_END, &TestEditEnum::applyTestInsertComplexMultiple) ) {
        return false;
    }
    return true;
}

bool TestEditEnum::testUpdateSimple()
{
    _testName = "testUpdateSimple";
    if(!testSkeleton( "testUpdateSimple", FILE_UPDATE_SIMPLE_START, FILE_UPDATE_SIMPLE_END, &TestEditEnum::applyTestUpdateSimple) ) {
        return false;
    }
    return true ;
}

bool TestEditEnum::testUpdateComplex()
{
    _testName = "testUpdateComplex";
    if(!testSkeleton( "testUpdateComplex", FILE_UPDATE_COMPLEX_START, FILE_UPDATE_COMPLEX_END, &TestEditEnum::applyTestUpdateComplex) ) {
        return false;
    }
    return true ;
}

bool TestEditEnum::testDeleteSimple()
{
    _testName = "testDeleteSimple";
    if(!testSkeleton( "testDeleteSimple", FILE_DELETE_SIMPLE_START, FILE_DELETE_SIMPLE_END, &TestEditEnum::applyTestDeleteSimple) ) {
        return false;
    }
    return true ;
}

bool TestEditEnum::testIdentity()
{
    _testName = "testIdentity";
    // as is
    if(!testSkeleton( "0", FILE_IDENTITY_0_START, FILE_INSERT_SIMPLE_END, &TestEditEnum::applyIdentity) ) {
        return false;
    }
    // attributes other ns
    if(!testSkeleton( "1", FILE_IDENTITY_1_START, FILE_IDENTITY_1_END, &TestEditEnum::applyIdentity) ) {
        return false;
    }
    // fixed, status
    if(!testSkeleton( "2", FILE_IDENTITY_2_START, FILE_IDENTITY_2_END, &TestEditEnum::applyIdentity) ) {
        return false;
    }
    // other objects
    if(!testSkeleton( "3", FILE_IDENTITY_3_START, FILE_IDENTITY_3_END, &TestEditEnum::applyIdentity) ) {
        return false;
    }
    // annotations only attributes
    if(!testSkeleton( "4", FILE_IDENTITY_4_START, FILE_IDENTITY_4_END, &TestEditEnum::applyIdentity) ) {
        return false;
    }
    // annotations inner
    if(!testSkeleton( "4.1", FILE_IDENTITY_4_1_START, FILE_IDENTITY_4_1_END, &TestEditEnum::applyIdentity) ) {
        return false;
    }
    // mixed undo
    if(!testSkeleton( "5", FILE_IDENTITY_5_START, FILE_IDENTITY_5_END, &TestEditEnum::applyIdentity) ) {
        return false;
    }
    /*// undo with annotation, mixed and all (every) different types
    if(!testSkeleton( "6", FILE_IDENTITY_6_START, FILE_IDENTITY_6_END, &TestEditEnum::applyIdentity) ) {
        return false;
    }*/
    return true;
}

bool TestEditEnum::testFast()
{
    _testName = "testFast";
    // annotations
    if(!testDeleteSimple()) {
        return false;
    }
    return true;
}
