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

#include "fakeuidelegate.h"
#include "utils.h"

FakeUIDelegate::FakeUIDelegate()
{
    isError = false;
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

void FakeUIDelegate::error(const QString& message)
{
    // fake method
    isError = true;
    lastErrorMsg = message ;
}

void FakeUIDelegate::error(QWidget * /*parent*/, const QString& message)
{
    // fake method
    isError = true;
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
    return false ;
}

bool FakeUIDelegate::askYN(QWidget * /*parent*/, const QString & /*message*/)
{
    // fake method
    return false ;
}

bool FakeUIDelegateYes::askYN(const QString & /*message*/)
{
    // fake method
    return true ;
}

bool FakeUIDelegateYes::askYN(QWidget * /*parent*/, const QString & /*message*/)
{
    // fake method
    return true ;
}


void FakeUIDelegate::errorNoSel(QWidget * /*parent*/)
{
    // fake method
    isError = true;
    lastErrorMsg = "NO SEL" ;

}

void FakeUIDelegate::errorOutOfMem(QWidget * /*parent*/)
{
    // fake method
    isError = true;
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
