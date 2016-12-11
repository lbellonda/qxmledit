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

#ifndef SOURCEMESSAGEMANAGER_H
#define SOURCEMESSAGEMANAGER_H

#include "xmlEdit.h"
#include "modules/messages/sourceerror.h"
#include "modules/utils/simpletree.h"

class SourceMessageManager
{
    QList<int> findPath(SimpleTreeItem *current, Regola* regola);
    bool bindToSource(QXmlStreamReader *reader, QList<SourceMessage*> &errors, Regola *regola);
    void setSource(QXmlStreamReader *reader, SourceMessage *error, const bool isFound);
    void setPath(SourceMessage *error, Regola *regola, SimpleTreeItem *thisElement);

public:
    SourceMessageManager();
    ~SourceMessageManager();
    bool bindToSource(QIODevice *ioDevice, QList<SourceMessage*> &errors, Regola *regola);
};

#endif // SOURCEMESSAGEMANAGER_H
