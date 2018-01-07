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

#include "fakeuidelegate.h"
#include "utils.h"

FakeUIDelegate::FakeUIDelegate()
{
    isError = false;
    _errors = 0 ;
    _askCountBeforeLoad = 0 ;
    _askCountAfterLoad = 0 ;
    _beforeLoad = true ;
}

FakeUIDelegate::~FakeUIDelegate()
{
}

FakeUIDelegateYes::FakeUIDelegateYes()
{
    isError = false;
}

FakeUIDelegateYes::~FakeUIDelegateYes()
{
}

void FakeUIDelegate::registerAsk()
{
    if(_beforeLoad) {
        _askCountBeforeLoad ++ ;
    } else {
        _askCountAfterLoad ++ ;
    }
}

void FakeUIDelegate::justBeforeLoad()
{
    _beforeLoad = false ;
}

void FakeUIDelegate::registerError()
{
    isError = true;
    _errors++;
}

void FakeUIDelegate::error(const QString& message)
{
    // fake method
    isError = true;
    _errors++;
    lastErrorMsg = message ;
}

void FakeUIDelegate::error(QWidget * /*parent*/, const QString& message)
{
    // fake method
    isError = true;
    _errors++;
    lastErrorMsg = message ;
}

void FakeUIDelegate::warning(const QString& /*message*/)
{
    // fake method
}

void FakeUIDelegate::message(const QString& /*message*/)
{
    // fake method
}

bool FakeUIDelegate::askYN(const QString & /*message*/)
{
    // fake method
    registerAsk();
    return false ;
}

bool FakeUIDelegate::askYN(QWidget * /*parent*/, const QString & /*message*/)
{
    // fake method
    registerAsk();
    return false ;
}

bool FakeUIDelegateYes::askYN(const QString & /*message*/)
{
    // fake method
    registerAsk();
    return true ;
}

bool FakeUIDelegateYes::askYN(QWidget * /*parent*/, const QString & /*message*/)
{
    // fake method
    registerAsk();
    return true ;
}


void FakeUIDelegate::errorNoSel(QWidget * /*parent*/)
{
    // fake method
    isError = true;
    _errors++;
    lastErrorMsg = "NO SEL" ;

}

void FakeUIDelegate::errorOutOfMem(QWidget * /*parent*/)
{
    // fake method
    isError = true;
    _errors++;
    lastErrorMsg = msgOutOfMem();
}

QWidget *FakeUIDelegate::getMainWidget()
{
    // fake method
    return NULL ;
}

QString FakeUIDelegate::getAppTitle()
{
    // fake method
    return "----" ;
}

QString FakeUIDelegate::editNodeElementAsXML(const bool /*isBase64Coded*/, Element * /*pElement*/, const QString & /*text*/, const bool /*isCData*/, bool &/*isCDataOut*/, bool & isOk)
{
    // fake method
    isOk = false ;
    return "" ;
}

QString FakeUIDelegate::msgOutOfMem()
{
    return Utils::msgOutOfMem();
}

void FakeUIDelegate::resetErrorCount()
{
    _errors = 0;
    _askCountAfterLoad = 0;
    _askCountBeforeLoad = 0 ;
    _beforeLoad = true ;
}

int FakeUIDelegate::errorCount()
{
    return _errors;
}

int FakeUIDelegate::askTotalCount() const
{
    return _askCountBeforeLoad + _askCountAfterLoad ;
}

int FakeUIDelegate::askCountBeforeLoad() const
{
    return _askCountBeforeLoad;
}

void FakeUIDelegate::setAskCountBeforeLoad(int askCountBeforeLoad)
{
    _askCountBeforeLoad = askCountBeforeLoad;
}

int FakeUIDelegate::askCountAfterLoad() const
{
    return _askCountAfterLoad;
}

void FakeUIDelegate::setAskCountAfterLoad(int askCountAfterLoad)
{
    _askCountAfterLoad = askCountAfterLoad;
}

