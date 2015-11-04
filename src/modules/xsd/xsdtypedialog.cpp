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


#include "xsdtypedialog.h"
#include "ui_xsdtypedialog.h"
#include "utils.h"
#include "element.h"
#include "xsdeditor/XSchemaIOContants.h"
#include <QPushButton>

XSDTypeDialog::XSDTypeDialog(const bool isInsert, const XSDOperationParameters::EObjectType objectType, const QString &startName, Element *selection, QWidget *parent) :
    QDialog(parent),
    _started(false),
    ui(new Ui::XSDTypeDialog)
{
    _started = false ;
    _objectType = objectType ;
    _params.setName(startName);
    _params.setObjType(objectType);

    setupSelection(selection, isInsert);
    ui->setupUi(this);
    updateFromData();
    _started = true ;
}

XSDTypeDialog::~XSDTypeDialog()
{
    delete ui;
}

void XSDTypeDialog::setupSelection(Element *selection, const bool isInsert)
{
    if((NULL == selection) || isInsert) {
        return ;
    }
    _params.setupFromElement(selection);
}

XSDOperationParameters::EObjectType XSDTypeDialog::decodeElement(Element *inputElement)
{
    QString localName = inputElement->localName();
    if(localName == IO_XSD_TAGELEMENT) {
        return XSDOperationParameters::EOI_ELEMENT;
    } else if(localName == IO_XSD_ATTRIBUTE) {
        return XSDOperationParameters::EOI_ATTRIBUTE;
    } else if(localName == IO_XSD_COMPLEXTYPE) {
        return XSDOperationParameters::EOI_TYPE;
    } else if(localName == IO_XSD_SIMPLETYPE) {
        return XSDOperationParameters::EOI_TYPE;
    }
    return XSDOperationParameters::EOI_ELEMENT;
}

void XSDTypeDialog::setPanelSimpleComplexType(const bool isComplexType, const bool isShowPanel)
{
    if(isShowPanel) {
        ui->childrenStack->setVisible(true);
        if(isComplexType) {
            ui->childrenStack->setCurrentIndex(1);
            ui->groupOContent->setEnabled(true);
            ui->groupSimpleType->setEnabled(false);
        } else {
            ui->childrenStack->setCurrentIndex(0);
            ui->groupOContent->setEnabled(false);
            ui->groupSimpleType->setEnabled(true);
        }
    } else {
        ui->childrenStack->setVisible(false);
        ui->groupOContent->setEnabled(false);
        ui->groupSimpleType->setEnabled(false);
    }
}


void XSDTypeDialog::updateFromData()
{
    bool isComplexContent = false;
    bool isSimpleContent = false;
    bool isSimpleType = false;
    bool isComplexType = false;
    bool isAttribute = false;
    bool isReference = false;


    if(XSDOperationParameters::EOI_ATTRIBUTE == _params.objType()) {
        isAttribute = true ;
        ui->typeComplex->setEnabled(false);
    }

    // initial values
    if(ui->ename->text() != _params.name()) {
        ui->ename->setText(_params.name());
    }
    switch(_params.typeTo()) {
    case XSDOperationParameters::EXT_REFERENCE_TYPE:
        isReference = true;
        break;
    default:
        switch(_params.typeSpec()) {
        case XSDOperationParameters::TS_COMPLEX:
            isComplexType = true ;
            if(isAttribute) {
                isSimpleType = true ;
                _params.setTypeSpec(XSDOperationParameters::TS_SIMPLE);
            }
            break;
        case XSDOperationParameters::TS_SIMPLE:
            isSimpleType = true ;
            break;
        }
    }
    if(isAttribute && !isReference) {
        isSimpleType = true ;
    }

    bool enableComplexContentPanel = false ;
    if(isReference) {
        ui->typeRef->setChecked(true);
        ui->reftype->setEditText(_params.typeName());
        ui->reftype->setEnabled(true);
    } else {
        ui->reftype->setEnabled(false);
        if(isSimpleType) {
            ui->typeSimple->setChecked(true);
        } else if(isComplexType) {
            enableComplexContentPanel = true ;
            ui->typeComplex->setChecked(true);
        } else {
            ui->typeRef->setChecked(true);
        }
    }
    ui->groupContent->setEnabled(enableComplexContentPanel);

    // individual items
    if(isComplexType) {
        switch(_params.typeContent()) {
        case XSDOperationParameters::TSC_NONE:
            ui->contentNone->setChecked(true);
            break;
        case XSDOperationParameters::TSC_SIMPLE:
            ui->contentSimple->setChecked(true);
            isSimpleContent = true ;
            break;
        case XSDOperationParameters::TSC_COMPLEX:
            ui->contentComplex->setChecked(true);
            isComplexContent = true ;
            break;
        default:
            break;
        }
    } else {
        ui->contentNone->setChecked(true);
    }

    if(isComplexType) {
        ui->cAll->setEnabled(!isComplexContent && !isSimpleContent);
        ui->cChoice->setEnabled(!isComplexContent && !isSimpleContent);
        ui->cSeq->setEnabled(!isComplexContent && !isSimpleContent);
        ui->cNone->setEnabled(true);
        ui->cGroup->setEnabled(!isComplexContent && !isSimpleContent);
        ui->cExtension->setEnabled(isComplexContent || isSimpleContent);
        ui->cRestriction->setEnabled(isComplexContent || isSimpleContent);
        ui->cBaseType->setEnabled(isComplexContent || isSimpleContent);
        ui->cBaseType->setEditText(_params.typeName());
    }
    if(isSimpleType) {
        ui->simpleTypeBase->setEditText(_params.typeName());
    }

    if(isReference) {
        setPanelSimpleComplexType(false, false);
    } else {
        if(isSimpleType) {
            setPanelSimpleComplexType(false, true);
        } else if(isComplexType) {
            setPanelSimpleComplexType(true, true);
        } else {
            setPanelSimpleComplexType(false, false);
        }
    }

    if(isComplexType) {
        bool typeBaseEnabled = false;
        if(isComplexContent) {
            switch(_params.subOper()) {
            case XSDOperationParameters::TSS_EXTENSION:
                ui->cExtension->setChecked(true);
                typeBaseEnabled = true ;
                break;
            case XSDOperationParameters::TSS_RESTRICTION:
                ui->cRestriction->setChecked(true);
                typeBaseEnabled = true ;
                break;
            default:
                ui->cNone->setChecked(true);
                break;
            }
            ui->cBaseType->setEnabled(typeBaseEnabled);
            ui->cBaseType->setEditText(_params.typeName());
        } else if(!isSimpleContent) {
            switch(_params.subOper()) {
            default:
                ui->cNone->setChecked(true);
                break;
            case XSDOperationParameters::TSS_ALL:
                ui->cAll->setChecked(true);
                break;
            case XSDOperationParameters::TSS_SEQUENCE:
                ui->cSeq->setChecked(true);
                break;
            case XSDOperationParameters::TSS_CHOICE:
                ui->cChoice->setChecked(true);
                break;
            case XSDOperationParameters::TSS_GROUP:
                ui->cGroup->setChecked(true);
                break;
            }
        } else if(isSimpleContent) {
            switch(_params.subOper()) {
            default:
                ui->cNone->setChecked(true);
                break;
            case XSDOperationParameters::TSS_EXTENSION:
                ui->cExtension->setChecked(true);
                typeBaseEnabled = true ;
                break;
            case XSDOperationParameters::TSS_RESTRICTION:
                ui->cRestriction->setChecked(true);
                typeBaseEnabled = true ;
                break;
            }
            ui->cBaseType->setEnabled(typeBaseEnabled);
            ui->cBaseType->setEditText(_params.typeName());
        }
    }
    //-----------------------------
    if(isSimpleType) {
        bool typeBaseEnabled = false ;
        switch(_params.subOper()) {
        default:
            ui->simpleTypeNone->setChecked(true);
            break;
        case XSDOperationParameters::TSS_LIST:
            ui->simpleTypeList->setChecked(true);
            break;
        case XSDOperationParameters::TSS_UNION:
            ui->simpleTypeUnion->setChecked(true);
            break;
        case XSDOperationParameters::TSS_RESTRICTION:
            ui->simpleTypeRestriction->setChecked(true);
            typeBaseEnabled = true ;
            break;
        case XSDOperationParameters::TSS_NONE:
            ui->simpleTypeNone->setChecked(true);
            break;
        }
        ui->simpleTypeBase->setEnabled(typeBaseEnabled);
        ui->simpleTypeBase->setEditText(_params.typeName());
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    checkEnable();
}

void XSDTypeDialog::checkEnable()
{
    bool isOk = true ;
    if(_params.typeName().isEmpty()) {
        if((_params.subOper() == XSDOperationParameters::TSS_EXTENSION)
                || (_params.subOper() == XSDOperationParameters::TSS_RESTRICTION)
                || (_params.typeTo() == XSDOperationParameters::EXT_REFERENCE_TYPE)) {
            isOk = false;
        }
    }
    if(_params.name().isEmpty()) {
        isOk = false;
    }
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isOk);
}


XSDOperationParameters* XSDTypeDialog::getParameters()
{
    XSDOperationParameters *newParams  = new XSDOperationParameters();
    if(NULL != newParams) {
        _params.copyTo(newParams);
    }
    return newParams  ;
}

void XSDTypeDialog::onUpdate()
{
    updateFromData();
}

void XSDTypeDialog::on_typeRef_clicked(bool /*clicked*/)
{
    if(_started) {
        _params.setTypeTo(XSDOperationParameters::EXT_REFERENCE_TYPE);
        onUpdate();
    }
}

void XSDTypeDialog::on_typeComplex_clicked(bool /*clicked*/)
{
    if(_started) {
        _params.setTypeTo(XSDOperationParameters::EXT_ANONYMOUS_TYPE);
        _params.setTypeSpec(XSDOperationParameters::TS_COMPLEX);
        onUpdate();
    }
}

void XSDTypeDialog::on_typeSimple_clicked(bool /*clicked*/)
{
    if(_started) {
        _params.setTypeTo(XSDOperationParameters::EXT_ANONYMOUS_TYPE);
        _params.setTypeSpec(XSDOperationParameters::TS_SIMPLE);
        onUpdate();
    }
}

void XSDTypeDialog::on_contentSimple_clicked(bool /*clicked*/)
{
    if(_started) {
        _params.setTypeContent(XSDOperationParameters::TSC_SIMPLE);
        onUpdate();
    }
}

void XSDTypeDialog::on_contentComplex_clicked(bool /*clicked*/)
{
    if(_started) {
        _params.setTypeContent(XSDOperationParameters::TSC_COMPLEX);
        onUpdate();
    }
}

void XSDTypeDialog::on_contentNone_clicked(bool /*clicked*/)
{
    if(_started) {
        _params.setTypeContent(XSDOperationParameters::TSC_NONE);
        onUpdate();
    }
}

void XSDTypeDialog::on_ename_textEdited(const QString & text)
{
    if(_started) {
        _params.setName(text);
        onUpdate();
    }
}


void XSDTypeDialog::on_reftype_editTextChanged(const QString & text)
{
    if(_started) {
        _params.setTypeName(text);
        onUpdate();
    }
}

//----
void XSDTypeDialog::on_cNone_clicked(bool /*clicked*/)
{
    if(_started) {
        _params.setSubOper(XSDOperationParameters::TSS_NONE);
        onUpdate();
    }
}

void XSDTypeDialog::on_simpleTypeNone_clicked(bool /*clicked*/)
{
    if(_started) {
        _params.setSubOper(XSDOperationParameters::TSS_NONE);
        onUpdate();
    }
}

void XSDTypeDialog::on_cExtension_clicked(bool /*clicked*/)
{
    if(_started) {
        _params.setSubOper(XSDOperationParameters::TSS_EXTENSION);
        onUpdate();
    }
}

void XSDTypeDialog::on_cRestriction_clicked(bool /*clicked*/)
{
    if(_started) {
        _params.setSubOper(XSDOperationParameters::TSS_RESTRICTION);
        onUpdate();
    }
}

void XSDTypeDialog::on_cAll_clicked(bool /*clicked*/)
{
    if(_started) {
        _params.setSubOper(XSDOperationParameters::TSS_ALL);
        onUpdate();
    }
}

void XSDTypeDialog::on_cGroup_clicked(bool /*clicked*/)
{
    if(_started) {
        _params.setSubOper(XSDOperationParameters::TSS_GROUP);
        onUpdate();
    }
}

void XSDTypeDialog::on_cSeq_clicked(bool /*clicked*/)
{
    if(_started) {
        _params.setSubOper(XSDOperationParameters::TSS_SEQUENCE);
        onUpdate();
    }
}

void XSDTypeDialog::on_cChoice_clicked(bool /*clicked*/)
{
    if(_started) {
        _params.setSubOper(XSDOperationParameters::TSS_CHOICE);
        onUpdate();
    }
}

void XSDTypeDialog::on_simpleTypeUnion_clicked(bool /*clicked*/)
{
    if(_started) {
        _params.setSubOper(XSDOperationParameters::TSS_UNION);
        onUpdate();
    }
}

void XSDTypeDialog::on_simpleTypeList_clicked(bool /*clicked*/)
{
    if(_started) {
        _params.setSubOper(XSDOperationParameters::TSS_LIST);
        onUpdate();
    }
}

void XSDTypeDialog::on_simpleTypeRestriction_clicked(bool /*clicked*/)
{
    if(_started) {
        _params.setSubOper(XSDOperationParameters::TSS_RESTRICTION);
        onUpdate();
    }
}

void XSDTypeDialog::on_simpleTypeBase_editTextChanged(const QString & text)
{
    if(_started) {
        _params.setTypeName(text);
        onUpdate();
    }
}

void XSDTypeDialog::on_cBaseType_editTextChanged(const QString & text)
{
    if(_started) {
        _params.setTypeName(text);
        onUpdate();
    }
}
