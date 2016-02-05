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

#include "undosortattributes.h"
#include "regola.h"
#include "utils.h"

UndoSortAttributes::UndoSortAttributes(QTreeWidget *theWidget, Regola *newRegola, QList<int> newPath, const bool isRecursive, QUndoCommand *parentCommand)
    : ElBaseCommand(theWidget, newRegola, NULL, newPath, parentCommand)
{
    _isRecursive = isRecursive ;
    if(isRecursive) {
        Element *element = regola->findElementByArray(newPath);
        if(NULL != element) {
            foreach(Element * child, element->getItems()) {
                if(child->isElement()) {
                    QList<int> childPath = child->indexPath();
                    /*UndoSortAttributes *childUndo = */new UndoSortAttributes(theWidget, regola, childPath, true, this);
                }
            }
        }
    }
}

UndoSortAttributes::~UndoSortAttributes()
{
}

void UndoSortAttributes::undo()
{
    Element *currentElement = regola->findElementByArray(path);
    if(NULL != currentElement) {
        currentElement->sortAttributesByMap(_oldPositionsForAttributes);
        regola->updateElementUI(currentElement, false);
        currentElement->forceUpdateGui(true);
    }
    QUndoCommand::undo();
}

void UndoSortAttributes::redo()
{
    _oldPositionsForAttributes.clear();
    Element *currentElement = regola->findElementByArray(path);
    if(NULL != currentElement) {
        currentElement->sortAttributes(&_oldPositionsForAttributes, false);
        regola->updateElementUI(currentElement, false);
        currentElement->forceUpdateGui(true);
    }
    QUndoCommand::redo();
}
