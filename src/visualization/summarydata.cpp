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

#include "xmlEdit.h"
#include "summarydata.h"

//----------------------------------------

SummaryData::SummaryData()
{
    reset();
}

SummaryData::~SummaryData()
{
}

void SummaryData::reset()
{
    totalFragments = 0 ;
    totalAttributes = 0 ;
    totalElements = 0 ;
    totalSize = 0 ;
    totalText = 0 ;
    totalAttributesSize = 0 ;

    maxAttributes = 0 ;
    maxChildren = 0 ;
    maxSize = 0 ;
    maxText = 0 ;
    maxAttributeSizePerElement = 0 ;

    /*attributesPerFragmentMean = 0 ;
    attributesPerFragmentMedian = 0 ;
    sizePerFragmentMean = 0 ;
    sizePerFragmentMedian = 0 ;
    elementsPerFragmentMean = 0 ;
    elementsPerFragmentMedian = 0 ;*/
    levels = 0 ;
}

double SummaryData::meanAttributesSize()
{
    if(totalAttributes != 0) {
        return totalAttributesSize / totalAttributes ;
    }
    return 0;
}
