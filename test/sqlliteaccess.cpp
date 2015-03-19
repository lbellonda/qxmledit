/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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


#include "sqlliteaccess.h"

SQLLiteTestAccess::SQLLiteTestAccess(const bool openAccess)
{
    _isOpenAccess = openAccess;
    if(_isOpenAccess) {
        access = new SQLLiteDataAccess();
    } else {
        access = NULL ;
    }
    logHandler.setEnabled(false);
    logHandler.setLevel(LogHandler::DEBUG);
    if(_isOpenAccess) {
        access->setLogger(&logHandler);
    }
}

SQLLiteTestAccess::~SQLLiteTestAccess()
{
    if(NULL != access) {
        access->closeAndDispose() ;
    }
    access = NULL ;
}

bool SQLLiteTestAccess::init()
{
    tempFile.open();
    dbFileName = tempFile.fileName();
    if( _isOpenAccess ) {
        return access->init(tempFile.fileName());
    } else {
        return true;
    }
}
