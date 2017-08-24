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


#include "nodessax.h"
#include "visualization/attributessummarydata.h"
#include "utils.h"

NodesSax::NodesSax(QHash<QString, TagNode*> *newTagNodes, AttributesSummaryData *newAttributesSummaryData)
{
    tagNodes = newTagNodes;
    attributesSummaryData = newAttributesSummaryData;
}

NodesSax::~NodesSax()
{
}

void NodesSax::addTagNode(const QString &name)
{
    TagNode *node = tagNodes->value(name);
    if(NULL == node) {
        node = new TagNode(name, tagNodes->count() + 1);
        tagNodes->insert(name, node) ;
    }
    node->count++;
    // add the relationship
    if(elements.size() > 0) {
        TagNode *parentNode = tagNodes->value(elements.top());
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

void NodesSax::handleAttributes(const QXmlAttributes & attributes)
{
    if(NULL != attributesSummaryData) {
        const int attrCount = attributes.count();
        for(int index = 0 ; index < attrCount ; index ++) {
            const QString &attributeLocalName = attributes.qName(index);
            const QString &attrValue = attributes.value(index);
            const int thisAttrSize = attrValue.length();
            QString attributePath = _currentElementPath + "/@" + attributeLocalName;
            AttributeSummaryData * attributeSummaryData = attributesSummaryData->attributeSummaryData(attributePath, attributeLocalName);
            attributeSummaryData->count ++ ;
            attributeSummaryData->dataSize += thisAttrSize ;
            if(0 == thisAttrSize) {
                attributeSummaryData->countEmpty ++ ;
            }
        }
    }
}

bool NodesSax::startElement(const QString &/*namespaceURI*/, const QString & /*localName*/,
                            const QString &qName, const QXmlAttributes & attributes)
{
    addTagNode(qName);
    elements.push(qName);
    _currentElementPath = Utils::pushCurrentElementPath(_currentElementPath, qName);
    handleAttributes(attributes);
    return true ;
}


bool NodesSax::endElement(const QString &/*namespaceURI*/, const QString &/*localName*/,
                          const QString &/*qName*/)
{
    elements.pop();
    _currentElementPath = Utils::popCurrentElementPath(_currentElementPath);
    return true;
}


bool NodesSax::fatalError(const QXmlParseException &exception)
{
    Utils::error(QObject::tr("Parse error (2) at line %1, column %2:\n%3")
                 .arg(exception.lineNumber())
                 .arg(exception.columnNumber())
                 .arg(exception.message()));
    return false;
}

bool NodesSax::error(const QXmlParseException &exception)
{
    Utils::error(QObject::tr("Parse error (1) at line %1, column %2:\n%3")
                 .arg(exception.lineNumber())
                 .arg(exception.columnNumber())
                 .arg(exception.message()));
    return false;
}

QString NodesSax::errorString() const
{
    return QObject::tr("Generic error.");
}

bool NodesSax::startDocument()
{
    _currentElementPath = "";
    return true ;
}
