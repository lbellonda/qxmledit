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
#include "scansax.h"
#include "utils.h"

XmlScanInfo::XmlScanInfo()
{
    occurrences = NULL ;
    currentCount = NULL ;
    groups = NULL ;
    inAxisArray = NULL ;
    size = 0 ;
    isError = false ;
    isAbort = false ;
    isGroup = false ;
}

XmlScanInfo::~XmlScanInfo()
{
    reset();
}

void XmlScanInfo::reset()
{
    if(occurrences) {
        delete [] occurrences ;
        occurrences = NULL ;
    }
    if(currentCount) {
        delete [] currentCount ;
        currentCount = NULL ;
    }
    if(inAxisArray) {
        delete [] inAxisArray ;
        inAxisArray = NULL ;
    }
    if(groups) {
        delete [] groups ;
        groups = NULL ;
    }
}

bool XmlScanInfo::check()
{
    QStringList res ;
    for(int i = 0 ; i < size ; i++) {
        QString str = tokens.at(i);
        QString s1 = str.trimmed();
        if(s1.isEmpty()) {
            return false;
        }
        res.append(s1);
    }
    tokens = res ;
    return true ;
}


bool XmlScanInfo::init(const QString &value, const bool newIsGroup)
{
    reset();
    isGroup = newIsGroup ;
    token = value ;
    tokens = token.split("/");
    size = tokens.length() ;
    if(size <= 0) {
        return false;
    }
    if(tokens.at(0) == "") {
        tokens.removeAt(0);
    }
    size = tokens.length() ;
    if(size <= 0) {
        return false;
    }
    occurrences = new int [size];
    currentCount = new int [size];
    inAxisArray = new bool [size];
    if(isGroup) {
        groups = new QMap<int, int> [size];
    }
    if((NULL == occurrences)
            || (NULL == currentCount)
            || (NULL == inAxisArray)
            || (isGroup && (NULL == groups))) {
        reset();
        return false;
    }

    for(int i = 0 ; i < size ; i++) {
        occurrences[i] = 0 ;
        currentCount[i] = 0;
        inAxisArray[i] = false ;
    }
    isError = false;
    isAbort = false ;
    errorMessage = "" ;
    return true ;
}


ScanSax::ScanSax(XmlScanInfo &newValue) : info(newValue)
{
    deep = -1 ;
}

ScanSax::~ScanSax()
{
}

//TODO: check if the axis is valid or we are seeking into a brother not to be searched
/**
  Note: to check that the scan is proceding on the selected axis, we take advantage from the
  fact that nodes are visited in hierarchical order, that is the constraint is: to access a node, first
  the father has to be visited
*/
bool ScanSax::startElement(const QString & /*namespaceURI*/, const QString &/*localName*/,
                           const QString &qName, const QXmlAttributes &/*attributes*/)
{
    if(info.isAbort) {
        return false;
    }
    deep++;
    if(deep < info.size) {
        // if is not into the search axis, don't count it.
        if(deep > 0) {
            if(!info.inAxisArray[deep - 1]) {
                return true ;
            }
        }
        if(info.tokens.at(deep) == qName) {
            info.inAxisArray[deep] = true ;
            // deep < info.size is tested at the start of the funciton
            if(deep >= 0) {
                info.occurrences[deep]++;
            }
            if(info.isGroup) {
                if((deep < (info.size - 1))) {
                    info.currentCount[deep + 1] = 0 ; // reset child counter on start
                }
                info.currentCount[deep]++; // one more element seen
            }
        } else {
            info.inAxisArray[deep] = false ;
        }
    }
    return true ;
}

bool ScanSax::endElement(const QString & /*namespaceURI*/, const QString & /*localName*/,
                         const QString & /*qName*/)
{
    if(info.isGroup) {
        if(info.inAxisArray[deep]) {
            if(deep < (info.size - 1)) {
                // End element: close n-1 level
                setFinalCountForItem(deep + 1);
            }
            // When deep is 0, the whole file scan is done, so we must take care of the first element (the root).
            // If it was post scan function, this will be the right place for that.
            if(0 == deep) {
                // the only and one
                setFinalCountForItem(deep);
            }
        }
    }
    deep -- ;
    return true;
}

void ScanSax::setFinalCountForItem(const int level)
{
    int tot = (info.groups[level])[info.currentCount[level]] ;
    tot ++;
    (info.groups[level])[info.currentCount[level]] = tot ;
    // reset at the end of an elment
    info.currentCount[level] = 0 ;
}


bool ScanSax::fatalError(const QXmlParseException &exception)
{
    if(info.isAbort) {
        return false;
    }

    Utils::error(QObject::tr("Parse error (2) at line %1, column %2:\n%3")
                 .arg(exception.lineNumber())
                 .arg(exception.columnNumber())
                 .arg(exception.message()));
    return false;
}

bool ScanSax::error(const QXmlParseException &exception)
{
    if(info.isAbort) {
        return false;
    }

    Utils::error(QObject::tr("Parse error (1) at line %1, column %2:\n%3")
                 .arg(exception.lineNumber())
                 .arg(exception.columnNumber())
                 .arg(exception.message()));
    return false;
}

QString ScanSax::errorString() const
{
    return QObject::tr("Generic error.");
}

