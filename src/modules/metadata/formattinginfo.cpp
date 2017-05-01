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

void Regola::applyFormatting(XMLIndentationSettings *settings)
{
    _formattingInfo = settings->useFormatting ;
    _useIndent = settings->useIndent;
    if(!_useIndent) {
        _indent = -1 ;
    } else {
        _indent = settings->indent;
    }
    _saveAttributesMethod = settings->saveAttrMethod;
    _indentAttributes = settings->indentAttributesSetting;
    _attributesIndentSettings = (settings->indentAttributesSetting == QXmlEditData::AttributesIndentationMaxCols);
    _indentAttributesColumns = settings->indentAttributesColumns;
}

void Regola::formattingInfoToSettings(XMLIndentationSettings *settings)
{
    settings->useFormatting = _formattingInfo ;
    settings->useIndent = _useIndent ;
    settings->indent = _indent ;
    settings->saveAttrMethod = _saveAttributesMethod ;
    settings->indentAttributesSetting = _indentAttributes ;
    if(_attributesIndentSettings) {
        settings->indentAttributesSetting = QXmlEditData::AttributesIndentationMaxCols;
    } else {
        settings->indentAttributesSetting = QXmlEditData::AttributesIndentationNone;
    }
    settings->indentAttributesColumns = _indentAttributesColumns ;
}

void Regola::updateMetaInfoFormatting()
{
    if(!hasFormattingInfo()) {
        return ;
    }
    foreach(Element * topLevel, childItems) {
        if(topLevel->getType() == Element::ET_PROCESSING_INSTRUCTION) {
            if(topLevel->getPITarget() == MetadataInfo::QXMLEDIT_TARGET_PI) {
                MetadataInfo info;
                if(info.isFormattingInfo(topLevel->getPIData())) {
                    XMLIndentationSettings settings;
                    formattingInfoToSettings(&settings);
                    const QString newSettings = info.toFormatInfo(&settings);
                    topLevel->setPIData(newSettings);
                }
            }
        }
    }
}
