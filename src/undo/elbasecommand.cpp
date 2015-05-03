/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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


#include "elbasecommand.h"
#include "utils.h"
#include "regola.h"

ElBaseCommand::ElBaseCommand(QTreeWidget *theWidget, Regola *newRegola, Element *element, QList<int> newPath, QUndoCommand *parent) : UndoCommand(theWidget, newRegola, newPath, parent)
{
    _hilite = false;
    _element = element ;
    _selectParent = false;
    _lastOpElement = NULL ;
}

ElBaseCommand::~ElBaseCommand()
{
    loseElement();
}


bool ElBaseCommand::selectParent() const
{
    return _selectParent;
}

void ElBaseCommand::setSelectParent(const bool selectParent)
{
    _selectParent = selectParent;
}


bool ElBaseCommand::hilite() const
{
    return _hilite;
}

void ElBaseCommand::setHilite(bool hilite)
{
    _hilite = hilite;
}

void ElBaseCommand::loseElement()
{
    if(NULL != _element) {
        delete _element;
        _element = NULL ;
    }
}

void ElBaseCommand::setCurrentElement(Element *newElement)
{
    loseElement();
    _element = newElement ;
}

void ElBaseCommand::insertElement()
{
    if(NULL != _element) {
        // insert the command at the parent index.
        QList<int> pathPos(path);
        int lastPos = pathPos.last();
        pathPos.removeLast();
        Element *parentElement = NULL ;
        if(!pathPos.isEmpty()) {
            parentElement = regola->findElementByArray(pathPos);
        }
        _lastOpElement = regola->insertInternal(widget, parentElement, _element, lastPos);
        if(_selectParent && (NULL != parentElement)) {
            widget->setCurrentItem(parentElement->getUI());
        }
        loseElement();
    }
}

void ElBaseCommand::insertElementObj(Element *element, const bool isUndo)
{
    if(NULL != _element) {
        // insert the command at the parent index.
        QList<int> pathPos(path);
        int lastPos = pathPos.last();
        pathPos.removeLast();
        Element *parentElement = NULL ;
        if(!pathPos.isEmpty()) {
            parentElement = regola->findElementByArray(pathPos);
        }
        _lastOpElement = regola->insertInternal(widget, parentElement, element, lastPos);
        if(NULL != _lastOpElement) {
            widget->setCurrentItem(_lastOpElement->getUI());
        }
        if(_selectParent && (NULL != parentElement)) {
            widget->setCurrentItem(parentElement->getUI());
        }
        if(!isUndo && _hilite && (NULL != _lastOpElement)) {
            _lastOpElement->hilite();
        }
    }
}

void ElBaseCommand::removeElement()
{
    Element *element = regola->findElementByArray(path);
    if(NULL != element) {
        loseElement();
        element->autoDelete(false);
        _element = element ;
    }
    _lastOpElement = NULL ;
}

Element *ElBaseCommand::removeElementAndReturnIt()
{
    Element *element = regola->findElementByArray(path);
    if(NULL != element) {
        element->autoDelete(false);
        return element;
    }
    return NULL ;
}

void ElBaseCommand::replaceElement(const bool isUndo)
{
    Element *currentElement = removeElementAndReturnIt();
    insertElementObj(_element, isUndo);
    setCurrentElement(currentElement);
}
