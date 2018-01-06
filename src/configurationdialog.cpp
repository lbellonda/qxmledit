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

#include "configurationdialog.h"
#include "ui_configurationdialog.h"
#include "utils.h"

void ConfigurationDialog::doOptions(QWidget * const parentWindow, ApplicationData* data)
{
    ConfigurationDialog options(data, parentWindow);
    options.setModal(true);
    options.exec();
}

ConfigurationDialog::ConfigurationDialog(ApplicationData* data, QWidget *parent) :
    QDialog(parent),
    _data(data),
    _configureStyle(NULL),
    _configureValidation(NULL),
    _configureEditModes(NULL),
    ui(new Ui::ConfigurationDialog)
{
    ui->setupUi(this);
    _generalOptions = new ConfigurationGeneralOptions(this);
    _configureStyle = new ConfigureStyle(this);
    _configureValidation = new ConfigValidation(this);
    _configureSessions = new ConfigureSessionsWidget(this);
    _configureEditModes = new ConfigureEditModesDialog(this);
    _configureXmlManagement = new ConfigureXMLManagementDialog(this);
    _configureEditorColors = new ConfigureEditorColors(this);

    generalIcon.addPixmap(QPixmap(QString::fromUtf8(":/configuration/general")), QIcon::Normal, QIcon::Off);
    styleIcon.addPixmap(QPixmap(QString::fromUtf8(":/configuration/style")), QIcon::Normal, QIcon::Off);
    editorColorsIcon.addPixmap(QPixmap(QString::fromUtf8(":/configuration/style")), QIcon::Normal, QIcon::Off);
    validationIcon.addPixmap(QPixmap(QString::fromUtf8(":/configuration/validation")), QIcon::Normal, QIcon::Off);
    sessionsIcon.addPixmap(QPixmap(QString::fromUtf8(":/configuration/sessions")), QIcon::Normal, QIcon::Off);
    editModesIcon.addPixmap(QPixmap(QString::fromUtf8(":/configuration/editModes")), QIcon::Normal, QIcon::Off);
    xmlManagementIcon.addPixmap(QPixmap(QString::fromUtf8(":/configuration/xmlManagement")), QIcon::Normal, QIcon::Off);

    setTab(TAB_GENERAL, _generalOptions, generalIcon, tr("General"), tr("General Options"));
    setTab(TAB_STYLE, _configureStyle, styleIcon, tr("Style"), tr("Configure style"));
    setTab(TAB_EDITORCOLORS, _configureEditorColors, editorColorsIcon, tr("Editor Colors"), tr("Configure editor colors"));
    setTab(TAB_VALIDATION, _configureValidation, validationIcon, tr("Validation"), tr("Configure schema and validation parameters"));
    setTab(TAB_SESSIONS, _configureSessions, sessionsIcon, tr("Sessions"), tr("Configure sessions"));
    setTab(TAB_EDITMODES, _configureEditModes, editModesIcon, tr("Edit Modes"), tr("Configure edit modes."));
    setTab(TAB_XMLMETADATA, _configureXmlManagement, xmlManagementIcon, tr("XML"), tr("Configure XML management."));

    init(data);
}

ConfigurationDialog::~ConfigurationDialog()
{
    delete ui;
}

void ConfigurationDialog::setTab(const ETabs tab, QWidget *widget, QIcon &icon, const QString &title, const QString &tooltip)
{
    ui->tabWidget->insertTab(tab, widget, icon, title);
    ui->tabWidget->setTabToolTip(tab, tooltip);
}

void ConfigurationDialog::init(ApplicationData* data)
{
    _generalOptions->init(data);
    _configureStyle->init(data);
    _configureEditorColors->init(data);
    _configureValidation->init(data);
    _configureSessions->init(data);
    _configureEditModes->init(data);
    _configureXmlManagement->init(data);
}

void ConfigurationDialog::on_buttonBox_accepted()
{
    accept();
}

void ConfigurationDialog::accept()
{
    _generalOptions->saveIfChanged();
    _configureStyle->saveIfChanged();
    _configureEditorColors->saveIfChanged();
    _configureValidation->saveIfChanged();
    _configureSessions->saveIfChanged();
    _configureEditModes->saveIfChanged();
    _configureXmlManagement->saveIfChanged();
    QDialog::accept();
}
