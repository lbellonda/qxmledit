/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2018 by Luca Bellonda and individual contributors  *
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

#include "anonimyzebatchdialog.h"
#include "ui_anonimyzebatchdialog.h"
#include "utils.h"
#include "qxmleditdata.h"
#include "qxmleditconfig.h"
#include "modules/services/systemservices.h"
#include "modules/anonymize/anonprofile.h"
#include "modules/anonymize/anonprofiledialog.h"
#include "modules/anonymize/anoncontext.h"
#include "anonprofilemanager.h"

#define TOKEN_BROWSE    "..."
#define POLL_TIMEOUT    250

AnonimyzeBatchDialog::AnonimyzeBatchDialog(QXmlEditData *theData, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnonimyzeBatchDialog)
{
    _data = theData ;
    _profile = NULL ;
    _running = false;
    _aborted = false ;
    ui->setupUi(this);
    ui->lblOperation->setText("");
    setupFolders();
    enableOk();
}

AnonimyzeBatchDialog::~AnonimyzeBatchDialog()
{
    delete ui;
    if(NULL != _profile) {
        delete _profile ;
    }
}

void AnonimyzeBatchDialog::setupFolders()
{
    setupRecentFolders(ui->cmdInputFile, false);
    setupRecentFolders(ui->cmdOutputFile, true);
}

void AnonimyzeBatchDialog::setupRecentFolders(QToolButton *button, const bool isSave)
{
    QStringList preferredDirsNames;
    Config::loadStringArray(Config::KEY_MAIN_PREFDIRS, preferredDirsNames);
    preferredDirsNames.sort();

    QStringList folderNames;
    QStringList values;
    values << TOKEN_BROWSE ;
    folderNames << tr("Browse...") ;
    foreach(QString s, preferredDirsNames) {
        folderNames << s;
        values << s ;
    }
    if(!preferredDirsNames.isEmpty()) {
        folderNames << "------------";
        values << TOKEN_BROWSE ;
    }
    if(isSave) {
        QSet<QString> foldersSet;
        foreach(QString last, _data->lastFiles()) {
            QFile lastFile(last);
            QFileInfo lastFileInfo(lastFile);
            QDir dir = lastFileInfo.dir();
            foldersSet.insert(dir.absolutePath());
        }
        foreach(QString f, foldersSet.values()) {
            folderNames << f;
            values << f ;
        }
    } else {
        foreach(QString last, _data->lastFiles()) {
            folderNames << last;
            values << last ;
        }
    }
    Utils::loadButtonMenu(button, folderNames, values, this, isSave ? SLOT(onChooseOutput()) : SLOT(onChooseInput()));
}

QString AnonimyzeBatchDialog::fileForSelection(const QString &value, const QString &message, const bool isOpen)
{
    QString folder = SystemServices::userDocumentsDirectory();
    QString str = value ;
    if(TOKEN_BROWSE != str) {
        folder = str ;
    }
    QString filePath ;
    if(isOpen) {
        filePath = QFileDialog::getOpenFileName(this, message,
                                                folder, Utils::getFileFilterForOpenFile());
    } else {
        filePath = QFileDialog::getSaveFileName(this, message,
                                                folder, Utils::getFileFilterForOpenFile());
    }
    return filePath;
}

void AnonimyzeBatchDialog::onChooseInput()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if(NULL != action) {
        QString filePath = fileForSelection(action->data().toString(), tr("Source File to Anonymize"), true);
        setInputFilePath(filePath);
    }
}

void AnonimyzeBatchDialog::onChooseOutput()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if(NULL != action) {
        QString filePath = fileForSelection(action->data().toString(), tr("New Anonymized File"), false);
        setOutputFilePath(filePath);
    }
}

void AnonimyzeBatchDialog::setInputFilePath(const QString &theFilePath)
{
    if(!theFilePath.isEmpty()) {
        _inputFilePath = theFilePath;
        ui->inputFile->setText(theFilePath);
        enableOk();
    }
}

void AnonimyzeBatchDialog::setOutputFilePath(const QString &theFilePath)
{
    if(!theFilePath.isEmpty()) {
        _outputFilePath = theFilePath;
        ui->outputFile->setText(theFilePath);
        enableOk();
    }
}

bool AnonimyzeBatchDialog::checkParameters()
{
    if(_inputFilePath.isEmpty()) {
        Utils::error(this, tr("Select an input file"));
    }
    if(_outputFilePath.isEmpty()) {
        Utils::error(this, tr("Select an output file"));
    }
    if(NULL == _profile) {
        Utils::error(this, tr("Select a profile"));
    }
    return true;
}

void AnonimyzeBatchDialog::enableOk()
{
    bool okEnabled = (!_inputFilePath.isEmpty() && !_outputFilePath.isEmpty() && (NULL != _profile));
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(okEnabled);
}


AnonProfile* AnonimyzeBatchDialog::getProfileFromProfileData(GenericPersistentData *input)
{
    AnonProfile * result = new AnonProfile() ;
    bool flag = result->readFromSerializedXmlString(input->payload());
    if(!flag) {
        Utils::error(tr("Unable to parse the profile data."));
        delete result;
        result = NULL ;
    }
    return result;
}

void AnonimyzeBatchDialog::on_cmdChooseProfile_clicked()
{
    GenericPersistentData *newProfileData = NULL ;
    AnonProfileManagerFactory * profileManagerFactory = new AnonProfileManagerFactory();
    if(NULL != profileManagerFactory) {
        AnonProfileManager *manager = profileManagerFactory->newProfileManager();
        if(NULL != manager) {
            newProfileData = manager->chooseProfile(_data, this);
            delete manager ;
        }
        delete profileManagerFactory;
    }
    if(NULL != newProfileData) {
        QString name ;
        AnonProfile *newProfile = NULL ;
        name = newProfileData->name();
        newProfile = getProfileFromProfileData(newProfileData);
        if(NULL != newProfile) {
            if(NULL != _profile) {
                delete _profile ;
            }
            _profile = newProfile;
            ui->profileName->setText(name);
            enableOk();
        } else {
            Utils::error(this, tr("Unable to load the new profile"));
        }
        delete newProfileData;
    }
}

void AnonimyzeBatchDialog::opStart()
{
    if(!checkParameters()) {
        return ;
    }

    Utils::TODO_NEXT_RELEASE("_operation.saveSettings();");
    ui->buttonBox->setEnabled(false);
    startOperation();
}

void AnonimyzeBatchDialog::on_buttonBox_clicked(QAbstractButton * button)
{
    if(ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
        opStart();
    }
}

void AnonimyzeBatchDialog::accept()
{
    opStart();
}

void AnonimyzeBatchDialog::reject()
{
    QDialog::reject();
}


void AnonimyzeBatchDialog::startOperation()
{
    ui->stackedWidget->setCurrentIndex(ProgressView);
    // disable apply
    ui->buttonBox->setEnabled(false);
    _running = true ;
    _aborted = false ;
    _future = QtConcurrent::run(this, &AnonimyzeBatchDialog::doOperationWorkThread);
    QTimer::singleShot(POLL_TIMEOUT, this, SLOT(checkIfDone()));
}

void AnonimyzeBatchDialog::doOperationWorkThread()
{
    AnonContext context(NULL, "");
    context.setAlg(_profile->params());
    context.setProfile(_profile->clone());
    _operation.perform(_inputFilePath, _outputFilePath, &context);
}

void AnonimyzeBatchDialog::closeEvent(QCloseEvent * event)
{
    if(_running) {
        event->ignore();
        return ;
    }
    event->accept();
}

void AnonimyzeBatchDialog::checkIfDone()
{
    if(!_running) {
        return ;
    }
    //------------------------------------------------------------
    if(_aborted) {
        setAborted();
        endOfOperation();
        return ;
    }
    if(!_future.isFinished()) {
        int counterOperations = _operation.operationsCount();
        //----------------------------
        ui->lblOperation->setText(tr("%1").arg(counterOperations));

        QTimer::singleShot(POLL_TIMEOUT, this, SLOT(checkIfDone()));
    } else {
        endOfOperation();
    }
}

void AnonimyzeBatchDialog::endOfOperation()
{
    _running = false;
    if(_aborted) {
        Utils::message(this, tr("User abort requested."));
    } else {
        const AnonOperationResult *res = _operation.result();
        if(res->isError()) {
            Utils::error(this, tr("Error: %1, '%2'").arg(res->code()).arg(res->message()));
        } else {
            if(Utils::askYN(tr("Operation terminated.\nDo you want to show the result folder in the browser?"))) {

                QFile outFile(_outputFilePath);
                QFileInfo outFileInfo(outFile);
                QDir dir = outFileInfo.dir();
                QDesktopServices::openUrl(QUrl::fromLocalFile(dir.absolutePath()));
            }
            _outputFilePath = "";
            ui->outputFile->setText("");
            enableOk();
        }
    }
    freeResources();
}

void AnonimyzeBatchDialog::onOpCanceled()
{
    setAborted();
    endOfOperation();
}

void AnonimyzeBatchDialog::on_cmdCancel_clicked()
{
    ui->lblOperation->setText(tr("Aborting..."));
    _aborted = true ;
}

void AnonimyzeBatchDialog::setAborted()
{
    if(_running) {
        _operation.setAborted() ;
        _future.cancel();
        if(_future.isStarted() && _future.isRunning()) {
            _future.waitForFinished();
        }
    }
}

void AnonimyzeBatchDialog::freeResources()
{
    ui->lblOperation->setText("");
    _running = false;
    _aborted = false ;
    ui->stackedWidget->setCurrentIndex(NormalView);
    ui->buttonBox->setEnabled(true);
}
