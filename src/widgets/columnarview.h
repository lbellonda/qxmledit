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

#ifndef COLUMNARVIEW_H
#define COLUMNARVIEW_H

#include <QDialog>

class Regola;

namespace Ui
{
class ColumnarView;
}

class ColumnarView : public QDialog
{
    Q_OBJECT

    Regola *regola ;
public:
    explicit ColumnarView(QWidget *parent = 0);
    ~ColumnarView();

    void setRegola(Regola *value);

    static void showModal(QWidget *parent, Regola *target);

private:
    Ui::ColumnarView *ui;

private slots:
    void on_showSplit_stateChanged(int state);
};

#endif // COLUMNARVIEW_H
