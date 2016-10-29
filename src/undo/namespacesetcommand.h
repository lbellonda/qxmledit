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


#ifndef NAMESPACESETCOMMAND_H
#define NAMESPACESETCOMMAND_H

#include "undodatareply.h"
#include "writeelementundoobserver.h"

class NamespaceSetCommand : public UndoDataReply
{
protected:
    QString _ns;
    QString _prefix;

    //---
    virtual bool innerRedoSpecific(QList<Element *>elements, WriteElementUndoObserver *observer);
public:
    NamespaceSetCommand(QTreeWidget *theWidget, Regola *newRegola, const QString &newNS, const QString &newPrefix, QList<int> path, TargetSelection::Type targetSelection, QUndoCommand *parent = NULL);
    virtual ~NamespaceSetCommand();

    virtual void undo();
    virtual void redo();
};

#endif // NAMESPACESETCOMMAND_H
