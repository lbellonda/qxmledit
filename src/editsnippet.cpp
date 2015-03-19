/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
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
#include "editsnippet.h"
#include "utils.h"
#include "ui_editsnippet.h"

#define MAXCHARS_NAME   40
#define MAXCHARS_DESCR  100


bool editSnippet(const bool isInsert, Snippet *snippet, QWidget *parent)
{
    EditSnippet editDialog(isInsert, snippet, parent);
    editDialog.setModal(true);
    if(editDialog.exec() == QDialog::Accepted) {
        return true;
    }
    return false ;
}

EditSnippet::EditSnippet(const bool isInsert, Snippet *snippet, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditSnippet),
    _snippet(snippet),
    _isInsert(isInsert)
{
    ui->setupUi(this);
    ui->name->setMaxLength(MAXCHARS_NAME);
    ui->description->setMaxLength(MAXCHARS_DESCR);
    setUpData();
}

EditSnippet::~EditSnippet()
{
    delete ui;
}

void EditSnippet::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch(e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


bool EditSnippet::areDataOK()
{
    if(ui->name->text().trimmed().isEmpty()
            ||  ui->description->text().isEmpty()
            ||  ui->tags->text().trimmed().isEmpty()
            ||  ui->payload->toPlainText().trimmed().isEmpty()) {
        return false;
    }
    return true;
}

void EditSnippet::setUpData()
{
    ui->name->setText(_snippet->name());
    ui->description->setText(_snippet->description());
    ui->payload->setPlainText(_snippet->payload());
    // convert tags in qstring
    ui->tags->setText(_snippet->tags().join(","));
    ui->creationDate->setText(_snippet->creationDate().toString());
    ui->updateDate->setText(_snippet->updateDate().toString());
}

void EditSnippet::accept()
{
    // save data into snippet
    QStringList tags = ui->tags->text().trimmed().split(',');
    QStringList newTags;
    foreach(QString tg, tags) {
        QString tx = tg.trimmed();
        if(!tx.isEmpty()) {
            newTags.append(tx);
        }
    }

    //check data consistency
    if(!areDataOK() || newTags.isEmpty()) {
        Utils::error(this, tr("Mandatory fields empty or invalid."));
        return ;
    }

    _snippet->setName(ui->name->text().trimmed());
    _snippet->setDescription(ui->description->text().trimmed());
    _snippet->setPayload(ui->payload->toPlainText().trimmed());

    _snippet->setTags(newTags);

    //snippet->setUpdateUser(getUser());
    QDateTime now = QDateTime::currentDateTime() ;
    _snippet->setUpdateDate(now);
    if(_isInsert) {
        //snippet->setCreationUser(getUser());
        _snippet->setCreationDate(now);
    }
    QDialog::accept();
}
