/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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


#include "sessionprivatedefault.h"

Session::PrivateDefault::PrivateDefault(Session *p, QObject *parent) : Session::Private(p, parent)
{
    _model.id = 0 ;
    _instances = -1;
}

Session::PrivateDefault::~PrivateDefault()
{
}

FileModel *Session::PrivateDefault::newFileModel(const QString &path)
{
    FileModel *model = new FileModel();
    _instances -- ;
    model->id = _instances ;
    model->path = path ;
    model->description = "";
    // lemodel->creationDate
    model->starred = 0;
    return model;
}

AccessModel *Session::PrivateDefault::newAccessModel(FileModel *file, const QDateTime &accessDate)
{
    AccessModel *accessModel = new AccessModel();
    accessModel->fileModel = file ;
    accessModel->accessDate = accessDate;
    return accessModel ;
}

bool Session::PrivateDefault::enrollFile(SessionDataInterface * /*access*/, const QString &filePath)
{
    if(enabled()) {
        // insert file
        FileModel *fileModel = NULL ;
        foreach(FileModel * file, _model.files) {
            if(file->path == filePath) {
                fileModel = file ;
                break;
            }
        }
        if(NULL == fileModel) {
            fileModel = newFileModel(filePath);
            _model.files.append(fileModel);
        }
        // insert access
        AccessModel *accessModel = newAccessModel(fileModel, QDateTime::currentDateTime());
        fileModel->accesses.append(accessModel);
        _model.accesses.append(accessModel);
        return true;
    }
    return false;
}

bool Session::PrivateDefault::read(SessionDataInterface * /*access*/, const int /*idSession*/)
{
    return true;
}


void Session::PrivateDefault::setDefaultData(QStringList files)
{
    _model.clear();
    _model.id = -1 ;
    _model.name = tr("");
    _model.description = tr("");
    _model.creationDate = QDateTime::currentDateTime();
    _model.updateDate = QDateTime::currentDateTime();
    _model.lastAccess = QDateTime::currentDateTime();
    _model.enabled = true ;
    _model.starred = 0;

    // sort access data by reverse order to have most recent first
    QDateTime lastAccess = QDateTime::currentDateTime();
    foreach(QString file, files) {
        FileModel *fileModel = newFileModel(file);
        _model.files.append(fileModel);
        AccessModel *accessModel = newAccessModel(fileModel, lastAccess);
        lastAccess = lastAccess.addSecs(-1000);
        _model.accesses.append(accessModel);
        fileModel->accesses.append(accessModel);
    }
}

bool Session::PrivateDefault::touch(SessionDataInterface * /*dataAccess*/)
{
    _model.lastAccess = QDateTime::currentDateTime();
    return true;
}

bool Session::PrivateDefault::isDefaultSession() const
{
    return true;
}

