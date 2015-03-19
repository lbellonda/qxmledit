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


#include "choosecolormap.h"
#include "ui_choosecolormap.h"
#include "cmapitemdelegate.h"
#include "utils.h"

ChooseColorMap::ChooseColorMap(QList<ColorMap *>&colorMaps, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseColorMap)
{
    choosenMap = NULL ;
    ui->setupUi(this);

    CMapItemDelegate w(ui->mapList);
    ui->mapList->setUniformItemSizes(true);
    bool isFirst = true;
    QRect r;
    foreach(ColorMap * map, colorMaps) {
        QListWidgetItem *item = new QListWidgetItem("");
        if(NULL != item) {
            item->setData(Qt::UserRole, qVariantFromValue((void*)map));
            item->setText(map->name());
            ui->mapList->addItem(item);
            if(isFirst) {
                isFirst = false ;
                QRect rectItem = ui->mapList->visualItemRect(item) ;
                r.setRect(0, 0, rectItem.width() / 2, rectItem.height());
                w.setGeometry(r);
            }
            w.setColorMap(map);
            w.update();
            QImage imageMap = w.colorMapImage();
            QImage image = imageMap.scaledToWidth(size().width() / 3);
            item->setData(Qt::DecorationRole, image);
        } else {
            Utils::error(tr("Error loading data."));
        }
    }
}

ChooseColorMap::~ChooseColorMap()
{
    delete ui;
}


void ChooseColorMap::on_mapList_currentRowChanged(int /*currentRow*/)
{
    QListWidgetItem *item = ui->mapList->currentItem();
    if(NULL != item) {
        QVariant v = item->data(Qt::UserRole);
        ColorMap *map =  static_cast<ColorMap *>(v.value<void*>());
        choosenMap = map ;
    }
}
