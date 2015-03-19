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


#include "session.h"
#include "sessionprivate.h"
#include "sessionprivatedefault.h"

Session::Session(const bool isDefault, QObject *parent) :
    QObject(parent),
    d(isDefault ? new Session::PrivateDefault(this) : new Session::Private(this))
{
}

Session::~Session()
{
    if(d != NULL) {
        delete d;
    }
}

void Session::setLogger(FrwLogger *newLogger)
{
    d->setLogger(newLogger);
}


int Session::id()
{
    return d->id();
}

Session::SessionState Session::state()
{
    return d->state();
}

void Session::setState(SessionState newState)
{
    d->setState(newState);
}

void Session::setId(const int newId)
{
    d->setId(newId);
}

QString Session::description()
{
    return d->description();
}

QString Session::name()
{
    return d->name();
}

bool Session::enabled()
{
    return d->enabled();
}

void Session::setEnabled(const bool newEnabled)
{
    d->setEnabled(newEnabled);
}

void Session::readFromModel(SessionModel *model)
{
    d->readFromModel(model);
}

SessionModel *Session::model()
{
    return d->model();
}

bool Session::enrollFile(SessionDataInterface *access, const QString &filePath)
{
    return d->enrollFile(access, filePath);
}

bool Session::read(SessionDataInterface *access, const int idSession)
{
    return d->read(access, idSession);
}

SessionSummary* Session::getSummary(SessionOperationStatus &context)
{
    return d->getSummary(context);
}

bool Session::touch(SessionDataInterface *dataAccess)
{
    return d->touch(dataAccess);
}

bool Session::isDefaultSession() const
{
    return d->isDefaultSession();
}

void Session::setDefaultData(QStringList files)
{
    if(d->isDefaultSession()) {
        static_cast<Session::PrivateDefault*>(d)->setDefaultData(files);
    }
}
