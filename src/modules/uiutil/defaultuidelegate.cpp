/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2018 by Luca Bellonda and individual contributors       *
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

#include "defaultuidelegate.h"
#include "utils.h"

extern const char *APP_TITLE ;

//-----------------------------------------

DefaultUIDelegate::DefaultUIDelegate()
{
    _window = NULL ;
    _errorCount = 0 ;
    _errors = 0 ;
    _askCountBeforeLoad = 0 ;
    _askCountAfterLoad = 0 ;
    _beforeLoad = true ;
}

DefaultUIDelegate::~DefaultUIDelegate()
{
}

QWidget *DefaultUIDelegate::getWindow() const
{
    return _window;
}

void DefaultUIDelegate::setWindow(QWidget *window)
{
    _window = window;
}


void DefaultUIDelegate::registerError()
{
    _errors++;
}

void DefaultUIDelegate::error(const QString& message)
{
    _errorCount++;
    Utils::error(_window, message);
}

void DefaultUIDelegate::error(QWidget *parent, const QString& message)
{
    _errorCount++;
    Utils::error(parent, message);
}

void DefaultUIDelegate::warning(const QString& message)
{
    Utils::warning(_window, message);
}

void DefaultUIDelegate::message(const QString& message)
{
    Utils::message(_window, message);
}

bool DefaultUIDelegate::askYN(const QString & message)
{
    return Utils::askYN(_window, message);
}

bool DefaultUIDelegate::askYN(QWidget *parent, const QString & message)
{
    return Utils::askYN(parent, message);
}

QString DefaultUIDelegate::msgOutOfMem()
{
    _errorCount++;
    return Utils::msgOutOfMem();
}

void DefaultUIDelegate::errorNoSel(QWidget * parent)
{
    Utils::errorNoSel(parent);
    _errorCount++;
}

void DefaultUIDelegate::errorOutOfMem(QWidget *parent)
{
    Utils::errorOutOfMem(parent);
    _errorCount++;
}

void DefaultUIDelegate::resetErrorCount()
{
    _errorCount = 0;
}

int DefaultUIDelegate::errorCount()
{
    return _errorCount;
}

QWidget *DefaultUIDelegate::getMainWidget()
{
    return _window ;
}

QString DefaultUIDelegate::getAppTitle()
{
    return APP_TITLE ;
}

// this method is not good and should be removed from interface in a future version refactoring it
QString DefaultUIDelegate::editNodeElementAsXML(const bool /*isBase64Coded*/, Element * /*pElement*/, const QString & /*text*/, const bool /*isCData*/, bool &/*isCDataOut*/, bool & isOk)
{
    // fake method, fail always
    isOk = false ;
    return "" ;
}

