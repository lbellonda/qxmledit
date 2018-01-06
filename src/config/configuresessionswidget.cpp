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

#include "configuresessionswidget.h"
#include "ui_configuresessionswidget.h"
#include "utils.h"

ConfigureSessionsWidget::ConfigureSessionsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigureSessionsWidget)
{
    ui->setupUi(this);
    QDate defaultDate = QDate::currentDate().addMonths(-1);
    defaultDate = defaultDate.addYears(-1);
    ui->dateLimit->setDate(defaultDate);
}

ConfigureSessionsWidget::~ConfigureSessionsWidget()
{
    delete ui;
}

void ConfigureSessionsWidget::init(ApplicationData* data)
{
    _data = data ;
    ui->enableSessions->setChecked(_data->areSessionsEnabled());
    ui->delOlderSixMonth->setChecked(true);
    ui->dateLimit->setEnabled(false);
}

void ConfigureSessionsWidget::on_delOlderOneMonth_toggled()
{
    ui->dateLimit->setEnabled(false);
}

void ConfigureSessionsWidget::on_delOlderSixMonth_toggled()
{
    ui->dateLimit->setEnabled(false);
}

void ConfigureSessionsWidget::on_deleteSpecifiedDate_toggled()
{
    ui->dateLimit->setEnabled(true);
}

void ConfigureSessionsWidget::saveIfChanged()
{
    _data->enableSessions(ui->enableSessions->isChecked());
}

void ConfigureSessionsWidget::on_deleteAllData_clicked()
{
    if(Utils::askYN(tr("Really delete all the data?\nThis operation cannot be undone."))) {
        if(!Utils::askYN(tr("Do you want to abort this operation?"))) {
            SessionDataInterface * dataAccess = _data->sessionManager()->dataAccess();
            if(NULL == dataAccess) {
                Utils::error(this, tr("Error connecting with storage."));
            }
            SessionOperationStatus context;
            setEnabled(false);
            bool isOk  = dataAccess->deleteAllSessionData(context);
            setEnabled(true);
            if(!isOk) {
                Utils::error(this, tr("Error deleting data."));
            } else {
                Utils::message(parentWidget(), tr("Operation finished."));
            }
        }
    }
}

void ConfigureSessionsWidget::on_deleteData_clicked()
{
    if(Utils::askYN(tr("Really delete all the data?\nThis operation cannot be undone."))) {
        if(!Utils::askYN(tr("Do you want to abort this operation?"))) {
            SessionDataInterface *dataAccess = _data->sessionManager()->dataAccess();
            if(NULL == dataAccess) {
                Utils::error(this, tr("Error connecting with storage."));
            }
            bool isValidChoice = true ;
            SessionDataInterface::EDateInterval dateInterval;
            QDateTime dt(ui->dateLimit->date());
            if(ui->delOlderOneMonth->isChecked()) {
                dateInterval = SessionDataInterface::DATE_INTERVAL_1_MONTH ;
            } else if(ui->delOlderSixMonth->isChecked()) {
                dateInterval = SessionDataInterface::DATE_INTERVAL_6_MONTHS ;
            } else if(ui->deleteSpecifiedDate->isChecked()) {
                dateInterval = SessionDataInterface::DATE_INTERVAL_LITERAL ;
            } else {
                isValidChoice = false ;
            }
            if(isValidChoice) {
                SessionOperationStatus context;
                setEnabled(false);
                bool isOk = dataAccess->deleteSessionDataWithDateFilter(context, dateInterval, dt, ui->deleteFilesInfo->isChecked());
                setEnabled(true);
                if(!isOk) {
                    Utils::error(parentWidget(), tr("Error deleting data."));
                } else {
                    Utils::message(parentWidget(), tr("Operation finished."));
                }
            } else {
                Utils::error(parentWidget(), tr("Select a valid option"));
            }
        }
    }
}

