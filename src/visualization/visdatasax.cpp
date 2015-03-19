/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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


#include "visdatasax.h"
#include "utils.h"

VisDataSax::VisDataSax(QSet<QString> *newNames, QHash<QString, TagNode*> *newTagNodes)
{
    root = NULL ;
    currentElement = NULL ;
    names = newNames;
    tagNodes = newTagNodes;
}

VisDataSax::~VisDataSax()
{
}

void VisDataSax::addTagNode(const QString &name)
{
    TagNode *node = tagNodes->value(name);
    if(NULL == node) {
        node = new TagNode(name, tagNodes->count() + 1);
        tagNodes->insert(name, node) ;
    }
    node->count++;
    // add the relationship
    if(NULL != currentElement) {
        TagNode *parentNode = tagNodes->value(currentElement->name);
        node->linksIn++;
        parentNode->linksOut++;
        // must be not null
        TagNodeTarget *tnt = parentNode->targets[name];
        if(NULL == tnt) {
            tnt = new TagNodeTarget(name);
            parentNode->targets[name] = tnt ;
        }
        tnt->count ++ ;
    }
}

bool VisDataSax::startElement(const QString &/*namespaceURI*/, const QString & /*localName*/,
                              const QString &qName, const QXmlAttributes &attributes)
{
    QSet<QString>::const_iterator iter = names->insert(qName);
    QString name = *iter;
    if(NULL != tagNodes) {
        addTagNode(name);
    }
    ElementBase *elem = new ElementBase(currentElement, name) ;
    if(NULL == currentElement) {
        root = elem;
    }
    int attrCount = attributes.count();
    for(int index = 0 ; index < attrCount ; index ++) {
        elem->size += attributes.localName(index).length();
        elem->size += attributes.value(index).length();
    }
    elem->attributesCount = attrCount;
    currentElement = elem ;
    return true ;
}

bool VisDataSax::endElement(const QString &/*namespaceURI*/, const QString &/*localName*/,
                            const QString &/*qName*/)
{
    if(NULL != currentElement) {
        currentElement = currentElement->parent;
    }
    return true;
}

bool VisDataSax::characters(const QString &str)
{
    if(NULL != currentElement) {
        int len = str.length();
        currentElement->size += len;
        QString s2 = str.trimmed();
        currentElement->payload += s2.length();
    }
    return true ;
}

bool VisDataSax::fatalError(const QXmlParseException &exception)
{
    Utils::error(QObject::tr("Parse error (2) at line %1, column %2:\n%3")
                 .arg(exception.lineNumber())
                 .arg(exception.columnNumber())
                 .arg(exception.message()));
    return false;
}

bool VisDataSax::error(const QXmlParseException &exception)
{
    Utils::error(QObject::tr("Parse error (1) at line %1, column %2:\n%3")
                 .arg(exception.lineNumber())
                 .arg(exception.columnNumber())
                 .arg(exception.message()));
    return false;
}

QString VisDataSax::errorString() const
{
    return QObject::tr("Generic error.");
}
