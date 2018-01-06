/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013-2018 by Luca Bellonda and individual contributors  *
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


#include "XsltElementDialogprivatetest.h"

#include "ui_xsltelementdialog.h"
#include "qxmleditconfig.h"


const QString XsltElementDialog::PrivateTest::KEY_CHECKBOX("/XsltElementDialog/checkParameters");
const QString XsltElementDialog::PrivateTest::KEY_GO("/XsltElementDialog/go");
//---------------------------------------------

bool XsltElementDialog::PrivateTest::isCheckParameters()
{
    return Config::getBool(Config::KEY_TEST+KEY_CHECKBOX,false);
}

void XsltElementDialog::PrivateTest::setCheckParameters(const bool value)
{
    Config::saveBool(Config::KEY_TEST+KEY_CHECKBOX,value);
}

bool XsltElementDialog::PrivateTest::isGo()
{
    return Config::getBool(Config::KEY_TEST+KEY_GO,false);
}

void XsltElementDialog::PrivateTest::setGo(const bool value)
{
    Config::saveBool(Config::KEY_TEST+KEY_GO,value);
}

//---------------------------------------------

XsltElementDialog::PrivateTest::PrivateTest(XsltElementDialog *dlg)
{
    _dlg = dlg ;
}

XsltElementDialog::PrivateTest::~PrivateTest()
{
}

void XsltElementDialog::PrivateTest::testStart()
{
    if( isCheckParameters() ) {
        _dlg->ui->cmdInsParam->setChecked(true);
        _dlg->accept();
    } if( isGo() ) {
        _dlg->accept();
    }
}
