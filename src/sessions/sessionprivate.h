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


#ifndef SESSIONPRIVATE_H
#define SESSIONPRIVATE_H

#include "xmlEdit.h"
#include "session.h"

class Session::Private : public QObject
{
    Q_OBJECT
protected:
    Session * const p;

    static const int MaxDataCount ;

    Session::SessionState _state;
    bool _enabled ;
    SessionModel _model;
    FrwLogger *_logger;

    SessionDirectoryAccess * SessionDirectoryAccessFromModel(FileModel *fileModel, const QString &path);
    SessionFileAccess * SessionFileAccessFromModel(FileModel *fileModel);

public:
    explicit Private(Session *p, QObject *parent = 0);
    virtual ~Private();

    void setLogger(FrwLogger *logger);

    int id();
    QString name();
    QString description();
    Session::SessionState state();
    void setState(SessionState newState);
    bool enabled();

    void setId(const int newId);
    void setEnabled(const bool newEnabled);

    void readFromModel(SessionModel *model);
    SessionModel *model();

    virtual bool enrollFile(SessionDataInterface *access, const QString &filePath);
    virtual bool read(SessionDataInterface *access, const int idSession);

    SessionSummary* getSummary(SessionOperationStatus &context);
    virtual bool touch(SessionDataInterface *dataAccess);

    virtual bool isDefaultSession() const;

signals:

public slots:

};

#endif // SESSIONPRIVATE_H
