/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013-2018 by Luca Bellonda and individual contributors  *
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

#include "xsltmanager.h"
#include "utils.h"


#define FILE_TOKENS ":/xslt/xsltTokens"
#define ROOT_ELEMENT    "root"
#define TOKEN_TAGNAME   "token"
#define TAG_NAME "name"
#define TAG_CLASS "class"
#define TAG_CARDINALITY "cardinality"
#define TAG_CHILDCLASSES "children-class"
#define TAG_POSITION "position"
#define TAG_USE "show"
#define NAME_ATTRIBUTE_NAME "name-attrib"
#define SELECT_ATTRIBUTE_NAME   "select-attrib"
#define TAG_COMPLETION  "compl"
#define TAG_CLEARATTRS  "clra"

XsltManager::XsltManager()
{
    _started = false;
}

XsltManager::~XsltManager()
{
    reset();
}

void XsltManager::reset()
{
    foreach(XsltElement * el, _elementsByTag.values()) {
        delete el;
    }
    _elementsByTag.clear();
}


bool XsltManager::init()
{
    if(_started) {
        return true ;
    }
    _started = true;
    return readTokensFile(FILE_TOKENS);
}


QMap<QString, XsltElement*> &XsltManager::elementsByTag()
{
    if(!init()) {
        Utils::error(tr("Error loading XSLT internal data."));
    }
    return _elementsByTag;
}


bool XsltManager::readTokensFile(const QString & filePath)
{
    bool isOk = false;
    QFile file(filePath);
    if(file.open(QIODevice::ReadOnly)) {
        QDomDocument document;
        if(document.setContent(&file)) {
            isOk = scanDataFile(document);
        } else {
            Utils::error(tr("Unable to parse XML"));
        }
        file.close();
    } else {
        Utils::error(QString(tr("Unable to load file.\n Error code is '%1'")).arg(file.error()));
    }
    return isOk ;
}

bool XsltManager::scanDataFile(QDomNode &rootNode)
{
    bool isOk = true ;
    int nodi = rootNode.childNodes().count();
    //D(printf("sono in assegna con %d nodi\n", nodi));
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = rootNode.childNodes().item(i) ;

        if(childNode.isElement()) {
            QDomElement element = childNode.toElement();
            _defaultAttributeName = element.attribute("default-name-attr", "name");
            _defaultAttributeSelectName = element.attribute("default-select-attrib", "select");
            if(ROOT_ELEMENT == element.tagName()) {
                if(!scanTokens(element)) {
                    isOk = false;
                }
            }
        }
    }//for
    return isOk ;
}//scanDataFile()


bool XsltManager::scanTokens(QDomElement &rootNode)
{
    bool isOk = true ;
    int nodi = rootNode.childNodes().count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = rootNode.childNodes().item(i) ;
        if(childNode.isElement()) {
            QDomElement element = childNode.toElement();
            if(TOKEN_TAGNAME == element.tagName()) {
                if(!buildToken(&element)) {
                    isOk = false;
                }
            } // if tag
        }//if element
    }//for
    return isOk ;
}//scanTokens()


bool XsltManager::buildToken(QDomElement *element)
{
    QString use = element->attribute(TAG_USE, "");
    QString id = element->attribute(TAG_NAME, "");
    QString theClass = element->attribute(TAG_CLASS, "");
    QString cardinality = element->attribute(TAG_CARDINALITY, "");
    QString childClasses = element->attribute(TAG_CHILDCLASSES, "");
    QString position = element->attribute(TAG_POSITION, "");
    QString completion = element->attribute(TAG_COMPLETION, "");

    XsltElement *el = new XsltElement(id);
    if(NULL == el) {
        return false;
    }
    if(!use.isEmpty()) {
        el->use = true ;
    }

    if(theClass.isEmpty()) {
        theClass = "a" ;
    }
    if("a" == theClass) {
        el->setInsertMode(XsltInsertable::InsAnyWhere);
    } else if("c" == theClass) {
        el->setInsertMode(XsltInsertable::InsChildInCall);
    } else if("p" == theClass) {
        el->setInsertMode(XsltInsertable::InsInCallParam);
    } else if("w" == theClass) {
        el->setInsertMode(XsltInsertable::InsWhen);
    } else if("t" == theClass) {
        el->setInsertMode(XsltInsertable::InsTopLevel);
    } else if("r" == theClass) {
        el->setInsertMode(XsltInsertable::InsRoot);
    } else {
        Utils::error(QString(tr("Invalid XSLT class ('%2') found for token '%1' ")).arg(id).arg(theClass));
        delete el;
        return false;
    }
    el->thisClass = theClass ;
    bool ok = false;
    el->cardinality = cardinality.toInt(&ok);
    if(!ok) {
        el->cardinality = 0 ;
    }
    // default value (change???)
    if(childClasses.isEmpty()) {
        childClasses = "a";
    }
    el->childrenClasses = childClasses.split(" ");

    if(position.isEmpty()) {
        el->position = XsltPosition::PositionAll;
    } else if("a" == position) {
        el->position = XsltPosition::PositionAll;
    } else if("f" == position) {
        el->position = XsltPosition::PositionFirst;
    } else if("l" == position) {
        el->position = XsltPosition::PositionLast;
    } else {
        Utils::error(QString(tr("Invalid XSLT position ('%2') found for token '%1' ")).arg(id).arg(position));
        delete el;
        return false;
    }

    if(completion.isEmpty()) {
        el->completionType = XsltCompletionType::All;
    } else if("a" == completion) {
        el->completionType = XsltCompletionType::All;
    } else if("t" == completion) {
        el->completionType = XsltCompletionType::TemplateNames;
    } else if("c" == completion) {
        el->completionType = XsltCompletionType::ParameterNames;
    } else {
        Utils::error(QString(tr("Invalid XSLT completion type ('%2') found for token '%1' ")).arg(id).arg(completion));
        delete el;
        return false;
    }

    el->isClearAttributes = element->attribute(TAG_CLEARATTRS, "") == "true";

    QString nameAttrib = element->attribute(NAME_ATTRIBUTE_NAME, _defaultAttributeName);
    if(nameAttrib == "~") {
        nameAttrib = "" ;
    }
    QString selectAttrib = element->attribute(SELECT_ATTRIBUTE_NAME, _defaultAttributeSelectName);
    if(selectAttrib == "~") {
        selectAttrib = "" ;
    }
    el->nameAttributeName = nameAttrib;
    el->valueAttributeName = selectAttrib;

    _elementsByTag.insert(id, el);
    return true ;
} // buildToken
