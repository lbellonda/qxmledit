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


#ifndef DIFFMAPWIDGET_H
#define DIFFMAPWIDGET_H

#include <QWidget>
#include <QTreeWidgetItem>

namespace Ui
{
class DiffMapWidget;
}

class DiffMapWidget : public QWidget
{
    Q_OBJECT
    QImage _cachedImage;
    bool _newData;
    QList<QTreeWidgetItem*> *_data;
    int _selection;

    void generateImage();

public:
    explicit DiffMapWidget(QWidget *parent = 0);
    ~DiffMapWidget();

    void setDiffList(QList<QTreeWidgetItem*> *newData);
    void setSelection(const int newValue);

protected:
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *e);

private:
    Ui::DiffMapWidget *ui;

signals:
    void userChangedSelection(int newValue);
};

#endif // DIFFMAPWIDGET_H
