/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016-2018 by Luca Bellonda and individual contributors  *
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

#include "scxmlsenddialog.h"
#include "ui_scxmlsenddialog.h"

#include "regola.h"
#include "modules/specialized/scxml/scxmltoken.h"
#include "modules/xsd/namespacemanager.h"
#include "utils.h"

#define READ_FROM_COMBO(field) d->setAttributeStringIfExisting(SCXMLsendToken::A_ ## field, ui-> field ->currentText() )
#define READ_FROM_TEXT(field) d->setAttributeStringIfExisting(SCXMLsendToken::A_ ## field, ui-> field ->text() )

#define SET_TEXT(field) ui-> field ->setText(d->attributeString(SCXMLsendToken::A_ ## field ))

SCXMLSendDialog::SCXMLSendDialog(QWidget *parent, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                                 Element *toModifyElement, Element *selectedElement, Element *parentElement) :
    QDialog(parent),
    p(info, regola, isInsertOrEdit, isInsertOrAppend, toModifyElement, selectedElement, parentElement),
    d(&p._d),
    ui(new Ui::SCXMLSendDialog)
{
    ui->setupUi(this);
    setupCommon();
    if(p._isInsertOrEdit) {
        setupInsert();
    }
    setupEdit();
}

SCXMLSendDialog::~SCXMLSendDialog()
{
    delete ui;
}


void SCXMLSendDialog::setupCommon()
{
}

// use default values
void SCXMLSendDialog::setupInsert()
{
    p.assignTag(SCXMLToken::Tag_send);
}

void SCXMLSendDialog::setupEdit()
{
    SET_TEXT(event);
    SET_TEXT(eventexpr);
    SET_TEXT(target);
    SET_TEXT(targetexpr);
    SET_TEXT(type);
    SET_TEXT(typeexpr);
    SET_TEXT(id);
    SET_TEXT(idlocation);
    SET_TEXT(delay);
    SET_TEXT(delayexpr);
    SET_TEXT(namelist);
}

void SCXMLSendDialog::accept()
{
    READ_FROM_TEXT(event);
    READ_FROM_TEXT(eventexpr);
    READ_FROM_TEXT(target);
    READ_FROM_TEXT(targetexpr);
    READ_FROM_TEXT(type);
    READ_FROM_TEXT(typeexpr);
    READ_FROM_TEXT(id);
    READ_FROM_TEXT(idlocation);
    READ_FROM_TEXT(delay);
    READ_FROM_TEXT(delayexpr);
    READ_FROM_TEXT(namelist);

    if(!d->checkID(this, SCXMLsendToken::A_id)
            ||  !d->checkID(this, SCXMLstateToken::A_id)) {
        return;
    }
    if(!d->checkExclusive(this, SCXMLsendToken::A_event, SCXMLsendToken::A_eventexpr)) {
        return ;
    }
    if(!d->checkExclusive(this, SCXMLsendToken::A_target, SCXMLsendToken::A_targetexpr)) {
        return ;
    }
    if(!d->checkExclusive(this, SCXMLsendToken::A_type, SCXMLsendToken::A_typeexpr)) {
        return ;
    }
    if(!d->checkExclusive(this, SCXMLsendToken::A_id, SCXMLsendToken::A_idlocation)) {
        return ;
    }
    if(!d->checkExclusive(this, SCXMLsendToken::A_delay, SCXMLsendToken::A_delayexpr)) {
        return ;
    }
    QDialog::accept();
}
