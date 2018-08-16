/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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

#include "paintinfo.h"
#include "qxmleditconfig.h"
#include "utils.h"
#include "modules/services/colormanager.h"

#define MAX_ZOOM    (5)
#define MIN_ZOOM    (0.4)

PaintInfo::PaintInfo()
{
    _attributeFilter = NULL ;
    isChanged = false;
    isOneAttrPerLine = false ;
    isIndexPerChild = false ;
    isCompactView = false;
    isShowAttributesLength = false ;
    isUseFixedLengthFont = false ;
    isShowUnBase64 = false;
    _currentStyle = NULL ;
    zoomFactor = 1 ;
    isShowElementTextLength = false ;
    isShowElementSize = false ;
    isExplore = false ;
    isAttributesHTML = false ;
    isHideView = false ;
    _colorManager = NULL ;
    isShowFullComments = false ;
    _sortAttributesAlpha = false ;
    _attributesColumnLimit = NumColumnsPerAttributeDefault ;
    _showElementsIcon = true ;
    _showLTOnTags = false ;
    _internalColorManager = new ColorManager();
    recalcColumns();
}

PaintInfo::~PaintInfo()
{
    if(NULL != _internalColorManager) {
        delete _internalColorManager ;
    }
}

void PaintInfo::loadState()
{
    isIndexPerChild = Config::getBool(Config::KEY_MAIN_INDEXPERCHILD, true);
    isOneAttrPerLine = Config::getBool(Config::KEY_MAIN_ONEATTRLINE, true);
    isCompactView = Config::getBool(Config::KEY_MAIN_COMPACTVIEW, true);
    isShowAttributesLength = Config::getBool(Config::KEY_MAIN_SHOWATTRLEN, false);
    isUseFixedLengthFont = Config::getBool(Config::KEY_MAIN_ATTRFIXEDS, false);
    isShowUnBase64  = Config::getBool(Config::KEY_MAIN_SHOWBASE64, false);
    isShowElementTextLength = Config::getBool(Config::KEY_MAIN_SHOWELTEXTLEN, false);
    isShowElementSize = Config::getBool(Config::KEY_MAIN_SHOWELSIZE, false);
    isHideView = Config::getBool(Config::KEY_MAIN_HIDEVIEW, false);
    internalSetZoom(Config::getReal(Config::KEY_MAIN_SHOWZOOM, 1));
    isShowFullComments = Config::getBool(Config::KEY_MAIN_SHOWFULLCOMMENTS, false);
    _sortAttributesAlpha = Config::getBool(Config::KEY_MAIN_SORTATTRIBUTESALPHA, false);
    _attributesColumnLimit = Config::getInt(Config::KEY_MAIN_ATTRCOLLLIMIT, NumColumnsPerAttributeDefault) ;
    _showElementsIcon = Config::getBool(Config::KEY_MAIN_SHOWELEMNTSICON, false);
    _showLTOnTags = Config::getBool(Config::KEY_MAIN_SHOWLTONTAGS, Config::ShowLOOnTagsDefault);
    recalcColumns();
    isChanged = false;
}


void PaintInfo::setChanged()
{
    isChanged = true ;
}

bool PaintInfo::saveState()
{
    if(!isChanged) {
        return true;
    }
    bool isOK = true ;
    if(!Config::saveBool(Config::KEY_MAIN_INDEXPERCHILD, indexPerChild())) {
        isOK = false;
    }
    if(!Config::saveBool(Config::KEY_MAIN_ONEATTRLINE, oneAttrPerLine())) {
        isOK = false;
    }
    if(!Config::saveBool(Config::KEY_MAIN_COMPACTVIEW, compactView())) {
        isOK = false;
    }
    if(!Config::saveBool(Config::KEY_MAIN_SHOWATTRLEN, showAttributesLength())) {
        isOK = false;
    }
    if(!Config::saveBool(Config::KEY_MAIN_ATTRFIXEDS, useFixedLengthFont())) {
        isOK = false;
    }
    if(!Config::saveBool(Config::KEY_MAIN_SHOWBASE64, showUnBase64())) {
        isOK = false;
    }
    if(!Config::saveBool(Config::KEY_MAIN_SHOWELTEXTLEN, showElementTextLength())) {
        isOK = false;
    }
    if(!Config::saveBool(Config::KEY_MAIN_SHOWELSIZE, showElementSize())) {
        isOK = false;
    }
    if(!Config::saveReal(Config::KEY_MAIN_SHOWZOOM, zoom())) {
        isOK = false;
    }
    if(!Config::saveBool(Config::KEY_MAIN_HIDEVIEW, hideView())) {
        isOK = false;
    }
    if(!Config::saveBool(Config::KEY_MAIN_SHOWFULLCOMMENTS, showFullComments())) {
        isOK = false;
    }
    if(!Config::saveBool(Config::KEY_MAIN_SORTATTRIBUTESALPHA, isSortAttributesAlpha())) {
        isOK = false;
    }
    if(!Config::saveInt(Config::KEY_MAIN_ATTRCOLLLIMIT, attributesColumnLimit())) {
        isOK = false;
    }
    if(!Config::saveBool(Config::KEY_MAIN_SHOWELEMNTSICON, isShowElementsIcon())) {
        isOK = false;
    }
    if(!Config::saveBool(Config::KEY_MAIN_SHOWLTONTAGS, isShowLTOnTags())) {
        isOK = false;
    }
    return isOK;
}

bool PaintInfo::isShowLTOnTags()
{
    return _showLTOnTags ;
}

void PaintInfo::setShowLTOnTags(const bool value)
{
    bool isDiff = _showLTOnTags != value ;
    if(isDiff) {
        _showLTOnTags = value ;
        isChanged = true ;
        saveState();
    }
}

void PaintInfo::setOneAttrPerLine(const bool newValue)
{
    isOneAttrPerLine = newValue ;
    isChanged = true ;
    saveState();
}

bool PaintInfo::oneAttrPerLine() const
{
    return isOneAttrPerLine;
}

void PaintInfo::setIndexPerChild(const bool newValue)
{
    isIndexPerChild = newValue ;
    isChanged = true ;
    saveState();
}

void PaintInfo::setCompactView(const bool newValue)
{
    isCompactView = newValue ;
    isChanged = true ;
    saveState();
}

bool PaintInfo::indexPerChild() const
{
    return isIndexPerChild ;
}

bool PaintInfo::compactView() const
{
    return isCompactView ;
}

bool PaintInfo::showAttributesLength() const
{
    return isShowAttributesLength ;
}

void PaintInfo::setShowAttributesLength(const bool newValue)
{
    isShowAttributesLength  = newValue ;
    isChanged = true ;
    saveState();
}

bool PaintInfo::useFixedLengthFont() const
{
    return isUseFixedLengthFont ;
}

void PaintInfo::setUseFixedLengthFont(const bool newValue)
{
    isUseFixedLengthFont = newValue ;
    isChanged = true ;
    saveState();
}

void PaintInfo::setCurrentStyle(VStyle *newStyle)
{
    _currentStyle = newStyle ;
}

VStyle *PaintInfo::currentStyle() const
{
    return _currentStyle ;
}

bool PaintInfo::showUnBase64() const
{
    return isShowUnBase64 ;
}

void PaintInfo::setShowUnBase64(const bool newValue)
{
    isShowUnBase64 = newValue ;
    isChanged = true ;
    saveState();
}

qreal PaintInfo::zoom() const
{
    return zoomFactor ;
}

void PaintInfo::setZoom(const qreal newValue)
{
    internalSetZoom(newValue);
    isChanged = true ;
    saveState();
}

bool PaintInfo::isSortAttributesAlpha() const
{
    return _sortAttributesAlpha;
}

void PaintInfo::setSortAttributesAlpha(bool newValue)
{
    _sortAttributesAlpha = newValue;
    isChanged = true ;
    saveState();
}

bool PaintInfo::isLimitAttributesPerColumns() const
{
    return _attributesColumnLimit > 0;
}

int PaintInfo::attributesColumnLimit() const
{
    return _attributesColumnLimit;
}

void PaintInfo::setAttributesColumnLimit(int value)
{
    _attributesColumnLimit = value;
    isChanged = true ;
    saveState();
}

bool PaintInfo::isShowElementsIcon() const
{
    return _showElementsIcon;
}

void PaintInfo::setShowElementsIcon(bool showElementsIcon)
{
    _showElementsIcon = showElementsIcon;
    isChanged = true ;
    saveState();
}

void PaintInfo::internalSetZoom(const qreal newValue)
{
    if(newValue > MAX_ZOOM) {
        zoomFactor = MAX_ZOOM;
    } else if(zoomFactor < MIN_ZOOM) {
        zoomFactor = MIN_ZOOM ;
    } else {
        zoomFactor = newValue ;
    }
}

bool PaintInfo::zoomIn()
{
    bool zoomChanged = false;
    if(zoomFactor < 1) {
        zoomFactor += 0.1;
        if(zoomFactor > 1) {
            zoomFactor = 1 ;
        }
        zoomChanged = true ;
    } else if(zoomFactor < MAX_ZOOM) {
        zoomFactor++;
        zoomChanged = true ;
    }
    if(zoomChanged) {
        if(NULL != _currentStyle) {
            _currentStyle->setZoom(zoomFactor);
        }
    }
    return zoomChanged ;
}

bool PaintInfo::zoomOut()
{
    bool zoomChanged = false;
    if(zoomFactor <= 1) {
        zoomChanged = true;
        zoomFactor -= 0.2;
        if(zoomFactor < MIN_ZOOM) {
            zoomFactor = MIN_ZOOM ;
        }
    } else if(zoomFactor > 1) {
        zoomFactor--;
        if(zoomFactor < 1) {
            zoomFactor = 1;
        }
        zoomChanged = true;
    }
    if(zoomChanged) {
        if(NULL != _currentStyle) {
            _currentStyle->setZoom(zoomFactor);
        }
    }
    return zoomChanged ;
}

bool PaintInfo::canZoomOut()
{
    return (zoomFactor > MIN_ZOOM);
}

bool PaintInfo::canZoomIn()
{
    return (zoomFactor < MAX_ZOOM);
}

bool PaintInfo::showElementTextLength() const
{
    return isShowElementTextLength ;
}

void PaintInfo::setShowElementTextLength(const bool newValue)
{
    isShowElementTextLength  = newValue ;
    isChanged = true ;
    saveState();
}

bool PaintInfo::showElementSize() const
{
    return isShowElementSize ;
}

void PaintInfo::setShowElementSize(const bool newValue)
{
    isShowElementSize  = newValue ;
    recalcColumns() ;
    isChanged = true ;
    saveState();
}

void PaintInfo::setExploreMode(const bool newIsExplore)
{
    isExplore = newIsExplore;
}

bool PaintInfo::exploreMode() const
{
    return isExplore ;
}

void PaintInfo::recalcColumns()
{
    columnForTag = 0 ;

    //if( isShowElementSize ) {
    columnForChildren = 1;
    columnForSize = 2;
    columnForAttributes = 3;
    columnForText = 4;
    columnForDecodedText = 5;
    /*} else {
        //TODO
        columnForChildren = -1;
        columnForSize = -2;

        columnForAttributes = 1;
        columnForText = 2;
        columnForDecodedText = 3;
    }*/
}

void PaintInfo::setAttributesHTML(const bool value)
{
    isAttributesHTML = value ;
}

bool PaintInfo::attributesHTML() const
{
    return isAttributesHTML;
}


bool PaintInfo::hideView() const
{
    return isHideView ;
}

void PaintInfo::setHideView(const bool value)
{
    isHideView = value ;
    isChanged = true ;
    saveState();
}

AttributeFilter* PaintInfo::attributeFilter()
{
    return _attributeFilter ;
}

void PaintInfo::setAttributeFilter(AttributeFilter* value)
{
    _attributeFilter = value ;
}


StyleEntry *PaintInfo::styleEntryById(const QString &id) const
{
    if(NULL == _currentStyle) {
        return NULL ;
    }
    return _currentStyle->getStyleEntry(id);
}

ColorManager *PaintInfo::colorManager() const
{
    if(NULL == _colorManager) {
        return _internalColorManager ;
    }
    return _colorManager;
}

void PaintInfo::setColorManager(ColorManager *newColorManager)
{
    _colorManager = newColorManager;
}

bool PaintInfo::showFullComments()
{
    return isShowFullComments;
}

void PaintInfo::setShowFullComments(const bool value)
{
    isShowFullComments = value ;
    isChanged = true ;
    saveState();
}

bool PaintInfo::updateAttributeColumnsLimit()
{
    int oldValue = _attributesColumnLimit ;
    _attributesColumnLimit = Config::getInt(Config::KEY_MAIN_ATTRCOLLLIMIT, NumColumnsPerAttributeDefault) ;
    return oldValue != _attributesColumnLimit ;
}

bool PaintInfo::updateShowElementIcon()
{
    int oldValue = _showElementsIcon ;
    _showElementsIcon = Config::getBool(Config::KEY_MAIN_SHOWELEMNTSICON, false);
    return oldValue != _showElementsIcon ;
}

bool PaintInfo::updateShowElementTag()
{
    int oldValue = _showLTOnTags ;
    _showLTOnTags = Config::getBool(Config::KEY_MAIN_SHOWLTONTAGS, Config::ShowLOOnTagsDefault);
    return oldValue != _showLTOnTags ;
}

void PaintInfo::applySettings(const bool parmIsOneAttrPerLine,
                              const bool parmIsIndexPerChild,
                              const bool parmIsCompactView,
                              const bool parmIsShowUnBase64,
                              const bool parmIsShowElementTextLength,
                              const bool parmIsShowElementSize,
                              const bool parmIsShowFullComments,
                              const bool parmIsSortAttributesAlpha,
                              const int parmAttributesColumnLimit)
{
    isOneAttrPerLine = parmIsOneAttrPerLine;
    isIndexPerChild = parmIsIndexPerChild;
    isCompactView = parmIsCompactView;
    isShowUnBase64 = parmIsShowUnBase64;
    isShowElementTextLength = parmIsShowElementTextLength;
    isShowElementSize = parmIsShowElementSize ;
    isShowFullComments =  parmIsShowFullComments,
    _sortAttributesAlpha  = parmIsSortAttributesAlpha;
    _attributesColumnLimit = parmAttributesColumnLimit;
}
