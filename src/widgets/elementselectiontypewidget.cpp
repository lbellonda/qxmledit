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


#include "elementselectiontypewidget.h"
#include "ui_elementselectiontypewidget.h"
#include "utils.h"

ElementSelectionTypeWidget::ElementSelectionTypeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ElementSelectionTypeWidget)
{
    _fireEvents = false;
    ui->setupUi(this);
    setSelectionTypeInner(TargetSelection::AllItems);
    _fireEvents = true ;
}

ElementSelectionTypeWidget::~ElementSelectionTypeWidget()
{
    delete ui;
}

TargetSelection::Type ElementSelectionTypeWidget::selectionType() const
{
    return _selectionType;
}

bool ElementSelectionTypeWidget::setSelectionTypeInner(const TargetSelection::Type value)
{
    const bool isChanged = (_selectionType != value);
    _selectionType = value;
    switch(_selectionType) {
    default: {
        QList<QAbstractButton*> buttons = ui->group->buttons();
        foreach(QAbstractButton * button , buttons) {
            button->setChecked(false);
        }
    }
    break;
    case TargetSelection::AllItems:
        ui->all->setChecked(true);
        break;
    case TargetSelection::SelectedItem:
        ui->sel->setChecked(true);
        ui->cbChildren->setChecked(false);
        break;
    case TargetSelection::SelectedItemAndChildren:
        ui->sel->setChecked(true);
        ui->cbChildren->setChecked(true);
        break;
    case TargetSelection::Bookmarks:
        ui->bookmarks->setChecked(true);
        ui->cbChildren->setChecked(false);
        break;
    case TargetSelection::BookmarksAndChildren:
        ui->bookmarks->setChecked(true);
        ui->cbChildren->setChecked(true);
        break;
    }
    enableUI();
    return isChanged ;
}

void ElementSelectionTypeWidget::setSelectionType(const TargetSelection::Type value)
{
    if(setSelectionTypeInner(value)) {
        // avoid "and" optimization
        if(_fireEvents) {
            emit selectionTypeChanged(_selectionType);
        }
    }
}

void ElementSelectionTypeWidget::on_cbChildren_clicked()
{
    evaluateState();
}

void ElementSelectionTypeWidget::on_group_buttonClicked(QAbstractButton * /*button*/)
{
    evaluateState();
}

void ElementSelectionTypeWidget::evaluateState()
{
    // decode the value
    bool children = ui->cbChildren->isChecked();
    if(ui->all->isChecked()) {
        setSelectionType(TargetSelection::AllItems);
    } else if(ui->sel->isChecked()) {
        setSelectionType(children ? TargetSelection::SelectedItemAndChildren : TargetSelection::SelectedItem);
    } else if(ui->bookmarks->isChecked()) {
        setSelectionType(children ? TargetSelection::BookmarksAndChildren : TargetSelection::Bookmarks);
    }
}

void ElementSelectionTypeWidget::enableUI()
{
    ui->cbChildren->setEnabled(TargetSelection::AllItems != _selectionType);
}
