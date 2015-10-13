/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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


#ifndef ANOTIFIER_H
#define ANOTIFIER_H

#include "libQXmlEdit_global.h"

#include "framework/include/Notifier.h"
#include <QSystemTrayIcon>
#include <QMainWindow>

class UIDelegate;

class LIBQXMLEDITSHARED_EXPORT MainMenuBlock : public QObject
{
    Q_OBJECT
public:
    QMenu *_contextMenu ;
    QAction *_newWindowAction ;
    QAction *_encodingToolsAction ;
    QAction *_codePageToolsAction;
    QAction *_sessionAction;
    QAction *_viewMapAction;
    QAction *_splitFileAction;
    QAction *_raiseWindows;

public:
    MainMenuBlock(QObject *parent = NULL);
    ~MainMenuBlock();

    void setup();
};

class LIBQXMLEDITSHARED_EXPORT ANotifier : public QObject, Notifier
{
    Q_OBJECT
    UIDelegate *_uiDelegate;
    bool _isEnabled;
    QSystemTrayIcon _trayIcon;
    MainMenuBlock _mainMenuBlock;

    void setup();
public:
    explicit ANotifier(QObject *parent = NULL);
    ~ANotifier();

    UIDelegate *setUIDelegate(UIDelegate *newUIDelegate);
    virtual void notify(QMainWindow *window, const QString &msg);

    bool isEnabled();
    void enable(const bool how);

    void show();
    void hide();


private slots:
    void onActivated(QSystemTrayIcon::ActivationReason reason);
    void onMessageClicked();
    void onNewWindow();
    void onEncodingTools();
    void onCodePageTools();
    void onManageSessions();
    void onSplitFile();
    void onViewMapXml();
    void onRaiseWindow();

signals:
    void newWindowRequested();
    void codePageToolsRequested();
    void encodingToolsRequested();
    void sessionRequested();
    void splitFileRequested();
    void viewMapRequested();
    void raiseWindowRequested();
};

#endif // ANOTIFIER_H
