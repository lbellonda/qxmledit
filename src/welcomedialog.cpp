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

#include "welcomedialog.h"
#include "ui_welcomedialog.h"
#include "utils.h"

WelcomeDialog::WelcomeDialog(QStringList &recentFiles, QWidget *parent) :
    QDialog(parent),
    _recentModel(recentFiles),
    ui(new Ui::WelcomeDialog)
{
    _showNextTime = true ;
    _result = E_CLOSE ;
    ui->setupUi(this);
    ui->showNextTime->setChecked(true);
    ui->recentFiles->setModel(&_recentModel);
    if(recentFiles.isEmpty()) {
        ui->recentFiles->setEnabled(false);
        ui->chooseRecent->setEnabled(false);
        ui->buttonBox->setFocus();
    } else {
        ui->recentFiles->setCurrentIndex(_recentModel.index(0));
        ui->chooseRecent->setFocus();
    }
}

WelcomeDialog::~WelcomeDialog()
{
    delete ui;
}

WelcomeDialog::EResult WelcomeDialog::result()
{
    return _result ;
}

QString WelcomeDialog::fileChoosen()
{
    return _fileChoosen ;
}

bool WelcomeDialog::isShowNextTime()
{
    return _showNextTime ;
}

//--------------------------------------------------------------------------------

void WelcomeDialog::reject()
{
    _showNextTime = ui->showNextTime->isChecked();
    _result = E_CLOSE ;
    QDialog::reject();
}

void WelcomeDialog::accept()
{
    _showNextTime = ui->showNextTime->isChecked();
    QDialog::accept();
}

void WelcomeDialog::on_examineSchema_clicked()
{
    _result = E_OPENSCHEMA ;
    accept();
}

void WelcomeDialog::on_splitFile_clicked()
{
    _result = E_SCANFILE ;
    accept();
}

void WelcomeDialog::on_chooseRecent_clicked()
{
    _fileChoosen = "" ;
    QModelIndex currentIndex = ui->recentFiles->currentIndex() ;
    if(currentIndex.isValid()) {
        _fileChoosen = _recentModel.data(currentIndex, Qt::EditRole).toString();
    }
    if(_fileChoosen.isEmpty()) {
        Utils::error(this, tr("Select a item from the recent files list, please."));
        return ;
    }
    _result = E_RECENTFILECHOOSE ;
    accept();
}

void WelcomeDialog::on_recentFiles_doubleClicked(const QModelIndex & index)
{
    if(index.isValid()) {
        on_chooseRecent_clicked();
    }
}

void WelcomeDialog::on_cmdManageSessions_clicked()
{
    _result = E_MANAGESESSIONS ;
    accept();
}

void WelcomeDialog::on_cmdViewData_clicked()
{
    _result = E_VIEWDATA ;
    accept();
}

void WelcomeDialog::on_compareFiles_clicked()
{
    _result = E_COMPARE ;
    accept();
}

