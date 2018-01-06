/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013-2018 by Luca Bellonda and individual contributors  *
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


#ifndef BINARYVIEWERMODEL_H
#define BINARYVIEWERMODEL_H

#include <QAbstractItemModel>
#include <QFile>
#include <QTextCodec>

#include "modules/binaryviewer/binaryblock.h"


class BinaryViewerOperationResult
{
public:

    enum errors {
        BNO_ERROR,
        SEEK_FAILED_ERROR,
        READ_ERROR
    };

    bool ok;
    int errorCode;
    int page;
    int row;
    BinaryViewerOperationResult();
    ~BinaryViewerOperationResult();

    void setError(const errors error);
};


class BinaryViewerModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    // Pages are composed by an integral number of blocks, so the pages can use an integral number of lines and blocks.

    static const int NumColumns = 3;

    static const int OffsetColumn = 0 ;
    static const int DataColumn = 1 ;
    static const int ValueColumn = 2 ;

    static const int MaxBlocksInCache = 10;

    // vediamo di allargare poi
    static const int BlocksPerPage = 16;
    static const int RowsPerPage = BlocksPerPage * BinaryBlock::RowsPerBlock;
    static const int SizeOfPage = BlocksPerPage * BinaryBlock::SizeOfBlock;

    static const int SearchBufferSize = 100 * 1024;

    enum ErrCodes {
        EC_NOERROR = 0,
        EC_FILEALREADYOPENED,
        EC_OTHERERROR,
        EC_FILEEMPTY
    };
private:
    QString _filePath;
    QIODevice *_io;

    qint64 _numPages;
    qint64 _numRows;
    qint64 _numBlocks;
    qint64 _totalData;
    //qint64 _rowsFetched;

    qint64 _lastPage;
    qint64 _lastBlock;
    qint64 _lastPageSize;
    qint64 _lastPageRows;
    qint64 _lastRowOfLastPage;
    quint64 _lastRowSize;
    quint64 _lastBlockSize;

    qint64 _currentPage;
    int _rowsOfThisPage;

    bool _isLastPage;
    QTextCodec *_codec ;

    QList<BinaryBlock*>_blocksList;
public:
    explicit BinaryViewerModel(QObject *parent = 0);
    ~BinaryViewerModel();

    BinaryViewerModel::ErrCodes setFile(QIODevice *newIoDevice);
    //BinaryViewerModel::ErrCodes setDummyData();
    int currentPage();
    void setPage(const int newPage);
    bool isFirstPage();
    bool isLastPage();
    int numPages();
    void goToPage(const bool isNextPage);
    void goToPageAbs(const int newPage);
    bool setCodecByName(const QString &codecName);
    bool findOccurrences(BinaryViewerOperationResult &result, const QString &searchLemma, const int startPage, const int startLine, const bool searchForward);
    bool findPageOfAddress(BinaryViewerOperationResult &result, const qint64 address);

    //------------------------------------------------
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
signals:
    void numberPopulated(int number);
    void pageChanged(int currentPage);
    //------------------------------------------------

private:
    void errorFile();
    void init();
    void calcSize(const qint64 size);
    void reset();
    QString formatOffsetOfRow(const int row) const ;
    QByteArray getRowData(const int nRow);
    BinaryBlock *readBlock(const qint64 realBlockAddress);
    void closeIO();
    bool isOpenIO();
    //---
    bool findOccurrencesBinary(BinaryViewerOperationResult &result, const QByteArray &reference, const int startPage, const int startLine, const bool searchForward);
    bool findOccurrenceInBuffer(BinaryViewerOperationResult &result, const QByteArray &reference, const QByteArray &buffer, const qint64 absoluteOffsetOfBuffer, const bool searchForward);
};

#endif // BINARYVIEWERMODEL_H
