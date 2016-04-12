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

#include "balsamiqbasedialog.h"
#include <QFileDialog>
#include "utils.h"
#include "qxmleditconfig.h"
#include "balsamiqwork.h"
#include "ui_balsamiqui.h"
#include <QUrl>
#include <QDesktopServices>


BalsamiqBaseDialog::BalsamiqBaseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BalsamiqUI)
{
    ui->setupUi(this);
    loadSettings();
    checkConditions();
}

BalsamiqBaseDialog::~BalsamiqBaseDialog()
{
    delete ui;
}

void BalsamiqBaseDialog::loadSettings()
{
    ui->overwriteFiles->setChecked(Config::getBool(Config::KEY_BALSAMIQ_OVERWRITEFILES, false));
    ui->outputDir->setText(Config::getString(Config::KEY_BALSAMIQ_OUTPUTDIR, ""));
    _inputDir = Config::getString(Config::KEY_BALSAMIQ_INPUTDIR, "");
}

void BalsamiqBaseDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch(e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void BalsamiqBaseDialog::on_cmdOpenDestinationDir_clicked()
{
    if(ui->outputDir->text().isEmpty()) {
        Utils::error(this, tr("The directory name is empty."));
        return ;
    }
    if(!QDesktopServices::openUrl(QUrl::fromLocalFile(ui->outputDir->text()))) {
        Utils::error(this, tr("An error occurred opening location."));
    }
}

void BalsamiqBaseDialog::on_buttonBox_clicked(QAbstractButton * button)
{
    bool isDisabled = false;
    try {
        if((NULL != button) && (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole)) {
            this->setEnabled(false);
            isDisabled = true;
            BalsamiqWork work;
            work.exec(this, ui->overwriteFiles->isChecked(), _files, ui->outputDir->text());
        }
    } catch(...) {
        Utils::error(tr("Error performing operation"));
    }
    if(isDisabled) {
        this->setEnabled(true);
    }
}

void BalsamiqBaseDialog::on_cmdOutputDir_clicked()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Choose the directory"),
                      ui->outputDir->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!dirPath.isEmpty()) {
        ui->outputDir->setText(dirPath);
        Config::saveString(Config::KEY_BALSAMIQ_OUTPUTDIR, dirPath);
    }
    checkConditions();
}

void BalsamiqBaseDialog::on_overwriteFiles_clicked()
{
    Config::saveBool(Config::KEY_BALSAMIQ_OVERWRITEFILES, ui->overwriteFiles->isChecked());
}

void BalsamiqBaseDialog::on_cmdChooseFiles_clicked()
{
    QStringList filesPath = QFileDialog::getOpenFileNames(this, tr("Source Files"),
                            _inputDir, tr("BMML files (*.bmml);;All files (*)"));
    if(!filesPath.empty()) {
        _files = filesPath;
        ui->files->clear();
        bool isFirst = true ;
        foreach(QString file, _files) {
            ui->files->addItem(new QListWidgetItem(file, ui->files));
            if(isFirst) {
                isFirst = false ;
                // salvare la directory di uscita
                QFileInfo fileInfo(file);
                _inputDir = fileInfo.path();
                Config::saveString(Config::KEY_BALSAMIQ_INPUTDIR, _inputDir);
            }
        }
    }
    checkConditions();
}

void BalsamiqBaseDialog::on_outputDir_textEdited(const QString & /*text*/)
{
    checkConditions();
}

void BalsamiqBaseDialog::checkConditions()
{
    bool isEnabled = false;
    if((_files.size() > 0) && !ui->outputDir->text().isEmpty()) {
        isEnabled = true ;
    }
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(isEnabled);
}
