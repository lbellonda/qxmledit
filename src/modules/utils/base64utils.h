/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2017 by Luca Bellonda and individual contributors  *
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

    enum EBase64 {
        RFC4648Standard,
        RFC6920Url,
    };

    Base64Utils();
    virtual ~Base64Utils();

    QString doLimitColumns(const QString &text, const bool limitColumns, const int columns);
    QString loadFromBinaryFile(const EBase64 type, QWidget *window, const QString &filePath, bool &isError, bool &isAbort, const bool limitColumns = false, const int columns = 80);
    bool saveBase64ToBinaryFile(const EBase64 type, QWidget *window, const QString &text, const QString &filePath);

    bool saveToBinaryDevice(const EBase64 type, QIODevice *device, const QString &text);
    bool saveToBinaryFile(const EBase64 type, QWidget *window, const QString &filePath, const QString &text);

    QString toBase64(const EBase64 type, const QString &text, const bool limitColumns = false, const int columns = 80);
    QString toBase64(const EBase64 type, const QByteArray &input, const bool limitColumns = false, const int columns = 80);
    QByteArray fromBase64(const EBase64 type, const QString &text);
protected:
    static QString standardToSpecific(const EBase64 type, const QString &text);
    static QString specificToStandard(const EBase64 type, const QString &text);

};

#endif // BASE64UTILS_H
