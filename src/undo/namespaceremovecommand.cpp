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

#include "namespaceremovecommand.h"
#include "utils.h"

NamespaceRemoveCommand::NamespaceRemoveCommand(QTreeWidget *theWidget, Regola *newRegola, const QString &removedNS, QList<int> newPath, const TargetSelection::Type targetSelection, const bool isAllNS, const bool isRemoveDeclarations)
    : UndoDataReply(theWidget, newRegola, newPath, targetSelection)
{
    _removedNS = removedNS ;
    _isAllNS = isAllNS ;
    _removeDeclarations = isRemoveDeclarations ;
}

NamespaceRemoveCommand::~NamespaceRemoveCommand()
{
}


void NamespaceRemoveCommand::undo()
{
    if(!innerUndo()) {
        Utils::error(widget->window(), QObject::tr("Error while undoing namespace remotion."));
    }
}

void NamespaceRemoveCommand::redo()
{
    if(!innerRedo()) {
        Utils::error(widget->window(), QObject::tr("Error removing namespaces."));
    }
}

bool NamespaceRemoveCommand::innerRedoSpecific(QList<Element *>elements, WriteElementUndoObserver * observer)
{
    return regola->removeNamespace(_removedNS, elements, _targetSelection, _isAllNS, _removeDeclarations, observer);
}
