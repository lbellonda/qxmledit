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
#include "xmlEdit.h"
#include "undoreplicacommand.h"
#include "regola.h"
#include "modules/replica/replicamanager.h"
#include "utils.h"

UndoReplicaCommand::UndoReplicaCommand(ReplicaCloneInfo *theInfo, QTreeWidget *theWidget, Regola *newRegola, QList<int> newPath, QUndoCommand *parent) : UndoCommand(theWidget, newRegola, newPath, parent)
{
    _info = theInfo ;
    _cloned = 0 ;
}

UndoReplicaCommand::~UndoReplicaCommand()
{
    if(NULL != _info) {
        delete _info ;
        _info = NULL ;
    }
}

void UndoReplicaCommand::redo()
{
    widget->setUpdatesEnabled(false);
    _cloned = 0 ;
    // execute the command
    Element *source = regola->findElementByArray(path);
    Element *parentElement = source->parent() ;
    Element *last = NULL ;
    Element *first = NULL ;
    int maxIndex = _info->numClones();
    int lastPos = path.last() + 1;
    FORINT(index, maxIndex) {
        // insert the command at the parent index.
        last = regola->insertInternal(widget, parentElement, source, lastPos, _info->deep());
        if(NULL == first) {
            first = last ;
        }
        lastPos ++ ;
        _cloned ++ ;
    }
    ReplicaCommand *fillInfo = _info->fillInfo() ;
    if(NULL != fillInfo) {
        ReplicaManager replicaManager;
        replicaManager.apply(widget, source->getParentRule(), first, fillInfo, _cloned);
    }

    if(NULL != last) {
        widget->setCurrentItem(last->getUI());
    }
    widget->setUpdatesEnabled(true);
}


void UndoReplicaCommand::undo()
{
    widget->setUpdatesEnabled(false);
    // execute the command
    Element *source = regola->findElementByArray(path);
    Q_ASSERT(NULL != source);
    Element *parentElement = source->parent() ;
    int posOfSource = path.last();
    int firstToDelete = posOfSource + 1 ;
    int toDelete = _cloned ;

    QVector<Element*> *els ;
    if(NULL != parentElement) {
        els = parentElement->getChildItems();
    } else {
        els = regola->getChildItems();
    }
#ifdef QXMLEDIT_TEST
    //int total = els->size();
#endif
    FORINT(index, toDelete) {
        Element *el = els->at(firstToDelete) ;
        el->autoDeleteRecursive();
    }
#ifdef QXMLEDIT_TEST
    //Q_ASSERT(total == (els->size() + _cloned));
#endif
    _cloned = 0 ;
    widget->setCurrentItem(source->getUI());
    widget->setUpdatesEnabled(true);
}
