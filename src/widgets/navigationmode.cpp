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

#include "navigationmode.h"
#include "ui_navigationmode.h"

NavigationMode::NavigationMode(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NavigationMode)
{
    ui->setupUi(this);
    setExploreMode(qxmledit::NORMAL);
}

NavigationMode::~NavigationMode()
{
    delete ui;
}

void NavigationMode::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch(e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

qxmledit::EDisplayMode NavigationMode::exploreMode()
{
    return _mode ;
}

void NavigationMode::setExploreMode(const qxmledit::EDisplayMode value)
{
    _mode = value ;
    QString label ;
    bool isShow = false;
    switch(_mode) {
    default:
        isShow = false;
        label = "" ;
        break;
    case qxmledit::BLIND:
        isShow = true;
        label = tr("Blind Mode");
        break;
    case qxmledit::EXPLORE:
        isShow = true;
        label = tr("Explore Mode");
        break;
    }
    ui->label->setText(label);
    ui->label->setVisible(isShow);
    this->setVisible(isShow);
}

