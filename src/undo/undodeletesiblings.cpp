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

#include "undodeletesiblings.h"
#include "utils.h"

DeleteSiblingsCommand::DeleteSiblingsCommand(const RegolaDeleteSiblings::DeleteOptions newOption, QTreeWidget *theWidget, Regola *newRegola, QList<int> newPath)
    : UndoCommand(theWidget, newRegola, newPath)
{
    _option = newOption ;
    reset();
}

DeleteSiblingsCommand::~DeleteSiblingsCommand()
{
    while(!_afterElements.isEmpty()) {
        delete _afterElements.last();
        _afterElements.removeLast();
    }
    while(!_beforeElements.isEmpty()) {
        delete _beforeElements.last();
        _beforeElements.removeLast();
    }
}

void DeleteSiblingsCommand::reset()
{
    _posAfter = -1 ;
    _posBefore = -1 ;
    _beforeElements.clear();
    _afterElements.clear();
}

void DeleteSiblingsCommand::redo()
{
    widget->setUpdatesEnabled(false);
    reset();
    //---
    deleteSiblings();
    widget->setUpdatesEnabled(true);
}


void DeleteSiblingsCommand::undo()
{
    widget->setUpdatesEnabled(false);
    bool changed = (_beforeElements.size() > 0) || (_afterElements.size() > 0);
    restoreSiblingsBefore();
    restoreSiblingsAfter();
    reset() ;
    updateRegola(changed);
    widget->setUpdatesEnabled(true);
}

//------

void DeleteSiblingsCommand::deleteSiblings()
{
    bool changed = false;
    Element *selected = regola->findElementByArray(path);
    if(NULL == selected) {
        Utils::error(Utils::errorNoSelString());
        return ;
    }
    switch(_option) {
    default:
        Utils::error(QObject::tr("Unknown delete method"));
        break;
    case DeleteAllSiblings:
        changed = deleteAllSiblings(selected);
        break;
    case DeleteAllSiblingsBefore:
        changed = deleteAllSiblingsBefore(selected);
        break;
    case DeleteAllSiblingsAfter:
        changed = deleteAllSiblingsAfter(selected);
        break;
    }
    updateRegola(changed);
}

void DeleteSiblingsCommand::updateRegola(const bool changed)
{
    if(changed) {
        regola->setModified(true) ;
    }
}

bool DeleteSiblingsCommand::deleteAllSiblingsAfter(Element *selected)
{
    int pos = selected->indexOfSelfAsChild();
    Element *parent = selected->parent();
    bool removed = false;
    if(NULL != parent) {
        int count = parent->getChildItemsCount();
        Utils::TODO_THIS_RELEASE("attenzione, provare e fare anche per gli altri comandi");
        _posAfter = pos + 1;
        int toDelete = count - (pos + 1);
        QList<QTreeWidgetItem*> siblings = parent->getUI()->takeChildren();
        while(toDelete > 0) {
            toDelete -- ;
            Element *removedElement = parent->getChildAt(pos + 1);
            removedElement->autoDelete(false, true, true);
            _afterElements.append(removedElement);
            // why this? because the takeChild() is very slow.
            removeItemInListAt(siblings, pos + 1);
            removed = true;
        }
        parent->getUI()->addChildren(siblings);
        parent->updateSizeInfo(true);
    }
    return removed ;
}

bool DeleteSiblingsCommand::deleteAllSiblingsBefore(Element *selected)
{
    int pos = selected->indexOfSelfAsChild();
    if(pos <= 0) {
        // nothing to do
        return false;
    }
    Element *parent = selected->parent();
    bool removed = false;
    if(NULL != parent) {
        Utils::TODO_THIS_RELEASE("testare con valgrind");
        Utils::TODO_THIS_RELEASE("attenzione, provare e fare anche per gli altri comandi");
        _posBefore = 0 ;
        int toDelete = pos;
        QList<QTreeWidgetItem*> siblings = parent->getUI()->takeChildren();
        while(toDelete > 0) {
            toDelete -- ;
            Element *removedElement = parent->getChildAt(0);
            removedElement->autoDelete(false, true, true);
            _beforeElements.append(removedElement);
            removeItemInListAt(siblings, 0);
            removed = true;
        }
        parent->getUI()->addChildren(siblings);
        parent->updateSizeInfo(true);
    }
    return removed ;
}

void DeleteSiblingsCommand::removeItemInListAt(QList<QTreeWidgetItem*> &siblings, const int position)
{
    QTreeWidgetItem * current = siblings.at(position);
    delete current;
    siblings.removeAt(position);
}

bool DeleteSiblingsCommand::deleteAllSiblings(Element *selected)
{
    bool removed = false;
    if(deleteAllSiblingsBefore(selected)) {
        removed = true ;
    }
    if(deleteAllSiblingsAfter(selected)) {
        removed = true ;
    }
    return removed ;
}

void DeleteSiblingsCommand::restoreSiblingsAfter()
{
    QList<int> parentPath(path);
    parentPath.removeLast();
    Element *parent = regola->findElementByArray(parentPath);
    if(NULL != parent) {
        while(!_beforeElements.isEmpty()) {
            regola->attachElementAt(widget, parent, _beforeElements.last(), 0);
            _beforeElements.removeLast();
        }
        parent->updateSizeInfo(true);
    }
}

void DeleteSiblingsCommand::restoreSiblingsBefore()
{
    QList<int> parentPath(path);
    parentPath.removeLast();
    Element *parent = regola->findElementByArray(parentPath);
    if(NULL != parent) {
        while(!_afterElements.isEmpty()) {
            regola->attachElementAt(widget, parent, _afterElements.last(), _posAfter);
            _afterElements.removeLast();
        }
        parent->updateSizeInfo(true);
    }
}
