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


#ifndef SESSIONDETAIL_H
#define SESSIONDETAIL_H

#include <QDialog>
#include "UIDelegate.h"
#include "SessionDataInterface.h"

namespace Ui
{
class SessionDetailDialog;
}

class SessionDetailDialog : public QDialog
{
    Q_OBJECT
public:
    enum UserChoice {
        UC_NONE,
        UC_LOADFILE
    };
private:
    UserChoice _result;
    SessionDataInterface *_dataAccess;
    UIDelegate *_uiDelegate;
    SessionModel *_model;
    QString _activationPath;

    bool saveModifiedData();

public:
    explicit SessionDetailDialog(QWidget *parent, UIDelegate *uiDelegate, SessionDataInterface *dataAccess, SessionModel *model);
    ~SessionDetailDialog();

    UserChoice userChoice();
    QString filePath();

private:
    Ui::SessionDetailDialog *ui;

private slots:
    void onFileSelected(FileModel* model);
    void onFileDoubleClicked(FileModel* model);
    void on_editFileCmd_clicked();
    void on_exportFilesCmd_clicked();

public slots:
    void accept();
};

#endif // SESSIONDETAIL_H
