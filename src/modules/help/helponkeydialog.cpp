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

#include "helponkeydialog.h"
#include "ui_helponkeydialog.h"
#include "utils.h"

HelpOnKeyDialog::HelpOnKeyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpOnKeyDialog)
{
    ui->setupUi(this);
    Utils::TODO_THIS_RELEASE("s");
    Utils::TODO_THIS_RELEASE("remove?");
}

HelpOnKeyDialog::~HelpOnKeyDialog()
{
    delete ui;
}
/*
{
    const QString lightTheme = "border: 1px solid #000040; border-radius:10px; padding:0px;margin:0px;\nbackground-color: qlineargradient(spread:pad, x1:0.469613, y1:1, x2:0.48563, y2:0.477, stop:0 rgba(228, 228, 228, 255), stop:1 rgba(255, 255, 255, 255));";
    const QString darkTheme = "border: 1px solid #000040; border-radius:10px; padding:0px;margin:0px;\nbackground-color: qlineargradient(spread:pad, x1:0.469613, y1:1, x2:0.48563, y2:0.477, stop:0 rgba(64,64, 64, 255), stop:1 rgba(128, 128, 128, 255));";
    const QString colorTextLightTheme = "#5d5d5d";
    const QString colorTextDarkTheme = "#5d5d5d";
    const QString templateData = "<html><head/><body><p><span style='font-weight:600;'>$(FKEY)</span><br/><span style='color:$(COLORTEXT);'>$(TEXT)</span></p></body></html>";
}
*/
