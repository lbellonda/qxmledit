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


#ifndef BINARYBLOCK_H
#define BINARYBLOCK_H

#include <QByteArray>

class BinaryBlock
{

    qint64 _baseAddress;
    QByteArray _data;

public:
    static const int DataPerRow = 16 ;
    static const int SizeOfRow = DataPerRow ;
    static const int RowsPerBlock = 1024;
    static const int SizeOfBlock = RowsPerBlock * DataPerRow;

public:
    BinaryBlock(const qint64 argBaseAddress);
    ~BinaryBlock();

    void setData(const QByteArray &newData);
    QByteArray rowAt(const int rowStart, const int len);
    qint64 baseAddress();
};

#endif // BINARYBLOCK_H
