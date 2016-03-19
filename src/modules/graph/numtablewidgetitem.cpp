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


#include "numtablewidgetitem.h"

NumTableWidgetItem::NumTableWidgetItem() :  QTableWidgetItem(QTableWidgetItem::UserType)
{
}

NumTableWidgetItem::~NumTableWidgetItem()
{
}

bool NumTableWidgetItem::operator< (const QTableWidgetItem & other)  const
{
    QString thisText = text();
    QString otherText = other.text();
    if(thisText.endsWith("%")) {
        thisText = thisText.left(thisText.length() - 1);
    }
    if(otherText.endsWith("%")) {
        otherText = otherText.left(otherText.length() - 1);
    }
    bool true1 = false;
    bool true2 = false;
    double d1 = thisText.toDouble(&true1) ;
    double d2 = otherText.toDouble(&true2);
    if(true1 && true2) {
        return d1 < d2 ;
    }
    return thisText < otherText;
}

