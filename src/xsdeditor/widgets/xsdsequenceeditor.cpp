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
#include "xsdsequenceeditor.h"
#include "ui_xsdsequenceeditor.h"

XsdSequenceEditor::XsdSequenceEditor(QWidget *parent) :
    QWidget(parent),
    _target(NULL),
    ui(new Ui::XsdSequenceEditor)
{
    ui->setupUi(this);
}

XsdSequenceEditor::~XsdSequenceEditor()
{
    delete ui;
}

void XsdSequenceEditor::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch(e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


void XsdSequenceEditor::setTarget(XSchemaObject *newGenericTarget)
{
    XSchemaSequence *newTarget = qobject_cast<XSchemaSequence *>(newGenericTarget);
    if(newTarget != _target) {
        if(_target != NULL) {
            //disconnect( _target, SIGNAL(propertyChanged(const QString &)), this, SLOT(onTargetPropertyChanged(const QString &)) );
        }
        _target = newTarget ;
        if(_target != NULL) {
            //connect( _target, SIGNAL(propertyChanged(const QString &)), this, SLOT(onTargetPropertyChanged(const QString &)) );
            //applyPropertyChanged( "", true );
        }
    }
}

XSchemaObject *XsdSequenceEditor::target()
{
    return _target ;
}
