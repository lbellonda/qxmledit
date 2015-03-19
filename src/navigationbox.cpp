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
#include "navigationbox.h"
#include "ui_navigationbox.h"

NavigationBox::NavigationBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NavigationBox)
{
    _minValue = 0 ;
    _maxValue = 0 ;
    _value = 0 ;
    ui->setupUi(this);
    setValues();
}

NavigationBox::~NavigationBox()
{
    delete ui;
}


/** set the range of the allowed values
  */
void NavigationBox::setRange(const int minValue, const int maxValue)
{
    _minValue = minValue ;
    _maxValue = maxValue ;
    setValues();
}

void NavigationBox::setValues()
{
    ui->valueBox->setRange(_minValue, _maxValue);
    ui->minVal->setText(QString::number(_minValue));
    ui->maxVal->setText(QString::number(_maxValue));
    ui->slider->setRange(_minValue, _maxValue);
    ui->valueBox->setValue(_value);
    _value = ui->valueBox->value();
    ui->slider->setValue(_value);
    enableButtons();
}

void NavigationBox::enableButtons()
{
    ui->goPrev->setEnabled(_value > _minValue);
    ui->goNext->setEnabled(_value < _maxValue);
}

void NavigationBox::notify()
{
    _value = ui->valueBox->value();
    emit navigateTo(_value);
}

void NavigationBox::updateValue()
{
    _value = ui->valueBox->value();
    enableButtons();
}


//------------------------------------------------------------------------

void NavigationBox::on_goPrev_clicked()
{
    _value = ui->valueBox->value() - 1;
    ui->valueBox->setValue(_value);
    updateValue();
    notify();
}

void NavigationBox::on_goNext_clicked()
{
    _value = ui->valueBox->value() + 1;
    ui->valueBox->setValue(_value);
    updateValue();
    notify();
}

void NavigationBox::on_go_clicked()
{
    updateValue();
    notify();
}

void NavigationBox::on_slider_valueChanged(int value)
{
    ui->valueBox->setValue(value);
    updateValue();
}

void NavigationBox::on_valueBox_valueChanged(int value)
{
    updateValue();
    ui->slider->setValue(value);
}
