/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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
#include "xsdgenericeditor.h"
#include "ui_xsdgenericeditor.h"
#include <QFormLayout>
#include <QLineEdit>
#include "utils.h"

// This code is experimental TODO

class XMethInfo
{
public:
    enum EType { MI_STRING, MI_CHECKBOX, MI_NUMERIC };

    const char *propertyName;
    const char *signalName;
    bool isstring;
};


class XDescriptor1/* : QObject*/
{
    /*Q_OBJECT*/
public:
    QList<XMethInfo*> methods;
    XDescriptor1();
    ~XDescriptor1();

};
XDescriptor1::XDescriptor1() {}
XDescriptor1::~XDescriptor1() {}


XMethInfo mi = {
    "name",
    "",
    true

};

XDescriptor1 dd ;

XSDGenericEditor::XSDGenericEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::XSDGenericEditor)
{
    ui->setupUi(this);
}

XSDGenericEditor::~XSDGenericEditor()
{
    delete ui;
}

void XSDGenericEditor::changeEvent(QEvent *e)
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

void XSDGenericEditor::setTarget(QObject *newTarget, XDescriptor1 &descriptor)
{
    _target = newTarget ;
    _widgets.clear();
    _properties.clear();
    if(_target != NULL) {
        QFormLayout *formLayout = new QFormLayout(this);
        foreach(XMethInfo * mi, descriptor.methods) {
            QLineEdit *newEdit = new QLineEdit(this);
            _widgets.insert(newEdit, mi);
            _properties.insert(mi->propertyName, newEdit);
            //TODO newEdit.setTag(mi);
            if(mi->isstring) {
                // bidirectional link
                connect(newEdit, SIGNAL(editTextChanged(const QString &)), this, SLOT(fieldEditTextChanged(const QString &)));
                connect(_target, mi->signalName, this, SLOT(textPropertyChanged(const char*)));
            }
            formLayout->addRow(mi->propertyName, newEdit);
        }
        setLayout(formLayout);
    }
}

/*
void XSDGenericEditor::setValue( const QString &newValue )
{
    this->ui->cmbReference->lineEdit()->setText(newValue);
}
*/

void XSDGenericEditor::fieldEditTextChanged(const QString & text)
{
    if(_target != NULL) {
        QWidget *widget = qobject_cast<QWidget*>(sender());
        // Trova il target
        XMethInfo *mi = _widgets[widget];
        if(NULL != mi) {
            _target->setProperty(mi->propertyName, text);
        }
    }
}

void XSDGenericEditor::textPropertyChanged(const char *propertyName)
{
    if(NULL != _target) {
        QLineEdit *widget = static_cast<QLineEdit*>(_properties[propertyName]);
        widget->setText(_target->property(propertyName).toString());
    }
}

