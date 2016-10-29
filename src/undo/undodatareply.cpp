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


#include "undodatareply.h"
#include "utils.h"

UndoDataReply::UndoDataReply(QTreeWidget *theWidget, Regola *newRegola, QList<int> path, const TargetSelection::Type targetSelection, QUndoCommand *parent)
    : UndoCommand(theWidget, newRegola, path, parent)
{
    _targetSelection = targetSelection ;
    _undoData = NULL ;
}

UndoDataReply::~UndoDataReply()
{
    if(NULL != _undoData) {
        // hope that works
        delete _undoData ;
    }
}

bool UndoDataReply::innerUndo()
{
    bool ok = false;
    bool okData = true;
    if(NULL == _undoData) {
        return true ;
    }
    if(_undoData->open()) {
        QDataStream dataStream(_undoData);
        dataStream.setVersion(DATASTREAM_VERSION);
        dataStream.resetStatus();
        widget->setUpdatesEnabled(false);
        int dataIndex = 1;
        while(!dataStream.atEnd()) {
            ElementUndoInfo info;
            dataStream >> info.path ;
            dataStream >> info.tag ;
            dataStream >> info.replaceAttributes ;
            dataStream >> info.attributes ;
            if(!regola->updateElementInplace(&info)) {
                okData = false ;
            }
            dataIndex++;
        }
        widget->setUpdatesEnabled(true);
        if((dataStream.status() == QDataStream::Ok) && okData) {
            ok = true ;
        }
        _undoData->close();
    } else {
        ok = false;
    }
    delete _undoData;
    _undoData = NULL ;
    return ok ;
}

bool UndoDataReply::innerRedo()
{
    if(NULL == _undoData) {
        _undoData = new QTemporaryFile();
        if(!_undoData->open()) {
            delete _undoData ;
            _undoData = NULL ;
            return false;
        }
    }
    WriteElementUndoObserver observer(_undoData, this);
    // find the selection type
    bool ok = preInnerRedoSpecific(&observer);
    if(!_undoData->flush()) {
        ok = false;
    }
    _undoData->close();

    widget->setUpdatesEnabled(true);
    return ok ;
}

/*!
  data format:
  path as?
  string tag,
  # attributes
   for each attribute:
     position, name
*/


bool UndoDataReply::observe(QDataStream &writeDataStream, ElementUndoInfo *info)
{
    writeDataStream << info->path ;
    writeDataStream << info->tag ;
    writeDataStream << info->replaceAttributes ;
    writeDataStream << info->attributes ;
    if(writeDataStream.status() == QDataStream::Ok) {
        return true ;
    }
    return false;
}

bool UndoDataReply::preInnerRedoSpecific(WriteElementUndoObserver *observer)
{
    // find the selection type
    widget->setUpdatesEnabled(false);
    QList<Element*> elements ;
    if(TargetSelection::AllItems == _targetSelection) {
        if(NULL != regola->root()) {
            elements.append(regola->root());
        }
    } else if((TargetSelection::SelectedItem == _targetSelection) || (TargetSelection::SelectedItemAndChildren == _targetSelection)) {
        Element *sel = regola->findElementByArray(path);
        if(NULL != sel) {
            elements.append(sel);
        }
    } else if((TargetSelection::Bookmarks == _targetSelection) || (TargetSelection::BookmarksAndChildren == _targetSelection)) {
        elements = regola->getUniqueBookmarksElements(_targetSelection);
    }
    bool ok = true ;
    if(elements.isEmpty()) {
        ok = false ;
    } else {
        if(!innerRedoSpecific(elements, observer)) {
            ok = false;
        }
    }
    widget->setUpdatesEnabled(true);
    return ok ;
}
