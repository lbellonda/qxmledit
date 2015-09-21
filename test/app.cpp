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


#include "app.h"

App::App()
{
    _mainWindow = NULL ;
}

App::~App()
{
    if(_mainWindow != NULL) {
        delete _mainWindow ;
        _mainWindow = NULL ;
    }
    Config::setBackend(NULL);
}

bool App::internalInit()
{
    Q_INIT_RESOURCE(risorse);

    if(!Config::init()) {
        return false;
    }
    Config::setBackend(&_configBackend);
    Config::saveBool(Config::KEY_SESSIONS_ENABLED, false);
    Config::saveBool(Config::KEY_GENERAL_OPEN_NEWWINDOW, false);
    if( !sessionDBPath.isEmpty()) {
        appData.setSessionDBPath(sessionDBPath);
    }
    appData.init();

    Element::loadIcons();
    return true;
}

bool App::init(const bool delegateYes)
{
    internalInit();
    _mainWindow = new MainWindow(false, qApp, &appData);
    if(NULL == _mainWindow) {
        return false;
    }
    if(delegateYes) {
        _mainWindow->setUIDelegate(&uiDelegateYes);
    } else {
        _mainWindow->setUIDelegate(&uiDelegate);
    }
    return true ;
}

bool App::initNoWindow()
{
    return internalInit();
}

bool App::init1()
{
    Q_INIT_RESOURCE(risorse);

    if(!Config::init()) {
        return false;
    }
    Config::setBackend(&_configBackend);
    Config::saveBool(Config::KEY_SESSIONS_ENABLED, false);
    Config::saveBool(Config::KEY_GENERAL_OPEN_NEWWINDOW, false);
    if( !sessionDBPath.isEmpty()) {
        appData.setSessionDBPath(sessionDBPath);
    }
    return true ;
}

bool App::init2()
{
    appData.init();

    Element::loadIcons();
    return true;
}

MainWindow *App::mainWindow()
{
    return _mainWindow;
}

ApplicationData* App::data()
{
    return &appData ;
}

FakeUIDelegate *App::getUiDelegate()
{
    return &uiDelegate;
}

FakeUIDelegate *App::getUiDelegateYes()
{
    return &uiDelegateYes;
}
