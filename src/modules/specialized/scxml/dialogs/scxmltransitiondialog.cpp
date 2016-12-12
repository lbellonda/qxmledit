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

#define External  "external"
#define Internal  "internal"

SCXMLTransitionDialog::SCXMLTransitionDialog(QWidget *parent, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
        Element *toModifyElement, Element *selectedElement, Element *parentElement) :
    QDialog(parent),
    p(info, regola, isInsertOrEdit, isInsertOrAppend, toModifyElement, selectedElement, parentElement),
    d(&p._d),
    ui(new Ui::SCXMLTransitionDialog)
{
    ui->setupUi(this);
    setupCommon();
    if(p._isInsertOrEdit) {
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
    QStringList allStates = p._info->allStates();
    Utils::loadComboTextArrays(ui->target, "", allStates, allStates);
    QStringList types;
    types << External << Internal;
    Utils::loadComboTextArrays(ui->type, "", types, types);
}

// use default values
void SCXMLTransitionDialog::setupInsert()
{
    p.assignTag(SCXMLToken::Tag_transition);
    d->setAttributeString(SCXMLtransitionToken::A_type, External);
}

void SCXMLTransitionDialog::setupEdit()
{
    ui->event->setText(d->attributeString(SCXMLtransitionToken::A_event));
    ui->cond->setText(d->attributeString(SCXMLtransitionToken::A_cond));
    ui->target->setEditText(d->attributeString(SCXMLtransitionToken::A_target));
    Utils::selectComboText(ui->type, d->attributeString(SCXMLtransitionToken::A_type, External));
}

void SCXMLTransitionDialog::accept()
{
    d->setAttributeStringIfExisting(SCXMLtransitionToken::A_type, ui->type->currentText());
    d->setAttributeStringIfExisting(SCXMLtransitionToken::A_target, ui->target->currentText());
    d->setAttributeStringIfExisting(SCXMLtransitionToken::A_cond, ui->cond->text());
    d->setAttributeStringIfExisting(SCXMLtransitionToken::A_event, ui->event->text());

    if(!d->checkIDREFS(this, SCXMLtransitionToken::A_target)) {
        return ;
    }
    if(d->attributeString(SCXMLtransitionToken::A_event).trimmed().isEmpty()
            &&  d->attributeString(SCXMLtransitionToken::A_cond).trimmed().isEmpty()
            &&  d->attributeString(SCXMLtransitionToken::A_target).trimmed().isEmpty()) {
        Utils::error(this, tr("Please specify at least one condition, event or target."));
        return;
    }
    QDialog::accept();
}
