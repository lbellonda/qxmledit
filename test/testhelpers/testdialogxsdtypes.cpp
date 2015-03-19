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


#include "testdialogxsdtypes.h"
#include "ui_xsdtypedialog.h"

TestDialogXSDTypes::TestDialogXSDTypes(const bool isInsert, const XSDOperationParameters::EObjectType objectType, const QString &startName, Element *selection, QWidget *parent) :
    XSDTypeDialog(isInsert, objectType, startName, selection, parent)
{
}

TestDialogXSDTypes::~TestDialogXSDTypes()
{
}

#define EVAL_TEXT(field, expectedField)\
if( ui->field->text() != expected->expectedField ) {\
    _cause = QString("Field :'%1', found:'%2' expected:'%3'").arg(#field).arg(ui->field->text()).arg(expected->expectedField);\
    return false;\
}

#define EVAL_CHECK(field, expectedField)\
if( ui->field->isChecked() != expected->expectedField ) {\
    _cause = QString("Field :'%1', found:'%2' expected:'%3'").arg(#field).arg(ui->field->isChecked()).arg(expected->expectedField);\
    return false;\
}

#define EVAL_COMBO(field, expectedField)\
if( ui->field->currentText() != expected->expectedField ) {\
    _cause = QString("Field :'%1', found:'%2' expected:'%3'").arg(#field).arg(ui->field->currentText()).arg(expected->expectedField);\
    return false;\
}

#define EVAL_ENABLED(field, expectedField)\
if( ui->field->isEnabled() != expected->expectedField ) {\
    _cause = QString("Field :'%1', found:'%2' expected:'%3'").arg(#field).arg(ui->field->isEnabled()).arg(expected->expectedField);\
    return false;\
}

bool TestDialogXSDTypes::checkSettings(const TestDialogXSDTypesResult *expected)
{
    /*if( ui->ename->text() != expected->name ) {
        _cause = QString("Field :'%1', found:'%2' expected:'%3'").arg(field).arg(ui->ename->text().arg(expected->name));
        return false;
    }*/
    EVAL_TEXT(ename, name );
    EVAL_CHECK(typeRef, aref);
    EVAL_COMBO(reftype, aname);
    EVAL_CHECK(typeSimple, asimple);
    EVAL_CHECK(typeComplex, acomplex);
    EVAL_ENABLED(groupContent, bcontentenabled);
    EVAL_CHECK(contentSimple, bsimplec);
    EVAL_CHECK(contentComplex, bcomplexc);
    EVAL_CHECK(contentNone, bnone);
    EVAL_ENABLED(groupSimpleType, csimpleenabled);
    EVAL_COMBO(simpleTypeBase, csimplename);
    EVAL_CHECK(simpleTypeList, clist);
    EVAL_CHECK(simpleTypeUnion, cunion);
    EVAL_CHECK(simpleTypeRestriction, crestr);
    EVAL_ENABLED(groupOContent, dcomplexenabled);
    EVAL_COMBO(cBaseType, dcomplexename);
    EVAL_CHECK(cNone, dnone);
    EVAL_CHECK(cExtension, dext);
    EVAL_CHECK(cRestriction, drestr);
    EVAL_CHECK(cAll, dall);
    EVAL_CHECK(cGroup, dgroup);
    EVAL_CHECK(cChoice, dchoice);
    EVAL_CHECK(cSeq, dseq);

    return true;
}
