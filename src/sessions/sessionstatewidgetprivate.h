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


#ifndef SESSIONSTATEWIDGETPRIVATE_H
#define SESSIONSTATEWIDGETPRIVATE_H

#include "xmlEdit.h"
#include "sessionmanager"
#include "sessionstatewidget"

class SessionStateWidgetPrivate : public QObject
{
    Q_OBJECT
    SessionStateWidget *p;
    SessionManager* _sessionManager;
    Session::SessionState _state;
public:
    explicit SessionStateWidgetPrivate(SessionStateWidget *theWidget, QObject *parent = 0);
    ~SessionStateWidgetPrivate();

    void setSessionManager(SessionManager* value);

signals:

public slots:

private slots:
    void onSessionStateChanged(Session::SessionState newState);
    void onEnablingChanged();

};

#endif // SESSIONSTATEWIDGETPRIVATE_H
