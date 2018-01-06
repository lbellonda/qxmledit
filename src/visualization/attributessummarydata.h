/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2017-2018 by Luca Bellonda and individual contributors  *
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
    quint64 count;
    quint64 dataSize;
    quint64 countEmpty;
    AttributeSummaryData();
    ~AttributeSummaryData();

    void setData(const quint64 newCount, const quint64 newDataSize, const quint64 newCountEmpty);
    void addHit(const int newSize);

    quint64 sizeInMemory();
    quint64 sizeCharacters();
    quint64 sizeEmpty();
    quint64 sizeEmptyInMemory();
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
    bool hasLists() const ;
    void insertInBlackList(const QString &data);
    void insertInWhiteList(const QString &data);

    static qreal calcPerc(const quint64 value, const quint64 total);
};

class LIBQXMLEDITSHARED_EXPORT AttributesSummaryTotal
{
public:
    quint64 attributesCount;
    quint64 hitCount;
    quint64 sizeInMemory;
    quint64 sizeCharacters;
    quint64 meanSize;
    quint64 emptyCount;
    quint64 sizeEmptyData;
    quint64 sizeMemoryEmptyData;
    qreal percTotalSize;

    void includeAttribute(AttributeSummaryData *attributeSummaryData);
    void calcEnd(const quint64 sizeFile);

    AttributesSummaryTotal();
    ~AttributesSummaryTotal();

    void setData(
        const quint64 attributesCount,
        const quint64 hitCount,
        const quint64 sizeInMemory,
        const quint64 sizeCharacters,
        const quint64 meanSize,
        const quint64 emptyCount,
        const quint64 sizeEmptyData,
        const quint64 sizeMemoryEmptyData,
        const qreal percTotalSize);
    bool compareTo(AttributesSummaryTotal *other, QString &reason);
};


class LIBQXMLEDITSHARED_EXPORT AttributesSummarySummary
{
private:
    void calculateBlock(QMap<QString, QString> attrsKeys, AttributesSummaryData* attributesSummaryData, AttributesSummaryTotal &total, const quint64 sizeFile);
public:
    AttributesSummarySummary();
    ~AttributesSummarySummary();

    quint64 sizeFile ;
    AttributesSummaryTotal totalTotal, totalUsed, totalNotUsed;
    QMap<QString, QString> attrsUsedKeys ;
    QMap<QString, QString> attrsNotUsedKeys ;

    bool calculate(AttributesSummaryData* attributesSummaryData);
};

#endif // ATTRIBUTESSUMMARYDATA_H
