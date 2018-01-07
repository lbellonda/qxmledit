/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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

#include "undoaddparentcommand.h"
#include "regola.h"
#include "utils.h"

// addParent: always 1! child
UndoAddParentCommand::UndoAddParentCommand(QTreeWidget *theWidget, Regola *newRegola, const QString &newTag, QList<Attribute*> attributesIn, QList<int> newPath) : UndoCommand(theWidget, newRegola, newPath)
{
    _tag = newTag ;
    reset();
    foreach(Attribute * attribute, attributesIn) {
        Attribute *newAttribute = attribute->clone();
        _attributes.append(newAttribute);
    }
}

UndoAddParentCommand::~UndoAddParentCommand()
{
    reset();
}

void UndoAddParentCommand::reset()
{
    foreach(Attribute * attribute, _attributes) {
        delete attribute;
    }
    _attributes.clear();
}

void UndoAddParentCommand::redo()
{
    // insert the command at the parent index.
    Element *parentElement = NULL ;
    if(!path.isEmpty()) {
        parentElement = regola->findElementByArray(path);
    }
    int pos = parentElement->indexOfSelfAsChild();
    regola->insertParentAction(parentElement, _tag, _attributes, widget, pos, 1);
}


void UndoAddParentCommand::undo()
{
    // insert the command at the parent index.
    Element *parentElement = NULL ;
    if(!path.isEmpty()) {
        parentElement = regola->findElementByArray(path);
    }
    int insPos = -1;
    int insLen = 0 ;
    regola->removeParentAction(parentElement, widget, insPos, insLen);
}
