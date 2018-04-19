/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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

#include "regoladefinitions.h"
#include "undo/undoaddchildcontainer.h"
#include "undo/undoaddparentcommand.h"
#include "undo/undoremoveparentcommand.h"
//-----

//------------region(utility methods)

// note: if there is a text, there is not a child and vice versa
// insert a parent for the elements at pos X and for len insLen

void Regola::insertParentForElement(QTreeWidget *tree, Element *element, Element *newParent, const int insPos, const int insLen, const bool replaceUI)
{
    Element *grandParent = element->parent() ;
    int indexOf = insPos ;
    int indexFirstItem = insPos ;
    QVector<Element*> &parentVector = element->getParentChildren();
    QList<Element*> elementsToRemove;
    for(int i = 0 ; i < insLen ; i++) {
        Element *e = parentVector.at(indexOf);
        elementsToRemove.append(e);
        parentVector.remove(indexOf); // vector shifts
        if(replaceUI) {
            if(NULL != grandParent) {
                int indexItem = grandParent->getUI()->indexOfChild(e->getUI());
                grandParent->getUI()->takeChild(indexItem);
                indexFirstItem = indexItem;
            } else {
                int indexItem = tree->indexOfTopLevelItem(e->getUI());
                tree->takeTopLevelItem(indexItem);
                indexFirstItem = indexItem;
            }
        }
    }

    if(replaceUI) {
        if(NULL != grandParent) {
            grandParent->getUI()->insertChild(indexFirstItem, newParent->getUI());
        } else {
            // Top level
            tree->insertTopLevelItem(indexFirstItem, newParent->getUI());
        }
    }
    // insert the new parent
    parentVector.insert(indexOf, newParent);
    newParent->setParent(grandParent);

    if(NULL == grandParent) {
        syncRoot();
    }

    // insert all the children
    foreach(Element * el, elementsToRemove) {
        newParent->addChild(el);
        if(replaceUI) {
            newParent->getUI()->addChild(el->getUI());
        }
    }
}


bool Regola::removeParentForChildAndMoveOtherNodes(QTreeWidget *tree, Element *element, int &insPos, int &insLen, const bool replaceUI)
{
    insPos = - 1;
    insLen = 0 ;
    Element *parent = element->parent() ;

    // check for double root element
    if(NULL == parent) {
        int elements = 0;
        foreach(Element * el, element->getChildItemsRef()) {
            if(el->isElement()) {
                elements++;
                if(elements > 1) {
                    return false;
                }
            }
        }
    }


    int indexOf = element->indexOfSelfAsChild();
    QVector<Element*> &parentVector = element->getParentChildren();
    // removes the parent
    parentVector.remove(indexOf);
    if(replaceUI) {
        QList<QTreeWidgetItem*> items = element->getUI()->takeChildren();
        if(NULL != parent) {
            int indexItem = parent->getUI()->indexOfChild(element->getUI());
            parent->getUI()->takeChild(indexItem);
            parent->getUI()->insertChildren(indexItem, items);
        } else {
            // Top level
            int indexItem = tree->indexOfTopLevelItem(element->getUI());
            tree->takeTopLevelItem(indexItem);
            tree->insertTopLevelItems(indexItem, items);
        }
    }
    int indexInsert = indexOf ;
    insPos = indexOf ;
    insLen = 0;
    foreach(Element * el, element->getChildItemsRef()) {
        parentVector.insert(indexInsert, el);
        el->setParent(parent);
        indexInsert++;
        insLen++;
    }
    element->getChildItemsRef().clear();
    syncRoot();
    return true;
}

//------------endregion(utility methods)


//------------region(command interface)

// insert a child that contains all the existing children
void Regola::insertChildContainer(QTreeWidget *tree, Element *parentElement)
{
    if((NULL != parentElement) && parentElement->isElement()) {
        Element *theNewElement = new Element("", "", NULL, NULL);
        bool result = editNodeElement(tree->window(), theNewElement, parentElement, false);
        if(result) {
            doInsertChildContainer(tree, theNewElement->tag(), theNewElement->getAttributesList(), parentElement);
        }
        if(NULL != theNewElement) {
            delete theNewElement ;
        }
    }
}

void Regola::insertDisabledParent(QTreeWidget *tree, Element *element)
{
    if(NULL != element) {
        bool createNS = false;
        QString xsltNameSpacePrefix = namespacePrefixXslt();
        if(xsltNameSpacePrefix.isEmpty()) {
            Element *parent = element->parent();
            if(NULL == parent) {
                parent = element ;
            }
            xsltNameSpacePrefix = unusedPrefixByElement(parent);
            createNS = true ;
        }
        const QString elementTag = XmlUtils::makeQualifiedName(xsltNameSpacePrefix, "if");
        Element *theNewElement = new Element(elementTag, "", NULL, NULL);
        theNewElement->setAttribute("test", "false()");
        if(createNS) {
            theNewElement->setAttribute(QString("xmlns:%1").arg(xsltNameSpacePrefix), Regola::XsltNameSpace);
        }
        doInsertParent(tree, theNewElement->tag(), theNewElement->getAttributesList(), element);
        if(NULL != theNewElement) {
            delete theNewElement ;
        }
    }
}

void Regola::insertParent(QTreeWidget *tree, Element *element, const bool useTextualInterface)
{
    if(NULL != element) {
        Element *theNewElement = new Element("", "", NULL, NULL);
        bool result ;
        if(useTextualInterface) {
            result = editNodeElementWithTextEditor(tree->window(), theNewElement);
        } else {
            result = editNodeElement(tree->window(), theNewElement, (NULL != element->parent()) ? element->parent()->parent() : NULL,  false);
        }
        if(result) {
            doInsertParent(tree, theNewElement->tag(), theNewElement->getAttributesList(), element);
        }
        if(NULL != theNewElement) {
            delete theNewElement ;
        }
    }
}

void Regola::removeParent(QTreeWidget *tree, Element *childElement)
{
    if((NULL != childElement)) {
        QList<int> elementPath = childElement->indexPath();
        if(elementPath.size() > 0) {
            elementPath.removeLast();
            UndoRemoveParentCommand *undoRemoveParentCommandInternal = new UndoRemoveParentCommand(tree, this, elementPath);
            _undoStack.push(undoRemoveParentCommandInternal);
            emit undoStateChanged();
        }
    }
}

//------------endregion(command interface)

//------------region(command interface internals)

void Regola::doInsertChildContainer(QTreeWidget *tree, const QString &tag, QList<Attribute *> attributesIn, Element *parentElement)
{
    UndoAddChildContainerCommand *undoInsertChildContainerInternal = new UndoAddChildContainerCommand(tree, this, tag, attributesIn, parentElement->indexPath());
    _undoStack.push(undoInsertChildContainerInternal);
    emit undoStateChanged();
}

void Regola::doInsertParent(QTreeWidget *tree, const QString &tag, QList<Attribute*> attributesIn, Element *parentElement)
{
    UndoAddParentCommand *undoAddParentInternal = new UndoAddParentCommand(tree, this, tag, attributesIn, parentElement->indexPath());
    _undoStack.push(undoAddParentInternal);
    emit undoStateChanged();
}


//------------endregion(command interface internals)


//------------region(undoredo)

bool Regola::insertChildContainerAction(Element *parentElement, const QString &tag, QList<Attribute*> attributesIn, QTreeWidget *tree)
{
    Element *theNewElement = new Element(tag, "", this, NULL);
    if(NULL == theNewElement) {
        return false;
    }
    theNewElement->setAttributes(attributesIn);
    // this reparents the UI too
    theNewElement->createUI(NULL, paintInfo, true);
    parentElement->moveChildrenTo(theNewElement);
    parentElement->addChild(theNewElement);
    parentElement->moveUIChildrenTo(theNewElement);
    //theNewElement->caricaFigli(tree, NULL, paintInfo, non ricorsivo);
    // recalc size
    theNewElement->updateSizeInfo();
    parentElement->expand(tree);
    parentElement->markEdited();
    afterInsertHousekeeping(theNewElement, tree, false);
    return true;
}

void Regola::removeChildContainerAction(Element *parentElement, QTreeWidget *tree)
{
    if(parentElement->getChildItemsCount() != 1) {
        return ;
    }
    Element * containerElement = parentElement->getChildAt(0);
    if(!containerElement->isElement()) {
        return ;
    }

    parentElement->getChildItems()->clear();
    containerElement->moveChildrenTo(parentElement);
    containerElement->moveUIChildrenTo(parentElement);
    takeOutElement(containerElement) ;
    containerElement->deleteUI();
    delete containerElement;
    // this reparents the UI too
    // recalc size
    parentElement->updateSizeInfo();
    parentElement->expand(tree);
    parentElement->markEdited();
    afterInsertHousekeeping(parentElement, tree, false);
}


bool Regola::insertParentAction(Element *element, const QString &tag, QList<Attribute*> attributesIn, QTreeWidget *tree, const int insPos, const int insLen)
{
    Element *theNewParent = new Element(tag, "", this, NULL);
    if(NULL == theNewParent) {
        return false;
    }
    theNewParent->setAttributes(attributesIn);
    // this reparents the UI too
    theNewParent->createUI(NULL, paintInfo, true);
    insertParentForElement(tree, element, theNewParent, insPos, insLen, true);

    // recalc size
    theNewParent->updateSizeInfo();
    theNewParent->expand(tree);
    theNewParent->markEdited();
    afterInsertHousekeeping(theNewParent, tree, false);
    return true;
}

bool Regola::removeParentAction(Element *element, QTreeWidget *tree, int &insPos, int &insLen)
{
    insPos = -1 ;
    insLen = 0 ;
    Element *parentElement = element->parent() ;
    if(!element->isElement()) {
        return false;
    }

    removeParentForChildAndMoveOtherNodes(tree, element, insPos, insLen, true);
    element->deleteUnbindUI();
    delete element;
    // this reparents the UI too
    // recalc size
    Element *newParent = parentElement;
    if(NULL == newParent) {
        newParent = root();
    }
    if(NULL != newParent) {
        newParent->updateSizeInfo();
        newParent->expand(tree);
        newParent ->markEdited();
        afterInsertHousekeeping(newParent, tree, false);
    }
    return true;
}

//------------endregion(undoredo)

QString Regola::unusedPrefixByElement(Element *element)
{
    if(NULL != element) {
        PrefixInfo info;
        element->collectAllPrefixes(info, element, true, false);
        {
            const QString stdPrefix = "xsl";
            if(!info.allPrefixes.contains(stdPrefix)) {
                return stdPrefix;
            }
        }
        int index = 0 ;
        do {
            const QString newPrefix = QString("xsl%1").arg(index);
            if(!info.allPrefixes.contains(newPrefix)) {
                return newPrefix ;
            }
            index ++ ;
        } while(true);
    }
    return namespacePrefixXslt();
}
