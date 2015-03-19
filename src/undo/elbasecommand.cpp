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
    _element = element ;
}

ElBaseCommand::~ElBaseCommand()
{
    loseElement();
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
        regola->insertInternal(widget, parentElement, _element, lastPos);
        loseElement();
    }
}

void ElBaseCommand::insertElementObj(Element *element)
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
        Element *newElement = regola->insertInternal(widget, parentElement, element, lastPos);
        if(NULL != newElement) {
            widget->setCurrentItem(newElement->getUI());
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

void ElBaseCommand::replaceElement()
{
    Element *currentElement = removeElementAndReturnIt();
    insertElementObj(_element);
    setCurrentElement(currentElement);
}
