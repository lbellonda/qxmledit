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

#ifndef ATTRIBUTESSUMMARYDATA_H
#define ATTRIBUTESSUMMARYDATA_H

#include "xmlEdit.h"
#include "libQXmlEdit_global.h"

class LIBQXMLEDITSHARED_EXPORT AttributeSummaryData
{
public:
    QString name;
    long count;
    long dataSize;
    long countEmpty;
    AttributeSummaryData();
    ~AttributeSummaryData();

    void setData(const long newCount, const long newDataSize, const long newCountEmpty);

    long sizeInMemory();
    long sizeCharacters();
    long sizeEmpty();
    long sizeEmptyInMemory();
};


class LIBQXMLEDITSHARED_EXPORT AttributesSummaryData
{
public:
    QHash<QString, AttributeSummaryData*> data;
    QSet<QString> blackList;
    QSet<QString> whiteList;

    AttributesSummaryData();
    ~AttributesSummaryData();

    AttributeSummaryData * attributeSummaryData(const QString &path, const QString &name);
    bool isUsed(const QString &key);

    void reset();
    void resetLists();
    bool compareTo(AttributesSummaryData *other, QString &reason);
    bool loadFileAttributeList(QWidget *window, const QString &filePath, const bool isWhitelist);

    static qreal calcPerc(const long value, const long total);
};

class LIBQXMLEDITSHARED_EXPORT AttributesSummaryTotal
{
public:
    long attributesCount;
    long hitCount;
    long sizeInMemory;
    long sizeCharacters;
    long meanSize;
    long emptyCount;
    long sizeEmptyData;
    long sizeMemoryEmptyData;
    qreal percTotalSize;

    void includeAttribute(AttributeSummaryData *attributeSummaryData);
    void calcEnd(const long sizeFile);

    AttributesSummaryTotal();
    ~AttributesSummaryTotal();

    void setData(
        const long attributesCount,
        const long hitCount,
        const long sizeInMemory,
        const long sizeCharacters,
        const long meanSize,
        const long emptyCount,
        const long sizeEmptyData,
        const long sizeMemoryEmptyData,
        const qreal percTotalSize);
    bool compareTo(AttributesSummaryTotal *other, QString &reason);

};


class LIBQXMLEDITSHARED_EXPORT AttributesSummarySummary
{
private:
    void calculateBlock(QMap<QString, QString> attrsKeys, AttributesSummaryData* attributesSummaryData, AttributesSummaryTotal &total, const long sizeFile);
public:
    AttributesSummarySummary();
    ~AttributesSummarySummary();

    long sizeFile ;
    AttributesSummaryTotal totalTotal, totalUsed, totalNotUsed;
    QMap<QString, QString> attrsUsedKeys ;
    QMap<QString, QString> attrsNotUsedKeys ;

    bool calculate(AttributesSummaryData* attributesSummaryData);
};

#endif // ATTRIBUTESSUMMARYDATA_H
