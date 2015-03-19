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


#include "sessiondetaildialog.h"
#include "ui_sessiondetaildialog.h"
#include "modelutility.h"

SessionDetailDialog::SessionDetailDialog(QWidget *parent, UIDelegate *uiDelegate, SessionDataInterface *dataAccess, SessionModel *newModel) :
    QDialog(parent),
    _dataAccess(dataAccess),
    _uiDelegate(uiDelegate),
    _model(newModel),
    ui(new Ui::SessionDetailDialog)
{
    _result = UC_NONE ;
    ui->setupUi(this);
    Q_ASSERT(NULL != _model);
    SessionOperationStatus context;
    _model->clear();
    if(!_dataAccess->readSessionData(context, _model)) {
        _uiDelegate->error(tr("Error reading session data."));
    }
    ui->details->setNewModel(_model);
    ui->name->setText(_model->name);
    ui->description->setPlainText(_model->description);
    ui->lastAccess->setText(_model->lastAccess.toString(Qt::DefaultLocaleShortDate));

    connect(ui->details, SIGNAL(fileSelected(FileModel*)), this, SLOT(onFileSelected(FileModel*)));
    connect(ui->details, SIGNAL(fileDoubleClicked(FileModel*)), this, SLOT(onFileDoubleClicked(FileModel*)));
}

SessionDetailDialog::~SessionDetailDialog()
{
    delete ui;
}

SessionDetailDialog::UserChoice SessionDetailDialog::userChoice()
{
    return _result;
}

QString SessionDetailDialog::filePath()
{
    return _activationPath;
}

void SessionDetailDialog::onFileSelected(FileModel* model)
{
    QString path ;
    if(NULL != model) {
        _activationPath = model->path;
    }
    ui->editFileCmd->setEnabled(!_activationPath.isEmpty());
}

void SessionDetailDialog::onFileDoubleClicked(FileModel* model)
{
    QString path ;
    if(NULL != model) {
        _activationPath = model->path;
    }
    ui->editFileCmd->setEnabled(!_activationPath.isEmpty());
    on_editFileCmd_clicked();
}

void SessionDetailDialog::on_editFileCmd_clicked()
{
    if(!_activationPath.isEmpty()) {
        _result = UC_LOADFILE ;
        accept();
    }
}

bool SessionDetailDialog::saveModifiedData()
{
    QString name = ui->name->text();
    QString description = ui->description->toPlainText();
    if((name != _model->name) || (description != _model->description)) {
        _model->name = name ;
        _model->description = description ;
        SessionOperationStatus context;
        if(!_dataAccess->updateSession(context, _model)) {
            _uiDelegate->error(tr("Error saving session data"));
            return false;
        }
    }
    return true ;
}

void SessionDetailDialog::accept()
{
    if(saveModifiedData()) {
        QDialog::accept();
    }
}

void SessionDetailDialog::on_exportFilesCmd_clicked()
{
    ModelUtility::putFilesPathInClipboard(_model);
    _uiDelegate->message(tr("Data copied in the clipboard."));
}
