/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2017 by Luca Bellonda and individual contributors       *
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

#include "attributessummarydata.h"
#include "utils.h"

//----------------------------------------

AttributeSummaryData::AttributeSummaryData()
{
    count = 0;
    dataSize = 0;
    countEmpty = 0 ;
}

AttributeSummaryData::~AttributeSummaryData()
{
}

void AttributeSummaryData::setData(const long newCount, const long newDataSize, const long newCountEmpty)
{
    count = newCount ;
    dataSize = newDataSize ;
    countEmpty = newCountEmpty;
}

long AttributeSummaryData::sizeInMemory()
{
    // name+ptr to data and storage into element
    return (2 * name.length()) + (count * 16) + (dataSize * 2);
}

long AttributeSummaryData::sizeCharacters()
{
    // on file a="XX"
    return (count * (name.length() + 4)) + dataSize;
}

long AttributeSummaryData::sizeEmpty()
{
    // on file?
    return (countEmpty * (name.length() + 4));
}

long AttributeSummaryData::sizeEmptyInMemory()
{
    // empty reference
    return (countEmpty * 8) ;
}

//----------------------------------------

AttributesSummaryData::AttributesSummaryData()
{
}

AttributesSummaryData::~AttributesSummaryData()
{
    reset();
}

void AttributesSummaryData::reset()
{
    EMPTYPTRLIST(data, AttributeSummaryData);
}

AttributeSummaryData * AttributesSummaryData::attributeSummaryData(const QString &path, const QString &name)
{
    AttributeSummaryData * attributeSummaryData = data[path];
    if(NULL == attributeSummaryData) {
        attributeSummaryData = new AttributeSummaryData();
        attributeSummaryData->name = name ;
        data.insert(path, attributeSummaryData);
    }
    return attributeSummaryData;
}


void AttributesSummaryData::resetLists()
{
    blackList.clear();
    whiteList.clear();
}

bool AttributesSummaryData::isUsed(const QString &key)
{
    bool isUsedResult = false;
    const bool isWhitelist = !whiteList.isEmpty();
    const bool isBlacklist = !blackList.isEmpty();
    if(isWhitelist) {
        if(whiteList.contains(key)) {
            isUsedResult = true ;
        } else {
            if(isBlacklist) {
                if(blackList.contains(key)) {
                    isUsedResult = false ;
                } else {
                    isUsedResult = true ;
                }
            } else {
                isUsedResult = false ;
            }
        }
    } else {
        if(blackList.contains(key)) {
            isUsedResult = false ;
        } else {
            isUsedResult = true ;
        }
    }
    return isUsedResult ;
}

qreal AttributesSummaryData::calcPerc(const long value, const long total)
{
    qreal result = 0 ;
    if(total != 0) {
        const long longPercTotalSize = (1000 * value) / total ;
        result = (qreal)longPercTotalSize / 10.0 ;
    }
    return result ;
}

bool AttributesSummaryData::compareTo(AttributesSummaryData *other, QString &reason)
{
    if(data.size() != other->data.size()) {
        reason = "data.size";
        return false;
    }
    foreach(const QString &key, data.keys()) {
        AttributeSummaryData *thisData = data[key];
        AttributeSummaryData *otherData = other->data[key];
        if(NULL == otherData) {
            reason = "key not found";
            return false;
        }
        if(thisData->name != otherData->name) {
            reason = "name";
            return false;
        }
        if(thisData->count != otherData->count) {
            reason = "count";
            return false;
        }
        if(thisData->dataSize != otherData->dataSize) {
            reason = "dataSize";
            return false;
        }
        if(thisData->countEmpty != otherData->countEmpty) {
            reason = "countEmpty";
            return false;
        }
    }
    return true ;
}

bool AttributesSummaryData::loadFileAttributeList(QWidget *window, const QString &filePath, const bool isWhitelist)
{
    if(filePath.isEmpty()) {
        Utils::errorFilePathInvalid(window);
        return false ;
    }
    //--- set data
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Utils::error(window, QObject::tr("An error occurred opening the file."));
        return false ;
    }
    if(isWhitelist) {
        whiteList.clear();
    } else {
        blackList.clear();
    }
    QTextStream inputStream(&file);
    QString line ;
    do {
        line = inputStream.readLine();
        QString data = line.trimmed();
        if(!data.isEmpty()) {
            if(isWhitelist) {
                whiteList.insert(data);
            } else  {
                blackList.insert(data);
            }
        }
    } while(!line.isNull());

    if(file.error() != QFile::NoError) {
        file.close();
        Utils::error(window, QObject::tr("An error occurred reading data."));
        return false;
    }

    file.close();

    return true ;
}


//----------------------------------------------------------

AttributesSummaryTotal::AttributesSummaryTotal()
{
    attributesCount = 0;
    hitCount = 0;
    sizeInMemory = 0;
    sizeCharacters = 0;
    meanSize = 0;
    emptyCount = 0;
    percTotalSize = 0;
    sizeEmptyData = 0;
    sizeMemoryEmptyData = 0;
}

AttributesSummaryTotal::~AttributesSummaryTotal()
{
}

void AttributesSummaryTotal::setData(
    const long newattributesCount,
    const long newhitCount,
    const long newsizeInMemory,
    const long newsizeCharacters,
    const long newmeanSize,
    const long newemptyCount,
    const long newsizeEmptyData,
    const long newsizeMemoryEmptyData,
    const qreal newpercTotalSize)
{
    attributesCount = newattributesCount;
    hitCount = newhitCount;
    sizeInMemory = newsizeInMemory;
    sizeCharacters = newsizeCharacters;
    meanSize = newmeanSize;
    emptyCount = newemptyCount;
    sizeEmptyData = newsizeEmptyData;
    sizeMemoryEmptyData = newsizeMemoryEmptyData;
    percTotalSize = newpercTotalSize;
}

bool AttributesSummaryTotal::compareTo(AttributesSummaryTotal *other, QString &reason)
{
    if(attributesCount != other->attributesCount) {
        reason = "attributesCount";
        return false;
    }
    if(hitCount != other->hitCount) {
        reason = "hitCount";
        return false;
    }
    if(sizeInMemory != other->sizeInMemory) {
        reason = "sizeInMemory";
        return false;
    }
    if(sizeCharacters != other->sizeCharacters) {
        reason = "sizeCharacters";
        return false;
    }
    if(meanSize != other->meanSize) {
        reason = "meanSize";
        return false;
    }
    if(emptyCount != other->emptyCount) {
        reason = "emptyCount";
        return false;
    }
    if(sizeEmptyData != other->sizeEmptyData) {
        reason = "sizeEmptyData";
        return false;
    }
    if(sizeMemoryEmptyData != other->sizeMemoryEmptyData) {
        reason = "sizeMemoryEmptyData";
        return false;
    }
    if(percTotalSize != other->percTotalSize) {
        reason = "percTotalSize";
        return false;
    }
    return true ;
}

void AttributesSummaryTotal::includeAttribute(AttributeSummaryData *attributeSummaryData)
{
    attributesCount ++ ;
    hitCount += attributeSummaryData->count;
    emptyCount += attributeSummaryData->countEmpty;
    sizeInMemory += attributeSummaryData->sizeInMemory();
    sizeCharacters += attributeSummaryData->sizeCharacters();
    sizeEmptyData += attributeSummaryData->sizeEmpty();
    sizeMemoryEmptyData += attributeSummaryData->sizeEmptyInMemory();
}

void AttributesSummaryTotal::calcEnd(const long sizeFile)
{
    if(sizeCharacters != 0) {
        meanSize = sizeCharacters / hitCount ;
    } else {
        meanSize = 0 ;
    }
    percTotalSize = AttributesSummaryData::calcPerc(sizeCharacters, sizeFile);
}

//------------------------------------------------------------
AttributesSummarySummary::AttributesSummarySummary()
{
    sizeFile = 0 ;
}

AttributesSummarySummary::~AttributesSummarySummary()
{
}

void AttributesSummarySummary::calculateBlock(QMap<QString, QString> attrsKeys, AttributesSummaryData* attributesSummaryData, AttributesSummaryTotal &total, const long sizeFile)
{
    foreach(const QString & key, attrsKeys.keys()) {
        AttributeSummaryData* attributeSummaryData = attributesSummaryData->data[key] ;
        total.includeAttribute(attributeSummaryData);
    }
    total.calcEnd(sizeFile);
}

bool AttributesSummarySummary::calculate(AttributesSummaryData* attributesSummaryData)
{
    if(NULL == attributesSummaryData) {
        return false;
    }
    sizeFile = 0 ;
    foreach(const QString & key, attributesSummaryData->data.keys()) {
        const bool isUsed = attributesSummaryData->isUsed(key);
        if(isUsed) {
            attrsUsedKeys.insert(key, key);
        } else {
            attrsNotUsedKeys.insert(key, key);
        }
        sizeFile += attributesSummaryData->data[key]->sizeCharacters();
    }
    if(!attrsUsedKeys.isEmpty()) {
        calculateBlock(attrsUsedKeys, attributesSummaryData, totalUsed, sizeFile);
    }
    if(!attrsNotUsedKeys.isEmpty()) {
        calculateBlock(attrsNotUsedKeys, attributesSummaryData, totalNotUsed, sizeFile);
    }
    totalTotal.attributesCount = totalNotUsed.attributesCount + totalUsed.attributesCount;
    totalTotal.hitCount = totalNotUsed.hitCount + totalUsed.hitCount;
    totalTotal.emptyCount = totalNotUsed.emptyCount + totalUsed.emptyCount;
    totalTotal.sizeCharacters = totalNotUsed.sizeCharacters + totalUsed.sizeCharacters;
    totalTotal.sizeInMemory = totalNotUsed.sizeInMemory + totalUsed.sizeInMemory;
    if(attrsUsedKeys.isEmpty()) {
        totalTotal.meanSize = totalNotUsed.meanSize ;
    } else if(attrsNotUsedKeys.isEmpty()) {
        totalTotal.meanSize = totalUsed.meanSize ;
    } else {
        totalTotal.meanSize = (totalNotUsed.sizeCharacters + totalUsed.sizeCharacters) / (totalNotUsed.hitCount + totalUsed.hitCount);
    }
    totalTotal.sizeEmptyData = totalNotUsed.sizeEmptyData + totalUsed.sizeEmptyData;
    totalTotal.sizeMemoryEmptyData = totalNotUsed.sizeMemoryEmptyData + totalUsed.sizeMemoryEmptyData;
    totalTotal.percTotalSize = 100 ;
    if((totalNotUsed.percTotalSize + totalUsed.percTotalSize) > 100) {
        return false;
    }
    return true;
}
