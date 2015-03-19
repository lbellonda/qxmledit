/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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

#include "xsdhelperoperations.h"
#include "element.h"

//-------

XSDOper::XSDOper()
{
    childrenMutuallyExclusive = false;
    add = false ;
    canStay = false;
}

XSDOper::~XSDOper()
{
    reset();
}

void XSDOper::reset()
{
    while(!children.isEmpty()) {
        delete children.at(0);
        children.removeAt(0);
    }
}


XSDOper *XSDOper::findOper(Element *element)
{
    QString name = element->localName();
    foreach(XSDOper * childOper, children) {
        if(childOper->name == name) {
            return childOper ;
        }
    }
    return NULL ;
}
