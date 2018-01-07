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


#ifndef DISPLAYSTYLESETTING_H
#define DISPLAYSTYLESETTING_H

#include "xmlEdit.h"
class PaintInfo ;

class DisplayStyleSetting
{
public:
    // settings
    bool isOneAttrPerLine ;
    bool isIndexPerChild  ;
    bool isCompactView ;
    bool isShowUnBase64;
    bool isShowElementTextLength ;
    bool isShowElementSize  ;
    bool isShowFullComments ;
    bool isSortAttributesAlpha  ;
    int attributesColumnLimit;
    //---
    QString description;
public:
    DisplayStyleSetting();
    ~DisplayStyleSetting();

    void makeEssential();
    void makeCompact();
    void makeExtended();
    void makeFullDetail();
    void makePredefined();
    void applyToPaintInfo(PaintInfo *paintInfo);
};

#endif // DISPLAYSTYLESETTING_H
