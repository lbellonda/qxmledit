/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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


#ifndef SESSION_H
#define SESSION_H

#include <QObject>
#include "QXmlEditSessions_global.h"
#include <sessiondata>
#include <sessiondata>
#include "data_access/model/filemodel.h"
#include "log.h"

class QXMLEDITSESSIONSSHARED_EXPORT Session : public QObject
{
    Q_OBJECT
    class Private;
    class PrivateDefault;
    Private * const d;
    friend class Private;
public:
    explicit Session(const bool isDefault = false, QObject *parent = 0);
    ~Session();

    void setLogger(FrwLogger *logger);

    int id();
    void setId(const int newId);
    QString name();
    QString description();
    bool enabled();
    void setEnabled(const bool newEnabled);

    /** \brief: the state of a session
      */
    enum SessionState {
        NoSession,
        Active,
        Paused
    };

    SessionState state();
    void setState(SessionState newState);
    SessionModel *model();

    void readFromModel(SessionModel *model);

    bool enrollFile(SessionDataInterface *access, const QString &filePath);
    bool read(SessionDataInterface *access, const int idSession);

    SessionSummary* getSummary(SessionOperationStatus &context);
    bool touch(SessionDataInterface *dataAccess);

    bool isDefaultSession() const;
    void setDefaultData(QStringList files);

signals:
    void dataChanged();
public slots:

};

#endif // SESSION_H
