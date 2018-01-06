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

#include "binaryviewermodel.h"
#include "utils.h"

#include <QBuffer>
#include <QByteArray>


//------

BinaryViewerOperationResult::BinaryViewerOperationResult()
{
    ok = true ;
    errorCode = BNO_ERROR ;
    page = -1 ;
    row = -1;
}

BinaryViewerOperationResult::~BinaryViewerOperationResult()
{
}


void BinaryViewerOperationResult::setError(const errors error)
{
    ok = false;
    errorCode = error;
}

//------

/* TESTS TO IMPLEMENT:
 *0 lettura dati
 *1 lettura encoding
 *2 salva encoding
 *3 traduzione in testo
 *4 setta pagina
 *5 cerca traduci in binario
 *6 esegui cerca forward
 *7 esegui cerca indietro
 *8 dati del file
 *9 costruisci file di prova
 *10 carica file
 *11 assegna file
 *12 go to address
 *13 drop file
 *14 errori nelle operazioni
 *15 reload
 **/

BinaryViewerModel::BinaryViewerModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    _io = NULL;
    init();
}

BinaryViewerModel::~BinaryViewerModel()
{
    reset();
    closeIO();
}

void BinaryViewerModel::closeIO()
{
    if(NULL != _io) {
        if(_io->isOpen()) {
            _io->close();
        }
        delete _io;
        _io = NULL ;
    }
}

void BinaryViewerModel::reset()
{
    foreach(BinaryBlock * blk, _blocksList) {
        delete blk;
    }
    _blocksList.clear();
}

void BinaryViewerModel::init()
{
    _io = NULL ;
    _codec = QTextCodec::codecForName("ISO-8859-15");
    _numRows = 0 ;
    _totalData = 0 ;
    //_rowsFetched = 0;
    _numPages = 0;
    _currentPage = 0;
    _isLastPage = false;
    _rowsOfThisPage = 0;
}
//----------

void BinaryViewerModel::errorFile()
{
    Utils::error(tr("Error reading data"));
}

/*
BinaryViewerModel::ErrCodes BinaryViewerModel::setDummyData()
{
    _io = new QBuffer();
    _io->open(QIODevice::ReadWrite);
    quint64 i ;
    for(i = 0 ; i < ((SizeOfPage * 3) / 2) ; i ++) {
        char ch[4];
        ch[0] = i & 0x00FF;
        //ch[1] = (i&0x00FF00)>>8;
        //ch[2] = (i&0x00FF0000)>>16;
        //ch[3] = (i&0xFF000000)>>24;
        _io->write(&ch[0], 1);
        //_io->write(&ch[1], 1);
        //_io->write(&ch[2], 1);
        //_io->write(&ch[3], 1);
    }
    calcSize(i);
    //calcSize(Q_INT64_C(4000000000));
    //_totalData =  Q_INT64_C(4100000);
    setPage(0);
    return EC_NOERROR ;
}
*/

void BinaryViewerModel::calcSize(const qint64 newSize)
{
    _totalData = newSize ;
    _numRows = _totalData / BinaryBlock::DataPerRow ;
    if((_totalData % BinaryBlock::DataPerRow) > 0) {
        _numRows++;
    }
    _numBlocks = _totalData / BinaryBlock::SizeOfBlock ;
    if((_totalData % BinaryBlock::SizeOfBlock) > 0) {
        _numBlocks ++ ;
    }
    _numPages = _totalData / SizeOfPage ;
    if((_totalData % SizeOfPage) > 0) {
        _numPages ++ ;
    }
    //------
    _lastPage = _numPages - 1;
    _lastBlock = _numBlocks - 1;
    //-----
    _lastPageSize = _totalData % SizeOfPage ;
    if((0 == _lastPageSize) && (_totalData > 0)) {
        _lastPageSize = SizeOfPage ;
    }
    _lastBlockSize = _totalData % BinaryBlock::SizeOfBlock ;
    if((0 == _lastBlockSize) && (_totalData > 0)) {
        _lastBlockSize = BinaryBlock::SizeOfBlock;
    }

    _lastPageRows = _numRows % RowsPerPage ;
    if((0 == _lastPageRows) && (_numRows > 0)) {
        _lastPageRows = RowsPerPage ;
    }
    _lastRowOfLastPage = _lastPageRows - 1;
    _lastRowSize = _totalData % BinaryBlock::SizeOfRow;
    if((0 == _lastRowSize) && (_totalData > 0)) {
        _lastRowSize = BinaryBlock::SizeOfRow;
    }
}

bool BinaryViewerModel::isOpenIO()
{
    return (NULL != _io) && _io->isOpen();
}

BinaryViewerModel::ErrCodes BinaryViewerModel::setFile(QIODevice *newIoDevice)
{
    closeIO();
    _io =  newIoDevice;
    if(_io->open(QIODevice::ReadOnly)) {
        //-- base data
        if(!_io->seek(0)) {
            errorFile();
        } else {
            quint64 fileSize = _io->size();
            if(0 == fileSize) {
                return EC_FILEEMPTY;
            }
            calcSize(fileSize);
            setPage(0);
        }

        return EC_NOERROR ;
    }

    return EC_OTHERERROR;
}

int BinaryViewerModel::currentPage()
{
    return _currentPage ;
}

void BinaryViewerModel::setPage(const int newPage)
{
    if((newPage < 0) || (newPage >= _numPages)) {
        Utils::error(tr("Invalid page number in binary model"));
        return ;
    }
    reset();
    _currentPage = newPage ;
    if(_currentPage == _lastPage) {
        _rowsOfThisPage = _lastPageRows ;
    } else {
        _rowsOfThisPage = RowsPerPage ;
    }
    emit pageChanged(_currentPage);
}

bool BinaryViewerModel::isFirstPage()
{
    return _currentPage == 0 ;
}

bool BinaryViewerModel::isLastPage()
{
    return _currentPage == _lastPage ;
}

int BinaryViewerModel::numPages()
{
    return _numPages ;
}

void BinaryViewerModel::goToPage(const bool isNextPage)
{
    setPage(_currentPage + (isNextPage ? 1 : -1));
}

void BinaryViewerModel::goToPageAbs(const int newPage)
{
    setPage(newPage);
}


QString BinaryViewerModel::formatOffsetOfRow(const int row) const
{
    qint64 address = (_currentPage * SizeOfPage) + (row * BinaryBlock::DataPerRow) ;
    return QString("%1 (0x%2)").arg(address).arg(address, 0, 16);
}


QVariant BinaryViewerModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }
    int row = index.row();

    if((row < 0) || (row >= _numRows)) {
        return QVariant();
    }
    switch(role) {
    case Qt::ToolTipRole:
        //return element->columnViewTooltipData();
        break;
    case Qt::DisplayRole:
        switch(index.column()) {
        case OffsetColumn:
            return formatOffsetOfRow(index.row());
        case DataColumn: {
            QByteArray array = const_cast<BinaryViewerModel*>(this)->getRowData(row);
            QString res;
            int iMax = array.length() ;
            for(int i = 0 ; i < iMax ; i ++) {
                if((i > 0) && ((i & 0x3) == 0)) {
                    res += "| ";
                }
                res += QString("%1 ").arg((array.at(i) & 0x00FF), 2, 16, QChar('0')).toUpper();
            }
            return res;
        }
        case ValueColumn: {
            QByteArray array = const_cast<BinaryViewerModel*>(this)->getRowData(row);
            QString result ;
            if(NULL != _codec) {
                result = _codec->toUnicode(array);
                result = result.replace("\n", ".");
                result = result.replace("\r", ".");
            }
            return result ;
        }
        default:
            break;
        }
        break;
    case Qt::FontRole:
    //return element->styleElementTagFont(paintInfo);
    case Qt::ForegroundRole:
    //return element->styleElementTagColor(paintInfo);
    case Qt::DecorationRole:
    //return element->styleElementTagIcon();
    default:
        break;
    } // switch displayRole
    return QVariant();
}


Qt::ItemFlags BinaryViewerModel::flags(const QModelIndex &index) const
{
    if(!index.isValid()) {
        return QAbstractItemModel::flags(index);
    }
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled ;
}

QVariant BinaryViewerModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
    if(Qt::DisplayRole == role) {
        switch(section) {
        case OffsetColumn:
            return tr("Offset");
        case DataColumn:
            return tr("Binary values");
        case ValueColumn:
            return tr("Characters");
        default:
            break;
        }
    }
    return QVariant();
}

QModelIndex BinaryViewerModel::index(int row, int column, const QModelIndex & /*parent*/) const
{
    if((row < 0) || (row >= _rowsOfThisPage)) {
        return QModelIndex();
    }
    return createIndex(row, column, (void*)0);
}


QModelIndex BinaryViewerModel::parent(const QModelIndex & /*index*/) const
{
    return QModelIndex();
}

int BinaryViewerModel::rowCount(const QModelIndex & parent) const
{
    if(parent.isValid()) {
        return 0;
    }
    return _rowsOfThisPage ;
}

int BinaryViewerModel::columnCount(const QModelIndex & /*parent*/) const
{
    return NumColumns;
}

QByteArray BinaryViewerModel::getRowData(const int nRow)
{
    qint64 startOffset = _currentPage * SizeOfPage ;
    qint64 realBlockAddress = startOffset + (nRow / BinaryBlock::RowsPerBlock) * BinaryBlock::SizeOfBlock ;

    // look for an existing block
    BinaryBlock *block = NULL ;
    foreach(BinaryBlock * blk, _blocksList) {
        if(blk->baseAddress() == realBlockAddress) {
            block = blk ;
            break;
        }
    }
    if(NULL != block) {
        _blocksList.removeOne(block);
    } else {
        block = readBlock(realBlockAddress);
    }

    if(NULL == block) {
        Utils::error(tr("Invalid data read"));
        QByteArray defaultData(BinaryBlock::SizeOfRow, 0);
        return defaultData ;
    }

    if(_blocksList.size() > MaxBlocksInCache) {
        BinaryBlock*last =  _blocksList.last();
        _blocksList.removeLast();
        delete last;
    }
    _blocksList.insert(0, block);

    // get the row
    int rowOfBlock = nRow % BinaryBlock::RowsPerBlock;
    int rowStart = rowOfBlock * BinaryBlock::DataPerRow;
    int rowLen = BinaryBlock::SizeOfRow;
    if(_isLastPage && (nRow == _lastRowOfLastPage)) {
        rowLen = _lastRowSize ;
    }
    QByteArray rowData = block->rowAt(rowStart, rowLen);
    return rowData;
}


BinaryBlock *BinaryViewerModel::readBlock(const qint64 realBlockAddress)
{
    if(_io->seek(realBlockAddress)) {
        int blockSizeToRead = BinaryBlock::SizeOfBlock;
        qint64 thisBlockIndex = realBlockAddress / BinaryBlock::SizeOfBlock;
        if(thisBlockIndex == _lastBlock) {
            blockSizeToRead = _lastBlockSize ;
        }
        QByteArray dataRead;
        dataRead.resize(blockSizeToRead);
        int realDataRead = _io->read(dataRead.data(), blockSizeToRead);
        if(realDataRead != blockSizeToRead) {
            return NULL;
        }
        BinaryBlock *block = new BinaryBlock(realBlockAddress);
        block->setData(dataRead);
        return block;
    }
    return NULL ;
}

bool BinaryViewerModel::setCodecByName(const QString &codecName)
{
    _codec = QTextCodec::codecForName(codecName.toLatin1().data());
    if(NULL == _codec) {
        return false;
    }
    return true;
}


//------------------------------------------------------------------------

bool BinaryViewerModel::findOccurrences(BinaryViewerOperationResult &result, const QString &searchLemma, const int startPage, const int startLine, const bool searchForward)
{
    QByteArray reference = _codec->fromUnicode(searchLemma);
    return findOccurrencesBinary(result, reference, startPage, startLine, searchForward);
}


bool BinaryViewerModel::findOccurrencesBinary(BinaryViewerOperationResult &result, const QByteArray &reference, const int startPage, const int startLine, const bool searchForward)
{
    int bufferSize = SearchBufferSize + reference.length() - 1; // one is comprised in the original buffer
    qint64 bufferPosition = startPage * SizeOfPage + (startLine + 1) * BinaryBlock::DataPerRow;
    if(!searchForward)     {
        bufferPosition = startPage * SizeOfPage + startLine * BinaryBlock::DataPerRow - bufferSize;
    }
    bool exit = false;

    do {
        qint64 dataToRead  = bufferSize;
        if(bufferPosition >= _totalData) {
            // not found, out of bounds
            return false;
        } else if(bufferPosition <= -bufferSize) {
            // not found, out of bounds
            return false;
        } else if(bufferPosition < 0) {
            dataToRead  = bufferSize + bufferPosition ; // positive fraction
            bufferPosition = 0 ;
            exit = true ;
        }

        if(!_io->seek(bufferPosition)) {
            result.setError(BinaryViewerOperationResult::SEEK_FAILED_ERROR);
            return false;
        }
        QByteArray buffer;
        buffer.resize(dataToRead);
        qint64 dataRead = _io->read(buffer.data(), dataToRead);
        if(-1 == dataRead) {
            result.setError(BinaryViewerOperationResult::READ_ERROR);
            return false;
        }
        // resize to the real length
        if(dataRead < bufferSize) {
            buffer.resize(dataRead);
        }
        if(findOccurrenceInBuffer(result, reference, buffer, bufferPosition, searchForward)) {
            return true;
        }
        bufferPosition += (searchForward ? 1 : -1) * bufferSize;
    } while(!exit);
    return false;
}


bool BinaryViewerModel::findOccurrenceInBuffer(BinaryViewerOperationResult &result, const QByteArray &reference, const QByteArray &buffer, const qint64 absoluteOffsetOfBuffer, const bool searchForward)
{
    qint64 resultRow = 0;

    int newPos = 0 ;
    if(searchForward) {
        newPos = buffer.indexOf(reference);
    } else {
        newPos = buffer.lastIndexOf(reference);
    }
    if(newPos < 0) {
        return false;
    }
    // the result must be in the buffer range.
    qint64 absolutePositionOfMatch = absoluteOffsetOfBuffer + newPos;
    resultRow = absolutePositionOfMatch / BinaryBlock::SizeOfRow;
    result.page = absolutePositionOfMatch / SizeOfPage;
    result.row = resultRow % RowsPerPage ;
    return true;
}


bool BinaryViewerModel::findPageOfAddress(BinaryViewerOperationResult &result, const qint64 address)
{
    result.page = address / SizeOfPage ;
    result.row = (address / BinaryBlock::SizeOfRow) % RowsPerPage ;
    return true;
}

