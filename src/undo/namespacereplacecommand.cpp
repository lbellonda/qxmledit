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


#include "namespacereplacecommand.h"
#include "utils.h"

NamespaceReplaceCommand::NamespaceReplaceCommand(QTreeWidget *theWidget, Regola *newRegola, const QString &replacedNS, const QString &newNS, const QString &newPrefix, QList<int> newPath, const TargetSelection::Type targetSelection, QUndoCommand *parent)
    : UndoDataReply(theWidget, newRegola, newPath, targetSelection, parent)
{
    _newNS = newNS ;
    _newPrefix = newPrefix ;
    _toReplaceNS = replacedNS ;
}

NamespaceReplaceCommand::~NamespaceReplaceCommand()
{
}


void NamespaceReplaceCommand::undo()
{
    if(!innerUndo()) {
        Utils::error(widget->window(), QObject::tr("Error while undoing namespace replacement."));
    }
}

void NamespaceReplaceCommand::redo()
{
    if(!innerRedo()) {
        Utils::error(widget->window(), QObject::tr("Error replacing a namespace."));
    }
}

bool NamespaceReplaceCommand::innerRedoSpecific(QList<Element *>elements, WriteElementUndoObserver * observer)
{
    return regola->replaceNamespace(_toReplaceNS, _newNS, _newPrefix, elements, _targetSelection, observer);
}
