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


#ifndef CONFIGURESESSIONSWIDGET_H
#define CONFIGURESESSIONSWIDGET_H

#include <QWidget>
#include "applicationdata.h"

namespace Ui
{
class ConfigureSessionsWidget;
}

class ConfigureSessionsWidget : public QWidget
{
    Q_OBJECT
    ApplicationData* _data;
public:
    explicit ConfigureSessionsWidget(QWidget *parent = 0);
    ~ConfigureSessionsWidget();

    void init(ApplicationData* data);
    void saveIfChanged();

private:
    Ui::ConfigureSessionsWidget *ui;

private slots:
    void on_deleteAllData_clicked();
    void on_deleteData_clicked();
    void on_deleteSpecifiedDate_toggled();
    void on_delOlderSixMonth_toggled();
    void on_delOlderOneMonth_toggled();
};

#endif // CONFIGURESESSIONSWIDGET_H
