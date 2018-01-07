/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2018 by Luca Bellonda and individual contributors  *
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

#include "replicacloneinfo.h"
#include "replicacommand.h"

ReplicaCloneInfo::ReplicaCloneInfo()
{
    _numClones = 1;
    _deep = false;
    _fillInfo = NULL ;
}

ReplicaCloneInfo::~ReplicaCloneInfo()
{
    if(NULL != _fillInfo) {
        delete _fillInfo;
        _fillInfo = NULL ;
    }
}

int ReplicaCloneInfo::numClones() const
{
    return _numClones;
}

void ReplicaCloneInfo::setNumClones(int numClones)
{
    _numClones = numClones;
}

bool ReplicaCloneInfo::deep() const
{
    return _deep;
}

void ReplicaCloneInfo::setDeep(bool deep)
{
    _deep = deep;
}

ReplicaCommand *ReplicaCloneInfo::fillInfo() const
{
    return _fillInfo;
}

void ReplicaCloneInfo::setFillInfo(ReplicaCommand *fillInfo)
{
    _fillInfo = fillInfo;
}
