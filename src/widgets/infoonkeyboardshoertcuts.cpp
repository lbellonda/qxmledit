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

#include "infoonkeyboardshoertcuts.h"
#include "ui_infoonkeyboardshoertcuts.h"
#include "modules/utils/animationutility.h"
#include "utils.h"

#define LINK_DISMISS    "self:dismiss"
#define LINK_INFO       "self:showPanel"

InfoOnKeyboardShortcuts::InfoOnKeyboardShortcuts(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoOnKeyboardShortcuts)
{
    ui->setupUi(this);
    //AnimationUtility::setupSizeForLabelIcon(ui->info, &_animation);
    _backColor = QWidget::palette().color(QWidget::backgroundRole());
    AnimationUtility::setupBackgroundColor(this, &_animation);
    _animation.start();
}

InfoOnKeyboardShortcuts::~InfoOnKeyboardShortcuts()
{
    _animation.stop();
    delete ui;
}

QColor InfoOnKeyboardShortcuts::backColor()
{
    return palette().color(backgroundRole());
}

void InfoOnKeyboardShortcuts::setBackColor(const QColor value )
{
    Utils::TODO_THIS_RELEASE("fare");
    ui->infoText->setStyleSheet(QString("background-color: rgb(%1, %2, %3);").arg(value.red()).arg(value.green()).arg(value.blue()));
    //palette().setColor(backgroundRole(), value);
}

void InfoOnKeyboardShortcuts::on_infoText_linkActivated(const QString &link)
{
    checkLink(link);
}

void InfoOnKeyboardShortcuts::on_infoText2_linkActivated(const QString &link)
{
    checkLink(link);
}

void InfoOnKeyboardShortcuts::on_infoText3_linkActivated(const QString &link)
{
    checkLink(link);
}

void InfoOnKeyboardShortcuts::checkLink(const QString &link)
{
    if(LINK_DISMISS == link) {
        emit requestDismiss();
    } else if(LINK_INFO == link) {
        emit requestOpenShortcutsPanel();
    }
}
