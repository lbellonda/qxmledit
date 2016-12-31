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

#include "scxmldatadialog.h"
#include "ui_scxmldatadialog.h"

#include "regola.h"
#include "modules/specialized/scxml/scxmltoken.h"
#include "modules/xsd/namespacemanager.h"
#include "utils.h"

SCXMLDataDialog::SCXMLDataDialog(QWidget *parent, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                                 Element *toModifyElement, Element *selectedElement, Element *parentElement) :
    QDialog(parent),
    p(info, regola, isInsertOrEdit, isInsertOrAppend, toModifyElement, selectedElement, parentElement),
    d(&p._d),
    ui(new Ui::SCXMLDataDialog)
{
    ui->setupUi(this);
    setupCommon();
    if(p._isInsertOrEdit) {
        setupInsert();
    }
    setupEdit();
}

SCXMLDataDialog::~SCXMLDataDialog()
{
    delete ui;
}

void SCXMLDataDialog::setupCommon()
{
}

// use default values
void SCXMLDataDialog::setupInsert()
{
    p.assignTag(SCXMLToken::Tag_data);
}

void SCXMLDataDialog::setupEdit()
{
    ui->id->setText(d->attributeString(SCXMLdataToken::A_id));
    ui->src->setText(d->attributeString(SCXMLdataToken::A_src));
    ui->expr->setText(d->attributeString(SCXMLdataToken::A_expr));
}

void SCXMLDataDialog::accept()
{
    d->setAttributeString(SCXMLdataToken::A_id, ui->id->text());
    d->setAttributeString(SCXMLdataToken::A_src, ui->src->text());
    d->setAttributeStringIfExisting(SCXMLdataToken::A_expr, ui->expr->text());

    if(!d->checkID(this, SCXMLdataToken::A_id, true)) {
        return ;
    }
    if(!d->attributeString(SCXMLdataToken::A_src).trimmed().isEmpty() && !d->attributeString(SCXMLdataToken::A_expr).trimmed().isEmpty()) {
        Utils::error(this, tr("'src' and 'expr' cannot be both present."));
        return ;
    }
    QDialog::accept();
}
