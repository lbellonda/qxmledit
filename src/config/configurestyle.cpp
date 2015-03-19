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

#include "configurestyle.h"
#include "ui_configurestyle.h"
#include "utils.h"
#include "config.h"
#include <QDir>
#include <QUrl>
#include <QDesktopServices>

ConfigureStyle::ConfigureStyle(QWidget *parent) :
    QWidget(parent),
    _data(NULL),
    ui(new Ui::ConfigureStyle)
{
    ui->setupUi(this);
    _group.addButton(ui->pgmLocation);
    _group.addButton(ui->pgmThisDirectory);
    _group.addButton(ui->pgmDefault);
    //connect( &_group, SIGNAL(buttonClicked ( QAbstractButton * button )), this, SLOT(buttonClicked ( QAbstractButton * button )) );
}

ConfigureStyle::~ConfigureStyle()
{
    delete ui;
}

void ConfigureStyle::changeEvent(QEvent *e)
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


void ConfigureStyle::init(ApplicationData* data)
{
    _data = data ;
    QString stylesDirectory = _data->getStylesDirSpecific();
    ui->aDirectory->setText(stylesDirectory);
    stylesDirectory = _data->getStylesDirStandard();
    ui->stdDirValue->setText(stylesDirectory);
    stylesDirectory = _data->getStylesDirApplication();
    ui->pgmDirValue->setText(stylesDirectory);
    EStylesDir type = _data->getStylesDirLocation();
    switch(type) {
    case ESTYLE_DIR_USEAPPL:
        ui->pgmLocation->setChecked(true);
        break;
    case ESTYLE_DIR_USEDIR:
        ui->pgmThisDirectory->setChecked(true);
        break;
    default:
        ui->pgmDefault->setChecked(true);
        break;
    }
}


void ConfigureStyle::save()
{
    if(ui->pgmLocation->isChecked()) {
        _data->setStylesDirLocation(ESTYLE_DIR_USEAPPL);
    } else if(ui->pgmThisDirectory->isChecked()) {
        QString dir = ui->aDirectory->text();
        if(dir.isEmpty()) {
            Utils::error(tr("Data directory not specified"));
            return ;
        }
        _data->setStylesDirLocation(ESTYLE_DIR_USEDIR);
        Config::saveString(Config::KEY_STYLE_DATADIR, dir);
    } else {
        _data->setStylesDirLocation(ESTYLE_DIR_USESTD);
    }
}

void ConfigureStyle::saveIfChanged()
{
    save();
}

void ConfigureStyle::on_openLocation_clicked()
{
    bool isOk = false;
    if(ui->pgmLocation->isChecked()) {
        QDir dir(_data->getStylesDirApplication());
        if(!dir.exists()) {
            dir.mkpath(_data->getStylesDirApplication());
        }
        isOk = QDesktopServices::openUrl(QUrl::fromLocalFile(_data->getStylesDirApplication()));
    } else if(ui->pgmThisDirectory->isChecked()) {
        if(ui->aDirectory->text().isEmpty()) {
            Utils::error(this, tr("The directory name is empty."));
            return ;
        }
        isOk = QDesktopServices::openUrl(QUrl::fromLocalFile(ui->aDirectory->text()));
    } else {
        QDir dir(_data->getStylesDirStandard());
        if(!dir.exists()) {
            dir.mkpath(_data->getStylesDirStandard());
        }
        isOk = QDesktopServices::openUrl(QUrl::fromLocalFile(_data->getStylesDirStandard()));
    }
    if(!isOk) {
        Utils::error(this, tr("An error occurred while opening location."));
    }
}

void ConfigureStyle::on_browseFolderCmd_clicked()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Choose the directory"),
                      ui->aDirectory->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!dirPath.isEmpty()) {
        ui->aDirectory->setText(dirPath);
    }
}
