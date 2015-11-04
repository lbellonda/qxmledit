/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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


#include "replicasettingsdialog.h"
#include "ui_replicasettingsdialog.h"
#include "utils.h"
#include "modules/replica/replicacommand.h"

ReplicaSettingsDialog::ReplicaSettingsDialog(Element *base, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReplicaSettingsDialog)
{
    _base = base ;
    ui->setupUi(this);
    setupValues();
}

ReplicaSettingsDialog::~ReplicaSettingsDialog()
{
    delete ui;
}

void ReplicaSettingsDialog::setupValues()
{
    ReplicaCommand defaultCommand;
    {
        QStringList attributes ;
        if(NULL != _base) {
            foreach(Attribute * attribute,  _base->getAttributesList()) {
                attributes.append(attribute->name);
            }
        }
        Utils::loadComboTextArrays(ui->attributes, "", attributes, attributes);
    }

    {
        QStringList labels;
        QList<int> formats;
        formats <<  ReplicaCommand::NumberUnpadded << ReplicaCommand::NumberPadded << ReplicaCommand::AlphaUnpadded << ReplicaCommand::AlphaPadded ;
        labels << tr("Unpadded number") << tr("Padded number") << tr("Unpadded string ") << tr("Padded string") ;
        Utils::loadComboCodedArrays(ui->cbFormat, defaultCommand.format(), labels, formats);
    }
    ui->cbApplyChildren->setChecked(defaultCommand.recursive());
    ui->cbOverWrite->setChecked(defaultCommand.overwrite());
    ui->startNumber->setValue(defaultCommand.startNumber());
    ui->cbAtEnd->setChecked(defaultCommand.atEnd());
    ui->separator->setText(defaultCommand.separator());
}


void ReplicaSettingsDialog::readValues(ReplicaCommand *cmd)
{
    cmd->setAttributeName(ui->attributes->currentText());
    cmd->setFormat((ReplicaCommand::EFormat)ui->cbFormat->itemData(ui->cbFormat->currentIndex()).toInt());
    cmd->setRecursive(ui->cbApplyChildren->isChecked());
    cmd->setOverwrite(ui->cbOverWrite->isChecked());
    cmd->setStartNumber(ui->startNumber->value());
    cmd->setAtEnd(ui->cbAtEnd->isChecked());
    cmd->setSeparator(ui->separator->text());
}


ReplicaCommand::EErrorCode ReplicaSettingsDialog::checkData(ReplicaCommand *cmd)
{
    readValues(cmd);
    const ReplicaCommand::EErrorCode code = cmd->check();
    return code ;
}

bool ReplicaSettingsDialog::evalOk()
{
    ReplicaCommand cmd;
    const ReplicaCommand::EErrorCode code = checkData(&cmd);
    if(code == ReplicaCommand::NoError) {
        return true;
    }
    return false ;
}

void ReplicaSettingsDialog::accept()
{
    const ReplicaCommand::EErrorCode code = checkData(&_result);
    if(code == ReplicaCommand::NoError) {
        QDialog::accept();
        return ;
    }
    Utils::error(this, tr("Error: %1").arg(_result.errorMessage(code)));
}

void ReplicaSettingsDialog::on_separator_textEdited(const QString & /*text*/)
{
    evalOk();
}

void ReplicaSettingsDialog::on_startNumber_valueChanged(int /*i*/)
{
    evalOk();
}

void ReplicaSettingsDialog::on_attributes_editTextChanged(const QString & /*text*/)
{
    evalOk();
}

ReplicaCommand *ReplicaSettingsDialog::result()
{
    ReplicaCommand *cloned = _result.clone();
    return cloned ;
}
