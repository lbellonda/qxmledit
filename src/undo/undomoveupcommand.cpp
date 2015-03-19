/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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

#include "undomoveupcommand.h"
#include "regola.h"

UndoMoveUpCommand::UndoMoveUpCommand(QTreeWidget *theWidget, Regola *newRegola, QList<int> newPath) : UndoCommand(theWidget, newRegola, newPath)
{
    _done = false;
}

UndoMoveUpCommand::~UndoMoveUpCommand()
{
}

void UndoMoveUpCommand::undo()
{
    if(_done) {
        QList<int> selPath(path);
        int size = selPath.size() ;
        if(size > 0) {
            int lastPos = selPath.at(size - 1);
            lastPos --;
            selPath.removeLast();
            selPath.append(lastPos);
        }
        Element *element = regola->findElementByArray(selPath);
        regola->internalMoveDown(element, false);
        regola->checkEncodingByEl(element);
    }
}

void UndoMoveUpCommand::redo()
{
    Element *element = regola->findElementByArray(path);
    _done = regola->internalMoveUp(element, false);
    regola->checkEncodingByEl(element);
}

bool UndoMoveUpCommand::done()
{
    return _done ;
}
