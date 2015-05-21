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

#include "testxsdfullannotationsdialog.h"
#include "testhelpers/xsd/testannotxsdannotationeditprovider.h"
#include "testhelpers/xsd/testannotxsdannotationeditor.h"
#include "utils.h"

TestXSDFullAnnotationsDialog::TestXSDFullAnnotationsDialog(TestAnnotXSDAnnotationEditor *newManager, XSDAnnotationModel *newModel, QWidget *parent) :
    XSDFullAnnotationsDialog(newModel, parent)
{
    _manager = newManager ;
    _calledCount = 0 ;
    _manager->provider->hasOpenedComplex = true ;
}

TestXSDFullAnnotationsDialog::~TestXSDFullAnnotationsDialog()
{
}


int TestXSDFullAnnotationsDialog::runExec()
{
    switch (_manager->provider->testCase ) {
    default:
    case TestInfoAnnotEdit::InsertComplexNoDataCancel:
    case TestInfoAnnotEdit::InsertComplexDataCancel:
        return QDialog::Rejected;
    case TestInfoAnnotEdit::InsertComplexNoDataInsert:
    case TestInfoAnnotEdit::InsertComplexDataConfirm:
    case TestInfoAnnotEdit::InsertMixDataConfirm:
        break;
    }
    // ---
    foreach( TestInfoAnnotEdit* info, _manager->provider->complexData ) {
        if( info->type == TestInfoAnnotEdit::Mod ) {
            // Update
            updateItem(info->index, info->createModel());
        }
    }
    foreach( TestInfoAnnotEdit* info, _manager->provider->complexData ) {
        if( info->type == TestInfoAnnotEdit::Del ) {
            // Delete
            deleteItem(info->index);
        }
    }
    foreach( TestInfoAnnotEdit* info, _manager->provider->complexData ) {
        if( info->type == TestInfoAnnotEdit::Add ) {
            // Add
            addNewItem(info->createModel());
        }
    }
    return QDialog::Accepted;
}

