/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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


#include "anoneditprofiledialog.h"
#include "ui_anoneditprofiledialog.h"
#include "utils.h"
#include <QPushButton>


#define MAXCHARS_NAME   40
#define MAXCHARS_DESCR  100

AnonEditProfileDialog::AnonEditProfileDialog(const bool isInsert, GenericPersistentData *snippet, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnonEditProfileDialog),
    _snippet(snippet),
    _isInsert(isInsert)
{
    //setModal(true);
    ui->setupUi(this);
    ui->name->setMaxLength(MAXCHARS_NAME);
    ui->description->setMaxLength(MAXCHARS_DESCR);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    setUpData();
    checkIfAcceptable();
}

AnonEditProfileDialog::~AnonEditProfileDialog()
{
    delete ui;
}

bool AnonEditProfileDialog::editProfile()
{
    if(exec() == QDialog::Accepted) {
        return true;
    }
    return false ;
}

void AnonEditProfileDialog::changeEvent(QEvent *e)
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


bool AnonEditProfileDialog::areDataOK()
{
    if(ui->name->text().trimmed().isEmpty()
            ||  ui->description->text().isEmpty()
            ||  ui->tags->text().trimmed().isEmpty()) {
        return false;
    }
    return true;
}

void AnonEditProfileDialog::setUpData()
{
    ui->name->setText(_snippet->name());
    ui->description->setText(_snippet->description());
    // convert tags in qstring
    QStringList tagList = QStringList::fromSet(_snippet->tags());
    ui->tags->setText(tagList.join(","));
    ui->creationDate->setText(_snippet->creationDate().toString());
    ui->updateDate->setText(_snippet->updateDate().toString());
}

void AnonEditProfileDialog::accept()
{
    // save data into snippet
    QSet<QString> newTags = tagsFromUI();

    //check data consistency
    if(!areDataOK() || newTags.isEmpty()) {
        Utils::error(this, tr("Mandatory fields empty or invalid."));
        return ;
    }

    _snippet->setName(ui->name->text().trimmed());
    _snippet->setDescription(ui->description->text().trimmed());

    _snippet->setTags(newTags);

    QDateTime now = QDateTime::currentDateTime() ;
    _snippet->setUpdateDate(now);
    if(_isInsert) {
        _snippet->setCreationDate(now);
    }
    QDialog::accept();
}

QSet<QString> AnonEditProfileDialog::tagsFromUI()
{
    QStringList tags = ui->tags->text().trimmed().split(',');
    QSet<QString> newTags;
    foreach(QString tg, tags) {
        QString tx = tg.trimmed();
        if(!tx.isEmpty()) {
            if(!newTags.contains(tx)) {
                newTags.insert(tx);
            }
        }
    }
    return newTags;
}


/**
 * @brief AnonEditProfileDialog::checkOk check if the accept action can be enabled
 * @return
 */
bool AnonEditProfileDialog::checkIfAcceptable()
{
    QSet<QString> newTags = tagsFromUI();

    //check data consistency
    if(!areDataOK() || newTags.isEmpty()) {
        return false ;
    }
    return true;
}

void AnonEditProfileDialog::enableOkIf()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(checkIfAcceptable());
}

void AnonEditProfileDialog::on_name_textChanged(const QString & /*text*/)
{
    enableOkIf();
}

void AnonEditProfileDialog::on_tags_textChanged(const QString & /*text*/)
{
    enableOkIf();
}
void AnonEditProfileDialog::on_description_textChanged(const QString & /*text*/)
{
    enableOkIf();
}

