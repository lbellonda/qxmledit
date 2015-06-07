/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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

#include "undocommandgroup.h"
#include "regola.h"
#include "utils.h"

UndoCommandGroup::UndoCommandGroup() :
    QUndoCommand()
{
    _target = NULL ;
    _treeWidget = NULL ;
    _isHiliteAll = false ;
    _isCloseUnrelated = false ;
}


UndoCommandGroup::~UndoCommandGroup()
{
}

QList<int> UndoCommandGroup::selectedPath() const
{
    return _selectedPath;
}

void UndoCommandGroup::setSelectedPath(const QList<int> &selectedPath)
{
    _selectedPath = selectedPath;
}

Regola *UndoCommandGroup::target() const
{
    return _target;
}

void UndoCommandGroup::setTarget(Regola *target)
{
    _target = target;
}

QTreeWidget *UndoCommandGroup::treeWidget() const
{
    return _treeWidget;
}

void UndoCommandGroup::setTreeWidget(QTreeWidget *treeWidget)
{
    _treeWidget = treeWidget;
}

bool UndoCommandGroup::isHiliteAll() const
{
    return _isHiliteAll;
}

void UndoCommandGroup::setIsHiliteAll(bool isHiliteAll)
{
    _isHiliteAll = isHiliteAll;
}

bool UndoCommandGroup::isCloseUnrelated() const
{
    return _isCloseUnrelated;
}

void UndoCommandGroup::setIsCloseUnrelated(bool isCloseUnrelated)
{
    _isCloseUnrelated = isCloseUnrelated;
}

void UndoCommandGroup::undo()
{
    QUndoCommand::undo();
}

void UndoCommandGroup::redo()
{
    Element *startElement = _selectedPath.isEmpty() ? _target->root() : _target->findElementByArray(_selectedPath);
    if(_isCloseUnrelated && (NULL != startElement)) {
        if(startElement->getUI()->isExpanded()) {
            startElement->getUI()->setExpanded(false);
        }
    }
    // apply all changess
    QUndoCommand::redo();
}
