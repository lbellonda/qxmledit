/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2018 by Luca Bellonda and individual contributors  *
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


#include "visdatamap.h"

//test: 5M : 62.000
//100M 1 946 626

VisDataMap::VisDataMap(QObject *parent) :
    QObject(parent)
{
    numColumns = 0 ;
    maxData = 0 ;
    reset();
}

VisDataMap::~VisDataMap()
{
    reset();
}

void VisDataMap::reset()
{
    maxSize = 0 ;
    maxChildrenCount = 0 ;
    maxAttributesCount = 0;
    maxText = 0;
    totalAttributesSize = 0 ;
    maxAttributesSizePerElement = 0 ;
    numElements = 0 ;
    workRow.clear();
    clearRows();
}

void VisDataMap::resetData()
{
    _root = NULL ;
}

void VisDataMap::clearRows()
{
    foreach(VisDataRow * row, rows) {
        delete row;
    }
    rows.clear();
}

void VisDataMap::calculate(ElementBase *root)
{
    _root = root;
    workRow.clear();
    clearRows();
    numColumns = 0 ;
    maxData = 0 ;
    reset();
    if(NULL != root) {
        buildMap(_root, 0);
    }
}

// when no children, the line is consolidated.
void VisDataMap::buildMap(ElementBase *element, const int level)
{
    workRow.append(element);
    if(element->firstChild == NULL) {
        consolidateLine();
    } else {
        ElementBase * child = element->firstChild;
        while(child != NULL) {
            buildMap(child, level + 1);
            child = child->next;
        }
    }
    workRow.remove(workRow.size() - 1);
}

void VisDataMap::consolidateLine()
{
    VisDataRow *newRow = new VisDataRow();
    newRow->_numColumns = workRow.size() ;
    if(numColumns < newRow->_numColumns) {
        numColumns = newRow->_numColumns;
    }
    newRow->_columns = new ElementBase * [workRow.size()];
    ElementBase ** p = newRow->_columns ;
    for(int i = 0 ; i < newRow->_numColumns ; i++) {
        *p++ = workRow.at(i);
    }
    rows.append(newRow);
}
