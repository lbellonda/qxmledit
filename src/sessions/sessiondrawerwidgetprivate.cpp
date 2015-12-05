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


#include "sessiondrawerwidgetprivate.h"
#include "sessiondrawerwidget.h"
#include "ui_sessiondrawerwidget.h"

SessionDrawerWidgetPrivate::SessionDrawerWidgetPrivate(SessionDrawerWidget *theWidget, QObject *parent) :
    QObject(parent),
    _filterModel(this),
    p(theWidget),
    _dataModel(NULL)
{
    _sessionManager = NULL ;
}

SessionDrawerWidgetPrivate::~SessionDrawerWidgetPrivate()
{
    disconnectSessionManager();
    if(NULL != _dataModel) {
        _dataModel->deleteData();
        delete _dataModel;
    }
}

void SessionDrawerWidgetPrivate::init()
{
    p->ui->mainTree->setSortingEnabled(false);
    connect(p->ui->mainTree, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(treeItemDoubleClicked(const QModelIndex &)));
    connect(p->ui->searchBox, SIGNAL(textChanged(const QString &)), this, SLOT(newSearchText(const QString &)));
    connect(p->ui->clearButton, SIGNAL(clicked()), this, SLOT(clearSearchText()));
    connect(p->ui->moreCmd, SIGNAL(clicked()), this, SLOT(onMoreCmd()));
    connect(p->ui->createSession, SIGNAL(clicked()), this, SLOT(onCreateSession()));
    connect(p->ui->cmdSearch, SIGNAL(clicked()), this, SLOT(onSearchChanged()));
    setNoSessionWidgetVisible(true);
    p->ui->mainTree->setTextElideMode(Qt::ElideMiddle);
    onEnablingChanged();
}

void SessionDrawerWidgetPrivate::disconnectSessionManager()
{
    if(NULL != _sessionManager) {
        disconnect(_sessionManager, SIGNAL(dataChanged()), this, SLOT(onSessionDataChanged()));
        disconnect(_sessionManager, SIGNAL(enablingChanged()), this, SLOT(onEnablingChanged()));
    }
}

void SessionDrawerWidgetPrivate::setSessionManager(SessionManager *value)
{
    disconnectSessionManager();
    _sessionManager = value ;
    if(NULL != _sessionManager) {
        connect(_sessionManager, SIGNAL(sessionStateChanged(Session::SessionState)), this, SLOT(onSessionStateChanged(Session::SessionState)));
        connect(_sessionManager, SIGNAL(dataChanged()), this, SLOT(onSessionDataChanged()));
        connect(_sessionManager, SIGNAL(enablingChanged()), this, SLOT(onEnablingChanged()));
        onEnablingChanged();
        onSessionDataChanged();
    }
}

void SessionDrawerWidgetPrivate::onSessionStateChanged(Session::SessionState)
{
    onEnablingChanged();
}

void SessionDrawerWidgetPrivate::onSearchChanged()
{
    onEnablingChanged();
}

void SessionDrawerWidgetPrivate::onEnablingChanged()
{
    bool areSessionControlsVisible = false ;
    bool isSessionEnabled = false ;
    bool isSessionDefault = false;
    if(NULL != _sessionManager) {
        areSessionControlsVisible = _sessionManager->isEnabled();
        isSessionEnabled = _sessionManager->isEnabled();
        if(_sessionManager->state() == Session::NoSession) {
            areSessionControlsVisible = false ;
        }
        isSessionDefault = isSessionEnabled && _sessionManager->isDefaultSession();
    }
    p->setEnabled(isSessionEnabled);
    setNoSessionWidgetVisible(!areSessionControlsVisible || isSessionDefault);
    p->ui->searchFrame->setVisible(p->ui->cmdSearch->isChecked());
}

void SessionDrawerWidgetPrivate::treeItemDoubleClicked(const QModelIndex & index)
{
    if(NULL != _dataModel) {
        BaseSessionDataInfo *element = _dataModel->getGenericItem(_filterModel.mapToSource(index));
        if(NULL != element) {
            if(element->type() == BaseSessionDataInfo::TYPE_FILE) {
                SessionFileAccess *fileAccess = static_cast<SessionFileAccess*>(element);
                emit p->fileLoadRequest(fileAccess->path()) ;
            } else  if(element->type() == BaseSessionDataInfo::TYPE_FOLDER) {
                SessionDirectoryAccess *folderAccess = static_cast<SessionDirectoryAccess*>(element);
                emit p->folderOpenRequest(folderAccess->path()) ;
            }
        }
    }
}

void SessionDrawerWidgetPrivate::onSessionDataChanged()
{
    if(NULL == _sessionManager) {
        setNewModel(NULL);
        return ;
    }
    setNoSessionWidgetVisible((_sessionManager->state() == Session::NoSession) || _sessionManager->isDefaultSession());
    SessionDataModel *newModel = new SessionDataModel();
    if(NULL != newModel) {
        SessionOperationStatus context;
        SessionSummary *newData = _sessionManager->getSummary(context);
        if(context.ok) {
            newModel->setData(newData);
        } else {
            if(NULL != newData) {
                delete newData;
            }
        }
        newModel->setFont(p->font()) ;
    }
    setNewModel(newModel);
}

void SessionDrawerWidgetPrivate::setNewModel(SessionDataModel *newModel)
{
    if(_dataModel != NULL) {
        p->ui->mainTree->setModel(NULL);
        _dataModel->deleteData();
        delete _dataModel ;
    }
    _dataModel = newModel;
    updateModel();
}

void SessionDrawerWidgetPrivate::updateModel()
{
    _filterModel.setFilter(_filter);
    _filterModel.setSourceModel(_dataModel);
    p->ui->mainTree->setModel(&_filterModel);
    p->ui->mainTree->expandAll();
}

void SessionDrawerWidgetPrivate::clearSearchText()
{
    p->ui->searchBox->setText("");
    _filter = "" ;
    updateModel();
}

void SessionDrawerWidgetPrivate::newSearchText(const QString &newText)
{
    p->ui->mainTree->setUpdatesEnabled(false);
    _filter = newText ;
    updateModel();
    p->ui->mainTree->setUpdatesEnabled(true);
}

void SessionDrawerWidgetPrivate::onMoreCmd()
{
    emit p->showSessionManagement();
}

void SessionDrawerWidgetPrivate::onCreateSession()
{
    emit p->createSession();
}

void SessionDrawerWidgetPrivate::setNoSessionWidgetVisible(const bool value)
{
    if(value) {
        p->ui->invitation->setVisible(true);
    } else {
        p->ui->invitation->setVisible(false);
    }
}
