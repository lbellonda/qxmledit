/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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

#include "xsdeditor/xschema.h"
#include "xsdeditor/XSchemaIOContants.h"
#include "utils.h"


//---------------------------- region(compare) ----------------------------------

XSDCompareObject::EXSDCompareObject XSchemaObject::compareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if((target->getType() == getType())
            && (_name == target->name())) {
        return innerCompareTo(target, options);
    }
    return XSDCompareObject::XSDOBJECT_DIFFERENT ;
}

void XSchemaObject::setCompareState(const XSDCompareState::EXSDCompareState newState)
{
    _compareStatus = newState;
}

XSDCompareState::EXSDCompareState XSchemaObject::compareState()
{
    return _compareStatus;
}

void XSchemaObject::markCompareStateRecursive(const XSDCompareState::EXSDCompareState newState)
{
    setCompareState(newState);
    if(NULL != _annotation) {
        _annotation->markCompareStateRecursive(newState);
    }
    foreach(XSchemaObject * child, getChildren()) {
        child->markCompareStateRecursive(newState);
    }
}

void XSchemaElement::markCompareStateRecursive(const XSDCompareState::EXSDCompareState newState)
{
    XSchemaObject::markCompareStateRecursive(newState);
    foreach(XSchemaObject * child, attributes()) {
        child->markCompareStateRecursive(newState);
    }
}

XSDCompareResult *XSchemaObject::compare(XSDCompareResult *result, XSchemaObject *target, XSDCompareOptions &options)
{
    return compareOrdered(result, getChildren(), target->getChildren(), options);
}

XSDCompareResult *XSDSchema::compare(XSDCompareResult *result, XSchemaObject *target, XSDCompareOptions &options)
{
    XSDCompareResult *resultOp = compareOrdered(result, getChildren(), target->getChildren(), options);
    regenerateInternalLists();
    return resultOp ;
}

XSDCompareResult *XSchemaElement::compare(XSDCompareResult *result, XSchemaObject *target, XSDCompareOptions &options)
{
    XSchemaObject::compare(result, target, options);
    if(target->getType() != getType()) {
        XsdError(tr("Comparing an element to a non element"));
    }
    QList<XSchemaObject*> sourceList;
    QList<XSchemaObject*> targetList;

    XSchemaElement *element = static_cast<XSchemaElement*>(target);
    copyAttributesListInGeneric(sourceList);
    element->copyAttributesListInGeneric(targetList);
    compareUnordered(result, sourceList, targetList, options);
    restoreAttributesListInGeneric(sourceList);
    element->restoreAttributesListInGeneric(targetList);
    return result;
}

void XSchemaElement::copyAttributesListInGeneric(QList<XSchemaObject*> &list)
{
    foreach(XSchemaObject * attribute, _attributes) {
        list.append(attribute);
    }
}

void XSchemaElement::restoreAttributesListInGeneric(QList<XSchemaObject*> &list)
{
    _attributes.clear();
    foreach(XSchemaObject * attribute, list) {
        _attributes.append(attribute);
    }
}

XSDCompareResult *XSchemaObject::compareOrdered(XSDCompareResult *result, QList<XSchemaObject*> &referenceList, QList<XSchemaObject*> &targetList, XSDCompareOptions &options)
{
    XSDCompareData data(referenceList, targetList);
    int indexRef = 0 ;
    for(indexRef = 0 ; indexRef < data.referenceCount ; indexRef++) {
        XSchemaObject *referenceChild = referenceList.at(indexRef);

        if(data.indexTarget >= data.targetCount) {
            // no target exists
            referenceChild->addChildBranch(data, XSDCompareState::COMPARE_ADDED);
            result->setDifferent(true);
        } else {
            XSchemaObject *targetChild = data.targetCollection.at(data.indexTarget);
            XSDCompareObject::EXSDCompareObject compareResult = referenceChild->compareTo(targetChild, options);
            switch(compareResult) {
            default:
                result->setError(true);
                XsdError(tr("Illegal state comparing XSD objects."));
                break;
            case XSDCompareObject::XSDOBJECT_UNCHANGED:
                referenceChild->advanceChild(data, XSDCompareState::COMPARE_UNCHANGED);
                referenceChild->compare(result, targetChild, options);
                break;

            case XSDCompareObject::XSDOBJECT_MODIFIED:
                referenceChild->advanceChild(data, XSDCompareState::COMPARE_MODIFIED);
                referenceChild->compare(result, targetChild, options);
                result->setDifferent(true);
                break;

                // if the object is not found, examine the list until it is found.
                // if a match is found, all the elements between the current+1 and the found, are marked deleted, the index of target is moved to the last pos
                // if no match is found, the scan follows with the next element and the current is new.
                // all the elements that are remaining in the target list, are considedered deleted.
                // the cloned elements are inserted at the end of the scan.
            case XSDCompareObject::XSDOBJECT_DIFFERENT:
                referenceChild->compareDifferentObjects(result, data, targetList, options);
                result->setDifferent(true);
                break;
            } // switch
            data.finalCollection.append(referenceChild);
        } // if indexTarget
    } // for()

    // take all the non examined target elements and add them as deleted to the source collection
    for(int indexRefOuter = data.indexTarget ; indexRefOuter < data.targetCount ; indexRefOuter++) {
        data.targetCollection.at(indexRefOuter)->addDeletedTarget(data.finalCollection, this, targetList);
        result->setDifferent(true);
    }

    // set the final children collection
    referenceList.clear();
    foreach(XSchemaObject * child, data.finalCollection) {
        if(child == this) {
            XsdError(tr("Wrong element referred."));
        }
        referenceList.append(child);
    }
    return result;
}

void XSchemaObject::advanceChild(XSDCompareData &data, const XSDCompareState::EXSDCompareState newState)
{
    setCompareState(newState);
    data.indexTarget = data.nextIndexTarget(data.indexTarget);
}


void XSchemaObject::compareDifferentObjects(XSDCompareResult *result, XSDCompareData &data, QList<XSchemaObject*> &targetList, XSDCompareOptions &options)
{
    for(int indexRefOuter = data.indexTarget + 1 ; indexRefOuter < data.targetCount ; indexRefOuter ++) {
        XSchemaObject *targetTest = data.targetCollection.at(indexRefOuter);
        XSDCompareObject::EXSDCompareObject compareResult = compareTo(targetTest, options);

        if((XSDCompareObject::XSDOBJECT_UNCHANGED == compareResult)
                || (XSDCompareObject::XSDOBJECT_MODIFIED == compareResult)) {
            // mark deleted the elements, and set it to the current element
            for(int indexToDel = data.indexTarget ; indexToDel < indexRefOuter ; indexToDel ++) {
                data.targetRemoved.insert(indexToDel);
                XSchemaObject *targetToRemove = data.targetCollection.at(indexToDel);
                targetToRemove->addDeletedTarget(data.finalCollection, xsdParent(), targetList);
            } // for
            // Move the index to the next available.
            data.indexTarget = indexRefOuter + 1 ;
            // set the value
            if(XSDCompareObject::XSDOBJECT_UNCHANGED == compareResult) {
                setCompareState(XSDCompareState::COMPARE_UNCHANGED);
            } else {
                setCompareState(XSDCompareState::COMPARE_MODIFIED);
            }
            compare(result, targetTest, options);
            data.indexTarget = indexRefOuter + 1 ;
            return ;
        } // if same object fountd
    } // for
    // no match, the source is added
    markCompareStateRecursive(XSDCompareState::COMPARE_ADDED);
    // No change to the target index.
}



QString XSchemaObject::hashNameOfObject(XSchemaObject* target)
{
    QString result = QString("%1<>%2<>%3").arg(target->getType()).arg(target->name()).arg(target->nameOrReference());
    return result;
}

XSDCompareResult *XSchemaObject::compareUnordered(XSDCompareResult *result, QList<XSchemaObject*>&referenceList, QList<XSchemaObject*>&targetList, XSDCompareOptions &options)
{
    QList<XSchemaObject*> finalList;
    QHash<QString, XSchemaObject*> scanMap;
    // fill the list
    foreach(XSchemaObject * child, targetList) {
        // warning to the name collisions
        scanMap.insert(hashNameOfObject(child), child);
    }

    foreach(XSchemaObject * child, referenceList) {
        finalList.append(child);
        QString key = hashNameOfObject(child);
        if(scanMap.contains(key)) {
            XSchemaObject *targetChild = scanMap[key];
            XSDCompareObject::EXSDCompareObject compareResult = child->compareTo(targetChild, options);
            switch(compareResult) {
            default:
                XsdError(tr("Illegal state comparing unordered XSD objects."));
                break;
            case XSDCompareObject::XSDOBJECT_UNCHANGED:
                child->setCompareState(XSDCompareState::COMPARE_UNCHANGED);
                child->compare(result, targetChild, options);
                break;

            case XSDCompareObject::XSDOBJECT_MODIFIED:
                child->setCompareState(XSDCompareState::COMPARE_MODIFIED);
                result->setDifferent(true);
                child->compare(result, targetChild, options);
                break;
            } // switch
        } else { // contains
            child->markCompareStateRecursive(XSDCompareState::COMPARE_ADDED);
            result->setDifferent(true);
        } // contains
        scanMap.remove(key);
    }
    // take all the non examined target elements and add them as deleted to the source collection
    foreach(QString key, scanMap.keys()) {
        scanMap[key]->addDeletedTarget(finalList, this, targetList);
        result->setDifferent(true);
    }

    // set the final children collection
    referenceList.clear();
    foreach(XSchemaObject * child, finalList) {
        if(child == this) {
            XsdError("Wrong element referred (2).");
        }
        referenceList.append(child);
    }
    return result;
}

void XSchemaObject::addDeletedTarget(QList<XSchemaObject *> &finalCollection, XSchemaObject *referenceFather, QList<XSchemaObject *> &targetList)
{
    markCompareStateRecursive(XSDCompareState::COMPARE_DELETED);
    if(!targetList.removeOne(this)) {
        XsdError(tr("Trying to reparent a non child element"));
    }
    insertElementFromOtherCollection(finalCollection, referenceFather);
}

void XSchemaObject::insertElementFromOtherCollection(QList<XSchemaObject *> &finalCollection, XSchemaObject *referenceFather)
{
    reparent(referenceFather);
    finalCollection.append(this);
}

void XSchemaObject::addChildBranch(XSDCompareData &data, const XSDCompareState::EXSDCompareState newState)
{
    markCompareStateRecursive(newState);
    data.finalCollection.append(this);
}


//---------------------------- endregion(compare) ----------------------------------


XSDCompareObject::EXSDCompareObject XSchemaObject::innerBaseCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(_id != target->_id) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    if(options.isCompareComment()) {
        return compareAnnotations(_annotation, target->_annotation, options);
    }
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

//------------ inner (specific) comparison routines)

bool XSchemaObject::baseInnerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    XSDCompareObject::EXSDCompareObject result ;
    result = XSchemaObject::innerBaseCompareTo(target, options);
    if(XSDCompareObject::XSDOBJECT_MODIFIED == result) {
        return false;
    }
    return true;
}

XSDCompareObject::EXSDCompareObject XSchemaAttribute::innerCompareTo(XSchemaObject *other, XSDCompareOptions &options)
{
    XSchemaAttribute *target = static_cast<XSchemaAttribute*>(other);
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    CMP_DATA(_defaultValue, target);
    CMP_DATA(_fixed, target);
    CMP_DATA(_form, target);
    CMP_DATA(_ref, target);
    CMP_DATA(_xsdType, target);
    CMP_DATA(_use, target);
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

XSDCompareObject::EXSDCompareObject XSchemaElement::innerCompareTo(XSchemaObject *other, XSDCompareOptions &options)
{
    XSchemaElement *target = static_cast<XSchemaElement*>(other);
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    if(compareAnnotations(_innerAnnotation, target->_innerAnnotation, options) == XSDCompareObject::XSDOBJECT_MODIFIED) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }

    CMP_DATA(_isTypeOrElement, target);
    CMP_DATA(_elementCategory, target);

    CMP_DATA(_abstract, target);
    CMP_DATA(_block, target);
    CMP_DATA(_defaultValue, target);
    CMP_DATA(_final, target);
    CMP_DATA(_fixed, target);
    CMP_OBJP(_maxOccurs, target);
    CMP_OBJP(_minOccurs, target);
    CMP_DATA(_nillable, target);
    CMP_DATA(_ref, target);
    CMP_DATA(_xsdType, target);

    //----- types specific
    CMP_DATA(_mixed, target);
    CMP_DATA(_containedId, target);

    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

XSDCompareObject::EXSDCompareObject XSchemaChoice::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    XSchemaChoice *other = static_cast<XSchemaChoice*>(target);
    CMP_OBJP(_maxOccurs, other);
    CMP_OBJP(_minOccurs, other);
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

XSDCompareObject::EXSDCompareObject XSchemaSequence::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    XSchemaSequence *other = static_cast<XSchemaSequence*>(target);
    CMP_OBJP(_maxOccurs, other);
    CMP_OBJP(_minOccurs, other);
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

XSDCompareObject::EXSDCompareObject XSchemaAll::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    XSchemaAll *other = static_cast<XSchemaAll*>(target);
    CMP_OBJP(_maxOccursOne, other);
    CMP_OBJP(_minOccurs, other);
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}


XSDCompareObject::EXSDCompareObject XSchemaAny::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    XSchemaAny *other = static_cast<XSchemaAny*>(target);
    CMP_OBJP(_maxOccurs, other);
    CMP_OBJP(_minOccurs, other);
    CMP_DATA(_namespaces, other);
    CMP_DATA(_processContent, other);

    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

XSDCompareObject::EXSDCompareObject XSchemaAnyAttribute::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    XSchemaAnyAttribute *other = static_cast<XSchemaAnyAttribute*>(target);
    CMP_LIST(_namespaces, other);
    CMP_DATA(_processContent, other);
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

XSDCompareObject::EXSDCompareObject XSchemaKey::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

XSDCompareObject::EXSDCompareObject XSchemaKeyRef::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    XSchemaKeyRef *other = static_cast<XSchemaKeyRef*>(target);
    CMP_DATA(_refer, other);
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

XSDCompareObject::EXSDCompareObject XSchemaUnique::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

XSDCompareObject::EXSDCompareObject XSchemaAnnotation::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    XSchemaAnnotation *other = static_cast<XSchemaAnnotation*>(target);
    CMP_LIST_O(_infos, other, options);
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

XSDCompareObject::EXSDCompareObject XSchemaAttributeGroup::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    XSchemaAttributeGroup *other = static_cast<XSchemaAttributeGroup*>(target);
    CMP_DATA(_ref, other);
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

XSDCompareObject::EXSDCompareObject XSchemaField::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    XSchemaField *other = static_cast<XSchemaField*>(target);
    CMP_DATA(_xpath, other);
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

XSDCompareObject::EXSDCompareObject XSchemaGroup::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    XSchemaGroup *other = static_cast<XSchemaGroup*>(target);
    CMP_OBJP(_maxOccurs, other);
    CMP_OBJP(_minOccurs, other);
    CMP_DATA(_ref, other);
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

XSDCompareObject::EXSDCompareObject XSchemaImport::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    XSchemaImport *other = static_cast<XSchemaImport*>(target);
    CMP_DATA(_namespace, other);
    CMP_DATA(_schemaLocation, other);
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

XSDCompareObject::EXSDCompareObject XSchemaInclude::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    XSchemaInclude *other = static_cast<XSchemaInclude*>(target);
    CMP_DATA(_schemaLocation, other);
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

XSDCompareObject::EXSDCompareObject XSchemaNotation::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    XSchemaNotation *other = static_cast<XSchemaNotation*>(target);
    CMP_DATA(_publicIdentifier, other);
    CMP_DATA(_system, other);
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

XSDCompareObject::EXSDCompareObject XSchemaRedefine::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    XSchemaRedefine *other = static_cast<XSchemaRedefine*>(target);
    CMP_DATA(_schemaLocation, other);
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

XSDCompareObject::EXSDCompareObject XSchemaSelector::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    XSchemaSelector *other = static_cast<XSchemaSelector*>(target);
    CMP_DATA(_xpath, other);
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}


XSDCompareObject::EXSDCompareObject XSDSchema::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    XSDSchema *other = static_cast<XSDSchema*>(target);

    CMP_DATA(_attributeFormDefault, other);
    CMP_DATA(_elementFormDefault, other);

    CMP_DATA(_targetNamespace, other);
    CMP_DATA(_version, other);
    CMP_DATA(_language, other);

    CMP_LIST(_blockDefault, other);
    CMP_LIST(_finalDefault, other);

    CMP_DATA(_namespacePrefix, other);

    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}


XSDCompareObject::EXSDCompareObject XSchemaSimpleTypeRestriction::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    XSchemaSimpleTypeRestriction *other = static_cast<XSchemaSimpleTypeRestriction*>(target);
    CMP_DATA(_base, other);
    CMP_OBJP(_facets, other);
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

XSDCompareObject::EXSDCompareObject XSchemaSimpleTypeUnion::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    XSchemaSimpleTypeUnion *other = static_cast<XSchemaSimpleTypeUnion*>(target);
    CMP_DATA(_memberTypes, other);
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

XSDCompareObject::EXSDCompareObject XSchemaSimpleTypeList::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    XSchemaSimpleTypeList *other = static_cast<XSchemaSimpleTypeList*>(target);
    CMP_DATA(_itemType, other);
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

XSDCompareObject::EXSDCompareObject XSchemaSimpleContentRestriction::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    XSchemaSimpleContentRestriction *other = static_cast<XSchemaSimpleContentRestriction*>(target);
    CMP_DATA(anyAttribute, other);
    CMP_DATA(_base, other);
    CMP_OBJP(_facets, other);
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

XSDCompareObject::EXSDCompareObject XSchemaSimpleContentExtension::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    XSchemaSimpleContentExtension *other = static_cast<XSchemaSimpleContentExtension*>(target);
    CMP_DATA(anyAttribute, other);
    CMP_DATA(_base, other);
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

XSDCompareObject::EXSDCompareObject XSchemaComplexContentRestriction::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    XSchemaComplexContentRestriction *other = static_cast<XSchemaComplexContentRestriction*>(target);
    CMP_DATA(anyAttribute, other);
    CMP_DATA(_base, other);
    CMP_DATA(_mixed, other);
    if(compareAnnotations(_mainAnnotation, other->_mainAnnotation, options) == XSDCompareObject::XSDOBJECT_MODIFIED) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }

    CMP_DATA(_idContainer, other);
    //Utils::TODO_THIS_RELEASE("  QMap<QString, QString> _otherTypeAttributes;");
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}


XSDCompareObject::EXSDCompareObject XSchemaComplexContentExtension::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    XSchemaComplexContentExtension *other = static_cast<XSchemaComplexContentExtension*>(target);
    CMP_DATA(anyAttribute, other);
    CMP_DATA(_base, other);
    CMP_DATA(_mixed, other);
    if(compareAnnotations(_mainAnnotation, other->_mainAnnotation, options) == XSDCompareObject::XSDOBJECT_MODIFIED) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    CMP_DATA(_idContainer, other);
    //Utils::TODO_THIS_RELEASE("  QMap<QString, QString> _otherTypeAttributes;");
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

XSDCompareObject::EXSDCompareObject XInfoBase::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    XInfoBase *other = static_cast<XInfoBase*>(target);
    CMP_DATA(_source, other);
    if(_content.toText() != other->_content.toText()) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    CMP_DATA(_content.toText(), other);
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

XSDCompareObject::EXSDCompareObject XDocumentation::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    XDocumentation *other = static_cast<XDocumentation*>(target);
    if(XInfoBase::innerCompareTo(target, options) == XSDCompareObject::XSDOBJECT_MODIFIED) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    CMP_DATA(_language, other);
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

XSDCompareObject::EXSDCompareObject XAppInfo::innerCompareTo(XSchemaObject *target, XSDCompareOptions &options)
{
    if(!baseInnerCompareTo(target, options)) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    XAppInfo *other = static_cast<XAppInfo*>(target);
    if(XInfoBase::innerCompareTo(other, options) == XSDCompareObject::XSDOBJECT_MODIFIED) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    return XSDCompareObject::XSDOBJECT_UNCHANGED;
}

// XSchemaAnnotation: use the base function

XSDCompareObject::EXSDCompareObject XSchemaObject::compareAnnotations(XSchemaAnnotation *reference, XSchemaAnnotation *target, XSDCompareOptions &options)
{
    if((reference == NULL) && (target == NULL)) {
        return XSDCompareObject::XSDOBJECT_UNCHANGED;
    }
    if(reference == NULL) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    if(target == NULL) {
        return XSDCompareObject::XSDOBJECT_MODIFIED;
    }
    return reference->compareTo(target, options);
}

//--------------------------------------

void XSchemaObject::getModifiedObjects(QList<XSchemaObject*> &added, QList<XSchemaObject*> &modified, QList<XSchemaObject*> &deleted)
{
    foreach(XSchemaObject * child, getChildren()) {
        switch(child->compareState()) {
        default:
            break;
        case XSDCompareState::COMPARE_ADDED:
            added.append(child);
            break;
        case XSDCompareState::COMPARE_MODIFIED:
            modified.append(child);
            break;
        case XSDCompareState::COMPARE_DELETED:
            deleted.append(child);
            break;
        }
        child->getModifiedObjects(added, modified, deleted);
    }
}

void XSchemaElement::getModifiedObjects(QList<XSchemaObject*> &added, QList<XSchemaObject*> &modified, QList<XSchemaObject*> &deleted)
{
    XSchemaObject::getModifiedObjects(added, modified, deleted);
    foreach(XSchemaObject * child, attributes()) {
        switch(child->compareState()) {
        default:
            break;
        case XSDCompareState::COMPARE_ADDED:
            added.append(child);
            break;
        case XSDCompareState::COMPARE_MODIFIED:
            modified.append(child);
            break;
        case XSDCompareState::COMPARE_DELETED:
            deleted.append(child);
            break;
        }
        child->getModifiedObjects(added, modified, deleted);
    }
}
