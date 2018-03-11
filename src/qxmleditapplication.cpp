/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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

#include "qxmleditapplication.h"
#include "mainwindow.h"
#include "modules/encoding/codepagedialog.h"
#include "uiservices.h"
#include "visualization/datavisualization.h"
#include "modules/services/anotifier.h"
#include "StartParams.h"
#include "utils.h"
#include <QDataStream>
#include <QLocalSocket>
#include "qxmleditconfig.h"
#include "modules/anonymize/anonymizebatch.h"
#include "extraction/extractfragmentsdialog.h"
#include "modules/help/firstaccessdialog.h"
#include "modules/help/guidedoperationsdialog.h"
#include "modules/help/guidedvalidationdialog.h"
#include "modules/style/choosestyledialog.h"

const QString QXmlEditApplication::ServerName("__qxmledit__server__");

QXmlEditApplication::QXmlEditApplication(int &argc, char **argv) :
    QApplication(argc, argv)
{
    _appData = NULL ;
    _server = NULL ;
    _logger = NULL ;
    _guidedOperationsDialog = NULL ;
    _guidedValidationDialog = NULL ;
}

QXmlEditApplication::~QXmlEditApplication()
{
    if(NULL != _guidedOperationsDialog) {
        connectToCommandsPanel(false, _guidedOperationsDialog);
        _guidedOperationsDialog->close();
        delete _guidedOperationsDialog ;
        _guidedOperationsDialog = NULL ;
    }
    if(NULL != _guidedValidationDialog) {
        _guidedValidationDialog->close();
        delete _guidedValidationDialog ;
        _guidedValidationDialog = NULL ;
    }

    if(NULL != _server) {
        disconnect(_server, SIGNAL(newConnection()), this, SLOT(newServerConnection()));
        if(_server->isListening()) {
            _server->close();
        }
        delete _server;
        _server = NULL ;
    }
}

FrwLogger *QXmlEditApplication::logger() const
{
    return _logger;
}

void QXmlEditApplication::setLogger(FrwLogger *logger)
{
    _logger = logger;
}


ApplicationData *QXmlEditApplication::appData() const
{
    return _appData;
}

void QXmlEditApplication::setAppData(ApplicationData *value)
{
    if(NULL != _appData) {
        disconnect(_appData, SIGNAL(openUserGuidedPanel()), this, SLOT(onOpenUserGuidedPanel()));
    }
    _appData = value;
    if(NULL != _appData) {
        connect(_appData, SIGNAL(openUserGuidedPanel()), this, SLOT(onOpenUserGuidedPanel()));
        if(_appData->notifier()->isEnabled()) {
            _appData->notifier()->show();
        }
    }
}

bool QXmlEditApplication::event(QEvent *event)
{
    switch(event->type()) {
    case QEvent::FileOpen: {
        QString newFile = static_cast<QFileOpenEvent*>(event)->file();
        MainWindow * newWindow = makeNewWindow();
        newWindow->show();
        newWindow->loadFile(newFile);
        return true;
    }
    break;
    default:
        return QApplication::event(event);
    }
    return false;
}

void QXmlEditApplication::onNewWindow()
{
    MainWindow * newWindow = makeNewWindow();
    newWindow->show();
}

MainWindow *QXmlEditApplication::makeNewWindow()
{
    return _appData->newWindow();
}

void QXmlEditApplication::onEncodingTools()
{
    _appData->uiServices()->doEncodingDialog(_appData, false);
}

void QXmlEditApplication::onCodePagesTools()
{
    CodePageDialog::showCodePageWindow(_appData, NULL);
}

void QXmlEditApplication::onManageSessions()
{
    SessionManager *sessionManager = _appData->sessionManager();
    if(!sessionManager->isEnabled()) {
        return;
    }
    sessionManager->manageSessions(NULL, NULL, _appData->lastFiles());
}

void QXmlEditApplication::onSplitFile()
{
    ExtractResults *results = new ExtractResults(this) ;
    extractFragmentsWindow(results, NULL, NULL);
}

void QXmlEditApplication::onViewData()
{
    DataVisualization::viewDataWindow(_appData, NULL, NULL, "");
}

bool QXmlEditApplication::errorCloseConnection(QLocalSocket *client)
{
    client->disconnectFromServer();
    if(NULL != _logger) {
        _logger->error(QString("Server::Disconnecting an incoming connection, cause:%1").arg(client->errorString()));
    }
    return false ;
}

void QXmlEditApplication::onRaiseWindows()
{
    MainWindow *lastWindow = NULL ;
    foreach(MainWindow * window, appData()->windows()) {
        if(window->isMinimized()) {
            window->showNormal();
        }
        window->show();
        window->raise();
        lastWindow = window ;
    }
    if(NULL != lastWindow) {
        lastWindow->activateWindow();
    }
}

void QXmlEditApplication::newServerConnection()
{
    QLocalSocket *newInstanceConnection = _server->nextPendingConnection();
    if(NULL != newInstanceConnection) {
        connect(newInstanceConnection, SIGNAL(disconnected()), newInstanceConnection, SLOT(deleteLater()));
        QDataStream inputDataStream(newInstanceConnection);
        inputDataStream.setVersion(QDataStream::Qt_4_0);
        quint32 expected = 0 ;
        int times = 0;
        while(expected == 0) {
            times++;
            if(times >= 6) {
                errorCloseConnection(newInstanceConnection);
                return ;
            }
            qint64 bytesAvailable = newInstanceConnection->bytesAvailable() ;
            if(bytesAvailable >= (qint64)sizeof(qint32)) {
                inputDataStream >> expected ;
                if(expected == 0) {
                    errorCloseConnection(newInstanceConnection);
                    return ;
                }
            }
            newInstanceConnection->waitForReadyRead(ReadTimeOutMs);
        }
        times = 0 ;
        while(newInstanceConnection->bytesAvailable() < expected) {
            times++;
            if(times >= 6) {
                errorCloseConnection(newInstanceConnection);
                return ;
            }
            newInstanceConnection->waitForReadyRead(ReadTimeOutMs);
        }
        inputDataStream.setVersion(QDataStream::Qt_4_0);
        StartParams params;
        quint32 aType ;
        inputDataStream >> aType ;
        params.type = (StartParams::ESPType) aType ;
        inputDataStream >> params.fileName ;
        inputDataStream >> params.parametersError ;
        newInstanceConnection->disconnectFromServer();
        if(NULL != _logger) {
            _logger->debug(QString("Server::received code:%1, file:'%2'").arg(params.type).arg(params.fileName));
        }
        switch(params.type) {
        case StartParams::OpenFile: {
            // scan existing files
            QString normalizedInputFile = Utils::normalizeFilePath(params.fileName);
            foreach(MainWindow * wnd, _appData->windows()) {
                if(Utils::normalizeFilePath(wnd->getRegola()->fileName()) == normalizedInputFile) {
                    if(NULL != _logger) {
                        _logger->debug(QString("Server::reusing window for:'%1'").arg(params.fileName));
                    }
                    wnd->show();
                    wnd->raise();
                    wnd->activateWindow();
                    wnd->reload();
                    return ;
                }
            }
            MainWindow *newWindow = makeNewWindow();
            newWindow->loadFile(params.fileName);
            newWindow->show();
            newWindow->raise();
            break;
        }
        case StartParams::VisFile: {
            MainWindow *newWindow = makeNewWindow();
            newWindow->loadVisFile(params.fileName);
            newWindow->show();
            break;
        }
        default:
            break;
        } // switch
    }
}

bool QXmlEditApplication::handleSingleInstance(StartParams * startParams)
{
    if(!Config::getBool(Config::KEY_GENERAL_SINGLE_INSTANCE, true)) {
        return false;
    }
    if(startParams->type == StartParams::Anonymize) {
        return false;
    }
    if(startParams->type != StartParams::Nothing) {
        if(connectToExistingServer(startParams)) {
            return true ;
        }
    }
    startServer();
    return false;
}

bool QXmlEditApplication::startServer()
{
    bool isConnected = false ;
    _server = new QLocalServer();
    connect(_server, SIGNAL(newConnection()), this, SLOT(newServerConnection()));
    if(!_server->listen(ServerName)) {
        _server->removeServer(ServerName);
        if(_server->listen(ServerName)) {
            isConnected = true ;
        }
    } else {
        isConnected = true ;
    }
    if(NULL != _logger) {
        if(_logger->isEnabled() && _logger->isLoggable(FrwLogger::DEBUG)) {
            _logger->debug(QString("Server listening=%1 at:'%2'").arg(isConnected).arg(_server->fullServerName()));
        }
    }
    return false;
}

/*!
 * \brief QXmlEditApplication::connectToExistingServer try to connect to an existing server.
 * If it succeeds, it sends the params to the server, thwen returns true.
 * \param startParams
 * \return if the connections is handled or not.
 */
bool QXmlEditApplication::connectToExistingServer(StartParams * startParams)
{
    bool handled = false;
    QLocalSocket *socket = new QLocalSocket();
    // no connections,
    socket->connectToServer(ServerName);
    if(socket->waitForConnected(ConnectionTimeout)) {
        QByteArray data = paramsToByteArray(startParams);
        qint64 bytesWritten = socket->write(data);
        if(bytesWritten == data.length()) {
            socket->flush();
            if(NULL != _logger) {
                _logger->debug(QString("Client::sent request for:'%1'").arg(startParams->fileName));
            }
            handled = true ;
        }
        socket->disconnectFromServer();
    }
    delete socket;
    if(!handled) {
        if(NULL != _logger) {
            _logger->debug(QString("Client::no server handled request for: '%1'").arg(startParams->fileName));
        }
    }
    return handled;
}


QByteArray QXmlEditApplication::paramsToByteArray(StartParams * startParams)
{
    QByteArray result;
    QDataStream dataStream(&result, QIODevice::WriteOnly);
    dataStream.setVersion(QDataStream::Qt_4_0);
    dataStream << (quint32)0;
    dataStream << (quint32)startParams->type;
    dataStream << startParams->fileName;
    dataStream << (bool)startParams->parametersError;
    dataStream.device()->seek(0);
    dataStream << (quint32)(result.size() - sizeof(quint32));
    return result;
}

OperationResult *QXmlEditApplication::anonymizeBatch(const QString &newFileInputPath, const QString &newProfileName, const QString &newFileOutputPath)
{
    OperationResult *result = new OperationResult();
    AnonymizeBatch batchOperation(_appData, newFileInputPath, newProfileName, newFileOutputPath);
    batchOperation.operation();
    if(batchOperation.isError()) {
        result->setErrorWithText(batchOperation.errorMessage());
    }
    return result;
}

bool QXmlEditApplication::handleFirstAccess()
{
    Utils::TODO_THIS_RELEASE("spostata");
    Utils::TEST_ME("");
    if(_appData->isUserFirstAccess()) {
        showUserTypePanel();
        _appData->fireUserFirstAccess();
    }
    if(_appData->isUserGuidedOperation()) {
        // modeless
        showGuidedOperationsPanel();
        return true;
    }
    return false;
}

bool QXmlEditApplication::showUserTypePanel()
{
    Utils::TODO_THIS_RELEASE("spostata, eliminare o redirect");
    Utils::TEST_ME("");
    FirstAccessDialog firstAccessDialog(_appData);
    firstAccessDialog.exec();
    return true ;
}

bool QXmlEditApplication::showGuidedOperationsPanel()
{
    Utils::TEST_ME("");
    Utils::TODO_THIS_RELEASE("fare");
    if(NULL == _guidedOperationsDialog) {
        _guidedOperationsDialog = new GuidedOperationsDialog(this, _appData);
        connectToCommandsPanel(true, _guidedOperationsDialog);
        _guidedOperationsDialog->setModal(false);
    }
    _guidedOperationsDialog->show();
    _guidedOperationsDialog->raise();
    _guidedOperationsDialog->activateWindow();
    return true;
}

bool QXmlEditApplication::showValidationOperationsPanel()
{
    Utils::TEST_ME("");
    Utils::TODO_THIS_RELEASE("fare");
    if(NULL == _guidedValidationDialog) {
        _guidedValidationDialog = new GuidedValidationDialog(NULL, _appData);
        _guidedValidationDialog->setModal(false);
    }
    _guidedValidationDialog->resetData();
    _guidedValidationDialog->show();
    _guidedValidationDialog->raise();
    _guidedValidationDialog->activateWindow();
    return true;
}

void QXmlEditApplication::bindCommandOperation(const bool isConnect, const QObject *sender, const char *signal, const char *method)
{
    if(isConnect) {
        connect(sender, signal, this, method);
    } else {
        disconnect(sender, signal, this, method);
    }
}

void QXmlEditApplication::connectToCommandsPanel(const bool isConnect, GuidedOperationsDialog *target)
{
    bindCommandOperation(isConnect, target, SIGNAL(triggerNew()), SLOT(onCommandNew()));
    Utils::TODO_THIS_RELEASE("fare");
    bindCommandOperation(isConnect, target, SIGNAL(triggerQuit()), SLOT(onCommandQuit()));
    bindCommandOperation(isConnect, target, SIGNAL(triggerOpen()), SLOT(onCommandOpen()));
    bindCommandOperation(isConnect, target, SIGNAL(triggerValidate()), SLOT(onCommandValidate()));
}


void QXmlEditApplication::setupFirstAccessForPreferences()
{
    if(!Config::getBool(Config::KEY_GENERAL_VIEW_EDITOR_ADJUST, false)) {
        Config::saveBool(Config::KEY_GENERAL_VIEW_EDITOR_ADJUST, true);
        taskChooseDetail();
    }
}

void QXmlEditApplication::taskChooseDetail()
{
    ChooseStyleDialog dlg(NULL);
    dlg.setModal(true);
    dlg.setWindowModality(Qt::ApplicationModal);
    if(dlg.exec() == QDialog::Accepted) {
        DisplayStyleSetting *theStyle = dlg.selectedStyle();
        if(NULL != theStyle) {
            // save the selection to the configuration
            PaintInfo paintInfo;
            paintInfo.loadState();
            theStyle->applyToPaintInfo(&paintInfo);
            paintInfo.setChanged();
            paintInfo.saveState();
            // apply to all the editors
            _appData->updateEditors();
        }
    }
}

MainWindow *QXmlEditApplication::getOrCreateMainWindow()
{
    if(appData()->windows().isEmpty()) {
        MainWindow *mainWindow = new MainWindow(false, _appData);
        return mainWindow ;
    }
    return appData()->windows().at(0);
}

void QXmlEditApplication::onOpenUserGuidedPanel()
{
    showGuidedOperationsPanel();
}

