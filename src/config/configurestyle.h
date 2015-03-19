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
#ifndef CONFIGURESTYLE_H
#define CONFIGURESTYLE_H

#include <QWidget>
#include "libQXmlEdit_global.h"
#include "applicationdata.h"

namespace Ui
{
class ConfigureStyle;
}

class ConfigureStyle : public QWidget
{
    Q_OBJECT

    ApplicationData* _data;
    QButtonGroup _group;

public:
    explicit ConfigureStyle(QWidget *parent = 0);
    ~ConfigureStyle();

    void init(ApplicationData* data);
    void saveIfChanged();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ConfigureStyle *ui;

    void save();

private slots:
    void on_openLocation_clicked();
    void on_browseFolderCmd_clicked();
};

#endif // CONFIGURESTYLE_H
