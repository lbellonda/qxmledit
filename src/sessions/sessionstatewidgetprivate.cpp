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

#include "sessionstatewidgetprivate.h"
#include "sessionstatewidget.h"
#include "ui_sessionstatewidget.h"

#define RES_ACTIVE_ICON ":/images16/session_open"
#define RES_PAUSED_ICON ":/images16/session_paused"
#define RES_CLOSED_ICON ":/images16/session_closed"

SessionStateWidgetPrivate::SessionStateWidgetPrivate(SessionStateWidget *theWidget, QObject *parent) :
    QObject(parent),
    p(theWidget)
{
    _sessionManager = NULL ;
    _state = Session::NoSession;

}

SessionStateWidgetPrivate::~SessionStateWidgetPrivate()
{
}

void SessionStateWidgetPrivate::setSessionManager(SessionManager* value)
{
    if(_sessionManager != NULL) {
        disconnect(_sessionManager, SIGNAL(sessionStateChanged(Session::SessionState)), this, SLOT(onSessionStateChanged(Session::SessionState)));
    }
    _sessionManager = value ;
    if(_sessionManager != NULL) {
        connect(_sessionManager, SIGNAL(sessionStateChanged(Session::SessionState)), this, SLOT(onSessionStateChanged(Session::SessionState)));
        connect(_sessionManager, SIGNAL(enablingChanged()), this, SLOT(onEnablingChanged()));
        onSessionStateChanged(_sessionManager->state());
        onEnablingChanged();
    } else {
        onEnablingChanged();
    }
}

void SessionStateWidgetPrivate::onEnablingChanged()
{
    if(NULL == _sessionManager) {
        p->setVisible(false);
        return ;
    }
    p->setVisible(_sessionManager->isEnabled());
}

void SessionStateWidgetPrivate::onSessionStateChanged(Session::SessionState newState)
{
    QString sessionName ;
    QString sessionDescription;
    bool isDefaultSession = false;
    if(NULL != _sessionManager) {
        Session *session = _sessionManager->currentSession();
        isDefaultSession = _sessionManager->isDefaultSession();
        if(NULL != session) {
            sessionName = session->name();
            sessionDescription = session->description();
        }
    }
    _state = newState;
    switch(_state) {
    case Session::NoSession: {
        QPixmap noSessionIcon;
        noSessionIcon.load(RES_CLOSED_ICON);
        p->setToolTip(tr("No session."));
        p->ui->stateLabel->setPixmap(noSessionIcon);
        p->setVisible(true);
        break;
    }
    case Session::Active: {
        QPixmap activeIcon;
        activeIcon.load(RES_ACTIVE_ICON);
        if(isDefaultSession) {
            p->setToolTip(tr("Default data management active."));
        } else {
            p->setToolTip(tr("Session '%1' active.\n%2").arg(sessionName).arg(sessionDescription));
        }
        p->ui->stateLabel->setPixmap(activeIcon);
        p->setVisible(true);
        break;
    }
    case Session::Paused: {
        QPixmap pausedIcon;
        pausedIcon.load(RES_PAUSED_ICON);
        if(isDefaultSession) {
            p->setToolTip(tr("Default data management paused."));
        } else {
            p->setToolTip(tr("Session '%1' paused.\n%2").arg(sessionName).arg(sessionDescription));
        }
        p->ui->stateLabel->setPixmap(pausedIcon);
        p->setVisible(true);
        break;
    }
    default:
        p->setToolTip(tr("State unknown"));
        p->setVisible(false);
        break;
    }
}
