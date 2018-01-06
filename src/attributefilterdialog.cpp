/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2018 by Luca Bellonda and individual contributors  *
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


#include "attributefilterdialog.h"
#include "ui_attributefilterdialog.h"
#include "utils.h"

AttributeFilter *AttributeFilterDialog::display(QWidget *parent, Element* element)
{
    if((NULL == element) || (element->getType() != Element::ET_ELEMENT)) {
        Utils::error(tr("Please, select an element"));
        return NULL ;
    }
    if(element->getAttributesList().size() == 0) {
        Utils::error(tr("The selected element does not have attributes."));
        return NULL ;
    }
    AttributeFilterDialog dialog(parent, element) ;
    if(dialog.exec() == QDialog::Accepted) {
        AttributeFilter *newFilter = dialog.buildFilter();
        if(NULL != newFilter) {
            return newFilter ;
        } else {
            Utils::errorOutOfMem(parent);
        }
    }
    return NULL;
}

AttributeFilterDialog::AttributeFilterDialog(QWidget *parent, Element *element) :
    QDialog(parent),
    ui(new Ui::AttributeFilterDialog)
{
    _element = element;
    ui->setupUi(this);
    completeUI();
}

AttributeFilterDialog::~AttributeFilterDialog()
{
    delete ui;
}

void AttributeFilterDialog::completeUI()
{
    QGridLayout *layout = new QGridLayout();
    layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    QList<Attribute*> attributes = _element->getAttributesList();
    QMap<QString, QString> orderMap;
    foreach(Attribute * attribute, attributes) {
        orderMap.insert(attribute->name, "");
    }

    int index = 0;
    foreach(QString name, orderMap.keys()) {
        QCheckBox *newBox = new QCheckBox();
        if(NULL == newBox) {
            Utils::errorOutOfMem(this);
            return ;
        }

        newBox->setText(name);
        attributeNamesBoxes.append(newBox);
        newBox->setChecked(true);
        int row = index / Columns;
        int column = index % Columns ;
        layout->addWidget(newBox, row, column);
        index++;
    }
    ui->groupBox->setLayout(layout);
}


void AttributeFilterDialog::on_selectAllCmd_clicked()
{
    applySelection(true);
}

void AttributeFilterDialog::on_selectNoneCmd_clicked()
{
    applySelection(false);
}

void AttributeFilterDialog::applySelection(const bool newState)
{
    foreach(QCheckBox * newBox, attributeNamesBoxes) {
        newBox->setChecked(newState);
    }
}


AttributeFilter *AttributeFilterDialog::buildFilter()
{
    AttributeFilter *newFilter = new AttributeFilter();
    if(NULL != newFilter) {
        foreach(QCheckBox * box, attributeNamesBoxes) {
            if(box->isChecked()) {
                newFilter->addName(box->text());
            }
        }
        return newFilter;
    }
    return NULL;
}



