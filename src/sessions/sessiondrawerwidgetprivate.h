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


#ifndef SESSIONDRAWERWIDGETPRIVATE_H
#define SESSIONDRAWERWIDGETPRIVATE_H

#include <QObject>

#include "sessiondrawerwidget"
#include "sessiondatamodel.h"
#include "sessionfiltermodel.h"

class SessionDrawerWidgetPrivate : public QObject
{
    Q_OBJECT

    QString _filter;
    SessionFilterModel _filterModel;
    SessionDrawerWidget *p;
    SessionManager  *_sessionManager;
    SessionDataModel *_dataModel;

    void disconnectSessionManager();
    void setNewModel(SessionDataModel *newModel);
    void updateModel();
    void setNoSessionWidgetVisible(const bool value);


public:
    explicit SessionDrawerWidgetPrivate(SessionDrawerWidget *newp, QObject *parent = 0);
    virtual ~SessionDrawerWidgetPrivate();
    void init();

    void setSessionManager(SessionManager *value);
signals:

public slots:
    void onSessionDataChanged();
    void onEnablingChanged();
    void treeItemDoubleClicked(const QModelIndex & index);
    void clearSearchText();
    void newSearchText(const QString &newText);
    void onMoreCmd();
    void onSessionStateChanged(Session::SessionState);
    void onCreateSession();
    void onSearchChanged();
};

#endif // SESSIONDRAWERWIDGETPRIVATE_H
