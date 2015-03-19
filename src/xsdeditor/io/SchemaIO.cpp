/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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

// IO for schema
#include "xsdeditor/xschema.h"
#include "xsdeditor/io/xschemaloader.h"
#include "modules/services/systemservices.h"
#include "utils.h"


bool XSDSchema::readFromString(XSDLoadContext *loadContext, const QString &inputText, const bool isRecursive, QNetworkAccessManager *newNetworkAccessManager, const QString &filePath)
{
    return readFromInputString(loadContext, inputText, isRecursive, newNetworkAccessManager, filePath);
}

bool XSDSchema::readFromClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    if(NULL == clipboard) {
        Utils::error(tr("No clipboard available."));
        return false;
    }
    QString inputText = clipboard->text();
    XSDLoadContext loadContext;
    return readFromInputString(&loadContext, inputText, false, NULL, NULL);
}

// Kludge to get namespaces
bool XSDSchema::scanForNS(QDomDocument &documentNS)
{
    bool isOk = false;
    int nodes = documentNS.childNodes().count();
    for(int i = 0 ; i < nodes ; i ++) {
        isOk = true ;
        QDomNode childNode = documentNS.childNodes().item(i) ;
        if(childNode.isElement()) {
            if(!scanSchemaNS(childNode.toElement())) {
                isOk = false;
            }
        }
    }
    return isOk ;
}

bool XSDSchema::applyScan(XSDLoadContext *loadContext, QDomDocument &document)
{
    bool isOk = false;

    int nodes = document.childNodes().count();
    for(int i = 0 ; i < nodes ; i ++) {
        isOk = true ;
        QDomNode childNode = document.childNodes().item(i) ;
        if(childNode.isElement()) {
            if(!isValidSchema(childNode.toElement())) {
                Utils::error(tr("The document does not appear to be a XML Schema document."));
                return false;
            }
            if(scanSchema(loadContext, childNode.toElement())) {
                isOk = true;
            } else {
                isOk = false;
            }
        }
    }
    return isOk;
}

bool XSDSchema::readFromInputString(XSDLoadContext *loadContext, const QString &inputText, const bool isRecursive, QNetworkAccessManager *networkAccessManager, const QString &filePath)
{
    bool isOk = false;
    reset(); // start from a known base
    {
        QDomDocument documentNS;
        // Kludge to get namespaces
        if(documentNS.setContent(inputText, false)) {
            if(scanForNS(documentNS)) {
                isOk = true;
            }
        }
    }
    if(isOk) {
        isOk = false;
        QDomDocument document;
        if(document.setContent(inputText, true)) {
            if(applyScan(loadContext, document)) {
                isOk = true;

                if(isRecursive) {
                    XSchemaLoader loader;
                    XSDLoadContext *theLoadContext;
                    XSDLoadContext localLoadContext;
                    if(NULL == loadContext) {
                        theLoadContext = &localLoadContext;
                    } else {
                        theLoadContext = loadContext ;
                    }
                    XSchemaLoader::State state = loader.loadDependent(theLoadContext, this, filePath, networkAccessManager);
                    if(XSchemaLoader::STATE_READY != state) {
                        Utils::error(tr("Error loading schema depenendencies."));
                    }
                }

            } else {
                isOk = false;
            }
        }
        if(!isOk) {
            Utils::error(tr("Unable to parse XML"));
        }
    } else {
        Utils::error(tr("Unable to load schema."));
    }
    return isOk ;
}


bool XSDSchema::readFromIoDevice(XSDLoadContext *loadContext, QIODevice *ioDevice)
{
    bool isOk = false;
    reset(); // start from a known base
    // copy data for random access
    qint64 dataSize = ioDevice->bytesAvailable();
    QBuffer buffer;
    QByteArray data ;
    data.resize((int)dataSize);
    if(!ioDevice->isOpen()) {
        if(!ioDevice->open(QIODevice::ReadOnly)) {
            Utils::error(tr("Unable to open for read schema data."));
            return false;
        }
    }
    qint64 dataRead = ioDevice->read(data.data(), dataSize);
    if(dataRead != dataSize) {
        Utils::error(tr("Unable to read schema data."));
        return false;
    }

    buffer.setData(data);
    {
        QDomDocument documentNS;
        // Kludge to get namespaces
        if(documentNS.setContent(&buffer, false)) {
            if(scanForNS(documentNS)) {
                isOk = true;
            }
        }
    }
    if(isOk) {
        isOk = false;
        QDomDocument document;
        buffer.reset();
        if(document.setContent(&buffer, true)) {
            if(applyScan(loadContext, document)) {
                isOk = true;
            } else {
                isOk = false;
            }
        }
        if(!isOk) {
            Utils::error(tr("Unable to parse XML"));
        }
    } else {
        Utils::error(tr("Unable to load schema."));
    }
    return isOk ;
}

bool XSDSchema::read(XSDLoadContext *loadContext, const QString &filePath)
{
    bool isOk = false;
    reset(); // start from a known base

    QFile file(filePath);
    if(file.open(QIODevice::ReadOnly)) {
        isOk = readFromIoDevice(loadContext, &file);
        file.close();

    } else {
        Utils::error(QString(tr("Unable to load schema file.\n Error code is '%1'")).arg(file.error()));
    }
    return isOk ;
}
