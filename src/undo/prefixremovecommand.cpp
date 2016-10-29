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

#include "prefixremovecommand.h"
#include "utils.h"

PrefixRemoveCommand::PrefixRemoveCommand(QTreeWidget *theWidget, Regola *newRegola, const QString &removedPrefix, QList<int> newPath, const TargetSelection::Type targetSelection, const bool isAllPrefixes)
    : UndoDataReply(theWidget, newRegola, newPath, targetSelection)
{
    _removedPrefix = removedPrefix ;
    _isAllPrefixes = isAllPrefixes ;
}

PrefixRemoveCommand::~PrefixRemoveCommand()
{
}

void PrefixRemoveCommand::undo()
{
    if(!innerUndo()) {
        Utils::error(widget->window(), QObject::tr("Error while undoing prefix remotion."));
    }
}

void PrefixRemoveCommand::redo()
{
    if(!innerRedo()) {
        Utils::error(widget->window(), QObject::tr("Error removing prefixes."));
    }
}

bool PrefixRemoveCommand::innerRedoSpecific(QList<Element*> elements, WriteElementUndoObserver *observer)
{
    return regola->removePrefix(_removedPrefix, elements, _targetSelection, _isAllPrefixes, observer);
}
