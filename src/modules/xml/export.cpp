/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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

#include "regoladefinitions.h"

bool Regola::writeAsJavaString(QIODevice * device)
{
    QDomDocument    document = createNewDocument();
    QVectorIterator<Element*> it(childItems);
    while(it.hasNext()) {
        if(!it.next()->generateDom(document, document)) {
            return false;
        }
    }
    if(!device->open(QIODevice::WriteOnly | QIODevice::Text)) {
        Utils::error(tr("Error writing data: %1").arg(device->errorString()));
        return false;
    }

    QString result = document.toString(4);
    QStringList resList = result.split(QRegExp("\\r?\\n"));
    if(resList.size() > 0) {
        if(resList.last() == "") {
            resList.removeLast();
        }
    }

    QTextStream streamOut(device);
    streamOut.setCodec("UTF-8");
    streamOut << " a = \"\"" ;
    int index = -1 ;
    const int lastIndex = resList.size() - 1;
    foreach(QString str, resList) {
        index++;
        streamOut << "\n + \"";
        str = str.replace("\\", "\\\\");
        str = str.replace("\"", "\\\"");
        int len = str.length();
        for(int i = 0 ; i < len ; i++) {
            QChar ch = str.at(i);
            ushort unicodeCode = ch.unicode();
            if(ch > 127) {
                streamOut << "&#x";
                QString value ;
                value.setNum(unicodeCode, 16);
                streamOut << value;
                streamOut << ";";
            } else {
                streamOut << ch ;
            }
        }
        if(lastIndex == index) {
            streamOut << "\"";
            //streamOut << "******last";
        } else {
            streamOut << "\\n\"";
        }
        //streamOut << QString("***%1:%2****").arg(str.length()).arg(str);
    }
    streamOut << ";\n";
    streamOut.flush();
    device->close();

    return true;
}

bool Regola::writeAsJavaStringInFile(const QString &filePath)
{
    QFile   file(filePath);
    QIODevice *outDevice = NULL ;
    if(NULL != _deviceProvider) {
        outDevice = _deviceProvider->newDeviceForWrite(filePath);
    } else {
        outDevice = &file ;
    }
    return writeAsJavaString(outDevice);
}

//------
bool Regola::writeAsCString(QIODevice * device)
{
    QDomDocument    document = createNewDocument();
    QVectorIterator<Element*> it(childItems);
    while(it.hasNext()) {
        if(!it.next()->generateDom(document, document)) {
            return false;
        }
    }
    if(!device->open(QIODevice::WriteOnly | QIODevice::Text)) {
        Utils::error(tr("Error writing data: %1").arg(device->errorString()));
        return false;
    }

    QString result = document.toString(4);
    QStringList resList = result.split(QRegExp("\\r?\\n"));
    if(resList.size() > 0) {
        if(resList.last() == "") {
            resList.removeLast();
        }
    }

    QTextStream streamOut(device);
    streamOut.setCodec("UTF-8");
    streamOut << "#define XML_SOURCE \"\\" ;
    int index = -1 ;
    const int lastIndex = resList.size() - 1;
    foreach(QString str, resList) {
        index++;
        streamOut << "\n";
        str = str.replace("\\", "\\\\");
        str = str.replace("\"", "\\\"");
        int len = str.length();
        for(int i = 0 ; i < len ; i++) {
            QChar ch = str.at(i);
            ushort unicodeCode = ch.unicode();
            if(ch > 127) {
                streamOut << "&#x";
                QString value ;
                value.setNum(unicodeCode, 16);
                streamOut << value;
                streamOut << ";";
            } else {
                streamOut << ch ;
            }
        }
        if(lastIndex == index) {
            streamOut << "\"";
            //streamOut << "******last";
        } else {
            streamOut << "\\n \\";
        }
        //streamOut << QString("***%1:%2****").arg(str.length()).arg(str);
    }
    streamOut << "\n";
    streamOut.flush();
    device->close();

    return true;
}

bool Regola::writeAsCStringInFile(const QString &filePath)
{
    QFile   file(filePath);
    QIODevice *outDevice = NULL ;
    if(NULL != _deviceProvider) {
        outDevice = _deviceProvider->newDeviceForWrite(filePath);
    } else {
        outDevice = &file ;
    }
    return writeAsCString(outDevice);
}





