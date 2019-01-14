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

#include "elementviewinfo.h"
#include "utils.h"

ElementViewInfo::ElementViewInfo()
{
    _useFgBrush = false ;
    _showAllComments = false ;
    _isAnonPreview = false ;
    _origDataForAnonPreview = NULL ;
    _isTextAnon = false;
    _attrTextInfoIsHtml = false;
}

ElementViewInfo::~ElementViewInfo()
{
}


void ElementViewInfo::reset()
{
    _icon = QIcon();
    _attributesIcon = QIcon();
    QString empty = "";
    _tagInfo = empty ;
    _childrenSizeInfo = empty ;
    _attrTextInfo = empty ;
    _inlineTextInfo = empty ;
    _b64Info = empty ;
    _useFgBrush = false ;
    _attrTextInfoIsHtml = false;
    _isTextAnon = false;
}

void ElementViewInfo::fillDebug()
{
    _icon = QIcon();
    _attributesIcon = QIcon();
    _tagInfo = "<element>" ;
    _childrenSizeInfo = "Ch:10(20) Sz:100(200)";
    _attrTextInfo = "<html><body>a='b'<body></html>" ;
    _inlineTextInfo = "bau bau bau" ;
    _b64Info = "" ;
    _attrTextInfoIsHtml = true;
    _showAllComments = true ;
    _isTextAnon = false;
}
