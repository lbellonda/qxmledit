/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2019 by Luca Bellonda and individual contributors       *
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

#include "xmltest.h"
#include "utils.h"
#include "modules/xml/xmlerrormanagerdialog.h"

XMLTest::XMLTest(QObject *parent) : QObject(parent)
{
}

XMLTest::~XMLTest()
{
}

bool XMLTest::readTestXML(XMLLoadContext *context, QXmlStreamReader *xmlReader)
{
    while(!xmlReader->atEnd()) {
        xmlReader->readNext();
        if(xmlReader->hasError()) {
            return context->setErrorFromReader(xmlReader);
        }
        switch(xmlReader->tokenType()) {
        default:
            break;
        case QXmlStreamReader::Invalid:
            return context->setErrorFromReader(xmlReader);
        case QXmlStreamReader::EntityReference:
            return context->setError(tr("This XML contains an entity reference.\nEntity references are not supported at the moment."), xmlReader);
        }
    }//for
    return true;
}

bool XMLTest::loadTestFileInnerStream(XMLLoadContext *context, QIODevice *ioDevice)
{
    QXmlStreamReader xmlReader ;
    xmlReader.setNamespaceProcessing(false);
    xmlReader.setDevice(ioDevice);
    readTestXML(context, &xmlReader);
    return !context->isError();
}

bool XMLTest::testLoadFile(XMLLoadContext *context, const QString &filePath)
{
    if(!filePath.isEmpty()) {
        QFile file(filePath);
        if(file.open(QIODevice::ReadOnly)) {
            loadTestFileInnerStream(context, &file);
            file.close();
        } else {
            context->setErrorWithMessage(tr("Unable to read data.\nError code is '%1', %2.").arg(file.error()).arg(file.errorString()));
        }
    } else {
        context->setErrorWithMessage(tr("File name empty.\nUnable to load."));
    }
    return !context->isError();
}

bool XMLTest::testXMLFile(UIDelegate *uiDelegate, const QString &filePath)
{
    XMLLoadContext context;
    if(!testLoadFile(&context, filePath) || context.isError()) {
        uiDelegate->error(context.errorMessage());
        return false;
    } else {
        uiDelegate->message(tr("XML is valid."));
        return true ;
    }
}
