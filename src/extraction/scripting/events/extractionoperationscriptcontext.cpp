/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2019 by Luca Bellonda and individual contributors       *
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

#include "extractionoperationscriptcontext.h"
#include "utils.h"

int ExtractionOperationScriptContext::instancesCount = 0 ;

ExtractionOperationScriptContext::ExtractionOperationScriptContext(QObject *parent) : QObject(parent)
{
    _level = 0 ;
    instancesCount++;
}

ExtractionOperationScriptContext::~ExtractionOperationScriptContext()
{
    instancesCount--;
}

bool ExtractionOperationScriptContext::compareTo(ExtractionOperationScriptContext &other)
{
    if(_path != other._path) {
        return false;
    }
    if(_level != other._level) {
        return false;
    }
    return true ;
}

QString ExtractionOperationScriptContext::path() const
{
    return _path;
}

void ExtractionOperationScriptContext::setPath(const QString &path)
{
    _path = path;
}

int ExtractionOperationScriptContext::level() const
{
    return _level;
}

void ExtractionOperationScriptContext::setLevel(int level)
{
    _level = level;
}
