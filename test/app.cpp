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


#include "app.h"
#include "testhelpers/testmainwindow.h"

App::App() : QObject(NULL)
{
    _mainWindow = NULL ;
    _useTestWindow = false;
    _currentDelegate = NULL ;
}

App::~App()
{
    if(_mainWindow != NULL) {
        _mainWindow->forgetData();
        delete _mainWindow ;
        _mainWindow = NULL ;
    }
    Config::setBackend(NULL);
}


bool App::useTestWindow() const
{
    return _useTestWindow;
}

void App::setUseTestWindow(bool value)
{
    _useTestWindow = value;
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
    TestMainWindow *testWindow = NULL ;
    if(_useTestWindow) {
        testWindow = new TestMainWindow(false, &appData);
        _mainWindow = testWindow ;
    } else {
        _mainWindow = new MainWindow(false, &appData);
    }
    if(NULL == _mainWindow) {
        return false;
    }
    if(delegateYes) {
        _mainWindow->setUIDelegate(&uiDelegateYes);
        _currentDelegate = &uiDelegateYes ;
    } else {
        _mainWindow->setUIDelegate(&uiDelegate);
        _currentDelegate = &uiDelegate ;
    }
    if( NULL != testWindow ) {
        testWindow->setFakeUIDelegate(_currentDelegate);
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

FakeUIDelegate *App::getCurrentUIDelegate()
{
    return _currentDelegate ;
}
