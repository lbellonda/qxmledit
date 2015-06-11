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


#ifndef SESSIONSTATEWIDGET_H
#define SESSIONSTATEWIDGET_H

#include "xmlEdit.h"

#include "QXmlEditSessions_global.h"

#include "sessionmanager"

namespace Ui
{
class SessionStateWidget;
}

class SessionStateWidgetPrivate;

class QXMLEDITSESSIONSSHARED_EXPORT SessionStateWidget : public QWidget
{
    Q_OBJECT

    SessionStateWidgetPrivate *d;
    friend class SessionStateWidgetPrivate;
public:
    explicit SessionStateWidget(QWidget *parent = 0);
    ~SessionStateWidget();
    /*
        state readonly
        Session " the current Session"
    */

    void setSessionManager(SessionManager* manager);

signals:
    void doubleClicked();
private:
    Ui::SessionStateWidget *ui;
    /* TODO: enable with context menu
    signals:
    void newSession();
    void pauseSession();
    void resumeSession();
    void stopSession();
    */
public slots:
    void mouseDoubleClickEvent(QMouseEvent *e);
};

#endif // SESSIONSTATEWIDGET_H
