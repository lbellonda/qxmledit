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
#include "configurationgeneraloptions.h"
#include "ui_configurationgeneraloptions.h"
#include "utils.h"
#include "config.h"
#include "element.h"

ConfigurationGeneralOptions::ConfigurationGeneralOptions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigurationGeneralOptions)
{
    _started = false;
    ui->setupUi(this);

    _elementFontSize = 0 ;
    _elementFontItalic = false ;
    _elementFontBold = false ;
}

ConfigurationGeneralOptions::~ConfigurationGeneralOptions()
{
    delete ui;
}

void ConfigurationGeneralOptions::init(ApplicationData* data)
{
    _data = data ;
    ui->fixedSizeAttrComboFont->setFontFilters(QFontComboBox::MonospacedFonts);
    ui->showWelcomePage->setChecked(data->isWelcomeDialogEnabled());
    ui->autoscroll->setChecked(data->isAutoscroll());
    ui->enableExperiments->setChecked(data->areExperimentalFeaturesEnabled());

    int fontIndex;
    QString font = Config::getString(Config::KEY_ELEMENT_FONTFIXEXDWIDTHFAMILY, DEFAULT_FIXEDWIDTHFONT);
    fontIndex = ui->fixedSizeAttrComboFont->findText(font);
    if(fontIndex >= 0) {
        ui->fixedSizeAttrComboFont->setCurrentIndex(fontIndex);
    }
    ui->enableFWAFont->setCheckState(Config::getBool(Config::KEY_ELEMENT_FONTFIXEXDWIDTHENABLED, false) ? Qt::Checked : Qt::Unchecked);
    ui->fixedSizeAttributeFontSize->setValue(Config::getInt(Config::KEY_ELEMENT_FONTFIXEXDWIDTHSIZE, 0));

    ui->enableEFontSize->setChecked(Config::getBool(Config::KEY_ELEMENT_ELEMENTFONTENABLED, false));
    _elementFontName = Config::getString(Config::KEY_ELEMENT_ELEMENTFONTFAMILY, "");
    _elementFontSize = Config::getInt(Config::KEY_ELEMENT_ELEMENTFONTSIZE, 0);
    _elementFontItalic = Config::getBool(Config::KEY_ELEMENT_ELEMENTFONTITALIC, false);
    _elementFontBold = Config::getBool(Config::KEY_ELEMENT_ELEMENTFONTBOLD, false);

    setElementFontInfo();
    checkEnablingStatus();

    ui->enableLogs->setChecked(Config::getBool(Config::KEY_LOGS_ENABLED, false));
    ui->logLevelCombo->addItem(tr("debug"), FrwLogger::DEBUG);
    ui->logLevelCombo->addItem(tr("info"), FrwLogger::INFO);
    ui->logLevelCombo->addItem(tr("warning"), FrwLogger::WARNING);
    ui->logLevelCombo->addItem(tr("error"), FrwLogger::ERROR);
    Utils::selectComboValue(ui->logLevelCombo, Config::getInt(Config::KEY_LOGS_LEVEL, FrwLogger::INFO));
    ui->expandTreeAfterLoad->setChecked(Config::getBool(Config::KEY_MAIN_EXPANDONLOAD, true));

    ui->chkDefBold->setChecked(Config::getBool(Config::KEY_VIEW_STYLE_DEFAULT_BOLD, QXmlEditData::DefaultStyleFontBold));
    ui->cbOpenFilesInNewWindow->setChecked(Config::getBool(Config::KEY_GENERAL_OPEN_NEWWINDOW, QXmlEditData::DefaultOpenInNewWindow));
    ui->cbShowImagesInTextTooltip->setChecked(QXmlEditData::isShowImagesInTooltip());
    ui->cbSingleInstance->setChecked(Config::getBool(Config::KEY_GENERAL_SINGLE_INSTANCE, true));
    _started = true ;
}

void ConfigurationGeneralOptions::checkEnablingStatus()
{
    bool isEFChecked = ui->enableEFontSize->isChecked();
    ui->changeElementsFont->setEnabled(isEFChecked);
    ui->elementFontSummary->setEnabled(isEFChecked);
    bool isFWAttrFontEnabled = ui->enableFWAFont->isChecked();
    ui->fixedSizeAttrComboFont->setEnabled(isFWAttrFontEnabled);
    ui->fixedSizeAttributeFontSize->setEnabled(isFWAttrFontEnabled);
}

void ConfigurationGeneralOptions::save()
{
    _data->enableWelcomeDialog(ui->showWelcomePage->isChecked());
    _data->enableAutoscroll(ui->autoscroll->isChecked());
    Config::saveBool(Config::KEY_MAIN_EXPANDONLOAD, ui->expandTreeAfterLoad->isChecked()) ;
}

void ConfigurationGeneralOptions::saveIfChanged()
{
    save();
}

void ConfigurationGeneralOptions::on_logLevelCombo_currentIndexChanged(int index)
{
    if(!_started) {
        return ;
    }
    int newLevel = ui->logLevelCombo->itemData(index).toInt();
    Config::saveInt(Config::KEY_LOGS_LEVEL, newLevel);
    if(NULL != _data->logger()) {
        _data->logger()->setLevel((FrwLogger::ELogLevel)newLevel);
    }
}

void ConfigurationGeneralOptions::on_enableLogs_stateChanged(int state)
{
    if(!_started) {
        return ;
    }
    bool isEnabled = (state == Qt::Checked) ? true : false;
    Config::saveBool(Config::KEY_LOGS_ENABLED, isEnabled);
    if(NULL != _data->logger()) {
        _data->logger()->setEnabled(isEnabled);
    }

}

void ConfigurationGeneralOptions::on_fixedSizeAttrComboFont_currentFontChanged(const QFont & font)
{
    if(!_started) {
        return ;
    }
    Config::saveString(Config::KEY_ELEMENT_FONTFIXEXDWIDTHFAMILY, font.family());
}

void ConfigurationGeneralOptions::on_enableFWAFont_stateChanged(int state)
{
    if(!_started) {
        return ;
    }
    bool isEnabled = (state == Qt::Checked) ? true : false;
    Config::saveBool(Config::KEY_ELEMENT_FONTFIXEXDWIDTHENABLED, isEnabled);
    checkEnablingStatus();
}

void ConfigurationGeneralOptions::on_fixedSizeAttributeFontSize_valueChanged(int value)
{
    if(!_started) {
        return ;
    }
    Config::saveInt(Config::KEY_ELEMENT_FONTFIXEXDWIDTHSIZE, value);
}

void ConfigurationGeneralOptions::on_cbShowImagesInTextTooltip_stateChanged(int state)
{
    if(!_started) {
        return ;
    }
    bool isEnabled = (state == Qt::Checked);
    Config::saveBool(Config::KEY_ELEMENT_TEXT_TOOLTIP_IMAGE, isEnabled);
}


void ConfigurationGeneralOptions::on_enableEFontSize_stateChanged(int state)
{
    if(!_started) {
        return ;
    }
    bool isEnabled = (state == Qt::Checked);
    Config::saveBool(Config::KEY_ELEMENT_ELEMENTFONTENABLED, isEnabled);
    checkEnablingStatus();
}

void ConfigurationGeneralOptions::on_chkDefBold_stateChanged(int /*state*/)
{
    if(!_started) {
        return ;
    }
    Config::saveBool(Config::KEY_VIEW_STYLE_DEFAULT_BOLD, ui->chkDefBold->isChecked());
}

void ConfigurationGeneralOptions::on_cbSingleInstance_stateChanged(int /*state*/)
{
    if(!_started) {
        return ;
    }
    Config::saveBool(Config::KEY_GENERAL_SINGLE_INSTANCE, ui->cbSingleInstance->isChecked());
}

void ConfigurationGeneralOptions::on_changeElementsFont_clicked()
{
    if(!_started) {
        return ;
    }

    bool isOk;
    QFont selectedFont = QFontDialog::getFont(&isOk, QFont(_elementFontName, _elementFontSize, _elementFontBold ? QFont::Bold : -1, _elementFontItalic), this, tr("Select the font for elements text display")) ;
    if(isOk) {
        _elementFontName = selectedFont.family();
        _elementFontSize = selectedFont.pointSize();
        _elementFontItalic = selectedFont.italic();
        _elementFontBold = selectedFont.bold();
        // save
        Config::saveString(Config::KEY_ELEMENT_ELEMENTFONTFAMILY, _elementFontName);
        Config::saveInt(Config::KEY_ELEMENT_ELEMENTFONTSIZE, _elementFontSize);
        Config::saveBool(Config::KEY_ELEMENT_ELEMENTFONTITALIC, _elementFontItalic);
        Config::saveBool(Config::KEY_ELEMENT_ELEMENTFONTBOLD, _elementFontBold);
        Config::saveBool(Config::KEY_VIEW_STYLE_DEFAULT_BOLD, ui->chkDefBold->isChecked());
        Config::saveBool(Config::KEY_GENERAL_SINGLE_INSTANCE, ui->cbSingleInstance->isChecked());

        setElementFontInfo();
    }
}

void ConfigurationGeneralOptions::setElementFontInfo()
{
    if(!_started) {
        return ;
    }
    QString fontName ;
    if(_elementFontName .isEmpty()) {
        fontName  = tr("<Not set>");
    } else {
        fontName = QString(tr("%1%2%3, %4")).arg(_elementFontName).arg((_elementFontBold ? tr(" bold") : "")).arg((_elementFontItalic ? tr(" italic") : "")).arg(_elementFontSize);
    }
    ui->elementFontSummary->setText(fontName);
}

void ConfigurationGeneralOptions::on_enableExperiments_stateChanged(int state)
{
    if(!_started) {
        return ;
    }
    bool isEnabled = (state == Qt::Checked);
    _data->setExperimentalFeaturesEnabled(isEnabled);
}

void ConfigurationGeneralOptions::on_cbOpenFilesInNewWindow_stateChanged(int /*state*/)
{
    if(!_started) {
        return ;
    }
    Config::saveBool(Config::KEY_GENERAL_OPEN_NEWWINDOW, ui->cbOpenFilesInNewWindow->isChecked());
}
