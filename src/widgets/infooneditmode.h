/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2018 by Luca Bellonda and individual contributors       *
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

#ifndef INFOONEDITMODE_H
#define INFOONEDITMODE_H

#include "xmlEdit.h"
#include <QPropertyAnimation>

namespace Ui
{
class InfoOnEditMode;
}

class InfoOnEditMode : public QWidget
{
    Q_OBJECT

    QPropertyAnimation _animation;

public:
    explicit InfoOnEditMode(QWidget *parent = 0);
    ~InfoOnEditMode();

private:
    Ui::InfoOnEditMode *ui;

    void checkLink(const QString &link);

signals:
    void requestDismiss();
    void requestOpenPanel();
private slots:
    void on_infoText_linkActivated(const QString &link);
    void on_infoText2_linkActivated(const QString &link);
    void on_infoText3_linkActivated(const QString &link);
};

#endif // INFOONEDITMODE_H
