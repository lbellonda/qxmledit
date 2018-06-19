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


#ifndef APP_H
#define APP_H

#include <QObject>

#include "qxmleditconfig.h"
#include "mainwindow.h"
#include "utils.h"
#include "fakeuidelegate.h"

class App : public QObject
{
    Q_OBJECT

    MainWindow *_mainWindow;
    ApplicationData appData;
    QMap<QString, QVariant> _configBackend;
    FakeUIDelegate uiDelegate;
    FakeUIDelegateYes uiDelegateYes;
    bool _useTestWindow;
    FakeUIDelegate *_currentDelegate;

    bool internalInit();

public:
    QString sessionDBPath;

    App();
    virtual ~App();

    bool init(const bool delegateYes=false);
    bool initNoWindow();
    bool init1();
    bool init2();

    MainWindow *mainWindow();
    ApplicationData* data();

    FakeUIDelegate *getCurrentUIDelegate();
    FakeUIDelegate *getUiDelegate();
    FakeUIDelegate *getUiDelegateYes();
    bool useTestWindow() const;
    void setUseTestWindow(bool useTestWindow);
};

#endif // APP_H
