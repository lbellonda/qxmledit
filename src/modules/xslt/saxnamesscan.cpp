/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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


#include "saxnamesscan.h"
#include "utils.h"


SaxNamesScan::SaxNamesScan(XsltHelper *helper)
{
    _helper = helper;
}

SaxNamesScan::~SaxNamesScan()
{
}


bool SaxNamesScan::startElement(const QString &/*namespaceURI*/, const QString &localName,
                                const QString &qName, const QXmlAttributes &attributes)
{
    // names with and without ns
    if(!qName.isEmpty()) {
        _helper->addNameForAutocompletion(qName);
    }
    if(!localName.isEmpty()) {
        _helper->addNameForAutocompletion(localName);
    }
    int count = attributes.count();
    for(int i = 0 ; i < count ; i ++) {
        QString name ;
        QString aLocalName = attributes.localName(i);
        if(!localName.isEmpty()) {
            name = QString("@%1").arg(aLocalName);
            _helper->addNameForAutocompletion(name);
        }
        QString aqName = attributes.qName(i);
        if(!aqName.isEmpty()) {
            name = QString("@%1").arg(aqName);
            _helper->addNameForAutocompletion(name);
        }
    }
    return true ;
}


bool SaxNamesScan::fatalError(const QXmlParseException &exception)
{
    Utils::error(QObject::tr("Parse error (2) at line %1, column %2:\n%3")
                 .arg(exception.lineNumber())
                 .arg(exception.columnNumber())
                 .arg(exception.message()));
    return false;
}

bool SaxNamesScan::error(const QXmlParseException &exception)
{
    Utils::error(QObject::tr("Parse error (1) at line %1, column %2:\n%3")
                 .arg(exception.lineNumber())
                 .arg(exception.columnNumber())
                 .arg(exception.message()));
    return false;
}
QString SaxNamesScan::errorString() const
{
    return QObject::tr("Generic error.");
}
