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


#include "undoaddchildcontainer.h"
#include "regola.h"
#include "utils.h"

UndoAddChildContainerCommand::UndoAddChildContainerCommand(QTreeWidget *theWidget, Regola *newRegola, const QString &newTag, QList<Attribute*> attributesIn, QList<int> newPath) : UndoCommand(theWidget, newRegola, newPath)
{
    _tag = newTag ;
    reset();
    foreach(Attribute * attribute, attributesIn) {
        Attribute *newAttribute = attribute->clone();
        _attributes.append(newAttribute);
    }
}

UndoAddChildContainerCommand::~UndoAddChildContainerCommand()
{
    reset();
}

void UndoAddChildContainerCommand::reset()
{
    foreach(Attribute * attribute, _attributes) {
        delete attribute;
    }
    _attributes.clear();
}

void UndoAddChildContainerCommand::redo()
{
    // insert the command at the parent index.
    Element *parentElement = NULL ;
    if(!path.isEmpty()) {
        parentElement = regola->findElementByArray(path);
    }
    regola->insertChildContainerAction(parentElement, _tag, _attributes, widget);
}


void UndoAddChildContainerCommand::undo()
{
    // insert the command at the parent index.
    Element *parentElement = NULL ;
    if(!path.isEmpty()) {
        parentElement = regola->findElementByArray(path);
    }
    regola->removeChildContainerAction(parentElement, widget);
}

