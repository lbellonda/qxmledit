/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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

/**************************************************************
 *tests structure:
Add:

 A. | oo | oo |
 oo | A. | oo |
 oo | oo | A. |


Del:

 .D | oo | oo |
 oo | .D | oo |
 oo | oo | .D |

Mixed:

A.
.D
oo
A.
.D
oo
A.
.D


Equals:
oo
oo
oo

strategy: test ordered, unordered, attributes only mixed based on the outcome of the previous base tests

*************************************************/

// TODO: test modified

#include "testxsddiff.h"
#include "xsdeditor/xsdcompare.h"

class Test1{
    public:
    bool one;
};

#define INPUT_1             "../test/data/xsd/diff/test1.xsd"
#define INPUT_2             "../test/data/xsd/diff/test2.xsd"
#define INPUT_U             "../test/data/xsd/diff/testu.xsd"

#define OUT_ADDBEFORE       "../test/data/xsd/diff/addBefore.xsd"
#define OUT_ADDMIDDLE       "../test/data/xsd/diff/addMiddle.xsd"
#define OUT_ADDAFTER        "../test/data/xsd/diff/addAfter.xsd"

#define OUT_DELBEFORE       "../test/data/xsd/diff/delBefore.xsd"
#define OUT_DELMIDDLE       "../test/data/xsd/diff/delMiddle.xsd"
#define OUT_DELAFTER        "../test/data/xsd/diff/delAfter.xsd"

#define OUT_ADDBEFOREU       "../test/data/xsd/diff/addBeforeu.xsd"
#define OUT_ADDMIDDLEU       "../test/data/xsd/diff/addMiddleu.xsd"
#define OUT_ADDAFTERU        "../test/data/xsd/diff/addAfteru.xsd"


#define OUT_MIXED           "../test/data/xsd/diff/mixed.xsd"
#define OUT_MIXEDU           "../test/data/xsd/diff/mixedu.xsd"
#define OUT_MIXED2           "../test/data/xsd/diff/mixed2.xsd"

#define TEST_T1             "../test/data/xsd/diff/t1.xsd"
#define TEST_C1             "../test/data/xsd/diff/c1.xsd"



#define TEST_ATTR   "t:test"

TestXSDDiff::TestXSDDiff()
{
    _isError = false;
}

TestXSDDiff::~TestXSDDiff()
{
}

QString TestXSDDiff::errorString()
{
    return _errorString;
}

bool TestXSDDiff::error(const QString &testName, const QString &msg)
{
    _errorString = QString("%1: %2").arg(testName).arg(msg);
    _isError = true ;
    return false ;
}


// compare two maps, one expected and one of found elements
bool TestXSDDiff::compareResults( const QString &testName, QList<XSchemaObject*> &children, int countExpected, const QString &attrTest)
{
    if(children.size() != countExpected) {
        return error( testName, "unordered lists size differ");
    }
    for( int index = 0 ; index < countExpected ; index++ ) {
        XSchemaObject* child = children.at(index);

        // controlla attributo
        QString attrValue = child->getOtherAttribute(TEST_ATTR);
        if( attrValue != attrTest ) {
            return error( testName, QString("attribute mark not found index %1 found %2").arg(index).arg(child->compareState()));
        }
        if( countExpected  <= 0 ) {
            return error( testName, QString("state not found index %1 found %2").arg(index).arg(child->compareState()));
        }
        countExpected -- ;
    }
    return true;
}

void TestXSDDiff::collectTestObject( QList<XSchemaObject*> &outList, QList<XSchemaObject*> &objList, const XSDCompareState::EXSDCompareState stateToLook)
{
    foreach(XSchemaObject *child, objList) {
        XSDCompareState::EXSDCompareState state = child->compareState() ;
        if( ( state == stateToLook ) && hasTestAttr(child) ) {
            outList.append(child);
        }
        if( child->getType() == SchemaTypeElement ) {
            XSchemaElement *el = static_cast<XSchemaElement*>(child);
            QList<XSchemaObject*> newList;
            foreach( XSchemaObject *attr, el->attributes() ) {
                newList.append(attr);
            }
            collectTestObject( outList, newList, stateToLook );
        }
        collectTestObject( outList, child->getChildren(), stateToLook );
    }
}



bool TestXSDDiff::testOrderedForChildren( const QString &testName, QList<XSchemaObject*> &children, QList<int>&expected)
{
    int expectedSize = expected.size();
    if(children.size() != expectedSize) {
        return error(testName, "lists size differ");
    }
    for( int index = 0 ; index < expectedSize ; index++ ) {
        XSchemaObject* child = children.at(index);
        if( child->compareState() != (XSDCompareState::EXSDCompareState)expected.at(index)) {
            return error(testName, QString("state differs at index %1 expected:%2, found %3").arg(index).arg(expected.at(index)).arg(child->compareState()));
        }
    }
    return true;
}

// TODO:  questo test non tiene conto di come dovrebbe essere l'uscita da un test unordered
bool TestXSDDiff::testUnorderedForChildren( const QString &testName, QList<XSchemaObject*> &children, QHash<XSDCompareState::EXSDCompareState,int>&expected)
{
    int expectedSize = expected.size();
    if(children.size() != expectedSize) {
        return error( testName, "unordered lists size differ");
    }
    for( int index = 0 ; index < expectedSize ; index++ ) {
        XSchemaObject* child = children.at(index);
        XSDCompareState::EXSDCompareState state = child->compareState() ;
        if( expected.contains(state)) {
            int count = expected[state];
            if( count <= 0 ) {
                return error( testName, QString("state not found index %1 found %2").arg(index).arg(child->compareState()));
            }
            count -- ;
            expected.insert(state, count);
        }
    }
    return true;
}

bool TestXSDDiff::hasTestAttr( XSchemaObject *object)
{
    return object->hasOtherAttribute(TEST_ATTR);
}

XSchemaObject *TestXSDDiff::getTestObject( QList<XSchemaObject*> &list)
{
    foreach(XSchemaObject *child, list) {
        if( hasTestAttr(child) ) {
            return child;
        }
        XSchemaObject *objFound = getTestObject( child->getChildren() );
        if( NULL != objFound ) {
            return objFound;
        }
    }
    return NULL;
}


bool TestXSDDiff::verifyCollectionOrdered( const QString &testName, XSDSchema *schema, QList<int> &expected)
{
    bool isOk = false;
    XSchemaObject *o = getTestObject( schema->getChildren() );
    if( NULL == o  ) {
        error( testName, "error in operation 2");
        return false ;
    }
    if( testOrderedForChildren( testName, o->getChildren(), expected ) ) {
        isOk = true;
    }
    return isOk;
}

bool TestXSDDiff::verifyCollectionUnordered( const QString &testName, XSDSchema *schema, QList<int> &expected)
{
    bool isOk = false;
    // Note: we are expecting an ordered output
    if( testOrderedForChildren( testName, schema->getChildren(), expected ) ) {
        isOk = true;
    }
    return isOk;
}

bool TestXSDDiff::verifyAttributesUnordered( const QString &testName, XSDSchema *schema, QList<int> &expected)
{
    bool isOk = false;
    XSchemaObject *o = getTestObject( schema->getChildren() );
    if( NULL == o  ) {
        error( testName, "error in operation 4");
        return false ;
    }
    XSchemaElement *element = static_cast<XSchemaElement*>(o);
    QList<XSchemaObject*> attributes;
    foreach( XSchemaObject* attribute, element->attributes() ) {
        attributes.append(attribute);
    }

    if( testOrderedForChildren( testName, attributes, expected ) ) {
        isOk = true;
    }
    return isOk;
}

bool TestXSDDiff::doTest( const ETTType type, const QString &testName, const QString &fileNameIn, const QString &fileNameOut, QList<int> &expected, const bool areDifferent )
{
    if(!app.init() ) {
        return error(testName, "init app failed");
    }
    bool isError = false;
    if( !app.mainWindow()->loadFile(fileNameIn) ) {
        isError = true ;
        return error(testName, QString("unable to load file: '%1' ").arg(fileNameIn));
    }
    Regola *regola = app.mainWindow()->getRegola();
    XSDCompare compare;
    XSDCompareOptions options;
    options.setCompareComment(true);
    XSDCompareResult *result = compare.innerCompare(regola->getAsText(), fileNameOut, options, false);
    if(NULL == result) {
        return error(testName, "null result");
    }
    XSDSchema *schema = result->_schema;
    if(result->isError()) {
        isError = true ;
        error( testName, "error in operation 1");
    } else {
        if( result->areDifferent() != areDifferent ) {
            isError = true ;
            error( testName, "the flag is not in accord with settings");
        }
    }

    delete result;

    if(!isError) {
        switch(type) {
            default:
                isError = true ;
                error(testName, "type not valid");
                break;
            case ORDERED:
                isError = !verifyCollectionOrdered( testName, schema, expected);
                break;
            case UNORDERED:
                isError = !verifyCollectionUnordered( testName, schema, expected);
                break;
            case ATTRIBUTES:
                isError = !verifyAttributesUnordered( testName, schema, expected);
                break;
        }
    }
    if( NULL != schema ) {
        delete schema ;
    }
    return !isError ;
}


bool TestXSDDiff::doCompare( const QString &testName, const QString &fileNameIn, const QString &fileNameOut, const int expAdd, const int expMod, const int expDel, const int expUn)
{
    if(!app.init() ) {
        return error(testName, "init app failed");
    }
    bool isError = false;
    if( !app.mainWindow()->loadFile(fileNameIn) ) {
        isError = true ;
        return error(testName, QString("unable to load file: '%1' ").arg(fileNameIn));
    }
    Regola *regola = app.mainWindow()->getRegola();
    XSDCompare compare;
    XSDCompareOptions options;
    options.setCompareComment(true);
    XSDCompareResult *result = compare.innerCompare(regola->getAsText(), fileNameOut, options, false);
    if(NULL == result) {
        return error(testName, "null result");
    }
    XSDSchema *schema = result->_schema;
    if(result->isError()) {
        isError = true ;
        error( testName, "error in operation 1");
    }

    delete result;

    if(!isError) {
        QList<XSchemaObject*> addedList;
        QList<XSchemaObject*> modList;
        QList<XSchemaObject*> delList;
        QList<XSchemaObject*> unList;
        collectTestObject( addedList, schema->getChildren(), XSDCompareState::COMPARE_ADDED);
        collectTestObject( modList, schema->getChildren(), XSDCompareState::COMPARE_MODIFIED);
        collectTestObject( delList, schema->getChildren(), XSDCompareState::COMPARE_DELETED);
        collectTestObject( unList, schema->getChildren(), XSDCompareState::COMPARE_UNCHANGED);
        if(! compareResults( testName, addedList, expAdd, "a") ) {
            isError = true ;
        }
        if(! compareResults( testName, modList, expMod, "m") ) {
            isError = true ;
        }
        if(! compareResults( testName, delList, expDel, "d") ) {
            isError = true ;
        }
        if(! compareResults( testName, unList, expUn, "u") ) {
            isError = true ;
        }
    }
    if( NULL != schema ) {
        delete schema ;
    }
    return !isError ;
}


bool TestXSDDiff::goOrdered( const QString &testName, const QString &fileNameIn, const QString &fileNameOut, QList<int> &expected, const bool areDifferent )
{
    return doTest( ORDERED, testName, fileNameIn, fileNameOut, expected, areDifferent );
}

bool TestXSDDiff::goUnordered( const QString &testName, const QString &fileNameIn, const QString &fileNameOut, QList<int> &expected, const bool areDifferent )
{
      return doTest( UNORDERED, testName, fileNameIn, fileNameOut, expected, areDifferent );
}

bool TestXSDDiff::goAttributes( const QString &testName, const QString &fileNameIn, const QString &fileNameOut, QList<int> &expected, const bool areDifferent )
{
      return doTest( ATTRIBUTES, testName, fileNameIn, fileNameOut, expected, areDifferent );
}

bool TestXSDDiff::testEquals()
{
    QList<int> expected;
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    return goOrdered( "testEquals", INPUT_1, INPUT_1, expected, false );
}

bool TestXSDDiff::testEqualsUnordered()
{
    QList<int> expected;
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    return goUnordered( "testEquals", INPUT_U, INPUT_U, expected, false );
}


//------------------- region(add) ------------------------

bool TestXSDDiff::testAddBefore()
{
    QList<int> expected;
    expected.append(XSDCompareState::COMPARE_ADDED);
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    return goOrdered( "testAddBefore", INPUT_1, OUT_ADDBEFORE, expected, true);
}

bool TestXSDDiff::testAddBeforeUnordered()
{
    QList<int> expected;
    expected.append(XSDCompareState::COMPARE_ADDED);
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    return goUnordered( "testAddBeforeUnordered", INPUT_U, OUT_ADDBEFOREU, expected, true);
}

bool TestXSDDiff::testAddMiddle()
{
    QList<int> expected;
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    expected.append(XSDCompareState::COMPARE_ADDED);
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    return goOrdered( "testAddMiddle", INPUT_1, OUT_ADDMIDDLE, expected, true);
}

bool TestXSDDiff::testAddMiddleUnordered()
{
    QList<int> expected;
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    expected.append(XSDCompareState::COMPARE_ADDED);
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    return goUnordered( "testAddMiddleUnordered", INPUT_U, OUT_ADDMIDDLEU, expected, true);
}

bool TestXSDDiff::testAddAfter()
{
    QList<int> expected;
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    expected.append(XSDCompareState::COMPARE_ADDED);
    return goOrdered( "testAddAfter", INPUT_1, OUT_ADDAFTER, expected, true);
}

bool TestXSDDiff::testAddAfterUnordered()
{
    QList<int> expected;
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    expected.append(XSDCompareState::COMPARE_ADDED);
    return goUnordered( "testAddAfterUnordered", INPUT_U, OUT_ADDAFTERU, expected, true);
}

//------------------- endregion(add) ------------------------

//------------------- region(del) ------------------------
bool TestXSDDiff::testDelBefore()
{
    QList<int> expected;
    expected.append(XSDCompareState::COMPARE_DELETED);
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    return goOrdered( "testDelBefore", OUT_ADDBEFORE, INPUT_1, expected, true);
}

bool TestXSDDiff::testDelMiddle()
{
    QList<int> expected;
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    expected.append(XSDCompareState::COMPARE_DELETED);
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    return goOrdered( "testDelMiddle", OUT_ADDMIDDLE, INPUT_1, expected, true);
}

bool TestXSDDiff::testDelAfter()
{
    QList<int> expected;
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    expected.append(XSDCompareState::COMPARE_DELETED);
    return goOrdered( "testDelAfter", OUT_ADDAFTER, INPUT_1, expected, true);
}

bool TestXSDDiff::testDelBeforeUnordered()
{
    QList<int> expected;
    expected.append(XSDCompareState::COMPARE_DELETED);
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    return goUnordered( "testDelBeforeUnordered", OUT_ADDBEFOREU, INPUT_U, expected, true);
}

bool TestXSDDiff::testDelMiddleUnordered()
{
    QList<int> expected;
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    expected.append(XSDCompareState::COMPARE_DELETED);
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    return goUnordered( "testDelMiddleUnordered", OUT_ADDMIDDLEU, INPUT_U, expected, true);
}

bool TestXSDDiff::testDelAfterUnordered()
{
    QList<int> expected;
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    expected.append(XSDCompareState::COMPARE_DELETED);
    return goUnordered( "testDelAfterUnordered", OUT_ADDAFTERU, INPUT_U, expected, true);
}

//------------------- endregion(del) ------------------------

//------------------- region(mixed) ------------------------
bool TestXSDDiff::testInnerMixed()
{
    QList<int> expected;
    expected.append(XSDCompareState::COMPARE_ADDED);
    expected.append(XSDCompareState::COMPARE_DELETED);
    expected.append(XSDCompareState::COMPARE_MODIFIED);
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    return goOrdered( "testMixed", INPUT_1, OUT_MIXED, expected, true);
}

bool TestXSDDiff::testInnerMixedUnordered()
{
    QList<int> expected;
    expected.append(XSDCompareState::COMPARE_ADDED);
    expected.append(XSDCompareState::COMPARE_DELETED);
    expected.append(XSDCompareState::COMPARE_MODIFIED);
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    return goUnordered( "testMixed", INPUT_U, OUT_MIXEDU, expected, true);
}

bool TestXSDDiff::testOpAttributes()
{
    QList<int> expected;
    expected.append(XSDCompareState::COMPARE_ADDED);
    expected.append(XSDCompareState::COMPARE_MODIFIED);
    expected.append(XSDCompareState::COMPARE_UNCHANGED);
    expected.append(XSDCompareState::COMPARE_DELETED);
    return goAttributes( "testAttributes", INPUT_2, OUT_MIXED2, expected, true);
}

//------------------- endregion(mixed) ------------------------

XSDCompareObject::EXSDCompareObject TestXSDDiff::testBaseEquals()
{
    Test1 other;
    one = false;
    other.one = false;
    CMP_DATA(one, &other);
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

XSDCompareObject::EXSDCompareObject TestXSDDiff::testBaseDifferent()
{
    Test1 other;
    one = false;
    other.one = true;
    CMP_DATA(one, &other);
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

//------------------- region(unittest) ------------------------

bool TestXSDDiff::testDiffx1()
{
    bool ok = true ;
    int numT = 0 ;
    // element
    numT++;
    // attributes
    numT++;
    // choice
    numT++;
    // sequence
    numT++;
    // all
    numT++;
    // any
    numT++;
    // anyattribute
    numT++;
    //key
    numT++;
    //keyref
    numT++;
    //unique
    numT++;
    //field
    numT++;
    //selector
    numT++;
    //include
    numT++;
    //import
    numT++;
    //redefine
    numT++;
    //attribute
    numT++;
    //annotation, documentation
    numT++;
    // extension (complexContent)
    numT++;
    //restriction (complexContent)
    numT++;
    //attributeGroup
    numT++;

    //group
    numT++;
    //list
    numT++;
    //notation
    numT++;
    // union
    numT++;
    // extension (simpleContent)
    numT++;
    // restriction (simpleContent)
    numT++;

    if(!doCompare( "test1", TEST_T1, TEST_C1, numT, numT, numT, numT) ) {
        ok = false;
    }
    return ok ;
}


bool TestXSDDiff::testObjects()
{
    XSDCompareOptions options;
    XSchemaElement el1(NULL,NULL);
    XSchemaElement el2(NULL,NULL);
    // distruttore!
    if( !el1.baseInnerCompareTo(&el2, options) ) {
        return error("testObjects", "1");
    }
    XSchemaElement el3(NULL,NULL);
    XSchemaElement el4(NULL,NULL);
    el3.setId("test");
    if( el3.baseInnerCompareTo(&el4,options) ) {
        return error("testObjects", "2");
    }
    XSchemaElement el5(NULL,NULL);
    XSchemaElement el6(NULL,NULL);
    el6.setId("test");
    if( el5.baseInnerCompareTo(&el6, options) ) {
        return error("testObjects", "3");
    }
    XSchemaElement el7(NULL,NULL);
    XSchemaElement el8(NULL,NULL);
    el7.setId("test");
    if( el7.baseInnerCompareTo(&el8, options) ) {
        return error("testObjects", "4");
    }
    XSchemaElement el9(NULL,NULL);
    XSchemaElement el10(NULL,NULL);
    el10.setId("test");
    if( el9.baseInnerCompareTo(&el10, options) ) {
        return error("testObjects", "5");
    }

    XDocumentation x1(NULL,NULL);
    XDocumentation x2(NULL,NULL);
    x1.setSource("a");
    x2.setSource("a");
    XSchemaAnnotation a1(NULL,NULL);
    XSchemaAnnotation a2(NULL,NULL);
    a1._infos.append(&x1);
    a2._infos.append(&x2);

    XSchemaElement el11(NULL,NULL);
    XSchemaElement el12(NULL,NULL);

    el11._annotation = &a1;
    el12._annotation = &a2;
    bool isOk = true ;
    if( !el11.baseInnerCompareTo(&el12, options) ) {
        isOk = false ;
    }

    if( !el11.baseInnerCompareTo(&el12, options) ) {
        isOk = false ;
    }
    options.setCompareComment(false);
    if( !el11.baseInnerCompareTo(&el12, options) ) {
        isOk = false ;
    }
    options.setCompareComment(true);
    x1.setSource("b");
    if( el11.baseInnerCompareTo(&el12, options) ) {
        isOk = false ;
    }
    options.setCompareComment(false);
    if( !el11.baseInnerCompareTo(&el12, options) ) {
        isOk = false ;
    }

    a1._infos.clear();
    a2._infos.clear();
    el11._annotation = NULL ;
    el12._annotation = NULL ;

    if(!isOk) {
        return error("testObjects","6");
    }
    return true;
}


bool TestXSDDiff::testDiffElementsAndTypes()
{
    XSDCompareOptions options;
    XSchemaElement el1(NULL,NULL);
    XSchemaElement el2(NULL,NULL);
    // distruttore!
    if( el1.innerCompareTo(&el2, options) != XSDCompareObject::XSDOBJECT_UNCHANGED ) {
        return error("testDiffElementsAndTypes", "1");
    }
    el2._minOccurs.occurrences = el1._minOccurs.occurrences+1;
    if( el1.innerCompareTo(&el2, options) != XSDCompareObject::XSDOBJECT_MODIFIED ) {
        return error("testDiffElementsAndTypes", "2");
    }
    return true;
}

bool TestXSDDiff::testDiffChoice()
{
    XSDCompareOptions options;
    XSchemaChoice choice1(NULL,NULL);
    XSchemaChoice choice2(NULL,NULL);
    // distruttore!
    if( choice1.innerCompareTo(&choice2, options) != XSDCompareObject::XSDOBJECT_UNCHANGED ) {
        return error("testDiffChoice", "1");
    }
    choice2._minOccurs.occurrences = choice1._minOccurs.occurrences+1;
    if( choice1.innerCompareTo(&choice2, options) != XSDCompareObject::XSDOBJECT_MODIFIED ) {
        return error("testDiffChoice", "2");
    }
    return true;
}

bool TestXSDDiff::testDiffSequence()
{
    XSDCompareOptions options;
    XSchemaSequence s1(NULL,NULL);
    XSchemaSequence s2(NULL,NULL);
    // distruttore!
    if( s1.innerCompareTo(&s2, options) != XSDCompareObject::XSDOBJECT_UNCHANGED ) {
        return error("testDiffSequence", "1");
    }
    s2._minOccurs.occurrences = s1._minOccurs.occurrences+1;
    if( s1.innerCompareTo(&s2, options) != XSDCompareObject::XSDOBJECT_MODIFIED ) {
        return error("testDiffSequence", "2");
    }
    return true;
}

bool TestXSDDiff::testDiffAttributes()
{
    XSDCompareOptions options;
    XSchemaAttribute el1(NULL,NULL);
    XSchemaAttribute el2(NULL,NULL);
    // distruttore!
    if( el1.innerCompareTo(&el2, options) != XSDCompareObject::XSDOBJECT_UNCHANGED ) {
        return error("testDiffAttributes", "1");
    }
    el2._fixed = "el1._fixed";
    if( el1.innerCompareTo(&el2, options) != XSDCompareObject::XSDOBJECT_MODIFIED ) {
        return error("testDiffAttributes", "2");
    }
    return true;
}

QColor TestXSDDiff::updateColor( const QColor & inputColor )
{
    QColor result;
    result.setRed(inputColor.red()^0xFF);
    result.setGreen(inputColor.green()^0xFF);
    result.setBlue(inputColor.blue()^0xFF);
    result.setAlpha(inputColor.alpha()^0xFF);
    return result;
}

bool TestXSDDiff::testBaseSettings()
{
    putConfigToSleep();
    XSDGraphicsBackgroundConfiguration defaultItem;
    XSDGraphicsBackgroundConfiguration saveItem;
    XSDGraphicsBackgroundConfiguration readItem;

    // test save - load

    saveItem.setMainColor(updateColor(defaultItem.mainColor()));
    saveItem.setAlternateColor (updateColor(defaultItem.alternateColor()));
    if( XSDGraphicsBackgroundConfiguration::GradientLinear == defaultItem.gradientType() ) {
        saveItem.setGradientType(XSDGraphicsBackgroundConfiguration::GradientRadial);
    } else {
        saveItem.setGradientType(XSDGraphicsBackgroundConfiguration::GradientLinear);
    }
    if( XSDGraphicsBackgroundConfiguration::DirectionVertical == defaultItem.gradientDirection() ) {
        saveItem.setGradientDirection(XSDGraphicsBackgroundConfiguration::DirectionDiagonal);
    } else {
        saveItem.setGradientDirection(XSDGraphicsBackgroundConfiguration::DirectionVertical);
    }
    saveItem.setGradientHeight(defaultItem.gradientHeight()+1);
    if( XSDGraphicsBackgroundConfiguration::GradientUnitsPct == defaultItem.gradientUnits() ) {
        saveItem.setGradientUnits(XSDGraphicsBackgroundConfiguration::GradientUnitsPixels);
    } else {
        saveItem.setGradientUnits( XSDGraphicsBackgroundConfiguration::GradientUnitsPct);
    }
    saveItem.save();
    readItem.load();
    // compare items item by item
    if( !compareBkConfig(saveItem, readItem) ) {
        return error("testBaseSettings", "saved");
    }
    // compare with the default
    if( compareBkConfig(defaultItem, saveItem) ) {
        return error("testBaseSettings", "compare");
    }
    // compare with itself
    if( !compareBkConfig(defaultItem, defaultItem) ) {
        return error("testBaseSettings", "compare2");
    }
    return true;
}

bool TestXSDDiff::errorCompare()
{
    return false;
}

bool TestXSDDiff::compareColors(const QColor &one, const QColor &two )
{
    if( one.red() != two.red() ) {
        return false;
    }
    if( one.green() != two.green() ) {
        return false;
    }
    if( one.blue() != two.blue() ) {
        return false;
    }
    if( one.alpha() != two.alpha() ) {
        return false;
    }
    return true ;
}

bool TestXSDDiff::compareBkConfig(XSDGraphicsBackgroundConfiguration &one, XSDGraphicsBackgroundConfiguration &two)
{
    if( !compareColors(one.mainColor(), two.mainColor() ) ) {
        return errorCompare();
    }
    if( !compareColors( one.alternateColor(), two.alternateColor() ) ) {
        return errorCompare();
    }
    if( one.gradientType() != two.gradientType() ) {
        return errorCompare();
    }
    if( one.gradientDirection() != two.gradientDirection() ) {
        return errorCompare();
    }
    if( one.gradientHeight() != two.gradientHeight() ) {
        return errorCompare();
    }
    if( one.gradientUnits() != two.gradientUnits() ) {
        return errorCompare();
    }
    return true ;
}

//------------------- endregion(unittest) ------------------------

// avoid config pollution
void TestXSDDiff::putConfigToSleep()
{
    Config::setBackend(&_configBackend);
    Config::saveBool(Config::KEY_SESSIONS_ENABLED, false);
}
//----------------------------------------------- external methods ----------------------------------------

//------------------- region(external) ------------------------

bool TestXSDDiff::testAdd()
{
    if( !testAddBefore()) {
        return false;
    }
    if( !testAddMiddle()) {
        return false;
    }
    if( !testAddAfter()) {
        return false;
    }
    return true;
}

bool TestXSDDiff::testDel()
{
    if( !testDelBefore()) {
        return false;
    }
    if( !testDelMiddle()) {
        return false;
    }
    if( !testDelAfter()) {
        return false;
    }
    return true;
}

bool TestXSDDiff::testEqual()
{
    if( !testEquals()) {
        return false;
    }
    return true;
}

bool TestXSDDiff::testMixed()
{
    return testInnerMixed();
}

/**
 * @brief TestXSDDiff::testCompareBase
 * tests the basis of the diff functions
 * @return
 */
bool TestXSDDiff::testCompareBase()
{
    if( testBaseEquals() != XSDCompareObject::XSDOBJECT_UNCHANGED ) {
        return error("testCompareBase", "equals");
    }
    if( testBaseDifferent() != XSDCompareObject::XSDOBJECT_MODIFIED ) {
        return error("testCompareBase", "different");
    }
    if( !testBaseSettings() ) {
        return error("testCompareBase", "settings");
    }
    return true;
}

bool TestXSDDiff::testAddUnordered()
{
    if( !testAddBeforeUnordered()) {
        return false;
    }
    if( !testAddMiddleUnordered()) {
        return false;
    }
    if( !testAddAfterUnordered()) {
        return false;
    }
    return true;
}

bool TestXSDDiff::testDelUnordered()
{
    if( !testDelBeforeUnordered()) {
        return false;
    }
    if( !testDelMiddleUnordered()) {
        return false;
    }
    if( !testDelAfterUnordered()) {
        return false;
    }
    return true;
}

bool TestXSDDiff::testEqualUnordered()
{
    return testEqualsUnordered();
}

bool TestXSDDiff::testMixedUnordered()
{
    return testInnerMixedUnordered();
}

bool TestXSDDiff::testDiffObjects()
{
    if( !testObjects() ) {
        return false;
    }

    if( !testDiffElementsAndTypes() ) {
        return false;
    }
    if( !testDiffChoice() ) {
        return false;
    }
    if( !testDiffSequence() ) {
        return false;
    }
    if( !testDiffAttributes() ) {
        return false;
    }
    if( !testDiffx1() ) {
        return false;
    }
    return true ;
}

bool TestXSDDiff::testDiffOpAttributes()
{
    return testOpAttributes();
}

//------------------- endregion(external) ------------------------
