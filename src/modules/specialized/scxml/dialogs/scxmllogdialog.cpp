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

#include "scxmllogdialog.h"
#include "ui_scxmllogdialog.h"

#include "regola.h"
#include "modules/specialized/scxml/scxmltoken.h"
#include "modules/xsd/namespacemanager.h"
#include "utils.h"

SCXMLLogDialog::SCXMLLogDialog(QWidget *parent, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                               Element *toModifyElement, Element *selectedElement, Element *parentElement) :
    QDialog(parent),
    p(info, regola, isInsertOrEdit, isInsertOrAppend, toModifyElement, selectedElement, parentElement),
    d(&p._d),
    ui(new Ui::SCXMLLogDialog)
{
    ui->setupUi(this);
    setupCommon();
    if(p._isInsertOrEdit) {
        setupInsert();
    }
    setupEdit();
}

SCXMLLogDialog::~SCXMLLogDialog()
{
    delete ui;
}

void SCXMLLogDialog::setupCommon()
{
}

// use default values
void SCXMLLogDialog::setupInsert()
{
    p.assignTag(SCXMLToken::Tag_log);
}

void SCXMLLogDialog::setupEdit()
{
    ui->label->setText(d->attributeString(SCXMLlogToken::A_label));
    ui->expr->setText(d->attributeString(SCXMLlogToken::A_expr));
}

void SCXMLLogDialog::accept()
{
    d->setAttributeStringIfExisting(SCXMLlogToken::A_label, ui->label->text());
    d->setAttributeStringIfExisting(SCXMLlogToken::A_expr, ui->expr->text());
    QDialog::accept();
}
