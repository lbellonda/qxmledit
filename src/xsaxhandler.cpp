/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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

#include "xsaxhandler.h"
#include "utils.h"

XSaxHandler::XSaxHandler(Regola *regola)
{
    xmlTree = regola ;
    currentElement = NULL ;
}

XSaxHandler::~XSaxHandler()
{
}

bool XSaxHandler::startElement(const QString &/*namespaceURI*/, const QString &/*localName*/,
                               const QString &qName, const QXmlAttributes &/*attributes*/)
{
    //TODO Element *elem = new Element( xmlTree->getTag(qName), QString(""), xmlTree, currentElement ) ;
    Element *elem = new Element(xmlTree->addNameToPool(qName), QString(""), xmlTree, currentElement) ;
    if(NULL == currentElement) {
        xmlTree->setRootElement(elem);
    } else {
        currentElement->addChild(elem) ;
    }
    currentElement = elem ;
    return true ;
}

bool XSaxHandler::endElement(const QString &/*namespaceURI*/, const QString &/*localName*/,
                             const QString &/*qName*/)
{
    if(NULL != currentElement) {
        currentElement = currentElement->parent();
    }
    return true;
}

bool XSaxHandler::characters(const QString &str)
{
    if(NULL != currentElement) {
        currentElement->incrementSizeInfo(str.length());
    }
    return true ;
}

bool XSaxHandler::fatalError(const QXmlParseException &exception)
{
    Utils::error(QObject::tr("Parse error (2) at line %1, column %2:\n%3")
                 .arg(exception.lineNumber())
                 .arg(exception.columnNumber())
                 .arg(exception.message()));
    return false;
}

bool XSaxHandler::error(const QXmlParseException &exception)
{
    Utils::error(QObject::tr("Parse error (1) at line %1, column %2:\n%3")
                 .arg(exception.lineNumber())
                 .arg(exception.columnNumber())
                 .arg(exception.message()));
    return false;
}
QString XSaxHandler::errorString() const
{
    return QObject::tr("Generic error.");
}
