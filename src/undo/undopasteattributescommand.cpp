/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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


#include "undopasteattributescommand.h"
#include "regola.h"


UndoPasteAttributesCommand::UndoPasteAttributesCommand(QTreeWidget *theWidget, Regola *newRegola, QList<int> newPath) : UndoCommand(theWidget, newRegola, newPath)
{
    _firstShot = false ;
}

UndoPasteAttributesCommand::~UndoPasteAttributesCommand()
{
    Attribute::emptyList(_originalAttributes);
    Attribute::emptyList(_newAttributes);
}

//--------------------------------------------------------------------------

QList<Attribute*> &UndoPasteAttributesCommand::originalAttributesList()
{
    return _originalAttributes;
}

QList<Attribute*> &UndoPasteAttributesCommand::newAttributesList()
{
    return _newAttributes;
}


bool UndoPasteAttributesCommand::setOriginalElement(Element *beforeEdit)
{
    QList<Attribute*> sourceList = beforeEdit->getAttributesList();
    return Attribute::copyAttributeListTo(sourceList, originalAttributesList()) ;
}

bool UndoPasteAttributesCommand::setModifiedElement(Element *afterEdit)
{
    QList<Attribute*> sourceList = afterEdit->getAttributesList();
    return Attribute::copyAttributeListTo(sourceList, newAttributesList()) ;
}

Element *UndoPasteAttributesCommand::getElement()
{
    // insert the command at the parent index.
    QList<int> pathPos(path);
    Element *element = regola->findElementByArray(pathPos);
    return element ;
}

void UndoPasteAttributesCommand::undo()
{
    Element *element = getElement();
    Q_ASSERT(NULL != element);
    element->setAttributes(_originalAttributes);
    element->updateSizeInfo();
    element->refreshUI();
}

void UndoPasteAttributesCommand::redo()
{
    if(_firstShot) {
        _firstShot = false ;
    } else {
        Element *element = getElement();
        Q_ASSERT(NULL != element);
        element->setAttributes(_newAttributes);
        element->updateSizeInfo();
        element->refreshUI();
    }
}
