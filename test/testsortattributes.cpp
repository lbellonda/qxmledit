/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2016 by Luca Bellonda and individual contributors  *
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

#define SORT_SORTALLINPUT BASE_PATH "sort_all_input.xml"

TestSortAttributes::TestSortAttributes()
{
}

TestSortAttributes::~TestSortAttributes()
{
}

bool TestSortAttributes::testFast()
{
    return testUnitSort() ;
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

//-----------------------------------------------------------------------------

bool TestSortAttributes::testUnitSort()
{
    _testName = "testUnitSort";
    // test for effective attribute operations.
    if(!testUnitOperation()) {
        return false;
    }
    if(!testSortUnsorted()) {
        return false;
    }
    if(!testSortSorted()) {
        return false;
    }
    if(!testSortUndo()) {
        return false;
    }
    return true;
}

bool TestSortAttributes::addAttributeVerify(Element *el, const QString &aName, const QString &aValue)
{
    el->addAttribute(aName, aValue);
    Attribute *attr = el->attributes.at(el->attributes.size()-1);
    if( NULL == attr ) {
        return error(QString("addAttributeVerify: for attr:'%1', expected last, but was not").arg(aName));
    }
    if( attr->name != aName ) {
        return error(QString("addAttributeVerify: for attr:'%1', expected name, but was:'%2'").arg(aName).arg(attr->name));
    }
    if( attr->value != aValue ) {
        return error(QString("addAttributeVerify: for attr:'%1', expected value:'%2', but was:'%3'").arg(aName).arg(aValue).arg(attr->value));
    }
    return true ;
}


bool TestSortAttributes::verifyAttributesList(Element *el, QStringList values)
{
    if( el->attributes.size() != values.count()) {
        return error(QString("verifyAttributesList: expected count:'%1', found %2").arg(values.count()).arg(el->attributes.size()));
    }
    int max = values.count();
    FORINT( index, max ) {
        Attribute *attr = el->attributes.at(index);
        QString expected = values.at(index);
        if( attr->name != expected ) {
            return error(QString("verifyAttributesList: for attr:'%1'', expected value:'%2'").arg(attr->name).arg(expected));
        }

    }
    return true ;
}


bool TestSortAttributes::testUnitOperation()
{
    _testName = "testUnitOperation";
    Element el1("a","", NULL);
    if(!addAttributeVerify(&el1, "a", "a-val")) {
        return false;
    }
    if(!addAttributeVerify(&el1, "b", "b-val")) {
        return false;
    }
    if(!addAttributeVerify(&el1, "c", "c-val")) {
        return false;
    }
    //----
    {
        QStringList vals ;
        vals << "a" << "b" << "c" ;
        if(!verifyAttributesList( &el1, vals))
        {
            return false;
        }
    }
    //--------------------------
    return true;
}

bool TestSortAttributes::fillDefaultAttributesUnsorted(Element *el)
{
    if(!addAttributeVerify(el, "c", "c-val")) {
        return false;
    }
    if(!addAttributeVerify(el, "b", "b-val")) {
        return false;
    }
    if(!addAttributeVerify(el, "a", "a-val")) {
        return false;
    }
    return true ;
}

bool TestSortAttributes::fillDefaultAttributesSorted(Element *el)
{
    if(!addAttributeVerify(el, "a", "a-val")) {
        return false;
    }
    if(!addAttributeVerify(el, "b", "b-val")) {
        return false;
    }
    if(!addAttributeVerify(el, "c", "c-val")) {
        return false;
    }
    return true ;
}

bool TestSortAttributes::testSortUnsorted()
{
    _testName = "testSortUnsorted";
    // reverse
    {
        Element el1("a","", NULL);
        if(!fillDefaultAttributesUnsorted(&el1)) {
            return false;
        }

        QList<int> list;
        el1.sortAttributes(&list, false);
        QStringList vals ;
        vals << "a" << "b" << "c" ;
        if(!verifyAttributesList( &el1, vals))
        {
            return false;
        }
        QList<int> expected;
        expected << 2 << 1 << 0 ;
        if(!compareListInts("Position list", expected, list)) {
            return false;
        }
    }
    {
        Element el1("a","", NULL);
        if(!fillDefaultAttributesUnsorted(&el1)) {
            return false;
        }

        el1.sortAttributes(NULL, false);
        QStringList vals ;
        vals << "a" << "b" << "c" ;
        if(!verifyAttributesList( &el1, vals))
        {
            return false;
        }
    }
    {
        Element el1("a","", NULL);
        if(!fillDefaultAttributesUnsorted(&el1)) {
            return false;
        }
        Element *e2 = new Element( "b", "", NULL, &el1);
        el1.addChild(e2);
        if(!fillDefaultAttributesUnsorted(e2)) {
            return false;
        }

        el1.sortAttributes(NULL, true);
        QStringList vals ;
        vals << "a" << "b" << "c" ;
        if(!verifyAttributesList( &el1, vals))
        {
            return false;
        }
        if(!verifyAttributesList( e2, vals))
        {
            return false;
        }
    }
    return true;
}

bool TestSortAttributes::testSortSorted()
{
    _testName = "testSortSorted";
    {
        Element el1("a","", NULL);
        if(!fillDefaultAttributesSorted(&el1)) {
            return false;
        }

        QList<int> list;
        el1.sortAttributes(&list, false);
        QStringList vals ;
        vals << "a" << "b" << "c" ;
        if(!verifyAttributesList( &el1, vals))
        {
            return false;
        }
        QList<int> expected;
        expected << 0 << 1 << 2 ;
        if(!compareListInts("Position list", expected, list)) {
            return false;
        }
    }
    {
        Element el1("a","", NULL);
        if(!fillDefaultAttributesSorted(&el1)) {
            return false;
        }

        el1.sortAttributes(NULL, false);
        QStringList vals ;
        vals << "a" << "b" << "c" ;
        if(!verifyAttributesList( &el1, vals))
        {
            return false;
        }
    }
    {
        Element el1("a","", NULL);
        if(!fillDefaultAttributesUnsorted(&el1)) {
            return false;
        }
        Element *e2 = new Element( "b", "", NULL, &el1);
        el1.addChild(e2);
        if(!fillDefaultAttributesSorted(e2)) {
            return false;
        }

        el1.sortAttributes(NULL, true);
        QStringList vals ;
        vals << "a" << "b" << "c" ;
        if(!verifyAttributesList( &el1, vals))
        {
            return false;
        }
        if(!verifyAttributesList( e2, vals))
        {
            return false;
        }
    }
    return true;
}

/*!
 * \brief TestSortAttributes::testSortUndo this test relies on the correct working of the previous one
 * \return
 */
bool TestSortAttributes::testSortUndo()
{
    _testName = "testSortUndo";
    // reverse
    {
        Element el1("a","", NULL);
        if(!fillDefaultAttributesUnsorted(&el1)) {
            return false;
        }

        QList<int> list;
        el1.sortAttributes(&list, false);
        el1.sortAttributesByMap(list);
        QStringList vals ;
        vals << "c" << "b" << "a" ;
        if(!verifyAttributesList( &el1, vals))
        {
            return false;
        }
        QList<int> expected;
        expected << 2 << 1 << 0 ;
        if(!compareListInts("Position list", expected, list)) {
            return false;
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
bool TestSortAttributes::checkAttributesOrder(Element *el, QStringList attr)
{
    if(!verifyAttributesList( el, attr)) {
        return false ;
    }
    foreach( Element * child, el->getChildItemsRef()) {
        if( !checkAttributesOrder(child, attr) ) {
            return false;
        }
    }
    return true;
}

bool TestSortAttributes::checkAttributesOrder(Regola *regola, QStringList attr)
{
    Element *root = regola->root();
    if( NULL == root ) {
        return error("Missiing root");
    }
    if(!checkAttributesOrder(root, attr)) {
        return false;
    }
    return true;
}

bool TestSortAttributes::testSortInEditor()
{
    _testName = "testSortInEditor";

    App app;
    if(!app.init()) {
        return error("app init");
    }
    // 1 - Use default: check
    if( !app.mainWindow()->loadFile(QString(SORT_SORTALLINPUT)) ) {
        return error(QString("Opening test file: '%1' ").arg(SORT_SORTALLINPUT));
    }
    QStringList attrBefore;
    attrBefore << "c" << "b" << "a" ;
    if(!checkAttributesOrder(app.mainWindow()->getRegola(), attrBefore)) {
        return false;
    }
    app.mainWindow()->getEditor()->sortAttributes();
    QStringList attrAfter;
    attrAfter << "a" << "b" << "c" ;
    if(!checkAttributesOrder(app.mainWindow()->getRegola(), attrAfter)) {
        return false;
    }
    return true ;
}
