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

#ifndef GUIDEDOPERATIONSDIALOG_H
#define GUIDEDOPERATIONSDIALOG_H

#include "xmlEdit.h"

class QXmlEditApplication ;
class ApplicationData ;

namespace Ui
{
class GuidedOperationsDialog;
}

class GuidedOperationsDialog : public QDialog
{
    Q_OBJECT
    QXmlEditApplication *_application;
    ApplicationData *_appData ;

    void makeButtonsSameSize();
public:
    explicit GuidedOperationsDialog(QXmlEditApplication *application, ApplicationData *appData, QWidget *parent = 0);
    ~GuidedOperationsDialog();

private:
    Ui::GuidedOperationsDialog *ui;

signals:
    void triggerNew();
    void triggerQuit();
    void triggerOpen();
    void triggerValidate();
    void triggerClose();

private slots:
    void on_cmdNew_clicked();
    void on_cmdQuit_clicked();
    void on_cmdOpen_clicked();
    void on_cmdValidate_clicked();

};

#endif // GUIDEDOPERATIONSDIALOG_H
