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

#ifndef PAINTINFO_H
#define PAINTINFO_H

#include "libQXmlEdit_global.h"
#include "xmlEdit.h"
#include "style.h"

#include "attributefilter.h"

class ColorManager;

class LIBQXMLEDITSHARED_EXPORT PaintInfo
{
public:
    static const int NumColumnsPerAttributeDefault = 60 ;
private:
    bool isChanged;
    //----------
    bool isOneAttrPerLine;
    bool isIndexPerChild;
    bool isCompactView;
    bool isShowAttributesLength;
    bool isUseFixedLengthFont;
    bool isShowUnBase64;
    int  zoomFactor;
    VStyle*_currentStyle;
    bool isShowElementTextLength;
    bool isShowElementSize ;
    bool isExplore;
    bool isAttributesHTML;
    bool isHideView;
    ColorManager *_colorManager;
    ColorManager *_internalColorManager;
    bool isShowFullComments;
    bool _sortAttributesAlpha;
    int _attributesColumnLimit;
    bool _showElementsIcon;
    bool _showLTOnTags;

    void internalSetZoom(const int newValue);
    void recalcColumns();

    AttributeFilter *_attributeFilter;

public:
    PaintInfo();
    ~PaintInfo();

    void loadState();
    bool saveState();
    void setChanged();

    void cloneTo(PaintInfo *other);

    void setIndexPerChild(const bool newValue);
    void setOneAttrPerLine(const bool newValue);
    void setCompactView(const bool newValue);
    bool oneAttrPerLine() const ;
    bool indexPerChild() const;
    bool compactView() const;
    bool showUnBase64() const;
    VStyle *currentStyle() const ;
    int zoom() const ;
    bool showElementTextLength() const;
    bool showElementSize() const;
    bool exploreMode() const;
    bool attributesHTML() const;
    bool hideView() const;
    StyleEntry *styleEntryById(const QString &id) const ;
    bool showFullComments();
    bool isShowLTOnTags();

    bool showAttributesLength() const;
    void setShowAttributesLength(const bool newValue);
    bool useFixedLengthFont() const;
    void setUseFixedLengthFont(const bool newValue);
    void setCurrentStyle(VStyle *newStyle);
    void setShowUnBase64(const bool newValue);
    void setZoom(const int newValue);
    void setShowElementTextLength(const bool newValue);
    void setShowElementSize(const bool newValue);
    void setExploreMode(const bool isExplore);
    void setAttributesHTML(const bool value);
    void setHideView(const bool value);
    void setShowFullComments(const bool value);
    void setShowLTOnTags(const bool value);

    bool zoomIn();
    bool zoomOut();

    bool canZoomOut();
    bool canZoomIn();

    AttributeFilter* attributeFilter();
    void setAttributeFilter(AttributeFilter* value);

    //------------------------------------------------------------

    int columnForTag ;
    int columnForAttributes ;
    int columnForText ;
    int columnForDecodedText ;
    int columnForChildren ;
    int columnForSize ;

    ColorManager *colorManager() const;
    void setColorManager(ColorManager *colorManager);
    bool isSortAttributesAlpha() const;
    void setSortAttributesAlpha(bool sortAttributesAlpha);
    bool isLimitAttributesPerColumns() const;
    int attributesColumnLimit() const;
    void setAttributesColumnLimit(int attributesColumnLimit);
    bool updateAttributeColumnsLimit();
    bool isShowElementsIcon() const;
    void setShowElementsIcon(bool showElementsIcon);
    bool updateShowElementIcon();
    bool updateShowElementTag();
    //-------
    void applySettings(const bool parmIsOneAttrPerLine,
                       const bool parmIsIndexPerChild,
                       const bool parmIsCompactView,
                       const bool parmIsShowUnBase64,
                       const bool parmIsShowElementTextLength,
                       const bool parmIsShowElementSize,
                       const bool parmIsShowFullComments,
                       const bool parmIsSortAttributesAlpha,
                       const int parmAttributesColumnLimit);
    //-------
};

#endif
