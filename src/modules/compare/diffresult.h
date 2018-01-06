/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2018 by Luca Bellonda and individual contributors  *
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


#ifndef DIFFRESULT_H
#define DIFFRESULT_H

#include "element.h"

class EDiff
{
public:
    enum KDiff {
        ED_EQUAL,
        ED_ADDED,
        ED_MODIFIED,
        ED_DELETED,
        ED_DIFFERENT
    };

    static QString value(const KDiff diff);
};

class SourceElementDiffOperation
{
    Element *_sourceElement;
    //--
    QList<Attribute*> _addedAttributes;
    QList<Attribute*> _modifiedAttributes;

public:
    SourceElementDiffOperation(Element *value);
    virtual ~SourceElementDiffOperation();

    void setTestData();
    Element *element()
    {
        return _sourceElement ;
    }

    QList<Attribute*> &addedAttributes();
    QList<Attribute*> &modifiedAttributes();

    void addAttribute(Attribute *sourceAttribute);
    void addModifiedAttribute(Attribute *sourceAttribute);
};


class DiffSingleNodeResult
{
    //EDiff _type;
    // TODO: distribute on appropriate classes
    SourceElementDiffOperation* _referenceElement;
    SourceElementDiffOperation* _compareElement;
    //-- attributes
    QList<Attribute*> _equalsAttributes;
    // Count of all the attributes.
    int _attributesCount ;

    QList<DiffSingleNodeResult*> _children;
    int evalElementForAttributes(SourceElementDiffOperation *value);

public:
    DiffSingleNodeResult(SourceElementDiffOperation* newReferenceElement, SourceElementDiffOperation* newCompareElement);
    virtual ~DiffSingleNodeResult();
    virtual EDiff::KDiff type() = 0;

    SourceElementDiffOperation* referenceElement();
    SourceElementDiffOperation* compareElement();
    void setReferenceElement(SourceElementDiffOperation* value);
    void setCompareElement(SourceElementDiffOperation* value);
    bool hasAttributes();

    void addEqualAttribute(Attribute *newAttr);
    QList<DiffSingleNodeResult*> &children();

    QList<Attribute*> &equalsAttributes();

    void evaluateAttributes();

};


class EqualsDiffNodeResult : public DiffSingleNodeResult
{
public:
    EqualsDiffNodeResult(SourceElementDiffOperation* newReferenceElement, SourceElementDiffOperation* newCompareElement);
    virtual ~EqualsDiffNodeResult();

    EDiff::KDiff type()
    {
        return EDiff::ED_EQUAL ;
    }
};


class AddedDiffNodeResult : public DiffSingleNodeResult
{
public:
    AddedDiffNodeResult(SourceElementDiffOperation* newReferenceElement);
    virtual ~AddedDiffNodeResult();
    EDiff::KDiff type()
    {
        return EDiff::ED_ADDED ;
    }
};

class DeletedDiffNodeResult : public DiffSingleNodeResult
{
public:
    DeletedDiffNodeResult(SourceElementDiffOperation* newCompareElement);
    virtual ~DeletedDiffNodeResult();
    EDiff::KDiff type()
    {
        return EDiff::ED_DELETED ;
    }
};

class ModifiedDiffNodeResult : public DiffSingleNodeResult
{
public:
    ModifiedDiffNodeResult(SourceElementDiffOperation* newReferenceElement, SourceElementDiffOperation* newCompareElement);
    virtual ~ModifiedDiffNodeResult();
    EDiff::KDiff type()
    {
        return EDiff::ED_MODIFIED ;
    }
};


class DiffNodesChangeList
{

    bool _isReferenceEqualToCompare;
    QList<DiffSingleNodeResult *>_rootLevel;
public:
    DiffNodesChangeList();
    ~DiffNodesChangeList();

    bool isReferenceEqualToCompare();

    void reset();
    QList<DiffSingleNodeResult *>& rootLevel();
    void addToRootLevel(DiffSingleNodeResult *node);
    void setDifferent();
};


class DiffInfo
{
public:
    int id;
    int itemPosition;
    DiffSingleNodeResult *diff;
    Attribute* attribute;
    EDiff::KDiff diffType;
    bool isElementOrAttribute; // elemento o attributo

    DiffInfo(const int newId, const EDiff::KDiff newType, const bool newIsElementOrAttribute, DiffSingleNodeResult *newDiff, Attribute* attribute);
    ~DiffInfo();
};



#endif // DIFFRESULT_H
