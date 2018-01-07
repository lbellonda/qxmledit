/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013-2018 by Luca Bellonda and individual contributors  *
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


#include "metadatainfodialog.h"
#include "ui_metadatainfodialog.h"
#include <QPushButton>
#include <QDialogButtonBox>

//------

MetadataEditRow::MetadataEditRow()
{
    check = NULL ;
    edit = NULL ;
    attribute = NULL ;
}

MetadataEditRow::~MetadataEditRow()
{
}

void MetadataEditRow::setup(QCheckBox *theCheck, QLineEdit *theEdit, PseudoAttribute *theAttribute)
{
    check = theCheck;
    edit = theEdit;
    attribute = theAttribute;
    if(attribute->exists()) {
        edit->setText(attribute->value());
    } else {
        edit->setText("");
    }
    edit->setEnabled(attribute->exists());
    check->setChecked(attribute->exists());
}

void MetadataEditRow::accept()
{
    attribute->setUsed(check->isChecked());
    if(check->isChecked()) {
        attribute->setValue(edit->text());
    } else {
        attribute->setUsed(false);
    }
}

//------


MetaDataInfoDialog::MetaDataInfoDialog(const bool isReadOnly, MetadataInfo *info, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MetaDataInfoDialog)
{
    _info = info ;
    _isReadOnly = isReadOnly ;
    ui->setupUi(this);
    setupNonEditable();
    setupEditable();
    if(_isReadOnly) {
        ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);
    }
}

MetaDataInfoDialog::~MetaDataInfoDialog()
{
    delete ui;
}

void MetaDataInfoDialog::setSingleDatum(QLabel *label, PseudoAttribute *attribute)
{
    if(!attribute->exists()) {
        label->setText(tr("<Not existent>"));
    } else {
        label->setText(attribute->value());
    }
}

// Set up non editable data.
void MetaDataInfoDialog::setupNonEditable()
{
    setSingleDatum(ui->lblCreationDate, _info->updatable()->creationDate());
    setSingleDatum(ui->lblCreationUser, _info->updatable()->creationUser());
    setSingleDatum(ui->lblRevision, _info->updatable()->revision());
    setSingleDatum(ui->lblMetaModelInfo, _info->updatable()->metaVersion());
    setSingleDatum(ui->lblUpdateDate, _info->updatable()->updateDate());
    setSingleDatum(ui->lblUpdateUser, _info->updatable()->updateUser());
}

void MetaDataInfoDialog::setupEditable()
{
    if(_isReadOnly) {
        ui->groupEditable->setEnabled(false);
    }
    _projectEdit.setup(ui->chkProject, ui->editPrject, &_info->project);
    _copyEdit.setup(ui->chkCopy, ui->editCopy, &_info->copyright);
    _versionEdit.setup(ui->chkVersion, ui->editVersion, &_info->version);
    _domainEdit.setup(ui->chkDomain, ui->editDomain, &_info->domain);
    _nameEdit.setup(ui->chkName, ui->editName, &_info->name);
}

void MetaDataInfoDialog::actionOnVariable(MetadataEditRow *metaEditRow)
{
    if(_isReadOnly) {
        return ;
    }
    metaEditRow->edit->setEnabled(metaEditRow->check->isChecked());
}

void MetaDataInfoDialog::on_chkProject_clicked(bool /*state*/)
{
    actionOnVariable(&_projectEdit);
}
void MetaDataInfoDialog::on_chkCopy_clicked(bool /*state*/)
{
    actionOnVariable(&_copyEdit);
}
void MetaDataInfoDialog::on_chkVersion_clicked(bool /*state*/)
{
    actionOnVariable(&_versionEdit);
}
void MetaDataInfoDialog::on_chkDomain_clicked(bool /*state*/)
{
    actionOnVariable(&_domainEdit);
}
void MetaDataInfoDialog::on_chkName_clicked(bool /*state*/)
{
    actionOnVariable(&_nameEdit);
}

//------

void MetaDataInfoDialog::accept()
{
    if(!_isReadOnly) {
        _projectEdit.accept();
        _copyEdit.accept();
        _versionEdit.accept();
        _domainEdit.accept();
        _nameEdit.accept();
    }
    QDialog::accept();
}
