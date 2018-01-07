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

#include "comparesidebysideview.h"
#include "utils.h"
#include "compareexception.h"


const int CompareSideBySideView::OffsetLastDiff = Qt::UserRole + 0 ;
const int CompareSideBySideView::OffsetThisDiff = Qt::UserRole + 1 ;

//----------------------------------
DiffInfo::DiffInfo(const int newId, const EDiff::KDiff newType, const bool newIsElementOrAttribute, DiffSingleNodeResult *newDiff, Attribute* newAttribute)
{
    id = newId ;
    //position yet to be assigned
    itemPosition = -1 ;
    diffType = newType;
    isElementOrAttribute = newIsElementOrAttribute;
    diff = newDiff ;
    attribute = newAttribute ;
}

DiffInfo::~DiffInfo()
{
}

//----------------------------------

CompareSideBySideView::CompareSideBySideView()
{
    _useItemsFonts = false;
}

CompareSideBySideView::~CompareSideBySideView()
{
}


void CompareSideBySideView::setGraphic(QTreeWidgetItem *currentNodeItem, const EDiff::KDiff state, const bool isEmpty)
{
    if(isEmpty) {
        currentNodeItem->setBackgroundColor(0, _chrome.emptyColor());
        return ;
    }

    bool useIcon = false;
    QIcon icon;
    QColor bkColor;
    switch(state) {
    default: {
        QString msg = tr("Unknown state:%1 in CompareSideBySideView::setGraphic").arg(state);
        Utils::error(msg);
        CompareError(msg);
    }
    break;
    case EDiff::ED_ADDED:
        /*if( isReference ) {*/
        bkColor = _chrome.addedColor();
        /*} else {
            bkColor = emptyColor();
        }*/
        useIcon = true;
        icon = _chrome.addedIcon() ;
        break;
    case EDiff::ED_MODIFIED:
        bkColor = _chrome.modifiedColor();
        useIcon = true;
        icon = _chrome.modIcon();
        break;
    case EDiff::ED_EQUAL:
        bkColor = _chrome.equalsColor();
        break;
    case EDiff::ED_DELETED:
        /*if( isReference ) {
            bkColor = emptyColor();
        } else {*/
        bkColor = _chrome.deletedColor();
        /*}*/
        useIcon = true;
        icon = _chrome.deletedIcon() ;
        break;
    }
    currentNodeItem->setBackgroundColor(0, bkColor);
    if(useIcon) {
        currentNodeItem->setIcon(0, icon);
    }
}

QString CompareSideBySideView::getElidedText(const QString &textIn)
{
    QString text = textIn;
    text = text.trimmed();
    text = text.replace("\n", "");
    text = text.replace("\r", "");
    if(text.length() > 20) {
        text = text.left(20);
        text.append("...");
    }
    return text ;
}

QString CompareSideBySideView::getText(SourceElementDiffOperation *source, const EDiff::KDiff state, const bool isEmpty)
{
    if(isEmpty) {
        return "" ;
    }
    QString text;
    bool useText = false ;
    switch(state) {
    default: {
        QString msg = tr("Unknown state:%1 in setup diff").arg(state);
        Utils::error(msg);
        CompareError(msg);
    }
    break;
    case EDiff::ED_ADDED:
        useText = true ;
        break;
    case EDiff::ED_MODIFIED:
        useText = true ;
        break;
    case EDiff::ED_EQUAL:
        useText = true ;
        break;
    case EDiff::ED_DELETED:
        useText = true ;
        break;
    }

    if(useText) {
        Element *element = source->element();
        switch(element->getType()) {
        case Element::ET_ELEMENT:
            text = QString("<%1>").arg(element->tag());
            break;
        case Element::ET_PROCESSING_INSTRUCTION:
            text = QString("<? %1 %2 ?>").arg(element->getPITarget()).arg(element->getPIData());
            break;
        case Element::ET_COMMENT:
            text = getElidedText(element->text);
            text = QString("Comment: %1").arg(text);
            break;
        case Element::ET_TEXT:
            text = getElidedText(element->text);
            if(element->isCDATA()) {
                text = QString("Text CDATA: [[%1]]").arg(text);
            } else {
                text = QString("Text: %1").arg(text);
            }
            break;
        default:
            break;
        }
    }
    return text ;
}

bool CompareSideBySideView::isNodeEmpty(DiffSingleNodeResult *node, const bool isReference)
{
    if((isReference && (node->type() == EDiff::ED_DELETED))
            || (!isReference && (node->type() == EDiff::ED_ADDED))) {
        return true ;
    }
    return false ;
}


SourceElementDiffOperation *CompareSideBySideView::getSource(DiffSingleNodeResult *node, const bool isReference)
{
    SourceElementDiffOperation *source ;
    bool isEmpty = isNodeEmpty(node, isReference) ;

    if((isEmpty && isReference) || (!isEmpty && !isReference)) {
        source = node->compareElement();
    } else {
        source = node->referenceElement();
    }
    return source ;
}


void CompareSideBySideView::setupItem(DiffSingleNodeResult *node, QTreeWidgetItem *currentNodeItem, const bool isReference, QList<QTreeWidgetItem*> &itemsList, QList<DiffInfo*> &diffList, const bool isChildrenBlocked)
{
    SourceElementDiffOperation *sourceRef = node->referenceElement();
    SourceElementDiffOperation *compareRef = node->compareElement();
    SourceElementDiffOperation *source = getSource(node, isReference);
    bool isEmpty = isNodeEmpty(node, isReference);
    QString text = getText(source, node->type(), isEmpty);
    setGraphic(currentNodeItem, node->type(), isEmpty) ;

    currentNodeItem->setText(0, text);

    if(node->hasAttributes()) {
        QTreeWidgetItem *attributeTextItem = newTreeItem(itemsList);
        if(isReference) {
            DiffInfo *info = NULL ;
            // if blocked, use the last item
            if(isChildrenBlocked) {
                info = diffList.at(diffList.size() - 1);
            }
            setLastItemDiff(isReference, itemsList, diffList, info);
        }
        if(!isEmpty) {
            //attributeTextItem->setBackgroundColor(0, colo);aaa
            attributeTextItem->setBackgroundColor(0, currentNodeItem->backgroundColor(0));
            attributeTextItem->setText(0, tr("Attributes:"));
        } else {
            attributeTextItem->setBackgroundColor(0, _chrome.emptyColor());
        }
        currentNodeItem->addChild(attributeTextItem);

        // added only if it is the reference
        bool emptyAddedAttributes = !isReference || isEmpty ;
        bool emptyDeletedAttributes = isReference || isEmpty ;

        if(NULL != sourceRef) {
            appendAttributes(node, attributeTextItem, EDiff::ED_ADDED, isReference, emptyAddedAttributes, sourceRef->addedAttributes(), itemsList, diffList, isChildrenBlocked);
        }
        appendAttributes(node, attributeTextItem, EDiff::ED_MODIFIED, isReference, isEmpty, source->modifiedAttributes(), itemsList, diffList, isChildrenBlocked);
        appendAttributes(node, attributeTextItem, EDiff::ED_EQUAL, isReference, isEmpty, node->equalsAttributes(), itemsList, diffList, isChildrenBlocked);

        if(NULL != compareRef) {
            appendAttributes(node, attributeTextItem, EDiff::ED_DELETED, isReference, emptyDeletedAttributes, compareRef->addedAttributes(), itemsList, diffList, isChildrenBlocked);
        }
    }
}


void CompareSideBySideView::appendAttributes(DiffSingleNodeResult *node, QTreeWidgetItem *attributeTextItem, const EDiff::KDiff state, const bool isReference, const bool isEmpty, QList<Attribute*> &attributes, QList<QTreeWidgetItem*> &itemsList, QList<DiffInfo*> &diffList, const bool isChildrenBlocked)
{
    foreach(Attribute * attr, attributes) {
        appendAttribute(node, attributeTextItem, attr, state, isReference, isEmpty, itemsList, diffList, isChildrenBlocked);
    }
}

void CompareSideBySideView::appendAttribute(DiffSingleNodeResult *node, QTreeWidgetItem *attributeTextItem, Attribute *attribute, const EDiff::KDiff state, const bool isReference, const bool isEmpty, QList<QTreeWidgetItem*> &itemsList, QList<DiffInfo*> &diffList, const bool isChildrenBlocked)
{
    QString text;
    if(!isEmpty) {
        text = QString("%1 = \"%2\"").arg(attribute->name).arg(attribute->value) ;
    }

    QTreeWidgetItem *attributeItem = newTreeItem(itemsList);
    if(!isEmpty) {
        bool useIcon = false;
        bool useColor = false;
        QColor bkColor ;
        QIcon icon ;
        switch(state) {
        case EDiff::ED_EQUAL:
            useColor = true;
            bkColor = _chrome.equalsColor();
            break;
        case EDiff::ED_ADDED:
            bkColor = _chrome.addedColor();
            icon = _chrome.addedIcon() ;
            useColor = true;
            useIcon = true;
            break;
        case EDiff::ED_MODIFIED:
            bkColor = _chrome.modifiedColor();
            icon = _chrome.modIcon() ;
            useColor = true;
            useIcon = true;
            break;
        case EDiff::ED_DELETED:
            bkColor = _chrome.deletedColor();
            icon = _chrome.deletedIcon() ;
            useColor = true;
            useIcon = true;
            break;
        default:
            break;
        }
        if(useColor) {
            attributeItem->setBackgroundColor(0, bkColor);
        }
        if(useIcon) {
            attributeItem->setIcon(0, icon);
        }
    } else {
        attributeItem->setBackgroundColor(0, _chrome.emptyColor());
    }
    attributeItem->setText(0, text);
    attributeTextItem->addChild(attributeItem);
    if(isReference) {
        DiffInfo *info = NULL ;
        // if bloccato, use the last item
        if(isChildrenBlocked) {
            info = diffList.at(diffList.size() - 1);
        } else {
            bool createDiff = (EDiff::ED_EQUAL != state);
            if(createDiff) {
                info = createNewDiff(diffList, state, false, node, attribute);
            }
        }
        setLastItemDiff(isReference, itemsList, diffList, info);
    }
}

DiffInfo* CompareSideBySideView::createNewDiff(QList<DiffInfo*> &diffList, const EDiff::KDiff diffType, const bool isElementOrAttribute, DiffSingleNodeResult *newDiff, Attribute* attribute)
{
    DiffInfo* diffInfo = new DiffInfo(diffList.size(), diffType, isElementOrAttribute, newDiff, attribute);
    diffList.append(diffInfo);
    return diffInfo;
}


void CompareSideBySideView::setLastItemDiff(const bool isReference, QList<QTreeWidgetItem*> &itemsList, QList<DiffInfo*> &diffList, DiffInfo *info)
{
    if(isReference) {
        QTreeWidgetItem* lastItem = itemsList.last();
        int diffLastIndex = diffList.size() - 1 ;
        int lastDiffIndex ;
        int thisDiffIndex ;
        // this works for 0 index also
        if(NULL == info) {
            thisDiffIndex = -1 ;
            lastDiffIndex = diffLastIndex;
        } else {
            if(info->itemPosition == -1) {
                info->itemPosition = itemsList.size() - 1 ;
            }
            thisDiffIndex = diffLastIndex ;
            if(diffLastIndex >= 0) {
                lastDiffIndex = diffLastIndex - 1;
            } else {
                lastDiffIndex = -1 ;
            }
        }

        QVariant lastIndexVariant(lastDiffIndex);
        lastItem->setData(0, OffsetLastDiff, lastIndexVariant);
        QVariant thisIndexVariant(thisDiffIndex);
        lastItem->setData(0, OffsetThisDiff, thisIndexVariant);
    }
}


int CompareSideBySideView::lastDiffIndexFromItem(QTreeWidgetItem *item)
{
    QVariant data = item->data(0, OffsetLastDiff);
    return data.toInt();
}

int CompareSideBySideView::thisDiffIndexFromItem(QTreeWidgetItem *item)
{
    QVariant data = item->data(0, OffsetThisDiff);
    return data.toInt();
}

QTreeWidgetItem *CompareSideBySideView::newTreeItem(QList<QTreeWidgetItem*> &itemsList)
{
    QTreeWidgetItem *newItem = new QTreeWidgetItem();
    if(_useItemsFonts) {
        newItem->setFont(0, _itemsFonts);
    }
    itemsList.append(newItem);
    return newItem ;
}


void CompareSideBySideView::loadSinglePart(DiffSingleNodeResult *node, QTreeWidgetItem *parentItem, QTreeWidget *tree, const bool isReference, QList<QTreeWidgetItem*> &itemsList, QList<DiffInfo*> &diffList, const bool diffChildrenBlocked)
{
    bool blockChildren = true ;
    DiffInfo *diffInfo = NULL ;
    if(isReference) {
        if(!diffChildrenBlocked) {
            switch(node->type()) {
            default:
                break;
            case EDiff::ED_EQUAL:
                blockChildren = false ;
                break;
            case EDiff::ED_MODIFIED:
                blockChildren = false ;
                diffInfo = createNewDiff(diffList, EDiff::ED_MODIFIED, true, node, NULL);
                break;
            case EDiff::ED_ADDED:
            case EDiff::ED_DELETED:
                // block them
                blockChildren = true ;
                diffInfo = createNewDiff(diffList, node->type(), true, node, NULL);
                break;
            }
        } else {
            blockChildren = true ;
            diffInfo = diffList.at(diffList.size() - 1);
        }
    }

    QTreeWidgetItem *currentNodeItem = newTreeItem(itemsList);
    setLastItemDiff(isReference, itemsList, diffList, diffInfo);
    setupItem(node, currentNodeItem, isReference, itemsList, diffList, blockChildren);
    if(NULL == parentItem) {
        tree->addTopLevelItem(currentNodeItem);
    } else {
        parentItem->addChild(currentNodeItem);
    }
    foreach(DiffSingleNodeResult * childNode, node->children()) {
        loadSinglePart(childNode, currentNodeItem, NULL, isReference, itemsList, diffList, blockChildren);
    }
}

void CompareSideBySideView::loadSinglePart(QList<DiffSingleNodeResult *>& nodes, QTreeWidget *tree, const bool isReference, QList<QTreeWidgetItem*> &itemsList, QList<DiffInfo*> &diffList)
{
    itemsList.clear();

    foreach(DiffSingleNodeResult * singleDiff,  nodes) {
        loadSinglePart(singleDiff, NULL, tree, isReference, itemsList, diffList, false);
    }
}

void CompareSideBySideView::setItemsFonts(const QFont &newFont)
{
    _itemsFonts = newFont ;
    _useItemsFonts = true ;
}

