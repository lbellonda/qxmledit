/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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

#include "anonprofile.h"
#include "utils.h"
#include "xmlutils.h"
#include "anonymizeparameters.h"

#define ELM_EXCEPTS "exceptions"
#define ELM_EXCEPT "exception"
#define ELM_PROFILE "profile"
#define ELM_PARAMS "params"
#define ATTR_USENS  "useNamespace"
//-----

AnonProfile::AnonProfile()
{
    _useNamespace = false;
    _params = new AnonymizeParameters() ;
}


AnonProfile::~AnonProfile()
{
    reset();
    delete _params;
}


AnonProfile *AnonProfile::clone()
{
    AnonProfile *newProfile = new AnonProfile();
    newProfile->_useNamespace = _useNamespace ;
    foreach(AnonException * ex, _exceptions) {
        newProfile->addException(ex->clone());
    }
    newProfile->setParams(_params);
    return newProfile ;
}

void AnonProfile::reset()
{
    while(!_exceptions.isEmpty()) {
        AnonException *e = _exceptions.first();
        _exceptions.removeFirst();
        delete e;
    }
    _exceptionsByPath.clear();
}

void AnonProfile::addException(AnonException *newExc)
{
    _exceptions.append(newExc);
    _exceptionsByPath.insert(newExc->path(), newExc);
}

void AnonProfile::removeException(AnonException *exc)
{
    if(_exceptions.contains(exc)) {
        _exceptions.removeOne(exc);
        _exceptionsByPath.remove((exc)->path());
        delete exc;
    }
}

QList<AnonException *> AnonProfile::exceptions()
{
    return _exceptions;
}

AnonException *AnonProfile::getExceptionByPath(const QString &thePath)
{
    AnonException *exception = NULL ;
    if(_exceptionsByPath.contains(thePath)) {
        exception = _exceptionsByPath[thePath];
    }
    return exception  ;
}

AnonException *AnonProfile::getExceptionByPathWithNamespace(const QString &thePath)
{
    AnonException *exception = NULL ;
    if(_exceptionsByPath.contains(thePath)) {
        exception = _exceptionsByPath[thePath];
        if(NULL != exception) {
            if(!exception->isUseNamespace()) {
                exception = NULL ;
            }
        }
    }
    return exception  ;
}

bool AnonProfile::readFromSerializedXmlString(const QString &string)
{
    QDomDocument document;
    if(document.setContent(string)) {
        int nodi = document.childNodes().size();
        for(int i = 0 ; i < nodi ; i ++) {
            QDomNode childNode = document.childNodes().item(i) ;
            if(childNode.isElement()) {
                QDomElement element = childNode.toElement();
                if(element.tagName() == ELM_PROFILE) {
                    if(readFromDom(element)) {
                        return true ;
                    }
                }
            }
        }
    }
    return false;
}

bool AnonProfile::readFromDom(const QDomElement &element)
{
    bool result = true;
    reset();
    _useNamespace = XmlUtils::readFromBool(element.attribute(ATTR_USENS), true);
    QList<AnonException*> eList;
    int nodi = element.childNodes().size();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = element.childNodes().item(i) ;

        if(childNode.isElement()) {
            QDomElement child = childNode.toElement();
            if(ELM_EXCEPTS == child.tagName()) {
                if(!scanExceptionsFromDom(child, eList)) {
                    result = false;
                }
            } else if(ELM_PARAMS == child.tagName()) {
                _params->readFromDom(child);
            }
        }
    }
    foreach(AnonException * ex, eList) {
        addException(ex);
    }
    return result;
}

bool AnonProfile::useNamespace() const
{
    return _useNamespace;
}

void AnonProfile::setUseNamespace(bool useNamespace)
{
    _useNamespace = useNamespace;
}


AnonymizeParameters *AnonProfile::params() const
{
    return _params;
}

void AnonProfile::setParams(AnonymizeParameters *params)
{
    _params->copyValuesFrom(params);
}

bool AnonProfile::scanExceptionsFromDom(const QDomElement &element, QList<AnonException*> &eList)
{
    bool result = true;

    int nodi = element.childNodes().size();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = element.childNodes().item(i) ;

        if(childNode.isElement()) {
            QDomElement childElement = childNode.toElement();
            if(ELM_EXCEPT == childElement.tagName()) {
                AnonException *ex = new AnonException();
                if(!ex->readFromDom(childElement)) {
                    result = false;
                }
                // insert it even if not fully read
                eList.append(ex);
            }
        }
    }
    return result;
}

bool AnonProfile::saveToDom(QDomDocument &document)
{
    bool result = true ;
    QDomProcessingInstruction preamble = document.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\" ");
    document.appendChild(preamble);
    QDomElement profileElement = document.createElement(ELM_PROFILE);
    document.appendChild(profileElement);
    profileElement.setAttribute(ATTR_USENS, XmlUtils::boolToBoolValue(_useNamespace));

    QDomElement paramsElement = document.createElement(ELM_PARAMS);
    _params->saveToDom(paramsElement);
    profileElement.appendChild(paramsElement);

    QDomElement exceptionsElement = document.createElement(ELM_EXCEPTS);
    profileElement.appendChild(exceptionsElement);
    foreach(AnonException * ex, _exceptions) {
        QDomElement exceptionElement = document.createElement(ELM_EXCEPT);
        exceptionsElement.appendChild(exceptionElement);
        if(!ex->saveToDom(exceptionElement)) {
            result = false;
        }
    }
    return result;
}


QString AnonProfile::toXMLSerializedString()
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
