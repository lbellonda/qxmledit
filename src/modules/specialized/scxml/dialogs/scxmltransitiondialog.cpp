/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016 by Luca Bellonda and individual contributors       *
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

#include "scxmltransitiondialog.h"
#include "ui_scxmltransitiondialog.h"
#include "regola.h"
#include "modules/specialized/scxml/scxmltoken.h"
#include "modules/xsd/namespacemanager.h"
#include "utils.h"

SCXMLTransitionDialog::SCXMLTransitionDialog(QWidget *parent, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
        Element *toModifyElement, Element *selectedElement, Element *parentElement) :
    QDialog(parent),
    _isInsertOrEdit(isInsertOrEdit),
    _isInsertOrAppend(isInsertOrAppend),
    _d(toModifyElement),
    ui(new Ui::SCXMLTransitionDialog)
{
    Utils::TODO_THIS_RELEASE("icona");
    _regola = regola;
    _info = info ;
    _selectedElement = selectedElement;
    _parentElement = parentElement;
    ui->setupUi(this);
    setupCommon();
    if(_isInsertOrEdit) {
        setupInsert();
    }
    setupEdit();
}

SCXMLTransitionDialog::~SCXMLTransitionDialog()
{
    delete ui;
}

void SCXMLTransitionDialog::setupCommon()
{
    Utils::TODO_THIS_RELEASE("fare");
}

// use default values
void SCXMLTransitionDialog::setupInsert()
{
    _d.assignTag(SCXMLToken::Tag_parallel, _regola, _parentElement);
}

void SCXMLTransitionDialog::setupEdit()
{
    Utils::TODO_THIS_RELEASE("fare");
}

void SCXMLTransitionDialog::accept()
{
    Utils::TODO_THIS_RELEASE("fare validazioni");
    Utils::TODO_THIS_RELEASE("set dati in elemento");
    Utils::TODO_THIS_RELEASE("aggiungi attributi obbligatori");

    Utils::TODO_THIS_RELEASE("fare");
    QDialog::accept();
}
