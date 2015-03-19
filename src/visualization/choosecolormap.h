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


#ifndef CHOOSECOLORMAP_H
#define CHOOSECOLORMAP_H

#include <QDialog>
#include "colormap.h"

namespace Ui
{
class ChooseColorMap;
}

class ChooseColorMap : public QDialog
{
    Q_OBJECT

public:
    ColorMap *choosenMap;

    explicit ChooseColorMap(QList<ColorMap *>&colorMaps, QWidget *parent = 0);
    ~ChooseColorMap();

private:
    Ui::ChooseColorMap *ui;
private slots:
    void on_mapList_currentRowChanged(int currentRow);
};

#endif // CHOOSECOLORMAP_H
