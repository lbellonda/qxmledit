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

#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QObject>

#include "QXmlEditSessions_global.h"
#include "session.h"
#include "sessionsummary.h"
#include "sessionoperationstatus.h"
#include "sessiondataaccess"
#include "UIDelegate.h"

class QXMLEDITSESSIONSSHARED_EXPORT SessionManager : public QObject
{
    Q_OBJECT

    class Private;
    Private * const p;
    friend class Private;
public:
    explicit SessionManager(QObject *parent = 0);
    ~SessionManager();

    void init(const QString &storageConfiguration);
    void end();
    bool isStarted();
    void setSessionDataFactory(SessionDataFactory *factory);
    void setLogger(FrwLogger *logger);

    void setEnabled(const bool isEnabled);
    bool isEnabled();
    Session::SessionState state();
    Session *currentSession();

    bool setActiveSession(const int idSession, const Session::SessionState sessionState);
    bool setDefaultSession(const QStringList lastFiles);
    bool isDefaultSession();

    /** \brief: session operations
      */
    Session *newSession(const QString &sessionName);
    void pauseSession();
    void resumeSession();
    void closeSession();

    /** \brief: enrolles the file \a filePath with the active session (if any and running)
      \returns true if no error.
      */
    bool enrollFile(const QString &filePath);

    SessionSummary* getSummary(SessionOperationStatus &context);

    void manageSessions(QWidget *parent, UIDelegate *uiDelegate, QStringList lastFiles);

    void editCurrentSession(QWidget *parent, UIDelegate *uiDelegate);

    SessionDataInterface *dataAccess();


signals:
    void sessionStateChanged(Session::SessionState state);
    void clearSession();
    void dataChanged();
    void enablingChanged();
    void storageError(const QString &errorMessage);
    void editFile(const QString &filePath);
    void sessionActivated(const int idSession);

public slots:

};

#endif // SESSIONMANAGER_H
