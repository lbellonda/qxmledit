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
#define MEMBER "member"
#define GROUP "group"
#define GROUPREF "groupRef"

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
    _currentGroup = NULL ;
    _tokens = NULL ;
    _foundChildren = false ;
}

SCXMLTokenLoader::~SCXMLTokenLoader()
{
    QList<SCXMLGroup*> groups = _groups.values();
    EMPTYPTRLIST(groups, SCXMLGroup);
    _groups.clear();
}

bool SCXMLTokenLoader::load(const QString &fileName, QHash<QString, SCXMLToken*> *tokens)
{
    _tokens = tokens ;
    _currentToken = NULL ;
    _currentGroup = NULL ;
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

bool SCXMLTokenLoader::startElement(const QString &/*namespaceURI*/, const QString &/*localName*/,
                                    const QString & qName, const QXmlAttributes &attributes)
{
    // names with and without ns
    if(qName == ROOT) {
        _foundChildren = false;
        return true ;
    }
    if(qName == CHILDREN) {
        _foundChildren = true ;
        return true ;
    }
    if(qName == TOKEN) {
        _foundChildren = false ;
        QString tag ;
        tag = attributes.value(A_NAME);
        QString ref = attributes.value(A_REF);
        if(tag.isEmpty() && (ref != V_ROOT)) {
            setErrorMessage(QObject::tr("Bad SCXML data, code 2"));
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
    if((qName == CHILD) || (qName == MEMBER)) {
        if(qName == CHILD) {
            if(!_foundChildren) {
                setErrorMessage(QObject::tr("Bad SCXML data, code 1"));
                return false;
            }
            if(NULL == _currentToken) {
                setErrorMessage(QObject::tr("Bad SCXML data, code 3"));
                return false;
            }
        } else {
            if(NULL == _currentGroup) {
                setErrorMessage(QObject::tr("Bad SCXML data, code 10"));
                return false;
            }
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
        if(qName == CHILD) {
            _currentToken->addChild(child);
        } else {
            _currentGroup->addChild(child);
        }
        return true;
    }
    if(qName == GROUPREF) {
        if(NULL == _currentToken) {
            return setErrorMessage(QObject::tr("Bad SCXML data, code 20"));
        }
        QString groupName = attributes.value(A_NAME);
        SCXMLGroup *group = _groups[groupName];
        if(NULL == group) {
            return setErrorMessage(QObject::tr("Bad SCXML data, code 21 for '%1'").arg(groupName));
        }
        // copy children
        foreach(SCXMLTokenChild *gChild, group->_children) {
            SCXMLTokenChild * child = new SCXMLTokenChild();
            if(NULL == child) {
                return oomError();
            }
            child->setName(gChild->name());
            child->setDescription(gChild->description());
            child->setMax(gChild->max());
            child->setMin(gChild->min());
            _currentToken->addChild(child);
        }
        //--- end
    }

    if(qName == GROUP) {
        _foundChildren = false ;
        QString name ;
        name = attributes.value(A_NAME);
        if(name.isEmpty()) {
            setErrorMessage(QObject::tr("Bad SCXML data, code 6"));
            return false;
        }
        _currentGroup = new SCXMLGroup(name);
        if(NULL == _currentGroup) {
            return oomError();
        }
        _groups.insert(_currentGroup->_name, _currentGroup);
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

#define LOAD(xxx)     else if(tag == SCXMLToken::Tag_ ## xxx ) { return new SCXML ## xxx ##Token(); }

SCXMLToken *SCXMLTokenLoader::createSCXMLToken(const QString &tag)
{
    /*if(tag.isEmpty()) {
        return new SCXMLRootToken();
    } else if(tag == SCXMLToken::Tag_scxml) {
        return new SCXMLscxmlToken();
    } else if(tag == SCXMLToken::Tag_state) {
        return new SCXMLstateToken();
    } else if(tag == SCXMLToken::Tag_parallel) {
        return new SCXMLparallelToken();
    } else if(tag == SCXMLToken::Tag_transition) {
        return new SCXMLtransitionToken();
    } else if(tag == SCXMLToken::Tag_initial) {
        return new SCXMLinitialToken();
    } else if(tag == SCXMLToken::Tag_final) {
        return new SCXMLfinalToken();
    } else if(tag == SCXMLToken::Tag_onentry) {
        return new SCXMLonentryToken();
    } else if(tag == SCXMLToken::Tag_onexit) {
        return new SCXMLonexitToken();
    } else if(tag == SCXMLToken::Tag_history) {
        return new SCXMLhistoryToken();
    } else if(tag == SCXMLToken::Tag_raise) {
        return new SCXMLraiseToken();
    } else if(tag == SCXMLToken::Tag_if) {
        return new SCXMLifToken();
    }*/
    if(tag.isEmpty()) {
        return new SCXMLRootToken();
    }
    LOAD(scxml)
    LOAD(state)
    LOAD(parallel)
    LOAD(transition)
    LOAD(initial)
    LOAD(final)
    LOAD(onentry)
    LOAD(onexit)
    LOAD(history)
    LOAD(raise)
    LOAD(if)
    LOAD(elseif)
    LOAD(else)
    LOAD(foreach)
    LOAD(log)
    LOAD(datamodel)
    LOAD(data)
    LOAD(assign)
    LOAD(donedata)
    LOAD(content)
    LOAD(param)
    LOAD(script)
    LOAD(cancel)
    LOAD(finalize)
    LOAD(send)
    LOAD(invoke)
    return new SCXMLGenericToken();
}
