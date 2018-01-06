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


#include "xsdbackgroundconfig.h"
#include "ui_xsdbackgroundconfig.h"
#include "utils.h"
#include <QColorDialog>

XSDBackgroundConfig::XSDBackgroundConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XSDBackgroundConfig)
{
    _started = false;
    ui->setupUi(this);
    setupConnections();
    initValues();
    setupData();
    _started = true;
}

XSDBackgroundConfig::~XSDBackgroundConfig()
{
    delete ui;
}

void XSDBackgroundConfig::setupConnections()
{
    connect(ui->buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(onReset()));
    //--

    _gradientLinearIcon.addPixmap(QPixmap(QString::fromUtf8(":/gradients/images/gradient-linear.png")), QIcon::Normal, QIcon::Off);
    _gradientSolidIcon.addPixmap(QPixmap(QString::fromUtf8(":/gradients/images/gradient-solid.png")), QIcon::Normal, QIcon::Off);
    _gradientRadialIcon.addPixmap(QPixmap(QString::fromUtf8(":/gradients/images/gradient-radial.png")), QIcon::Normal, QIcon::Off);
    _gradientHorizontalIcon.addPixmap(QPixmap(QString::fromUtf8(":/gradients/images/gradient-horizontal.png")), QIcon::Normal, QIcon::Off);
    _gradientVerticalIcon.addPixmap(QPixmap(QString::fromUtf8(":/gradients/images/gradient-vertical.png")), QIcon::Normal, QIcon::Off);
    _gradientDiagonalIcon.addPixmap(QPixmap(QString::fromUtf8(":/gradients/images/gradient-diagonal.png")), QIcon::Normal, QIcon::Off);

    //--
    ui->gradientTypeCombo->addItem(_gradientSolidIcon, tr("Solid color"), XSDGraphicsBackgroundConfiguration::GradientSolid);
    ui->gradientTypeCombo->addItem(_gradientLinearIcon, tr("Linear"), XSDGraphicsBackgroundConfiguration::GradientLinear);
    ui->gradientTypeCombo->addItem(_gradientRadialIcon, tr("Radial"), XSDGraphicsBackgroundConfiguration::GradientRadial);
    //--
    ui->gradientDirectionCombo->addItem(_gradientVerticalIcon, tr("Vertical"), XSDGraphicsBackgroundConfiguration::DirectionVertical);
    ui->gradientDirectionCombo->addItem(_gradientHorizontalIcon, tr("Horizontal"), XSDGraphicsBackgroundConfiguration::DirectionHorizontal);
    ui->gradientDirectionCombo->addItem(_gradientDiagonalIcon, tr("Diagonal"), XSDGraphicsBackgroundConfiguration::DirectionDiagonal);
    //--
    ui->gradientUnitsCombo->addItem(tr("Pixels"), XSDGraphicsBackgroundConfiguration::GradientUnitsPixels);
    ui->gradientUnitsCombo->addItem(tr("Percentual"), XSDGraphicsBackgroundConfiguration::GradientUnitsPct);
    //--
    ui->repetitionAt->setMaximum(32000);
}

void XSDBackgroundConfig::initValues()
{
    //--
    _configuration.load();
}

void XSDBackgroundConfig::onReset()
{
    //--
    _started = false;
    _configuration.reset();
    setupData();
    _started = true;
    valuesChanged();
}


void XSDBackgroundConfig::setBkColorToButton(QPushButton *button, const QColor &color)
{
    QString colorString = "background-color: " + color.name();
    colorString += ";";
    button->setStyleSheet(colorString);
    button->update();
}


void XSDBackgroundConfig::setupData()
{
    //--
    Utils::selectComboValue(ui->gradientTypeCombo, _configuration.gradientType());
    //--
    Utils::selectComboValue(ui->gradientDirectionCombo, _configuration.gradientDirection());
    //--
    Utils::selectComboValue(ui->gradientUnitsCombo, _configuration.gradientUnits());
    //--
    ui->repetitionAt->setValue(_configuration.gradientHeight());
    //-
    setBkColorToButton(ui->mainColor, _configuration.mainColor());
    setBkColorToButton(ui->alternateColor, _configuration.alternateColor());
    //-
    enableUI();

}

void XSDBackgroundConfig::valuesChanged()
{
    _configuration.save();
    emit onBackgroundConfigurationChanged(&_configuration);
}

void XSDBackgroundConfig::enableUI()
{
    bool isEnabledGradient = !(_configuration.gradientType() == XSDGraphicsBackgroundConfiguration::GradientSolid);
    bool isRadial = _configuration.gradientType() == XSDGraphicsBackgroundConfiguration::GradientRadial;
    ui->alternateColor->setVisible(isEnabledGradient);
    ui->alternateColorLabel->setVisible(isEnabledGradient);
    ui->gradientDirectionCombo->setVisible(isEnabledGradient && !isRadial);
    ui->lblGradientDirectionCombo->setVisible(isEnabledGradient && !isRadial);
    ui->repetitionAt->setVisible(isEnabledGradient);
    ui->gradientUnitsCombo->setVisible(isEnabledGradient);
    ui->repeatLbl->setVisible(isEnabledGradient);
}

void XSDBackgroundConfig::on_mainColor_clicked()
{
    if(!_started) {
        return ;
    }
    QColor color = QColorDialog::getColor(_configuration.mainColor(), this, tr("Choose the main color"));
    if(color.isValid()) {
        _configuration.setMainColor(color);
        setBkColorToButton(ui->mainColor, _configuration.mainColor());
        valuesChanged();
    }
}

void XSDBackgroundConfig::on_alternateColor_clicked()
{
    if(!_started) {
        return ;
    }
    QColor color = QColorDialog::getColor(_configuration.alternateColor(), this, tr("Choose the alternate color"));
    if(color.isValid()) {
        _configuration.setAlternateColor(color);
        setBkColorToButton(ui->alternateColor, _configuration.alternateColor());
        valuesChanged();
    }
}


void XSDBackgroundConfig::on_gradientTypeCombo_currentIndexChanged(int /*index*/)
{
    if(!_started) {
        return ;
    }
    _configuration.setGradientType((XSDGraphicsBackgroundConfiguration::EGradientType)Utils::comboSelectedCodeAsInt(ui->gradientTypeCombo, _configuration.gradientType())) ;
    enableUI();
    valuesChanged();
}

void XSDBackgroundConfig::on_gradientDirectionCombo_currentIndexChanged(int /*index*/)
{
    if(!_started) {
        return ;
    }
    _configuration.setGradientDirection((XSDGraphicsBackgroundConfiguration::EGradientDirection)Utils::comboSelectedCodeAsInt(ui->gradientDirectionCombo, _configuration.gradientDirection())) ;
    valuesChanged();
}

void XSDBackgroundConfig::on_gradientUnitsCombo_currentIndexChanged(int /*index*/)
{
    if(!_started) {
        return ;
    }
    _configuration.setGradientUnits((XSDGraphicsBackgroundConfiguration::EGradientUnits)Utils::comboSelectedCodeAsInt(ui->gradientUnitsCombo, _configuration.gradientUnits())) ;
    valuesChanged();
}

void XSDBackgroundConfig::on_repetitionAt_valueChanged(int /*value*/)
{
    if(!_started) {
        return ;
    }
    _configuration.setGradientHeight(ui->repetitionAt->value());
    valuesChanged();
}
