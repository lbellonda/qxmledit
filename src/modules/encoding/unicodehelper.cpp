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


#include "unicodehelper.h"
#include <QFile>
#include <QDomDocument>

#define RESFILE ":/data/unicodeChars.xml"

UnicodeHelper::UnicodeHelper()
{
    _inited = false ;
}

UnicodeHelper::~UnicodeHelper()
{
}

void UnicodeHelper::init()
{
    _map.clear();
    QString resFile = RESFILE ;
    QFile file(resFile);
    if(file.open(QFile::ReadOnly)) {
        QDomDocument document;
        if(document.setContent(&file)) {
            QDomNodeList childNodes = document.childNodes();
            // root node
            int nodi = document.childNodes().count();
            for(int i = 0 ; i < nodi ; i ++) {
                QDomNode childNode = childNodes.item(i) ;
                if(childNode.isElement()) {
                    QDomElement rootElement = childNode.toElement();
                    if(rootElement.nodeName() == "root") {
                        QDomNodeList nameNodes = rootElement.childNodes();
                        int namesCount = rootElement.childNodes().count();
                        for(int name = 0 ; name < namesCount ; name ++) {
                            QDomNode nameNode = nameNodes.item(name) ;
                            if(nameNode.isElement()) {
                                QDomElement nameElement = nameNode.toElement();
                                if(nameElement.nodeName() == "char") {
                                    QString code = nameElement.attribute("code", "0");
                                    QString value = nameElement.attribute("value", "");
                                    bool isOk = false;
                                    int codeNum = code.toInt(&isOk, 16);
                                    _map.insert(codeNum, value);
                                }
                            }
                        }
                    }
                }
            }
        }
        file.close();
    }
}

QString UnicodeHelper::nameForChar(const QChar ch)
{
    if(!_inited) {
        _inited = true ;
        init();
    }
    int charValue = ch.unicode();
    if(_map.contains(charValue)) {
        return _map[charValue];
    }
    return "" ;
}
