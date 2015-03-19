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


#include "xsdcomparetools.h"
#include "xsdcompare.h"
#include "config.h"
#include "ui_xsdcomparetools.h"
#include "xsdeditor/xsdwindow.h"
#include "utils.h"

XSDCompareTools::XSDCompareTools(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::XSDCompareTools)
{
    ui->setupUi(this);
    bool state = isCompareAnnotationsPersistentOption();
    ui->compareAnnotationOption->setChecked(state ? Qt::Checked : Qt::Unchecked);
    Utils::setBkColorAndBorderToWidget(ui->colorAdded, XSDItem::colorAddStart);
    Utils::setBkColorAndBorderToWidget(ui->colorModified, XSDItem::colorModStart);
    Utils::setBkColorAndBorderToWidget(ui->colorDeleted, XSDItem::colorDelStart);
    Utils::setBkColorAndBorderToWidget(ui->colorEquals, XSDItem::colorEqualsStart);
}

XSDCompareTools::~XSDCompareTools()
{
    delete ui;
}


bool XSDCompareTools::isCompareAnnotations()
{
    return ui->compareAnnotationOption->isChecked();
}


void XSDCompareTools::on_compareAnnotationOption_stateChanged(int state)
{
    bool value  = (state == Qt::Checked);
    setCompareAnnotationsPersistentOption(value) ;
    emit compareAnnotationChanged(value);
}

void XSDCompareTools::on_compareSwapReferenceAction_triggered()
{
    emit swapReference();
}

