/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016 by Luca Bellonda and individual contributors       *
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

#include "scxmltokenloader.h"
#include <QXmlSimpleReader>
#include "utils.h"

#define ROOT "root"
#define TOKEN "token"
#define CHILD "child"
#define CHILDREN "children"

#define A_NAME  "name"
#define A_MIN   "min"
#define A_MAX   "max"
#define A_DESCR "d"
#define A_REF "ref"

#define V_ROOT "root"

#define ERROR_CODE -2

SCXMLTokenLoader::SCXMLTokenLoader()
{
    _currentToken = NULL ;
    _tokens = NULL ;
    _foundChildren = false ;
}

SCXMLTokenLoader::~SCXMLTokenLoader()
{

}

bool SCXMLTokenLoader::load(const QString &fileName, QHash<QString, SCXMLToken*> *tokens)
{
    _tokens = tokens ;
    _currentToken = NULL ;
    _foundChildren = false;
    bool isOk = false;
    QXmlSimpleReader reader;
    reader.setFeature("http://xml.org/sax/features/namespaces", false);
    reader.setFeature("http://xml.org/sax/features/namespace-prefixes", true);
    reader.setContentHandler(this);
    reader.setErrorHandler(this);

    QFile file(fileName);
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        return false;
    }
    QXmlInputSource xmlInput(&file);
    if(reader.parse(xmlInput)) {
        isOk = true ;
    }
    file.close();
    return isOk ;
}

bool SCXMLTokenLoader::startElement(const QString &/*namespaceURI*/, const QString &localName,
                                    const QString & /*qName*/, const QXmlAttributes &attributes)
{
    // names with and without ns
    if(localName == ROOT) {
        _foundChildren = false;
        return true ;
    }
    if(localName == CHILDREN) {
        _foundChildren = true ;
        return true ;
    }
    if(localName == TOKEN) {
        _foundChildren = false ;
        QString tag ;
        tag = attributes.value(A_NAME);
        QString ref = attributes.value(A_REF);
        if(tag.isEmpty() && (ref != V_ROOT)) {
            setErrorMessage(QObject::tr("Bad SCXML data, code 2"));
            delete _currentToken ;
            _currentToken = NULL ;
            return false;
        }
        _currentToken = createSCXMLToken(tag);
        if(NULL == _currentToken) {
            return oomError();
        }
        _currentToken->setName(tag);
        _tokens->insert(_currentToken->name(), _currentToken);
        return true;
    }
    if(localName == CHILD) {
        if(!_foundChildren) {
            setErrorMessage(QObject::tr("Bad SCXML data, code 1"));
            return false;
        }
        _foundChildren = false ;
        if(NULL == _currentToken) {
            setErrorMessage(QObject::tr("Bad SCXML data, code 3"));
            return false;
        }
        //<child name="state" min="0" max="u" d="A compound or atomic state. Occurs zero or more times."/>
        QString childName = attributes.value(A_NAME);
        QString childMin = attributes.value(A_MIN);
        QString childMax = attributes.value(A_MAX);
        QString childDescr = attributes.value(A_DESCR);
        SCXMLTokenChild * child = new SCXMLTokenChild();
        if(NULL == child) {
            return oomError();
        }
        child->setName(childName);
        child->setDescription(childDescr);
        child->setMax(decodeCardinality(childMax));
        child->setMin(decodeCardinality(childMin));
        if((ERROR_CODE == child->min()) || (ERROR_CODE == child->min())) {
            setErrorMessage(QObject::tr("Bad SCXML data for '%1'/'%2', code 4").arg(_currentToken->name()).arg(childName));
            return false;
        }
        _currentToken->addChild(child);
        return true;
    }
    return true ;
}

bool SCXMLTokenLoader::fatalError(const QXmlParseException &exception)
{
    Utils::error(QObject::tr("Parse error (2) at line %1, column %2:\n%3")
                 .arg(exception.lineNumber())
                 .arg(exception.columnNumber())
                 .arg(exception.message()));
    return false;
}

bool SCXMLTokenLoader::error(const QXmlParseException &exception)
{
    Utils::error(QObject::tr("Parse error (1) at line %1, column %2:\n%3")
                 .arg(exception.lineNumber())
                 .arg(exception.columnNumber())
                 .arg(exception.message()));
    return false;
}

QString SCXMLTokenLoader::errorString() const
{
    return _parseError;
}

bool SCXMLTokenLoader::setErrorMessage(const QString &value)
{
    _parseError = value ;
    return false;
}

bool SCXMLTokenLoader::oomError()
{
    Utils::errorOutOfMem(NULL);
    _parseError = QObject::tr("Out of Memory during SCXML initialization.");
    return false;
}

int SCXMLTokenLoader::decodeCardinality(const QString &value)
{
    int result = SCXMLTokenChild::Unbounded ;
    if(value.isEmpty() || ("u" == value)) {
        return SCXMLTokenChild::Unbounded ;
    }
    bool ok = false;
    result = value.toInt(&ok);
    if(!ok) {
        result = ERROR_CODE ;
    }
    return result ;
}

SCXMLToken *SCXMLTokenLoader::createSCXMLToken(const QString &tag)
{
    if(tag.isEmpty()) {
        return new SCXMLRootToken();
    } else if(tag == SCXMLToken::Tag_scxml) {
        return new SCXMLscxmlToken();
    }
    return new SCXMLGenericToken();
}
