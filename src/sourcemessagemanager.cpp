/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016-2018 by Luca Bellonda and individual contributors  *
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

#include "sourcemessagemanager.h"
#include "regola.h"
#include "utils.h"

SourceMessageManager::SourceMessageManager()
{
}

SourceMessageManager::~SourceMessageManager()
{
}

bool SourceMessageManager::bindToSource(QIODevice *ioDevice, QList<SourceMessage*> &errors, Regola *regola)
{
    QXmlStreamReader reader(ioDevice);
    return bindToSource(&reader, errors, regola);
}

void SourceMessageManager::setSource(QXmlStreamReader *reader, SourceMessage *error, const bool isFound)
{
    error->setFoundLine(reader->lineNumber());
    error->setFoundColumn(reader->columnNumber());
    error->setFound(isFound);
}

void SourceMessageManager::setPath(SourceMessage *error, Regola *regola, SimpleTreeItem *thisElement)
{
    if(NULL != thisElement) {
        error->setPath(findPath(thisElement, regola));
    }
}

bool SourceMessageManager::bindToSource(QXmlStreamReader *xmlReader, QList<SourceMessage*> &errors, Regola *regola)
{
    xmlReader->setNamespaceProcessing(false);
    SimpleTreeItem all;
    SimpleTreeItem *current = NULL ;
    SimpleTreeItem *previous = NULL ;
    while(!xmlReader->atEnd()) {
        xmlReader->readNext();
        if(xmlReader->hasError()) {
            return false;
        }
        switch(xmlReader->tokenType()) {
        default:
            break;
        case QXmlStreamReader::Invalid:
            return false;
        case QXmlStreamReader::EndElement: {
            current = current->parent();
        }
        break;
        case QXmlStreamReader::StartElement: {
            SimpleTreeItem *thisElement = new SimpleTreeItem();
            previous = current ;
            if(NULL == current) {
                all.append(thisElement);
            } else {
                current->append(thisElement);
            }
            thisElement->setParent(current);
            current = thisElement;

            foreach(SourceMessage *error, errors) {
                if(!error->isFound()) {
                    if((error->line() == xmlReader->lineNumber()) && (error->column() == xmlReader->columnNumber())) {
                        setSource(xmlReader, error, true);
                        setPath(error, regola, thisElement);
                    } else if(xmlReader->lineNumber() > error->line()) {
                        // take last pos
                        error->setFound(true);
                        setPath(error, regola, previous);
                    } else if(xmlReader->lineNumber() == error->line()) {
                        if(xmlReader->columnNumber() >= error->column()) {
                            setSource(xmlReader, error, true);
                            setPath(error, regola, thisElement);
                        }
                    }
                    // last known position if still open
                    if(!error->isFound()) {
                        setSource(xmlReader, error, false);
                    }
                }
            }
        }
        break;
        } //if
    }//for
    // never reached
    return true;
}//setChildrenTreeFromStream()

QList<int> SourceMessageManager::findPath(SimpleTreeItem *current, Regola* regola)
{
    QList<int> path;
    SimpleTreeItem *elm = current ;
    SimpleTreeItem *parent = elm->parent();
    while(NULL != parent) {
        const int pos = parent->children().indexOf(elm);
        if(pos < 0) {
            return QList<int>();
        }
        path.prepend(pos);
        elm = parent ;
        parent = elm->parent();
    }
    path.prepend(0);
    return regola->elementPathForPath(path);
}
