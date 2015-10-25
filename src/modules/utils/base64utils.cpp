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

#include "base64utils.h"
#include "qxmleditdata.h"
#include "utils.h"

Base64Utils::Base64Utils()
{
}

Base64Utils::~Base64Utils()
{
}


QString Base64Utils::standardToSpecific(const EBase64 type, const QString &text)
{
    if(RFC6920Url == type) {
        QString res(text);
        res = res.replace("+", "-");
        res = res.replace("/", "_");
        return res;
    }
    return text ;
}


QString Base64Utils::specificToStandard(const EBase64 type, const QString &text)
{
    if(RFC6920Url == type) {
        QString res(text);
        res = res.replace("-", "+");
        res = res.replace("_", "/");
        return res;
    }
    return text ;
}

QString Base64Utils::toBase64(const EBase64 type, const QString &text)
{
    QString result = Utils::toBase64(text);
    result = standardToSpecific(type, result);
    return result ;
}

QString Base64Utils::toBase64(const EBase64 type, const QByteArray &input)
{
    QByteArray resultBytes = input.toBase64();
    QString result(resultBytes);
    result = standardToSpecific(type, result);
    return result ;
}

QByteArray Base64Utils::fromBase64(const EBase64 type, const QString &text)
{
    QString toConvert = specificToStandard(type, text);
    QByteArray array(toConvert.toLatin1());
    QByteArray array2 = QByteArray::fromBase64(array);
    return array2;
}


QString Base64Utils::loadFromBinaryFile(const EBase64 type, QWidget *window, const QString &filePath, bool &isError, bool isAbort)
{
    QString result;
    isError = true ;
    isAbort = false ;
    QFile file(filePath);
    QByteArray data ;
    if(file.open(QIODevice::ReadOnly)) {
        qint64 fileSize = file.size();
        if(fileSize > InputSizeLimit) {
            if(!Utils::askYN(window, QObject::tr("Warning: the size of the file to import is %1. Do you want to continue?").arg(Utils::getSizeForPresentation(fileSize)))) {
                isAbort = true ;
                isError = false ;
            }
        }
        if(!isAbort) {
            data = file.readAll();
            if(file.error() == QFile::NoError) {
                isError = false ;
            }
        }
        file.close();
        if(isError) {
            Utils::error(window, QObject::tr("Error reading file."));
        } else {
            QByteArray converted = data.toBase64();
            // this is ASCII, always
            result = converted.data();
            result = standardToSpecific(type, result);
        }
    } else {
        Utils::error(window, QString(QObject::tr("Unable to load file.\nError code is '%1'")).arg(file.error()));
    }
    return result ;
}


/////------------

bool Base64Utils::saveBase64ToBinaryFile(const EBase64 type, QWidget *window, const QString &text, const QString &fileStartPath)
{
    QString filePath = QFileDialog::getSaveFileName(window, QObject::tr("Save Base 64 Coded Data to File"),
                       QXmlEditData::sysFilePathForOperation(fileStartPath),
                       QObject::tr("XML files (*.xml);;XML Schema files (*.xsd);;All files (*);;"));
    if(!filePath.isEmpty()) {
        return saveToBinaryFile(type, window, filePath, text);
    }
    return false;
}

bool Base64Utils::saveToBinaryFile(const EBase64 type, QWidget *window, const QString &filePath, const QString &text)
{
    bool isError = true ;
    QFile file(filePath);
    if(file.open(QIODevice::WriteOnly)) {
        if(saveToBinaryDevice(type, &file, text)) {
            isError = false ;
        }
        file.close();
        if(file.error() != QFile::NoError) {
            isError = true ;
        }
        if(isError) {
            Utils::error(window, QObject::tr("Error writing to file."));
        }
    } else {
        Utils::error(window, QString(QObject::tr("Unable to open file.\n Error code is '%1'")).arg(file.error()));
    }
    return !isError ;
}

bool Base64Utils::saveToBinaryDevice(const EBase64 type, QIODevice *device, const QString &text)
{
    bool isOk = true ;
    QString base64 = specificToStandard(type, text);
    QByteArray array(base64.toLatin1());
    QByteArray binaryArray = QByteArray::fromBase64(array);
    if(-1 == device->write(binaryArray)) {
        isOk = false ;
    }
    return isOk;
}
