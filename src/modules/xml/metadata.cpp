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

/**
 * @brief Regola::updateMetadata update the updatable metadata if existing or insert them
 * @return
 */
void Regola::insertMetadata(QTreeWidget *tree)
{
    //---
    /* breakpoint trap
    extern bool stopMeta;
    if( stopMeta ) {
        stopMeta = true ;
    }*/
    //---
    bool found = false;
    Element *firstElement = NULL ;
    int row = 0 ;
    foreach(Element * topLevel, childItems) {
        if(topLevel->getType() == Element::ET_PROCESSING_INSTRUCTION) {
            if(NULL == firstElement) {
                firstElement = topLevel ;
            }
            if(topLevel->getPITarget() == MetadataInfo::QXMLEDIT_TARGET_PI) {
                MetadataInfo info;
                if(info.parseUpdatable(topLevel->getPIData(), row)) {
                    found = true ;
                    updateMetadataRecord(tree, topLevel, &info, true);
                    break;
                }
            }
            row ++ ;
        }
    }
    if(!found) {
        // insert new metadata
        MetadataInfo newInfo;
        newInfo.markNewRecord();
        QString newData = newInfo.updatable()->toProcessingInstruction();
        newInfo.updatable()->update(false);
        Element *newPI = new Element(this, Element::ET_PROCESSING_INSTRUCTION, NULL);
        newPI->setPITarget(MetadataInfo::QXMLEDIT_TARGET_PI);
        newPI->setPIData(newData);
        newPI->markEdited();
        // at the second place, if possible
        int insertIndex = 0 ;
        if((NULL != firstElement)
                && (firstElement->getType() == Element::ET_PROCESSING_INSTRUCTION)
                && (firstElement->getPITarget().toLower() == "xml")) {
            insertIndex = 1;

        }
        childItems.insert(insertIndex, newPI);
        newPI->caricaFigli(tree, NULL, paintInfo, true, insertIndex);
        afterInsertHousekeeping(newPI, tree, true);
    }
}

void Regola::insertCompleteMetadata(QTreeWidget *tree)
{
    insertMetadata(tree);
    MetadataInfo info;
    parseMetadata(&info);
    //---
    appendAMetadatum(tree, &info.project, MetadataInfo::ProjectMetaType);
    appendAMetadatum(tree, &info.copyright, MetadataInfo::CopyrightMetaType);
    appendAMetadatum(tree, &info.version, MetadataInfo::VersionMetaType);
    appendAMetadatum(tree, &info.domain, MetadataInfo::DomainMetaType);
    appendAMetadatum(tree, &info.name, MetadataInfo::NameMetaType);
    //---

}

void Regola::appendAMetadatum(QTreeWidget *tree, PseudoAttribute *attribute, const QString &type)
{
    if(!attribute->exists()) {
        attribute->setUsed(true);
        Element *newPI = new Element(this, Element::ET_PROCESSING_INSTRUCTION, NULL);
        newPI->setPITarget(MetadataInfo::QXMLEDIT_TARGET_PI);
        newPI->setPIData(QString(" type=\"%1\" %2").arg(type).arg(attribute->toStringStd()));
        newPI->markEdited();
        childItems.append(newPI);
        newPI->caricaFigli(tree, NULL, paintInfo, true);
        afterInsertHousekeeping(newPI, tree, true);
    }
}

void Regola::updateMetadata(QTreeWidget *tree)
{
    int index = 0 ;
    foreach(Element * topLevel, childItems) {
        if((topLevel->getType() == Element::ET_PROCESSING_INSTRUCTION)
                && (topLevel->getPITarget() == MetadataInfo::QXMLEDIT_TARGET_PI)) {
            MetadataInfo info;
            if(info.parseUpdatable(topLevel->getPIData(), index)) {
                updateMetadataRecord(tree, topLevel, &info, false);
                break;
            }
        }
        index ++ ;
    }
}

bool Regola::parseMetadata(MetadataInfo *info)
{
    bool updatableMetadataFound = false;
    int index = 0;
    foreach(Element * topLevel, childItems) {
        if((topLevel->getType() == Element::ET_PROCESSING_INSTRUCTION)
                && (topLevel->getPITarget() == MetadataInfo::QXMLEDIT_TARGET_PI)) {
            bool parsed = false;
            if(!updatableMetadataFound) {
                if(info->parseUpdatable(topLevel->getPIData(), index)) {
                    updatableMetadataFound = true;
                    parsed = true;
                }
            }
            if(!parsed) {
                info->parseOtherMetadata(topLevel->getPIData(), index);
            }
        }
        index++;
    }
    return updatableMetadataFound ;
}


/**
 * @brief Regola::applyMetadata apply metadata, merging it into the editr
 * @param info
 * @return error or not
 * rules for merging:
 * - read: the data are already existing at line "row"
 * - write: the data should be written
 *
 *   READ   |  WRITE |
 * ---------+--------+-----------
 *    0     |   0    | nothing to do
 *    1     |   0    | delete the data
 *    1     |   1    | update the data inline
 *    0     |   1    | add the data at the end of the metadata
 */

bool Regola::applyMetadata(QTreeWidget *tree, MetadataInfo *info)
{
    //---
    bool result = true;
    int operations = 0 ;
    // build a list of commands
    QList<PseudoAttribute*>updateDataList;
    QList<PseudoAttribute*>insertDataList;
    QList<PseudoAttribute*>deleteDataList;
    // populate the lists
    updateDataList  = info->manualAttributeListByCondition(true, true);
    insertDataList  = info->manualAttributeListByCondition(false, true);
    deleteDataList  = info->manualAttributeListByCondition(true, false);

    if((updateDataList.size() + insertDataList.size() + deleteDataList.size()) == 0) {
        return true ;
    }

    _undoStack.beginMacro(tr("Metadata"));
    // update in place
    foreach(PseudoAttribute * attr, updateDataList) {
        int row = attr->row();
        Element *metaElement = topElement(row);
        if((NULL == metaElement)
                || ((NULL != metaElement) && (metaElement->getType() != Element::ET_PROCESSING_INSTRUCTION)
                    && (metaElement->getPITarget() != MetadataInfo::QXMLEDIT_TARGET_PI))) {
            result = false ;
        } else {
            QString newData = attr->toStringComplete();
            UndoEditCommand *undoCommand = new UndoEditCommand(tree, this, metaElement->indexPath());
            undoCommand->setOriginalElement(metaElement);
            metaElement->setPIData(newData);
            metaElement->display(metaElement->getUI(), paintInfo);
            metaElement->markEdited();
            undoCommand->setModifiedElement(metaElement);
            operations++;
            _undoStack.push(undoCommand);
        }
    }

    if(result)  {
        // delete items: to maintain the index valid, first we collect the elements, then we delete them
        QList<Element*> elementsToDelete;
        foreach(PseudoAttribute * attr, deleteDataList) {
            elementsToDelete.append(topElement(attr->row()));
        }
        foreach(Element * metaElement, elementsToDelete) {
            if((NULL == metaElement)
                    || ((NULL != metaElement) && (metaElement->getType() != Element::ET_PROCESSING_INSTRUCTION)
                        && (metaElement->getPITarget() != MetadataInfo::QXMLEDIT_TARGET_PI))) {
                result = false ;
            } else {
                UndoDeleteCommand *undoCmd = new UndoDeleteCommand(tree, this, metaElement->indexPath());
                metaElement->autoDelete(true);
                operations++;
                _undoStack.push(undoCmd);
            }
        }
    }

    if(result)  {
        // queue new items
        foreach(PseudoAttribute * attr, insertDataList) {
            operations++;
            QString newData = attr->toStringComplete();
            Element *newPI = new Element(this, Element::ET_PROCESSING_INSTRUCTION, NULL);
            newPI->setPITarget(MetadataInfo::QXMLEDIT_TARGET_PI);
            newPI->setPIData(newData);
            newPI->markEdited();
            childItems.append(newPI);
            newPI->caricaFigli(tree, NULL, paintInfo, true);
            afterInsertHousekeeping(newPI, tree, false);
            addUndoInsert(tree, newPI);
        }
    }
    _undoStack.endMacro();
    emit undoStateChanged();

    return result;
}

void Regola::updateMetadataRecord(QTreeWidget *tree, Element *metaElement, MetadataInfo *info, const bool metaExists)
{
    info->updatable()->update(metaExists);
    QString newData = info->updatable()->toProcessingInstruction();
    UndoEditCommand *undoCommand = new UndoEditCommand(tree, this, metaElement->indexPath());
    undoCommand->setOriginalElement(metaElement);
    metaElement->setPIData(newData);
    metaElement->display(metaElement->getUI(), paintInfo);
    metaElement->markEdited();
    undoCommand->setModifiedElement(metaElement);
    _undoStack.push(undoCommand);
}


