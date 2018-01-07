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


#ifndef SESSIONSMANAGEMENT_H
#define SESSIONSMANAGEMENT_H

#include <QDialog>
#include <QTableWidgetItem>
#include "UIDelegate.h"
#include "SessionDataInterface.h"

namespace Ui
{
class SessionsManagement;
}

class SessionsManagementDialog : public QDialog
{
    Q_OBJECT

public:
    enum UserChoice {
        UC_NONE,
        UC_LOADFILE,
        UC_ACTIVATESESSION
    };
private:
    SessionDataInterface *_dataAccess;
    UIDelegate *_uiDelegate;
    QVector<SessionListModel*> _data;
    SessionModel _model;
    UserChoice _result;
    QString _activationPath;
    int _sessionCode;

    QTableWidgetItem *addItem(const int row, const int column, const QString &text);
    void readSessionsData();
    void enableItems();
    SessionListModel *selectedModel();
    void errorNoSelection();
public:
    explicit SessionsManagementDialog(QWidget *parent, UIDelegate *uiDelegate, SessionDataInterface *dataAccess);
    ~SessionsManagementDialog();


    UserChoice userChoice();
    QString filePath();
    int sessionCode();

private:
    Ui::SessionsManagement *ui;

private slots:
    void onSessionTableChanged(QTableWidgetItem * current, QTableWidgetItem * previous);
    void on_activateCmd_clicked();
    void on_deleteCmd_clicked();
    void on_editCmd_clicked();
    void on_newCmd_clicked();
    void onFileSelected(FileModel* model);
    void onFileDoubleClicked(FileModel* model);
    void on_loadFileCmd_clicked();
    void on_exportFilesCmd_clicked();

signals:
    void sessionDeleted(const int sessionCode);
};

#endif // SESSIONSMANAGEMENT_H
