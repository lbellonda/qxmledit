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

#include "editxsdenumcommand.h"
#include "element.h"
#include "modules/xsd/xsdhelper.h"
#include "utils.h"

EditXSDEnumCommand::EditXSDEnumCommand(QTreeWidget *theWidget, Regola *newRegola, QList<int> newPath, QList<Element*> newElements)
    : UndoCommand(theWidget, newRegola, newPath)
{
    foreach(Element * e, newElements) {
        _elementsToInsert.append(e);
    }
}

EditXSDEnumCommand::~EditXSDEnumCommand()
{
    EMPTYPTRLIST(_elementsToInsert, Element);
    EMPTYPTRLIST(_elementsToDelete, Element);
}

void EditXSDEnumCommand::updateRegola(Element *selected)
{
    selected->updateSizeInfo(true);
    regola->setModified(true) ;
}

void EditXSDEnumCommand::redo()
{
    //---
    Element *selected = regola->findElementByArray(path);
    if(NULL == selected) {
        Utils::error(Utils::errorNoSelString());
        return ;
    }
    removeOldObjects(selected);
    insertNewObjects(selected);
    selected->updateSizeInfo(true);
    updateRegola(selected);
}


void EditXSDEnumCommand::undo()
{
    Element *selected = regola->findElementByArray(path);
    if(NULL == selected) {
        Utils::error(Utils::errorNoSelString());
        return ;
    }
    removeNewObjects(selected);
    insertOldObjects(selected);
    selected->updateSizeInfo(true);
    updateRegola(selected);
}

void EditXSDEnumCommand::removeOldObjects(Element *selected)
{
    Q_ASSERT(NULL != selected);
    _oldPositions.clear();
    _elementsToDelete.clear();
    // collect info on objects
    XSDOperationParameters params;
    XSDHelper helper;
    regola->XSDSetNamespaceToParams(&params, selected);
    QList<Element*> facetsElements = helper.findFacetsElements(selected, &params);
    foreach(Element * child, facetsElements) {
        _oldPositions.append(child->indexOfSelfAsChild());
        _elementsToDelete.append(child);
    }
    // remove objects from parent, from last to first
    for(int pos = _oldPositions.size() - 1 ; pos >= 0 ; pos--) {
        Element *removeThis = _elementsToDelete.at(pos);
        removeThis->autoDelete(false);
    }
}

void EditXSDEnumCommand::insertNewObjects(Element *selected)
{
    Q_ASSERT(NULL != selected);
    _newPositions.clear();
    while(!_elementsToInsert.isEmpty()) {
        Element *newElement = _elementsToInsert.first();
        regola->attachElementAt(widget, selected, newElement, -1);
        _elementsToInsert.removeFirst();
        _newPositions.append(newElement->indexOfSelfAsChild());
    }
}

//-----------------------------------------------

void EditXSDEnumCommand::removeNewObjects(Element *selected)
{
    Q_ASSERT(NULL != selected);
    _elementsToInsert.clear();
    while(! _newPositions.isEmpty()) {
        int pos = _newPositions.last();
        _newPositions.removeLast();
        Element *removeThis = selected->getChildAt(pos);
        removeThis->autoDelete(false);
        _elementsToInsert.prepend(removeThis);
    }
}

void EditXSDEnumCommand::insertOldObjects(Element *selected)
{
    Q_ASSERT(NULL != selected);
    while(!_oldPositions.isEmpty()) {
        int pos = _oldPositions.first();
        _oldPositions.removeFirst();
        Element *insertThis = _elementsToDelete.first();
        regola->attachElementAt(widget, selected, insertThis, pos);
        _elementsToDelete.removeFirst();
    }
    _elementsToDelete.clear();
}
