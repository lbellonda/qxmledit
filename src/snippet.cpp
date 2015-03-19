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

#include "snippet.h"
#include "utils.h"
#include <QFile>
#include <QUuid>
#include <QTextStream>

#define TAG_ROOT    "root"
#define TAG_NAME    "name"
#define TAG_DESCRIPTION "description"
#define TAG_ID  "id"
#define TAG_CREATIONDATE    "creationDate"
#define TAG_UPDATEDATE  "updateDate"
#define TAG_TAGS    "tags"
#define TAG_TAG     "tag"
#define TAG_PAYLOAD "payload"


Snippet::Snippet()
{
    _isReadOnly = false;
    setId(QUuid::createUuid().toString());
}

Snippet::~Snippet()
{
}

bool Snippet::isReadOnly()
{
    return _isReadOnly ;
}

void Snippet::setReadOnly(const bool newReadOnly)
{
    _isReadOnly = newReadOnly ;
}

void Snippet::setId(const QString &newId)
{
    _id = newId;
}

QString Snippet::id()
{
    return _id ;
}

void Snippet::setName(const QString &newName)
{
    _name = newName;
}

QString Snippet::name()
{
    return _name ;
}

void Snippet::setDescription(const QString &newDescription)
{
    _description = newDescription;
}

QString Snippet::description()
{
    return _description ;
}


void Snippet::setPayload(const QString &newPayload)
{
    _xmlPayload = newPayload ;
}

QString Snippet::payload()
{
    return _xmlPayload ;
}

QStringList Snippet::tags()
{
    return _tags;
}

void Snippet::setTags(const QStringList &newTags)
{
    QStringList filteredTags ;
    foreach(QString s, newTags) {
        filteredTags.append(s.toLower());
    }

    _tags = filteredTags ;
}

void Snippet::setUpdateDate(const QDateTime &date)
{
    _updateDate = date ;
}

void Snippet::setCreationDate(const QDateTime &date)
{
    _creationDate = date ;
}

QDateTime Snippet::updateDate()
{
    return _updateDate ;
}

QDateTime Snippet::creationDate()
{
    return _creationDate;
}

//-------------------------- i/o ----------------------------------


bool Snippet::save(const QString &filePath)
{
    QDomDocument document;
    QDomElement rootNode = document.createElement(TAG_ROOT);
    document.appendChild(rootNode);
    //name
    Utils::appendTextNode(document, rootNode, TAG_NAME, name());
    Utils::appendTextNode(document, rootNode, TAG_DESCRIPTION, description());
    Utils::appendTextNode(document, rootNode, TAG_ID , id());
    Utils::appendTextNode(document, rootNode, TAG_CREATIONDATE, creationDate().toString(Qt::ISODate));
    Utils::appendTextNode(document, rootNode, TAG_UPDATEDATE, updateDate().toString(Qt::ISODate));

    QDomElement tagsNode = document.createElement(TAG_TAGS);
    rootNode.appendChild(tagsNode);

    //QDomElement tagsNode = Utils::appendNode( document, rootNode, TAG_TAGS );
    foreach(QString tag, tags()) {
        Utils::appendTextNode(document, tagsNode, TAG_TAG, tag);
    }
    Utils::appendTextNode(document, rootNode, TAG_PAYLOAD, /*Utils::toBase64(*/payload()/*)*/);

    QFile   file(filePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        //Utils::error(tr("Error writing data: %1").arg(file.error()));
        return false;
    }
    QTextStream streamOut(&file);
    streamOut.setCodec("UTF-8");
    streamOut << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n";
    streamOut << document.toString(4);
    streamOut.flush();
    file.close();
    return true;
}

bool Snippet::scanData(QDomNode &rootNode)
{
    bool isOk = true ;
    int nodi = rootNode.childNodes().count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = rootNode.childNodes().item(i) ;

        if(childNode.isElement()) {
            QDomElement element = childNode.toElement();
            if(element.tagName() == TAG_NAME) {
                setName(element.text());
            } else if(element.tagName() == TAG_DESCRIPTION) {
                setDescription(element.text());
            } else if(element.tagName() == TAG_ID) {
                setId(element.text());
            } else if(element.tagName() == TAG_CREATIONDATE) {
                setCreationDate(QDateTime::fromString(element.text(), Qt::ISODate));
            } else if(element.tagName() == TAG_UPDATEDATE) {
                setUpdateDate(QDateTime::fromString(element.text(), Qt::ISODate));
            } else if(element.tagName() == TAG_PAYLOAD) {
                setPayload(element.text());
            } else if(element.tagName() == TAG_TAGS) {
                int nodes = element.childNodes().count();
                for(int i = 0 ; i < nodes ; i ++) {
                    QDomNode childNode = element.childNodes().item(i) ;
                    if(childNode.isElement()) {
                        QDomElement element = childNode.toElement();
                        if(element.tagName() == TAG_TAG) {
                            _tags.append(element.text());
                        }
                    }
                }
            }
        }
    }//for
    //check consistency
    if(id().isEmpty() || name().isEmpty()) {
        return false;
    }
    return isOk ;
}//scanData()


bool Snippet::read(const QString &filePath)
{
    bool isOk = false;
    QFile file(filePath);
    if(file.open(QIODevice::ReadOnly)) {
        QDomDocument document;
        if(document.setContent(&file)) {
            int nodes = document.childNodes().count();
            for(int i = 0 ; i < nodes ; i ++) {
                QDomNode childNode = document.childNodes().item(i) ;
                if(childNode.isElement()) {
                    isOk = scanData(childNode);
                }
            }
        }
        if(!isOk) {
            Utils::error(tr("Unable to parse XML"));
        }
        file.close();
    } else {
        Utils::error(QString(tr("Unable to load snippet file.\n Error code is '%1'")).arg(file.error()));
    }
    return isOk ;
}


bool Snippet::remove(const QString &filePath)
{
    bool isOk = true;
    QFile file(filePath);
    if(!file.remove()) {
        Utils::error(tr("Error deleting data: %1 - %2").arg(file.error()).arg(file.errorString()));
        isOk = false ;
    }
    return isOk ;
}

//-------------------------- utility ----------------------------------

void Snippet::resetSnippetContainer(QMap<QString, Snippet*> &snippets)
{
    foreach(Snippet * entry, snippets.values()) {
        delete entry;
    }
    snippets.clear();
}


//-------------------------- test ----------------------------------

#define XML_TEST ""\
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"\
    "<!-- this is a sample QXmlEdit data file -->\n"\
    "<data>\n"\
    "<keywords id=\"2\">\n"\
    "</keywords>\n"\
    "<bau>testo\n"\
    "</bau>\n"\
    "</data>"

//static
void Snippet::loadMockSnippet(QMap<QString, Snippet*> &snippetArray)
{
    Snippet *one = new Snippet();
    one->setId("uno");
    one->setPayload(XML_TEST);
    one->setName("one");
    one->setDescription("this is one");
    one->_tags.append("J2EE");
    one->_tags.append("Java");
    snippetArray[ one->id()] =  one ;
    /*Snippet *two = new Snippet();
    two->setId("two");
    two->setName("two");
    two->setDescription("this is two");
    two->setPayload(XML_TEST);
    two->_tags.append("C++");
    two->_tags.append("Java");
    snippetArray [two->id()] = two ;*/
}

