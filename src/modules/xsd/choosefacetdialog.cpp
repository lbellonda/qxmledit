/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016 by Luca Bellonda and individual contributors       *
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


#include "choosefacetdialog.h"
#include "ui_choosefacetdialog.h"
#include "utils.h"

#define SELECT(val) case XSDFacet::val: ui->val->setChecked(true) ; break;
#define CHECK(val) if(ui->val->isChecked()) { type = XSDFacet::val ; }

ChooseFacetDialog::ChooseFacetDialog(QWidget *parent, XSDFacet * inputFacet) :
    QDialog(parent),
    ui(new Ui::ChooseFacetDialog)
{
    _facet = inputFacet ;
    ui->setupUi(this);

    ui->fixed->clear();

    _buttonGroup = new QButtonGroup(this);
    connect(_buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(onTypeIndexChanged(int)));
    _buttonGroup->addButton(ui->MinExclusive);
    _buttonGroup->addButton(ui->MinInclusive);
    _buttonGroup->addButton(ui->MaxExclusive);
    _buttonGroup->addButton(ui->MaxInclusive);
    _buttonGroup->addButton(ui->TotalDigits);
    _buttonGroup->addButton(ui->FractionDigits);
    _buttonGroup->addButton(ui->Length);
    _buttonGroup->addButton(ui->MinLength);
    _buttonGroup->addButton(ui->MaxLength);
    _buttonGroup->addButton(ui->Enumeration);
    _buttonGroup->addButton(ui->WhiteSpace);
    _buttonGroup->addButton(ui->Pattern);

    {
        switch(_facet->type()) {
            SELECT(MinExclusive);
            SELECT(MinInclusive);
            SELECT(MaxExclusive);
            SELECT(MaxInclusive);
            SELECT(TotalDigits);
            SELECT(FractionDigits);
            SELECT(Length);
            SELECT(MinLength);
            SELECT(MaxLength);
            SELECT(Enumeration);
            SELECT(WhiteSpace);
            SELECT(Pattern);
        default: Utils::error(this, tr("Unknown facet")); break;
        }
    }
    ui->value->setText(_facet->value());
    QStringList itemsText;
    QStringList itemsValue;
    itemsText << "true" << "false" << tr("<Not used>");
    itemsValue << "true" << "false" << "unk";
    QString selValue = "unk" ;
    switch(_facet->fixed()) {
    default:
        selValue = "unk" ;
        break;
    case XEnums::XBOOL_TRUE:
        selValue = "true" ;
        break;
    case XEnums::XBOOL_FALSE:
        selValue = "false" ;
        break;
    }

    Utils::loadComboTextArrays(ui->fixed, selValue, itemsText, itemsValue);
    checkEnabled();
}

ChooseFacetDialog::~ChooseFacetDialog()
{
    delete ui;
}

void ChooseFacetDialog::doIt()
{
    accept();
}

XSDFacet::EType ChooseFacetDialog::readType()
{
    XSDFacet::EType type = XSDFacet::Enumeration;
    CHECK(MinExclusive);
    CHECK(MinInclusive);
    CHECK(MaxExclusive);
    CHECK(MaxInclusive);
    CHECK(TotalDigits);
    CHECK(FractionDigits);
    CHECK(Length);
    CHECK(MinLength);
    CHECK(MaxLength);
    CHECK(Enumeration);
    CHECK(WhiteSpace);
    CHECK(Pattern);
    return type;
}

void ChooseFacetDialog::accept()
{
    QStringList wsList ;
    wsList << "preserve" << "replace" << "collapse" ;
    QString value = ui->value->text();
    XSDFacet::EType type = readType();
    if(XSDFacet::WhiteSpace == type) {
        if(ui->WhiteSpace->isChecked()) {
            if(!wsList.contains(value)) {
                Utils::error(this, tr("Valid values for whitespace are: %1").arg(wsList.join(",")));
                return ;
            }
        }
    }
    _facet->setType(type);
    _facet->setValue(value);
    if(_facet->hasFixed()) {
        int index = ui->fixed->currentIndex();
        if(index >= 0) {
            QString value = ui->fixed->itemData(index).toString();
            _facet->setFixedString(value);
        }
    }
    QDialog::accept();
}

void ChooseFacetDialog::checkEnabled()
{
    XSDFacet::EType aType = readType();
    XSDFacet facet(aType, "");
    bool hasFixed = facet.hasFixed();
    ui->lblFixed->setVisible(hasFixed);
    ui->fixed->setVisible(hasFixed);
}

void ChooseFacetDialog::onTypeIndexChanged(int /*index*/)
{
    checkEnabled();
}
