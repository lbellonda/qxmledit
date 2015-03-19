/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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


#include "comparechrome.h"

#define  NEUTRAL_CLR    QColor::fromRgb(192,192,192)
#define  ADDED_CLR    QColor::fromRgb(192,255,192)
#define  DELETED_CLR    QColor::fromRgb(255,192,192)
#define  MODIFIED_CLR    QColor::fromRgb(0xFF,0xDA,0x68)
#define  EQUALS_CLR    QColor::fromRgb(255,255,255)
//#define  EQUALS_CLR    QColor(255,255,255,0)


CompareChrome::CompareChrome()
{
    loadIcons();
}

CompareChrome::~CompareChrome()
{
}

void CompareChrome::loadIcons()
{
    _modIcon.addPixmap(QPixmap(QString::fromUtf8(":/commands/modify")), QIcon::Normal, QIcon::Off);
    _addedIcon.addPixmap(QPixmap(QString::fromUtf8(":/images/add")), QIcon::Normal, QIcon::Off);
    _deletedIcon.addPixmap(QPixmap(QString::fromUtf8(":/images/edit-delete")), QIcon::Normal, QIcon::Off);

    _emptyColor = NEUTRAL_CLR;
    _addedColor = ADDED_CLR ;
    _deletedColor = DELETED_CLR ;
    _modifiedColor = MODIFIED_CLR ;
    _equalsColor = EQUALS_CLR ;

}

QColor CompareChrome::emptyColor() const
{
    return _emptyColor ;
}

QColor CompareChrome::addedColor() const
{
    return _addedColor ;
}

QColor CompareChrome::deletedColor() const
{
    return _deletedColor;
}

QColor CompareChrome::modifiedColor() const
{
    return _modifiedColor;
}

QColor CompareChrome::equalsColor() const
{
    return _equalsColor;
}

QIcon CompareChrome::modIcon() const
{
    return _modIcon;
}

QIcon CompareChrome::addedIcon() const
{
    return _addedIcon;
}

QIcon CompareChrome::deletedIcon() const
{
    return _deletedIcon;
}
