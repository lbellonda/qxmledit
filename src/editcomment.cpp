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
#include "editcomment.h"
#include "utils.h"

bool EditCommentNode(QWidget * parent, Element *pTarget)
{
    EditComment edit(parent);
    edit.setTarget(pTarget);
    return (edit.exec() == QDialog::Accepted) ;
}

EditComment::EditComment(QWidget * parent) : QDialog(parent)
{
    ui.setupUi(this);
    target = NULL;
    ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

EditComment::~EditComment()
{
}

void EditComment::on_commentText_textChanged()
{
    QString comment = ui.commentText->toPlainText();
    bool isEnabled = true ;
    isEnabled = !comment.isEmpty();
    ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isEnabled);
}

void EditComment::setTarget(Element* pTarget)
{
    target = pTarget ;
    //should fire an event
    ui.commentText->setPlainText(target->text);
}

void EditComment::accept()
{
    if(NULL == target) {
        error(tr("No target"));
        return;
    }
    QString text = ui.commentText->toPlainText();
    if(0 == text.length()) {
        error(tr("text is invalid"));
        return ;
    }
    target->setComment(text) ;
    target->markEdited();
    QDialog::accept();
}

void EditComment::error(const QString& message)
{
    Utils::error(this, message);
}

