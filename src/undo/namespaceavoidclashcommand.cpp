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

#include "namespaceavoidclashcommand.h"
#include "utils.h"

NamespaceAvoidClashCommand::NamespaceAvoidClashCommand(QTreeWidget *theWidget, Regola *newRegola, const QString &legalNS, const QString &legalPrefix, NamespacesInfo *namespacesInfo, QUndoCommand *parent)
    : UndoDataReply(theWidget, newRegola, QList<int>(), TargetSelection::AllItems, parent)
{
    _ns = legalNS ;
    _prefix = legalPrefix ;
    _namespacesInfo = namespacesInfo->clone();
}

NamespaceAvoidClashCommand::~NamespaceAvoidClashCommand()
{
    if(NULL != _namespacesInfo) {
        delete _namespacesInfo ;
    }
}

void NamespaceAvoidClashCommand::undo()
{
    if(!innerUndo()) {
        Utils::error(widget->window(), QObject::tr("Error while undoing namespace remastering."));
    }
}

void NamespaceAvoidClashCommand::redo()
{
    if(!innerRedo()) {
        Utils::error(widget->window(), QObject::tr("Error remastering namespaces."));
    }
}

bool NamespaceAvoidClashCommand::innerRedoSpecific(QList<Element *>, WriteElementUndoObserver * observer)
{
    return regola->namespaceAvoidClash(_prefix, _ns, _namespacesInfo, observer);
}
