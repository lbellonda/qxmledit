/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2018 by Luca Bellonda and individual contributors  *
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


#include "undobasechangeelement.h"
#include "regola.h"

UndoBaseChangeElement::UndoBaseChangeElement(QTreeWidget *theWidget, Regola *newRegola, QList<int> newPath) : UndoCommand(theWidget, newRegola, newPath)
{
    _firstShot = true ;
    _element = NULL ;

}

UndoBaseChangeElement::~UndoBaseChangeElement()
{
    loseElement();
}


void UndoBaseChangeElement::loseElement()
{
    if(NULL != _element) {
        delete _element;
        _element = NULL ;
    }
}

void UndoBaseChangeElement::insertElement()
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
    }
}

void UndoBaseChangeElement::removeElement()
{
    Element *element = regola->findElementByArray(path);
    if(NULL != element) {
        loseElement();
        element->autoDelete(false);
        _element = element ;
    }
}
