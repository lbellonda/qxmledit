/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
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

#include "columnarview.h"
#include "ui_columnarview.h"
#include "regola.h"
#include "utils.h"


void ColumnarView::showModal(QWidget *parent, Regola *target)
{
    ColumnarView columnarView(parent);
    columnarView.setRegola(target);
    columnarView.setModal(true);
    columnarView.exec();
}


ColumnarView::ColumnarView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColumnarView)
{
    regola = NULL ;
    ui->setupUi(this);
    ui->columnView2->setVisible(false);
}

ColumnarView::~ColumnarView()
{
    delete ui;
}

void ColumnarView::setRegola(Regola *value)
{
    regola = value ;
    if((NULL != regola) && (NULL != ui)) {
        ui->columnView->setModel(regola);
        ui->columnView2->setModel(regola);
    }
}

void ColumnarView::on_showSplit_stateChanged(int state)
{
    bool isVisible = (state == Qt::Checked) ? true : false;
    ui->columnView2->setVisible(isVisible);
}



