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

#ifndef NAVIGATIONBOX_H
#define NAVIGATIONBOX_H

#include <QWidget>

namespace Ui
{
class NavigationBox;
}

class NavigationBox : public QWidget
{
    Q_OBJECT

    int _value ;
    int _minValue ;
    int _maxValue ;


public:
    explicit NavigationBox(QWidget *parent = 0);
    ~NavigationBox();

    void setRange(const int minValue, const int maxValue);

private:
    Ui::NavigationBox *ui;

    void setValues();
    void enableButtons();
    void notify();
    void updateValue();


private slots:
    void on_goPrev_clicked();
    void on_goNext_clicked();
    void on_go_clicked();
    void on_slider_valueChanged(int value);
    void on_valueBox_valueChanged(int value);


signals:
    void navigateTo(const int page);
};

#endif // NAVIGATIONBOX_H
