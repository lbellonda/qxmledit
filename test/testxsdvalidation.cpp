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


#include "testxsdvalidation.h"
#include "testxsdload.h"
#include "xsdeditor/xschema.h"
#include "xsdeditor/io/xschemaloader.h"


#define FILE_ATTRIBUTES_OK  "../test/data/xsd/validation/attributeok.xsd"
#define FILE_ATTRIBUTES_FIXED_USE_TOP   "../test/data/xsd/validation/attribute_fixed_use.top.xsd"
#define FILE_ATTRIBUTES_FIXED_USE_INTERNAL   "../test/data/xsd/validation/attribute_fixed_use.internal.xsd"
#define FILE_ATTRIBUTES_USE_DEFAULT_TOP     "../test/data/xsd/validation/attribute_default_use.internal.xsd"
#define FILE_ATTRIBUTES_USE_DEFAULT_INTERNAL        "../test/data/xsd/validation/attribute_default_use.internal.xsd"
#define FILE_ATTRIBUTES_USE_DEFAULT_INTERNAL_OK        "../test/data/xsd/validation/attribute_default_use.internal.ok.xsd"

#define FILE_ATTRIBUTES_NAME_AND_REF_NONE_BOTH_TOP_OK   "../test/data/xsd/validation/attribute_name_ref_both.top.ok.xsd"
#define FILE_ATTRIBUTES_NAME_AND_REF_NONE_BOTH_INTERNAL_OK "../test/data/xsd/validation/attribute_name_ref_both.internal.ok.xsd"
#define FILE_ATTRIBUTES_NAME_AND_REF_BOTH_TOP        "../test/data/xsd/validation/attribute_name_ref_both.top.xsd"
#define FILE_ATTRIBUTES_NAME_AND_REF_BOTH_INTERNAL   "../test/data/xsd/validation/attribute_name_ref_both.internal.xsd"
#define FILE_ATTRIBUTES_NAME_AND_REF_NONE_TOP        "../test/data/xsd/validation/attribute_name_ref_none.top.xsd"
#define FILE_ATTRIBUTES_NAME_AND_REF_NONE_INTERNAL   "../test/data/xsd/validation/attribute_name_ref_none.internal.xsd"

#define FILE_ATTRIBUTES_REF_TYPE_1  "../test/data/xsd/validation/attribute_ref_type.1.internal.xsd"
#define FILE_ATTRIBUTES_REF_TYPE_2  "../test/data/xsd/validation/attribute_ref_type.2.internal.xsd"
#define FILE_ATTRIBUTES_REF_TYPE_3  "../test/data/xsd/validation/attribute_ref_type.3.internal.xsd"

//-----
#define FILE_ELEMENTS_OK  "../test/data/xsd/validation/elementok.xsd"
#define FILE_TYPES_OK  "../test/data/xsd/validation/typeok.xsd"
#define FILE_ELEMENTS_USE_DEFAULT_TOP           "../test/data/xsd/validation/element_default_use.top.xsd"
#define FILE_ELEMENTS_USE_DEFAULT_INTERNAL      "../test/data/xsd/validation/element_default_use.internal.xsd"
#define FILE_ELEMENTS_USE_NODEFAULT_INTERNAL    "../test/data/xsd/validation//element_nodefault_use.internal.xsd"
#define FILE_ELEMENTS_NOUSE_DEFAULT_INTERNAL    "../test/data/xsd/validation//element_default_nouse.internal.xsd"
#define FILE_ELEMENTS_NOUSE_NODEFAULT_INTERNAL    "../test/data/xsd/validation/element_nodefault_nouse.internal.xsd"

#define FILE_ELEMENTS_USE_NONAME_NOREF  "../test/data/xsd/validation/element_noname_noref.internal.xsd"
#define FILE_ELEMENTS_USE_NAME_NOREF  "../test/data/xsd/validation/element_name_noref.internal.xsd"
#define FILE_ELEMENTS_USE_NONAME_REF  "../test/data/xsd/validation/element_noname_ref.internal.xsd"
#define FILE_ELEMENTS_USE_NAME_REF  "../test/data/xsd/validation/element_name_ref.internal.xsd"

#define FILE_ATTRIBUTES_TOP_TYPE_1  "../test/data/xsd/validation/attr.top.name.1.xsd"
#define FILE_ATTRIBUTES_TOP_TYPE_2  "../test/data/xsd/validation/attr.top.name.2.xsd"
#define FILE_ATTRIBUTES_TOP_TYPE_3  "../test/data/xsd/validation/attr.top.name.3.xsd"

#define FILE_XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE_INTERNALS_OK   "../test/data/xsd/validation/element_internal_2.2.ok.xsd"
#define FILE_XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE_INTERNALS_FORM   "../test/data/xsd/validation/element_internal_2.2.form.xsd"
#define FILE_XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE_INTERNALS_TYPE   "../test/data/xsd/validation/element_internal_2.2.type.xsd"
#define FILE_XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE_INTERNALS_SIMPLETYPE   "../test/data/xsd/validation/element_internal_2.2.simpletype.xsd"
#define FILE_XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE_INTERNALS_COMPLEXTYPE   "../test/data/xsd/validation/element_internal_2.2.complextype.xsd"
#define FILE_XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE_INTERNALS_KEY   "../test/data/xsd/validation/element_internal_2.2.key.xsd"
#define FILE_XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE_INTERNALS_KEYREF   "../test/data/xsd/validation/element_internal_2.2.keyref.xsd"
#define FILE_XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE_INTERNALS_UNIQUE   "../test/data/xsd/validation/element_internal_2.2.unique.xsd"
#define FILE_XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE_INTERNALS_BLOCK   "../test/data/xsd/validation/element_internal_2.2.block.xsd"

#define FILE_XSD_LOAD_ERROR_ELEMENT_INTERNALS_TYPEME_SIMPLE_COMPLEX   "../test/data/xsd/validation/element_internal_3.simplecomplex.xsd"
#define FILE_XSD_LOAD_ERROR_ELEMENT_INTERNALS_TYPEME_COMPLEX_SIMPLE  "../test/data/xsd/validation/element_internal_3.complexsimple.xsd"
#define FILE_XSD_LOAD_ERROR_ELEMENT_INTERNALS_TYPEME_TYPE_COMPLEX  "../test/data/xsd/validation/element_internal_3.type.complex.xsd"
#define FILE_XSD_LOAD_ERROR_ELEMENT_INTERNALS_TYPEME_TYPE_SIMPLE  "../test/data/xsd/validation/element_internal_3.type.simple.xsd"

#define FILE_TYPES_USE_NOTNAME_SIMPLE       "../test/data/xsd/validation/type.simple.notname.xsd"
#define FILE_TYPES_USE_NOTNAME_COMPLEX       "../test/data/xsd/validation/type.complex.notname.xsd"
#define FILE_TYPES_USE_REF_SIMPLE       "../test/data/xsd/validation/type.simple.ref.xsd"
#define FILE_TYPES_USE_REF_COMPLEX       "../test/data/xsd/validation/type.complex.ref.xsd"
#define FILE_TYPE_AN_SIMPLE_NAME            "../test/data/xsd/validation/type.an.simple.name.xsd"
#define FILE_TYPE_AN_COMPLEX_NAME           "../test/data/xsd/validation/type.an.complex.name.xsd"

//------------------------------------

TestXSDValidation::TestXSDValidation()
{
}

TestXSDValidation::~TestXSDValidation()
{
}

//------------------------------------

XSDLoadContext *TestXSDValidation::setupLoadContext()
{
    XSDLoadContext *newContext = new XSDLoadContext();
    newContext->setErrorPolicy(XSD_LOADPOLICY_CONTINUE);
    return newContext;
}

bool TestXSDValidation::testLoad(XSDLoadContext *loadContext, const QString &fileIn)
{
    App app;
    if(!app.init()) {
        return error("init app");
    }
    loadContext->setErrorPolicy(XSD_LOADPOLICY_CONTINUE);
    XSchemaLoader loader;
    XSchemaLoader::State state = loader.load(loadContext, fileIn, false, "");
    if( XSchemaLoader::STATE_READY != state ) {
        return error(QString("expected STATE_READY state, found '%1', file: '%2'").arg(state).arg(fileIn));
    }
    if( XSchemaLoader::SCHEMA_READY != loader.code() ) {
        return error(QString("expected SCHEMA_READY code, found '%1', file: '%2'").arg(loader.code()).arg(fileIn));
    }
    if( NULL == loader.schema()) {
        return error(QString("expected not null schema, file: '%1'").arg(fileIn));
    }
    return true ;
}

bool TestXSDValidation::returnCondition(XSDLoadContext *loadContext, const bool result )
{
    if( NULL != loadContext ) {
        delete loadContext ;
    }
    return result;
}


QString TestXSDValidation::firstMessage(XSDLoadContext *loadContext)
{
    if( loadContext->errorsCount()>1 ) {
        return QString("'%1'-'%2'").arg(loadContext->errorAt(0)->errorCode()).arg(loadContext->errorAt(0)->msg());
    }
    return "" ;
}

bool TestXSDValidation::testExec(const QString &thisTestName, const EXSDLoadError errorExpected, const QString &fileIn)
{
    _testName = _origName +thisTestName ;
    XSDLoadContext *loadContext = setupLoadContext();
    if( !testLoad(loadContext, fileIn) ) {
        return returnCondition( loadContext, false);
    }
    if( (errorExpected == XSD_LOAD_NOERROR ) && (loadContext->hasErrors()) ) {
        return returnCondition( loadContext, error(QString("Expected no errors, found:%1 errs, first is: %2").arg(loadContext->errorsCount()).arg(firstMessage(loadContext))));
    }
    if( !loadContext->containsCode(errorExpected)) {
        return returnCondition( loadContext, error(QString("Expected code:%1, found count:%2 errs, first is: %3").arg(errorExpected).arg(loadContext->errorsCount())
                                                   .arg(firstMessage(loadContext))));
    }
    return returnCondition( loadContext, true);
}

bool TestXSDValidation::testAttributeOK()
{
    if(!testExec("/testAttributeOk", XSD_LOAD_NOERROR, FILE_ATTRIBUTES_OK)) {
        return false;
    }
    return true;
}

bool TestXSDValidation::testAttributeOKRule2()
{
    if(!testExec("/testAttributeOKRule2", XSD_LOAD_NOERROR, FILE_ATTRIBUTES_USE_DEFAULT_INTERNAL_OK)) {
        return false;
    }
    return true;
}

bool TestXSDValidation::testAttributeRule1Top()
{
    if(!testExec("/testAttributeDefaultFixed.top r1", XSD_LOAD_ERROR_ATTRIBUTE_FIXED_AND_USE, FILE_ATTRIBUTES_FIXED_USE_TOP)) {
        return false;
    }
    return true;
}

bool TestXSDValidation::testAttributeRule1Internal()
{
    if(!testExec("/testAttributeDefaultFixed.internal r1", XSD_LOAD_ERROR_ATTRIBUTE_FIXED_AND_USE, FILE_ATTRIBUTES_FIXED_USE_INTERNAL)) {
        return false;
    }
    return true;
}

bool TestXSDValidation::testAttributeRule2Top()
{
    if(!testExec("/testAttributeUseDefault.top r2", XSD_LOAD_ERROR_ATTRIBUTE_DEFAULT_AND_USE, FILE_ATTRIBUTES_USE_DEFAULT_TOP)) {
        return false;
    }
    return true;
}

bool TestXSDValidation::testAttributeRule2Internal()
{
    if(!testExec("/testAttributeUseDefault.internal r2", XSD_LOAD_ERROR_ATTRIBUTE_DEFAULT_AND_USE, FILE_ATTRIBUTES_USE_DEFAULT_INTERNAL)) {
        return false;
    }
    return true;
}

bool TestXSDValidation::testAttributeRule31Internal()
{
    if(!testExec("/testAttributeUseDefaultNone.internal r31", XSD_LOAD_NOERROR, FILE_ATTRIBUTES_NAME_AND_REF_NONE_BOTH_INTERNAL_OK)) {
        return false;
    }
    if(!testExec("/testAttributeUseDefault.internal.Both r31", XSD_LOAD_ERROR_ATTRIBUTE_NAME_AND_REF, FILE_ATTRIBUTES_NAME_AND_REF_BOTH_INTERNAL)) {
        return false;
    }
    if(!testExec("/testAttributeUseDefault.internal.None r31", XSD_LOAD_ERROR_ATTRIBUTE_NAME_AND_REF, FILE_ATTRIBUTES_NAME_AND_REF_NONE_INTERNAL)) {
        return false;
    }
    return true;
}

bool TestXSDValidation::testAttributeRule32Internal()
{
    if(!testExec("/testAttributeUseDefault.internal.ref with form r32", XSD_LOAD_ERROR_ATTRIBUTE_REF_AND_TYPE, FILE_ATTRIBUTES_REF_TYPE_1)) {
        return false;
    }
    if(!testExec("/testAttributeUseDefault.internal.ref with type r32", XSD_LOAD_ERROR_ATTRIBUTE_REF_AND_TYPE, FILE_ATTRIBUTES_REF_TYPE_2)) {
        return false;
    }
    if(!testExec("/testAttributeUseDefault.internal.ref with simple r32", XSD_LOAD_ERROR_ATTRIBUTE_REF_AND_TYPE, FILE_ATTRIBUTES_REF_TYPE_3)) {
        return false;
    }
    return true;
}

bool TestXSDValidation::testAttributeRule6()
{
    if(!testExec("/testAttributeTopRefName.top with form r6", XSD_LOAD_ERROR_ATTRIBUTE_REF_AND_TYPE, FILE_ATTRIBUTES_TOP_TYPE_1)) {
        return false;
    }
    if(!testExec("/testAttributeTopRefName.top ref r6", XSD_LOAD_ERROR_ATTRIBUTE_REF_GLOBAL, FILE_ATTRIBUTES_TOP_TYPE_2)) {
        return false;
    }
    if(!testExec("/testAttributeTopRefName.top name  r6", XSD_LOAD_ERROR_ATTRIBUTE_NAME_GLOBAL, FILE_ATTRIBUTES_TOP_TYPE_3)) {
        return false;
    }
    return true;
}

//------------------------------------


bool TestXSDValidation::testElementOK()
{
    if(!testExec("/testElementsOK", XSD_LOAD_NOERROR, FILE_ELEMENTS_OK)) {
        return false;
    }
    return true;
}

bool TestXSDValidation::testTypeOK()
{
    if(!testExec("/testTypeOK", XSD_LOAD_NOERROR, FILE_TYPES_OK)) {
        return false;
    }
    return true;
}

bool TestXSDValidation::testElement1()
{
    if(!testExec("/testElement1", XSD_LOAD_ERROR_ELEMENT_DEFAULT_AND_FIXED, FILE_ELEMENTS_USE_DEFAULT_TOP)) {
        return false;
    }
    return true;
}

bool TestXSDValidation::testTopType()
{
    if(!testExec("/testTopTypeSimple", XSD_LOAD_ERROR_TYPE_TOP_NAME_NOT_SET, FILE_TYPES_USE_NOTNAME_SIMPLE)) {
        return false;
    }
    if(!testExec("/testTopComplex", XSD_LOAD_ERROR_TYPE_TOP_NAME_NOT_SET, FILE_TYPES_USE_NOTNAME_COMPLEX)) {
        return false;
    }
    /*if(!testExec("/testTopSimpleRef", XSD_LOAD_ERROR_TYPE_TOP_REF_SET, FILE_TYPES_USE_REF_SIMPLE)) {
        return false;
    }
    if(!testExec("/testTopComplexRef", XSD_LOAD_ERROR_TYPE_TOP_REF_SET, FILE_TYPES_USE_REF_COMPLEX)) {
        return false;
    }*/
    return true;
}

bool TestXSDValidation::testElementInternalRule2_2()
{
    if(!testExec("/testElement2.2InternalOK", XSD_LOAD_NOERROR, FILE_XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE_INTERNALS_OK)) {
        return false;
    }
    if(!testExec("/testElement2.2aInternal", XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE, FILE_XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE_INTERNALS_FORM)) {
        return false;
    }
    if(!testExec("/testElement2.2bInternal", XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE, FILE_XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE_INTERNALS_TYPE)) {
        return false;
    }
    if(!testExec("/testElement2.2cInternal", XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE, FILE_XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE_INTERNALS_SIMPLETYPE)) {
        return false;
    }
    if(!testExec("/testElement2.2dInternal", XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE, FILE_XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE_INTERNALS_COMPLEXTYPE)) {
        return false;
    }
    if(!testExec("/testElement2.2eInternal", XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE, FILE_XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE_INTERNALS_KEY)) {
        return false;
    }
    if(!testExec("/testElement2.2fInternal", XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE, FILE_XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE_INTERNALS_KEYREF)) {
        return false;
    }
    if(!testExec("/testElement2.2gInternal", XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE, FILE_XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE_INTERNALS_UNIQUE)) {
        return false;
    }
    if(!testExec("/testElement2.2hInternal", XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE, FILE_XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE_INTERNALS_BLOCK)) {
        return false;
    }
    return true;
}

bool TestXSDValidation::testElementInternalRule3()
{
    if(!testExec("/testElement3aInternal", XSD_LOAD_ERROR_ELEMENT_SIMPLE_AND_COMPLEX_TYPE, FILE_XSD_LOAD_ERROR_ELEMENT_INTERNALS_TYPEME_SIMPLE_COMPLEX)) {
        return false;
    }
    if(!testExec("/testElement3bInternal", XSD_LOAD_ERROR_ELEMENT_SIMPLE_AND_COMPLEX_TYPE, FILE_XSD_LOAD_ERROR_ELEMENT_INTERNALS_TYPEME_COMPLEX_SIMPLE)) {
        return false;
    }
    if(!testExec("/testElement3cInternal", XSD_LOAD_ERROR_ELEMENT_SIMPLE_AND_COMPLEX_TYPE, FILE_XSD_LOAD_ERROR_ELEMENT_INTERNALS_TYPEME_TYPE_COMPLEX)) {
        return false;
    }
    if(!testExec("/testElement3dInternal", XSD_LOAD_ERROR_ELEMENT_SIMPLE_AND_COMPLEX_TYPE, FILE_XSD_LOAD_ERROR_ELEMENT_INTERNALS_TYPEME_TYPE_SIMPLE)) {
        return false;
    }
    return true;
}

bool TestXSDValidation::testElementInternal()
{
    // rule 1 internal
    if(!testExec("/testElement1aInternal", XSD_LOAD_ERROR_ELEMENT_DEFAULT_AND_FIXED, FILE_ELEMENTS_USE_DEFAULT_INTERNAL)) {
        return false;
    }
    if(!testExec("/testElement1bInternal", XSD_LOAD_NOERROR, FILE_ELEMENTS_USE_NODEFAULT_INTERNAL)) {
        return false;
    }
    if(!testExec("/testElement1cInternal", XSD_LOAD_NOERROR, FILE_ELEMENTS_NOUSE_DEFAULT_INTERNAL)) {
        return false;
    }
    if(!testExec("/testElement1dInternal", XSD_LOAD_NOERROR, FILE_ELEMENTS_NOUSE_NODEFAULT_INTERNAL)) {
        return false;
    }
    //----
    if(!testExec("/testElement2aInternal", XSD_LOAD_ERROR_ELEMENT_NAME_AND_REF, FILE_ELEMENTS_USE_NONAME_NOREF)) {
        return false;
    }
    if(!testExec("/testElement2bInternal", XSD_LOAD_NOERROR, FILE_ELEMENTS_USE_NAME_NOREF)) {
        return false;
    }
    if(!testExec("/testElement2cInternal", XSD_LOAD_NOERROR, FILE_ELEMENTS_USE_NONAME_REF)) {
        return false;
    }
    if(!testExec("/testElement2dInternal", XSD_LOAD_ERROR_ELEMENT_NAME_AND_REF, FILE_ELEMENTS_USE_NAME_REF)) {
        return false;
    }
    //----
    if(!testElementInternalRule2_2()) {
        return false;
    }
    //--
    if(!testElementInternalRule3()) {
        return false;
    }
    //--
    return true;
}

bool TestXSDValidation::testAnonType()
{
    if(!testExec("/testTypeAnonSimpleName", XSD_LOAD_ERROR_TYPE_AN_NAME_SET, FILE_TYPE_AN_SIMPLE_NAME)) {
        return false;
    }
    if(!testExec("/testTypeAnonComplexName", XSD_LOAD_ERROR_TYPE_AN_NAME_SET, FILE_TYPE_AN_COMPLEX_NAME)) {
        return false;
    }
    return true ;
}

//------------------------------------

bool TestXSDValidation::testAttributes()
{
    _origName = "testAttributes" ;

    if(!testAttributeOK()) {
        return false;
    }
    if(!testAttributeRule1Top()) {
        return false;
    }
    if(!testAttributeRule1Internal()) {
        return false;
    }
    // -----
    if(!testAttributeOKRule2()) {
        return false;
    }
    if(!testAttributeRule2Top()) {
        return false;
    }
    if(!testAttributeRule2Internal()) {
        return false;
    }
    // -----
    if(!testAttributeRule31Internal()) {
        return false;
    }
    if(!testAttributeRule32Internal()) {
        return false;
    }
    //---
    if(!testAttributeRule6()) {
        return false;
    }
    // -----
    return true;
}


bool TestXSDValidation::testElements()
{
    _origName = "testElements" ;

    if(!testElementOK()) {
        return false;
    }
    if(!testElement1()) {
        return false;
    }
    if(!testElementInternal()) {
        return false;
    }
    return true;
}

bool TestXSDValidation::testTypes()
{
    _origName = "testTypes" ;

    if(!testTypeOK()) {
        return false;
    }
    if(!testTopType()) {
        return false;
    }
    if(!testAnonType()) {
        return false;
    }
    return true;
}

bool TestXSDValidation::testLast()
{
    if(!testTopType()) {
        return false;
    }

    return true;
}


//------------------------------
#if 0
#define VALIDATE_SCHEMA_FILE    "../test/data/xsd/validation/data/data_schema.xsd"
#define VALIDATE_XML_FILE    "../test/data/xsd/validation/data/data_schema.xml"


bool TestXSDValidation::XXXXX(const QString &inputFileName, const QString &fileXSDUrl, const QString &fileResults, const QString &actionName)
{
    App app;
    if(!app.initNoWindow() ) {
        return error("init");
    }
    MainWindow mainWindow(false, qApp, app.data());
    XmlEditWidget *editor = mainWindow.getEditor();
    if(!mainWindow.loadFile(inputFileName) ) {
        return error(QString("Unable to load input file:%1").arg(inputFileName));
    }
    Regola *regola = mainWindow.getRegola();
    if( fileXSDUrl.isEmpty() ) {
        mainWindow.getEditor()->validateUsingDocumentReferences();
    } else {
        mainWindow.getEditor()->validateWithFile(fileXSDUrl);
    }
    controllo ritorno con expected;

    //Controlla con il file campione, nota che gli elementi non sono inseriti, ma appartengono ad una lista.
    bool startAsRoot ;
    XElementContent content;
    if(! editor->getPrivate()->findAllowedItemsElement(&content, element, startAsRoot)) {
        return error(QString("findAllowedItemsElement bad result"));
    }
    if(content.allowedItems().size() != 1) {
        return error("findAllowedItemsElement bad size");
    }
    XSingleElementContent * targetContent = content.allowedItems().at(0);
    if(targetContent->item()->name() != "plane") {
        return error("findAllowedItemsElement bad element");
    }
    return true ;
}

bool TestXSDValidation::testValidateOK()
{
    _testName = "testValidateOK";
}
#endif

bool TestXSDValidation::testValidateSchema()
{
    _testName = "testValidateSchema";
    _origName = "testValidateSchema" ;

    /*if(!testValidateOK()) {
        return false;
    }*/
    return error("NYI");
    /*if(!testValidateElement()) {
        return false;
    }
    if(!testValidateAttribute()) {
        return false;
    }
    return true;*/
}

