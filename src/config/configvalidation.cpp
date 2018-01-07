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
#include "configvalidation.h"
#include "qxmleditconfig.h"
#include "ui_configvalidation.h"
#include "utils.h"

ConfigValidation::ConfigValidation(QWidget *parent) :
    QWidget(parent),
    _data(NULL),
    ui(new Ui::ConfigValidation)
{
    ui->setupUi(this);
}

ConfigValidation::~ConfigValidation()
{
    delete ui;
}

void ConfigValidation::init(ApplicationData* data)
{
    _data = data ;
    ui->chbAutomaticValidationLoading->setChecked(_data->isAutovalidationOn());
    ui->chkEnableDiskCache->setChecked(_data->isXsdCacheEnabled());
    ui->chkDisplayHorizontal->setChecked(_data->isXsdDisplayHoriz());
    ui->chkDisplayHorizontal->setEnabled(false);
    ui->embedFontsPDFReport->setChecked(Config::getBool(Config::KEY_XSD_REPORT_PDF_EMBEDFONTS, true));
    ui->embedImageInHTML->setChecked(Config::getBool(Config::KEY_XSD_REPORT_EMBEDIMAGES, true));
    ui->useGraphVizReportHTML->setChecked(Config::getBool(Config::KEY_XSD_REPORT_USEGRAPHVIZ, false));
    ui->overrideGraphVizPathReport->setChecked(Config::getBool(Config::KEY_XSD_REPORT_OVERRIDEGRAPHVIZ, false));
    ui->dotVizPathReportHTML->setText(Config::getString(Config::KEY_XSD_REPORT_PATHGRAPHVIZ, ""));
    enableButtons();
}

void ConfigValidation::save()
{
    _data->setAutovalidationOn(ui->chbAutomaticValidationLoading->isChecked());
    _data->setXsdCacheEnabled(ui->chkEnableDiskCache->isChecked());
    Config::saveBool(Config::KEY_XSD_REPORT_PDF_EMBEDFONTS, ui->embedFontsPDFReport->isChecked());
    Config::saveBool(Config::KEY_XSD_REPORT_PDF_EMBEDFONTS, ui->embedFontsPDFReport->isChecked());
    Config::saveBool(Config::KEY_XSD_REPORT_EMBEDIMAGES, ui->embedImageInHTML->isChecked());
    Config::saveBool(Config::KEY_XSD_REPORT_USEGRAPHVIZ, ui->useGraphVizReportHTML->isChecked());
    Config::saveBool(Config::KEY_XSD_REPORT_OVERRIDEGRAPHVIZ, ui->overrideGraphVizPathReport->isChecked());
    Config::saveString(Config::KEY_XSD_REPORT_PATHGRAPHVIZ, ui->dotVizPathReportHTML->text());
}

void ConfigValidation::saveIfChanged()
{
    save();
}

void ConfigValidation::on_browseDotVizPath_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
                           this, tr("Choose Graphviz path"),
                           QXmlEditData::sysFilePathForOperation(ui->dotVizPathReportHTML->text()),
                           tr("All files (*)")
                       );
    if(!filePath.isEmpty()) {
        ui->dotVizPathReportHTML->setText(filePath);
    }
}

void ConfigValidation::enableButtons()
{
    ui->dotVizPathReportHTML->setEnabled(ui->overrideGraphVizPathReport->isChecked());
    ui->browseDotVizPath->setEnabled(ui->overrideGraphVizPathReport->isChecked());
}

void ConfigValidation::on_overrideGraphVizPathReport_clicked()
{
    enableButtons();
}
