/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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

#define CONSOLE_LOG

#include "sessionmanagerprivate.h"
#include "data_access/testdataaccess.h"
#include "sessionsmanagementdialog.h"
#include "sessiondetaildialog.h"

SessionManager::Private::Private(SessionManager* manager, QObject *parent) :
    QObject(parent),
    _manager(manager)
{
    _logger = NULL ;
    _isEnabled = false ;
    _currentSession = NULL ;
    _dataAccess = NULL ;
}

SessionManager::Private::~Private()
{
    // _dataAccess is owned by the external entity, so we do not close it.
    if(_currentSession != NULL) {
        delete _currentSession;
        _currentSession = NULL ;
    }
}

void SessionManager::Private::init(const QString &newStorageConfiguration)
{
    _isStarted = true ;
    _dataAccess = _dataFactory->sessionDataInterface(newStorageConfiguration);
    _storageConfiguration = newStorageConfiguration ;
}

void SessionManager::Private::end()
{
}

//-------------------------------------------------------------------------

void SessionManager::Private::setLogger(FrwLogger *newLogger)
{
    _logger = newLogger;
}

bool SessionManager::Private::isStarted()
{
    return _isStarted ;
}

void SessionManager::Private::setSessionDataFactory(SessionDataFactory *factory)
{
    _dataFactory = factory;
}

void SessionManager::Private::setEnabled(const bool value)
{
    bool startDB = false;
    if(!_isEnabled && value) {
        startDB = true ;
    }
    _isEnabled = value ;
    if(startDB && (NULL != _dataAccess)) {
        if(!_dataAccess->init(_storageConfiguration)) {
            emit _manager->storageError("Session storage could not be inited.");
        }
    }
    emit _manager->enablingChanged();
}

bool SessionManager::Private::isEnabled()
{
    return _isEnabled;
}

bool SessionManager::Private::isDefaultSession()
{
    if(NULL != currentSession()) {
        return currentSession()->isDefaultSession();
    }
    return false ;
}


Session::SessionState SessionManager::Private::state()
{
    if(NULL != _currentSession) {
        return _currentSession->state() ;
    }
    return Session::NoSession;
}

Session *SessionManager::Private::currentSession()
{
    return _currentSession;
}

Session *SessionManager::Private::newSession(const QString &sessionName)
{
    SessionOperationStatus context;
    closeSession();
    SessionModel model;
    model.name = sessionName ;
    model.description = "";
    model.enabled = true ;
    if(_dataAccess->newSession(context, &model)) {
        Session *session = new Session();
        if(NULL != session) {
            session->setLogger(_logger);
            session->readFromModel(&model);
            _currentSession = session ;
            activateSession();
        }
    }
    emit _manager->sessionStateChanged(state());
    emit _manager->dataChanged();
    return _currentSession;
}

void SessionManager::Private::pauseSession()
{
    if(_logger) {
        _logger->debug("SessionManager::Private::pauseSession()");
    }
    if(isEnabled() && (NULL != _currentSession) && (_currentSession->state() == Session::Active)) {
        _currentSession->setState(Session::Paused);
        emit _manager->sessionStateChanged(state());
    }
}

void SessionManager::Private::resumeSession()
{
    if(_logger) {
        _logger->debug("SessionManager::Private::resumeSession()");
    }
    if(isEnabled() && (NULL != _currentSession) && (_currentSession->state() == Session::Paused)) {
        activateSession();
        emit _manager->sessionStateChanged(state());
    }
}

void SessionManager::Private::closeSession()
{
    if(_logger) {
        _logger->debug("SessionManager::Private::closeSession()");
    }
    if(NULL != _currentSession) {
        delete _currentSession ;
        _currentSession = NULL ;
        emit _manager->dataChanged();
        emit _manager->sessionStateChanged(state());
    }
}

void SessionManager::Private::activateSession(const Session::SessionState newState)
{
    _currentSession->setState(newState);
    _currentSession->touch(_dataAccess);
}

bool SessionManager::Private::enrollFile(const QString &filePath)
{
    if(_logger) {
        _logger->debug("SessionManager::enrollFile(const QString filePath)");
    }
    if(isEnabled()) {
        if(NULL != _currentSession) {
            bool result = _currentSession ->enrollFile(_dataAccess, filePath);
            if(result) {
                _currentSession->read(_dataAccess, _currentSession ->id());
                emit _manager->dataChanged();
            }
            return result ;
        }
    }
    return true ;
}

SessionSummary* SessionManager::Private::getSummary(SessionOperationStatus &context)
{
    if(_logger) {
        _logger->debug("SessionManager::getSummaryData");
    }
    if(isEnabled() && (NULL != _currentSession)) {
        return _currentSession->getSummary(context);
    }
    return NULL ;
}

bool SessionManager::Private::setActiveSession(const int idSession, const Session::SessionState sessionState)
{
    if(_logger) {
        _logger->debug("SessionManager::setActiveSession");
    }
    closeSession();

    bool isOk = false ;
    Session *theNewSession = new Session();
    if(NULL != theNewSession) {
        theNewSession->setLogger(_logger);
        if(theNewSession->read(_dataAccess, idSession))  {
            _currentSession = theNewSession ;
            activateSession(sessionState);
            emit _manager->sessionActivated(idSession);
            isOk = true ;
        } else {
            delete theNewSession ;
        }
    }
    emit _manager->sessionStateChanged(state());
    emit _manager->dataChanged();
    return isOk;
}

bool SessionManager::Private::setDefaultSession(const QStringList lastFiles)
{
    if(_logger) {
        _logger->debug("SessionManager::setDefaultSession");
    }
    closeSession();

    bool isOk = false ;
    Session *theNewSession = new Session(true);
    if(NULL != theNewSession) {
        theNewSession->setLogger(_logger);
        theNewSession->setDefaultData(lastFiles);
        _currentSession = theNewSession ;
        activateSession(Session::Active);
        emit _manager->sessionActivated(_currentSession->id());
        isOk = true ;
    }
    emit _manager->sessionStateChanged(state());
    emit _manager->dataChanged();
    return isOk;
}


void SessionManager::Private::onSessionDeleted(const int sessionCode)
{
    if(NULL != _currentSession) {
        if(_currentSession->id() == sessionCode) {
            closeSession();
            emit _manager->clearSession();
            setDefaultSession(_lastFiles);
        }
    }
}

void SessionManager::Private::manageSessions(QWidget *parent, UIDelegate *uiDelegate, QStringList lastFiles)
{
    if(!isEnabled()) {
        return ;
    }
    _lastFiles = lastFiles ;
    SessionsManagementDialog managementDialog(parent, uiDelegate, _dataAccess);
    connect(&managementDialog, SIGNAL(sessionDeleted(const int)), this, SLOT(onSessionDeleted(const int)));
    managementDialog.exec();
    disconnect(&managementDialog, SIGNAL(sessionDeleted(const int)), this, SLOT(onSessionDeleted(const int)));
    refreshCurrentSessionData(uiDelegate);
    switch(managementDialog.userChoice()) {
    case SessionsManagementDialog::UC_ACTIVATESESSION:
        setActiveSession(managementDialog.sessionCode(), Session::Active);
        break;
    case SessionsManagementDialog::UC_LOADFILE:
        emit _manager->editFile(managementDialog.filePath());
        break;
    default:
        break;
    }
}

void SessionManager::Private::editCurrentSession(QWidget *parent, UIDelegate *uiDelegate)
{
    if(!isEnabled()) {
        return ;
    }
    if((_currentSession != NULL) && !_currentSession->isDefaultSession()) {
        SessionDetailDialog sessionDetailDialog(parent, uiDelegate, _dataAccess, _currentSession->model());
        sessionDetailDialog.exec();
        refreshCurrentSessionData(uiDelegate);
        switch(sessionDetailDialog.userChoice()) {
        case SessionDetailDialog::UC_LOADFILE:
            emit _manager->editFile(sessionDetailDialog.filePath());
            break;
        default:
            break;
        }
    }
}

void SessionManager::Private::refreshCurrentSessionData(UIDelegate *uiDelegate)
{
    if(NULL != _currentSession && !_currentSession->isDefaultSession()) {
        if(!_currentSession->read(_dataAccess, _currentSession ->id())) {
            uiDelegate->error(tr("unable to read sessions data."));
        }
        emit _manager->dataChanged();
    }
}

SessionDataInterface *SessionManager::Private::dataAccess()
{
    return _dataAccess;
}
