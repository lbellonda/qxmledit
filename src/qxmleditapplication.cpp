/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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
#include "StartParams.h"
#include "utils.h"
#include <QDataStream>
#include <QLocalSocket>

void extractFragmentsWindow(ExtractResults *extractResult, QWidget *parent);

const QString QXmlEditApplication::ServerName("__qxmledit__server__");


QXmlEditApplication::QXmlEditApplication(int &argc, char **argv) :
    QApplication(argc, argv)
{
    _server = NULL ;
    _logger = NULL ;
    Utils::TODO_THIS_RELEASE("logger to error");
}

QXmlEditApplication::~QXmlEditApplication()
{
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
    _appData = value;
    if(NULL != _appData) {
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
    MainWindow * newWindow = new MainWindow(false, this, _appData);
    newWindow->setAutoDelete();
    return newWindow;
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
    extractFragmentsWindow(results, NULL);
}

void QXmlEditApplication::onViewData()
{
    DataVisualization::viewDataWindow(_appData, NULL, "");
}


void QXmlEditApplication::newServerConnection()
{
    Utils::TODO_THIS_RELEASE("finire");
    QLocalSocket *newInstanceConnection = _server->nextPendingConnection();
    if(NULL != newInstanceConnection) {
        connect(newInstanceConnection, SIGNAL(disconnected()), newInstanceConnection, SLOT(deleteLater()));
        QDataStream inputDataStream(newInstanceConnection);
        inputDataStream.setVersion(QDataStream::Qt_4_0);
        while(!inputDataStream.atEnd()) {
            if(!newInstanceConnection->waitForReadyRead()) {
                newInstanceConnection->disconnectFromServer();
                if(NULL != _logger) {
                    _logger->error(QString("Server::Disconnecting an incoming connection, cause:%1").arg(newInstanceConnection->errorString()));
                }
                return ;
            }
        }
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
            MainWindow *newWindow = makeNewWindow();
            // scan existing files
            foreach(MainWindow * wnd, _appData->windows()) {
                if(wnd->getRegola()->fileName() == params.fileName) {
                    wnd->show();
                    wnd->raise();
                    wnd->activateWindow();
                    if(NULL != _logger) {
                        _logger->debug(QString("Server::reusing window for:'%1'").arg(params.fileName));
                    }
                    return ;
                }
            }
            newWindow->loadFile(params.fileName);
            newWindow->show();
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
    if(connectToExistingServer(startParams)) {
        return true ;
    }
    startServer();
    return false;
}

bool QXmlEditApplication::startServer()
{
    _server = new QLocalServer();
    connect(_server, SIGNAL(newConnection()), this, SLOT(newServerConnection()));
    _server->listen(ServerName);
    if(NULL != _logger) {
        if(_logger->isEnabled() && _logger->isLoggable(FrwLogger::DEBUG))
            _logger->debug(QString("Server listening at:%1").arg(_server->fullServerName()));
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
            Utils::TODO_THIS_RELEASE("the lack of ack from the server can stop the file opening");
        }
        socket->abort();
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
    dataStream << (quint32)startParams->type;
    dataStream << startParams->fileName;
    dataStream << (bool)startParams->parametersError;
    return result;
}
