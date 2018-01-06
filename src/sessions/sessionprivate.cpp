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


#include "sessionprivate.h"
#include <QFileInfo>

const int Session::Private::MaxDataCount = 8 ;

Session::Private::Private(Session *newp, QObject *parent) :
    QObject(parent),
    p(newp)
{
    _model.id = 0 ;
    _state = NoSession;
    _enabled = true ;
    _logger = NULL ;
}

Session::Private::~Private()
{
}

void Session::Private::setLogger(FrwLogger *newLogger)
{
    _logger = newLogger;
}

int Session::Private::id()
{
    return _model.id ;
}

QString Session::Private::name()
{
    return _model.name;
}

QString Session::Private::description()
{
    return _model.description;
}

Session::SessionState Session::Private::state()
{
    return _state;
}

void Session::Private::setState(SessionState newState)
{
    if(!enabled() && (newState != Session::NoSession)) {
        return ;
    }
    switch(newState) {
    case NoSession:
    case Active:
    case Paused:
        _state = newState;
        break;
    default:
        break;
    }

}


void Session::Private::setId(const int newId)
{
    _model.id = newId;
}
/*
void Session::Private::setName(const QString &sessionName)
{
    _name = sessionName;
}

void Session::Private::setDescription(const QString &sessionDescription)
{
    _description = sessionDescription;
}
*/
bool Session::Private::enabled()
{
    return _enabled;
}

void Session::Private::setEnabled(const bool newEnabled)
{
    _enabled = newEnabled;
}

void Session::Private::readFromModel(SessionModel *model)
{
    Q_ASSERT(NULL != model);
    //setName(model->name);
    setId(model->id);
    //setDescription(model->description);
    setEnabled(model->enabled);
}


SessionModel *Session::Private::model()
{
    return &_model;
}

bool Session::Private::enrollFile(SessionDataInterface *access, const QString &filePath)
{
    if(enabled()) {
        SessionOperationStatus context;
        SessionModel model;
        model.id = id();
        bool result = access->enrollFile(context, &model, filePath);
        return result && context.ok;
    }
    return false;
}

bool Session::Private::read(SessionDataInterface *access, const int idSession)
{
    SessionOperationStatus context;
    _model.clear();
    _model.id = idSession ;
    if(access->readSession(context, &_model)) {
        if(access->readSessionData(context, &_model)) {
            return true ;
        }
    }
    return false ;
}

bool caseInsensitiveLessThan(const QString &s1, const QString &s2)
{
    return s1.toLower() < s2.toLower();
}

static bool reverseCompareByDateFunction(const AccessModel *access1, const AccessModel *access2)
{
    return access1->accessDate >= access2->accessDate ;
}

static bool reverseCompareByCountFunction(const FileModel *file1, const FileModel *file2)
{
    return file1->accesses.size() >= file2->accesses.size() ;
}

SessionFileAccess * Session::Private::SessionFileAccessFromModel(FileModel *fileModel)
{
    SessionFileAccess *fileAccess = new SessionFileAccess();
    if(NULL != fileAccess) {
        fileAccess->setPath(fileModel->path);
        if(fileModel->accesses.size() > 0) {
            fileAccess->setLastAccess(fileModel->accesses.at(0)->accessDate);
        }
        fileAccess->setTimes(fileModel->accesses.size());
    }
    return fileAccess  ;
}


bool Session::Private::isDefaultSession() const
{
    return false;
}

SessionDirectoryAccess * Session::Private::SessionDirectoryAccessFromModel(FileModel *fileModel, const QString &path)
{
    SessionDirectoryAccess *access = new SessionDirectoryAccess();
    if(NULL != access) {
        access->setPath(path);
        if(fileModel->accesses.size() > 0) {
            access->setLastAccess(fileModel->accesses.at(0)->accessDate);
        }
    }
    return access  ;
}


//TODO: manca il calcolo
SessionSummary* Session::Private::getSummary(SessionOperationStatus &context)
{
    context.ok = false;
    SessionSummary *summary = new SessionSummary();
    if(NULL != summary) {
        context.ok = true;

        QVector<FileModel *>tempFiles;
        foreach(FileModel * file, _model.files) {
            tempFiles.append(file);
        }
        QVector<AccessModel *>tempAccess;
        foreach(AccessModel * access, _model.accesses) {
            tempAccess.append(access);
        }
        // sort by date
        qSort(tempAccess.begin(), tempAccess.end(), reverseCompareByDateFunction);
        // sort by count
        qSort(tempFiles.begin(), tempFiles.end(), reverseCompareByCountFunction);

        int index = 0 ;
        index = 0 ;
        foreach(FileModel * file, tempFiles) {
            if(index >= MaxDataCount) {
                break;
            }
            SessionFileAccess *fileAccess = SessionFileAccessFromModel(file);
            if(NULL != fileAccess) {
                summary->addMostUsedFile(fileAccess);
            }
            index ++ ;
        }

        int fileIndex = 0 ;
        int folderIndex = 0 ;
        bool doneFiles = false;
        bool doneFolders = false;
        QSet<int> fileSet;
        QSet<QString> folderSet;
        foreach(AccessModel * access, tempAccess) {

            if(doneFiles && doneFolders) {
                break;
            }

            if(fileIndex < MaxDataCount) {
                if(!fileSet.contains(access->fileModel->id)) {
                    SessionFileAccess *fileAccess = SessionFileAccessFromModel(access->fileModel);
                    if(NULL != fileAccess) {
                        summary->addRecentFile(fileAccess);
                    }
                    fileSet.insert(access->fileModel->id);
                    fileIndex ++ ;
                }
            } else {
                doneFiles = true ;
            }

            if(folderIndex < MaxDataCount) {
                QFileInfo fileInfo(access->fileModel->path);
                QString folderPath = fileInfo.path();
                if(!folderSet.contains(folderPath)) {
                    SessionDirectoryAccess *folderAccess = SessionDirectoryAccessFromModel(access->fileModel, folderPath);
                    if(NULL != folderAccess) {
                        summary->addMostRecentFolder(folderAccess);
                    }
                    folderSet.insert(folderPath);
                    folderIndex ++ ;
                }
            } else {
                doneFolders = true ;
            }
        }
    }
    if(!context.ok) {
        if(NULL != summary) {
            delete summary ;
            summary = NULL ;
        }
    }
    return summary;
}

bool Session::Private::touch(SessionDataInterface *dataAccess)
{
    SessionOperationStatus context;
    bool isOk = dataAccess->touchSession(context, &_model);
    if(_logger != NULL) {
        if(isOk) {
            _logger->info(QString("SessionManager::Private::touch(%1) ok ").arg(_model.id));
        } else {
            _logger->error(QString("SessionManager::Private::touch(%1) KO ").arg(_model.id));
        }
    }
    return isOk ;
}

