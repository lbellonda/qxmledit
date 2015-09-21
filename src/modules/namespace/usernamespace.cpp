/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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


#include "usernamespace.h"


/* xml serialized form:
 * <namespace uri="" defaultPrefix="" >
 *  <prefix value=""/>
 * </namespace>
 */

#define ELM_NAMESPACE "namespace"
#define ATTR_URI "uri"
#define ELM_PREFIX "prefix"
#define ATTR_DEFPREFIX  "defaultPrefix"
#define ATTR_VALUE  "value"
#define ATTR_LOCATION  "schemaLocation"

UserNamespace::UserNamespace()
{
    _data = NULL ;
}

UserNamespace::~UserNamespace()
{
}


QString UserNamespace::schemaLocation() const
{
    return _schemaLocation;
}

void UserNamespace::setSchemaLocation(const QString &value)
{
    _schemaLocation = value;
}
void UserNamespace::reset()
{
    _uri = "" ;
    _schemaLocation = "";
    _prefixes.clear();
    _preferredPrefix = "" ;
}

//----------------------------------------------------------------------------------------


QString UserNamespace::name() const
{
    return _data->name();
}


void UserNamespace::setName(const QString &name)
{
    _data->setName(name);
}

QString UserNamespace::uri() const
{
    return _uri;
}

void UserNamespace::setUri(const QString &uri)
{
    _uri = uri;
}

QString UserNamespace::description() const
{
    return _data->description();
}

void UserNamespace::setDescription(const QString &description)
{
    _data->setDescription(description);
}

QStringList UserNamespace::prefixes() const
{
    return _prefixes;
}

void UserNamespace::setPrefixes(const QStringList &prefixes)
{
    _prefixes = prefixes;
}

QString UserNamespace::preferredPrefix() const
{
    return _preferredPrefix;
}

void UserNamespace::setPreferredPrefix(const QString &preferredPrefix)
{
    _preferredPrefix = preferredPrefix;
}

GenericPersistentDBData *UserNamespace::data() const
{
    return _data;
}

bool UserNamespace::setData(GenericPersistentDBData *value)
{
    if(NULL != _data) {
        delete _data ;
    }
    _data = value ;
    bool ok = readFromSerializedXmlString(_data->payload());
    return ok;
}

bool UserNamespace::syncToData()
{
    QString newPayload = toXMLSerializedString();
    _data->setPayload(newPayload);
    return true ;
}

//----------------------------------------------------------------------------------------

bool UserNamespace::saveToDom(QDomDocument &document)
{
    bool result = true ;
    QDomProcessingInstruction preamble = document.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\" ");
    document.appendChild(preamble);
    QDomElement profileNamespace = document.createElement(ELM_NAMESPACE);
    document.appendChild(profileNamespace);
    profileNamespace.setAttribute(ATTR_URI, _uri);
    profileNamespace.setAttribute(ATTR_DEFPREFIX, _preferredPrefix);
    profileNamespace.setAttribute(ATTR_LOCATION, _schemaLocation);

    foreach(QString prefix, _prefixes) {
        QDomElement prefixElement = document.createElement(ELM_PREFIX);
        prefixElement.setAttribute(ATTR_VALUE, prefix);
        profileNamespace.appendChild(prefixElement);
    }
    return result;
}

QString UserNamespace::toXMLSerializedString()
{
    QDomDocument domDocument ;
    saveToDom(domDocument);
    QString result;
    QTextStream streamOut(&result);
    streamOut.setCodec("UTF-8");
    streamOut << domDocument.toString(1);
    streamOut.flush();
    return result ;
}

bool UserNamespace::readFromSerializedXmlString(const QString &string)
{
    QDomDocument document;
    if(document.setContent(string)) {
        int nodi = document.childNodes().size();
        for(int i = 0 ; i < nodi ; i ++) {
            QDomNode childNode = document.childNodes().item(i) ;
            if(childNode.isElement()) {
                QDomElement element = childNode.toElement();
                if(element.tagName() == ELM_NAMESPACE) {
                    if(readFromDom(element)) {
                        return true ;
                    }
                }
            }
        }
    }
    return false;
}

bool UserNamespace::readFromDom(const QDomElement &element)
{
    bool result = true;
    reset();
    _uri = element.attribute(ATTR_URI);
    _preferredPrefix = element.attribute(ATTR_DEFPREFIX);
    _schemaLocation = element.attribute(ATTR_LOCATION);
    int nodi = element.childNodes().size();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = element.childNodes().item(i) ;

        if(childNode.isElement()) {
            QDomElement child = childNode.toElement();
            if(ELM_PREFIX == child.tagName()) {
                QString prefix = child.attribute(ATTR_VALUE);
                _prefixes.append(prefix);
            }
        }
    }
    return result;
}


