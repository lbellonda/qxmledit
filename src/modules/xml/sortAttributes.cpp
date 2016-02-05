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

#include "element.h"
#include "regola.h"
#include "utils.h"
#include "undo/undosortattributes.h"

/*!
 * \brief Element::sortAttributes sorts the attributes of element registering their
 * original position in a list passed as parameter if it is not null
 * \param undoPositionList
 */
void Element::sortAttributes(QList<int> *undoPositionList, const bool isRecursive)
{
    QMap<QString, int> sortedPosAndOriginalPosition;
    QHash<QString, Attribute*> well;
    QVector<Attribute*> newPos;
    int index = 0;
    foreach(Attribute * attribute, attributes) {
        sortedPosAndOriginalPosition.insert(attribute->name, index);
        well.insert(attribute->name, attribute);
        index ++ ;
    }
    foreach(QString key, sortedPosAndOriginalPosition.keys()) {
        newPos.append(well.value(key));
        if((NULL != undoPositionList) && !isRecursive) {
            undoPositionList->append(sortedPosAndOriginalPosition.value(key));
        }
    }
    if(isRecursive) {
        foreach(Element * child, childItems) {
            if(child->isElement()) {
                child->sortAttributes(NULL, true);
            }
        }
    }
    //
    attributes = newPos ;
}

/*!
 * \brief Element::sortAttributesByMap undo the sortAttributes operation
 * \param oldPositions
 * \return
 */
bool Element::sortAttributesByMap(QList<int> oldPositions)
{
    // consistency check
    if(oldPositions.size() != attributes.size()) {
        return false;
    }
    QVector<Attribute*> newList ;
    int maxIndex = attributes.size();
    FORINT(index, maxIndex) {
        int positionInTheNewArray = oldPositions.at(index);
        Attribute *attribute = attributes.at(positionInTheNewArray);
        newList.append(attribute);
    }
    attributes = newList ;
    return true ;
}

//--------------------

/*!
 * \brief Regola::sortAttributesOfElement sorts the attributes og one element, recursively or not
 * \param element
 * \param isRecursive
 * \return
 */
bool Regola::sortAttributesOfElement(Element* element, const bool isRecursive)
{
    if(NULL != element) {
        UndoSortAttributes *undoCmd = new UndoSortAttributes(element->getUI()->treeWidget(), this, element->indexPath(), isRecursive);
        _undoStack.push(undoCmd);
        setModified(true);
        emit undoStateChanged();
        return true;
    }
    return false;
}

/*!
 * \brief Regola::sortAttributes sorts the attributes in all the elements
 * No undo is possible.
 */
void Regola::sortAttributes()
{
    Utils::TODO_THIS_RELEASE("attenzione: manca la parte di paint e update;");
    clearUndo();

    QVectorIterator<Element*> it(childItems);
    while(it.hasNext()) {
        Element* child = it.next();
        if(child->isElement()) {
            child->sortAttributes(NULL, true);
        }
    }
    setModified(true);
}


