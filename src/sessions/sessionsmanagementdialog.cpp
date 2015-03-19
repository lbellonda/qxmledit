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


#include "sessionsmanagementdialog.h"
#include "ui_sessionsmanagementdialog.h"
#include <QListWidgetItem>
#include "sessiondetaildialog.h"
#include "sessionrequester.h"
#include "modelutility.h"

SessionsManagementDialog::SessionsManagementDialog(QWidget *parent, UIDelegate *uiDelegate, SessionDataInterface *dataAccess) :
    QDialog(parent),
    _dataAccess(dataAccess),
    _uiDelegate(uiDelegate),
    ui(new Ui::SessionsManagement)
{
    _result = UC_NONE ;
    _sessionCode = 0 ;

    ui->setupUi(this);
    // select an item
    connect(ui->sessionList, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)), this, SLOT(onSessionTableChanged(QTableWidgetItem *, QTableWidgetItem *)));
    connect(ui->details, SIGNAL(fileSelected(FileModel*)), this, SLOT(onFileSelected(FileModel*)));
    connect(ui->details, SIGNAL(fileDoubleClicked(FileModel*)), this, SLOT(onFileDoubleClicked(FileModel*)));

    ui->details->setNewModel(&_model);

    ui->sessionList->setColumnCount(6);
    QStringList labels;
    labels.append(tr("Id"));
    labels.append(tr("Name"));
    labels.append(tr("Description"));
    labels.append(tr("Created"));
    labels.append(tr("Last Access"));
    labels.append(tr("Count"));
    ui->sessionList->setHorizontalHeaderLabels(labels);
    readSessionsData();
    enableItems();
}

SessionsManagementDialog::~SessionsManagementDialog()
{
    delete ui;
    foreach(SessionListModel * model, _data) {
        delete model ;
    }
}

SessionsManagementDialog::UserChoice SessionsManagementDialog::userChoice()
{
    return _result;
}

QString SessionsManagementDialog::filePath()
{
    return _activationPath;
}

int SessionsManagementDialog::sessionCode()
{
    return _sessionCode;
}


QTableWidgetItem *SessionsManagementDialog::addItem(const int row, const int column, const QString &text)
{
    QTableWidgetItem *item = new QTableWidgetItem();
    item->setText(text);
    ui->sessionList->setItem(row, column, item);
    return item ;
}

void SessionsManagementDialog::readSessionsData()
{
    SessionOperationStatus context;
    _data = _dataAccess->readSessionList(context);
    if(!context.ok) {
        _uiDelegate->error(tr("error retrieving session data"));
    }
    ui->sessionList->setUpdatesEnabled(false);
    ui->sessionList->setRowCount(_data .size());
    int row = 0 ;
    foreach(SessionListModel * model, _data) {
        QTableWidgetItem *item = addItem(row, 0, QString("%1").arg(model->session.id));
        addItem(row, 1, model->session.name);
        addItem(row, 2, model->session.description);
        QDateTime dtCreation = model->session.creationDate;
        QString sCreation = dtCreation.toString(Qt::DefaultLocaleShortDate);
        addItem(row, 3, sCreation);
        QDateTime dtAccess = model->lastAccess;
        QString sAccess = dtAccess.toString(Qt::DefaultLocaleShortDate);
        addItem(row, 4, sAccess);
        addItem(row, 5, QString("%1").arg(model->count));
        item->setData(Qt::UserRole, qVariantFromValue((void*)model));
        row++;
    }
    ui->sessionList->resizeColumnsToContents();
    ui->sessionList->setUpdatesEnabled(true);
}

void SessionsManagementDialog::onSessionTableChanged(QTableWidgetItem * current, QTableWidgetItem * previous)
{
    QString selectedDescription = "";
    QString selectedName = "";

    bool go = false;
    if(current != NULL) {
        if(previous == NULL) {
            go = true ;
        } else {
            if(current->row() != previous->row()) {
                go = true ;
            }
        }
    }
    if(go) {
        bool error = true ;
        SessionOperationStatus context;
        ui->details->setNewModel(NULL);
        _model.clear();
        _model.id = _data.at(current->row())->session.id;
        if(_dataAccess->readSession(context, &_model)) {
            if(_dataAccess->readSessionData(context, &_model)) {
                ui->details->setNewModel(&_model);
                error = false;
            }
            selectedDescription = _model.description ;
            selectedName = _model.name;
        }
        if(error) {
            _uiDelegate->error(tr("Error reading session data"));
        }
    }
    ui->selectedDescription->setText(selectedDescription) ;
    ui->selectedName->setText(selectedName) ;
    enableItems();
}


void SessionsManagementDialog::enableItems()
{
    bool enabledSButtons = (ui->sessionList->currentItem() != NULL);
    bool enabledDButtons = !_activationPath.isEmpty();
    ui->editCmd->setEnabled(enabledSButtons);
    ui->deleteCmd->setEnabled(enabledSButtons);
    ui->activateCmd->setEnabled(enabledSButtons);
    ui->loadFileCmd->setEnabled(enabledDButtons);
    ui->exportFilesCmd->setEnabled(enabledSButtons);
}

SessionListModel *SessionsManagementDialog::selectedModel()
{
    QTableWidgetItem *item = ui->sessionList->currentItem();
    if(NULL != item) {
        int row = item->row();
        item = ui->sessionList->item(row, 0);
        QVariant data = item->data(Qt::UserRole);
        SessionListModel *model = (SessionListModel *)data.value<void*>();
        return model ;
    }
    return NULL ;
}

void SessionsManagementDialog::errorNoSelection()
{
    _uiDelegate->error(tr("Select a session."));
}
void SessionsManagementDialog::on_activateCmd_clicked()
{
    SessionListModel *theModel = selectedModel();
    if(NULL != theModel) {
        _result = UC_ACTIVATESESSION ;
        _sessionCode = theModel->session.id;
        done(0);
    } else {
        errorNoSelection();
    }
}

void SessionsManagementDialog::on_deleteCmd_clicked()
{
    SessionListModel *theModel = selectedModel();
    if(NULL != theModel) {
        if(_uiDelegate->askYN(tr("Delete the selected session?"))) {
            SessionOperationStatus status;
            if(!_dataAccess->deleteSession(status, &theModel->session)) {
                _uiDelegate->error(tr("An error occurred deleting the session data"));
            } else {
                emit sessionDeleted(theModel->session.id);
            }
        }
        readSessionsData();
        enableItems();
    } else {
        errorNoSelection();
    }
}

void SessionsManagementDialog::on_editCmd_clicked()
{
    bool isReload = false;
    SessionListModel *theModel = selectedModel();
    if(NULL != theModel) {
        SessionDetailDialog sessionDetailDialog(this, _uiDelegate, _dataAccess, &theModel->session);
        sessionDetailDialog.exec();
        switch(sessionDetailDialog.userChoice()) {
        case SessionDetailDialog::UC_LOADFILE:
            _activationPath = sessionDetailDialog.filePath();
            on_loadFileCmd_clicked();
            break;
        default:
            isReload = true ;
            break;
        }
        if(isReload) {
            readSessionsData();
            enableItems();
        }
    } else {
        errorNoSelection();
    }
}

void SessionsManagementDialog::on_newCmd_clicked()
{
    SessionOperationStatus context;
    SessionModel model;
    QString newSessionName = SessionRequester::askForNewSessionName(this, _uiDelegate);
    if(!newSessionName.isEmpty()) {
        model.name = newSessionName ;
        model.description = "";
        model.enabled = true ;
        if(!_dataAccess->newSession(context, &model)) {
            _uiDelegate->error("Error crating a session.");
        }
        readSessionsData();
        enableItems();
    }
}

void SessionsManagementDialog::onFileSelected(FileModel* model)
{
    if(NULL != model) {
        _activationPath = model->path;
    }
    ui->loadFileCmd->setEnabled(!_activationPath.isEmpty());
}


void SessionsManagementDialog::onFileDoubleClicked(FileModel* model)
{
    if(NULL != model) {
        _activationPath = model->path;
    }
    ui->loadFileCmd->setEnabled(!_activationPath.isEmpty());
    on_loadFileCmd_clicked();
}

void SessionsManagementDialog::on_loadFileCmd_clicked()
{
    if(!_activationPath.isEmpty()) {
        _result = UC_LOADFILE ;
        accept();
    }
}

void SessionsManagementDialog::on_exportFilesCmd_clicked()
{
    ModelUtility::putFilesPathInClipboard(&_model);
    _uiDelegate->message(tr("Data copied in the clipboard."));
}
