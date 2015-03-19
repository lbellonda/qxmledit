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


#ifndef TESTXSDMODE_H
#define TESTXSDMODE_H

#include "testbase.h"
#include "element.h"
#include "modules/xsd/xsdhelper.h"
#include "app.h"

class TestDialogXSDTypesResult;

class TestXSDMode : public TestBase
{

    void fillParamsBaseElementComplexType(XSDOperationParameters &params, QList<int> &sel, const XSDOperationParameters::EXSDTypeContent typeContent = XSDOperationParameters::TSC_NONE);
    void fillParamsBaseTypeComplexType(XSDOperationParameters &params, QList<int> &sel, const XSDOperationParameters::EXSDTypeContent typeContent = XSDOperationParameters::TSC_NONE);
    void fillParamsBaseTypeSimpleType(XSDOperationParameters &params, QList<int> &sel);
    void fillParamsBaseElementSimpleType(XSDOperationParameters &params, QList<int> &sel);

    bool testModifyTypeSimpleType(const QString &theTestName, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile);
    bool testModifyElementSimpleType(const QString &theTestName, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile);

    bool testModifyChildElementComplexType(const QString &theTestName, const XSDOperationParameters::EXSDTypeContent typeContent, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile, const bool isReference = false);
    bool testModifyChildTypeComplexType(const QString &theTestName, const XSDOperationParameters::EXSDTypeContent typeContent, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile, const bool isReference = false);
    bool testModifyChildElementComplexTypeSimpleContent(const QString &theTestName, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile, const bool isReference = false);
    bool testModifyChildTypeComplexTypeSimpleContent(const QString &theTestName, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile, const bool isReference = false);
    bool testInsertChildElementSimpleType(const QString &theTestName, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile);
    bool testInsertChildTypeComplexType(const QString &theTestName, const XSDOperationParameters::EXSDTypeContent typeContent, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile, const bool isReference = false);

    bool cfr(Regola *regola, const QString &step, const QString &fileResult);
    bool testSkeleton(const QString &fileStart, const QString &fileResult, const ElementOp::Op op, XSDOperationParameters *params, QList<int> &sel );
    bool testInsertChildElementSimple();
    bool testInsertSiblingElementSimple();

    bool testModifyRefElementSimple();
    bool testModifyBareElementSimple();
    bool testModifyAnonymousElementSimple();
    bool testModifyComplexElementSimple();
    bool testModifyRestrictionElementSimple();
   // bool testModifyExtensionElementSimple();
    //bool testInsertExtensionElementSimple();
    //---
    bool testInsertElementSimpleTypeRestriction();
    bool testInsertElementListTypeSimple();
    bool testInsertElementUnionTypeSimple();
    bool testModifyElementSimpleTypeRestriction();
    bool testModifyElementListTypeSimple();
    bool testModifyElementUnionTypeSimple();
    // ---
    bool testInsertChildElementComplexTypeReference();
    bool testInsertChildTypeComplexTypeReference();
    bool testModChildElementComplexTypeReferenceExistingAnnotation();
    bool testModChildElementComplexTypeReferenceExistingAnnotationOther();
    bool testInsertChildElementComplexTypeComplexContentRestriction();
    bool testInsertChildTypeComplexTypeComplexContentRestriction();
    bool testInsertChildElementComplexTypeComplexContentExtension();
    bool testInsertChildTypeComplexTypeComplexContentExtension();
    bool testInsertChildElementComplexTypeNone();
    bool testInsertChildTypeComplexTypeNone();
    bool testInsertChildElementComplexTypeSequence();
    bool testInsertChildTypeComplexTypeSequence();
    bool testInsertChildElementComplexTypeAll();
    bool testInsertChildTypeComplexTypeAll();
    bool testInsertChildElementComplexTypeGroup();
    bool testInsertChildTypeComplexTypeGroup();
    bool testInsertChildElementComplexTypeChoice();
    bool testInsertChildTypeComplexTypeChoice();
    bool testModifyChildElementComplexTypeComplexContentExtension();
    bool testModifyChildTypeComplexTypeComplexContentExtension();
    bool testModifyChildElementComplexTypeComplexContentExtensionOther();
    bool testModifyChildTypeComplexTypeComplexContentExtensionOther();
    bool testModifyChildElementComplexTypeComplexContentRestrictionOther();
    bool testModifyChildTypeComplexTypeComplexContentRestrictionOther();
    bool testModifyChildElementComplexTypeComplexContentRestriction();
    bool testModifyChildTypeComplexTypeComplexContentRestriction();
    bool testModifyChildElementComplexTypeSequence();
    bool testModifyChildElementComplexTypeSequenceOther();
    bool testModifyChildElementComplexTypeChoice();
    bool testModifyChildElementComplexTypeChoiceOther();
    bool testModifyChildElementComplexTypeAll();
    bool testModifyChildElementComplexTypeAllOther();
    bool testModifyChildElementComplexTypeGroup();
    bool testModifyChildElementComplexTypeGroupOther();
    bool testModifyChildElementComplexTypeNone();
    bool testModifyChildElementComplexTypeNoneOther();
    bool testModifyChildElementComplexTypeSimpleContentExtension();
    bool testModifyChildElementComplexTypeSimpleContentExtensionOther();
    bool testModifyChildElementComplexTypeSimpleContentRestriction();
    bool testModifyChildElementComplexTypeSimpleContentRestrictionOther();
    //--
    bool testModifyChildTypeComplexTypeSequence();
    bool testModifyChildTypeComplexTypeSequenceOther();
    bool testModifyChildTypeComplexTypeGroup();
    bool testModifyChildTypeComplexTypeGroupOther();
    bool testModifyChildTypeComplexTypeChoice();
    bool testModifyChildTypeComplexTypeChoiceOther();
    bool testModifyChildTypeComplexTypeAll();
    bool testModifyChildTypeComplexTypeAllOther();
    bool testModifyChildTypeComplexTypeNone();
    bool testModifyChildTypeComplexTypeNoneOther();
    bool testModifyChildTypeComplexTypeSimpleContentExtension();
    bool testModifyChildTypeComplexTypeSimpleContentExtensionOther();
    bool testModifyChildTypeComplexTypeSimpleContentRestriction();
    bool testModifyChildTypeComplexTypeSimpleContentRestrictionOther();
    bool testModifyChildTypeComplexTypeReference();
    bool testModifyChildTypeComplexTypeReferenceOther();
    //--
    bool testModifyChildElementComplexTypeReference();
    bool testModifyChildElementComplexTypeReferenceOther();

    //----
    bool testInsertChildTypeSimpleTypeBare();
    bool testInsertSiblingTypeSimpleTypeBare();
    bool testModifySimpleTypeRestriction();
    bool testInsertSimpleTypeRestriction();
    bool testInsertListElementSimple();
    bool testInsertListTypeSimple();
    bool testModifyListElementSimple();
    bool testModifyListTypeSimple();
    bool testModifyUnionElementSimple();
    bool testInsertUnionElementSimple();
    bool testInsertUnionTypeSimple();
    bool testModifyUnionTypeSimple();
    bool testInsertChildTypeSimpleType(const QString &theTestName, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile);
    bool testInsertSiblingTypeSimpleType(const QString &theTestName, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile);

    //---

    bool testInsertChildAttributeSimple();
    bool testInsertSiblingAttributeSimple();
    bool testModifyRefAttributeSimple();
    bool testInsertAttributeSimpleTypeRestriction();
    bool testInsertListAttributeSimple();
    bool testInsertUnionAttributeSimple();
    bool testModifyAttributeSimpleTypeRestriction();
    bool testModifyListAttributeSimple();
    bool testModifyUnionAttributeSimple() ;
    bool testInsertChildAttribute(const QString &theTestName, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile);
    bool testInsertSiblingAttribute(const QString &theTestName, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile);
    bool testModifyAttributeSimpleType(const QString &theTestName, const XSDOperationParameters::EXSDTypeSpecSub subOper, const QString &theStartFile, const QString &theFinalFile);
    void fillParamsAttribute(XSDOperationParameters &params, QList<int> &sel);
    //---
    bool testEnableInsertType();
    bool testEnableAppendType();
    bool testEnableInsertElement() ;
    bool testEnableAppendElement();
    bool testEnableInsertAttribute();
    bool testEnableAppendAttribute();
    bool testEnableModify() ;

    //----
    bool testForEnabling(App &app, const QString &fileStart, QList<int> &sel , const int id);
    bool testEnableLowLevel(const int id, const QString &actionName, QList<int> &sel, const bool expectedResult);
    //---
    bool testForDialog(App &app, const QString &fileStart, QList<int> &sel );
    bool testEnableDialog(QList<int> &sel, const TestDialogXSDTypesResult *expectedResult);
    bool testDialogAttributeRef();
    bool testDialogAttributeSimpleTypeRestr();
    bool testDialogAttributeNegative();
    bool testDialogAttributeSimpleTypeUnion();
    bool testDialogAttributeSimpleTypeList();
    bool testDialogSimpleTypeUnion();
    bool testDialogSimpleTypeList();
    bool testDialogSimpleTypeRestr();
    bool testDialogComplexTypeCCExt();
    bool testDialogComplexTypeCCRestr();
    bool testDialogComplexTypeSCRes();
    bool testDialogComplexTypeSCExt();
    //bool testDialogComplexTypeSCList();
    //bool testDialogComplexTypeSCUnion();
    bool testDialogComplexTypeChoice();
    bool testDialogComplexTypeGroup();
    bool testDialogComplexTypeSeq();
    bool testDialogComplexTypeAll();

    bool testDialogElementRef();
    bool testDialogElementSimpleTypeRestr();
    bool testDialogElementSimpleTypeList();
    bool testDialogElementSimpleTypeUnion();
    bool testDialogElementComplexTypeCCRestr();
    bool testDialogElementComplexTypeCCExt();
    bool testDialogElementComplexTypeSCExt();
    bool testDialogElementComplexTypeSCRes();
    //bool testDialogElementComplexTypeSCList();
    //bool testDialogElementComplexTypeSCUnion();
    bool testDialogElementComplexTypeAll();
    bool testDialogElementComplexTypeSeq();
    bool testDialogElementComplexTypeGroup();
    bool testDialogElementComplexTypeChoice();


public:
    TestXSDMode();
    ~TestXSDMode();

    bool testElement();
    bool testType();
    bool testAttribute();
    bool testLast();
    bool testEnabling();
    bool testDialogTypes();

};

#endif // TESTXSDMODE_H
