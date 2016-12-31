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

#include "scxmlroot.h"
#include "ui_scxmlroot.h"
#include "regola.h"
#include "modules/specialized/scxml/scxmltoken.h"
#include "modules/xsd/namespacemanager.h"
#include "utils.h"

SCXMLRoot::SCXMLRoot(QWidget *parent, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                     Element *toModifyElement, Element *selectedElement, Element *parentElement) :
    QDialog(parent),
    p(info, regola, isInsertOrEdit, isInsertOrAppend,
      toModifyElement, selectedElement, parentElement),
    d(&p._d),
    ui(new Ui::SCXMLRoot)
{
    ui->setupUi(this);
    setupCommon();
    if(p._isInsertOrEdit) {
        setupInsert();
    }
    setupEdit();
}

SCXMLRoot::~SCXMLRoot()
{
    delete ui;
}

void SCXMLRoot::setupCommon()
{
    Utils::loadComboTextArrays(ui->initial, "", p._info->allStates(), p._info->allStates());
}

// use default values
void SCXMLRoot::setupInsert()
{
    d->setAttributeString(SCXMLscxmlToken::A_binding, "early");
    d->setAttributeString(SCXMLscxmlToken::A_datamodel, "null");
    d->setAttributeString(SCXMLscxmlToken::A_name, tr("NewMachine", "this should be a valid SCXML identifier"));
    d->setAttributeString(SCXMLscxmlToken::A_version, "1.0");
    d->setAttributeString(SCXMLscxmlToken::A_xmlns, NamespaceManager::SCXMLNamespace);
    d->setAttributeString("xmlns:xi", NamespaceManager::XIncludeNamespace);
    d->assignTag(SCXMLToken::Tag_scxml, p._regola, p._parentElement);
    Utils::TODO_THIS_RELEASE("testa il xi include");
}

void SCXMLRoot::setupEdit()
{
    //fai fill su elemento se insert, poi una sola funzione per settare UI.
    ui->binding->setChecked(d->attributeString(SCXMLscxmlToken::A_datamodel, "early") != "late");
    ui->datamodel->setEditText(d->attributeString(SCXMLscxmlToken::A_datamodel));
    ui->name->setText(d->attributeString(SCXMLscxmlToken::A_name));
    ui->version->setText(d->attributeString(SCXMLscxmlToken::A_version));
    ui->initial->setEditText(d->attributeString(SCXMLscxmlToken::A_initial));
}

void SCXMLRoot::accept()
{
    d->setAttributeString(SCXMLscxmlToken::A_binding, ui->binding->isChecked() ? "early" : "late");
    d->setAttributeStringIfExisting(SCXMLscxmlToken::A_datamodel, ui->datamodel->currentText());
    d->setAttributeStringIfExisting(SCXMLscxmlToken::A_name, ui->name->text());
    d->setAttributeString(SCXMLscxmlToken::A_version, ui->version->text());
    d->setAttributeString(SCXMLscxmlToken::A_xmlns, NamespaceManager::SCXMLNamespace);
    d->setAttributeStringIfExisting(SCXMLscxmlToken::A_initial, ui->initial->currentText());
    //d->setAttributeStringIfMissing(SCXMLscxmlToken::A_xmlns, NamespaceManager::SCXMLNamespace);

    if(!d->checkIDREFS(this, SCXMLscxmlToken::A_initial)
            ||  !d->checkNMTOKEN(this, SCXMLscxmlToken::A_name)
            ||  !d->checkNMTOKEN(this, SCXMLscxmlToken::A_datamodel)) {
        return;
    }

    QDialog::accept();
}
