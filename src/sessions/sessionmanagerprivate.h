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


#ifndef QXMLEDIT_SESSIONMANAGERPRIVATE_H
#define QXMLEDIT_SESSIONMANAGERPRIVATE_H

#include <QObject>
#include "session.h"
#include "sessionmanager.h"
#include "SessionDataInterface.h"
#include "log.h"

class SessionManager::Private : public QObject
{
    Q_OBJECT

    bool _isStarted ;
    bool _isEnabled;
    Session *_currentSession;
    SessionManager* _manager;
    SessionDataInterface *_dataAccess;
    SessionDataFactory *_dataFactory;
    QString _storageConfiguration ;
    QStringList _lastFiles;
    FrwLogger *_logger;

    void refreshCurrentSessionData(UIDelegate *uiDelegate);
    void activateSession(const Session::SessionState newState = Session::Active);

public:
    explicit Private(SessionManager* manager, QObject *parent = 0);
    ~Private();

    void init(const QString &storageConfiguration);
    void end();
    bool isStarted();
    void setLogger(FrwLogger *logger);

    void setSessionDataFactory(SessionDataFactory *factory);

    void setEnabled(const bool isEnabled);
    bool isEnabled();
    bool isDefaultSession();

    Session::SessionState state();
    Session *currentSession();

    bool setActiveSession(const int idSession, const Session::SessionState sessionState);
    bool setDefaultSession(const QStringList lastFiles);

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

    /**\brief: registers the access
      */
    bool touch();

    SessionDataInterface *dataAccess();

signals:

public slots:
    void onSessionDeleted(const int sessionCode);

};

#endif // QXMLEDIT_SESSIONMANAGERPRIVATE_H
