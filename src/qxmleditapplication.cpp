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
#include <QDesktopWidget>
#include "qxmleditconfig.h"
#include "modules/anonymize/anonymizebatch.h"
#include "extraction/extractfragmentsdialog.h"
#include "modules/help/firstaccessdialog.h"
#include "modules/help/guidedoperationsdialog.h"
#include "modules/help/guidedvalidationdialog.h"
#include "modules/help/functionkeysinfo.h"
#include "modules/uiutil/defaultuidelegate.h"
#include "widgets/shortcutinfo.h"

const QString QXmlEditApplication::ServerName("__qxmledit__server__");

QXmlEditApplication::QXmlEditApplication(int &argc, char **argv) :
    QApplication(argc, argv)
{
    _appData = NULL ;
    _server = NULL ;
    _logger = NULL ;
    _guidedOperationsDialog = NULL ;
    _functionKeysInfo = NULL ;
    _uiDelegate = new DefaultUIDelegate();
}

QXmlEditApplication::~QXmlEditApplication()
{
    if(NULL != _guidedOperationsDialog) {
        connectToCommandsPanel(false, _guidedOperationsDialog);
        _guidedOperationsDialog->close();
        delete _guidedOperationsDialog ;
        _guidedOperationsDialog = NULL ;
    }
    if(NULL != _functionKeysInfo) {
        disconnect(_functionKeysInfo, SIGNAL(actionRequested(const QString &)), this, SLOT(onShortcutActivated(const QString &)));
        _functionKeysInfo->close();
        delete _functionKeysInfo ;
        _functionKeysInfo = NULL ;
    }

    if(NULL != _server) {
        disconnect(_server, SIGNAL(newConnection()), this, SLOT(newServerConnection()));
        if(_server->isListening()) {
            _server->close();
        }
        delete _server;
        _server = NULL ;
    }
    if(NULL != _uiDelegate) {
        delete _uiDelegate ;
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
        disconnect(_appData, SIGNAL(openUserTypePanel(const bool)), this, SLOT(onOpenUserTypePanel(const bool)));
        disconnect(_appData, SIGNAL(windowActivated(MainWindow *, bool)), this, SLOT(onWindowActivated(MainWindow *, bool)));
        disconnect(_appData, SIGNAL(keyboardShortcutOpenCloseRequest()), this, SLOT(onKeyboardShortcutOpenCloseRequest()));
        disconnect(_appData, SIGNAL(requestEnableKeys(MainWindow *)), this, SLOT(onRequestEnableKeys(MainWindow *)));
    }
    _appData = value;
    if(NULL != _appData) {
        connect(_appData, SIGNAL(openUserGuidedPanel()), this, SLOT(onOpenUserGuidedPanel()));
        connect(_appData, SIGNAL(openUserTypePanel(const bool)), this, SLOT(onOpenUserTypePanel(const bool)));
        connect(_appData, SIGNAL(windowActivated(MainWindow *, bool)), this, SLOT(onWindowActivated(MainWindow *, bool)));
        connect(_appData, SIGNAL(keyboardShortcutOpenCloseRequest()), this, SLOT(onKeyboardShortcutOpenCloseRequest()));
        connect(_appData, SIGNAL(requestEnableKeys(MainWindow *)), this, SLOT(onRequestEnableKeys(MainWindow *)));
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
        _logger->error(QString("Server::Disconnecting an incoming connection, cause: %1").arg(client->errorString()));
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

bool QXmlEditApplication::showUserTypePanel(const bool nextAccess)
{
    FirstAccessDialog firstAccessDialog(_appData);
    firstAccessDialog.exec();
    if(!nextAccess && _appData->isUserGuidedOperation()) {
        _appData->setDefaultViewDetail();
        // remove any outstanding requests
        _appData->testAndMarkFirstAccessForViewPreferences();
    }
    return true ;
}

bool QXmlEditApplication::showGuidedOperationsPanel()
{
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
    // Warning! this panel is ApplicationModal
    GuidedValidationDialog::showValidationDialog(_appData);
    return true ;
}

bool QXmlEditApplication::showFunctionKeysInfo(const bool forceShow)
{
    bool isVisible = false;
    if(NULL == _functionKeysInfo) {
        Utils::TODO_THIS_RELEASE("fare");
        //_functionKeysInfo = new FunctionKeysInfo(NULL, _appData);
        _functionKeysInfo = new ShortcutInfo(NULL);

        //_functionKeysInfo->setWindowFlag( Qt::FramelessWindowHint, true);
        //_functionKeysInfo->setWindowFlags((Qt::WindowFlags)(Qt::Window | Qt::FramelessWindowHint));
        //_functionKeysInfo->setWindowFlags((Qt::WindowFlags)(Qt::Window | Qt::CustomizeWindowHint | Qt::BypassWindowManagerHint | Qt::WindowTitleHint));
        _functionKeysInfo->setWindowTitle(tr("Keyboard Shortcuts List"));
        connect(_functionKeysInfo, SIGNAL(actionRequested(const QString &)), this, SLOT(onShortcutActivated(const QString &)));
    } else {
        isVisible = _functionKeysInfo->isVisible() ;
    }
    // move to the bottom of the screen
    if(!isVisible || forceShow) {
        QRect screenGeometry = QApplication::desktop()->screenGeometry();
        int left = (screenGeometry.width() - _functionKeysInfo->width()) >> 1;
        int top = screenGeometry.height() - _functionKeysInfo->height();
        _functionKeysInfo->move(left, top);
        _functionKeysInfo->show();
        _functionKeysInfo->raise();
    } else {
        _functionKeysInfo->hide();
    }
    _appData->newStateKeyboardInfo(!isVisible);
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
    bindCommandOperation(isConnect, target, SIGNAL(triggerQuit()), SLOT(onCommandQuit()));
    bindCommandOperation(isConnect, target, SIGNAL(triggerOpen()), SLOT(onCommandOpen()));
    bindCommandOperation(isConnect, target, SIGNAL(triggerValidate()), SLOT(onCommandValidate()));
    bindCommandOperation(isConnect, target, SIGNAL(triggerFormatting()), SLOT(onCommandFormatting()));
    bindCommandOperation(isConnect, target, SIGNAL(triggerCfgVision()), SLOT(onCommandConfigureVision()));
    bindCommandOperation(isConnect, target, SIGNAL(triggerCfgEditing()), SLOT(onCommandConfigureEditing()));
    bindCommandOperation(isConnect, target, SIGNAL(triggerConfigure()), SLOT(onCommandConfigure()));
    bindCommandOperation(isConnect, target, SIGNAL(triggerUserProfile()), SLOT(onCommandUserProfile()));
    bindCommandOperation(isConnect, target, SIGNAL(triggerExtractFile()), SLOT(onCommandExtractFile()));
    bindCommandOperation(isConnect, target, SIGNAL(triggerViewXSD()), SLOT(onCommandViewXSD()));
    bindCommandOperation(isConnect, target, SIGNAL(triggerUserManual()), SLOT(onCommandUserManual()));
    //--
    bindCommandOperation(isConnect, target, SIGNAL(triggerCompare()), SLOT(onCommandCompare()));
    bindCommandOperation(isConnect, target, SIGNAL(triggerAnon()), SLOT(onCommandAnon()));
    bindCommandOperation(isConnect, target, SIGNAL(triggerEditingShortcut()), SLOT(onCommandEditingShortcut()));
    bindCommandOperation(isConnect, target, SIGNAL(triggerBase64()), SLOT(onCommandBase64()));
    bindCommandOperation(isConnect, target, SIGNAL(triggerViewXMLMap()), SLOT(onCommandViewXMLMap()));
    bindCommandOperation(isConnect, target, SIGNAL(openFile(const QString &)), SLOT(onCommandOpenFile(const QString &)));
}


void QXmlEditApplication::setupFirstAccessForPreferences()
{
    if(!_appData->testAndMarkFirstAccessForViewPreferences()) {
        _appData->chooseVisualDetail();
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

void QXmlEditApplication::onOpenUserTypePanel(const bool nextAccess)
{
    showUserTypePanel(nextAccess);
    if(_appData->isUserGuidedOperation()) {
        showGuidedOperationsPanel();
    }
}

UIDelegate *QXmlEditApplication::uiDelegate()
{
    return _uiDelegate ;
}

void QXmlEditApplication::onWindowActivated(MainWindow * window, bool how)
{
    if(NULL != _functionKeysInfo) {
        if(how) {
            showFunctionKeysInfo(how);
            _functionKeysInfo->setTarget(window);
        }
    }
}

void QXmlEditApplication::onShortcutActivated(const QString & actionName)
{
    _appData->activateShortcut(actionName);
}

void QXmlEditApplication::onKeyboardShortcutOpenCloseRequest()
{
    showFunctionKeysInfo();
}

void QXmlEditApplication::onRequestEnableKeys(MainWindow * window)
{
    if(_functionKeysInfo->isVisible()) {
        _functionKeysInfo->setTarget(window);
    }
}
