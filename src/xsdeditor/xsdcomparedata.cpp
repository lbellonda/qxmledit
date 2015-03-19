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


#include "xsdcomparedata.h"
#include "xschema.h"
#include "utils.h"

XSDCompareData::XSDCompareData(QList<XSchemaObject*> &referenceList, QList<XSchemaObject*> &targetList)
{
    referenceCount = referenceList.count();
    targetCount = targetList.count();
    foreach(XSchemaObject * child, targetList) {
        targetCollection.append(child);
    }
    indexTarget  = 0 ;
}

XSDCompareData::~XSDCompareData()
{
}

int XSDCompareData::nextIndexTarget(const int indexTarget)
{
    int result = indexTarget + 1;
    return result;
}


