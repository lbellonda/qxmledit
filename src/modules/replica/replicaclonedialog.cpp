/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2018 by Luca Bellonda and individual contributors  *
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


#include "replicaclonedialog.h"
#include "ui_replicaclonedialog.h"
#include "replicacloneinfo.h"
#include "replicacommand.h"
#include "replicasettingsdialog.h"
#include "utils.h"

ReplicaCloneDialog::ReplicaCloneDialog(QWidget *parent, Element *theElement) :
    QDialog(parent),
    ui(new Ui::ReplicaCloneDialog)
{
    _fillWasCleared = false ;
    _fillInfo = NULL ;
    _element = theElement;
    ui->setupUi(this);
    enableDeleteFillInfo();
}

ReplicaCloneDialog::~ReplicaCloneDialog()
{
    delete ui;
    deleteFillInfo();
}

void ReplicaCloneDialog::deleteFillInfo()
{
    if(NULL != _fillInfo) {
        delete _fillInfo;
        _fillInfo = NULL ;
    }
}

ReplicaCloneInfo *ReplicaCloneDialog::results()
{
    ReplicaCloneInfo *info = new ReplicaCloneInfo();
    if(NULL != _fillInfo) {
        info->setFillInfo(_fillInfo->clone());
    }
    info->setDeep(ui->cbRecursive->isChecked());
    info->setNumClones(ui->numClones->value());
    return info ;
}

void ReplicaCloneDialog::on_removeIndex_clicked()
{
    _fillWasCleared = true ;
    deleteFillInfo();
    enableDeleteFillInfo();
}

void ReplicaCloneDialog::on_addIndex_clicked()
{
    _fillWasCleared = true ;
    addIndex();
    enableDeleteFillInfo();
}

void ReplicaCloneDialog::addIndex()
{
    ReplicaSettingsDialog dlg(_element, this);
    dlg.setModal(true);
    if(dlg.exec() == QDialog::Accepted) {
        ReplicaCommand *command = dlg.result();
        if(NULL != command) {
            deleteFillInfo();
            _fillInfo = command;
        }
    }
}

void ReplicaCloneDialog::enableDeleteFillInfo()
{
    ui->removeIndex->setEnabled(NULL != _fillInfo);
}

void ReplicaCloneDialog::accept()
{
    if((NULL == _fillInfo) && !_fillWasCleared) {
        if(Utils::askYN(this, tr("Do you want to use an unique index for the new data?"))) {
            addIndex();
        }
    }
    QDialog::accept();
}
