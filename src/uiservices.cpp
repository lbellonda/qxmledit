/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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

#include "uiservices.h"
#include "base64dialog.h"
#include "charencodingdialog.h"

UIServices::UIServices()
{
}

UIServices::~UIServices()
{
}

void UIServices::doBase64Dialog()
{
    Base64Dialog::showDialog();
}

void UIServices::doEncodingDialog(QXmlEditData *theData, const bool doModal)
{
    if(doModal) {
        CharEncodingDialog dialog(theData);
        dialog.exec();
    } else {
        //autodelete on exit
        CharEncodingDialog *newDialog = new CharEncodingDialog(theData);
        newDialog->setAutoDelete();
        newDialog->setModal(false);
        newDialog->show();
    }
}

