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


#ifndef CMAPITEMDELEGATE_H
#define CMAPITEMDELEGATE_H

#include <QWidget>
#include "colormap.h"

namespace Ui
{
class CMapItemDelegate;
}

class CMapItemDelegate : public QWidget
{
    Q_OBJECT

public:
    explicit CMapItemDelegate(QWidget *parent = 0);
    ~CMapItemDelegate();

    void setColorMap(ColorMap *map);
    QPixmap colorMapImage(const QSize &size);
    QImage colorMapImage();

private:
    Ui::CMapItemDelegate *ui;
};

#endif // CMAPITEMDELEGATE_H
