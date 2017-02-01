/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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


#include "elementbase.h"

int ElementBase::instances = 0;

ElementBase::ElementBase(ElementBase *newParent, const QString &newName)
{
    id = instances++;
    size = 0 ;
    totalSize = 0 ;
    childrenCount = 0 ;
    totalChildrenCount = 0 ;
    attributesCount = 0;
    totalAttributesSize = 0;
    totalAttributesCount = 0 ;
    text = 0 ;
    totalText = 0 ;
    parent = newParent ;
    name = newName ;
    next = NULL ;
    if(NULL != newParent) {
        newParent->childrenCount++;
        if(NULL != newParent->lastChild) {
            newParent->lastChild->next = this ;
        }
        newParent->lastChild = this ;
        if(NULL == newParent->firstChild) {
            newParent->firstChild = this ;
        }
    }
    firstChild = NULL ;
    lastChild = NULL ;
}

ElementBase::~ElementBase()
{
    ElementBase *e   = firstChild ;
    while(e != NULL) {
        ElementBase *e2   = e->next ;
        delete e;
        e = e2 ;
    }
}

