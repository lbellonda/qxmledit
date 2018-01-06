/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2018 by Luca Bellonda and individual contributors  *
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

#include "elupdateinplacecommand.h"
#include "regola.h"

ElUpdateInPlaceCommand::ElUpdateInPlaceCommand(QTreeWidget *theWidget, Regola *newRegola, Element *originalElement, Element *newElement, QList<int> newPath, QUndoCommand *parentCommand)
    : ElBaseCommand(theWidget, newRegola, newElement, newPath, parentCommand)
{
    _addToBookmarks = false;
    _orignal = originalElement->copyTo(*new Element("", "", NULL, NULL), false);
}

ElUpdateInPlaceCommand::~ElUpdateInPlaceCommand()
{
    if(NULL != _orignal) {
        delete _orignal ;
    }
}

void ElUpdateInPlaceCommand::undo()
{
    replaceElementInPlace(true, _orignal);
    if(_addToBookmarks) {
        regola->addBookmark(_element);
    }
}

void ElUpdateInPlaceCommand::redo()
{
    replaceElementInPlace(false, _element);
}

bool ElUpdateInPlaceCommand::addToBookmarks() const
{
    return _addToBookmarks;
}

void ElUpdateInPlaceCommand::setAddToBookmarks(bool addToBookmarks)
{
    _addToBookmarks = addToBookmarks;
}

