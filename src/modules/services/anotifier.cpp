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

#include "anotifier.h"
#include <QStatusBar>
#include <QMenu>
#include "config.h"
#include "utils.h"

#define SHORT_TIMEOUT   5000

//-----
MainMenuBlock::MainMenuBlock(QObject *parent): QObject(parent)
{
    _contextMenu = NULL ;
    _newWindowAction = NULL ;
    _encodingToolsAction = NULL ;
    _codePageToolsAction = NULL ;
    _sessionAction = NULL ;
    _viewMapAction = NULL ;
    _splitFileAction = NULL ;
}

MainMenuBlock::~MainMenuBlock()
{
    if(NULL != _contextMenu) {
        delete _contextMenu ;
    }
}

void MainMenuBlock::setup()
{
    _contextMenu = new QMenu(NULL);
    _newWindowAction = new QAction(tr("&New Window"), this);
    _encodingToolsAction = new QAction(tr("Encoding Tools"), this);
    _codePageToolsAction = new QAction(tr("Code Pages"), this);
    _sessionAction = new QAction(tr("Manage Sessions"), this);
    _viewMapAction = new QAction(tr("View Data"), this);
    _splitFileAction = new QAction(tr("Extract Fragments from a File"), this);
    _contextMenu->addAction(_newWindowAction);
    _contextMenu->addSeparator();
    _contextMenu->addAction(_encodingToolsAction);
    _contextMenu->addAction(_codePageToolsAction);
    _contextMenu->addAction(_sessionAction);
    _contextMenu->addAction(_viewMapAction);
    _contextMenu->addAction(_splitFileAction);
}

//-----

ANotifier::ANotifier(QObject *parent) : QObject(parent)
{
    _uiDelegate = NULL ;
    _isEnabled = Config::getBool(Config::KEY_GENERAL_NOTIFICATIONS_ENABLED, Config::KEY_GENERAL_NOTIFICATIONS_ENABLED_DEFAULT_VALUE);
    setup();
}

ANotifier::~ANotifier()
{
    disconnect(&_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onActivated(QSystemTrayIcon::ActivationReason)));
    disconnect(&_trayIcon, SIGNAL(messageClicked()), this, SLOT(onMessageClicked()));


    if(NULL != _mainMenuBlock._newWindowAction) {
        disconnect(_mainMenuBlock._newWindowAction, SIGNAL(triggered()), this, SLOT(onNewWindow()));
    }
    if(NULL != _mainMenuBlock._encodingToolsAction) {
        disconnect(_mainMenuBlock._encodingToolsAction, SIGNAL(triggered()), this, SLOT(onEncodingTools()));
    }
    if(NULL != _mainMenuBlock._codePageToolsAction) {
        disconnect(_mainMenuBlock._codePageToolsAction, SIGNAL(triggered()), this, SLOT(onCodePageTools()));
    }
    if(NULL != _mainMenuBlock._sessionAction) {
        disconnect(_mainMenuBlock._sessionAction, SIGNAL(triggered()), this, SLOT(onManageSessions()));
    }
    if(NULL != _mainMenuBlock._viewMapAction) {
        disconnect(_mainMenuBlock._viewMapAction, SIGNAL(triggered()), this, SLOT(onViewMapXml()));
    }
    if(NULL != _mainMenuBlock._splitFileAction) {
        disconnect(_mainMenuBlock._splitFileAction, SIGNAL(triggered()), this, SLOT(onSplitFile()));
    }

    _trayIcon.setContextMenu(NULL);
}

void ANotifier::setup()
{
    connect(&_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onActivated(QSystemTrayIcon::ActivationReason)));
    connect(&_trayIcon, SIGNAL(messageClicked()), this, SLOT(onMessageClicked()));
    QIcon icon(":/icon/images/icon.png");
    _trayIcon.setIcon(icon);
    _trayIcon.setToolTip(tr("QXmlEdit"));
    // context menu
    _mainMenuBlock.setup();
    connect(_mainMenuBlock._newWindowAction, SIGNAL(triggered()), this, SLOT(onNewWindow()));
    connect(_mainMenuBlock._encodingToolsAction, SIGNAL(triggered()), this, SLOT(onEncodingTools()));
    connect(_mainMenuBlock._codePageToolsAction, SIGNAL(triggered()), this, SLOT(onCodePageTools()));
    connect(_mainMenuBlock._sessionAction, SIGNAL(triggered()), this, SLOT(onManageSessions()));
    connect(_mainMenuBlock._viewMapAction, SIGNAL(triggered()), this, SLOT(onViewMapXml()));
    connect(_mainMenuBlock._splitFileAction, SIGNAL(triggered()), this, SLOT(onSplitFile()));

    _trayIcon.setContextMenu(_mainMenuBlock._contextMenu);
    // end menu
}

void ANotifier::onActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason) {
    case QSystemTrayIcon::Unknown:
    case QSystemTrayIcon::Context:
    case QSystemTrayIcon::DoubleClick:
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::MiddleClick:
    default:
        break;
    }
}

void ANotifier::onMessageClicked()
{
    //
}


void ANotifier::notify(QMainWindow *window, const QString &msg)
{
    if(_isEnabled) {
        if(NULL != window) {
            QStatusBar *statusBar = window->statusBar();
            if(NULL != statusBar) {
                statusBar->showMessage(msg, SHORT_TIMEOUT);
            }
        }
        if(QSystemTrayIcon::isSystemTrayAvailable() && QSystemTrayIcon::supportsMessages()) {
            _trayIcon.showMessage("QXmlEdit", msg, QSystemTrayIcon::Information, SHORT_TIMEOUT);
        } else {
            Utils::message(window, msg);
        }
    } else {
        Utils::message(window, msg);
    }
}

bool ANotifier::isEnabled()
{
    return _isEnabled ;
}

void ANotifier::enable(const bool how)
{
    _isEnabled = how ;
    if(_isEnabled) {
        _trayIcon.show();
    } else {
        _trayIcon.hide();
    }
    Config::saveBool(Config::KEY_GENERAL_NOTIFICATIONS_ENABLED, _isEnabled);
}

void ANotifier::show()
{
    if(_isEnabled) {
        _trayIcon.show();
    }
}

void ANotifier::hide()
{
    _trayIcon.hide();
}

//------

void ANotifier::onNewWindow()
{
    emit newWindowRequested();
}

void ANotifier::onManageSessions()
{
    emit sessionRequested();
}

void ANotifier::onViewMapXml()
{
    emit viewMapRequested();
}

void ANotifier::onSplitFile()
{
    emit splitFileRequested();
}

void ANotifier::onEncodingTools()
{
    emit encodingToolsRequested();
}

void ANotifier::onCodePageTools()
{
    emit codePageToolsRequested();
}

