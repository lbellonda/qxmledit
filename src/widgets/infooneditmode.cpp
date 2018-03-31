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

#include "infooneditmode.h"
#include "ui_infooneditmode.h"
#include "modules/utils/animationutility.h"
#include "utils.h"

#define LINK_DISMISS    "self:dismiss"
#define LINK_INFO       "self:showPanel"

InfoOnEditMode::InfoOnEditMode(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoOnEditMode)
{
    ui->setupUi(this);
    AnimationUtility::setupSizeForLabelIcon(ui->info, &_animation);
    _animation.start();
}

InfoOnEditMode::~InfoOnEditMode()
{
    _animation.stop();
    delete ui;
}

void InfoOnEditMode::on_infoText_linkActivated(const QString &link)
{
    checkLink(link);
}

void InfoOnEditMode::on_infoText2_linkActivated(const QString &link)
{
    checkLink(link);
}

void InfoOnEditMode::on_infoText3_linkActivated(const QString &link)
{
    checkLink(link);
}

void InfoOnEditMode::checkLink(const QString &link)
{
    if(LINK_DISMISS == link) {
        emit requestDismiss();
    } else if(LINK_INFO == link) {
        emit requestOpenPanel();
    }
}
