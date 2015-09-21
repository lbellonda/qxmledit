/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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


#ifndef BASE64UTILS_H
#define BASE64UTILS_H

#include "xmlEdit.h"
#include "libQXmlEdit_global.h"

class LIBQXMLEDITSHARED_EXPORT Base64Utils
{
    static const int InputSizeLimit = 1024 * 1024 ;

public:
    Base64Utils();
    virtual ~Base64Utils();

    QString loadFromBinaryFile(QWidget *window, const QString &filePath, bool &isError, bool isAbort);
    bool saveBase64ToBinaryFile(QWidget *window, const QString &text, const QString &filePath);

    bool saveToBinaryDevice(QIODevice *device, const QString &text);
    bool saveToBinaryFile(QWidget *window, const QString &filePath, const QString &text);

};

#endif // BASE64UTILS_H
