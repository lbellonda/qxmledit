/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2018 by Luca Bellonda and individual contributors  *
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

#include "sessionmanager.h"
#include "sessionmanagerprivate.h"

SessionManager::SessionManager(QObject *parent) : QObject(parent),
    p(new SessionManager::Private(this))
{
}

SessionManager::~SessionManager()
{
    if(NULL != p) {
        delete p;
    }
}

void SessionManager::init(const QString &storageConfiguration)
{
    p->init(storageConfiguration);
}

void SessionManager::end()
{
    p->end();
}

bool SessionManager::isStarted()
{
    if(NULL == p) {
        return false;
    }
    return p->isStarted();
}

void SessionManager::setEnabled(const bool isEnabled)
{
    p->setEnabled(isEnabled);
}

bool SessionManager::isEnabled()
{
    return p->isEnabled();
}

Session::SessionState SessionManager::state()
{
    return p->state();
}

Session *SessionManager::currentSession()
{
    return p->currentSession();
}

Session *SessionManager::newSession(const QString &sessionName)
{
    return p->newSession(sessionName);
}

void SessionManager::pauseSession()
{
    p->pauseSession();
}

void SessionManager::resumeSession()
{
    p->resumeSession();
}

void SessionManager::closeSession()
{
    p->closeSession();
}

bool SessionManager::enrollFile(const QString &filePath)
{
    return p->enrollFile(filePath);
}

SessionSummary* SessionManager::getSummary(SessionOperationStatus &context)
{
    return p->getSummary(context);
}

bool SessionManager::setActiveSession(const int idSession, const Session::SessionState sessionState)
{
    return p->setActiveSession(idSession, sessionState);
}

bool SessionManager::setDefaultSession(const QStringList lastFiles)
{
    return p->setDefaultSession(lastFiles);
}

bool SessionManager::isDefaultSession()
{
    return p->isDefaultSession();
}

void SessionManager::setSessionDataFactory(SessionDataFactory *factory)
{
    p->setSessionDataFactory(factory);
}

void SessionManager::setLogger(FrwLogger *newLogger)
{
    p->setLogger(newLogger);
}

void SessionManager::manageSessions(QWidget *parent, UIDelegate *uiDelegate, QStringList lastFiles)
{
    p->manageSessions(parent, uiDelegate, lastFiles);
}

void SessionManager::editCurrentSession(QWidget *parent, UIDelegate *uiDelegate)
{
    p->editCurrentSession(parent, uiDelegate);
}

SessionDataInterface *SessionManager::dataAccess()
{
    return p->dataAccess();
}
