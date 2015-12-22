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

#include "xmlEdit.h"
#include "anoncontext.h"
#include "anonfixedalg.h"
#include "anonseqproducer.h"
#include "anonallalg.h"
#include "anoncodealg.h"
#include "utils.h"
#include "xmlutils.h"

//--------------------------

AnonContextText::AnonContextText(AnonContext *newParent) : AnonContext(newParent, QString("text()"))
{
}

AnonContextText::~AnonContextText()
{
}

//--------------------------

AnonContextAttribute::AnonContextAttribute(AnonContext *newParent, const QString &name) : AnonContext(newParent, QString("@%1").arg(name))
{
}

AnonContextAttribute::~AnonContextAttribute()
{
}

//--------------------------

AnonContext::AnonContext(AnonContext *newParent, const QString &parmName)
{
    _thisAlg = NULL ;
    _parent = newParent ;
    _name = parmName ;
    _inited = false ;
    if(NULL == newParent) {
        _path = "";
        _pathQualified = "";
        _criteria = AnonInclusionCriteria::IncludeWithChildren ;
        _alg = NULL ;
        _thisProfile = new AnonProfile();
        _profile = _thisProfile ;
        _origData = NULL ;
    } else {
        _path = QString("%1/%2").arg(newParent->path()).arg(parmName)  ;
        _pathQualified = newParent->_pathQualified ;
        _criteria = _parent->_criteria ;
        _alg = _parent->_alg;
        _thisProfile = NULL ;
        _profile = _parent->_profile ;
        _origData = _parent->_origData;
        _currentNs = _parent->_currentNs ;
    }
    _anonType = AnonType::UseDefault ;
}

AnonContext::~AnonContext()
{
    deleteAlg();
    if(NULL != _thisProfile) {
        delete _thisProfile ;
    }
}

void AnonContext::removeExceptions()
{
    if(NULL != _profile) {
        _profile->reset();
    }
}

void AnonContext::addNamespaceAttribute(const QString &attributeName, const QString &value)
{
    if(XmlUtils::isDeclaringNS(attributeName)) {
        QString nsPrefix;
        if(XmlUtils::getNsPrefix(attributeName, nsPrefix)) {
            addNamespace(nsPrefix, value);
        }
    }
}

void AnonContext::addNamespace(const QString &prefix, const QString &ns)
{
    _namespacesByPrefix.insert(prefix, ns);
}

void AnonContext::setContextNamespace(const QString &ns, const QString &localName)
{
    _currentNs = ns ;
    if(ns.isEmpty()) {
        _pathQualified += QString("/%1").arg(localName);
    } else {
        _pathQualified += QString("/{%1}%2").arg(ns).arg(localName);
    }
}

void AnonContext::setContextNamespaceAttribute(const QString &ns, const QString &localName)
{
    _currentNs = ns ;
    if(ns.isEmpty()) {
        _pathQualified += QString("/@%1").arg(localName);
    } else {
        _pathQualified += QString("/@{%1}%2").arg(ns).arg(localName);
    }
}

void AnonContext::setContextElement(const QString &name)
{
    QString prefix;
    QString localName;
    XmlUtils::decodeQualifiedName(name, prefix, localName);
    QString nsUri = uriFromPrefix(prefix);
    setContextNamespace(nsUri, localName);
}

void AnonContext::pushContextNamespaceAttribute(const QString &name)
{
    QString prefix;
    QString localName;
    XmlUtils::decodeQualifiedName(name, prefix, localName);
    QString nsUri;
    if(!prefix.isEmpty()) {
        nsUri = uriFromPrefix(prefix);
    }
    setContextNamespaceAttribute(nsUri, localName);
}

void AnonContext::pushContextNamespaceText()
{
    _pathQualified    += QString("/text()");
}

QString AnonContext::uriFromName(const QString &name)
{
    QString prefix;
    QString localName;
    XmlUtils::decodeQualifiedName(name, prefix, localName);
    return uriFromPrefix(prefix);
}

QString AnonContext::uriFromPrefix(const QString &name)
{
    if(_namespacesByPrefix.contains(name)) {
        return _namespacesByPrefix[name];
    }
    if(NULL != _parent) {
        return _parent->uriFromPrefix(name);
    }
    return "";
}

void AnonContext::resetProfile()
{
    if(NULL != _thisProfile) {
        delete _thisProfile ;
        _thisProfile = NULL ;
    }
}

void AnonContext::deleteAlg()
{
    if(_thisAlg != NULL) {
        _thisAlg->autodelete();
        _thisAlg = NULL ;
    }
}

void AnonContext::setProfile(AnonProfile *newProfile)
{
    resetProfile();
    _thisProfile = newProfile;
    _profile = newProfile ;
}

QString AnonContext::path()
{
    return _path;
}

QString AnonContext::qualifiedPath()
{
    return _pathQualified;
}


AnonContext *AnonContext::clone(const QString &newTag)
{
    AnonContext *newContext = new AnonContext(this, newTag);
    return newContext ;
}

void AnonContext::setAlg(AnonymizeParameters *params)
{
    deleteAlg();
    _thisAlg = getAlg(params);
    _alg = _thisAlg ;
}

void AnonContext::setAlg(AnonAlg *parmAlg)
{
    deleteAlg();
    _thisAlg = parmAlg ;
    _alg = _thisAlg ;
}


AnonAlg *AnonContext::getAlg(AnonymizeParameters *params)
{
    AnonAlg *alg = NULL ;
    AnonProducer *producer = NULL ;
    if(params->useFixedLetter) {
        producer = new AnonFixedProducer();
    } else {
        producer = new AnonSeqProducer();
    }

    switch(params->mode) {
    default:
    case AnonymizeParameters::AllText:
        alg = new AnonAllAlg(true, producer);
        break;
    case AnonymizeParameters::UsingPatterns: {
        AnonCodeAlg *alg2 = new AnonCodeAlg(true, producer);
        alg2->setThreshold(params->threshold);
        alg = alg2 ;
    }
    break;
    }
    return alg;
}

bool AnonContext::canAnonymize(AnonException *exception)
{
    AnonInclusionCriteria::Criteria crit = _criteria ;
    if(NULL != exception) {
        crit = exception->criteria();
    }
    switch(crit) {
    default:
        return false;
    case AnonInclusionCriteria::Include:
    case AnonInclusionCriteria::IncludeWithChildren:
        return true ;
    }
}

QString AnonContext::anonymize(AnonException *exception, const QString &inputData)
{
    AnonType::Type anonType = _anonType;
    QString translatedValue ;
    if(NULL != exception) {
        anonType = exception->anonType();
    }
    switch(anonType) {
    case AnonType::FixedValue:
        translatedValue = exception->fixedValue();
        break;
    default:
    case AnonType::UseDefault:
        translatedValue = _alg->processText(inputData);
        break;
    }
    return translatedValue ;
}

bool AnonContext::isCriteriaTemporary()
{
    switch(_criteria) {
    default:
        return false;
    case AnonInclusionCriteria::Exclude:
    case AnonInclusionCriteria::Include:
        return true ;
    }
}

void AnonContext::restoreContext()
{
    if(isCriteriaTemporary()) {
        if(NULL != _parent) {
            _criteria = _parent->_criteria ;
        }
    }
}

void AnonContext::setExceptionForElement()
{
    init();
    if(!applyException(_profile->getExceptionByPathWithNamespace(_pathQualified))) {
        applyException(_profile->getExceptionByPath(_path));
    }
}

bool AnonContext::applyException(AnonException *exception)
{
    if(NULL != exception) {
        _criteria = exception->criteria() ;
        return true ;
    }
    return false;
}

AnonException *AnonContext::getException(const QString & /*parmName*/, const QString &parmPath)
{
    init();
    AnonException *exception = _profile->getExceptionByPath(parmPath);
    if(NULL != exception) {
        return exception;
    }
    return NULL ;
}

AnonException *AnonContext::getExceptionQualified(const QString & /*parmName*/, const QString &parmPath)
{
    init();
    AnonException *exception = _profile->getExceptionByPathWithNamespace(parmPath);
    if(NULL != exception) {
        return exception;
    }
    return NULL ;
}

AnonException *AnonContext::getException()
{
    AnonException *exc = getExceptionQualified(_name, _pathQualified);
    if(NULL != exc) {
        return exc ;
    }
    return getException(_name, _path);
}

void AnonContext::addException(AnonException *exc)
{
    _profile->addException(exc);
}

void AnonContext::init()
{
    if(_inited) {
        return ;
    }
    _inited = true ;
    // build exception maps
}

void AnonContext::setOrigData(void *key, const QString &ref)
{
    if(NULL != _origData) {
        _origData->insert(key, ref);
    }
}

QHash<void *, QString> *AnonContext::origData()
{
    return _origData;
}

void AnonContext::setOrigData(QHash<void *, QString> *newOrigData)
{
    _origData = newOrigData;
}

bool AnonContext::isCollectingData()
{
    return _origData != NULL ;
}
