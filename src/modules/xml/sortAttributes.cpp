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

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
static bool sortFunctionCaseInsensitive(const QString &firstParameter, const QString &secondParameter)
{
    return firstParameter.toLower() < secondParameter.toLower();aaac
}
#endif

/*!
 * \brief Element::sortAttributes sorts the attributes of element registering their
 * original position in a list passed as parameter if it is not null
 * \param undoPositionList
 */
void Element::sortAttributes(QList<int> *undoPositionList, const bool isRecursive)
{
    QHash<QString, int> unsortedPosAndOriginalPosition;
    QHash<QString, Attribute*> well;
    QVector<Attribute*> newPos;
    QStringList names;
    int index = 0;
    foreach(Attribute * attribute, attributes) {
        names << attribute->name;
        unsortedPosAndOriginalPosition.insert(attribute->name, index);
        well.insert(attribute->name, attribute);
        index ++ ;
    }
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    names.sort(Qt::CaseInsensitive);
#else
    qSort(names.begin(), names.end(), sortFunctionCaseInsensitive);
#endif
    foreach(const QString & key, names) {
        newPos.append(well.value(key));
        if((NULL != undoPositionList) && !isRecursive) {
            undoPositionList->append(unsortedPosAndOriginalPosition.value(key));
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

QList<Attribute*> Element::sortAttributesList(const QVector<Attribute *> &attributes)
{
    QStringList attrNamesList;
    QHash<QString, Attribute*> unsortedCollection;
    foreach(Attribute * attr, attributes) {
        if(NULL != attr) {
            attrNamesList << attr->name;
            unsortedCollection.insert(attr->name, attr);
        }
    }
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    attrNamesList.sort(Qt::CaseInsensitive);
#else
    qSort(attrNamesList.begin(), attrNamesList.end(), sortFunctionCaseInsensitive);
#endif
    QList<Attribute*> result;
    foreach(const QString & key, attrNamesList) {
        Attribute* value = unsortedCollection[key];
        result.append(value);
    }
    return result;
    //---
}

