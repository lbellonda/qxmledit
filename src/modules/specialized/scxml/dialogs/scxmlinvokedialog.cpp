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

#include "scxmlinvokedialog.h"
#include "ui_scxmlinvokedialog.h"

#include "regola.h"
#include "modules/specialized/scxml/scxmltoken.h"
#include "modules/xsd/namespacemanager.h"
#include "utils.h"

#define READ_FROM_COMBO(field) d->setAttributeStringIfExisting(SCXMLinvokeToken::A_ ## field, ui-> field ->currentText() )
#define READ_FROM_TEXT(field) d->setAttributeStringIfExisting(SCXMLinvokeToken::A_ ## field, ui-> field ->text() )

SCXMLInvokeDialog::SCXMLInvokeDialog(QWidget *parent, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                                     Element *toModifyElement, Element *selectedElement, Element *parentElement) :
    QDialog(parent),
    p(info, regola, isInsertOrEdit, isInsertOrAppend, toModifyElement, selectedElement, parentElement),
    d(&p._d),
    ui(new Ui::SCXMLInvokeDialog)
{
    ui->setupUi(this);
    setupCommon();
    if(p._isInsertOrEdit) {
        setupInsert();
    }
    setupEdit();
}

SCXMLInvokeDialog::~SCXMLInvokeDialog()
{
    delete ui;
}


void SCXMLInvokeDialog::setupCommon()
{
    QStringList some ;
    some << "http://www.w3.org/TR/scxml/" << "http://www.w3.org/TR/ccxml/" << "http://www.w3.org/TR/voicexml30/" << "http://www.w3.org/TR/voicexml21/" ;
    Utils::loadComboTextArrays(ui->type, "", some, some);
}

// use default values
void SCXMLInvokeDialog::setupInsert()
{
    p.assignTag(SCXMLToken::Tag_invoke);
}

void SCXMLInvokeDialog::setupEdit()
{
    ui->type->setEditText(d->attributeString(SCXMLinvokeToken::A_type));
    ui->typeexpr->setText(d->attributeString(SCXMLinvokeToken::A_typeexpr));
    ui->src->setText(d->attributeString(SCXMLinvokeToken::A_src));
    ui->srcexpr->setText(d->attributeString(SCXMLinvokeToken::A_srcexpr));
    ui->id->setText(d->attributeString(SCXMLinvokeToken::A_id));
    ui->idlocation->setText(d->attributeString(SCXMLinvokeToken::A_idlocation));
    ui->namelist->setText(d->attributeString(SCXMLinvokeToken::A_namelist));
    ui->autoforward->setChecked(d->attributeString(SCXMLinvokeToken::A_autoforward, "false") == "true");

}

void SCXMLInvokeDialog::accept()
{
    READ_FROM_COMBO(type);
    READ_FROM_TEXT(typeexpr);
    READ_FROM_TEXT(src);
    READ_FROM_TEXT(srcexpr);
    READ_FROM_TEXT(id);
    READ_FROM_TEXT(idlocation);
    READ_FROM_TEXT(namelist);
    d->setAttributeString(SCXMLinvokeToken::A_autoforward, ui->autoforward->isChecked() ? "true" : "false");

    if(!d->checkID(this, SCXMLstateToken::A_id)) {
        return;
    }
    if(!d->checkExclusive(this, SCXMLinvokeToken::A_id, SCXMLinvokeToken::A_idlocation)) {
        return ;
    }
    if(!d->checkExclusive(this, SCXMLinvokeToken::A_type, SCXMLinvokeToken::A_typeexpr)) {
        return ;
    }
    if(!d->checkExclusive(this, SCXMLinvokeToken::A_src, SCXMLinvokeToken::A_srcexpr)) {
        return ;
    }

    QDialog::accept();
}






















