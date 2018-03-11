/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2018 by Luca Bellonda and individual contributors       *
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

#include "guidedoperationsdialog.h"
#include "ui_guidedoperationsdialog.h"
#include "utils.h"

GuidedOperationsDialog::GuidedOperationsDialog(QXmlEditApplication *application, ApplicationData *appData, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GuidedOperationsDialog)
{
    Utils::TODO_THIS_RELEASE("fare hide window senza uscire");
    _application = application;
    _appData = appData;
    ui->setupUi(this);
    makeButtonsSameSize();
}

GuidedOperationsDialog::~GuidedOperationsDialog()
{
    delete ui;
}

void GuidedOperationsDialog::makeButtonsSameSize()
{
    int maxSizeX = 0 ;
    QList<QPushButton*> buttons = findChildren<QPushButton*>();
    foreach(QPushButton* button, buttons) {
        maxSizeX = qMax(maxSizeX, button->size().width());
    }
    maxSizeX *= 1.3;
    foreach(QPushButton* button, buttons) {
        button->setFixedSize(maxSizeX, button->height());
    }

}

void GuidedOperationsDialog::on_testNEW_clicked()
{
    Utils::TODO_THIS_RELEASE("");
    emit triggerNew();
}

void GuidedOperationsDialog::on_testQUIT_clicked()
{
    Utils::TODO_THIS_RELEASE("");
    emit triggerQuit();
}

void GuidedOperationsDialog::on_testOPEN_clicked()
{
    Utils::TODO_THIS_RELEASE("");
    emit triggerOpen();
}
