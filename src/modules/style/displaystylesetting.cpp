/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016-2018 by Luca Bellonda and individual contributors  *
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

#include "displaystylesetting.h"
#include "paintinfo.h"
#include "utils.h"

DisplayStyleSetting::DisplayStyleSetting()
{
    isOneAttrPerLine = false;
    isIndexPerChild = false ;
    isCompactView = false;
    isShowUnBase64 = false;
    isShowElementTextLength = false ;
    isShowElementSize = false ;
    isShowFullComments = false ;
    isSortAttributesAlpha = false ;
    attributesColumnLimit = -1 ;
}

DisplayStyleSetting::~DisplayStyleSetting()
{
}

void DisplayStyleSetting::makeEssential()
{
    isOneAttrPerLine = false;
    isIndexPerChild = false ;
    isCompactView = true;
    isShowUnBase64 = false;
    isShowElementTextLength = false ;
    isShowElementSize = false ;
    isShowFullComments = false ;
    isSortAttributesAlpha = true ;
    attributesColumnLimit = -1 ;
    description = QObject::tr("Essential");
}

void DisplayStyleSetting::makeCompact()
{
    isOneAttrPerLine = true ;
    isIndexPerChild = true ;
    isCompactView = true;
    isShowUnBase64 = false;
    isShowElementTextLength = false ;
    isShowElementSize = false ;
    isShowFullComments = false ;
    isSortAttributesAlpha = true ;
    attributesColumnLimit = 80 ;
    description = QObject::tr("Compact");
}

void DisplayStyleSetting::makeExtended()
{
    isOneAttrPerLine = true;
    isIndexPerChild = true ;
    isCompactView = false;
    isShowUnBase64 = false;
    isShowElementTextLength = false ;
    isShowElementSize = false ;
    isShowFullComments = false ;
    isSortAttributesAlpha = true ;
    attributesColumnLimit = 80 ;
    description = QObject::tr("Extended");
}

void DisplayStyleSetting::makeFullDetail()
{
    isOneAttrPerLine = true;
    isIndexPerChild = true ;
    isCompactView = false;
    isShowUnBase64 = false;
    isShowElementTextLength = true ;
    isShowElementSize = true ;
    isShowFullComments = true ;
    isSortAttributesAlpha = true ;
    attributesColumnLimit = 80 ;
    description = QObject::tr("Full Detail");
}

void DisplayStyleSetting::makePredefined()
{
    isOneAttrPerLine = true;
    isIndexPerChild = true ;
    isCompactView = true;
    isShowUnBase64 = false;
    isShowElementTextLength = false ;
    isShowElementSize = false ;
    isShowFullComments = false ;
    isSortAttributesAlpha = true ;
    attributesColumnLimit = PaintInfo::NumColumnsPerAttributeDefault ;
    description = QObject::tr("Predefined");
}

void DisplayStyleSetting::applyToPaintInfo(PaintInfo *paintInfo)
{
    paintInfo->applySettings(isOneAttrPerLine,
                             isIndexPerChild,
                             isCompactView,
                             isShowUnBase64,
                             isShowElementTextLength,
                             isShowElementSize,
                             isShowFullComments,
                             isSortAttributesAlpha,
                             attributesColumnLimit);
}
