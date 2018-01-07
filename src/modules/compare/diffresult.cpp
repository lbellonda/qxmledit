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


#include "diffresult.h"
#include "utils.h"

//----------------------------------------

#define EDiff_CLASSNAME "EDiff"

QString EDiff::value(const KDiff diff)
{
    QString result;
    switch(diff) {
    case ED_EQUAL:
        result = qApp->translate(EDiff_CLASSNAME, "equals");
        break;
    case ED_ADDED:
        result = qApp->translate(EDiff_CLASSNAME, "added");
        break;
    case ED_MODIFIED:
        result = qApp->translate(EDiff_CLASSNAME, "modified");
        break;
    case ED_DELETED:
        result = qApp->translate(EDiff_CLASSNAME, "deleted");
        break;
    case ED_DIFFERENT:
        result = qApp->translate(EDiff_CLASSNAME, "different");
        break;
    }
    return result;
}

//--------------------------------------

SourceElementDiffOperation::SourceElementDiffOperation(Element *value)
{
    _sourceElement = value;
}

SourceElementDiffOperation::~SourceElementDiffOperation()
{
}

void SourceElementDiffOperation::setTestData()
{
    _sourceElement = new Element("test", "test", NULL);
    _addedAttributes.append(new Attribute("added", "value"));
    _modifiedAttributes.append(new Attribute("modified", "value"));
}

QList<Attribute*> &SourceElementDiffOperation::addedAttributes()
{
    return _addedAttributes;
}

QList<Attribute*> &SourceElementDiffOperation::modifiedAttributes()
{
    return _modifiedAttributes ;
}

void SourceElementDiffOperation::addAttribute(Attribute *sourceAttribute)
{
    _addedAttributes.append(sourceAttribute);
}

void SourceElementDiffOperation::addModifiedAttribute(Attribute *sourceAttribute)
{
    _modifiedAttributes.append(sourceAttribute);
}
//------------------------------------------------

DiffSingleNodeResult::DiffSingleNodeResult(SourceElementDiffOperation* newReferenceElement, SourceElementDiffOperation* newCompareElement)
{
    _referenceElement = newReferenceElement;
    _compareElement = newCompareElement;
    _attributesCount = 0 ;
}

DiffSingleNodeResult::~DiffSingleNodeResult()
{
    foreach(DiffSingleNodeResult * child,  _children) {
        delete child ;
    }
}

QList<Attribute*> &DiffSingleNodeResult::equalsAttributes()
{
    return _equalsAttributes ;
}

bool DiffSingleNodeResult::hasAttributes()
{
    if(_attributesCount > 0) {
        return true ;
    }
    return false;
}

void DiffSingleNodeResult::evaluateAttributes()
{
    _attributesCount = evalElementForAttributes(_referenceElement) + evalElementForAttributes(_compareElement) ;
}

int DiffSingleNodeResult::evalElementForAttributes(SourceElementDiffOperation *value)
{
    if(NULL != value) {
        Element *element = value->element() ;
        if(NULL != element) {
            if(element->getType() == Element::ET_ELEMENT) {
                return element->attributes.size();
            }
        }
    }
    return 0;
}


SourceElementDiffOperation* DiffSingleNodeResult::referenceElement()
{
    return _referenceElement ;
}

SourceElementDiffOperation* DiffSingleNodeResult::compareElement()
{
    return _compareElement ;
}

void DiffSingleNodeResult::setReferenceElement(SourceElementDiffOperation* value)
{
    _referenceElement = value ;
}

void DiffSingleNodeResult::setCompareElement(SourceElementDiffOperation* value)
{
    _compareElement = value ;
}


QList<DiffSingleNodeResult*> &DiffSingleNodeResult::children()
{
    return _children;
}

void DiffSingleNodeResult::addEqualAttribute(Attribute *newAttr)
{
    _equalsAttributes.append(newAttr);
}

//------------------------------------------------
EqualsDiffNodeResult::EqualsDiffNodeResult(SourceElementDiffOperation* newReferenceElement, SourceElementDiffOperation* newCompareElement)
    : DiffSingleNodeResult(newReferenceElement, newCompareElement)
{
}

EqualsDiffNodeResult::~EqualsDiffNodeResult()
{
}

//------------------------------------------------
AddedDiffNodeResult::AddedDiffNodeResult(SourceElementDiffOperation* newReferenceElement)
    : DiffSingleNodeResult(newReferenceElement, NULL)
{
}

AddedDiffNodeResult::~AddedDiffNodeResult()
{
}
//------------------------------------------------
DeletedDiffNodeResult::DeletedDiffNodeResult(SourceElementDiffOperation* newCompareElement)
    : DiffSingleNodeResult(NULL, newCompareElement)
{
}

DeletedDiffNodeResult::~DeletedDiffNodeResult()
{
}

//------------------------------------------------
ModifiedDiffNodeResult::ModifiedDiffNodeResult(SourceElementDiffOperation* newReferenceElement, SourceElementDiffOperation* newCompareElement)
    : DiffSingleNodeResult(newReferenceElement, newCompareElement)
{
}

ModifiedDiffNodeResult::~ModifiedDiffNodeResult()
{
}

//------------------------------------------------
//------------------------------------------------
//------------------------------------------------
//------------------------------------------------
//------------------------------------------------

DiffNodesChangeList::DiffNodesChangeList()
{
    _isReferenceEqualToCompare = true;
}

DiffNodesChangeList::~DiffNodesChangeList()
{
    reset();
}

void DiffNodesChangeList::reset()
{
    _isReferenceEqualToCompare = true;
    foreach(DiffSingleNodeResult * singleResult, _rootLevel) {
        delete singleResult ;
    }
    _rootLevel.clear();
}

bool DiffNodesChangeList::isReferenceEqualToCompare()
{
    return _isReferenceEqualToCompare ;
}

QList<DiffSingleNodeResult *>& DiffNodesChangeList::rootLevel()
{
    return _rootLevel;
}

void DiffNodesChangeList::addToRootLevel(DiffSingleNodeResult *node)
{
    _rootLevel.append(node);
}

void DiffNodesChangeList::setDifferent()
{
    _isReferenceEqualToCompare = false ;
}

//------------------------------------------------

