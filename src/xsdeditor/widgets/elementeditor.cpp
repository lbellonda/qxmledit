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

#include "elementeditor.h"
#include "xsdeditor/xproperties.h"
#include "ui_elementeditor.h"
#include "WidgetUtility.h"

#include "utils.h"

ElementEditor::ElementEditor(QWidget *parent) :
    QWidget(parent),
    _target(NULL),
    ui(new Ui::ElementEditor)
{
    ui->setupUi(this);
    finalizeUI();
}

ElementEditor::~ElementEditor()
{
    delete ui;
}

void ElementEditor::changeEvent(QEvent *e)
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

void ElementEditor::finalizeUI()
{
    WidgetUtility::LoadComboBoxFinalType(ui->cmbFinal);
    WidgetUtility::LoadComboBoxFinalType(ui->cmbBlock);
    WidgetUtility::LoadComboBoxElementType(ui->cmbElementCategory);
    QStringList customTypes;
    //TODO
    WidgetUtility::loadComboTypes(ui->type, customTypes);
}

void ElementEditor::setTarget(XSchemaObject *newGenericTarget)
{
    XSchemaElement *newTarget = qobject_cast<XSchemaElement *>(newGenericTarget);
    if(newTarget != _target) {
        if(_target != NULL) {
            disconnect(_target, SIGNAL(propertyChanged(const QString &)), this, SLOT(onTargetPropertyChanged(const QString &)));
        }
        _target = newTarget ;
        if(_target != NULL) {
            connect(_target, SIGNAL(propertyChanged(const QString &)), this, SLOT(onTargetPropertyChanged(const QString &)));
            applyPropertyChanged("", true);
        }
    }
}

XSchemaObject *ElementEditor::target()
{
    return _target ;
}


void ElementEditor::onTargetPropertyChanged(const QString &propertyName)
{
    applyPropertyChanged(propertyName, false);
}

void ElementEditor::applyPropertyChanged(const QString &propertyName, const bool isAll)
{
    if((propertyName == PROPERTY_ELEMENT_FIXED) || isAll) {
        setFixed(_target->fixed());
    }
    if((propertyName == PROPERTY_ELEMENT_ABSTRACT) || isAll) {
        setAbstract(_target->abstract());
    }
    if((propertyName == PROPERTY_ELEMENT_NILLABLE) || isAll) {
        setNillable(_target->nillable());
    }
    if((propertyName == PROPERTY_ELEMENT_DEFAULTVALUE) || isAll) {
        setDefaultValue(_target->defaultValue());
    }
    if((propertyName == PROPERTY_ELEMENT_REF) || isAll) {
        setRef(_target->ref());
    }
    /*if( ( propertyName == PROPERTY_ELEMENT_SUBSTITUTIONGROUP ) || isAll ) {
    }*/
    /* TODO if( ( propertyName == PROPERTY_ELEMENT_MAXOCCURS ) || isAll ) {
        setMinOccurs(_target->maxOccurs());
    }
    if( ( propertyName == PROPERTY_ELEMENT_MINOCCURS ) || isAll ) {
        setMaxOccurs(_target->minOccurs());
    } */
    /*TODO if((propertyName == PROPERTY_ELEMENT_FINAL) || isAll) {
        setFinal(_target->final());
    }
    if((propertyName == PROPERTY_ELEMENT_BLOCK) || isAll) {
        setBlock(_target->block());
    }*/
    if((propertyName == PROPERTY_ELEMENT_TYPE) || isAll) {
        setType(_target->xsdType());
    }
    if((propertyName == PROPERTY_ELEMENT_CATEGORY) || isAll) {
        setCategory(_target->category());
    }
}


void ElementEditor::setAbstract(const XEnums::XBool newValue)
{
    WidgetUtility::setCheckState(newValue, this->ui->chkAbstract);
}

void ElementEditor::on_chkAbstract_clicked()
{
    if(_target != NULL) {
        _target->setAbstract(WidgetUtility::getCheckState(this->ui->chkAbstract));
    }
}

void ElementEditor::setNillable(const XEnums::XBool newValue)
{
    WidgetUtility::setCheckState(newValue, this->ui->chkNillable);
}

void ElementEditor::on_chkNillable_clicked()
{
    if(_target != NULL) {
        _target->setNillable(WidgetUtility::getCheckState(this->ui->chkNillable));
    }
}

void ElementEditor::setFixed(const QString &newValue)
{
    this->ui->fixed->setText(newValue);
}

void ElementEditor::on_fixed_textChanged(const QString &text)
{
    if(_target != NULL) {
        _target->setFixed(text);
    }
}

void ElementEditor::setDefaultValue(const QString &newValue)
{
    this->ui->defaultValue->setText(newValue);
}

void ElementEditor::on_defaultValue_textChanged(const QString &text)
{
    if(_target != NULL) {
        _target->setDefaultValue(text);
    }
}

void ElementEditor::setMinOccurs(const int /*newValue*/)
{
    /* TODO
      this->ui->minOccurs->setValue(newValue);*/
}

void ElementEditor::on_minOccurs_valueChanged(const int /*newValue*/)
{
    /* TODO
       if( _target != NULL ) {
           _target->setMinOccurs(newValue);
       }*/
}

void ElementEditor::setMaxOccurs(const int /*newValue*/)
{
    /* TODO
       this->ui->maxOccurs->setValue(newValue);*/
}

void ElementEditor::on_maxOccurs_valueChanged(const int /*newValue*/)
{
    /* TODO
       if( _target != NULL ) {
           _target->setMaxOccurs(newValue);
       }*/
}

void ElementEditor::setFinal(const XSchemaObject::EFinalType /*newValue*/)
{
    /*TODO WidgetUtility::selectItemWithData(this->ui->cmbFinal, (int)newValue);*/
}

void ElementEditor::on_cmbFinal_currentIndexChanged(const int /*newValue*/)
{
    /*TODO if(_target != NULL) {
        XSchemaObject::EFinalType finalType ;
        if(WidgetUtility::getComboBoxFinalTypeSelected(ui->cmbFinal, finalType)) {
            _target->setFinal(finalType);
        }
    }*/
}

void ElementEditor::setBlock(const XSchemaObject::EFinalType /*newValue*/)
{
    /* TODO WidgetUtility::selectItemWithData(this->ui->cmbBlock, (int)newValue);*/
}

void ElementEditor::on_cmbBlock_currentIndexChanged(const int /*newValue*/)
{
    /* TODO if(_target != NULL) {
        XSchemaObject::EFinalType blockType ;
        if(WidgetUtility::getComboBoxFinalTypeSelected(ui->cmbBlock, blockType)) {
            _target->setBlock(blockType);
        }
    }*/
}

void ElementEditor::setType(const QString &newValue)
{
    this->ui->type->lineEdit()->setText(newValue);
}

void ElementEditor::on_type_editTextChanged(const QString & text)
{
    if(_target != NULL) {
        _target->setXsdType(text);
    }
}

void ElementEditor::setCategory(const XSchemaElement::ElementCategory newValue)
{
    WidgetUtility::selectItemWithData(this->ui->cmbElementCategory, (int)newValue);
}

void ElementEditor::on_cmbElementCategory_currentIndexChanged(const int /*newValue*/)
{
    if(_target != NULL) {
        int data = 0;
        if(WidgetUtility::getComboBoxIntSelected(ui->cmbElementCategory, data)) {
            _target->setCategory((XSchemaElement::ElementCategory)data);
        }
    }
}

void ElementEditor::setRef(const QString &newValue)
{
    this->ui->cmbReference->lineEdit()->setText(newValue);
}

void ElementEditor::on_cmbReference_editTextChanged(const QString & text)
{
    if(_target != NULL) {
        _target->setRef(text);
    }
}
