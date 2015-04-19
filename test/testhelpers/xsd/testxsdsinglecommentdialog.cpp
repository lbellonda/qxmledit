/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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

#include "testxsdsinglecommentdialog.h"
#include "testhelpers/xsd/testannotxsdannotationeditprovider.h"
#include "testhelpers/xsd/testannotxsdannotationeditor.h"

TestXSDSingleCommentDialog::TestXSDSingleCommentDialog(TestAnnotXSDAnnotationEditor *newManager, const bool argOpenedAsStarting, XInfoBase *newModel, QWidget *parent) :
    XSDSingleCommentDialog( argOpenedAsStarting, newModel, parent)
{
    _manager = newManager ;
    _calledCount = 0 ;
    _manager->provider->hasOpenedSimple = true ;
}

TestXSDSingleCommentDialog::~TestXSDSingleCommentDialog()
{
}

int TestXSDSingleCommentDialog::exec()
{
    switch(_manager->provider->testCase ) {
    default:
        return QDialog::Rejected;
    case TestInfoAnnotEdit::InsertComplexDataCancel:
    case TestInfoAnnotEdit::InsertComplexDataConfirm:
    case TestInfoAnnotEdit::CancelNoData:
        return QDialog::Rejected;
    case TestInfoAnnotEdit::ConfirmSimple:
    case TestInfoAnnotEdit::ConfirmSimpleData:
    case TestInfoAnnotEdit::InsertComplexNoDataCancel:
    case TestInfoAnnotEdit::InsertComplexNoDataInsert:
        return QDialog::Accepted;
    }
    return QDialog::Rejected;
}

bool TestXSDSingleCommentDialog::isCallExtended()
{
    switch(_manager->provider->testCase ) {
    default:
        return false;
    case TestInfoAnnotEdit::InsertComplexNoDataCancel:
    case TestInfoAnnotEdit::InsertComplexNoDataInsert:
        return true;
    }
    return XSDSingleCommentDialog::isCallExtended();
}

XInfoBase* TestXSDSingleCommentDialog::createModel()
{
    switch(_manager->provider->testCase ) {
    case TestInfoAnnotEdit::InsertMixDataConfirm:
    default:
        return NULL ;
    case TestInfoAnnotEdit::ConfirmSimple:
    case TestInfoAnnotEdit::InsertComplexNoDataCancel:
    case TestInfoAnnotEdit::InsertComplexNoDataInsert:
        {
            XDocumentation *documentation = new XDocumentation(NULL, NULL);
            documentation->setSource("theSource");
            documentation->setLanguage("en");
            documentation->setContentString("a<b attr=\"x\">Sample annotation.</b>end");
            return documentation ;
        }
    case TestInfoAnnotEdit::ConfirmSimpleData: {
            XDocumentation *documentation = new XDocumentation(NULL, NULL);
            documentation->setSource("theSource");
            documentation->setLanguage("en");
            documentation->setContentString("a<b attr=\"x\">Sample annotation edited.</b>end");
            return documentation ;
        }
    }
    return NULL ;
}
