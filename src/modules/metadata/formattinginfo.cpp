/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2017 by Luca Bellonda and individual contributors       *
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

#include "xmlEdit.h"
#include "regola.h"
#include "utils.h"
#include "modules/metadata/metadatainfo.h"
#include "undo/commandaddformattinginfo.h"
#include "undo/commandremoveformattinginfo.h"

XMLIndentationSettings::XMLIndentationSettings()
{
    useFormatting = false ;
    useIndent = false ;
    indent = QXmlEditData::XmlIndentDefault ;
    saveAttrMethod = Regola::SaveAttributesNoSort;
    indentAttributesSetting = QXmlEditData::AttributesIndentationNone;
    indentAttributesColumns = QXmlEditData::XmlIndentAttributesColumnsDefault;
}

XMLIndentationSettings::~XMLIndentationSettings()
{
}

void XMLIndentationSettings::setup(
    const bool puseIndent, const int pindent,
    const Regola::ESaveAttributes psaveAttributesMethod,
    const QXmlEditData::EIndentAttributes pindentAttributesSetting, const int pindentAttributesColumns)
{
    //--
    useIndent = puseIndent ;
    indent = pindent ;
    saveAttrMethod = psaveAttributesMethod ;
    indentAttributesSetting = pindentAttributesSetting;
    indentAttributesColumns = pindentAttributesColumns ;
    //--
}

bool XMLIndentationSettings::applyPreset(const XMLIndentationSettings::ESettings preset)
{
    switch(preset) {
    default:
        return false;
    case PresetApacheFOP:
        setup(true, 2, Regola::SaveAttributesNoSort, QXmlEditData::AttributesIndentationNone, 0);
        break;
    case PresetNoIndent:
        setup(false, 0, Regola::SaveAttributesNoSort, QXmlEditData::AttributesIndentationNone, 0);
        break;
    case Preset2SpacesOneAttributePerLine:
        setup(true, 2, Regola::SaveAttributesNoSort, QXmlEditData::AttributesIndentationMaxCols, 0);
        break;
    }
    useFormatting = true ;
    return true ;
}

//----------------------

bool Regola::readFormattingInfo()
{
    if(decodeFormattingInfo()) {
        return true ;
    } else {
        _formattingInfo = false ;
    }
    return false;
}

bool Regola::decodeFormattingInfo()
{
    //---
    /* breakpoint trap
    extern bool stopMeta;
    if( stopMeta ) {
        stopMeta = true ;
    }*/
    //---
    foreach(Element * topLevel, childItems) {
        if(topLevel->getType() == Element::ET_PROCESSING_INSTRUCTION) {
            if(topLevel->getPITarget() == MetadataInfo::QXMLEDIT_TARGET_PI) {
                MetadataInfo info;
                XMLIndentationSettings settings;
                if(info.parseFormattingInfo(topLevel->getPIData(), -1, &settings)) {
                    applyFormatting(&settings);
                    return true ;
                }
            }
        }
    }
    return false ;
}

Element *Regola::findFormattingInfo()
{
    //---
    foreach(Element * topLevel, childItems) {
        if(topLevel->getType() == Element::ET_PROCESSING_INSTRUCTION) {
            if(topLevel->getPITarget() == MetadataInfo::QXMLEDIT_TARGET_PI) {
                MetadataInfo info;
                XMLIndentationSettings settings;
                if(info.parseFormattingInfo(topLevel->getPIData(), -1, &settings)) {
                    return topLevel;
                }
            }
        }
    }
    return NULL ;
}

void Regola::applyFormatting(XMLIndentationSettings *settings)
{
    _formattingInfo = settings->useFormatting ;
    _overrideGlobalIndentation = true ;
    if(!settings->useIndent) {
        _indent = -1 ;
    } else {
        _indent = settings->indent;
    }
    _saveAttributesMethod = settings->saveAttrMethod;
    _indentAttributes = settings->indentAttributesSetting;
    _attributesIndentSettings = true ;
    _indentAttributesColumns = settings->indentAttributesColumns;
    emitIndentationChange();
}

void Regola::formattingInfoToSettings(XMLIndentationSettings *settings)
{
    settings->useFormatting = _formattingInfo ;
    settings->useIndent = _indent != -1 ;
    settings->indent = _indent ;
    settings->saveAttrMethod = _saveAttributesMethod ;
    settings->indentAttributesSetting = _indentAttributes ;
    settings->indentAttributesColumns = _indentAttributesColumns ;
}

void Regola::updateMetaInfoFormatting()
{
    if(!hasFormattingInfo()) {
        return ;
    }
    Element *fmtInfoElement = findFormattingInfo();
    if(NULL != fmtInfoElement) {
        const QString newSettings = makeFormattingInfo();
        fmtInfoElement->setPIData(newSettings);
    }
    foreach(Element * topLevel, childItems) {
        if(topLevel->getType() == Element::ET_PROCESSING_INSTRUCTION) {
            if(topLevel->getPITarget() == MetadataInfo::QXMLEDIT_TARGET_PI) {
                MetadataInfo info;
                if(info.isFormattingInfo(topLevel->getPIData())) {
                    const QString newSettings = makeFormattingInfo();
                    topLevel->setPIData(newSettings);
                }
            }
        }
    }
}

QString Regola::makeFormattingInfo()
{
    XMLIndentationSettings settings;
    formattingInfoToSettings(&settings);
    const QString newSettings = MetadataInfo::toFormatInfo(&settings);
    return newSettings;
}

Element *Regola::newFormattingInfoElement()
{
    QString newData = makeFormattingInfo();
    Element *newInfo = new Element(this, Element::ET_PROCESSING_INSTRUCTION, NULL);
    newInfo->setPITarget(MetadataInfo::QXMLEDIT_TARGET_PI);
    newInfo->setPIData(newData);
    return newInfo;
}

Element *Regola::insertFormattingInfoWithPosition(const int position)
{
    Element *newInfo = newFormattingInfoElement();
    childItems.insert(position, newInfo);
    newInfo->markEdited();
    return newInfo;
}

bool Regola::insertFormattingInfo(QTreeWidget *tree, const bool undoableOperation)
{
    if(NULL != findFormattingInfo()) {
        return false;
    }
    int position = topPositionForFormattingInfo() ;
    Element *newInfo = insertFormattingInfoWithPosition(position);
    newInfo->caricaFigli(tree, NULL, paintInfo, true, position);
    afterInsertHousekeeping(newInfo, tree, undoableOperation);
    return true;
}

int Regola::topPositionForFormattingInfo()
{
    int position = 0 ;
    if(hasProlog()) {
        position = 1 ;
    }
    return position;
}

bool Regola::addFormattingInfo(QTreeWidget *tree)
{
    if(NULL != findFormattingInfo()) {
        return false;
    }
    int position = topPositionForFormattingInfo() ;
    Element *newInfo = newFormattingInfoElement();
    QList<int> path;
    path << position ;

    CommandAddFormattingInfo *command = new CommandAddFormattingInfo(tree, this, newInfo, path);
    addUndo(command);
    return true;
}

bool Regola::removeFormattingInfo(QTreeWidget *tree)
{
    Element *formattingInfo = findFormattingInfo();
    if(NULL == formattingInfo) {
        return false;
    }
    QList<int> path = formattingInfo->indexPath();

    CommandRemoveFormattingInfo *command = new CommandRemoveFormattingInfo(tree, this, path);
    addUndo(command);
    return true;
}
