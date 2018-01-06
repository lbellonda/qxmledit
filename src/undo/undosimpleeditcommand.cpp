/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016-2018 by Luca Bellonda and individual contributors  *
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

#include "undosimpleeditcommand.h"
#include "utils.h"
#include "regola.h"

UndoSimpleEditCommand::UndoSimpleEditCommand(QTreeWidget *theWidget, Regola *newRegola, QList<int> newPath, Element *newElement) : UndoCommand(theWidget, newRegola, newPath)
{
    _element = newElement ;
}

UndoSimpleEditCommand::~UndoSimpleEditCommand()
{
    if(NULL != _element) {
        delete _element;
    }
}

void UndoSimpleEditCommand::undo()
{
    swap();
}

void UndoSimpleEditCommand::redo()
{
    swap();
}


/**
  algorithm:
  strip any attributes, text nodes and text children from current
  copy the attributes, text nodes from the original
  paste new text nodes placing them between elements (that does not change)
  */
void UndoSimpleEditCommand::swap()
{
    Element swapElement(NULL) ;
    if(!regola->applyEditAndSwapElement(_element, &swapElement, path)) {
        Utils::error(tr("An error occurred applying editing feature."));
    } else {
        swapElement.copyTo(*_element, false);
    }
}


