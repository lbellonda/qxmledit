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


#include "testxsdmode.h"
#include "app.h"
#include "modules/xsd/xsdhelper.h"
#include "comparexml.h"
#include "testhelpers/testdialogxsdtypes.h"

#define FILE_CHECK_ENABLE       "../test/data/xsd/mode/enabling.xml"

#define FILE_ELEMENT_SIMPLE_INSERT_START  "../test/data/xsd/mode/element_simple.insert.start.xsd"
#define FILE_ELEMENT_SIMPLE_INSERT_SIBLING_RESULT "../test/data/xsd/mode/element_simple.insert.final.xsd"
#define FILE_ELEMENT_SIMPLE_INSERT_CHILD_RESULT "../test/data/xsd/mode/element_simple.insert.child.final.xsd"
#define FILE_ELEMENT_SIMPLE_START_REF "../test/data/xsd/mode/element_simple.mod.ref.start.xsd"
#define FILE_ELEMENT_SIMPLE_FINAL_REF "../test/data/xsd/mode/element_simple.mod.ref.final.xsd"
#define FILE_ELEMENT_SIMPLE_START_BARE "../test/data/xsd/mode/element_simple.mod.bare.start.xsd"
#define FILE_ELEMENT_SIMPLE_FINAL_BARE "../test/data/xsd/mode/element_simple.mod.bare.final.xsd"
#define FILE_ELEMENT_SIMPLE_START_AN "../test/data/xsd/mode/element_simple.mod.an.start.xsd"
#define FILE_ELEMENT_SIMPLE_FINAL_AN "../test/data/xsd/mode/element_simple.mod.an.final.xsd"
#define FILE_ELEMENT_SIMPLE_START_CPX "../test/data/xsd/mode/element_simple.mod.cpx.start.xsd"
#define FILE_ELEMENT_SIMPLE_FINAL_CPX "../test/data/xsd/mode/element_simple.mod.cpx.final.xsd"
#define FILE_ELEMENT_SIMPLE_START_RES "../test/data/xsd/mode/element_simple.mod.res.start.xsd"
#define FILE_ELEMENT_SIMPLE_FINAL_RES "../test/data/xsd/mode/element_simple.mod.res.final.xsd"
#define FILE_ELEMENT_SIMPLE_START_EXT "../test/data/xsd/mode/element_simple.mod.ext.start.xsd"
#define FILE_ELEMENT_SIMPLE_FINAL_EXT "../test/data/xsd/mode/element_simple.mod.ext.final.xsd"
#define FILE_ELEMENT_SIMPLE_START_EXT_INS "../test/data/xsd/mode/element_simple.mod.ext.ins.start.xsd"
#define FILE_ELEMENT_SIMPLE_FINAL_EXT_INS "../test/data/xsd/mode/element_simple.mod.ext.ins.final.xsd"
#define FILE_ELEMENT_COMPLEX_START_REF "../test/data/xsd/mode/element_complex.ins.ref.start.xsd"
#define FILE_ELEMENT_COMPLEX_FINAL_REF "../test/data/xsd/mode/element_complex.ins.ref.final.xsd"
#define FILE_ELEMENT_COMPLEX_START_REF_EA "../test/data/xsd/mode/element_complex.mod.refea.start.xsd"
#define FILE_ELEMENT_COMPLEX_FINAL_REF_EA "../test/data/xsd/mode/element_complex.mod.refea.final.xsd"
#define FILE_ELEMENT_COMPLEX_START_REF_EAO "../test/data/xsd/mode/element_complex.mod.refeao.start.xsd"
#define FILE_ELEMENT_COMPLEX_FINAL_REF_EAO "../test/data/xsd/mode/element_complex.mod.refeao.final.xsd"

#define FILE_ELEMENT_COMPLEX_START_RES "../test/data/xsd/mode/element_complex.ins.res.start.xsd"
#define FILE_ELEMENT_COMPLEX_FINAL_RES "../test/data/xsd/mode/element_complex.ins.res.final.xsd"
#define FILE_ELEMENT_COMPLEX_START_EXT "../test/data/xsd/mode/element_complex.ins.ext.start.xsd"
#define FILE_ELEMENT_COMPLEX_FINAL_EXT "../test/data/xsd/mode/element_complex.ins.ext.final.xsd"
#define FILE_ELEMENT_COMPLEX_START_NONE "../test/data/xsd/mode/element_complex.ins.none.start.xsd"
#define FILE_ELEMENT_COMPLEX_FINAL_NONE "../test/data/xsd/mode/element_complex.ins.none.final.xsd"
#define FILE_ELEMENT_COMPLEX_FINAL_SEQUENCE "../test/data/xsd/mode/element_complex.ins.sequence.final.xsd"
#define FILE_ELEMENT_COMPLEX_FINAL_ALL "../test/data/xsd/mode/element_complex.ins.all.final.xsd"
#define FILE_ELEMENT_COMPLEX_FINAL_CHOICE "../test/data/xsd/mode/element_complex.ins.choice.final.xsd"
#define FILE_ELEMENT_COMPLEX_FINAL_GROUP "../test/data/xsd/mode/element_complex.ins.group.final.xsd"

//-------
#define FILE_ELEMENT_COMPLEX_EXT_START_MODIFY           "../test/data/xsd/mode/element_complex.mod.ext.start.xsd"
#define FILE_ELEMENT_COMPLEX_EXT_FINAL_MODIFY           "../test/data/xsd/mode/element_complex.mod.ext.final.xsd"
#define FILE_ELEMENT_COMPLEX_EXT_START_MODIFY_OTHER     "../test/data/xsd/mode/element_complex.mod.ext.other.start.xsd"
#define FILE_ELEMENT_COMPLEX_EXT_FINAL_MODIFY_OTHER     "../test/data/xsd/mode/element_complex.mod.ext.other.final.xsd"
//--
#define FILE_ELEMENT_COMPLEX_RES_START_MODIFY           "../test/data/xsd/mode/element_complex.mod.res.start.xsd"
#define FILE_ELEMENT_COMPLEX_RES_FINAL_MODIFY           "../test/data/xsd/mode/element_complex.mod.res.final.xsd"
#define FILE_ELEMENT_COMPLEX_RES_START_MODIFY_OTHER     "../test/data/xsd/mode/element_complex.mod.res.other.start.xsd"
#define FILE_ELEMENT_COMPLEX_RES_FINAL_MODIFY_OTHER     "../test/data/xsd/mode/element_complex.mod.res.other.final.xsd"
//--
#define FILE_ELEMENT_COMPLEX_SEQ_START_MODIFY           "../test/data/xsd/mode/element_complex.mod.seq.start.xsd"
#define FILE_ELEMENT_COMPLEX_SEQ_FINAL_MODIFY           "../test/data/xsd/mode/element_complex.mod.seq.final.xsd"
#define FILE_ELEMENT_COMPLEX_SEQ_START_MODIFY_OTHER     "../test/data/xsd/mode/element_complex.mod.seq.other.start.xsd"
#define FILE_ELEMENT_COMPLEX_SEQ_FINAL_MODIFY_OTHER     "../test/data/xsd/mode/element_complex.mod.seq.other.final.xsd"
//--
#define FILE_ELEMENT_COMPLEX_CHOICE_START_MODIFY           "../test/data/xsd/mode/element_complex.mod.choice.start.xsd"
#define FILE_ELEMENT_COMPLEX_CHOICE_FINAL_MODIFY           "../test/data/xsd/mode/element_complex.mod.choice.final.xsd"
#define FILE_ELEMENT_COMPLEX_CHOICE_START_MODIFY_OTHER     "../test/data/xsd/mode/element_complex.mod.choice.other.start.xsd"
#define FILE_ELEMENT_COMPLEX_CHOICE_FINAL_MODIFY_OTHER     "../test/data/xsd/mode/element_complex.mod.choice.other.final.xsd"
//--
#define FILE_ELEMENT_COMPLEX_ALL_START_MODIFY           "../test/data/xsd/mode/element_complex.mod.all.start.xsd"
#define FILE_ELEMENT_COMPLEX_ALL_FINAL_MODIFY           "../test/data/xsd/mode/element_complex.mod.all.final.xsd"
#define FILE_ELEMENT_COMPLEX_ALL_START_MODIFY_OTHER     "../test/data/xsd/mode/element_complex.mod.all.other.start.xsd"
#define FILE_ELEMENT_COMPLEX_ALL_FINAL_MODIFY_OTHER     "../test/data/xsd/mode/element_complex.mod.all.other.final.xsd"
//--
#define FILE_ELEMENT_COMPLEX_GROUP_START_MODIFY           "../test/data/xsd/mode/element_complex.mod.group.start.xsd"
#define FILE_ELEMENT_COMPLEX_GROUP_FINAL_MODIFY           "../test/data/xsd/mode/element_complex.mod.group.final.xsd"
#define FILE_ELEMENT_COMPLEX_GROUP_START_MODIFY_OTHER     "../test/data/xsd/mode/element_complex.mod.group.other.start.xsd"
#define FILE_ELEMENT_COMPLEX_GROUP_FINAL_MODIFY_OTHER     "../test/data/xsd/mode/element_complex.mod.group.other.final.xsd"
//--
#define FILE_ELEMENT_COMPLEX_NONE_START_MODIFY           "../test/data/xsd/mode/element_complex.mod.none.start.xsd"
#define FILE_ELEMENT_COMPLEX_NONE_FINAL_MODIFY           "../test/data/xsd/mode/element_complex.mod.none.final.xsd"
#define FILE_ELEMENT_COMPLEX_NONE_START_MODIFY_OTHER     "../test/data/xsd/mode/element_complex.mod.none.other.start.xsd"
#define FILE_ELEMENT_COMPLEX_NONE_FINAL_MODIFY_OTHER     "../test/data/xsd/mode/element_complex.mod.none.other.final.xsd"
//-------
#define FILE_ELEMENT_COMPLEX_SIMPLEEXT_START_MODIFY           "../test/data/xsd/mode/element_complex.mod.simple.ext.start.xsd"
#define FILE_ELEMENT_COMPLEX_SIMPLEEXT_FINAL_MODIFY           "../test/data/xsd/mode/element_complex.mod.simple.ext.final.xsd"
#define FILE_ELEMENT_COMPLEX_SIMPLEEXT_START_MODIFY_OTHER     "../test/data/xsd/mode/element_complex.mod.simple.ext.other.start.xsd"
#define FILE_ELEMENT_COMPLEX_SIMPLEEXT_FINAL_MODIFY_OTHER     "../test/data/xsd/mode/element_complex.mod.simple.ext.other.final.xsd"
//--
#define FILE_ELEMENT_COMPLEX_SIMPLERES_START_MODIFY           "../test/data/xsd/mode/element_complex.mod.simple.res.start.xsd"
#define FILE_ELEMENT_COMPLEX_SIMPLERES_FINAL_MODIFY           "../test/data/xsd/mode/element_complex.mod.simple.res.final.xsd"
#define FILE_ELEMENT_COMPLEX_SIMPLERES_START_MODIFY_OTHER     "../test/data/xsd/mode/element_complex.mod.simple.res.other.start.xsd"
#define FILE_ELEMENT_COMPLEX_SIMPLERES_FINAL_MODIFY_OTHER     "../test/data/xsd/mode/element_complex.mod.simple.res.other.final.xsd"

///------------------------

#define FILE_TYPE_COMPLEX_SIMPLERES_START_MODIFY           "../test/data/xsd/mode/type/type_complex.mod.simple.res.plain.start.xsd"
#define FILE_TYPE_COMPLEX_SIMPLERES_FINAL_MODIFY           "../test/data/xsd/mode/type/type_complex.mod.simple.res.plain.final.xsd"
#define FILE_TYPE_COMPLEX_SIMPLERES_START_MODIFY_OTHER     "../test/data/xsd/mode/type/type_complex.mod.simple.res.other.start.xsd"
#define FILE_TYPE_COMPLEX_SIMPLERES_FINAL_MODIFY_OTHER     "../test/data/xsd/mode/type/type_complex.mod.simple.res.other.final.xsd"

//-------
#define FILE_TYPE_COMPLEX_SIMPLEEXT_START_MODIFY           "../test/data/xsd/mode/type/type_complex.mod.simple.ext.plain.start.xsd"
#define FILE_TYPE_COMPLEX_SIMPLEEXT_FINAL_MODIFY           "../test/data/xsd/mode/type/type_complex.mod.simple.ext.plain.final.xsd"
#define FILE_TYPE_COMPLEX_SIMPLEEXT_START_MODIFY_OTHER     "../test/data/xsd/mode/type/type_complex.mod.simple.ext.other.start.xsd"
#define FILE_TYPE_COMPLEX_SIMPLEEXT_FINAL_MODIFY_OTHER     "../test/data/xsd/mode/type/type_complex.mod.simple.ext.other.final.xsd"
//--
//--
#define FILE_TYPE_COMPLEX_NONE_START_MODIFY           "../test/data/xsd/mode/type/type_complex.mod.none.plain.start.xsd"
#define FILE_TYPE_COMPLEX_NONE_FINAL_MODIFY           "../test/data/xsd/mode/type/type_complex.mod.none.plain.final.xsd"
#define FILE_TYPE_COMPLEX_NONE_START_MODIFY_OTHER     "../test/data/xsd/mode/type/type_complex.mod.none.other.start.xsd"
#define FILE_TYPE_COMPLEX_NONE_FINAL_MODIFY_OTHER     "../test/data/xsd/mode/type/type_complex.mod.none.other.final.xsd"
//-------

#define FILE_TYPE_COMPLEX_ALL_START_MODIFY           "../test/data/xsd/mode/type/type_complex.mod.all.plain.start.xsd"
#define FILE_TYPE_COMPLEX_ALL_FINAL_MODIFY           "../test/data/xsd/mode/type/type_complex.mod.all.plain.final.xsd"
#define FILE_TYPE_COMPLEX_ALL_START_MODIFY_OTHER     "../test/data/xsd/mode/type/type_complex.mod.all.other.start.xsd"
#define FILE_TYPE_COMPLEX_ALL_FINAL_MODIFY_OTHER     "../test/data/xsd/mode/type/type_complex.mod.all.other.final.xsd"
//--
#define FILE_TYPE_COMPLEX_CHOICE_START_MODIFY           "../test/data/xsd/mode/type/type_complex.mod.choice.plain.start.xsd"
#define FILE_TYPE_COMPLEX_CHOICE_FINAL_MODIFY           "../test/data/xsd/mode/type/type_complex.mod.choice.plain.final.xsd"
#define FILE_TYPE_COMPLEX_CHOICE_START_MODIFY_OTHER     "../test/data/xsd/mode/type/type_complex.mod.choice.other.start.xsd"
#define FILE_TYPE_COMPLEX_CHOICE_FINAL_MODIFY_OTHER     "../test/data/xsd/mode/type/type_complex.mod.choice.other.final.xsd"

//--
#define FILE_TYPE_COMPLEX_GROUP_START_MODIFY           "../test/data/xsd/mode/type/type_complex.mod.group.plain.start.xsd"
#define FILE_TYPE_COMPLEX_GROUP_FINAL_MODIFY           "../test/data/xsd/mode/type/type_complex.mod.group.plain.final.xsd"
#define FILE_TYPE_COMPLEX_GROUP_START_MODIFY_OTHER     "../test/data/xsd/mode/type/type_complex.mod.group.other.start.xsd"
#define FILE_TYPE_COMPLEX_GROUP_FINAL_MODIFY_OTHER     "../test/data/xsd/mode/type/type_complex.mod.group.other.final.xsd"
//--
#define FILE_TYPE_COMPLEX_SEQ_START_MODIFY           "../test/data/xsd/mode/type/type_complex.mod.seq.plain.start.xsd"
#define FILE_TYPE_COMPLEX_SEQ_FINAL_MODIFY           "../test/data/xsd/mode/type/type_complex.mod.seq.plain.final.xsd"
#define FILE_TYPE_COMPLEX_SEQ_START_MODIFY_OTHER     "../test/data/xsd/mode/type/type_complex.mod.seq.other.start.xsd"
#define FILE_TYPE_COMPLEX_SEQ_FINAL_MODIFY_OTHER     "../test/data/xsd/mode/type/type_complex.mod.seq.other.final.xsd"
//-------
#define FILE_TYPE_COMPLEX_EXT_START_MODIFY           "../test/data/xsd/mode/type/type_complex.mod.ext.plain.start.xsd"
#define FILE_TYPE_COMPLEX_EXT_FINAL_MODIFY           "../test/data/xsd/mode/type/type_complex.mod.ext.plain.final.xsd"
#define FILE_TYPE_COMPLEX_EXT_START_MODIFY_OTHER     "../test/data/xsd/mode/type/type_complex.mod.ext.other.start.xsd"
#define FILE_TYPE_COMPLEX_EXT_FINAL_MODIFY_OTHER     "../test/data/xsd/mode/type/type_complex.mod.ext.other.final.xsd"
//--
#define FILE_TYPE_COMPLEX_RES_START_MODIFY           "../test/data/xsd/mode/type/type_complex.mod.res.plain.start.xsd"
#define FILE_TYPE_COMPLEX_RES_FINAL_MODIFY           "../test/data/xsd/mode/type/type_complex.mod.res.plain.final.xsd"
#define FILE_TYPE_COMPLEX_RES_START_MODIFY_OTHER     "../test/data/xsd/mode/type/type_complex.mod.res.other.start.xsd"
#define FILE_TYPE_COMPLEX_RES_FINAL_MODIFY_OTHER     "../test/data/xsd/mode/type/type_complex.mod.res.other.final.xsd"
//--

#define FILE_ELEMENT_COMPLEX_REF_START_MODIFY           "../test/data/xsd/mode/element_complex.mod.ref.plain.start.xsd"
#define FILE_ELEMENT_COMPLEX_REF_FINAL_MODIFY           "../test/data/xsd/mode/element_complex.mod.ref.plain.final.xsd"
#define FILE_ELEMENT_COMPLEX_REF_START_MODIFY_OTHER     "../test/data/xsd/mode/element_complex.mod.ref.other.start.xsd"
#define FILE_ELEMENT_COMPLEX_REF_FINAL_MODIFY_OTHER     "../test/data/xsd/mode/element_complex.mod.ref.other.final.xsd"
//--

#define FILE_TYPE_COMPLEX_COMPLEX_CONTENT_RESTR_FINAL_INS       "../test/data/xsd/mode/type/type_complex.ins.cc.res.final.xsd"
#define FILE_TYPE_COMPLEX_COMPLEX_CONTENT_EXTENSION_FINAL_INS   "../test/data/xsd/mode/type/type_complex.ins.cc.ext.final.xsd"
#define FILE_TYPE_COMPLEX_NONE_FINAL_INS                        "../test/data/xsd/mode/type/type_complex.ins.none.final.xsd"
#define FILE_TYPE_COMPLEX_SEQ_FINAL_INS                         "../test/data/xsd/mode/type/type_complex.ins.seq.final.xsd"
#define FILE_TYPE_COMPLEX_CHOICE_FINAL_INS                      "../test/data/xsd/mode/type/type_complex.ins.choice.final.xsd"
#define FILE_TYPE_COMPLEX_ALL_FINAL_INS                           "../test/data/xsd/mode/type/type_complex.ins.all.final.xsd"
#define FILE_TYPE_COMPLEX_GROUP_FINAL_INS                       "../test/data/xsd/mode/type/type_complex.ins.group.final.xsd"
#define FILE_TYPE_COMPLEX_REF_START_MODIFY                      "../test/data/xsd/mode/type/type_complex.mod.ref.plain.start.xsd"
#define FILE_TYPE_COMPLEX_REF_FINAL_MODIFY                      "../test/data/xsd/mode/type/type_complex.mod.ref.plain.final.xsd"
#define FILE_TYPE_COMPLEX_REF_START_MODIFY_OTHER                "../test/data/xsd/mode/type/type_complex.mod.ref.other.start.xsd"
#define FILE_TYPE_COMPLEX_REF_FINAL_MODIFY_OTHER                "../test/data/xsd/mode/type/type_complex.mod.ref.other.final.xsd"
#define FILE_TYPE_START_INS                                     "../test/data/xsd/mode/type/type.ins.generic.start.xsd"

#define FILE_TYPE_START_SIMPLETYPE_MOD                      "../test/data/xsd/mode/type/type.mod.simple.start.xsd"
#define FILE_TYPE_FINAL_REF_CHILD_INS                       "../test/data/xsd/mode/type/type.ins.child.ref.final.xsd"
#define FILE_TYPE_FINAL_REF_SIBLING_INS                     "../test/data/xsd/mode/type/type.ins.sibling.ref.final.xsd"
//-------
#define FILE_TYPE_SIMPLE_FINAL_RESTR_CHILD_INS     "../test/data/xsd/mode/type/type.simple.ins.child.restr.final.xsd"
#define FILE_TYPE_SIMPLE_FINAL_LIST_CHILD_INS      "../test/data/xsd/mode/type/type.simple.ins.child.list.final.xsd"
#define FILE_TYPE_SIMPLE_FINAL_UNION_CHILD_INS     "../test/data/xsd/mode/type/type.simple.ins.child.union.final.xsd"
#define FILE_TYPE_SIMPLE_FINAL_RESTR_MOD           "../test/data/xsd/mode/type/type.simple.mod.restr.final.xsd"
#define FILE_TYPE_SIMPLE_FINAL_LIST_MOD           "../test/data/xsd/mode/type/type.simple.mod.list.final.xsd"
#define FILE_TYPE_SIMPLE_FINAL_UNION_MOD           "../test/data/xsd/mode/type/type.simple.mod.union.final.xsd"
///------------------------
#define FILE_ELEMENT_SIMPLE_FINAL_RESTR_CHILD_INS     "../test/data/xsd/mode/element.simple.ins.child.restr.final.xsd"
#define FILE_ELEMENT_SIMPLE_FINAL_LIST_CHILD_INS      "../test/data/xsd/mode/element.simple.ins.child.list.final.xsd"
#define FILE_ELEMENT_SIMPLE_FINAL_UNION_CHILD_INS     "../test/data/xsd/mode/element.simple.ins.child.union.final.xsd"
//  #define FILE_ELEMENT_START_SIMPLETYPE_MOD               "../test/data/xsd/mode/element.simple.mod.start.xsd"
#define FILE_ELEMENT_SIMPLE_FINAL_RESTR_MOD           "../test/data/xsd/mode/element.simple.mod.restr.final.xsd"
#define FILE_ELEMENT_SIMPLE_FINAL_LIST_MOD           "../test/data/xsd/mode/element.simple.mod.list.final.xsd"
#define FILE_ELEMENT_SIMPLE_FINAL_UNION_MOD           "../test/data/xsd/mode/element.simple.mod.union.final.xsd"
///------------------------
#define FILE_ELEMENT_SIMPLE_FINAL_LIST_INS              "../test/data/xsd/mode/element.simple.ins.list.final.xsd"
#define FILE_ELEMENT_SIMPLE_FINAL_UNION_INS             "../test/data/xsd/mode/element.simple.ins.union.final.xsd"
#define FILE_ELEMENT_SIMPLE_FINAL_LIST_MOD              "../test/data/xsd/mode/element.simple.mod.list.final.xsd"
#define FILE_ELEMENT_SIMPLE_FINAL_UNION_MOD              "../test/data/xsd/mode/element.simple.mod.union.final.xsd"
///------------------------

#define FILE_ELEMENT_START_INS                                 "../test/data/xsd/mode/element.ins.generic.start.xsd"
#define FILE_ELEMENT_START_SIMPLETYPE_MOD                      "../test/data/xsd/mode/element.mod.simpletype.generic.start.xsd"
///------------------------

#define FILE_ATTRIBUTE_START_INS            "../test/data/xsd/mode/attributes/start.ins.xsd"
#define FILE_ATTRIBUTE_START_MOD            "../test/data/xsd/mode/attributes/start.mod.xsd"
#define FILE_ATTRIBUTE_FINAL_CHILD_SIMPLE_INS   "../test/data/xsd/mode/attributes/ins.child.final.xsd"
#define FILE_ATTRIBUTE_FINAL_SIBLING_SIMPLE_INS "../test/data/xsd/mode/attributes/ins.sibling.final.xsd"
#define FILE_ATTRIBUTE_FINAL_REF_MOD        "../test/data/xsd/mode/attributes/mod.ref.final.xsd"
#define FILE_ATTRIBUTE_FINAL_RESTR_INS      "../test/data/xsd/mode/attributes/ins.restr.final.xsd"
#define FILE_ATTRIBUTE_FINAL_LIST_INS       "../test/data/xsd/mode/attributes/ins.list.final.xsd"
#define FILE_ATTRIBUTE_FINAL_UNION_INS      "../test/data/xsd/mode/attributes/ins.union.final.xsd"
#define FILE_ATTRIBUTE_FINAL_RESTR_MOD      "../test/data/xsd/mode/attributes/mod.restr.final.xsd"
#define FILE_ATTRIBUTE_FINAL_LIST_MOD       "../test/data/xsd/mode/attributes/mod.list.final.xsd"
#define FILE_ATTRIBUTE_FINAL_UNION_MOD      "../test/data/xsd/mode/attributes/mod.union.final.xsd"

#define FILE_DIALOG_START                   "../test/data/xsd/mode/dialog/schema.xsd"

///------------------------


TestXSDMode::TestXSDMode()
{
}

TestXSDMode::~TestXSDMode()
{
}

//------

bool TestXSDMode::testLast()
{
    _testName = "testLast";
    if(!testInsertChildElementComplexTypeSequence()) {
        return false;
    }
    return true;
}

bool TestXSDMode::testElement()
{
    _testName = "testElement";
    if(!testInsertChildElementSimple()) {
        return false;
    }
    if(!testInsertSiblingElementSimple()) {
        return false;
    }
    if(!testModifyRefElementSimple()) {
        return false;
    }
    if(!testModifyBareElementSimple()) {
        return false;
    }
    if(!testModifyAnonymousElementSimple()) {
        return false;
    }
    if(!testModifyComplexElementSimple()) {
        return false;
    }
    if(!testModifyRestrictionElementSimple()) {
        return false;
    }

    //---
    if(!testInsertElementSimpleTypeRestriction()) {
        return false;
    }

    if(!testInsertListElementSimple()) {
        return false;
    }

    if(!testInsertUnionElementSimple()) {
        return false;
    }
    //---

    if(!testModifyElementSimpleTypeRestriction()) {
        return false;
    }

    if(!testModifyListElementSimple()) {
        return false;
    }

    if(!testModifyUnionElementSimple()) {
        return false;
    }
    // ---

    // ---

    if(!testInsertChildElementComplexTypeReference()) {
        return false;
    }
    if(!testModChildElementComplexTypeReferenceExistingAnnotation()) {
        return false;
    }
    if(!testInsertChildElementComplexTypeComplexContentRestriction()) {
        return false;
    }
    if(!testInsertChildElementComplexTypeComplexContentExtension()) {
        return false;
    }
    if(!testInsertChildElementComplexTypeNone()) {
        return false;
    }
    if(!testInsertChildElementComplexTypeSequence()) {
        return false;
    }
    if(!testInsertChildElementComplexTypeChoice()) {
        return false;
    }
    if(!testInsertChildElementComplexTypeAll()) {
        return false;
    }
    if(!testInsertChildElementComplexTypeGroup()) {
        return false;
    }
    if(!testModifyChildElementComplexTypeReference()) {
        return false;
    }
    if(!testModifyChildElementComplexTypeReferenceOther()) {
        return false;
    }
    if(!testModifyChildElementComplexTypeComplexContentExtension()) {
        return false;
    }
    if( !testModifyChildElementComplexTypeComplexContentExtensionOther() ) {
        return false;
    }
    if(!testModifyChildElementComplexTypeComplexContentRestriction()) {
        return false;
    }
    if( !testModifyChildElementComplexTypeComplexContentRestrictionOther() ) {
        return false;
    }
    if(!testModifyChildElementComplexTypeSequence()) {
        return false;
    }
    if( !testModifyChildElementComplexTypeSequenceOther() ) {
        return false;
    }
    if(!testModifyChildElementComplexTypeChoice()) {
        return false;
    }
    if( !testModifyChildElementComplexTypeChoiceOther() ) {
        return false;
    }
    if(!testModifyChildElementComplexTypeGroup()) {
        return false;
    }
    if( !testModifyChildElementComplexTypeGroupOther() ) {
        return false;
    }
    if(!testModifyChildElementComplexTypeAll()) {
        return false;
    }
    if( !testModifyChildElementComplexTypeAllOther() ) {
        return false;
    }
    if(!testModifyChildElementComplexTypeNone()) {
        return false;
    }
    if( !testModifyChildElementComplexTypeNoneOther() ) {
        return false;
    }
    if(!testModifyChildElementComplexTypeSimpleContentExtension()) {
        return false;
    }
    if( !testModifyChildElementComplexTypeSimpleContentExtensionOther() ) {
        return false;
    }
    if(!testModifyChildElementComplexTypeSimpleContentRestriction()) {
        return false;
    }
    if( !testModifyChildElementComplexTypeSimpleContentRestrictionOther() ) {
        return false;
    }

    return true;
}

bool TestXSDMode::testType()
{
    _testName = "testType";

    if(!testInsertChildTypeSimpleTypeBare()) {
        return false;
    }

    if(!testInsertSiblingTypeSimpleTypeBare()) {
        return false;
    }

    //---
    if(!testInsertSimpleTypeRestriction()) {
        return false;
    }

    if(!testInsertListTypeSimple()) {
        return false;
    }

    if(!testInsertUnionTypeSimple()) {
        return false;
    }
    //---

    if(!testModifySimpleTypeRestriction()) {
        return false;
    }

    if(!testModifyListTypeSimple()) {
        return false;
    }

    if(!testModifyUnionTypeSimple()) {
        return false;
    }
    // ---

    if(!testInsertChildTypeComplexTypeComplexContentRestriction()) {
        return false;
    }
    if(!testInsertChildTypeComplexTypeComplexContentExtension()) {
        return false;
    }
    if(!testInsertChildTypeComplexTypeNone()) {
        return false;
    }
    if(!testInsertChildTypeComplexTypeSequence()) {
        return false;
    }
    if(!testInsertChildTypeComplexTypeChoice()) {
        return false;
    }
    if(!testInsertChildTypeComplexTypeAll()) {
        return false;
    }
    if(!testInsertChildTypeComplexTypeGroup()) {
        return false;
    }
    //---

    /*if(!testModifyChildTypeComplexTypeReference()) {
        return false;
    }
    if(!testModifyChildTypeComplexTypeReferenceOther()) {
        return false;
    }*/

    if(!testModifyChildTypeComplexTypeComplexContentExtension()) {
        return false;
    }
    if( !testModifyChildTypeComplexTypeComplexContentExtensionOther() ) {
        return false;
    }
    if(!testModifyChildTypeComplexTypeComplexContentRestriction()) {
        return false;
    }
    if( !testModifyChildTypeComplexTypeComplexContentRestrictionOther() ) {
        return false;
    }
    if(!testModifyChildTypeComplexTypeSequence()) {
        return false;
    }
    if( !testModifyChildTypeComplexTypeSequenceOther() ) {
        return false;
    }
    if(!testModifyChildTypeComplexTypeChoice()) {
        return false;
    }
    if( !testModifyChildTypeComplexTypeChoiceOther() ) {
        return false;
    }
    if(!testModifyChildTypeComplexTypeGroup()) {
        return false;
    }
    if( !testModifyChildTypeComplexTypeGroupOther() ) {
        return false;
    }
    if(!testModifyChildTypeComplexTypeAll()) {
        return false;
    }
    if( !testModifyChildTypeComplexTypeAllOther() ) {
        return false;
    }
    if(!testModifyChildTypeComplexTypeNone()) {
        return false;
    }
    if( !testModifyChildTypeComplexTypeNoneOther() ) {
        return false;
    }
    if(!testModifyChildTypeComplexTypeSimpleContentExtension()) {
        return false;
    }
    if( !testModifyChildTypeComplexTypeSimpleContentExtensionOther() ) {
        return false;
    }
    if(!testModifyChildTypeComplexTypeSimpleContentRestriction()) {
        return false;
    }
    if( !testModifyChildTypeComplexTypeSimpleContentRestrictionOther() ) {
        return false;
    }

    return true ;
}

bool TestXSDMode::testAttribute()
{
    _testName = "testAttribute";

    if(!testInsertChildAttributeSimple()) {
        return false;
    }
    if(!testInsertSiblingAttributeSimple()) {
        return false;
    }
    if(!testModifyRefAttributeSimple()) {
        return false;
    }

    //---
    if(!testInsertAttributeSimpleTypeRestriction()) {
        return false;
    }

    if(!testInsertListAttributeSimple()) {
        return false;
    }

    if(!testInsertUnionAttributeSimple()) {
        return false;
    }
    //---

    if(!testModifyAttributeSimpleTypeRestriction()) {
        return false;
    }

    if(!testModifyListAttributeSimple()) {
        return false;
    }

    if(!testModifyUnionAttributeSimple()) {
        return false;
    }
    // ---

    return true;
}


//-----

bool TestXSDMode::cfr(Regola *regola, const QString &step, const QString &fileResult)
{
    QByteArray resultData = regola->writeMemory();
    QDomDocument document1;
    QDomDocument document2;
    CompareXML compare;
    if(!compare.loadFileIntoDocument(fileResult, document1)) {
        return error(QString("step: %1, load file result %2").arg(step).arg(fileResult));
    }
    QBuffer outputData(&resultData);
    if(!compare.loadFileIntoDocument(&outputData, document2)) {
        return error(QString("step %1 load modified data").arg(step));
    }
    bool result = compare.compareDomDocuments(document1, document2);
    if( !result ) {
        compare.dumpErrorCause();
        return error(QString("Step: %1 comparing file with doc: %2").arg(step).arg(compare.errorString()));
    }
    return true ;
}

bool TestXSDMode::testSkeleton(const QString &fileStart, const QString &fileResult, const ElementOp::Op op, XSDOperationParameters *params, QList<int> &sel )
{
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(fileStart) ) {
        return error(QString("unable to load input file: '%1' ").arg(fileStart));
    }
    Regola *regola = app.mainWindow()->getRegola();
    Element *selectedElement = app.mainWindow()->getRegola()->findElementByArray(sel);
    if(NULL == selectedElement) {
        return error("no selected element");
    }
    app.mainWindow()->getEditor()->setCurrentItem(selectedElement);
    if(!app.mainWindow()->getEditor()->XSDApplyOperation(op, params)) {
        return error("operation failed");
    }
    if(!cfr(regola, "operation", fileResult)){
        return false;
    }
    regola->undo();
    if(!cfr(regola, "undo", fileStart)){
        return false;
    }
    regola->redo();
    if(!cfr(regola, "redo", fileResult)){
        return false;
    }
    return true;
}

//-----
// region(element) ------------------------------------------------------------------------------------------------

bool TestXSDMode::testInsertChildElementSimple()
{
    _testName = "testInsertChildElementSimple";
    XSDOperationParameters params;
    params.setObjType(XSDOperationParameters::EOI_ELEMENT);
    params.setTypeSpec(XSDOperationParameters::TS_SIMPLE);
    params.setTypeTo(XSDOperationParameters::EXT_REFERENCE_TYPE);
    params.setTypeName("xs:string");
    params.setName("test");

    QList<int> sel;
    sel<<1;
    if(!testSkeleton(FILE_ELEMENT_SIMPLE_INSERT_START, FILE_ELEMENT_SIMPLE_INSERT_CHILD_RESULT, ElementOp::EOInsChild, &params, sel ) ) {
        return false ;
    }
    return true;
}

bool TestXSDMode::testInsertSiblingElementSimple()
{
    _testName = "testInsertSiblingElementSimple";
    XSDOperationParameters params;
    params.setObjType(XSDOperationParameters::EOI_ELEMENT);
    params.setTypeSpec(XSDOperationParameters::TS_SIMPLE);
    params.setTypeTo(XSDOperationParameters::EXT_REFERENCE_TYPE);
    params.setTypeName("xs:string");
    params.setName("test");

    QList<int> sel;
    sel<<1<<0;
    if(!testSkeleton(FILE_ELEMENT_SIMPLE_INSERT_START, FILE_ELEMENT_SIMPLE_INSERT_SIBLING_RESULT, ElementOp::EOInsSibling, &params, sel ) ) {
        return false ;
    }
    return true;
}

bool TestXSDMode::testModifyRefElementSimple()
{
    _testName = "testModifyRefElementSimple";
    XSDOperationParameters params;
    params.setObjType(XSDOperationParameters::EOI_ELEMENT);
    params.setTypeSpec(XSDOperationParameters::TS_SIMPLE);
    params.setTypeTo(XSDOperationParameters::EXT_REFERENCE_TYPE);
    params.setTypeName("xs:string");
    params.setName("test");

    QList<int> sel;
    sel<<1<<0<<1<<1;
    if(!testSkeleton(FILE_ELEMENT_SIMPLE_START_REF, FILE_ELEMENT_SIMPLE_FINAL_REF, ElementOp::EOModify, &params, sel ) ) {
        return false ;
    }
    return true;
}

bool TestXSDMode::testModifyBareElementSimple()
{
    _testName = "testModifyBareElementSimple";
    XSDOperationParameters params;
    params.setObjType(XSDOperationParameters::EOI_ELEMENT);
    params.setTypeSpec(XSDOperationParameters::TS_SIMPLE);
    params.setTypeTo(XSDOperationParameters::EXT_REFERENCE_TYPE);
    params.setTypeName("xs:string");
    params.setName("test");

    QList<int> sel;
    sel<<1<<0<<1<<1;
    if(!testSkeleton(FILE_ELEMENT_SIMPLE_START_BARE, FILE_ELEMENT_SIMPLE_FINAL_BARE, ElementOp::EOModify, &params, sel ) ) {
        return false ;
    }
    return true;
}

bool TestXSDMode::testModifyAnonymousElementSimple()
{
    _testName = "testModifyAnonymousElementSimple";
    XSDOperationParameters params;
    params.setObjType(XSDOperationParameters::EOI_ELEMENT);
    params.setTypeSpec(XSDOperationParameters::TS_SIMPLE);
    params.setTypeTo(XSDOperationParameters::EXT_REFERENCE_TYPE);
    params.setTypeName("xs:string");
    params.setName("test");

    QList<int> sel;
    sel<<1<<0<<1<<1;
    if(!testSkeleton(FILE_ELEMENT_SIMPLE_START_AN, FILE_ELEMENT_SIMPLE_FINAL_AN, ElementOp::EOModify, &params, sel ) ) {
        return false ;
    }
    return true;
}

bool TestXSDMode::testModifyComplexElementSimple()
{
    _testName = "testModifyComplexElementSimple";
    XSDOperationParameters params;
    params.setObjType(XSDOperationParameters::EOI_ELEMENT);
    params.setTypeSpec(XSDOperationParameters::TS_SIMPLE);
    params.setTypeTo(XSDOperationParameters::EXT_REFERENCE_TYPE);
    params.setTypeName("xs:string");
    params.setName("test");

    QList<int> sel;
    sel<<1<<0<<1<<1;
    if(!testSkeleton(FILE_ELEMENT_SIMPLE_START_CPX, FILE_ELEMENT_SIMPLE_FINAL_CPX, ElementOp::EOModify, &params, sel ) ) {
        return false ;
    }
    return true;
}

bool TestXSDMode::testModifyRestrictionElementSimple()
{
    _testName = "testModifyRestrictionElementSimple";
    XSDOperationParameters params;
    params.setObjType(XSDOperationParameters::EOI_ELEMENT);
    params.setTypeSpec(XSDOperationParameters::TS_SIMPLE);
    params.setTypeTo(XSDOperationParameters::EXT_ANONYMOUS_TYPE);
    params.setSubOper(XSDOperationParameters::TSS_RESTRICTION);
    params.setTypeName("xs:string");
    params.setName("test");

    QList<int> sel;
    sel<<1<<0<<1<<1;
    if(!testSkeleton(FILE_ELEMENT_SIMPLE_START_RES, FILE_ELEMENT_SIMPLE_FINAL_RES, ElementOp::EOModify, &params, sel ) ) {
        return false ;
    }
    return true;
}
/*
bool TestXSDMode::testModifyExtensionElementSimple()
{
    _testName = "testModifyExtensionElementSimple";
    XSDOperationParameters params;
    params.setObjType(XSDOperationParameters::EOI_ELEMENT);
    params.setTypeSpec(XSDOperationParameters::TS_SIMPLE);
    params.setTypeTo(XSDOperationParameters::EXT_ANONYMOUS_TYPE);
    params.setSubOper(XSDOperationParameters::TSS_EXTENSION);
    params.setTypeName("xs:string");
    params.setName("test");

    QList<int> sel;
    sel<<1<<0<<1<<1;
    if(!testSkeleton(FILE_ELEMENT_SIMPLE_START_EXT, FILE_ELEMENT_SIMPLE_FINAL_EXT, ElementOp::EOModify, &params, sel ) ) {
        return false ;
    }
    return true;
}

bool TestXSDMode::testInsertExtensionElementSimple()
{
    _testName = "testInsertExtensionElementSimple";
    XSDOperationParameters params;
    params.setObjType(XSDOperationParameters::EOI_ELEMENT);
    params.setTypeSpec(XSDOperationParameters::TS_SIMPLE);
    params.setTypeTo(XSDOperationParameters::EXT_ANONYMOUS_TYPE);
    params.setSubOper(XSDOperationParameters::TSS_EXTENSION);
    params.setTypeName("xs:string");
    params.setName("test");

    QList<int> sel;
    sel<<1<<0<<1;
    if(!testSkeleton(FILE_ELEMENT_SIMPLE_START_EXT_INS, FILE_ELEMENT_SIMPLE_FINAL_EXT_INS, ElementOp::EOInsChild, &params, sel ) ) {
        return false ;
    }
    return true;
}
*/
bool TestXSDMode::testInsertChildElementComplexTypeReference()
{
    _testName = "testInsertChildElementComplexTypeReference";
    XSDOperationParameters params;
    params.setObjType(XSDOperationParameters::EOI_ELEMENT);
    params.setTypeSpec(XSDOperationParameters::TS_COMPLEX);
    params.setTypeTo(XSDOperationParameters::EXT_REFERENCE_TYPE);
    params.setSubOper(XSDOperationParameters::TSS_NONE);
    params.setTypeName("Parent");
    params.setName("test");

    QList<int> sel;
    sel<<1<<0<<1;
    if(!testSkeleton(FILE_ELEMENT_COMPLEX_START_REF, FILE_ELEMENT_COMPLEX_FINAL_REF, ElementOp::EOInsChild, &params, sel ) ) {
        return false ;
    }
    return true;
}

bool TestXSDMode::testModChildElementComplexTypeReferenceExistingAnnotation()
{
    _testName = "testModChildElementComplexTypeReferenceExistingAnnotation";
    XSDOperationParameters params;
    params.setObjType(XSDOperationParameters::EOI_ELEMENT);
    params.setTypeSpec(XSDOperationParameters::TS_COMPLEX);
    params.setTypeTo(XSDOperationParameters::EXT_REFERENCE_TYPE);
    params.setSubOper(XSDOperationParameters::TSS_NONE);
    params.setTypeName("Parent");
    params.setName("test");

    QList<int> sel;
    sel<<1<<0<<1<<1;
    if(!testSkeleton(FILE_ELEMENT_COMPLEX_START_REF_EA, FILE_ELEMENT_COMPLEX_FINAL_REF_EA, ElementOp::EOModify, &params, sel ) ) {
        return false ;
    }
    return true;
}

bool TestXSDMode::testModChildElementComplexTypeReferenceExistingAnnotationOther()
{
    _testName = "testModChildElementComplexTypeReferenceExistingAnnotationOther";
    XSDOperationParameters params;
    params.setObjType(XSDOperationParameters::EOI_ELEMENT);
    params.setTypeSpec(XSDOperationParameters::TS_COMPLEX);
    params.setTypeTo(XSDOperationParameters::EXT_REFERENCE_TYPE);
    params.setSubOper(XSDOperationParameters::TSS_NONE);
    params.setTypeName("Parent");
    params.setName("test");

    QList<int> sel;
    sel<<1<<0<<1<<1;
    if(!testSkeleton(FILE_ELEMENT_COMPLEX_START_REF_EAO, FILE_ELEMENT_COMPLEX_FINAL_REF_EAO, ElementOp::EOModify, &params, sel ) ) {
        return false ;
    }
    return true;
}

bool TestXSDMode::testInsertChildElementComplexTypeComplexContentRestriction()
{
    _testName = "testInsertChildElementComplexTypeComplexContentRestriction";
    XSDOperationParameters params;
    params.setObjType(XSDOperationParameters::EOI_ELEMENT);
    params.setTypeSpec(XSDOperationParameters::TS_COMPLEX);
    params.setTypeTo(XSDOperationParameters::EXT_ANONYMOUS_TYPE);
    params.setTypeContent(XSDOperationParameters::TSC_COMPLEX);
    params.setSubOper(XSDOperationParameters::TSS_RESTRICTION);
    params.setTypeName("Parent");
    params.setName("test");

    QList<int> sel;
    sel<<1<<0<<1;
    if(!testSkeleton(FILE_ELEMENT_COMPLEX_START_RES, FILE_ELEMENT_COMPLEX_FINAL_RES, ElementOp::EOInsChild, &params, sel ) ) {
        return false ;
    }
    return true;
}

bool TestXSDMode::testInsertChildElementComplexTypeComplexContentExtension()
{
    _testName = "testInsertChildElementComplexTypeComplexContentExtension";
    XSDOperationParameters params;
    params.setObjType(XSDOperationParameters::EOI_ELEMENT);
    params.setTypeSpec(XSDOperationParameters::TS_COMPLEX);
    params.setTypeTo(XSDOperationParameters::EXT_ANONYMOUS_TYPE);
    params.setTypeContent(XSDOperationParameters::TSC_COMPLEX);
    params.setSubOper(XSDOperationParameters::TSS_EXTENSION);
    params.setTypeName("Parent");
    params.setName("test");

    QList<int> sel;
    sel<<1<<0<<1;
    if(!testSkeleton(FILE_ELEMENT_COMPLEX_START_EXT, FILE_ELEMENT_COMPLEX_FINAL_EXT, ElementOp::EOInsChild, &params, sel ) ) {
        return false ;
    }
    return true;
}

bool TestXSDMode::testInsertChildElementComplexTypeNone()
{
    _testName = "testInsertChildElementComplexTypeComplexContentNone";
    XSDOperationParameters params;
    params.setObjType(XSDOperationParameters::EOI_ELEMENT);
    params.setTypeSpec(XSDOperationParameters::TS_COMPLEX);
    params.setTypeTo(XSDOperationParameters::EXT_ANONYMOUS_TYPE);
    params.setTypeContent(XSDOperationParameters::TSC_COMPLEX);
    params.setSubOper(XSDOperationParameters::TSS_NONE);
    params.setTypeName("Parent");
    params.setName("test");

    QList<int> sel;
    sel<<1<<0<<1;
    if(!testSkeleton(FILE_ELEMENT_COMPLEX_START_NONE, FILE_ELEMENT_COMPLEX_FINAL_NONE, ElementOp::EOInsChild, &params, sel ) ) {
        return false ;
    }
    return true;
}

bool TestXSDMode::testInsertChildElementComplexTypeSequence()
{
    _testName = "testInsertChildElementComplexTypeSequence";
    XSDOperationParameters params;
    params.setObjType(XSDOperationParameters::EOI_ELEMENT);
    params.setTypeSpec(XSDOperationParameters::TS_COMPLEX);
    params.setTypeTo(XSDOperationParameters::EXT_ANONYMOUS_TYPE);
    params.setTypeContent(XSDOperationParameters::TSC_NONE);
    params.setSubOper(XSDOperationParameters::TSS_SEQUENCE);
    params.setTypeName("Parent");
    params.setName("test");

    QList<int> sel;
    sel<<1<<0<<1;
    if(!testSkeleton(FILE_ELEMENT_COMPLEX_START_NONE, FILE_ELEMENT_COMPLEX_FINAL_SEQUENCE, ElementOp::EOInsChild, &params, sel ) ) {
        return false ;
    }
    return true;
}

bool TestXSDMode::testInsertChildElementComplexTypeChoice()
{
    _testName = "testInsertChildElementComplexTypeChoice";
    XSDOperationParameters params;
    params.setObjType(XSDOperationParameters::EOI_ELEMENT);
    params.setTypeSpec(XSDOperationParameters::TS_COMPLEX);
    params.setTypeTo(XSDOperationParameters::EXT_ANONYMOUS_TYPE);
    params.setTypeContent(XSDOperationParameters::TSC_NONE);
    params.setSubOper(XSDOperationParameters::TSS_CHOICE);
    params.setTypeName("Parent");
    params.setName("test");

    QList<int> sel;
    sel<<1<<0<<1;
    if(!testSkeleton(FILE_ELEMENT_COMPLEX_START_NONE, FILE_ELEMENT_COMPLEX_FINAL_CHOICE, ElementOp::EOInsChild, &params, sel ) ) {
        return false ;
    }
    return true;
}

bool TestXSDMode::testInsertChildElementComplexTypeAll()
{
    _testName = "testInsertChildElementComplexTypeAll";
    XSDOperationParameters params;
    params.setObjType(XSDOperationParameters::EOI_ELEMENT);
    params.setTypeSpec(XSDOperationParameters::TS_COMPLEX);
    params.setTypeTo(XSDOperationParameters::EXT_ANONYMOUS_TYPE);
    params.setTypeContent(XSDOperationParameters::TSC_NONE);
    params.setSubOper(XSDOperationParameters::TSS_ALL);
    params.setTypeName("Parent");
    params.setName("test");

    QList<int> sel;
    sel<<1<<0<<1;
    if(!testSkeleton(FILE_ELEMENT_COMPLEX_START_NONE, FILE_ELEMENT_COMPLEX_FINAL_ALL, ElementOp::EOInsChild, &params, sel ) ) {
        return false ;
    }
    return true;
}

bool TestXSDMode::testInsertChildElementComplexTypeGroup()
{
    _testName = "testInsertChildElementComplexTypeGroup";
    XSDOperationParameters params;
    params.setObjType(XSDOperationParameters::EOI_ELEMENT);
    params.setTypeSpec(XSDOperationParameters::TS_COMPLEX);
    params.setTypeTo(XSDOperationParameters::EXT_ANONYMOUS_TYPE);
    params.setTypeContent(XSDOperationParameters::TSC_NONE);
    params.setSubOper(XSDOperationParameters::TSS_GROUP);
    params.setTypeName("Parent");
    params.setName("test");

    QList<int> sel;
    sel<<1<<0<<1;
    if(!testSkeleton(FILE_ELEMENT_COMPLEX_START_NONE, FILE_ELEMENT_COMPLEX_FINAL_GROUP, ElementOp::EOInsChild, &params, sel ) ) {
        return false ;
    }
    return true;
}

void TestXSDMode::fillParamsBaseElementComplexType(XSDOperationParameters &params, QList<int> &sel, const  XSDOperationParameters::EXSDTypeContent typeContent)
{
    params.setObjType(XSDOperationParameters::EOI_ELEMENT);
    params.setTypeSpec(XSDOperationParameters::TS_COMPLEX);
    params.setTypeTo(XSDOperationParameters::EXT_ANONYMOUS_TYPE);
    params.setTypeContent(typeContent);
    params.setSubOper(XSDOperationParameters::TSS_NONE);
    params.setTypeName("Parent");
    params.setName("test");

    sel<<1<<0<<1<<1;
}

void TestXSDMode::fillParamsBaseTypeComplexType(XSDOperationParameters &params, QList<int> &sel, const  XSDOperationParameters::EXSDTypeContent typeContent)
{
    params.setObjType(XSDOperationParameters::EOI_TYPE);
    params.setTypeSpec(XSDOperationParameters::TS_COMPLEX);
    params.setTypeTo(XSDOperationParameters::EXT_ANONYMOUS_TYPE);
    params.setTypeContent(typeContent);
    params.setSubOper(XSDOperationParameters::TSS_NONE);
    params.setTypeName("Parent");
    params.setName("test");

    sel<<1<<1;
}

void TestXSDMode::fillParamsBaseTypeSimpleType(XSDOperationParameters &params, QList<int> &sel)
{
    params.setObjType(XSDOperationParameters::EOI_TYPE);
    params.setTypeSpec(XSDOperationParameters::TS_SIMPLE);
    params.setTypeTo(XSDOperationParameters::EXT_ANONYMOUS_TYPE);
    params.setTypeContent(XSDOperationParameters::TSC_NONE);
    params.setSubOper(XSDOperationParameters::TSS_NONE);
    params.setTypeName("Parent");
    params.setName("test");

    sel<<1<<1;
}

void TestXSDMode::fillParamsBaseElementSimpleType(XSDOperationParameters &params, QList<int> &sel)
{
    params.setObjType(XSDOperationParameters::EOI_ELEMENT);
    params.setTypeSpec(XSDOperationParameters::TS_SIMPLE);
    params.setTypeTo(XSDOperationParameters::EXT_ANONYMOUS_TYPE);
    params.setTypeContent(XSDOperationParameters::TSC_NONE);
    params.setSubOper(XSDOperationParameters::TSS_NONE);
    params.setTypeName("Parent");
    params.setName("test");

    sel<<1;
}


//---

bool TestXSDMode::testInsertChildTypeSimpleType(const QString &theTestName, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile)
{
    _testName = theTestName;
    QList<int> sel;
    XSDOperationParameters params;
    fillParamsBaseTypeSimpleType(params, sel);
    params.setSubOper(subOper);
    sel.clear();
    sel << 1;
    if(!testSkeleton(theStartFile, theFinalFile, ElementOp::EOInsChild, &params, sel ) ) {
        return false ;
    }
    return true;
}


bool TestXSDMode::testModifyTypeSimpleType(const QString &theTestName, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile)
{
    _testName = theTestName;
    QList<int> sel;
    XSDOperationParameters params;
    fillParamsBaseTypeSimpleType(params, sel);
    params.setSubOper(subOper);
    if(!testSkeleton(theStartFile, theFinalFile, ElementOp::EOModify, &params, sel ) ) {
        return false ;
    }
    return true;
}

bool TestXSDMode::testModifyElementSimpleType(const QString &theTestName, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile)
{
    _testName = theTestName;
    QList<int> sel;
    XSDOperationParameters params;
    fillParamsBaseElementSimpleType(params, sel);
    sel.clear();
    sel <<1 <<1 ;
    params.setSubOper(subOper);
    if(!testSkeleton(theStartFile, theFinalFile, ElementOp::EOModify, &params, sel ) ) {
        return false ;
    }
    return true;
}

bool TestXSDMode::testInsertSiblingTypeSimpleType(const QString &theTestName, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile)
{
    _testName = theTestName;
    QList<int> sel;
    XSDOperationParameters params;
    fillParamsBaseTypeSimpleType(params, sel);
    params.setSubOper(subOper);
    sel.clear();
    sel<< 1 << 0 ;
    if(!testSkeleton(theStartFile, theFinalFile, ElementOp::EOInsSibling, &params, sel ) ) {
        return false ;
    }
    return true;
}

bool TestXSDMode::testModifyChildElementComplexType(const QString &theTestName, const XSDOperationParameters::EXSDTypeContent typeContent, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile, const bool isReference)
{
    _testName = theTestName;
    QList<int> sel;
    XSDOperationParameters params;
    fillParamsBaseElementComplexType(params, sel, typeContent);
    params.setSubOper(subOper);
    if( isReference ) {
        params.setTypeTo(XSDOperationParameters::EXT_REFERENCE_TYPE);
    }
    if(!testSkeleton(theStartFile, theFinalFile, ElementOp::EOModify, &params, sel ) ) {
        return false ;
    }
    return true;
}

bool TestXSDMode::testModifyChildTypeComplexType(const QString &theTestName, const XSDOperationParameters::EXSDTypeContent typeContent, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile, const bool isReference )
{
    _testName = theTestName;
    QList<int> sel;
    XSDOperationParameters params;
    fillParamsBaseTypeComplexType(params, sel, typeContent);
    params.setSubOper(subOper);
    if( isReference ) {
        params.setTypeTo(XSDOperationParameters::EXT_REFERENCE_TYPE);
    }
    if(!testSkeleton(theStartFile, theFinalFile, ElementOp::EOModify, &params, sel ) ) {
        return false ;
    }
    return true;
}

bool TestXSDMode::testInsertChildTypeComplexType(const QString &theTestName, const XSDOperationParameters::EXSDTypeContent typeContent, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile, const bool isReference )
{
    _testName = theTestName;
    QList<int> sel;
    XSDOperationParameters params;
    fillParamsBaseTypeComplexType(params, sel, typeContent);
    params.setSubOper(subOper);
    sel.clear();
    sel<< 1 ;
    if( isReference ) {
        params.setTypeTo(XSDOperationParameters::EXT_REFERENCE_TYPE);
    }
    if(!testSkeleton(theStartFile, theFinalFile, ElementOp::EOInsChild, &params, sel ) ) {
        return false ;
    }
    return true;
}
bool TestXSDMode::testModifyChildElementComplexTypeSimpleContent(const QString &theTestName, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile, const bool isReference)
{
    _testName = theTestName;
    QList<int> sel;
    XSDOperationParameters params;
    fillParamsBaseElementComplexType(params, sel, XSDOperationParameters::TSC_SIMPLE);
    params.setSubOper(subOper);
    if(isReference) {
        params.setTypeTo(XSDOperationParameters::EXT_REFERENCE_TYPE);
    }
    if(!testSkeleton(theStartFile, theFinalFile, ElementOp::EOModify, &params, sel ) ) {
        return false ;
    }
    return true;
}

//--- region(attributes)

void TestXSDMode::fillParamsAttribute(XSDOperationParameters &params, QList<int> &sel)
{
    params.setObjType(XSDOperationParameters::EOI_ATTRIBUTE);
    params.setTypeSpec(XSDOperationParameters::TS_SIMPLE);
    params.setTypeTo(XSDOperationParameters::EXT_ANONYMOUS_TYPE);
    params.setTypeContent(XSDOperationParameters::TSC_NONE);
    params.setSubOper(XSDOperationParameters::TSS_NONE);
    params.setTypeName("Parent");
    params.setName("test");

    sel<<1<<0<<2;
}

bool TestXSDMode::testInsertChildAttribute(const QString &theTestName, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile)
{
    _testName = theTestName;
    QList<int> sel;
    XSDOperationParameters params;
    fillParamsAttribute(params, sel);
    params.setSubOper(subOper);
    sel.clear();
    sel << 1 << 0 ;
    if(!testSkeleton(theStartFile, theFinalFile, ElementOp::EOInsChild, &params, sel ) ) {
        return false ;
    }
    return true;
}

bool TestXSDMode::testInsertSiblingAttribute(const QString &theTestName, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile)
{
    _testName = theTestName;
    QList<int> sel;
    XSDOperationParameters params;
    fillParamsAttribute(params, sel);
    params.setSubOper(subOper);
    sel.clear();
    sel << 1 << 0 << 1 ;
    if(!testSkeleton(theStartFile, theFinalFile, ElementOp::EOInsSibling, &params, sel ) ) {
        return false ;
    }
    return true;
}

bool TestXSDMode::testModifyAttributeSimpleType(const QString &theTestName, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile)
{
    _testName = theTestName;
    QList<int> sel;
    XSDOperationParameters params;
    fillParamsAttribute(params, sel);
    params.setSubOper(subOper);
    sel.clear();
    sel << 1 << 0 << 2 ;
    if(!testSkeleton(theStartFile, theFinalFile, ElementOp::EOModify, &params, sel ) ) {
        return false ;
    }
    return true;
}

bool TestXSDMode::testInsertChildAttributeSimple()
{
    return testInsertChildAttribute("testInsertChildAttributeSimple",
                                             XSDOperationParameters::TSS_NONE, FILE_ATTRIBUTE_START_INS, FILE_ATTRIBUTE_FINAL_CHILD_SIMPLE_INS);
}

bool TestXSDMode::testInsertSiblingAttributeSimple()
{
    return testInsertSiblingAttribute("testInsertSiblingAttributeSimple",
                                             XSDOperationParameters::TSS_NONE, FILE_ATTRIBUTE_START_INS, FILE_ATTRIBUTE_FINAL_SIBLING_SIMPLE_INS);
}

bool TestXSDMode::testModifyRefAttributeSimple()
{
    return testModifyAttributeSimpleType("testModifyRefAttributeSimple",
                                             XSDOperationParameters::TSS_NONE, FILE_ATTRIBUTE_START_MOD, FILE_ATTRIBUTE_FINAL_REF_MOD);
}

bool TestXSDMode::testInsertAttributeSimpleTypeRestriction()
{
    return testInsertChildAttribute("testInsertAttributeSimpleTypeRestriction",
                                             XSDOperationParameters::TSS_RESTRICTION, FILE_ATTRIBUTE_START_INS, FILE_ATTRIBUTE_FINAL_RESTR_INS);
}

bool TestXSDMode::testInsertListAttributeSimple()
{
    return testInsertChildAttribute("testInsertListAttributeSimple",
                                             XSDOperationParameters::TSS_LIST, FILE_ATTRIBUTE_START_INS, FILE_ATTRIBUTE_FINAL_LIST_INS);
}

bool TestXSDMode::testInsertUnionAttributeSimple()
{
    return testInsertChildAttribute("testInsertUnionAttributeSimple",
                                             XSDOperationParameters::TSS_UNION, FILE_ATTRIBUTE_START_INS, FILE_ATTRIBUTE_FINAL_UNION_INS);
}

bool TestXSDMode::testModifyAttributeSimpleTypeRestriction()
{
    return testModifyAttributeSimpleType("testModifyAttributeSimpleTypeRestriction",
                                             XSDOperationParameters::TSS_RESTRICTION, FILE_ATTRIBUTE_START_MOD, FILE_ATTRIBUTE_FINAL_RESTR_MOD);
}

bool TestXSDMode::testModifyListAttributeSimple()
{
    return testModifyAttributeSimpleType("testModifyListAttributeSimple",
                                             XSDOperationParameters::TSS_LIST, FILE_ATTRIBUTE_START_MOD, FILE_ATTRIBUTE_FINAL_LIST_MOD);
}

bool TestXSDMode::testModifyUnionAttributeSimple()
{
    return testModifyAttributeSimpleType("testModifyUnionAttributeSimple",
                                             XSDOperationParameters::TSS_UNION, FILE_ATTRIBUTE_START_MOD, FILE_ATTRIBUTE_FINAL_UNION_MOD);
}

//-- endregion(attributes)

bool TestXSDMode::testInsertChildTypeSimpleTypeBare()
{
    return testInsertChildTypeSimpleType("testInsertChildTypeSimpleTypeBare",
                                             XSDOperationParameters::TSS_NONE, FILE_TYPE_START_INS, FILE_TYPE_FINAL_REF_CHILD_INS);
}

bool TestXSDMode::testInsertSiblingTypeSimpleTypeBare()
{
    return testInsertSiblingTypeSimpleType("testInsertSiblingTypeSimpleTypeBare",
                                             XSDOperationParameters::TSS_NONE, FILE_TYPE_START_INS, FILE_TYPE_FINAL_REF_SIBLING_INS);
}

bool TestXSDMode::testModifySimpleTypeRestriction()
{
    return testModifyTypeSimpleType("testModifySimpleTypeRestriction",
                                             XSDOperationParameters::TSS_RESTRICTION, FILE_TYPE_START_SIMPLETYPE_MOD, FILE_TYPE_SIMPLE_FINAL_RESTR_MOD);
}

bool TestXSDMode::testInsertSimpleTypeRestriction()
{
    return testInsertChildTypeSimpleType("testInsertSimpleTypeRestriction",
                                             XSDOperationParameters::TSS_RESTRICTION, FILE_TYPE_START_INS, FILE_TYPE_SIMPLE_FINAL_RESTR_CHILD_INS);
}

bool TestXSDMode::testModifyElementSimpleTypeRestriction()
{
    return testModifyElementSimpleType("testModifyElementSimpleTypeRestriction",
                                             XSDOperationParameters::TSS_RESTRICTION, FILE_ELEMENT_START_SIMPLETYPE_MOD, FILE_ELEMENT_SIMPLE_FINAL_RESTR_MOD);
}

bool TestXSDMode::testInsertElementSimpleTypeRestriction()
{
    return testInsertChildElementSimpleType("testInsertElementSimpleTypeRestriction",
                                             XSDOperationParameters::TSS_RESTRICTION, FILE_ELEMENT_START_INS, FILE_ELEMENT_SIMPLE_FINAL_RESTR_CHILD_INS);
}

bool TestXSDMode::testModifyListTypeSimple()
{
    return testModifyTypeSimpleType("testModifyListTypeSimple",
                                             XSDOperationParameters::TSS_LIST, FILE_TYPE_START_SIMPLETYPE_MOD, FILE_TYPE_SIMPLE_FINAL_LIST_MOD);
}

bool TestXSDMode::testModifyListElementSimple()
{
    return testModifyElementSimpleType("testModifyListElementSimple",
                                             XSDOperationParameters::TSS_LIST, FILE_ELEMENT_START_SIMPLETYPE_MOD, FILE_ELEMENT_SIMPLE_FINAL_LIST_MOD);
}

bool TestXSDMode::testInsertListElementSimple()
{
    return testInsertChildElementSimpleType("testInsertListElementSimple",
                                             XSDOperationParameters::TSS_LIST, FILE_ELEMENT_START_INS, FILE_ELEMENT_SIMPLE_FINAL_LIST_INS);
}

bool TestXSDMode::testInsertListTypeSimple()
{
    return testInsertChildTypeSimpleType("testInsertListTypeSimple",
                                             XSDOperationParameters::TSS_LIST, FILE_TYPE_START_INS, FILE_TYPE_SIMPLE_FINAL_LIST_CHILD_INS);
}


bool TestXSDMode::testModifyUnionElementSimple()
{
    return testModifyElementSimpleType("testModifyUnionElementSimple",
                                             XSDOperationParameters::TSS_UNION, FILE_ELEMENT_START_SIMPLETYPE_MOD, FILE_ELEMENT_SIMPLE_FINAL_UNION_MOD);
}

bool TestXSDMode::testModifyUnionTypeSimple()
{
    return testModifyTypeSimpleType("testModifyUnionTypeSimple",
                                             XSDOperationParameters::TSS_UNION, FILE_TYPE_START_SIMPLETYPE_MOD, FILE_TYPE_SIMPLE_FINAL_UNION_MOD);
}

bool TestXSDMode::testInsertUnionElementSimple()
{
    return testInsertChildElementSimpleType("testInsertUnionElementSimple",
                                             XSDOperationParameters::TSS_UNION, FILE_ELEMENT_START_INS, FILE_ELEMENT_SIMPLE_FINAL_UNION_INS);
}

bool TestXSDMode::testInsertUnionTypeSimple()
{
    return testInsertChildTypeSimpleType("testInsertUnionTypeSimple",
                                             XSDOperationParameters::TSS_UNION, FILE_TYPE_START_INS, FILE_TYPE_SIMPLE_FINAL_UNION_CHILD_INS);
}

bool TestXSDMode::testInsertChildElementSimpleType(const QString &theTestName, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile)
{
    _testName = theTestName;
    QList<int> sel;
    XSDOperationParameters params;
    fillParamsBaseElementSimpleType(params, sel);
    params.setSubOper(subOper);
    params.setTypeContent(XSDOperationParameters::TSC_SIMPLE);
    if(!testSkeleton(theStartFile, theFinalFile, ElementOp::EOInsChild, &params, sel ) ) {
        return false ;
    }
    return true;
}

/*
bool TestXSDMode::testModifyChildTypeComplexTypeSimpleContent(const QString &theTestName, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile)
{
    _testName = theTestName;
    QList<int> sel;
    XSDOperationParameters params;
    fillParamsBaseTypeComplexType(params, sel);
    params.setSubOper(subOper);
    params.setTypeContent(XSDOperationParameters::TSC_SIMPLE);
    if(!testSkeleton(theStartFile, theFinalFile, ElementOp::EOModify, &params, sel ) ) {
        return false ;
    }
    return true;
}
*/
bool TestXSDMode::testModifyChildTypeComplexTypeSimpleContent(const QString &theTestName, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile, const bool isReference)
{
    _testName = theTestName;
    QList<int> sel;
    XSDOperationParameters params;
    fillParamsBaseTypeComplexType(params, sel);
    params.setSubOper(subOper);
    params.setTypeContent(XSDOperationParameters::TSC_SIMPLE);
    if( isReference ) {
        params.setTypeTo(XSDOperationParameters::EXT_REFERENCE_TYPE);
    }
    if(!testSkeleton(theStartFile, theFinalFile, ElementOp::EOModify, &params, sel ) ) {
        return false ;
    }
    return true;
}

bool TestXSDMode::testModifyChildElementComplexTypeReference()
{
    return testModifyChildElementComplexType("testModifyChildElementComplexTypeReference", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_NONE, FILE_ELEMENT_COMPLEX_REF_START_MODIFY, FILE_ELEMENT_COMPLEX_REF_FINAL_MODIFY, true);
}

bool TestXSDMode::testModifyChildElementComplexTypeReferenceOther()
{
    return testModifyChildElementComplexType("testModifyChildElementComplexTypeReferenceOther", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_NONE, FILE_ELEMENT_COMPLEX_REF_START_MODIFY_OTHER, FILE_ELEMENT_COMPLEX_REF_FINAL_MODIFY_OTHER, true);
}

bool TestXSDMode::testModifyChildElementComplexTypeComplexContentExtension()
{
    return testModifyChildElementComplexType("testModifyChildElementComplexTypeComplexContentExtension", XSDOperationParameters::TSC_COMPLEX,
                                             XSDOperationParameters::TSS_EXTENSION, FILE_ELEMENT_COMPLEX_EXT_START_MODIFY, FILE_ELEMENT_COMPLEX_EXT_FINAL_MODIFY);
}

bool TestXSDMode::testModifyChildElementComplexTypeComplexContentExtensionOther()
{
    return testModifyChildElementComplexType("testModifyChildElementComplexTypeComplexContentExtensionOther", XSDOperationParameters::TSC_COMPLEX,
                                             XSDOperationParameters::TSS_EXTENSION, FILE_ELEMENT_COMPLEX_EXT_START_MODIFY_OTHER, FILE_ELEMENT_COMPLEX_EXT_FINAL_MODIFY_OTHER);
}


bool TestXSDMode::testModifyChildElementComplexTypeComplexContentRestriction()
{
    return testModifyChildElementComplexType("testModifyChildElementComplexTypeComplexContentRestriction", XSDOperationParameters::TSC_COMPLEX,
                                             XSDOperationParameters::TSS_RESTRICTION, FILE_ELEMENT_COMPLEX_RES_START_MODIFY, FILE_ELEMENT_COMPLEX_RES_FINAL_MODIFY);
}

bool TestXSDMode::testModifyChildElementComplexTypeComplexContentRestrictionOther()
{
    return testModifyChildElementComplexType("testModifyChildElementComplexTypeComplexContentRestrictionOther", XSDOperationParameters::TSC_COMPLEX,
                                             XSDOperationParameters::TSS_RESTRICTION, FILE_ELEMENT_COMPLEX_RES_START_MODIFY_OTHER, FILE_ELEMENT_COMPLEX_RES_FINAL_MODIFY_OTHER);
}


bool TestXSDMode::testModifyChildElementComplexTypeSequence()
{
    return testModifyChildElementComplexType("testModifyChildElementComplexTypeSequence", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_SEQUENCE, FILE_ELEMENT_COMPLEX_SEQ_START_MODIFY, FILE_ELEMENT_COMPLEX_SEQ_FINAL_MODIFY);
}

bool TestXSDMode::testModifyChildElementComplexTypeSequenceOther()
{
    return testModifyChildElementComplexType("testModifyChildElementComplexTypeSequenceOther", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_SEQUENCE, FILE_ELEMENT_COMPLEX_SEQ_START_MODIFY_OTHER, FILE_ELEMENT_COMPLEX_SEQ_FINAL_MODIFY_OTHER);
}

bool TestXSDMode::testModifyChildElementComplexTypeChoice()
{
    return testModifyChildElementComplexType("testModifyChildElementComplexTypeChoice", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_CHOICE, FILE_ELEMENT_COMPLEX_CHOICE_START_MODIFY, FILE_ELEMENT_COMPLEX_CHOICE_FINAL_MODIFY);
}

bool TestXSDMode::testModifyChildElementComplexTypeChoiceOther()
{
    return testModifyChildElementComplexType("testModifyChildElementComplexTypeChoiceOther", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_CHOICE, FILE_ELEMENT_COMPLEX_CHOICE_START_MODIFY_OTHER, FILE_ELEMENT_COMPLEX_CHOICE_FINAL_MODIFY_OTHER);
}

bool TestXSDMode::testModifyChildElementComplexTypeAll()
{
    return testModifyChildElementComplexType("testModifyChildElementComplexTypeAll", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_ALL, FILE_ELEMENT_COMPLEX_ALL_START_MODIFY, FILE_ELEMENT_COMPLEX_ALL_FINAL_MODIFY);
}

bool TestXSDMode::testModifyChildElementComplexTypeAllOther()
{
    return testModifyChildElementComplexType("testModifyChildElementComplexTypeAllOther", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_ALL, FILE_ELEMENT_COMPLEX_ALL_START_MODIFY_OTHER, FILE_ELEMENT_COMPLEX_ALL_FINAL_MODIFY_OTHER);
}

bool TestXSDMode::testModifyChildElementComplexTypeGroup()
{
    return testModifyChildElementComplexType("testModifyChildElementComplexTypeGroup", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_GROUP, FILE_ELEMENT_COMPLEX_GROUP_START_MODIFY, FILE_ELEMENT_COMPLEX_GROUP_FINAL_MODIFY);
}

bool TestXSDMode::testModifyChildElementComplexTypeGroupOther()
{
    return testModifyChildElementComplexType("testModifyChildElementComplexTypeGroupOther", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_GROUP, FILE_ELEMENT_COMPLEX_GROUP_START_MODIFY_OTHER, FILE_ELEMENT_COMPLEX_GROUP_FINAL_MODIFY_OTHER);
}

bool TestXSDMode::testModifyChildElementComplexTypeNone()
{
    return testModifyChildElementComplexType("testModifyChildElementComplexTypeNone", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_NONE, FILE_ELEMENT_COMPLEX_NONE_START_MODIFY, FILE_ELEMENT_COMPLEX_NONE_FINAL_MODIFY);
}

bool TestXSDMode::testModifyChildElementComplexTypeNoneOther()
{
    return testModifyChildElementComplexType("testModifyChildElementComplexTypeNoneOther", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_NONE, FILE_ELEMENT_COMPLEX_NONE_START_MODIFY_OTHER, FILE_ELEMENT_COMPLEX_NONE_FINAL_MODIFY_OTHER);
}

bool TestXSDMode::testModifyChildElementComplexTypeSimpleContentExtension()
{
    return testModifyChildElementComplexTypeSimpleContent("testModifyChildElementComplexTypeSimpleContentExtension",
                                             XSDOperationParameters::TSS_EXTENSION, FILE_ELEMENT_COMPLEX_SIMPLEEXT_START_MODIFY, FILE_ELEMENT_COMPLEX_SIMPLEEXT_FINAL_MODIFY);
}

bool TestXSDMode::testModifyChildElementComplexTypeSimpleContentExtensionOther()
{
    return testModifyChildElementComplexTypeSimpleContent("testModifyChildElementComplexTypeSimpleContentExtensionOther",
                                             XSDOperationParameters::TSS_EXTENSION, FILE_ELEMENT_COMPLEX_SIMPLEEXT_START_MODIFY_OTHER, FILE_ELEMENT_COMPLEX_SIMPLEEXT_FINAL_MODIFY_OTHER);
}

bool TestXSDMode::testModifyChildElementComplexTypeSimpleContentRestriction()
{
    return testModifyChildElementComplexTypeSimpleContent("testModifyChildElementComplexTypeSimpleContentRestriction",
                                             XSDOperationParameters::TSS_RESTRICTION, FILE_ELEMENT_COMPLEX_SIMPLERES_START_MODIFY, FILE_ELEMENT_COMPLEX_SIMPLERES_FINAL_MODIFY);
}

bool TestXSDMode::testModifyChildElementComplexTypeSimpleContentRestrictionOther()
{
    return testModifyChildElementComplexTypeSimpleContent("testModifyChildElementComplexTypeSimpleContentRestrictionOther",
                                             XSDOperationParameters::TSS_RESTRICTION, FILE_ELEMENT_COMPLEX_SIMPLERES_START_MODIFY_OTHER, FILE_ELEMENT_COMPLEX_SIMPLERES_FINAL_MODIFY_OTHER);
}
// endregion(element) ------------------------------------------------------------------------------------------------

// region(type) ------------------------------------------------------------------------------------------------



bool TestXSDMode::testInsertChildTypeComplexTypeComplexContentRestriction()
{
    return testInsertChildTypeComplexType("testInsertChildTypeComplexTypeComplexContentRestriction", XSDOperationParameters::TSC_COMPLEX,
                                             XSDOperationParameters::TSS_RESTRICTION, FILE_TYPE_START_INS, FILE_TYPE_COMPLEX_COMPLEX_CONTENT_RESTR_FINAL_INS );
}

bool TestXSDMode::testInsertChildTypeComplexTypeComplexContentExtension()
{
    return testInsertChildTypeComplexType("testInsertChildTypeComplexTypeComplexContentExtension", XSDOperationParameters::TSC_COMPLEX,
                                             XSDOperationParameters::TSS_EXTENSION, FILE_TYPE_START_INS, FILE_TYPE_COMPLEX_COMPLEX_CONTENT_EXTENSION_FINAL_INS );
}


bool TestXSDMode::testInsertChildTypeComplexTypeNone()
{
    return testInsertChildTypeComplexType("testInsertChildTypeComplexTypeNone", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_NONE, FILE_TYPE_START_INS, FILE_TYPE_COMPLEX_NONE_FINAL_INS );
}

bool TestXSDMode::testInsertChildTypeComplexTypeSequence()
{
    return testInsertChildTypeComplexType("testInsertChildTypeComplexTypeSequence", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_SEQUENCE, FILE_TYPE_START_INS, FILE_TYPE_COMPLEX_SEQ_FINAL_INS );
}

bool TestXSDMode::testInsertChildTypeComplexTypeChoice()
{
    return testInsertChildTypeComplexType("testInsertChildTypeComplexTypeChoice", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_CHOICE, FILE_TYPE_START_INS, FILE_TYPE_COMPLEX_CHOICE_FINAL_INS );
}

bool TestXSDMode::testInsertChildTypeComplexTypeAll()
{
    return testInsertChildTypeComplexType("testInsertChildTypeComplexTypeAll", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_ALL, FILE_TYPE_START_INS, FILE_TYPE_COMPLEX_ALL_FINAL_INS );
}

bool TestXSDMode::testInsertChildTypeComplexTypeGroup()
{
    return testInsertChildTypeComplexType("testInsertChildTypeComplexTypeGroup", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_GROUP, FILE_TYPE_START_INS, FILE_TYPE_COMPLEX_GROUP_FINAL_INS );
}

bool TestXSDMode::testModifyChildTypeComplexTypeComplexContentExtension()
{
    return testModifyChildTypeComplexType("testModifyChildTypeComplexTypeComplexContentExtension", XSDOperationParameters::TSC_COMPLEX,
                                             XSDOperationParameters::TSS_EXTENSION, FILE_TYPE_COMPLEX_EXT_START_MODIFY, FILE_TYPE_COMPLEX_EXT_FINAL_MODIFY);
}

bool TestXSDMode::testModifyChildTypeComplexTypeComplexContentExtensionOther()
{
    return testModifyChildTypeComplexType("testModifyChildTypeComplexTypeComplexContentExtensionOther", XSDOperationParameters::TSC_COMPLEX,
                                             XSDOperationParameters::TSS_EXTENSION, FILE_TYPE_COMPLEX_EXT_START_MODIFY_OTHER, FILE_TYPE_COMPLEX_EXT_FINAL_MODIFY_OTHER);
}

bool TestXSDMode::testModifyChildTypeComplexTypeComplexContentRestriction()
{
    return testModifyChildTypeComplexType("testModifyChildTypeComplexTypeComplexContentRestriction", XSDOperationParameters::TSC_COMPLEX,
                                             XSDOperationParameters::TSS_RESTRICTION, FILE_TYPE_COMPLEX_RES_START_MODIFY, FILE_TYPE_COMPLEX_RES_FINAL_MODIFY);
}

bool TestXSDMode::testModifyChildTypeComplexTypeComplexContentRestrictionOther()
{
    return testModifyChildTypeComplexType("testModifyChildTypeComplexTypeComplexContentRestrictionOther", XSDOperationParameters::TSC_COMPLEX,
                                             XSDOperationParameters::TSS_RESTRICTION, FILE_TYPE_COMPLEX_RES_START_MODIFY_OTHER, FILE_TYPE_COMPLEX_RES_FINAL_MODIFY_OTHER);
}


bool TestXSDMode::testModifyChildTypeComplexTypeSequence()
{
    return testModifyChildTypeComplexType("testModifyChildTypeComplexTypeSequence", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_SEQUENCE, FILE_TYPE_COMPLEX_SEQ_START_MODIFY, FILE_TYPE_COMPLEX_SEQ_FINAL_MODIFY);
}

bool TestXSDMode::testModifyChildTypeComplexTypeSequenceOther()
{
    return testModifyChildTypeComplexType("testModifyChildTypeComplexTypeSequenceOther", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_SEQUENCE, FILE_TYPE_COMPLEX_SEQ_START_MODIFY_OTHER, FILE_TYPE_COMPLEX_SEQ_FINAL_MODIFY_OTHER);
}

bool TestXSDMode::testModifyChildTypeComplexTypeGroup()
{
    return testModifyChildTypeComplexType("testModifyChildTypeComplexTypeGroup", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_GROUP, FILE_TYPE_COMPLEX_GROUP_START_MODIFY, FILE_TYPE_COMPLEX_GROUP_FINAL_MODIFY);
}

bool TestXSDMode::testModifyChildTypeComplexTypeGroupOther()
{
    return testModifyChildTypeComplexType("testModifyChildTypeComplexTypeGroupOther", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_GROUP, FILE_TYPE_COMPLEX_GROUP_START_MODIFY_OTHER, FILE_TYPE_COMPLEX_GROUP_FINAL_MODIFY_OTHER);
}

bool TestXSDMode::testModifyChildTypeComplexTypeChoice()
{
    return testModifyChildTypeComplexType("testModifyChildTypeComplexTypeChoice", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_CHOICE, FILE_TYPE_COMPLEX_CHOICE_START_MODIFY, FILE_TYPE_COMPLEX_CHOICE_FINAL_MODIFY);
}

bool TestXSDMode::testModifyChildTypeComplexTypeChoiceOther()
{
    return testModifyChildTypeComplexType("testModifyChildTypeComplexTypeChoiceOther", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_CHOICE, FILE_TYPE_COMPLEX_CHOICE_START_MODIFY_OTHER, FILE_TYPE_COMPLEX_CHOICE_FINAL_MODIFY_OTHER);
}

bool TestXSDMode::testModifyChildTypeComplexTypeAll()
{
    return testModifyChildTypeComplexType("testModifyChildTypeComplexTypeAll", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_ALL, FILE_TYPE_COMPLEX_ALL_START_MODIFY, FILE_TYPE_COMPLEX_ALL_FINAL_MODIFY);
}

bool TestXSDMode::testModifyChildTypeComplexTypeAllOther()
{
    return testModifyChildTypeComplexType("testModifyChildTypeComplexTypeAllOther", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_ALL, FILE_TYPE_COMPLEX_ALL_START_MODIFY_OTHER, FILE_TYPE_COMPLEX_ALL_FINAL_MODIFY_OTHER);
}

bool TestXSDMode::testModifyChildTypeComplexTypeNone()
{
    return testModifyChildTypeComplexType("testModifyChildTypeComplexTypeNone", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_NONE, FILE_TYPE_COMPLEX_NONE_START_MODIFY, FILE_TYPE_COMPLEX_NONE_FINAL_MODIFY);
}

bool TestXSDMode::testModifyChildTypeComplexTypeNoneOther()
{
    return testModifyChildTypeComplexType("testModifyChildTypeComplexTypeNoneOther", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_NONE, FILE_TYPE_COMPLEX_NONE_START_MODIFY_OTHER, FILE_TYPE_COMPLEX_NONE_FINAL_MODIFY_OTHER);
}

bool TestXSDMode::testModifyChildTypeComplexTypeSimpleContentExtension()
{
    return testModifyChildTypeComplexTypeSimpleContent("testModifyChildTypeComplexTypeSimpleContentExtension",
                                             XSDOperationParameters::TSS_EXTENSION, FILE_TYPE_COMPLEX_SIMPLEEXT_START_MODIFY, FILE_TYPE_COMPLEX_SIMPLEEXT_FINAL_MODIFY);
}

bool TestXSDMode::testModifyChildTypeComplexTypeSimpleContentExtensionOther()
{
    return testModifyChildTypeComplexTypeSimpleContent("testModifyChildTypeComplexTypeSimpleContentExtensionOther",
                                             XSDOperationParameters::TSS_EXTENSION, FILE_TYPE_COMPLEX_SIMPLEEXT_START_MODIFY_OTHER, FILE_TYPE_COMPLEX_SIMPLEEXT_FINAL_MODIFY_OTHER);
}

bool TestXSDMode::testModifyChildTypeComplexTypeSimpleContentRestriction()
{
    return testModifyChildTypeComplexTypeSimpleContent("testModifyChildTypeComplexTypeSimpleContentRestriction",
                                             XSDOperationParameters::TSS_RESTRICTION, FILE_TYPE_COMPLEX_SIMPLERES_START_MODIFY, FILE_TYPE_COMPLEX_SIMPLERES_FINAL_MODIFY);
}

bool TestXSDMode::testModifyChildTypeComplexTypeSimpleContentRestrictionOther()
{
    return testModifyChildTypeComplexTypeSimpleContent("testModifyChildTypeComplexTypeSimpleContentRestrictionOther",
                                             XSDOperationParameters::TSS_RESTRICTION, FILE_TYPE_COMPLEX_SIMPLERES_START_MODIFY_OTHER, FILE_TYPE_COMPLEX_SIMPLERES_FINAL_MODIFY_OTHER);
}


bool TestXSDMode::testModifyChildTypeComplexTypeReference()
{
    return testModifyChildTypeComplexType("testModifyChildTypeComplexTypeReference", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_NONE, FILE_TYPE_COMPLEX_REF_START_MODIFY, FILE_TYPE_COMPLEX_REF_FINAL_MODIFY, true);
}

bool TestXSDMode::testModifyChildTypeComplexTypeReferenceOther()
{
    return testModifyChildTypeComplexType("testModifyChildTypeComplexTypeReferenceOther", XSDOperationParameters::TSC_NONE,
                                             XSDOperationParameters::TSS_NONE, FILE_TYPE_COMPLEX_REF_START_MODIFY_OTHER, FILE_TYPE_COMPLEX_REF_FINAL_MODIFY_OTHER, true );
}

// endregion(type) ------------------------------------------------------------------------------------------------

bool TestXSDMode::testEnabling()
{
    _testName = "testEnabling";
    if( !testEnableInsertAttribute() ) {
        return false;
    }
    if( !testEnableAppendAttribute() ) {
        return false;
    }
    if( !testEnableInsertType() ) {
        return false;
    }
    if( !testEnableAppendType() ) {
        return false;
    }
    if( !testEnableInsertElement() ) {
        return false;
    }
    if( !testEnableAppendElement() ) {
        return false;
    }
    if( !testEnableModify() ) {
        return false;
    }
    return true ;
}

bool TestXSDMode::testEnableModify()
{
    _testName = "testEnableModify";
    QList<int> sel;
    QString cmd = "actionXSDModifyType";

    //a
    sel.clear();
    sel << 1 << 9 << 0;
    if( !testEnableLowLevel(0, cmd, sel, false) ) {
        return false;
    }
    sel.clear();
    sel << 1 << 9 << 1;
    //attr
    if( !testEnableLowLevel(1, cmd, sel, true) ) {
        return false;
    }
    //complex
    sel.clear();
    sel << 1 << 9 << 2;
    if( !testEnableLowLevel(2, cmd, sel, true ) ) {
        return false;
    }
    //simple
    sel.clear();
    sel << 1 << 9 << 3;
    if( !testEnableLowLevel(3, cmd, sel, true ) ) {
        return false;
    }
    //element
    sel.clear();
    sel << 1 << 9 << 4;
    if( !testEnableLowLevel(4, cmd, sel, true ) ) {
        return false;
    }
    return true;
}


bool TestXSDMode::testEnableInsertElement()
{
    _testName = "testEnableInsertElement";
    QList<int> sel;
    QString cmd = "actionXSDInsertElement";

    //a
    sel.clear();
    sel << 1 << 8 << 0;
    if( !testEnableLowLevel(0, cmd, sel, false) ) {
        return false;
    }
    sel.clear();
    sel << 1 << 8 << 1;
    //schema
    if( !testEnableLowLevel(1, cmd, sel, true) ) {
        return false;
    }
    //choice
    sel.clear();
    sel << 1 << 8 << 2;
    if( !testEnableLowLevel(2, cmd, sel, true ) ) {
        return false;
    }
    //seq
    sel.clear();
    sel << 1 << 8 << 3;
    if( !testEnableLowLevel(3, cmd, sel, true ) ) {
        return false;
    }
    //all
    sel.clear();
    sel << 1 << 8 << 4;
    if( !testEnableLowLevel(4, cmd, sel, true ) ) {
        return false;
    }
    return true;
}

bool TestXSDMode::testEnableAppendElement()
{
    _testName = "testEnableAppendElement";
    QList<int> sel;
    QString cmd = "actionXSDAppendElement";

    //a
    sel.clear();
    sel << 1 << 8 << 0 << 0;
    if( !testEnableLowLevel(0, cmd, sel, false) ) {
        return false;
    }
    sel.clear();
    sel << 1 << 8 << 1 << 0;
    //schema
    if( !testEnableLowLevel(1, cmd, sel, true) ) {
        return false;
    }
    //choice
    sel.clear();
    sel << 1 << 8 << 2 << 0;
    if( !testEnableLowLevel(2, cmd, sel, true ) ) {
        return false;
    }
    //seq
    sel.clear();
    sel << 1 << 8 << 3 << 0;
    if( !testEnableLowLevel(3, cmd, sel, true ) ) {
        return false;
    }
    //all
    sel.clear();
    sel << 1 << 8 << 4 << 0;
    if( !testEnableLowLevel(4, cmd, sel, true ) ) {
        return false;
    }
    return true;
}


bool TestXSDMode::testEnableInsertType()
{
    _testName = "testEnableInsertType";
    QList<int> sel;
    QString cmd = "actionXSDInsertType";

    //a
    sel.clear();
    sel << 1 << 7 << 0;
    if( !testEnableLowLevel(0, cmd, sel, false) ) {
        return false;
    }
    sel.clear();
    sel << 1 << 7 << 1;
    //schema
    if( !testEnableLowLevel(1, cmd, sel, true) ) {
        return false;
    }
    //redefine
    sel.clear();
    sel << 1 << 7 << 2;
    if( !testEnableLowLevel(2, cmd, sel, true ) ) {
        return false;
    }
    //attr
    sel.clear();
    sel << 1 << 7 << 3;
    if( !testEnableLowLevel(3, cmd, sel, true ) ) {
        return false;
    }
    //elm
    sel.clear();
    sel << 1 << 7 << 4;
    if( !testEnableLowLevel(4, cmd, sel, true ) ) {
        return false;
    }
    //list
    sel.clear();
    sel << 1 << 7 << 5;
    if( !testEnableLowLevel(5, cmd, sel, true ) ) {
        return false;
    }
    //restr
    sel.clear();
    sel << 1 << 7 << 6;
    if( !testEnableLowLevel(6, cmd, sel, true ) ) {
        return false;
    }
    //union
    sel.clear();
    sel << 1 << 7 << 7;
    if( !testEnableLowLevel(7, cmd, sel, true ) ) {
        return false;
    }

    return true;
}

bool TestXSDMode::testEnableAppendType()
{
    _testName = "testEnableAppendType";
    QList<int> sel;
    QString cmd = "actionXSDAppendType";

    //a
    sel.clear();
    sel << 1 << 7 << 0 << 0 ;
    if( !testEnableLowLevel(0, cmd, sel, false) ) {
        return false;
    }
    sel.clear();
    sel << 1 << 7 << 1 << 0;
    //schema
    if( !testEnableLowLevel(1, cmd, sel, true) ) {
        return false;
    }
    //redefine
    sel.clear();
    sel << 1 << 7 << 2 << 0;
    if( !testEnableLowLevel(2, cmd, sel, true ) ) {
        return false;
    }
    //attr
    sel.clear();
    sel << 1 << 7 << 3 << 0;
    if( !testEnableLowLevel(3, cmd, sel, true ) ) {
        return false;
    }
    //elm
    sel.clear();
    sel << 1 << 7 << 4 << 0;
    if( !testEnableLowLevel(4, cmd, sel, true ) ) {
        return false;
    }
    //list
    sel.clear();
    sel << 1 << 7 << 5 << 0;
    if( !testEnableLowLevel(5, cmd, sel, true ) ) {
        return false;
    }
    //restr
    sel.clear();
    sel << 1 << 7 << 6 << 0;
    if( !testEnableLowLevel(6, cmd, sel, true ) ) {
        return false;
    }
    //union
    sel.clear();
    sel << 1 << 7 << 7 << 0;
    if( !testEnableLowLevel(7, cmd, sel, true ) ) {
        return false;
    }

    return true;
}

bool TestXSDMode::testEnableInsertAttribute()
{
    _testName = "testEnableInsertAttribute";
    QList<int> sel;
    QString cmd = "actionXSDInsertAttribute";

    //element
    sel.clear();
    sel << 1 << 0;
    if( !testEnableLowLevel(0, cmd, sel, false) ) {
        return false;
    }
    sel.clear();
    sel << 1;
    //schema
    if( !testEnableLowLevel(1, cmd, sel, true) ) {
        return false;
    }
    //complex
    sel.clear();
    sel << 1 << 1;
    if( !testEnableLowLevel(2, cmd, sel, true ) ) {
        return false;
    }
    //attr group
    sel.clear();
    sel << 1 << 4;
    if( !testEnableLowLevel(3, cmd, sel, true ) ) {
        return false;
    }
    //extens
    sel.clear();
    sel << 1 << 5;
    if( !testEnableLowLevel(4, cmd, sel, true ) ) {
        return false;
    }
    //restr
    sel.clear();
    sel << 1 << 6;
    if( !testEnableLowLevel(5, cmd, sel, true ) ) {
        return false;
    }

    return true;
}

bool TestXSDMode::testEnableAppendAttribute()
{
    _testName = "testEnableAppendAttribute";
    QList<int> sel;
    QString cmd = "actionXSDAppendAttribute";

    //element
    sel.clear();
    sel << 1 << 0 << 0 ;
    if( !testEnableLowLevel(0, cmd, sel, false) ) {
        return false;
    }
    sel.clear();
    sel << 1 << 0;
    //schema
    if( !testEnableLowLevel(1, cmd, sel, true) ) {
        return false;
    }
    //complex
    sel.clear();
    sel << 1 << 1 << 0 ;
    if( !testEnableLowLevel(2, cmd, sel, true ) ) {
        return false;
    }
    //attr group
    sel.clear();
    sel << 1 << 4 << 0 ;
    if( !testEnableLowLevel(3, cmd, sel, true ) ) {
        return false;
    }
    //extens
    sel.clear();
    sel << 1 << 5 << 0 ;
    if( !testEnableLowLevel(4, cmd, sel, true ) ) {
        return false;
    }
    //restr
    sel.clear();
    sel << 1 << 6 << 0 ;
    if( !testEnableLowLevel(5, cmd, sel, true ) ) {
        return false;
    }

    return true;
}

bool TestXSDMode::testEnableLowLevel(const int id, const QString &actionName, QList<int> &sel, const bool expectedResult)
{
    App app;
    if( !testForEnabling(app, FILE_CHECK_ENABLE, sel, id ) ) {
        return false;
    }
    bool boolResult = false ;
    if( !checkActionEnabled(app.mainWindow(), actionName, boolResult) ) {
        return error(QString("unable to find the action %1, id:%2").arg(actionName).arg(id));
    }
    if( expectedResult != boolResult ) {
        return error(QString("expected %1, found %2 id: %3").arg(expectedResult).arg(boolResult).arg(id));
    }

    return true;
}

bool TestXSDMode::testForEnabling(App &app, const QString &fileStart, QList<int> &sel, const int id )
{
    if(!app.init() ) {
        return error(QString("init app failed id:%1").arg(id));
    }
    if( !app.mainWindow()->loadFile(fileStart) ) {
        return error(QString("unable to load input file: '%1' id:%2").arg(fileStart).arg(id));
    }
    Element *selectedElement = app.mainWindow()->getRegola()->findElementByArray(sel);
    if(NULL == selectedElement) {
        return error(QString("no selected element, id:%1").arg(id));
    }
    app.mainWindow()->getEditor()->setCurrentItem(selectedElement);
    return true;
}

//-------------

bool TestXSDMode::testForDialog(App &app, const QString &fileStart, QList<int> &sel )
{
    if(!app.init() ) {
        return error(QString("init app failed"));
    }
    if( !app.mainWindow()->loadFile(fileStart) ) {
        return error(QString("unable to load input file: '%1'").arg(fileStart));
    }
    Element *selectedElement = app.mainWindow()->getRegola()->findElementByArray(sel);
    if(NULL == selectedElement) {
        return error(QString("no selected element"));
    }
    app.mainWindow()->getEditor()->setCurrentItem(selectedElement);
    return true;
}

bool TestXSDMode::testEnableDialog(QList<int> &sel, const TestDialogXSDTypesResult *expectedResult)
{
    App app;
    if( !testForDialog(app, FILE_DIALOG_START, sel ) ) {
        return false;
    }
    TestDialogXSDTypes theDialog( false, XSDOperationParameters::EOI_ATTRIBUTE, "", app.mainWindow()->getEditor()->getSelectedItem());
    if( !theDialog.checkSettings(expectedResult) ) {
        return error(QString("testDialog: %1").arg(theDialog.cause()));
    }
    return true;
}

bool TestXSDMode::testDialogAttributeRef()
{
    _testName = "testDialogAttributeRef";
    QList<int> sel;

    sel.clear();
    sel << 1 << 0 ;


    TestDialogXSDTypesResult expectedResult = {
           "name1",
            true, "xs:string", false,  false,
          false, false, false, true,
          false, "", false, false, false,
           false, "", false,false,false,false,false,false,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;
}

bool TestXSDMode::testDialogAttributeSimpleTypeRestr()
{
    _testName = "testDialogAttributeSimpleTypeRestr";
    QList<int> sel;

    sel.clear();
    sel << 1 << 1 ;


    TestDialogXSDTypesResult expectedResult = {
           "name2",
            false, "", true,  false,
          false, false, false, true,
          true, "xs:string", true, false, false,
           false, "", false,false,false,false,false,false,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;

}

bool TestXSDMode::testDialogAttributeSimpleTypeList()
{
    _testName = "testDialogAttributeSimpleTypeList";
    QList<int> sel;

    sel.clear();
    sel << 1 << 2 ;


    TestDialogXSDTypesResult expectedResult = {
           "name3",
            false, "", true,  false,
          false, false, false, true,
          true, "", false, true, false,
           false, "", false,false,false,false,false,false,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;

}

bool TestXSDMode::testDialogAttributeSimpleTypeUnion()
{
    _testName = "testDialogAttributeSimpleTypeUnion";
    QList<int> sel;

    sel.clear();
    sel << 1 << 3 ;


    TestDialogXSDTypesResult expectedResult = {
           "name4",
            false, "", true,  false,
          false, false, false, true,
          true, "", false, false, true,
           false, "", false,false,false,false,false,false,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;

}

bool TestXSDMode::testDialogSimpleTypeRestr()
{
    _testName = "testDialogSimpleTypeRestr";
    QList<int> sel;

    sel.clear();
    sel << 1 << 4 ;


    TestDialogXSDTypesResult expectedResult = {
           "type1",
            false, "", true,  false,
          false, false, false, true,
          true, "xs:string", true, false, false,
           false, "", false,false,false,false,false,false,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;

}

bool TestXSDMode::testDialogSimpleTypeList()
{
    _testName = "testDialogASimpleTypeList";
    QList<int> sel;

    sel.clear();
    sel << 1 << 5 ;


    TestDialogXSDTypesResult expectedResult = {
           "type2",
            false, "", true,  false,
          false, false, false, true,
          true, "", false, true, false,
           false, "", false,false,false,false,false,false,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;

}

bool TestXSDMode::testDialogSimpleTypeUnion()
{
    _testName = "testDialogSimpleTypeUnion";
    QList<int> sel;

    sel.clear();
    sel << 1 << 6 ;


    TestDialogXSDTypesResult expectedResult = {
           "type3",
            false, "", true,  false,
          false, false, false, true,
          true, "", false, false, true,
           false, "", false,false,false,false,false,false,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;

}

bool TestXSDMode::testDialogAttributeNegative()
{
    _testName = "testDialogAttributeNegative";
    QList<int> sel;

    sel.clear();
    sel << 1 << 2 ;


    TestDialogXSDTypesResult expectedResult = {
           "name0",
            false, "", true,  false,
          false, false, false, true,
          true, "", false, false, true,
           false, "", false,false,false,false,false,false,false };

    if( testEnableDialog(sel, &expectedResult) ) {
        return error("negative test failed");
    }
    return true;

}

bool TestXSDMode::testDialogComplexTypeCCRestr()
{
    _testName = "testDialogComplexTypeCCRestr";
    QList<int> sel;

    sel.clear();
    sel << 1 << 8 ;


    TestDialogXSDTypesResult expectedResult = {
           "ccores",
            false, "", false,  true,
          true, false, true, false,
          false, "", false, false, false,
           true, "theBase", false,false,true,false,false,false,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;

}

bool TestXSDMode::testDialogComplexTypeCCExt()
{
    _testName = "testDialogComplexTypeCCExt";
    QList<int> sel;

    sel.clear();
    sel << 1 << 7 ;


    TestDialogXSDTypesResult expectedResult = {
           "ccoex",
            false, "", false,  true,
          true, false, true, false,
          false, "", false, false, false,
           true, "theBase", false,true,false,false,false,false,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;

}

bool TestXSDMode::testDialogComplexTypeSCRes()
{
    _testName = "testDialogComplexTypeSCRes";
    QList<int> sel;

    sel.clear();
    sel << 1 << 9 ;


    TestDialogXSDTypesResult expectedResult = {
           "ccscr",
            false, "", false,  true,
          true, true, false, false,
          false, "", false, false, false,
          true, "xs:string", false,false,true,false,false,false,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;

}


/*
bool TestXSDMode::testDialogComplexTypeSCList()
{
    _testName = "testDialogComplexTypeSCList";
    QList<int> sel;

    sel.clear();
    sel << 1 << 10 ;


    TestDialogXSDTypesResult expectedResult = {
        "ccscl",
         false, "", false,  true,
       true, true, false, false,
       true, "", false, true, false,
        false, "", false,false,false,false,false,false,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;

}

bool TestXSDMode::testDialogComplexTypeSCUnion()
{
    _testName = "testDialogComplexTypeSCUnion";
    QList<int> sel;

    sel.clear();
    sel << 1 << 11 ;

    TestDialogXSDTypesResult expectedResult = {
        "ccscu",
         false, "", false,  true,
       true, true, false, false,
       true, "", false, false, true,
        false, "", false,false,false,false,false,false,false };


    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;
}
*/
bool TestXSDMode::testDialogComplexTypeAll()
{
    _testName = "testDialogComplexTypeAll";
    QList<int> sel;

    sel.clear();
    sel << 1 << 12 ;

    TestDialogXSDTypesResult expectedResult = {
           "ccall",
            false, "", false,  true,
          true, false, false, true,
          false, "", false, false, false,
           true, "", false,false,false,true,false,false,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;
}

bool TestXSDMode::testDialogComplexTypeSeq()
{
    _testName = "testDialogComplexTypeSeq";
    QList<int> sel;

    sel.clear();
    sel << 1 << 13;

    TestDialogXSDTypesResult expectedResult = {
           "cseq",
            false, "", false,  true,
          true, false, false, true,
          false, "", false, false, false,
           true, "", false,false,false,false,false,false,true };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;
}


bool TestXSDMode::testDialogComplexTypeGroup()
{
    _testName = "testDialogComplexTypeGroup";
    QList<int> sel;

    sel.clear();
    sel << 1 << 14;

    TestDialogXSDTypesResult expectedResult = {
           "cgroup",
            false, "", false,  true,
          true, false, false, true,
          false, "", false, false, false,
           true, "", false,false,false,false,true,false,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;
}

bool TestXSDMode::testDialogComplexTypeChoice()
{
    _testName = "testDialogComplexTypeChoice";
    QList<int> sel;

    sel.clear();
    sel << 1 << 15;

    TestDialogXSDTypesResult expectedResult = {
           "choice",
            false, "", false,  true,
          true, false, false, true,
          false, "", false, false, false,
           true, "", false,false,false,false,false,true,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;
}

bool TestXSDMode::testDialogComplexTypeSCExt()
{
    _testName = "testDialogComplexTypeSCExt";
    QList<int> sel;

    sel.clear();
    sel << 1 << 29 ;


    TestDialogXSDTypesResult expectedResult = {
           "ccsce",
            false, "", false,  true,
          true, true, false, false,
          false, "", false, false, false,
          true, "xs:string", false,true,false,false,false,false,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;

}
bool TestXSDMode::testDialogTypes()
{
    _testName = "testDialogTypes";

    if( !testDialogAttributeRef() ) {
        return false;
    }
    if( !testDialogAttributeSimpleTypeRestr() ) {
        return false;
    }
    if( !testDialogAttributeSimpleTypeList() ) {
        return false;
    }
    if( !testDialogAttributeSimpleTypeUnion() ) {
        return false;
    }
    // use here to avoid errors on loaded files
    if( !testDialogAttributeNegative() ) {
        return false;
    }

    if( !testDialogSimpleTypeRestr() ) {
        return false;
    }
    if( !testDialogSimpleTypeList() ) {
        return false;
    }
    if( !testDialogSimpleTypeUnion() ) {
        return false;
    }

    if( !testDialogComplexTypeCCRestr() ) {
        return false;
    }
    if( !testDialogComplexTypeCCExt() ) {
        return false;
    }

    if( !testDialogComplexTypeSCRes() ) {
        return false;
    }
    if( !testDialogComplexTypeSCExt() ) {
        return false;
    }
    /*if( !testDialogComplexTypeSCList() ) {
        return false;
    }
    if( !testDialogComplexTypeSCUnion() ) {
        return false;
    }*/
    if( !testDialogComplexTypeAll() ) {
        return false;
    }
    if( !testDialogComplexTypeSeq() ) {
        return false;
    }
    if( !testDialogComplexTypeGroup() ) {
        return false;
    }
    if( !testDialogComplexTypeChoice() ) {
        return false;
    }
    //---
    if( !testDialogElementRef() ) {
        return false;
    }
    if( !testDialogElementSimpleTypeRestr() ) {
        return false;
    }
    if( !testDialogElementSimpleTypeList() ) {
        return false;
    }
    if( !testDialogElementSimpleTypeUnion() ) {
        return false;
    }

    if( !testDialogElementComplexTypeCCRestr() ) {
        return false;
    }
    if( !testDialogElementComplexTypeCCExt() ) {
        return false;
    }

    if( !testDialogElementComplexTypeSCRes() ) {
        return false;
    }
    /*if( !testDialogElementComplexTypeSCList() ) {
        return false;
    }
    if( !testDialogElementComplexTypeSCUnion() ) {
        return false;
    }*/
    if( !testDialogElementComplexTypeAll() ) {
        return false;
    }
    if( !testDialogElementComplexTypeSeq() ) {
        return false;
    }
    if( !testDialogElementComplexTypeGroup() ) {
        return false;
    }
    if( !testDialogElementComplexTypeChoice() ) {
        return false;
    }

    return true ;
}

bool TestXSDMode::testDialogElementRef()
{
    _testName = "testDialogElementRef";
    QList<int> sel;

    sel.clear();
    sel << 1 << 16 ;


    TestDialogXSDTypesResult expectedResult = {
           "e1",
            true, "xs:string", false,  false,
          false, false, false, true,
          false, "", false, false, false,
           false, "", false,false,false,false,false,false,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;
}

bool TestXSDMode::testDialogElementSimpleTypeRestr()
{
    _testName = "testDialogElementSimpleTypeRestr";
    QList<int> sel;

    sel.clear();
    sel << 1 << 17 ;


    TestDialogXSDTypesResult expectedResult = {
           "e2",
            false, "", true,  false,
          false, false, false, true,
          true, "xs:string", true, false, false,
           false, "", false,false,false,false,false,false,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;

}

bool TestXSDMode::testDialogElementSimpleTypeList()
{
    _testName = "testDialogElementSimpleTypeList";
    QList<int> sel;

    sel.clear();
    sel << 1 << 18 ;


    TestDialogXSDTypesResult expectedResult = {
           "e3",
            false, "", true,  false,
          false, false, false, true,
          true, "", false, true, false,
           false, "", false,false,false,false,false,false,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;

}

bool TestXSDMode::testDialogElementSimpleTypeUnion()
{
    _testName = "testDialogElementSimpleTypeUnion";
    QList<int> sel;

    sel.clear();
    sel << 1 << 19 ;


    TestDialogXSDTypesResult expectedResult = {
           "e4",
            false, "", true,  false,
          false, false, false, true,
          true, "", false, false, true,
           false, "", false,false,false,false,false,false,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;

}

bool TestXSDMode::testDialogElementComplexTypeCCExt()
{
    _testName = "testDialogElementComplexTypeCCExt";
    QList<int> sel;

    sel.clear();
    sel << 1 << 20 ;


    TestDialogXSDTypesResult expectedResult = {
           "e5",
            false, "", false,  true,
          true, false, true, false,
          false, "", false, false, false,
           true, "theBase", false,true,false,false,false,false,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;

}

bool TestXSDMode::testDialogElementComplexTypeCCRestr()
{
    _testName = "testDialogElementComplexTypeCCRestr";
    QList<int> sel;

    sel.clear();
    sel << 1 << 21 ;


    TestDialogXSDTypesResult expectedResult = {
           "e6",
            false, "", false,  true,
          true, false, true, false,
          false, "", false, false, false,
           true, "theBase", false,false,true,false,false,false,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;

}


bool TestXSDMode::testDialogElementComplexTypeSCRes()
{
    _testName = "testDialogElementComplexTypeSCRes";
    QList<int> sel;

    sel.clear();
    sel << 1 << 22 ;


    TestDialogXSDTypesResult expectedResult = {
           "ccscr",
            false, "", false,  true,
          true, true, false, false,
          false, "", false, false, false,
           true, "xs:string", false,false,true,false,false,false,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;

}

/*
bool TestXSDMode::testDialogElementComplexTypeSCList()
{
    _testName = "testDialogElementComplexTypeSCList";
    QList<int> sel;

    sel.clear();
    sel << 1 << 23 ;


    TestDialogXSDTypesResult expectedResult = {
        "ccscl",
         false, "", false,  true,
       true, true, false, false,
       true, "", false, true, false,
        false, "", false,false,false,false,false,false,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;

}

bool TestXSDMode::testDialogElementComplexTypeSCUnion()
{
    _testName = "testDialogElementComplexTypeSCUnion";
    QList<int> sel;

    sel.clear();
    sel << 1 << 24 ;

    TestDialogXSDTypesResult expectedResult = {
        "ccscu",
         false, "", false,  true,
       true, true, false, false,
       true, "", false, false, true,
        false, "", false,false,false,false,false,false,false };


    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;
}
*/
bool TestXSDMode::testDialogElementComplexTypeAll()
{
    _testName = "testDialogComplexTypeAll";
    QList<int> sel;

    sel.clear();
    sel << 1 << 25 ;

    TestDialogXSDTypesResult expectedResult = {
           "ccall",
            false, "", false,  true,
          true, false, false, true,
          false, "", false, false, false,
           true, "", false,false,false,true,false,false,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;
}

bool TestXSDMode::testDialogElementComplexTypeSeq()
{
    _testName = "testDialogComplexTypeSeq";
    QList<int> sel;

    sel.clear();
    sel << 1 << 26;

    TestDialogXSDTypesResult expectedResult = {
           "cseq",
            false, "", false,  true,
          true, false, false, true,
          false, "", false, false, false,
           true, "", false,false,false,false,false,false,true };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;
}


bool TestXSDMode::testDialogElementComplexTypeGroup()
{
    _testName = "testDialogElementComplexTypeGroup";
    QList<int> sel;

    sel.clear();
    sel << 1 << 27;

    TestDialogXSDTypesResult expectedResult = {
           "cgroup",
            false, "", false,  true,
          true, false, false, true,
          false, "", false, false, false,
           true, "", false,false,false,false,true,false,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;
}

bool TestXSDMode::testDialogElementComplexTypeChoice()
{
    _testName = "testDialogElementComplexTypeChoice";
    QList<int> sel;

    sel.clear();
    sel << 1 << 28;

    TestDialogXSDTypesResult expectedResult = {
           "choice",
            false, "", false,  true,
          true, false, false, true,
          false, "", false, false, false,
           true, "", false,false,false,false,false,true,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;
}

bool TestXSDMode::testDialogElementComplexTypeSCExt()
{
    _testName = "testDialogElementComplexTypeSCExt";
    QList<int> sel;

    sel.clear();
    sel << 1 << 26 ;


    TestDialogXSDTypesResult expectedResult = {
           "ectsce",
            false, "", false,  true,
          true, true, false, false,
          false, "", false, false, false,
           true, "xs:string", false,true,false,false,false,false,false };

    if( !testEnableDialog(sel, &expectedResult) ) {
        return false;
    }
    return true;

}
