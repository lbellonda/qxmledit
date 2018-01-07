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


#ifndef ANONCONTEXT_H
#define ANONCONTEXT_H

#include "libQXmlEdit_global.h"
#include "modules/anonymize/anonbase.h"
#include "modules/anonymize/anonprofile.h"
#include "modules/anonymize/anonexception.h"

class AnonAttr;

/*!
 * \brief The AnonElem class has info for current element anon state (to invert changes interactivly)
 */
class LIBQXMLEDITSHARED_EXPORT AnonElem
{
public:
    AnonElem();
    ~AnonElem();

    void addAttribute(AnonAttr *anonAttr);
};

class LIBQXMLEDITSHARED_EXPORT AnonContext
{
protected:
    AnonAlg *_alg;
    AnonAlg *_thisAlg;
    AnonContext *_parent;
    QString _path;
    QString _pathQualified;
    QString _name;
    QString _currentNs;
    AnonInclusionCriteria::Criteria _criteria;
    AnonType::Type _anonType ;
    AnonProfile *_profile;
    AnonProfile *_thisProfile;
    bool _inited;
    QHash<QString, QString> _namespacesByPrefix;
    //--- these variables are temporary operation data, i.e. are cleared at next run
    QHash<void *, QString> *_origData;

    AnonAlg *getAlg(AnonymizeParameters *params);
    void deleteAlg();
    void init();
    void resetProfile();
    void addNamespace(const QString &prefix, const QString &ns);
    void setContextNamespace(const QString &ns, const QString &localName);
    void setContextNamespaceAttribute(const QString &ns, const QString &localName);
    AnonException *getExceptionQualified(const QString & parmName, const QString &parmPath);

public:
    AnonContext(AnonContext *newParent, const QString &parmName);
    virtual ~AnonContext();
    //---------
    AnonContext *clone(const QString &newTag);

    void setPathTo(const QString &tag);
    QString path();
    QString qualifiedPath();
    void setContextElement(const QString &name);
    void pushContextNamespaceAttribute(const QString &name);
    void pushContextNamespaceText();
    void addNamespaceAttribute(const QString &attributeName, const QString &value);

    AnonException *exceptionByName(const QString &name);
    AnonException *exceptionByPath(const QString &path);
    void setExceptionForElement();
    void removeExceptions();

    void addAttribute(AnonAttr *newValue);
    bool applyException(AnonException *exc);
    /*!
     * \brief isForce if the operation is imposed as dogma
     */
    bool isForce();

    void findAndApplyExceptionOnElement();
    AnonException *getException();
    AnonException *getException(const QString &parmName, const QString &parmPath);
    QString anonymize(AnonException *exception, const QString &data);
    bool canAnonymize(AnonException *exception);

    void setAlg(AnonymizeParameters *params);
    void setAlg(AnonAlg *parmAlg);
    /*!
     * \brief restoreContext removes any temporary exception
     */
    void restoreContext();
    bool isCriteriaTemporary();
    void addException(AnonException *exc);
    void setProfile(AnonProfile *newProfile);
    //---
    QHash<void *, QString> *origData();
    void setOrigData(QHash<void *, QString> *newOrigData);
    void setOrigData(void *key, const QString &ref);
    bool isCollectingData();
    //--
    QString uriFromName(const QString &name);
    QString uriFromPrefix(const QString &name);
};

class LIBQXMLEDITSHARED_EXPORT AnonContextText : public AnonContext
{
public:
    AnonContextText(AnonContext *newParent);
    virtual ~AnonContextText();
};

class LIBQXMLEDITSHARED_EXPORT AnonContextAttribute : public AnonContext
{
public:
    AnonContextAttribute(AnonContext *newParent, const QString &parmName);
    virtual ~AnonContextAttribute();
};

#endif // ANONCONTEXT_H
