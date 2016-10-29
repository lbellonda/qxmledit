/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016 by Luca Bellonda and individual contributors       *
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

#include "writeelementundoobserver.h"
#include "undodatareply.h"
#include "utils.h"

//---------------------------------------------------------------

ElementUndoObserver::ElementUndoObserver()
{
}

ElementUndoObserver::~ElementUndoObserver()
{
}

//---------------------------------------------------------------

WriteElementUndoObserver::WriteElementUndoObserver(QIODevice *ioDevice, UndoDataReply *reply)
{
    _inited = false;
    _reply = reply;
    _ioDevice = ioDevice;
}

WriteElementUndoObserver::~WriteElementUndoObserver()
{
}

bool WriteElementUndoObserver::init()
{
    if(_inited) {
        return true ;
    }
    if(NULL == _ioDevice) {
        return false;
    }
    if(!_ioDevice->isOpen()) {
        if(!_ioDevice->open(QIODevice::ReadWrite)) {
            return false;
        }
    }
    _writeDataStream.setDevice(_ioDevice);
    _writeDataStream.setVersion(UndoDataReply::DATASTREAM_VERSION);
    _writeDataStream.resetStatus();
    _inited = true ;
    return true ;
}

/*!
  data format:
  path as?
  string tag,
  # attributes
   for each attribute:
     position, name
*/

bool WriteElementUndoObserver::observe(ElementUndoInfo *info)
{
    if(!init()) {
        return false;
    }
    return _reply->observe(_writeDataStream, info);
}

