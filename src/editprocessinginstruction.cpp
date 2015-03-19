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
#include "xmlEdit.h"
#include "editprocessinginstruction.h"
#include "utils.h"

bool editProcessingInstruction(QWidget * parent, Element *pTarget)
{
    EditProcessingInstruction edit(parent);
    edit.setTarget(pTarget);
    return (edit.exec() == QDialog::Accepted) ;
}

EditProcessingInstruction::EditProcessingInstruction(QWidget * parent) : QDialog(parent)
{
    ui.setupUi(this);
    target = NULL;
    ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

EditProcessingInstruction::~EditProcessingInstruction()
{
}

void EditProcessingInstruction::checkOKEnabled()
{
    QString target = ui.editTarget->text();
    QString data = ui.editData->text();
    bool isEnabled = true ;
    isEnabled = !(target.isEmpty() || data.isEmpty());
    ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isEnabled);
}

void EditProcessingInstruction::on_editTarget_textChanged()
{
    checkOKEnabled();
}

void EditProcessingInstruction::on_editData_textChanged()
{
    checkOKEnabled();
}

void EditProcessingInstruction::setTarget(Element* pTarget)
{
    target = pTarget ;
    //should fire an event
    ui.editTarget->setText(target->getPITarget());
    ui.editData->setText(target->getPIData());
}

void EditProcessingInstruction::accept()
{
    if(NULL == target) {
        error(tr("No target"));
        return;
    }
    QString textTarget = ui.editTarget->text();
    if(0 == textTarget.length()) {
        error(tr("Target text is invalid"));
        return ;
    }
    QString textData = ui.editData->text();
    if(0 == textData.length()) {
        error(tr("Data text is invalid"));
        return ;
    }
    target->setPITarget(textTarget);
    target->setPIData(textData);
    target->markEdited();
    QDialog::accept();
}

void EditProcessingInstruction::error(const QString& message)
{
    Utils::error(this, message);
}

