/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
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

#include "xmlEdit.h"
#include <QDomDocument>
#include "validatormessagehandler.h"
#include "xmleditwidgetprivate.h"
#include "texteditutils.h"
#include "utils.h"


class FindNodeWithLocationInfo
{
public:
    QDomNode lastKnownNode;
    QDomNode matchedNode;
    FindNodeWithLocationInfo() {}
};

void XmlEditWidgetPrivate::showValidationResults(const QString xmlAsText, ValidatorMessageHandler &validator)
{
    Element *element = NULL ;
    QDomDocument document;
    if(document.setContent(xmlAsText)) {
        FindNodeWithLocationInfo info;
        findDomNodeScan(document, document, validator.line(), validator.column(), info);
        QList<int> errorPath;
        if(!info.matchedNode.isNull()) {
            errorPath = makeDomNodePath(info.matchedNode);
        } else if(!info.lastKnownNode.isNull()) {
            errorPath = makeDomNodePath(info.lastKnownNode);
        }
        if(!errorPath.isEmpty()) {
            Element *selectedElement = regola->findElementByArray(errorPath);
            p->setCurrentItem(selectedElement);
        }
    }
    QString htmlTextError = tr("Line:%1, Col:%2 :%3")
                            .arg(validator.line())
                            .arg(validator.column())
                            .arg(validator.descriptionInPlainText());

    emit p->schemaValidationError(htmlTextError, element);
}


QList<int> XmlEditWidgetPrivate::makeDomNodePath(QDomNode elementToExamine)
{
    QList<int> path;
    QDomNode node = elementToExamine ;
    do {
        QDomNode parent = node.parentNode();
        QDomNodeList childNodes = parent.childNodes();
        int nodes = childNodes.size();
        for(int i = 0 ; i < nodes ; i ++) {
            QDomNode nd = childNodes.at(i);
            if(nd == node) {
                path.insert(0, i);
                break;
            }
        }
        node = parent ;
    } while(!node.isNull());
    return path ;
}

/**
 * @brief XmlEditWidgetPrivate::findDomNodeScan find the nearest match to a position
 * @param node
 * @param nodeTarget
 * @param lineSearched
 * @param columnSearched
 * @param lastKnownNode: last known "good" position
 * @return
 */
bool XmlEditWidgetPrivate::findDomNodeScan(QDomNode node, QDomNode nodeTarget, const int lineSearched, const int columnSearched, FindNodeWithLocationInfo &info)
{
    int row = node.lineNumber();
    int col = node.columnNumber();
    if(!node.isDocument()) {
        if((lineSearched == row) && (columnSearched == col)) {
            info.matchedNode = nodeTarget ;
            return true ;
        }

        if((lineSearched == row) && (columnSearched == col)) {
            info.matchedNode = nodeTarget ;
            return true ;
        }
        if((lineSearched == row) && (columnSearched < col)) {
            info.matchedNode = nodeTarget ;
            return true ;
        }
        if((lineSearched < row)) {
            info.matchedNode = nodeTarget ;
            return true ;
        }
        if((lineSearched <= row)) {
            info.lastKnownNode = nodeTarget ;
        }

        if(node.nodeType() == QDomNode::ElementNode) {
            QDomElement element = node.toElement();
            QDomNamedNodeMap attributes = element.attributes();
            int numAttrs = attributes.length();
            for(int i = 0 ; i < numAttrs ; i++) {
                QDomNode node = attributes.item(i);
                QDomAttr attr = node.toAttr();
                if(findDomNodeScan(attr, nodeTarget, lineSearched, columnSearched, info)) {
                    return true;
                }
            } // for
        }
    }

    int nodes = node.childNodes().count();
    for(int i = 0 ; i < nodes ; i ++) {
        QDomNode childNode = node.childNodes().item(i) ;
        if(childNode.isText() || childNode.isCDATASection()) {
            if(findDomNodeScan(childNode, nodeTarget, lineSearched, columnSearched, info)) {
                return true;
            }
        } else {
            if(findDomNodeScan(childNode, childNode, lineSearched, columnSearched, info)) {
                return true ;
            }
        }
    }
    return false ;
}
