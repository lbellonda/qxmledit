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


#ifndef XSCHEMAOPERATIONCONTEXT_H
#define XSCHEMAOPERATIONCONTEXT_H

#include <QObject>
#include <QSet>


class XSchemaObject;
class XSchemaElement;

/**
 * @brief base class for operations
 */
class XSchemaOperationContext : public QObject
{
    Q_OBJECT
public:
    explicit XSchemaOperationContext(QObject *parent = 0);
    virtual ~XSchemaOperationContext();

signals:

public slots:

};

class XSchemaGroup ;
class XSchemaInquiryContext : public XSchemaOperationContext
{
    Q_OBJECT

    //static const int MaxGroupObjects = 100;
    QSet<XSchemaObject*> _enrolled;
    QSet<QString> _typeErrors;
    //--- group reference: for each group: count of children
    class GroupWeight
    {
        int _occurrences ;
        int _childrenCount;
        int size()
        {
            return _occurrences * _childrenCount ;
        }
        GroupWeight()
        {
            _occurrences = 0;
            _childrenCount = 0 ;
        }
        friend class XSchemaInquiryContext ;
    };
    QHash<XSchemaGroup*, GroupWeight*> _groups ;
    bool *_enrollFlag;
    bool _honorRestrictions;
    bool _isFullCollection;
    bool _isRecursive;

public:
    explicit XSchemaInquiryContext(QObject *parent = 0);
    virtual ~XSchemaInquiryContext();

    void reset();
    void resetErrors();

    void enroll(XSchemaObject* object);
    bool isEnrolled(XSchemaObject* object);
    void addError(XSchemaElement*element);
    QStringList typeErrors();

    bool isGroupEnrolled(XSchemaGroup *group);
    void enrollGroup(XSchemaGroup *group, const int childrenCount);

    bool *enrollFlag() const;
    void setEnrollFlag(bool *enrollFlag);
    bool testAndResetEnrollFlag();

    bool isHonorRestrictions() const;
    void setHonorRestrictions(bool honorRestrictions);

    bool isFullCollection() const;
    void setFullCollection(bool isFullCollection);

    bool isRecursive() const;
    void setIsRecursive(bool isRecursive);

signals:

public slots:

};

class XSchemaFindReferenceContext : public XSchemaOperationContext
{
    Q_OBJECT

    bool _isPredefinedType;
    XSchemaElement *_resolved;
    QString _namespaceUri;
    QString _namespacePrefix;

public:
    explicit XSchemaFindReferenceContext(QObject *parent = 0);
    virtual ~XSchemaFindReferenceContext();


    bool isPredefinedType() const;
    void setIsPredefinedType(bool isPredefinedType);

    XSchemaElement *resolved() const;
    void setResolved(XSchemaElement *resolved);

    QString namespaceUri() const;
    void setNamespaceUri(const QString &namespaceUri);

    QString namespacePrefix() const;
    void setNamespacePrefix(const QString &namespacePrefix);

signals:

public slots:

};

#endif // XSCHEMAOPERATIONCONTEXT_H
